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

  function userManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("userfilter")){
      $base= get_base_from_people($ui->dn);
      $userfilter= array( "mailusers"       => "checked",
          "unixusers"       => "checked",
          "templates"       => "",
          "subsearch"       => "",
          "proxyusers"      => "checked",
          "sambausers"      => "checked",
          "ftpusers"        => "checked",
          "faxusers"        => "checked",
          "functionalusers" => "checked",
          "depselect"       => $base,
          "regex"           => "*");
      register_global("userfilter", $userfilter);
    }
  }

  function execute()
  {
    $smarty= get_smarty();
    $userfilter= get_global("userfilter");

    /* Save filter data if we are in the headpage */
    if (!isset($this->usertab)){
      foreach( array("depselect", "regex") as $type){
        if (isset($_POST[$type])){
          $userfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_POST['depselect'])){
        foreach( array("functionalusers", "unixusers", "mailusers",
              "sambausers", "proxyusers", "ftpusers",
              "faxusers", "templates", "subsearch") as $type){

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
    }

    /* React on user interaction here */
    if ($_SERVER["REQUEST_METHOD"] == "POST"){

      /* User wants to edit data? */
      if ( (isset($_POST['select_user']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['userlist']) && $_POST['userlist'] != ""){

        /* Get 'dn' from posted 'uid', must be unique */
        $this->dn= trim($_POST['userlist']);

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
        $this->usertab->set_acl($acl);

        $_SESSION['objectinfo']= $this->dn;
      }
    }

    /* Reset requested? */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->usertab->dn);
      unset ($this->usertab);
      $this->usertab= NULL;
      $this->lognames= array();;
      $this->sn= "";
      $this->givenName= "";
      $this->uid= "";
      unset ($_SESSION['objectinfo']);
    }

    /* Password change requested */
    if (isset($_POST['setpass_user']) && isset($_POST['userlist'])){

      /* Get 'dn' from posted 'uid' */
      $this->dn= trim($_POST['userlist']);

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

        /* Set password, perform required steps */
        if ($this->usertab){
          if ($this->usertab->password_change_needed()){
            $obj= $this->usertab->by_object['user'];
            change_password ($this->usertab->dn, $_POST['new_password'],
                0, $obj->pw_storage);
            gosa_log ("Password for '".$this->usertab->dn."' has been changed");
            unset($this->usertab);
            $this->usertab= NULL;
          }
        } else {
          change_password ($this->dn, $_POST['new_password']);
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
    if (isset($_POST['delete_user']) && isset($_POST['userlist'])){

      /* Get 'dn' from posted 'uid' */
      $this->dn= trim($_POST['userlist']);

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
    if (isset($_POST['new_user'])){
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
    if (isset($_POST['new_user']) || isset($_POST['new_template'])){

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
      if (isset($_POST['new_template'])){
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
        if (isset($_POST['template'])){
          $smarty->assign("template", $_POST['template']);
        }
        return($smarty->fetch(get_template_path('template.tpl', TRUE)));
      }

    }

    /* Continue template editing */
    if (isset($_POST['template_continue']) && $_POST['template'] != 'none' && !isset($_POST['uid'])){
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
      $this->sn= $_POST['sn'];
      $this->givenName= $_POST['givenName'];

      /* Move user supplied data to sub plugins */
      $this->uid= $_POST['uid'];
      $this->usertab->uid= $this->uid;
      $this->usertab->sn= $this->sn;
      $this->usertab->givenName= $this->givenName;
      $template_dn= $_POST['template'];
      $this->usertab->adapt_from_template($template_dn);
      $template_base= preg_replace("/^[^,]+,".get_people_ou()."/", '', $template_dn);
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

    /* Show main page */
    $smarty->assign("userlist", $this->userlist);
    }
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    foreach( array("depselect", "regex", "functionalusers", "unixusers",
          "mailusers", "sambausers", "proxyusers", "ftpusers",
          "faxusers", "templates", "subsearch") as $type){

      $smarty->assign("$type", $userfilter[$type]);
    }

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
      $filter.= "(&(objectClass=gosaAccount)(!(|(objectClass=posixAccount)(objectClass=gosaMailAccount)(objectClass=PureFTPdUser)(objectClass=$samba)(objectClass=goFaxAccount)(objectClass=gosaProxyAccount))))";
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
    if ($userfilter['ftpusers'] == "checked"){
      $filter.= "(objectClass=PureFTPdUser)";
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
