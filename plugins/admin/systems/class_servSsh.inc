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

class servssh extends plugin
{
  /* Definitions */
  var $plHeadline= "SSH systems keys";
  var $plDescription= "This plugin store ssh public keys for systems";

  var $sshPublickey = "";
  var $ignore_account= TRUE;

  /* attribute list for save action */
  var $attributes= array("sshPublickey");
  var $objectclasses= array("HostldapPublicKey");

  var $uid ="";

  function servssh ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    
    /* Always is account... */
    $this->is_account= TRUE;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign($attr."ACL", chkacl($this->acl, "$attr"));
    }

    $smarty->assign("sstate", "");
    if ($this->is_account){
      $smarty->assign("sshState", "checked");
      $smarty->assign("sstate", "");
    } else {
      $smarty->assign("sshState", "");
      if($_SESSION['js']==1){
        if($this->acl!="#none#")
        $smarty->assign("sstate", "disabled");
      }else{
        $smarty->assign("sstate", "");
      }
    }

    /* Allow account status toogle?  */
    $smarty->assign("sshACL", "disabled");
    if(!$this->is_account && chkacl($this->acl,"create") == ""){
      $smarty->assign("sshACL", "");
    }elseif($this->is_account && $this->acl == "#all#" ){
      $smarty->assign("sshACL", "");
    }

    $changeState = "";
    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
      if(chkacl($this->acl,$attr)==""){
        $changeState .= "changeState('".$attr."');\n";
      }
    }
    $smarty->assign("changeState",$changeState);

    return($smarty->fetch (get_template_path('servssh.tpl', TRUE)));

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
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      show_ldap_error($ldap->get_error(), _("Removing SSH account failed"));

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove',array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){

      /* Change state if needed */
      if(!$this->is_account && isset($_POST["ssh"]) && chkacl($this->acl,"create") == ""){
        $this->is_account = true;
      }elseif($this->is_account && !isset($_POST["ssh"]) && $this->acl == "#all#"){
        $this->is_account = false;
      }
    }

    plugin::save_object();
    
    #FIXME seams to be unused code !
    #if (isset($_POST["sshStatus"])){
    #  $this->sshStatus = "disabled";
    #} else {
    #  $this->sshStatus = "enabled";
    #}
  }

  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Check for positive integer values */
    /* if ($this->is_account){
      if ((!is_id($this->sshPublickey))&&(chkacl($this->acl,"sshPublickey")=="")){
        $message[]= _("Value specified as 'SSH Key' is not valid.");
      }
    }*/

    return $message;
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

      show_ldap_error($ldap->get_error(), _("Saving SSH account failed"));

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
