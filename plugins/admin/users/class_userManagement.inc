<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2006 - Cajus Pollmeier <pollmeier@gonicus.de>

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

/* Include user tab class */
require "tabs_user.inc";


class userManagement extends plugin
{
  /* Plugin definitions */
  var $plHeadline= "Users";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $usertab              = NULL;
  var $ui                   = NULL;
  var $acl                  = "";
  var $templates            = array();
  var $got_uid              = false;
  var $CopyPasteHandler     = NULL;
  var $CPPasswordChange     = ""; // Contains the entry id which should get a new password
  var $DivListUsers;

  function userManagement($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

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
    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^user_edit_/","/^user_del_/");

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
      foreach(array("del"       => "user_del",    "edit"      => "user_edit",
                    "new"       => "user_new",
                    "new_tpl"   => "user_tplnew",
                    "create_user_from_tpl"          => "userfrom_tpl",
                    "change_pw" => "user_chgpw", 
                    "editPaste" => "editPaste",   "copy"      => "copy",
                    "cut"       => "cut",
                    "toggle_lock_status" => "toggle_lock_status" ) as $act => $name){

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

    /* Some may be active but diabled in gosa.conf. */
    if(!search_config($this->config->data['TABS'], $s_tab , "CLASS")){
      $s_tab = "user";
    }

    /* Get 'dn' from posted 'uid' */
    if(in_array_ics($s_action,array("editPaste","cut","copy")) || ($this->CopyPasteHandler && $this->CopyPasteHandler->stillOpen())){

      if(isset($this->list[trim($s_entry)]['dn'])){
        $dn= $this->list[trim($s_entry)]['dn'];
      }else{
        $dn = $this->DivListUsers->selectedBase;
      }

      $acl= get_permissions ($dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "user", $dn);

      if($acl != "#all#"){
        print_red (_("You are not allowed to execute this method!")); 
      }else{
        /* Display the copy & paste dialog, if it is currently open */
        $ret = $this->copyPasteHandling($s_action,$s_entry);
        if($ret){
          return($ret);
        }
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
        if($locked){
          show_ldap_error($ldap->get_error(),_("Could not set user status from locked to unlocked."));
        }else{
          show_ldap_error($ldap->get_error(),_("Could not set user status from unlocked to locked."));
        }
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

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Register usertab to trigger edit dialog */
      $this->usertab= new usertabs($this->config, 
          $this->config->data['TABS']['USERTABS'], $this->dn);

      /* Switch tab, if it was requested by the user */
      $this->usertab->current = $s_tab;

      /* Set ACL and move DN to the headline */
      $this->usertab->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
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
      unset ($_SESSION['objectinfo']);
    }


    /********************
      Change password requested  
     ********************/

    /* Password change requested */
    if (($s_action == "change_pw") || (!empty($this->CPPasswordChange))){

      if(!empty($this->CPPasswordChange)){
        $s_entry = $this->CPPasswordChange;
        $this->CPPasswordChange = "";
      }

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->list[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "user", $this->dn);
      if (chkacl($acl, "password") == ""){

        /* User is allowed to change passwords, save 'dn' and 'acl' for next
           dialog. */
        $this->acl= $acl;
        $_SESSION['objectinfo']= $this->dn;
        return ($smarty->fetch(get_template_path('password.tpl', TRUE)));

      } else {
        /* User is not allowed. Show message and cancel. */
        print_red (_("You are not allowed to set this users password!"));
      }
    }


    /********************
      Change password confirmed
     ********************/

    /* Perform password change */
    if (isset($_POST['password_finish'])){

      /* For security reasons, check if user is allowed to set password again */
      if (chkacl($this->acl, "password") == "" || chkacl($this->acl, "create")){

        /* Check input and feed errors into 'message' */
        $message= array();

        /* Sanity checks... */
        if ($_POST['new_password'] != $_POST['repeated_password']){

          /* Matching passwords in new and repeated? */
          $message[]= _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
        } else {

          /* Empty password is not permitted by default. */
          if ($_POST['new_password'] == ""){
            $message[]= _("The password you've entered as 'New password' is empty.");
          }
        }

        /* Errors, or password change? */
        if (count($message) != 0){

          /* Show error message and continue editing */
          show_errors ($message);
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

            gosa_log ("Password for '".$this->usertab->dn."' has been changed");
            unset($this->usertab);
            $this->usertab= NULL;
          }
        } else {
          change_password ($this->dn, $_POST['new_password']);
          if (isset($config->data['MAIN']['EXTERNALPWDHOOK'])){
            exec($config->data['MAIN']['EXTERNALPWDHOOK']." ".$username." ".$_POST['new_password'], $resarr);
          }

          gosa_log ("Password for '".$this->dn."' has been changed");
        }
      } else {

        /* Missing permissions, show message */
        print_red (_("You are not allowed to set this users password!"));
      }

      /* Clean session, delete lock */
      del_lock ($this->dn);
      unset ($this->usertab);
      $this->usertab= NULL;
      $this->lognames= array();;
      $this->sn= "";
      $this->givenName= "";
      $this->uid= "";
      unset ($_SESSION['objectinfo']);
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
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "user", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the user %s."), @LDAP::fix($this->dn)));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this user!"));
      }
    }


    /********************
      Delete entry confirmed 
     ********************/

    /* Confirmation for deletion has been passed. User should be deleted. */
    if (isset($_POST['delete_user_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->usertab= new usertabs($this->config, $this->config->data['TABS']['USERTABS'],$this->dn);
        $this->usertab->set_acl(array($this->acl));
        $this->usertab->delete ();
        gosa_log ("User object '".$this->dn."' has been removed");
        unset ($this->usertab);
        $this->usertab= NULL;
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this user!"));

        if(isset($this->ui->uid)){
          gosa_log ("Warning: '".$this->ui->uid."' tried to trick user deletion.");
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
    if ((isset($_POST['edit_finish'])) && (isset($this->usertab->config))){

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
          gosa_log ("User object '".$this->dn."' saving failed.");
          return;
        }
        gosa_log ("User object '".$this->dn."' has been saved");

        /* User has been saved successfully, remove lock from LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* In case of new users, ask for a password, skip this for templates */
        if (($set_pass || $this->usertab->password_change_needed()) && !$this->is_template){
          return($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }

        unset ($this->usertab);
        $this->usertab= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /********************
      We want to create a new user, so fetch all available user templates 
     ********************/

    /* Generate template list */
    if (($s_action=="new")||($s_action=="create_user_from_tpl")){

      $this->templates= array();
      $ldap= $this->config->get_ldap_link();

      /* Create list of templates */
      foreach ($this->config->departments as $key => $value){
    
        /* Get acls from different ou's */
        $acl= get_permissions (get_people_ou().$value, $this->ui->subtreeACL);
        $acl= get_module_permission($acl, "user", get_people_ou().$value);
  
        /* If creation of a new user is allowed, append this template */
        if (chkacl($acl, "create") == ""){
          
          /* Search all templates from the current dn */
          $ldap->cd (get_people_ou().$value);
          $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));

          /* Append */
          if ($ldap->count() != 0){
            while ($attrs= $ldap->fetch()){
              $this->templates[$ldap->getDN()]=
                $attrs['uid'][0]." - ".@LDAP::fix($key);
            }
            $this->templates['none']= _("none");
          }
        }
      }

      /* Sort templates */
      natcasesort ($this->templates);
      reset ($this->templates);
    }


    /********************
      Create a new user,template, user from template 
     ********************/

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
      $this->usertab->set_acl(array(':all'));
      $this->usertab->by_object['user']->base= $this->DivListUsers->selectedBase;

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
        $message[]= _("The required field 'Name' is not set.");
      }
      if ($this->givenName == "") {
        $message[]= _("The required field 'Given name' is not set.");
      }

      /* Check if dn is used */
      $dn= preg_replace("/^[^,]+,/i", "", $_POST['template']);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($dn);
      $ldap->search ("(&(sn=".normalizeLdap($this->sn).")(givenName=".normalizeLdap($this->givenName)."))", array("givenName"));
      if ($ldap->count () != 0){
        $message[]= _("A person with the choosen name is already used in this tree.");
      }

      /* Show error message / continue editing */
      if (count($message) > 0){
        show_errors ($message);
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

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($template_base, $this->ui->subtreeACL);
      $this->usertab->set_acl($acl);
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
      if (!$this->usertab->by_object[$this->usertab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }
      
    /* Return rendered main page */
        /* Display dialog with system list */
    $this->DivListUsers->parent = $this;
    $this->DivListUsers->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListUsers->SubSearch){
      $this->DivListUsers->AddDepartments($this->DivListUsers->selectedBase,4);
    }
    $this->reload();
    $this->DivListUsers->setEntries($this->list);
    return($this->DivListUsers->Draw());
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
      $ListTemp =  get_list($filter, $this->ui->subtreeACL, $base,
                            array("uid", "givenName", "sn", "objectClass","userPassword"), GL_SUBSEARCH | GL_SIZELIMIT);
    } else {
      $base= get_people_ou().$base;
      $ListTemp = get_list($filter, $this->ui->subtreeACL, $base, 
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

 
  /* Perform copy & paste requests
      If copy&paste is in progress this returns a dialog to fix required attributes 
   */ 
  function copyPasteHandling($s_action,$s_entry)
  {
    /* Only perform copy/paste if it is enabled */
    if($this->CopyPasteHandler){

      /* Prepare current object to be pasted */
      if( $s_action == "editPaste" || $this->CopyPasteHandler->stillOpen()){

        $this->CopyPasteHandler->save_object();
        $this->CopyPasteHandler->SetVar("base", $this->DivListUsers->selectedBase);

        /* Execute copy & paste dialog and display returned data, normaly a dialog which allows 
            us to solve all attribute mismatches for this object.
            If nothing is returned, copy & paste was succesfully or aborted */
        if(($ret= $this->CopyPasteHandler->execute())){
          return ($ret);
        }

        /* Use the last dn to search for it's ID in the newly generated list. */
        $dn= $this->CopyPasteHandler->lastdn;

        /* Get new user list */
        $this->reload();
        foreach($this->list as $id => $entry){
          if($entry['dn'] == $dn){
            $s_entry= $id;
            break;
          }
        }
       
        /* Set CPPasswordChange to s_entry which indicates that this entry requires a new password. */
        if(isset($_POST['passwordTodo']) && ($_POST['passwordTodo'] == "new")){
          $this->CPPasswordChange = $s_entry;
        }
      }

      /* Copy selected object 
          Create a new empty object and the current selected object. 
          Send both to copy&paste class*/
      if($s_action == "copy"){
        $this->CopyPasteHandler->Clear();
        $dn= $this->list[trim($s_entry)]['dn'];
        $acl= get_permissions ($dn, $this->ui->subtreeACL);
        $obj    = new usertabs($this->config, $this->config->data['TABS']['USERTABS'], $dn);
        $objNew = new usertabs($this->config, $this->config->data['TABS']['USERTABS'], "new");
        $obj->    set_acl($acl);
        $objNew-> set_acl($acl);
        $this->CopyPasteHandler->Copy($obj,$objNew);
      }

      /* Cut selected object. 
          Open user object and send it to the copy & paste handler */
      if($s_action == "cut"){
        $this->CopyPasteHandler->Clear();
        $dn= $this->list[trim($s_entry)]['dn'];
        $acl= get_permissions ($dn, $this->ui->subtreeACL);
        $obj= new usertabs($this->config, $this->config->data['TABS']['USERTABS'], $dn);
        $obj->set_acl($acl);
        $this->CopyPasteHandler->Cut($obj);
      }
    }
  }

  function save_object()
  {
    /* Handle divlist filter && department selection*/
    if(!is_object($this->usertab)){
      $this->DivListUsers->save_object();
    }
  }

  /* A set of disabled and therefore overloaded functions. They are
     not needed in this class. */
  function remove_from_parent() { } 
  function check() { } 
  function save() { } 
  function adapt_from_template($dn) { } 
  function password_change_needed() { } 
  function show_header($button_text, $text, $disabled= FALSE) { }

} /* ... class userManagement */
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
