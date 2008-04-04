<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_sudoManagement.inc 10099 2008-04-01 12:52:01Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class sudoManagement extends plugin
{
  /* Definitions */
  var $plHeadline     = "Sudo";
  var $plDescription  = "Manage sudo roles";

  var $DivListSudo    = NULL;
  var $base           = "";

  var $sudotabs       = NULL;

  function sudoManagement (&$config, &$ui)
  {
    /* Save configuration for internal use */
    $this->config = &$config;
    $this->ui     = &$ui;
    $this->base   = @get_ou("sudoou");

    /***
      GET sudo base
     ***/
    if(empty($this->base)){
      /* Default is ou=sudoers,BASE */
      $this->base = "ou=sudoers,".$this->config->current['BASE'];
    }else{
  
      /* Append base to given sudoers ou if missing */
      if(!preg_match("/".normalizePreg($this->config->current['BASE'])."$/i",$this->base)){
        if(!preg_match("/,$/",$this->base)){
          $this->base = $this->base.",".$this->config->current['BASE'];
        }else{
          $this->base = $this->base.$this->config->current['BASE'];
        }
      }
    }

    /* Create dialog object */
    $this->DivListSudo = new divListSudo($this->config,$this);
  }


  function execute()
  {
  	/* Call parent execute */
  	plugin::execute();

    /********************
      Handle Posts
     ********************/

    /* Store these posts if the current object is locked (used by somebody else)*/
    session::set('LOCK_VARS_TO_USE',array(
          "/^act$/","/^id$/","/^sudo_edit_/",
          "/^sudo_del_/","/^item_selected/","/menu_action/"));

    /* Save data */
    $s_action   = "";
    $s_entry    = "";

    foreach($_POST as $name => $value){
      if(preg_match("/^sudo_edit_/",$name)){
        $s_action = "edit_role";
        $s_entry  = preg_replace("/^sudo_edit_([0-9]*).*$/","\\1",$name);
      }
      if(preg_match("/^sudo_del_/",$name)){
        $s_action = "del_role";
        $s_entry  = preg_replace("/^sudo_del_([0-9]*).*$/","\\1",$name);
      }
    }
  
    if(isset($_GET['act']) && isset($_GET['id']) && $_GET['act'] == "edit_entry"){
      $id = trim($_GET['id']);
      if(isset($this->list[$id])){
        $s_action = "edit_role";
        $s_entry  = $id;
      } 
    }
 
    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && in_array($_POST['menu_action'],array("new_role","remove_multiple"))){
      $s_action = $_POST['menu_action'];
    }

    $smarty= get_smarty();

    /********************
      Create a new sudo  ...
     ********************/

    /* New sudo? */
    if ($s_action=="new_role"){

      /* Check create permissions */
      $acl = $this->ui->get_permissions($this->DivListSudo->selectedBase,"sudo/sudo");
      if(preg_match("/c/",$acl)){

        /* By default we set 'dn' to 'new', all relevant plugins will
           react on this. */
        $this->dn= "new";

        /* Create new sudotabs object */
        $this->sudotabs= new sudotabs($this->config, $this->config->data['TABS']['SUDOTABS'], $this->dn);

        /* Set up the sudo ACL's for this 'dn' */
        $this->sudotabs->set_acl_base($this->base);
      }
    }


    /********************
      Save Sudo Tab/Object Changes
     ********************/

    /* Finish sudo edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && is_object($this->sudotabs)){

      /* Check tabs, will feed message array 
         Save, or display error message? */
      $message= $this->sudotabs->check();
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->sudotabs->save();

        if (!isset($_POST['edit_apply'])){
          /* Sudo has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          /* There's no page reload so we have to read new sudos at this point. */
          //$this->reload ();
          unset ($this->sudotabs);
          $this->sudotabs= NULL;
          session::un_set('objectinfo');
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        msg_dialog::displayChecks($message);
      }
    }


    /********************
      Edit existing role 
     ********************/

    /* User wants to edit data? */
    if (($s_action=="edit_role") &&  !is_object($this->sudotabs)){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Check locking & lock entry if required */
      $user = get_lock($this->dn);
      if ($user != ""){
        return(gen_locked_message ($user, $this->dn));
      }
      add_lock ($this->dn, $this->ui->dn);

      /* Register sudotabs to trigger edit dialog */
      $this->sudotabs= new sudotabs($this->config,$this->config->data['TABS']['SUDOTABS'], $this->dn);
      $this->sudotabs->set_acl_base($this->base);
      session::set('objectinfo',$this->dn);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){
        foreach($ids as $id){
          $dn = $this->list[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn;
        }

        $dns_names = array();
        foreach($this->dns as $dn){
          add_lock ($dn, $this->ui->dn);
          $dns_names[] =@LDAP::fix($dn);
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        $smarty->assign("info", msgPool::deleteInfo($dns_names,_("sudo role")));
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

      /* Confirmation for deletion has been passed. Sudo should be deleted. */
      if (isset($_POST['delete_multiple_sudos_confirmed'])){

        /* Remove user by user and check acls before removeing them */
        foreach($this->dns as $key => $dn){

          /* Load permissions for selected 'dn' and check if
             we're allowed to remove this 'dn' */
          $acl = $this->ui->get_permissions($dn,"sudo/sudo");
          if(preg_match("/d/",$acl)){

            /* Delete request is permitted, perform LDAP action */
            $this->sudotabs= new sudotabs($this->config,$this->config->data['TABS']['SUDOTABS'], $dn);
            $this->sudotabs->set_acl_base($dn);
            $this->sudotabs->delete ();
            unset ($this->sudotabs);
            $this->sudotabs= NULL;

          } else {

            /* Normally this shouldn't be reached, send some extra
               logs to notify the administrator */
            msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
            new log("security","sudo/".get_class($this),$dn,array(),"Tried to trick deletion.");
          }
          /* Remove lock file after successfull deletion */
          del_lock ($dn);
          unset($this->dns[$key]);
        }
      }


      /********************
        Delete MULTIPLE entries Canceled
       ********************/

      /* Remove lock */
    if(isset($_POST['delete_multiple_user_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /********************
      Delete sudo role 
     ********************/

    /* Remove sudo was requested */
    if ($s_action=="del_role"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl = $this->ui->get_permissions($this->dn,"sudo/sudo");
      if(preg_match("/d/",$acl)){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", msgPool::deleteInfo(@LDAP::fix($this->dn),_("sudo role")));
        $smarty->assign("multiple", false);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));

      } else {
        
        /* Obviously the user isn't allowed to delete. Show message and clean session. */
        msg_dialog::display(_("Permission error"), msgPool::permDelete());
      }
    }


    /********************
      Delete sudo confirmed  
     ********************/

    /* Confirmation for deletion has been passed. Sudo should be deleted. */
    if (isset($_POST['delete_sudo_confirmed'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      $acl = $this->ui->get_permissions($this->dn,"sudo/sudo");
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->sudotabs= new sudotabs($this->config,$this->config->data['TABS']['SUDOTABS'], $this->dn);
        $this->sudotabs->set_acl_base($this->dn);
        $this->sudotabs->delete ();
        unset ($this->sudotabs);
        $this->sudotabs= NULL;

        /* Sudo list has changed, reload it. */
        //$this->reload ();

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
        new log("security","sudo/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }


    /********************
      Delete sudo canceled  
     ********************/

    /* Delete sudo canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }
    

    /********************
      A dialog was canceled  
     ********************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel'])){
      if(isset($this->sudotabs->dn)){
        del_lock ($this->sudotabs->dn);
      }
      unset ($this->sudotabs);
      $this->sudotabs= NULL;
      session::un_set('objectinfo');
    }


    /********************
      If there is currently a dialog open, display it
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->sudotabs->config)){
      $display= $this->sudotabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if(isset($this->sudotabs->by_object)){
        if (!$this->sudotabs->by_object[$this->sudotabs->current]->dialog){
          $display.= "<p style=\"text-align:right\">\n";
          $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\"".msgPool::okButton()."\">\n";
          $display.= "&nbsp;\n";
          if ($this->dn != "new"){
            $display.= "<input type=submit name=\"edit_apply\" value=\"".msgPool::applyButton()."\">\n";
            $display.= "&nbsp;\n";
          }
          $display.= "<input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";
          $display.= "</p>";
        }
      }
      return ($display);
    }

    /* Display dialog with sudo list */
    $this->DivListSudo->parent = &$this;
    $this->DivListSudo->execute();
    $this->reload ();
    $this->DivListSudo->setEntries($this->list);
    return($this->DivListSudo->Draw());
  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  function reload($CreatePosixsList=false)
  {
    $this->list             = array();
    $base                   = $this->base;

    $Regex                  = trim($this->DivListSudo->Regex);
    $UserRegex              = trim($this->DivListSudo->UserRegex);
    $SubSearch              = $this->DivListSudo->SubSearch;

    /********************
      Create filter depending on selected checkboxes 
     ********************/
    $values = array("cn","description","sudoUser","sudoCommand","sudoOption");
    if($UserRegex == "*"){
      $ff     = "(&(|(cn=".$Regex.")(description=".$Regex."))(objectClass=sudoRole))";
    }else{
      $ff     = "(&(|(cn=".$Regex.")(description=".$Regex."))(sudoUser=".$UserRegex.")(objectClass=sudoRole))";
    }
    $res = get_list($ff, "sudo",$base,$values, GL_SIZELIMIT);
    $tmp = array();
    foreach($res as $attrs){
      $tmp[$attrs['cn'][0]] = $attrs;
    }
    uksort($tmp, 'strnatcasecmp');  
    $this->list = array_values($tmp);
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListSudo->save_object();
  }


  function remove_lock()
  {
    if (isset($this->sudotabs->dn)){
      del_lock ($this->sudotabs->dn);
    }
  }


  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save to LDAP */
  function save()
  {
    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

  /* Unused functions  */
  function check()  { }
  function adapt_from_template($dn, $skip= array()) { }
  function password_change_needed()  { }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
