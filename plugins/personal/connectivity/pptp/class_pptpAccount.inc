<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2005 Guillaume Delecourt
   Copyright (C) 2005 Benoit Mortier

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

class pptpAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PPTP";
  var $plDescription= "Manage PPTP user settings";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("pptpServerAccount");

  var $ReadOnly = false;
  var $view_logged = FALSE;
  var $uid ="";

  function __construct(&$config, $dn= NULL)
  {
    plugin::__construct ($config, $dn);
    
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }

  function execute()
  {
    /* Call parent execute */
    //	plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    if ($this->is_account){
      $smarty->assign("pptpState", "checked");
    } else {
      $smarty->assign("pptpState", "");
      $smarty->assign("wstate", "disabled");
    }

    if((!$this->ReadOnly) && (($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable()))) {
      $smarty->assign('gosapptpACL', "");
    }else{
      $smarty->assign('gosapptpACL', " disabled ");
    }

    $display.= $smarty->fetch (get_template_path('pptp.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if($this->acl_is_removeable()){
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

      /* Log last action */
      new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
      }

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove',array("uid" => $this->uid));
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['pptp'])){
        if (!$this->is_account && $_POST['pptp'] == "B"){
          if($this->acl_is_createable()){
            $this->is_account= TRUE;
          }
        }
      } else {
        if($this->acl_is_removeable()){
          $this->is_account= FALSE;
        }
      }
    }

    plugin::save_object();
    if (isset($_POST["pptpStatus"])){
      $this->pptpStatus = "disabled";
    } else {
      $this->pptpStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
      plugin::save();

      /* Write back to ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      /* Log last action */
      if($this->initially_was_account){
          new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }else{
          new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }

      if (!$ldap->success()){
          msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
      }

      /* Optionally execute a command after we're done */
      if ($this->initially_was_account == $this->is_account){
          if ($this->is_modified){
              $this->handle_post_events("modify",array("uid" => $this->uid));
          }
      } else {
          $this->handle_post_events("add",array("uid" => $this->uid));
      }
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("PPTP"),
          "plDescription"   => _("PPTP account")."&nbsp;("._("Connectivity add-on").")",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 28,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plRequirements"=> array(
              'ldapSchema' => array('pptpServerAccount' => ''),
              'onFailureDisablePlugin' => array(get_class())
              ),
          "plOptions"       => array(),

          "plProvidedAcls"  => array()
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
