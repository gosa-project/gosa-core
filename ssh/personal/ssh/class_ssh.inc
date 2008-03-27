<?php

/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2007 Benoit Mortier

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

class sssh extends plugin
{
  /* Definitions */
  var $plHeadline= "SSH systems keys";
  var $plDescription= "This plugin store ssh public keys for systems";

  var $sshPublicKey = "";
  var $ignore_account= FALSE;

  /* attribute list for save action */
  var $attributes = array("sshPublicKey");
  var $objectclasses = array("ldapPublicKey");

  var $uid ="";

  /* Used to remember if this was an account (simply: is this an edited entry) */
  var $initialy_was_account = false;

  function ssh ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    
    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }

    $this->is_account            = false;
    $this->initially_was_account = false;

		if(isset($this->attrs['sshPublicKey'])) {
        $this->is_account            = true;
        $this->initially_was_account = true;
    }


  }

  function execute()
  {
		/* Call parent execute */
		plugin::execute();

    /* Fill templating stuff 
     */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? 
     */
    if (isset($_POST['modify_state'])){

      /* Only change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }
    }

    if ($this->is_account){
      $display= $this->show_header(_("Remove SSH keys"),
          _("This server has SSH features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add SSH keys"),
          _("This server has SSH features disabled. You can enable them by clicking below."));
      return ($display);
    }

    /* Load attributes */
    foreach($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign($attr."ACL", chkacl($this->acl, "$attr"));
    }


    $smarty->assign("sshPublicKeyACL",chkacl($this->acl,"sshPublicKey"));

    /* Display template 
     */
    //$smarty->assign("ZoneList",$ZoneList->DrawList());
    $display.= $smarty->fetch(get_template_path('ssh.tpl', TRUE));
    return($display);

  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

      plugin::remove_from_parent();

      $ldap= $this->config->get_ldap_link();

      $ldap->cd($this->dn);
      $this->cleanup();
			
    	$ldap->modify ($this->attrs);

      show_ldap_error($ldap->get_error(), _("Removing SSH key failed"));

      /* Optionally execute a command after we're done */
//      $this->handle_post_events('remove',array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
   	plugin::save_object();
	}

  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message = plugin::check();

    /* Check for empty or not */
		if(empty($this->sshPublicKey)){
        $message[]= _("Value specified as 'SSH Key' is not valid.");
      }

    return($message);
  }

  /* Save to LDAP */
  function save()
  {

    plugin::save();


    foreach($this->attributes as $attr){
      if(chkacl($this->acl,$attr)!=""){
        unset($this->attrs[$attr]);
      }
    }


      /* Write back to ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      show_ldap_error($ldap->get_error(), _("Saving SSH key failed"));

      /* Optionally execute a command after we're done */
      if ($this->initially_was_account == $this->is_account){
        if ($this->is_modified){
          $this->handle_post_events("modify",array("uid" => $this->uid));
        }
      } else {
        $this->handle_post_events("add",array("uid" => $this->uid));
      }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
