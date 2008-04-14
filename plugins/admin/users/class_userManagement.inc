<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class userManagement extends plugin
{
  /* Plugin definitions */
  var $plHeadline= "Users";
  var $plDescription= "Manage users";

  /* Dialog attributes */
  var $usertab              = NULL;
  var $ui                   = NULL;
  var $templates            = array();
  var $got_uid              = false;
  var $CopyPasteHandler     = NULL;
  var $CPPasswordChange     = ""; // Contains the entry id which should get a new password
  var $DivListUsers;

  var $pwd_change_queue     = array();

  var $start_pasting_copied_objects = FALSE;
  var $current_action ="";
  var $msg_dialog= NULL;
  
  function userManagement(&$config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= &$config;
    $this->ui= &$ui;

    /* Copy & Paste handler */
    if ($this->config->boolValueIsTrue("main", "enableCopyPaste")){
      $this->CopyPasteHandler= new CopyPasteHandler($this->config);
    }

    /* Creat dialog object */
    $this->DivListUsers = new divListUsers($this->config,$this);

  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* LOCK MESSAGE Vars */
    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^user_edit_/","/^user_del_/","/^item_selected/","/^remove_multiple_users/","/^multiple_edit/","/menu_action/"));

    $smarty       = get_smarty();                 // Smarty instance
    $s_action     = "";                           // Contains the action to be taken
    $s_entry      = "";                           // The value for s_action

    /* Edit entry button pressed? */
    if( isset($_GET['act']) && $_GET['act'] == "edit_entry" ){
      $s_action= "edit";
      $s_entry= validate($_GET['id']);
    }

    /* Test relevant POST values */  
    foreach($_POST as $key => $val){

      /* Get every possible POST combination and set s_action/s_entry accordingly */
      foreach(array("del"       => "user_del",    
                    "edit"      => "user_edit",
                    "new"       => "user_new",
                    "new_tpl"   => "user_tplnew",
                    "del_multiple" => "^remove_multiple_users",
                    "create_user_from_tpl"          => "userfrom_tpl",
                    "change_pw" => "user_chgpw", 
                    "editPaste" => "editPaste",  
                    "copy_multiple" => "multiple_copy_users",
                    "multiple_edit" => "multiple_edit",
                    "cut_multiple" => "multiple_cut_users",
                    "multiple_password_change" => "multiple_password_change",
                    "copy"      => "^copy",
                    "toggle_lock_status" => "toggle_lock_status",
                    "cut"       => "^cut") as $act => $name){

        if (preg_match("/".$name.".*/", $key)){
          $s_action= $act;
          $s_entry= preg_replace("/".$name."_/i", "", $key);
          break;
        }
      }
      
    } /* ...Test POST */

    /* Remove coordinate prefix from POST, required by some browsers */
    $s_entry= preg_replace("/_.$/", "", $s_entry);

    /* Seperate possibly encoded tab and entry, default to tab "user" */
    if(preg_match("/.*-.*/", $s_entry)){
      $s_tab= preg_replace("/^[^-]*-/i", "" ,$s_entry);
      $s_entry= preg_replace("/-[^-]*$/i", "", $s_entry);
    }else{
      $s_tab= "user";
    }

    if(!$this->config->search($s_tab, 'class',array('tabs'))){
      $s_tab = "user";
    }

    if (isset($_POST['menu_action'])){

	    /* handle C&P from layers menu */
	    if(preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
	      $s_action = "copy_multiple";
	    }
	    if(preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
	      $s_action = "cut_multiple";
	    }
	    if(preg_match("/^editPaste/",$_POST['menu_action'])){
	      $s_action = "editPaste";
	    }

	    /* Create options */
	    if($_POST['menu_action'] == "user_new"){
	      $s_action = "new";
	    }
	    if($_POST['menu_action'] == "user_tplnew"){
	      $s_action = "new_tpl";
	    }
	    if($_POST['menu_action'] == "multiple_edit"){
	      $s_action = "multiple_edit";
	    }

	    /* handle remove from layers menu */
	    if(preg_match("/^multiple_password_change/",$_POST['menu_action'])){
	      $s_action = "multiple_password_change";
	    }

	    /* handle remove from layers menu */
	    if(preg_match("/^remove_multiple/",$_POST['menu_action'])){
	      $s_action = "del_multiple";
	    }
	    if(preg_match("/^templatize_multiple/",$_POST['menu_action'])){
	      $s_action = "templatize_multiple";
	    }

	    if(preg_match("/^event/",$_POST['menu_action'])){
	      $s_action = $_POST['menu_action'];
	    }
    }

    /* Use template */
    if(isset($_POST['templatize_continue'])){
      $s_action = "templatize_continue";
    }


    /********************
      Create notification event 
     ********************/

    if(preg_match("/^event_/",$s_action) && class_available("DaemonEvent")){
      $ids = $this->list_get_selected_items();
      $uids = array();
      foreach($ids as $id){
        $uids[] = $this->list[$id]['uid'][0];
      }
      if(count($uids)){
        $events = DaemonEvent::get_event_types(USER_EVENT);
        $event = preg_replace("/^event_/","",$s_action);
        if(isset($events['BY_CLASS'][$event])){
          $type = $events['BY_CLASS'][$event];
          $this->usertab = new $type['CLASS_NAME']($this->config);
          $this->usertab->add_users($uids);
          $this->usertab->set_type(TRIGGERED_EVENT);
        }
      }
    }

    /* Abort event dialog */
    if(isset($_POST['abort_event_dialog'])){
      $this->usertab = FALSE;
    }

    /* Save event */
    if(isset($_POST['save_event_dialog'])){
      $this->usertab->save_object();
      $msgs = $this->usertab->check();
      if(count($msgs)){
        msg_dialog::displayChecks($msgs);
      }else{

        $o_queue = new gosaSupportDaemon();
        $o_queue->append($this->usertab);
        if($o_queue->is_error()){
          msg_dialog::display(_("Infrastructure error"), msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
        }else{
          $this->usertab = FALSE;
        }
      }
    }

    /* Display event */
    if($this->usertab instanceof DaemonEvent){
      $this->usertab->save_object();
      return($this->usertab->execute());
    }


    /********************
      Copy & Paste 
     ********************/

    /* Display the copy & paste dialog, if it is currently open */
    if($this->CPPasswordChange == ""){
      $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
      if($ret){
        return($ret);
      }
    }


    /********************
      Change password confirmed
     ********************/

    /* Perform password change */
    if (isset($_POST['password_finish'])){

      /* For security reasons, check if user is allowed to set password again */
      $dn  = $this->dn;
      $acl = $this->ui->get_permissions($dn, "users/password");
      $cacl= $this->ui->get_permissions($dn, "users/user");

      if (preg_match('/w/', $acl) || preg_match('/c/', $cacl)){

        /* Check input and feed errors into 'message' */
        $message= array();

        /* Sanity checks... */
        if ($_POST['new_password'] != $_POST['repeated_password']){

          /* Matching passwords in new and repeated? */
          $message[]= _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
        } else {

          /* Empty password is not permitted by default. */
          if ($_POST['new_password'] == ""){
            msgPool::required(_("New password"));
          }
        }

        /* Errors, or password change? */
        if (count($message) != 0){

          /* Show error message and continue editing */
          msg_dialog::displayChecks($message);
          return($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }

        $config= $this->config;
        $ldap_ui= $this->config->get_ldap_link();
        if(isset($this->usertab->dn)){
          $ldap_ui->cat($this->usertab->dn,array("uid"));
          $user = $ldap_ui->fetch();
        }else{
          $ldap_ui->cat($this->dn,array("uid"));
          $user = $ldap_ui->fetch();
        }
        if((is_array($user))&&(isset($user['uid']))){
          $username= $user['uid'][0];
        }

        /* Set password, perform required steps */
        if ($this->usertab){
          if ($this->usertab->password_change_needed()){
            $obj= $this->usertab->by_object['user'];
            change_password ($this->usertab->dn, $_POST['new_password'],0, $obj->pw_storage);
            if (isset($config->data['MAIN']['EXTERNALPWDHOOK'])){
              exec($config->data['MAIN']['EXTERNALPWDHOOK']." ".$username." ".$_POST['new_password'], $resarr);
            }
            new log("modify","users/".get_class($this),$this->usertab->dn,array(),"Password has been changed");
            unset($this->usertab);
            $this->usertab= NULL;
          }
        } else {
          change_password ($this->dn, $_POST['new_password']);
          if (isset($config->data['MAIN']['EXTERNALPWDHOOK'])){
            exec($config->data['MAIN']['EXTERNALPWDHOOK']." ".$username." ".$_POST['new_password'], $resarr);
          }
          new log("modify","users/".get_class($this),$this->dn,array(),"Password has been changed");
        }
      } else {

        /* Missing permissions, show message */
        msg_dialog::display(_("Password change"),_("You have no permission to change this users password!"),WARNING_DIALOG);
      }
      /* Clean session, delete lock */
      del_lock ($this->dn);
      unset ($this->usertab);
      $this->usertab= NULL;
      $this->lognames= array();;
      $this->sn= "";
      $this->givenName= "";
      $this->uid= "";
      session::un_set('objectinfo');
    }


    /********************
     Change multiple passwords requested 
     ********************/
  
    if($s_action == "multiple_password_change"){
      $this->pwd_change_queue = $this->list_get_selected_items();
    }    


    /********************
      Change password requested  
     ********************/

    /* Password change requested */
    if (($s_action == "change_pw") || (!empty($this->CPPasswordChange)) || count($this->pwd_change_queue)){

      /* Get users whose passwords should be changed. */
      if(count($this->pwd_change_queue)){
        $s_entry= array_pop($this->pwd_change_queue);
      }

      if(!empty($this->CPPasswordChange)){
        $s_entry = $this->CPPasswordChange;
        $this->CPPasswordChange = "";
      }

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      if (preg_match("/w/",$this->ui->get_permissions($this->dn,"users/password"))){

        /* User is allowed to change passwords, save 'dn' and 'acl' for next
           dialog. */
        session::set('objectinfo',$this->dn);
        return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
        

      } else {
        /* User is not allowed. Show message and cancel. */
        msg_dialog::display(_("Password change"),_("You have no permission to change this users password!"),WARNING_DIALOG);
      }
    }



     /********************
      Edit existing entry
     ********************/

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->usertab->config))){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Register usertab to trigger edit dialog */
      $this->usertab= new usertabs($this->config,
          $this->config->data['TABS']['USERTABS'], $this->dn);

      /* Switch tab, if it was requested by the user */
      $this->usertab->current = $s_tab;

      /* Set ACL and move DN to the headline */
      $this->usertab->set_acl_base($this->dn);
      session::set('objectinfo',$this->dn);
    }


    /********************
      Edit multiple entries
     ********************/

    /* User wants to edit data? */
    if ($s_action == "multiple_edit" && !isset($this->usertab->config)){

      $this->dn = array();
      foreach($this->list_get_selected_items() as $id){
        $this->dn[] = $this->list[$id]['dn'];;
      }
      $tmp = new multi_plug($this->config,"usertabs",$this->config->data['TABS']['USERTABS'],
            $this->dn,$this->DivListUsers->selectedBase,"user");
      if ($tmp->entries_locked()){
        return($tmp->display_lock_message());
      }
      $tmp->lock_entries($this->ui->dn);
      if($tmp->multiple_available()){
        $this->usertab = $tmp;
        $this->usertab->set_active_tab($s_tab);
        session::set('objectinfo',$this->usertab->get_object_info());
      }
    }


    /********************
      Edit canceled 
     ********************/

    /* Reset all relevant data, if we get a _cancel request */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      if (isset($this->usertab)){
        del_lock ($this->usertab->dn);
        unset ($this->usertab);
      }
      $this->usertab= NULL;
      $this->lognames= array();;
      $this->sn= "";
      $this->givenName= "";
      $this->uid= "";
      session::un_set('objectinfo');
    }


    /********************
      We want to create a new user, so fetch all available user templates 
     ********************/

    /* Generate template list */
    if ($s_action == "new" || $s_action == "create_user_from_tpl" || $s_action == "templatize_multiple"){

      $this->templates= array();
      $ldap= $this->config->get_ldap_link();

      /* Create list of templates */
      foreach ($this->config->departments as $key => $value){
    
        /* Get acls from different ou's */
        $acl = $this->ui->get_permissions("cn=dummy,".get_people_ou().$value,"users/user")       ; 
 
        /* If creation of a new user is allowed, append this template */
        if (preg_match("/c/",$acl)){
          
          /* Search all templates from the current dn */
          $ldap->cd (get_people_ou().$value);
          $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));

          /* Append */
          if ($ldap->count() != 0){
            while ($attrs= $ldap->fetch()){
              $this->templates[$ldap->getDN()]=
                $attrs['uid'][0]." - ".@LDAP::fix($key);
            }
	    if ($s_action != "templatize_multiple"){
		    $this->templates['none']= _("none");
	    }
          }
        }
      }

      /* Sort templates */
      natcasesort ($this->templates);
      reset ($this->templates);
    }


    /********************
      Apply template to multiple entries requested, display confirm dialog
     ********************/

    if ($s_action=="templatize_multiple"){
      $ids = $this->list_get_selected_items();
      $this->dns = array();
      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->list[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn; 
        }
      }

      $smarty->assign("templates", $this->templates);

      return($smarty->fetch(get_template_path('templatize.tpl', TRUE)));
    }

    /* Perform templatizing after the button has been pressed */
    if ($s_action == "templatize_continue"){

      $acl = $this->ui->get_permissions($_POST['template'], "users/user");

      /* Template readable? */
      if (preg_match('/r/', $acl)){
	$template_dn= $_POST['template'];

        foreach ($this->dns as $dn){
      	  $acl = $this->ui->get_permissions($_POST['template'], "users/user");
      	  if (preg_match('/w/', $acl)){
	          $usertab= new usertabs($this->config, $this->config->data['TABS']['USERTABS'], $dn);
	          $usertab->adapt_from_template($template_dn, array("sn", "givenName", "uid"));
	          $usertab->save();
	          unset ($usertab);
	          $usertab= NULL;
	  } else {
		  msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to modify object '%s'!"), $dn), ERROR_DIALOG);
          }
        }
      } else {
	msg_dialog::display(_("Permission error"), _("You have no permission to use this template!"), ERROR_DIALOG);
      }

    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();
      $this->dns = array();
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
          $dns_names[] = @LDAP::fix($dn);
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        $info = sprintf(msgPool::deleteInfo($dns_names,_("user")));
        $this->msg_dialog = new msg_dialog(_("Delete users"),$info,CONFIRM_DIALOG);
        $this->current_action = $s_action;
      }
    }


    /********************
      Delete MULTIPLE entries confirmed 
     ********************/

      /* Confirmation for deletion has been passed. Users should be deleted. */
      if ($this->current_action == "del_multiple" && is_object($this->msg_dialog) && $this->msg_dialog->is_confirmed()){
        
        $this->current_action = "";  
        
        /* Remove user by user and check acls before removeing them */
        foreach($this->dns as $key => $dn){

          $acl = $this->ui->get_permissions($dn, "users/user"); 
          if (preg_match('/d/', $acl)){

            /* Delete request is permitted, perform LDAP action */
            $this->usertab= new usertabs($this->config, $this->config->data['TABS']['USERTABS'],$dn);
            $this->usertab->set_acl_base();
            $this->usertab->delete ();
            unset ($this->usertab);
            $this->usertab= NULL;
          } else {
            msg_dialog::display(_("Warning"),msgPool::permDelete($dn),WARNING_DIALOG);
            if(isset($this->ui->uid)){
              new log("security","users/".get_class($this),$dn,array(),"Tried to trick deletion.");
            }
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
      Toggle lock status for user
     ********************/
  
    if($s_action == "toggle_lock_status" && isset($this->list[$s_entry])){

      /* Get entry check current status */
      $val = $this->list[$s_entry];
      $pwd = $val['userPassword'][0];

      if(!preg_match("/^\{[^\}]/",$pwd)){
        trigger_error("Can not deactivate user which is using clear password encryption.");
      }else{

        $locked = false;
        if(preg_match("/^[^\}]*+\}!/",$pwd)){
          $locked = true;
        }

        /* Create ldap array to update status */
        $attrs = array("userPassword" => $pwd);
        if($locked){
          $attrs['userPassword'] = preg_replace("/(^[^\}]+\})!(.*$)/","\\1\\2",$attrs['userPassword']);
        }else{
          $attrs['userPassword'] = preg_replace("/(^[^\}]+\})(.*$)/","\\1!\\2",$attrs['userPassword']);
        }

        /* Write new status back to ldap */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($val['dn']);
        $ldap->modify($attrs);
        if (!$ldap->success()){
          msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $val['dn'], 0, get_class()));
        }
      }
    }


    /********************
      Delete entry requested, display confirm dialog
     ********************/

    /* Remove user was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      $this->msg_dialog = new msg_dialog( _("Delete user"),msgPool::deleteInfo(@LDAP::fix($this->dn),_("user")),CONFIRM_DIALOG);
      $this->current_action = $s_action;
    }


    /********************
      Delete entry confirmed 
     ********************/

    /* Confirmation for deletion has been passed. User should be deleted. */
    if ($this->current_action == "del" && is_object($this->msg_dialog) && $this->msg_dialog->is_confirmed()){

      $this->current_action = ""; 
 
      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */

      $acl = $this->ui->get_permissions($this->dn, "users/user"); 
 
      if (preg_match('/d/', $acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->usertab= new usertabs($this->config, $this->config->data['TABS']['USERTABS'],$this->dn);
        $this->usertab->set_acl_base();
        $this->usertab->delete ();
        unset ($this->usertab);
        $this->usertab= NULL;
        msg_dialog::display(_("Information"),_("User successfully removed."),INFO_DIALOG);
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Warning"),msgPool::permDelete(),WARNING_DIALOG);

        if(isset($this->ui->uid)){
          new log("security","users/".get_class($this),$this->dn,array(),"Tried to trick deletion.");
        }
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }

    
    /********************
      Delete entry Canceled 
     ********************/

    /* Delete user canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
    }


    /********************
      Edit entry finished (Save) 
     ********************/

    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->usertab->config))){

      /* Check tabs, will feed message array */
      $this->usertab->last= $this->usertab->current;
      $this->usertab->save_object();
      $message= $this->usertab->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* No errors. Go ahead and prepare to ask for a password
           in case we're creating a new user. 'dn' will be 'new'
           in this case. It is set to the correct value later. */
        if ($this->dn == "new"){
          $set_pass= 1;
        } else {
          $set_pass= 0;
        }

        /* Save user data to ldap */
        if($this->usertab->save() == 1){
          return;
        }

        if (!isset($_POST['edit_apply'])){
          /* User has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          /* In case of new users, ask for a password, skip this for templates */
          if (($set_pass || $this->usertab->password_change_needed()) && !$this->is_template){
            $this->dn = $this->usertab->dn;
            return($smarty->fetch(get_template_path('password.tpl', TRUE)));
          }

          unset ($this->usertab);
          $this->usertab= NULL;
          session::un_set('objectinfo');
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        msg_dialog::displayChecks($message);
      }
    }



    /********************
      Create a new user,template, user from template 
     ********************/

    /* Check selected options for template */
    if (isset($_POST['template_continue'])){
      $message = array();
      if(!isset($_POST['template']) || (empty($_POST['template']))){
        $message[]= msgPool::invalid(_("Template"));
      }
      if(!isset($_POST['sn']) || (empty($_POST['sn']))){
        $message[]= msgPool::required(_("Name"));
      }
      if(!isset($_POST['givenName']) || (empty($_POST['givenName']))){
        $message[]= msgPool::required(_("Given name"));
      }
    
      /* Show error message / continue editing */
      if (count($message) > 0){
        msg_dialog::displayChecks($message);

        foreach(array("sn", "givenName", "uid", "template") as $attr){
          if(isset($_POST[$attr])){
            $smarty->assign("$attr", $_POST[$attr]);
          }else{
            $smarty->assign("$attr", "");
          }
        }
        $smarty->assign("templates",$this->templates);
        $smarty->assign("got_uid",$this->got_uid);
        $smarty->assign("edit_uid",false);
        return($smarty->fetch(get_template_path('template.tpl', TRUE)));

      }
    }

    /* New user/template request */
    if (($s_action=="create_user_from_tpl")||($s_action=="new") || ($s_action=="new_tpl")){
      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";
      
      if (isset($this->config->current['IDGEN'])){
        $this->got_uid= false;
      } else {
        $this->got_uid= true;
      }

      /* Create new usertab object */
      $this->usertab= new usertabs($this->config,$this->config->data['TABS']['USERTABS'], $this->dn);
      $this->usertab->by_object['user']->base= $this->DivListUsers->selectedBase;
      $this->usertab->set_acl_base('dummy,'.$this->DivListUsers->selectedBase);

      /* Take care about templates */
      if ($s_action=="new_tpl"){
        $this->is_template= TRUE;
        $this->usertab->set_template_mode ();
      } else {
        $this->is_template= FALSE;
      }

      /* Use template if there are any of them */
      if ((count($this->templates) && ($s_action!='new_tpl'))||($s_action=="create_user_from_tpl")){
        foreach(array("sn", "givenName", "uid", "got_uid", "templates") as $attr){
          $smarty->assign("$attr", $this->$attr);
        }
        if ($s_action=="create_user_from_tpl"){
          $smarty->assign("template", $this->dn= $this->list[trim($s_entry)]['dn']);
        } else {
          $smarty->assign("template", "none");
        }
        $smarty->assign("edit_uid", "");
        return($smarty->fetch(get_template_path('template.tpl', TRUE)));
      }
    }

    /********************
      Template selected continue edit
     ********************/

    /* Continue template editing */
    if ((isset($_POST['template_continue'])) && ($_POST['template'] != 'none') && (!isset($_POST['uid']))){

      $this->sn             = $_POST['sn'];
      $this->givenName      = $_POST['givenName'];

      /* Check for requred values */
      $message= array();
      if ($this->sn == "") {
        $message[]= msgPool::required(_("Name"));
      }
      if ($this->givenName == "") {
        $message[]= msgPool::required(_("Given name"));
      }

      /* Check if dn is used */
      $dn= preg_replace("/^[^,]+,/i", "", $_POST['template']);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($dn);
      $ldap->search ("(&(sn=".normalizeLdap($this->sn).")(givenName=".normalizeLdap($this->givenName)."))", array("givenName"));
      if ($ldap->count () != 0){
        msgPool::duplicated(_("Name"));
      }

      /* Show error message / continue editing */
      if (count($message) > 0){
        msg_dialog::displayChecks($message);
      } else {
        $attributes= array('sn' => $this->sn, 'givenName' => $this->givenName);
        if (isset($this->config->current['IDGEN']) &&
            $this->config->current['IDGEN'] != ""){
          $uids= gen_uids ($this->config->current['IDGEN'], $attributes);
          if (count($uids)){
            $smarty->assign("edit_uid", "false");
            $smarty->assign("uids", $uids);
            $this->uid= current($uids);
          }
        } else {
          $smarty->assign("edit_uid", "");
          $this->uid= "";
        }
        $this->got_uid= true;
      }

      foreach(array("sn", "givenName", "uid", "got_uid", "templates") as $attr){
        $smarty->assign("$attr", $this->$attr);
      }
      if (isset($_POST['template'])){
        $smarty->assign("template", $_POST['template']);
      }
      return($smarty->fetch(get_template_path('template.tpl', TRUE)));
    }

    /********************
      No template selected continue edit
     ********************/

    /* No template. Ok. Lets fill data into the normal user dialog */
    if (isset($_POST['template_continue']) && $_POST['template'] == 'none'){
      foreach(array("sn", "givenName", "uid") as $attr){
        if (isset($_POST[$attr])){
          $this->usertab->by_object['user']->$attr= $_POST[$attr];
        }
      }
    }


    /********************
      Template selected continue edit
     ********************/

    /* Finish template preamble */
    if (isset($_POST['template_continue']) && $_POST['template'] != 'none' && (isset($_POST['uid']))){

      /* Might not be filled if IDGEN is unset */
      $this->sn                 = $_POST['sn'];
      $this->givenName          = $_POST['givenName'];

      /* Move user supplied data to sub plugins */
      $this->uid                = $_POST['uid'];
      $this->usertab->uid       = $this->uid;
      $this->usertab->sn        = $this->sn;
      $this->usertab->givenName = $this->givenName;
      $template_dn              = $_POST['template'];
      $this->usertab->adapt_from_template($template_dn);
      $template_base            = preg_replace("/^[^,]+,".normalizePreg(get_people_ou())."/", '', $template_dn);
      $this->usertab->by_object['user']->base= $template_base;
    }
   
 
    /********************
      If no template was selected set base
     ********************/

    if (isset($_POST['template_continue']) && ($_POST['template'] == 'none')){
      $this->usertab->by_object['user']->base= $this->DivListUsers->selectedBase;
    }


    /********************
      Display subdialog 
     ********************/

    /* Show tab dialog if object is present */
    if(isset($this->usertab->config)){

      $display= $this->usertab->execute();

      /* Don't show buttons if tab dialog requests this */
      
        $dia = FALSE;
        if(isset($this->usertab->by_object[$this->usertab->current]->dialog)){
          $dia = $this->usertab->by_object[$this->usertab->current]->dialog;
        }

        if(!is_object($dia) && $dia != TRUE){
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
      return ($display);
    }
    
    /* Check if there is a snapshot dialog open */
    $base = $this->DivListUsers->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }
  
    /* Return rendered main page */
        /* Display dialog with system list */
    $this->DivListUsers->parent = $this;
    $this->DivListUsers->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListUsers->SubSearch){
      $this->DivListUsers->AddDepartments($this->DivListUsers->selectedBase,4,1);
    }
    $this->reload();
    $this->DivListUsers->setEntries($this->list);
    return($this->DivListUsers->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_people_ou().$this->DivListUsers->selectedBase));
  }  


  function reload()
  {
    /* Set base for all searches */
    $base= $this->DivListUsers->selectedBase;
    $this->list =array();

    /* Get filter configuration */
    $Regex                = $this->DivListUsers->Regex;
    $SubSearch            = $this->DivListUsers->SubSearch;
    $ShowTemplates        = $this->DivListUsers->ShowTemplates;
    $ShowFunctionalUsers  = $this->DivListUsers->ShowFunctionalUsers;
    $ShowUnixUsers        = $this->DivListUsers->ShowUnixUsers;
    $ShowMailUsers        = $this->DivListUsers->ShowMailUsers;
    $ShowSambaUsers       = $this->DivListUsers->ShowSambaUsers;
    $ShowProxyUsers       = $this->DivListUsers->ShowProxyUsers;

    /* Setup filter depending on selection */
    $filter="";
    if ($this->config->current['SAMBAVERSION'] == 3){
      $samba= "sambaSamAccount";
    } else {
      $samba= "sambaAccount";
    }

    if ($ShowFunctionalUsers){
      $filter.= "(&(objectClass=gosaAccount)(!(|(objectClass=posixAccount)".
                "(objectClass=gosaMailAccount)(objectClass=$samba)".
                "(objectClass=gosaProxyAccount))))";
    }
    if ($ShowUnixUsers){
      $filter.= "(objectClass=posixAccount)";
    }
    if ($ShowMailUsers){
      $filter.= "(objectClass=gosaMailAccount)";
    }
    if ($ShowSambaUsers){
      $filter.= "(objectClass=$samba)";
    }
    if ($ShowProxyUsers){
      $filter.= "(objectClass=gosaProxyAccount)";
    }
    if ($ShowTemplates){
      $filter= "(|(objectClass=gosaUserTemplate)(&(objectClass=gosaAccount)(|$filter)))";
    } else {
      $filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(|$filter))";
    }
    $filter= "(&(|(uid=".normalizeLdap($Regex).")(sn=".normalizeLdap($Regex).")(givenName=".normalizeLdap($Regex)."))$filter)";

    /* Generate userlist */
    $ldap= $this->config->get_ldap_link(TRUE);

    if ($SubSearch){
      $ListTemp =  get_sub_list($filter, "users", get_people_ou(),$base,
                            array("uid", "givenName", "sn", "objectClass","userPassword"), GL_SUBSEARCH | GL_SIZELIMIT);
    } else {
      $base= get_people_ou().$base;
      $ListTemp = get_sub_list($filter, "users", get_people_ou(),$base, 
                            array("uid", "givenName", "sn", "objectClass","userPassword"), GL_SIZELIMIT);
    }
    $SortTemp = array();
    $List = array();
    foreach($ListTemp as $Key => $Entry){

      /* Skip entries that are not located under the people ou (normaly 'ou=people,')
       * Else winstations will be listed too, if you use the subtree flag. 
       */
      if(!preg_match("/".normalizePreg(get_people_ou())."/i",$Entry['dn'])){
        continue;
      }else{

        // Generate caption for rows
        if (isset($Entry["sn"]) && isset($Entry["givenName"])){
          $display= $Entry["sn"][0].", ".$Entry["givenName"][0]." [".$Entry["uid"][0]."]";
        } else {
          $display= "[".$Entry["uid"][0]."]";
        }

        $display = strtolower($display);
        $List[$display] = $Entry;
        $SortTemp[$display] = $display;
      }
    }
    natcasesort($SortTemp);
    reset($SortTemp);

    $this->list = array();
    foreach($SortTemp as $Key){
      $this->list[] = $List[$Key];
    }
  }

  function remove_lock()
  {
    /* Remove user lock if a DN is marked as "currently edited" */
    if (isset($this->usertab->dn)){
      del_lock ($this->usertab->dn);
    }
  }


  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){
      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn = $this->list[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"usertabs","USERTABS","users");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->list[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","usertabs","USERTABS","users");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","usertabs","USERTABS","users");
        }
      }
    }
    
    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }

    /* Return C&P dialog */ 
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){
   
      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();
      $this->CopyPasteHandler->SetVar("base",$this->DivListUsers->selectedBase); 

      /* Set CPPasswordChange to s_entry which indicates that this entry requires a new password. */
      if(isset($_POST['passwordTodo']) && ($_POST['passwordTodo'] == "new")){
        $s_entry = $this->CopyPasteHandler->last_entry();
        $this->reload();
        foreach($this->list as $key => $entry){
          if($entry['dn'] == $s_entry){
            $this->CPPasswordChange = $key;
          }
        }
      }

      /* Return dialog data */
      if(!empty($data) && $this->CPPasswordChange == ""){
        return($data);
      }
    }

    /* Automatically disable status for pasting */ 
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
  }


  function save_object()
  {
    /* Handle divlist filter && department selection*/
    if(!is_object($this->usertab)){
      $this->DivListUsers->save_object();
    }
    if(is_object($this->CopyPasteHandler)){
      $this->CopyPasteHandler->save_object();
    }
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
  

  /* A set of disabled and therefore overloaded functions. They are
     not needed in this class. */
  function remove_from_parent() { } 
  function check() { } 
  function save() { } 
  function adapt_from_template($dn, $skip= array()) { } 
  function password_change_needed() { } 

} /* ... class userManagement */
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
