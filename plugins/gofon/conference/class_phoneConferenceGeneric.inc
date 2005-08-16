<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003  Cajus Pollmeier

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

class conference extends plugin
{
  /* department attributes */
  var $cn= "";
  var $description= "";
  var $base ="";


  var $goFonPIN				   = "";

  var $goFonConferenceOption_D = "";
  var $goFonConferenceOption_r = "";
  var $goFonConferenceOption_M = "";
  var $goFonConferenceOption_s = "";
  var $goFonConferenceOption_i = "";
  var $goFonConferenceOption_c = "";
  var $goFonConferenceOption_P = "";

  var $goFonConferenceOptionFormat 		= "";
  var $goFonConferenceOptionLifetime 	= "";
  var $goFonConferenceOptionNumber 		= "";
	
  /* Headpage attributes */
  var $last_dep_sorting= "invalid";
  var $departments= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description", "goFonPIN","goFonConferenceOption_D","goFonConferenceOption_r",
						 "goFonConferenceOption_M","goFonConferenceOption_s","goFonConferenceOption_i","goFonConferenceOption_c",
						 "goFonConferenceOption_P","goFonConferenceOptionFormat","goFonConferenceOptionLifetime","goFonConferenceOptionNumber");

  var $objectclasses= array("top", "gosaDepartment", "organizationalUnit");

  function conference ($config, $dn)
  {
  	plugin::plugin($config, $dn);
	$this->is_account= TRUE;
	$this->ui= get_userinfo();
	$this->dn= $dn;
	$this->orig_dn= $dn;
	$this->config= $config;

	/* Set base */
    if ($this->dn == "new"){
		$ui= get_userinfo();
		if(isset($_SESSION['depfilter']['depselect'])){
			$this->base = $_SESSION['depfilter']['depselect'];
		}else{
			$this->base= dn2base($ui->dn);
		}
	} else {
                $this->base= preg_replace ("/^[^,]+,/", "", $this->dn);
        }
  }

  function execute()
  {
	/* Reload departments */
	$this->config->departments= get_departments($this->dn);
	$this->config->make_idepartments();
	$smarty= get_smarty();

	$smarty->assign("goFonConferenceOptions",		array("P"=>"Conference ",""=>"Conference without PIN"));
	$smarty->assign("goFonConferenceOptionFormats",	array("P"=>"Conference ",""=>"Conference without PIN"));
	$smarty->assign("goFonConferenceOption" ,$this->goFonConferenceOption_P);
	
	foreach ($this->attributes as $val){
		$smarty->assign("$val", $this->$val);
		$smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
		if($this->$val){
			$smarty->assign($val."CHK", "");
		}else{
			$smarty->assign($val."CHK", " checked ");
		}
	}
	$smarty->assign("bases", $this->config->idepartments);
	$smarty->assign("base_select", $this->base);
	return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function clear_fields()
  {
	$this->dn= "";
	$this->base= "";
	$this->acl= "#none#";

	foreach ($this->attributes as $val){
		$this->$val= "";
	}
  }


  function remove_from_parent()
  {
	$ldap= $this->config->get_ldap_link();
	$ldap->cd ($this->dn);
	$ldap->recursive_remove();

	/* Optionally execute a command after we're done */
	$this->handle_post_events('remove');
  }


  /* Save data to object */
  function save_object()
  {
	if (isset($_POST['base'])){
		plugin::save_object();

		/* Save base, since this is no LDAP attribute */
		if (chkacl($this->acl, "create") == ""){
			$this->base= $_POST['base'];
		}
	}
  }


  /* Check values */
  function check()
  {
	$message= array();

	/* Permissions for that base? */
	$this->dn= "ou=$this->ou,".$this->base;
	$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
	$acl= get_module_permission($acl, "department", $this->dn);
	if (chkacl($acl, "create") != ""){
		$message[]= _("You have no permissions to create a department on this 'Base'.");
	}

	/* Check for presence of this department */
	$ldap= $this->config->get_ldap_link();
	$attrs= $ldap->cat ($this->dn);
	if ($this->orig_dn == "new" && !($attrs === FALSE)){
		$message[]= _("Department with that 'Name' already exists.");
	} elseif ($this->orig_dn != $this->dn && !($attrs === FALSE)){
		$message[]= _("Department with that 'Name' already exists.");
	}

	/* All required fields are set? */
	if ($this->ou == ""){
		$message[]= _("Required field 'Name' is not set.");
	}
	if ($this->description == ""){
		$message[]= _("Required field 'Description' is not set.");
	}

	/* Validate and modify - or: spaghetti rules! */
	if ($this->ou == "incoming"){
		$message[]= _("The field 'Name' contains the reserved word 'incoming'.".
				" Please choose another name.");
	}
	if (preg_match ('/[,#+:=>\\\\]/', $this->ou)){
		$message[]= _("The field 'Name' contains invalid characters.");
	}
	if (!is_phone_nr($this->telephoneNumber)){
		$message[]= _("The field 'Phone' contains an invalid phone number.");
	}
	if (!is_phone_nr($this->facsimileTelephoneNumber)){
		$message[]= _("The field 'Fax' contains an invalid phone number.");
	}

	return $message;
  }


  /* Save to LDAP */
  function save()
  {
	plugin::save();

	/* Write back to ldap */
	$ldap= $this->config->get_ldap_link();
	$ldap->cat($this->dn);
	$a= $ldap->fetch();
	$ldap->cd($this->dn);
	if (count($a)){
		$ldap->modify($this->attrs);
		$this->handle_post_events('modify');
	} else {
		$ldap->add($this->attrs);
		$this->handle_post_events('add');
	}
	show_ldap_error($ldap->get_error());

	/* Optionally execute a command after we're done */
	$this->postcreate();
  }

}

?>
