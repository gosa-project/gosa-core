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

class department extends plugin
{
	/* department attributes */
	var $ou= "";
	var $description= "";
	var $base= "";
	var $st= "";
	var $l= "";
	var $postalAddress= "";
	var $businessCategory= "";
	var $telephoneNumber= "";
	var $facsimileTelephoneNumber= "";
	var $orig_dn= "";

	var $rec_dst=false;	// Destination for recursive move
	var $rec_src=false;	// Source for recursive move 
	var $rec_cpy=false;	// Is recursive move requested ? 

	function am_i_moved()
	{
		return $this->rec_cpy;
	}

	/* Headpage attributes */
	var $last_dep_sorting= "invalid";
	var $departments= array();

	/* attribute list for save action */
	var $attributes= array("ou", "description", "businessCategory", "st", "l", "postalAddress",
			"telephoneNumber", "facsimileTelephoneNumber");
	var $objectclasses= array("top", "gosaDepartment", "organizationalUnit");

	function department ($config, $dn)
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
			if(isset($_SESSION['CurrentMainBase'])){
				$this->base= $_SESSION['CurrentMainBase'];
			}else{
				$this->base= dn2base($ui->dn);
			}
		} else {
			$this->base= preg_replace ("/^[^,]+,/", "", $this->dn);
		}
		/* set permissions */
		$ui= get_userinfo();
		$acl= get_permissions ($ui->dn, $ui->subtreeACL);
		$this->acl= get_module_permission($acl, "department", $ui->dn);
	}

	function execute()
	{
		/* Call parent execute */
		plugin::execute();

		/* Reload departments */
		$this->config->departments= get_departments($this->dn);
		$this->config->make_idepartments();
		$smarty= get_smarty();

		/* Base select dialog */
		$once = true;
		foreach($_POST as $name => $value){
			if(preg_match("/^chooseBase/",$name) && $once){
				$once = false;
				$this->dialog = new baseSelectDialog($this->config);
				$this->dialog->setCurrentBase($this->base);
			}
		}

		/* Dialog handling */
		if(is_object($this->dialog)){
			/* Must be called before save_object */
			$this->dialog->save_object();

			if($this->dialog->isClosed()){
				$this->dialog = false;
			}elseif($this->dialog->isSelected()){
				$this->base = $this->dialog->isSelected();
				$this->dialog= false;
			}else{
				return($this->dialog->execute());
			}
		}

		/* Hide all departments, that are subtrees of this department */
		$bases 	= $this->config->idepartments;
		if(($this->dn == "new")||($this->dn == "")){
			$tmp = $bases;
		}else{
			$tmp	= array();	
			foreach($bases as $dn=>$base){
				$fixed = str_replace("/","\\",$this->dn);
				/* Only attach departments which are not a subtree of this one */
				if(!preg_match("/".$fixed."/",$dn)){
					$tmp[$dn]=$base;
				}
			}
		}
		$smarty->assign("bases", $tmp);

		foreach ($this->attributes as $val){
			$smarty->assign("$val", $this->$val);
			$smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
		}
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
		/* Call common method to give check the hook */
		$message= plugin::check();

		/* Permissions for that base? */
		//	$this->dn= "ou=$this->ou,".$this->base;
		if (chkacl($this->acl, "create") != ""){
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
		if (preg_match ('/[#+:=>\\\\\/]/', $this->ou)){
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
			$this->cleanup();
			$ldap->modify ($this->attrs); 

			$this->handle_post_events('modify');
		} else {
			$ldap->add($this->attrs);
			$this->handle_post_events('add');
		}
		show_ldap_error($ldap->get_error());

		/* Optionally execute a command after we're done */
		$this->postcreate();
	}

	/* Move/Rename complete trees */
	function recursive_move($src_dn, $dst_dn,$force = false)
	{
		if(!$force){

			$this->rec_cpy 	= true;
			$this->rec_src	= $src_dn;
			$this->rec_dst	= $dst_dn;

			$smarty = get_smarty();

			$smarty->assign("src","?plug=".$_GET['plug']."&PerformRecMove");

			$display=  $smarty->fetch(get_template_path("recursive_move.tpl",TRUE));
			return($display);
			exit();
		}else{
			if(!$this->rec_cpy){ 
				return;
			}

			$src_dn = $this->rec_src;
			$dst_dn = $this->rec_dst;

			/* Print header to have styles included */
			$smarty= get_smarty();
			echo "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
			echo "<body style='background-image:none;margin:3px;color:black'>";

			echo "<h3>".sprintf(_("Moving %s to %s"),"<i>".$src_dn."</i>","<i>".$dst_dn."</i>")."</h3>";


			/* Check if the destination entry exists */
			$ldap= $this->config->get_ldap_link();

			/* Check if destination exists - abort */
			$ldap->cat($dst_dn);
			if ($ldap->fetch()){
				trigger_error("Recursive_move $dst_dn already exists.",
						E_USER_WARNING);
				echo "Recursive_move :$dst_dn already exists.<br>"; 
				return (FALSE);
			}
	
			/* Perform a search for all objects to be moved */
			$objects= array();
			$ldap->cd($src_dn);
			$ldap->search("(objectClass=*)", array("dn"));
			while($attrs= $ldap->fetch()){
				$dn= $attrs['dn'];
				$objects[$dn]= strlen($dn);
			}

			/* Sort objects by indent level */
			asort($objects);
			reset($objects);

			/* Copy objects from small to big indent levels by replacing src_dn by dst_dn */
			foreach ($objects as $object => $len){

				
				$src= str_replace("\\","\\\\",$object);
				$dst= preg_replace("/".str_replace("\\","\\\\",$src_dn)."$/", "$dst_dn", $object);
				$dst= str_replace($src_dn,$dst_dn,$object);

				echo "<b>"._("Object").":</b> $src<br>";
				
				if (!$this->copy($src, $dst)){
					echo "<font color='#FF0000'><br>".sprintf(_("FAILED to copy %s, aborting operation"),$src)."</font>";
					return (FALSE);
				}

				flush();
			}

			/* Remove src_dn */
			$ldap->cd($src_dn);
			$ldap->recursive_remove();
			$this->rec_src = $this->rec_dst = "";
			$this->rec_cpy =false;

			echo '<p class="seperator">&nbsp;</p>';

			echo "<div style='width:100%;text-align:right;'><form name='form' method='post' action='?plug=".$_GET['plug']."' target='_parent'>
				<br><input type='submit' name='back' value='"._("Continue")."'>
				</form></div>";

			echo "</body></html>";
			
			return (TRUE);
		}
	}


}

?>
