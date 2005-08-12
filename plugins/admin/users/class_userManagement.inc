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
require "tabs_user.inc";

class userManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Users";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $usertab= NULL;
  var $userlist= array();
  var $ui= NULL;
  var $acl= "";
  var $templates= array();
  var $got_uid= false;
  var $departments=array();

  function userManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!isset($_SESSION["userfilter"])){
      $base= get_base_from_people($ui->dn);
      $userfilter= array( "mailusers"       => "checked",
          "unixusers"       => "checked",
          "templates"       => "",
          "subsearch"       => "",
          "proxyusers"      => "checked",
          "sambausers"      => "checked",
          "faxusers"        => "checked",
          "functionalusers" => "checked",
          "depselect"       => $base,
          "regex"           => "*");
      $_SESSION["userfilter"] = $userfilter;
    }
  }

  function execute()
  {
    $smarty       = get_smarty();             // Smarty instance
    $userfilter   = get_global("userfilter"); // contains Filter Settings
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
      $s_action="open";
      $s_entry = base64_decode($_GET['dep_id']);
      $userfilter['depselect']= "".$this->config->departments[trim($s_entry)];
    }

    /* Test Posts */  
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/user_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/user_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/user_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/user_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_chgpw_/i","",$key);
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      }elseif(preg_match("/userfrom_tpl.*/i",$key)){
        $s_action="create_user_from_tpl";
        $s_entry  = preg_replace("/userfrom_tpl_/i","",$key);
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action = "edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);
    if(preg_match("/.*-.*/",$s_entry)){
      $s_tab   = preg_replace("/^.*-/i","",$s_entry);
      $s_entry = preg_replace("/-.*$/i","",$s_entry);
    }else{
      $s_tab   = "user";
    };

    /* Department changed? */
    if(isset($_POST['depselect']) && $_POST['depselect']){
      $userfilter['depselect']= $_POST['depselect'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $userfilter['depselect']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $userfilter['depselect']=(preg_replace("/^[^,]+,/","",$userfilter['depselect']));
    }

    if($s_action=="root"){
      $userfilter['depselect']=($this->config->current['BASE']);
    }


    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$userfilter['depselect']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $userfilter['depselect']= $this->config->departments[trim($base_back)];
      }else{
        $userfilter['depselect']= $this->config->departments["/"];
      }
    }

    /* End for new List Managment */

    /* Save filter data if we are in the headpage */
    if (!isset($this->usertab)){
      if (isset($_POST['regexit'])){
        $userfilter["regex"]= $_POST['regexit'];
      }
    }
    if(isset($_POST['regexit'])){
      foreach( array("functionalusers", "unixusers", "mailusers","sambausers", "proxyusers", "faxusers", "templates", "subsearch") as $type){
        if (isset($_POST[$type])) {
          $userfilter[$type]= "checked";
        } else {
          $userfilter[$type]= "";
        }
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $userfilter['regex']= $s;
    }
    register_global("userfilter", $userfilter);


    /* React on user interaction here */
    if (($_SERVER["REQUEST_METHOD"] == "POST")||($_SERVER["REQUEST_METHOD"]== "GET")){

      /* User wants to edit data? */
      if ($s_action=="edit"){

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
        //        $_POST[$s_tab]=$s_tab;

        // This would be better
        $this->usertab->current = $s_tab;
        $this->usertab->set_acl($acl);

        $_SESSION['objectinfo']= $this->dn;
      }
    }

    /* Reset requested? */
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

    /* Password change requested */
    if ($s_action=="change_pw"){

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

        /* Errors, or change password? */
        if (count($message) != 0){

          /* Show error message and continue editing */
          show_errors ($message);
          return($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }

        $config= $this->config;
        $ldap_ui= $this->config->get_ldap_link();
        if(isset($this->usertab->dn)){
          $user = ($ldap_ui->fetch($ldap_ui->cat($this->usertab->dn)));
        }else{
          $user = ($ldap_ui->fetch($ldap_ui->cat($this->dn)));
        }
        if((is_array($user))&&(isset($user['uid']))){
          $username= $user['uid'][0];
        }

        /* Set password, perform required steps */
        if ($this->usertab){
          if ($this->usertab->password_change_needed()){
            $obj= $this->usertab->by_object['user'];
            change_password ($this->usertab->dn, $_POST['new_password'],
                0, $obj->pw_storage);
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
      $this->reload();
      unset ($this->usertab);
      $this->usertab= NULL;
      $this->lognames= array();;
      $this->sn= "";
      $this->givenName= "";
      $this->uid= "";
      unset ($_SESSION['objectinfo']);
    }


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
        $smarty->assign("info", sprintf(_("You're about to delete the user %s."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this user!"));
      }
    }



    /* Confirmation for deletion has been passed. User should be deleted. */
    if (isset($_POST['delete_user_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->usertab= new usertabs($this->config, $this->config->data['TABS']['USERTABS'],
            $this->dn);
        $this->usertab->set_acl(array($this->acl));
        $this->usertab->delete ();
        gosa_log ("User object '".$this->dn."' has been removed");
        unset ($this->usertab);
        $this->usertab= NULL;

        /* User list has changed, reload it. */
        $this->reload ();
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


    /* Delete user canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
    }


    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if (isset($_POST['edit_finish'])){

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

        /* User has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* In case of new users, ask for a password */
        if (($set_pass || $this->usertab->password_change_needed()) &&
            !$this->is_template){

          return($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }

        /* There's no page reload so we have to read new users at
           this point. */
        $this->reload ();
        unset ($this->usertab);
        $this->usertab= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* Generate template list */
    if ($s_action=="new"){
      $this->templates= array();
      $ldap= $this->config->get_ldap_link();
      foreach ($this->config->departments as $key => $value){
        $acl= get_permissions (get_people_ou().$value, $this->ui->subtreeACL);
        $acl= get_module_permission($acl, "user", get_people_ou().$value);
        if (chkacl($acl, "create") == ""){
          $ldap->cd (get_people_ou().$value);
          $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));

          if ($ldap->count() != 0){
            while ($attrs= $ldap->fetch()){
              $this->templates[$ldap->getDN()]=
                $attrs['uid'][0]." - $key";
            }
            $this->templates['none']= _("none");
          }
        }
      }
      natcasesort ($this->templates);
      reset ($this->templates);
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
      $this->usertab= new usertabs($this->config,
          $this->config->data['TABS']['USERTABS'], $this->dn);
      $this->usertab->set_acl(array(':all'));

      /* Take care about templates */
      if ($s_action=="new_tpl"){
        $this->is_template= TRUE;
        $this->usertab->set_template_mode ();
      } else {
        $this->is_template= FALSE;
      }

      /* Use template if there are any of them */
      if (count($this->templates) && !isset($_POST['new_template'])){
        foreach(array("sn", "givenName", "uid", "got_uid", "templates") as $attr){
          $smarty->assign("$attr", $this->$attr);
        }
        if ($s_action=="create_user_from_tpl"){
          $smarty->assign("template", $this->dn= $this->list[trim($s_entry)]['dn']);
        } else {
          $smarty->assign("template", "");
        }
        $smarty->assign("edit_uid", "");
        return($smarty->fetch(get_template_path('template.tpl', TRUE)));
      }
    }

    /* Continue template editing */
    if ((isset($_POST['template_continue']) && $_POST['template'] != 'none' && !isset($_POST['uid']))){
      $this->sn= $_POST['sn'];
      $this->givenName= $_POST['givenName'];

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
      $ldap->search ("(&(sn=".$this->sn.")(givenName=".$this->givenName."))", array("givenName"));
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

    /* No template. Ok. Lets fill data into the normal user dialog */
    if (isset($_POST['template_continue']) && $_POST['template'] == 'none'){
      foreach(array("sn", "givenName", "uid") as $attr){
        if (isset($_POST[$attr])){
          $this->usertab->by_object['user']->$attr= $_POST[$attr];
        }
      }
    }

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
      $template_base            = $userfilter['depselect'];//preg_replace("/^[^,]+,".get_people_ou()."/", '', $template_dn);
      $this->usertab->by_object['user']->base= $template_base;

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($template_base, $this->ui->subtreeACL);
      $this->usertab->set_acl($acl);
    }

    /* Show tab dialog if object is present */
    if ($this->usertab){
      $display= $this->usertab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->usertab->by_object[$this->usertab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    $smarty= get_smarty();

    /* Check sorting variable */
    if (!isset($this->usertab) &&
        !isset($_POST['new_user']) &&
        !isset($_POST['new_template']) &&
        !isset($_POST['delete_user']) &&
        !isset($_POST['setpass_user']) &&
        !isset($_POST['select_user'])){
      $this->reload();
    }

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Prepare departments */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($userfilter['depselect'] == $key){
        $options.= "<option selected value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    /* NEW LIST MANAGMENT */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input type='image' align='middle' src='images/list_new_user.png' title='"._("Create new user")."' alt='"._("New user")."' name='user_new'>&nbsp;".
      " <input type='image' align='middle' src='images/list_new.png' title='"._("Create new template")."' alt='"._("New template")."' name='user_tplnew'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Current base")."&nbsp;<select name='depselect' onChange='mainform.submit()'>$options</select>".
      " <input type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";

    $action= "<input type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit user")."'>";
    $action.= "<input type='image' src='images/list_password.png' alt='"._("password")."' name='user_chgpw_%KEY%' title='"._("Change password")."'>";
    $action.= "<input type='image' src='images/editdelete.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete user")."'>";



    $divlist = new divlist("userstab");
    $divlist->SetHeader(array(
          array("string" => "&nbsp;"),
          array("string" => _("Username")." / "._("Department")),
          array("string" => _("Properties")),
          array("string" => _("Actions"), "attach" => "style='border:none'" )));


    $divlist->SetSummary(_("This table displays all users, in the selected tree."));
    $divlist->SetEntriesPerPage(20);

    // Defining Links 
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    /* Insert departments in divlist*/
    foreach($this->departments as $key=> $val){

      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/".$keys."\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val));
      $field3 = array("string" => "&nbsp;");
      $field4 = array("string" => "&nbsp;", "attach" => "style='border:none'");


      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }

    /* Pictures for Extensions */
    $usrimg   ="<input type='image' src='images/penguin.png' alt='"._("GOsa")."'   
                    name='user_edit_%KEY%-user' title='"._("Edit generic properties")."'>";
    $posiximg = "<input type='image' src='images/select_user.png' alt='"._("Posix")."'  
                    name='user_edit_%KEY%-posixAccount' title='"._("Edit UNIX properties")."'>";
    $mailimg  = "<input type='image' src='images/mailto.png' alt='"._("Mail")."'     
                    name='user_edit_%KEY%-mailAccount' title='"._("Edit mail properties")."'>";
    $fonimg   = "<input type='image' src='images/%image%' alt='"._("Phone")."'     
                    name='user_edit_%KEY%-phoneAccount' title='"._("Edit phone properties")."%title%'>";
    $faximg   = "<input type='image' src='images/fax_small.png' alt='"._("Fax")."'     
                    name='user_edit_%KEY%-gofaxAccount' title='"._("Edit fax properies")."'>";
    $sambaimg = "<input type='image' src='images/select_winstation.png' alt='"._("Samba")."'     
                    name='user_edit_%KEY%-sambaAccount' title='"._("Edit samba properties")."'>"; 
    $tplcreateuserimg  = "<input type='image' src='images/list_new.png' alt='"._("Create user from template")."'
                    name='userfrom_tpl_%KEY%' title='"._("Create user with this template")."'>";   
    

    $empty    = "<img src='images/empty.png' style='width:16px;height:16px;' alt=''>"; 

    // User and Template  Images  
    $userimg  = "<img src='images/select_user.png' alt='User' title='%s'>";   
    $tplimg   = "<img src='images/select_template.png' alt='Template' title='%s'>";   
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    // Defines Arrays to save User and Templates
    $tpls = array();
    $users= array();

    /* Get Configuration for goFon DB (if it is set), to connect to the asterisk tables.
     * Read Sip Table for specified account (for each user with phoneAccount).
     * Check the attributes ip port and regseconds.
     * If regseconds is set and >0 , the phone is logged in.
     * Else the phone is currently not logged.
     * If we can't read any Data from the DB or there is no goFon DB specified
     * show old style without status icons.
     */
    $r_db   =false;
    $r_con  =false;
    if (isset($_SESSION['config']->data['SERVERS']['FON'])){
      $a_SETUP= $_SESSION['config']->data['SERVERS']['FON'];
      // Connect to DB server
  
      /* 
        If you want to disable the status pictures, just comment out the next line
      */
      $r_con= @mysql_connect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);
      // Check if we are  connected correctly
      if($r_con){
        $r_db  =  @mysql_select_db($a_SETUP['DB'],$r_con);
      }
    }

    // Test Every Entry and generate divlist Array  
    foreach($this->list as $key => $val){
      $connected  = ""; // This string represents timestamp or offline status 
      $ip_port    = ""; // String that will represent ip : port of the connected phone
      if((in_array("goFonAccount"    ,$val['objectClass']))){
  
        /* Set defaults */
        $fonac = preg_replace("/%image%/", "select_phone.png", $fonimg);
        $fonac = preg_replace("/%KEY%/", "$key", $fonac);
        $fonac = preg_replace("/%title%/", "", $fonac);

        /* Database connection is ok ?*/
        if($r_db){
          $res= @mysql_query("SELECT regseconds,name,port,ipaddr FROM ".$a_SETUP['SIP_TABLE']." WHERE (name='".$val['uid'][0]."')");
          $mysql_entry = @mysql_fetch_row($res);
          if(is_array($mysql_entry)){
            if((isset($mysql_entry[0]))&&($mysql_entry[0]>1)){
              $connected = " | "._("Online")." : ".gmdate("d.m.Y H:i:s",($mysql_entry[0]+(60*60)));
              $fonac = preg_replace("/%image%/", "select_phone_connected.png", $fonimg);
              $fonac = preg_replace("/%KEY%/", "$key", $fonac);
              $fonac = preg_replace("/%title%/", $connected, $fonac);
              $ip_port= " - ".$mysql_entry[3].":".$mysql_entry[2];
            }
            if((isset($mysql_entry[0]))&&($mysql_entry[0]==0)){
              $connected = " | "._("Offline");
              $fonac = preg_replace("/%image%/", "select_phone_notconnected.png", $fonimg);
              $fonac = preg_replace("/%KEY%/", "$key", $fonac);
              $fonac = preg_replace("/%title%/", $connected, $fonac);
            }
          }
        }
      }else{
        $fonac=$empty;
      }

      // Specify Pics for Extensions  
      if(in_array("posixAccount"    ,$val['objectClass'])){ 
        $posix = preg_replace("/%KEY%/", "$key", $posiximg);
      }else{ 
        $posix=$empty;
      }
   
      if(in_array("gosaMailAccount" ,$val['objectClass'])){ 
        $maila = preg_replace("/%KEY%/", "$key", $mailimg);   
      }else{ 
        $maila=$empty;
      } 
  
      if(in_array("goFaxAccount"    ,$val['objectClass'])){ 
        $faxac = preg_replace("/%KEY%/", "$key", $faximg);    
      }else{ 
        $faxac=$empty;
      } 

      if(in_array("sambaSamAccount" ,$val['objectClass'])){ 
        $samba = preg_replace("/%KEY%/", "$key", $sambaimg);  
      }else{ 
        $samba=$empty;
      } 

      if(in_array("gosaUserTemplate",$val['objectClass'])){ 
        $tpl                          = preg_replace("/%KEY%/", "$key", $tplimg);   
        $s_img_create_from_template   = preg_replace("/%KEY%/", "$key", $tplcreateuserimg);
      }else{ 
        $s_img_create_from_template   = "";
        $tpl=$userimg;
      }

      $usrimg = preg_replace("/%KEY%/", "$key", $usrimg);

      // Generate caption for rows
      if (isset($val["sn"]) && isset($val["givenName"])){
        $display= $val["sn"][0].", ".$val["givenName"][0]." [".$val["uid"][0]."]";
      } else {
        $display= "[".$val["uid"][0]."]";
      }

      /* Create each field */
      $field1 = array("string" => sprintf($tpl,$val['dn']),"attach"=>"style='width:20px;align:middle;'");
      $field2 = array("string" => sprintf($editlink,$key,$display).$ip_port,"attach"=>"title='".$val['dn']."'");
      $field3 = array("string" => $usrimg."&nbsp;".$posix."&nbsp;".$maila."&nbsp;".$fonac."&nbsp;".$faxac."&nbsp;".$samba."&nbsp;".$s_img_create_from_template, "attach" => "style='width:138px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", "$key", $action),"attach" => "style='border:none; text-align:right;width:48px'");

      $add = array($field1,$field2,$field3,$field4);

      // Template or User
      if(in_array("gosaUserTemplate",$val['objectClass'])){
        $tpls[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }else{
        $users[strtolower( $val['sn']['0'].$val['uid']['0'])]=$add;
      }
    }

    // Sort Entries 
    ksort($users);
    ksort($tpls);

    // Append Templates
    foreach($tpls as $key => $val){
      $divlist->AddEntry( $val, array(""));
    }
    // Append Users 
    foreach($users as $key => $val){
      $divlist->AddEntry( $val, array(""));
    }
    /* Show main page */
    //$smarty->assign("userlist",   nl2br(htmlentities($divlist->DrawList())));
    $smarty->assign("userlist",   $divlist->DrawList());
    $smarty->assign("userlisthead", $listhead);

    /* END NEW LIST MANAGMENT
     */  
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    foreach( array("depselect",  "functionalusers", "unixusers",
          "mailusers", "sambausers", "proxyusers", 
          "faxusers", "templates", "subsearch") as $type){

      $smarty->assign("$type", $userfilter[$type]);
    }

    $smarty->assign("regex", $userfilter["regex"]);
    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return ($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function reload()
  {
    /* Get config */
    $userfilter= get_global('userfilter');

    /* Set base for all searches */
    $base= $userfilter['depselect'];

    /* Regex filter? */
    if ($userfilter['regex'] != ""){
      $regex= $userfilter['regex'];
    } else {
      $regex= "*";
    }

    /* Set filter depending on selection */
    if ($this->config->current['SAMBAVERSION'] == 3){
      $samba= "sambaSamAccount";
    } else {
      $samba= "sambaAccount";
    }

    $filter="";
    if ($userfilter['functionalusers'] == "checked"){
      $filter.= "(&(objectClass=gosaAccount)(!(|(objectClass=posixAccount)(objectClass=gosaMailAccount)(objectClass=$samba)(objectClass=goFaxAccount)(objectClass=gosaProxyAccount))))";
    }
    if ($userfilter['unixusers'] == "checked"){
      $filter.= "(objectClass=posixAccount)";
    }
    if ($userfilter['mailusers'] == "checked"){
      $filter.= "(objectClass=gosaMailAccount)";
    }
    if ($userfilter['sambausers'] == "checked"){
      $filter.= "(objectClass=$samba)";
    }
    if ($userfilter['proxyusers'] == "checked"){
      $filter.= "(objectClass=gosaProxyAccount)";
    }
    if ($userfilter['faxusers'] == "checked"){
      $filter.= "(objectClass=goFaxAccount)";
    }

    if ($userfilter['templates'] == "checked"){
      $filter= "(|(objectClass=gosaUserTemplate)(&(objectClass=gosaAccount)(|$filter)))";
    } else {
      $filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(|$filter))";
    }
    $filter= "(&(|(uid=$regex)(sn=$regex)(givenName=$regex))$filter)";

    /* Generate userlist */
    $ldap= $this->config->get_ldap_link(TRUE);
    if ($userfilter['subsearch'] == "checked"){
      $this->list= get_list($this->ui->subtreeACL, "$filter", TRUE, $base, array("uid", "givenName", "sn", "objectClass"), TRUE);
    } else {
      $base= get_people_ou().$base;
      $this->list= get_list($this->ui->subtreeACL, "$filter", FALSE, $base, array("uid", "givenName", "sn", "objectClass"), TRUE);
    }



    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer 
     */
    $base2 = preg_replace("/ou=people,/i","",$base);

    $res3 =  get_list2($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
        TRUE, $base2, array("ou", "description"), TRUE);

    $this->departments= array();

    $tmp = array();
    foreach ($res3 as $value){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if($value["description"][0]!=".."){
        $this->departments[$value['dn']]=convert_department_dn2($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]=$value["description"][0];
      }
    }
    /* END NEW LIST MANAGMENT 
     */


    $this->userlist= array();

    foreach ($this->list as $value){
      if (isset($value["uid"][0]) && !preg_match('/\$$/', $value["uid"][0])){
        if (in_array_ics('gosaUserTemplate', $value['objectClass'])){
          $this->userlist[$value["dn"]]= "* ".$value["uid"][0]." ("._("Template").")";
          continue;
        }
        if (isset($value["givenName"][0]) && isset($value["sn"][0])){
          $this->userlist[$value["dn"]]= $value["sn"][0].", ".
            $value["givenName"][0].
            " [".$value["uid"][0]."]";
        } else {
          $this->userlist[$value["dn"]]= "[".$value["uid"][0]."]";
        }
      }
    }

    natcasesort ($this->userlist);
    reset ($this->userlist);
  }

  function remove_from_parent()
  {
  }


  /* Check values */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
  }

  function adapt_from_template($dn)
  {
  }

  function password_change_needed()
  {
  }

  function show_header($button_text, $text, $disabled= FALSE)
  {
  }

  function remove_lock()
  {
    if (isset($this->usertab->dn)){
      del_lock ($this->usertab->dn);
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
