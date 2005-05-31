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
require "tabs_terminal.inc";
require "tabs_workstation.inc";
require "tabs_printers.inc";
require "tabs_phone.inc";
require "tabs_server.inc";
require "tabs_component.inc";
require "tabs_winstation.inc";


class systems extends plugin
{
  /* Definitions */
  var $plHeadline= "Systems";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $systab= NULL;
  var $terminals= array();
  var $ui= NULL;
  var $acl= "";

  function systems ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("terminalfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $terminalfilter= array("workstations" => "checked",
          "thins" => "checked",
          "winstations" => "checked",
          "servers" => "checked",
          "printers" => "checked",
          "phones" => "checked",
          "netdev" => "checked",
          "user" => "*",
          "depselect" => $base,
          "regex" => "*");
      register_global("terminalfilter", $terminalfilter);
    }
  }

  function execute()
  {
    /* Save data */
    $terminalfilter= get_global("terminalfilter");
    foreach( array("depselect", "user", "regex") as $type){
      if (isset($_POST[$type])){
        $terminalfilter[$type]= $_POST[$type];
      }
    }
    if (isset($_POST['depselect'])){
      foreach( array("workstations", "thins", "winstations", "printers", "phones", "servers", "netdev") as $type){

        if (isset($_POST[$type])) {
          $terminalfilter[$type]= "checked";
        } else {
          $terminalfilter[$type]= "";
        }
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $terminalfilter['regex']= $s;
    }
    register_global("terminalfilter", $terminalfilter);

    /* Check sorting variable */
    if (!isset($this->systab) &&
        !isset($_POST['new_terminal']) &&
        !isset($_POST['delete_terminal']) &&
        !isset($_POST['set_password']) &&
        !isset($_POST['select_terminal'])){
      $this->reload();
    }
    $smarty= get_smarty();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Try to get informations about what kind of system to create */
    if (isset($_POST['new_terminal'])) {
      return ($smarty->fetch(get_template_path('chooser.tpl', TRUE)));
    }

    /* Create new default terminal */
    if (isset($_POST['create_system'])) {
      $this->dn= "new";
      $this->acl= array(":all");

      switch ($_POST['system']){
        case 'terminal':
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['termgeneric']->cn= "default";
          $this->systab->by_object['termservice']->cn= "default";
          break;

        case 'workstation':
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['workgeneric']->cn= "wdefault";
          $this->systab->by_object['termservice']->cn= "default";
          break;

        case 'server':
          $this->systab= new servtabs($this->config,
              $this->config->data['TABS']['SERVTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'printer':
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'phone':
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;

        case 'component':
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          break;
      }
    }

    /* User wants to edit data? */
    if ((isset($_POST['select_terminal']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['termlist']) &&
          $_POST['termlist'] != ""){

      /* Get 'dn' from posted 'cn', must be unique */
      $this->dn= trim($_POST['termlist']);

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Find out more about the object type */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn);
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs['objectClass']);

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);

      switch ($type){
        case "terminal":
          /* Register systab to trigger edit dialog */
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "server":
          /* Register systab to trigger edit dialog */
          $this->systab= new servtabs($this->config,
              $this->config->data['TABS']['SERVTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "workstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "printer":
          /* Register systab to trigger edit dialog */
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "phone":
          /* Register systab to trigger edit dialog */
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;

        case "component":
          /* Register systab to trigger edit dialog */
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;
       
        case "winstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new wintabs($this->config,
              $this->config->data['TABS']['WINTABS'], $this->dn);
          $this->systab->set_acl($acl);
          $_SESSION['objectinfo']= $this->dn;
          break;


        default:
          print_red (_("You can't edit this object type yet!"));
          del_lock($this->dn);
          break;
      }
    }

    /* Set terminals root password */
    if (isset($_POST['set_password']) && isset($_POST['termlist'])){
      $this->dn= trim($_POST['termlist']);
      $_SESSION['objectinfo']= $this->dn;
      return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
    }

    /* Correctly specified? */
    if (isset($_POST['password_finish'])){
      if ($_POST['new_password'] != $_POST['repeated_password']){
        print_red (_("Passwords entered as new and repeated do not match!"));
        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }
    }

    /* Change terminal password */
    if (isset($_POST['password_finish']) && 
        $_POST['new_password'] == $_POST['repeated_password']){

      /* Check if user is allowed to set password */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($acl, "password") != ""){
        print_red (_("You are not allowed to set this systems password!"));
      } else {
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);

        $attrs= array();
        if ($_POST['new_password'] == ""){
          $attrs['gotoRootPasswd']= array();
        } else {
          $attrs['gotoRootPasswd']= crypt($_POST['new_password'],
              substr(session_id(),0,2));
        }
        $ldap->modify($attrs);
        gosa_log ("Password for '".$this->dn."' has been changed");
      }
      unset($_SESSION['objectinfo']);
    }

    /* Delete terminal canceled? */
    if (isset($_POST['delete_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Remove terminal was requested */
    if (isset($_POST['delete_terminal']) && isset($_POST['termlist'])){

      /* Get 'dn' from posted termlinst */
      $this->dn= $_POST['termlist'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this component!"));
      }
    }

    /* Confirmation for deletion has been passed. Terminal should be deleted. */
    if (isset($_POST['delete_terminal_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Find out more about the object type */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn);
        $attrs= $ldap->fetch();
        $type= $this->get_system_type($attrs['objectClass']);

        switch ($type){
          case "terminal":
            $tabtype= "termtabs";
          break;

          case "workstation":
            $tabtype= "worktabs";
          break;

          case "server":
            $tabtype= "servtabs";
          break;

          default:
#print_red (_("You can't remove this object type yet!"));
#del_lock($this->dn);
#return;
          $tabtype= "termtabs";
          break;
        }

        /* Delete request is permitted, perform LDAP action */
        $this->systab= new $tabtype($this->config,
            $this->config->data['TABS']['TERMTABS'], $this->dn);
        $this->systab->set_acl(array($this->acl));
        $this->systab->by_object['termgeneric']->remove_from_parent ();
        unset ($this->systab);
        gosa_log ("System object'".$this->dn."' has been removed");
        $this->systab= NULL;

        /* Terminal list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this component!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick system ".
            "deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }

    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if (isset($_POST['edit_finish'])){

      /* Check tabs, will feed message array */
      $message= $this->systab->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save terminal data to ldap */
        gosa_log ("System object'".$this->dn."' has been saved");
        $this->systab->save();

        /* Terminal has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new terminals at
           this point. */
        $this->reload ();
        unset ($this->systab);
        $this->systab= NULL;
        unset($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }

    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      if (isset($this->systab)){
        del_lock ($this->systab->dn);
        unset ($this->systab);
      }
      $this->systab= NULL;
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if ($this->systab){
      $display= $this->systab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->systab->by_object[$this->systab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Show main page */
    $smarty->assign("terminals", $this->convert_list($this->terminals));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    foreach( array("depselect", "user", "regex", "workstations", "thins", "servers",
          "winstations", "printers", "phones", "netdev") as $type){

      $smarty->assign("$type", $terminalfilter[$type]);
    }
    $smarty->assign("deplist", $this->config->idepartments);

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(	"D" => "select_default.png",
        "T" => "select_terminal.png",
        "L" => "select_workstation.png",
        "F" => "select_phone.png",
        "S" => "select_server.png",
        "NT" => "select_new_terminal.png",
        "NL" => "select_new_workstation.png",
        "W" => "select_winstation.png",
        "C" => "select_component.png",
        "P" => "select_printer.png");

    foreach ($input as $key => $value){
      $mode= preg_replace('/^.*(.)$/', '\\1', $value);
      if ($mode == "D"){
        $temp= "<option value=\"$key\" class=\"select\" style=\"background-image:url('".get_template_path('images/select_default.png')."');\">".preg_replace('/.$/', '', $value)."</option>\n".$temp;
      } else {
        if (preg_match('/^\+/', $value)){
          $value= preg_replace('/^\+ /', '', $value);
          $new= "N";
        } else {
          $new= "";
        }
        $temp.= "<option value=\"$key\" class=\"select\" style=\"background-image:url('".get_template_path('images/'.$conv["$new$mode"])."');\">".preg_replace('/.$/', '', $value)."</option>\n";
      }
    }

    return ($temp);
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
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

  function reload()
  {
    /* Load terminal shortcuts */
    $responsible= array();
    foreach ($this->config->departments as $key => $value){
      if (get_module_permission(get_permissions ($value, $this->ui->subtreeACL),
            "terminal", $value) == "#all#"){
        $responsible[$key]= $value;
      }
    }

    /* Get config */
    $terminalfilter= get_global('terminalfilter');
    $filter= "";

    /* Set base for all searches */
    $base= $terminalfilter['depselect'];

    /* Regex filter? */
    if ($terminalfilter['regex'] != ""){
      $regex= $terminalfilter['regex'];
    } else {
      $regex= "*";
    }

    /* Get list of terminals to be shown */
    if ($terminalfilter['thins'] == "checked"){
      $termfilter= "(&(objectClass=gotoTerminal)(cn=$regex))";
    } else {
      $termfilter= "";
    }
    if ($terminalfilter['workstations'] == "checked"){
      $workfilter= "(&(objectClass=gotoWorkstation)(cn=$regex))";
    } else {
      $workfilter= "";
    }
    if ($terminalfilter['winstations'] == "checked"){
      if ($this->config->current['SAMBAVERSION'] == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
      $winfilter= "(&(objectClass=$samba)(cn=$regex\$))";
    } else {
      $winfilter= "";
    }
    if ($terminalfilter['printers'] == "checked"){
      $printfilter= "(&(objectClass=gotoPrinter)(cn=$regex))";
    } else {
      $printfilter= "";
    }
    if ($terminalfilter['phones'] == "checked"){
      $phonefilter= "(&(objectClass=goFonHardware)(cn=$regex))";
    } else {
      $phonefilter= "";
    }
    if ($terminalfilter['netdev'] == "checked"){
      $netfilter= "(&(objectClass=ieee802Device)(cn=$regex))";
    } else {
      $netfilter= "";
    }
    if ($terminalfilter['servers'] == "checked"){
      $serverfilter= "(&(objectClass=goServer)(cn=$regex))";
    } else {
      $serverfilter= "";
    }

    /* User filter? */
    if ($terminalfilter['user'] != "" && $terminalfilter['user'] != "*"){
      $filter.= "(gotoLastUser=".$terminalfilter['user'].")";
    }

    /* Get all gotoTerminal's */
    $this->terminals= array();

    $res= get_list($this->ui->subtreeACL, "(|$termfilter$workfilter$winfilter$printfilter$phonefilter$serverfilter$netfilter)", TRUE, $base, array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), TRUE);
    foreach ($res as $value){

      /* Look for new terminals and mark them with '+' */
      $tmp= "";
      if (preg_match('/,ou=terminals,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=terminals,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=workstations,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=workstations,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=servers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=servers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,'.get_winstations_ou().'/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*".get_winstations_ou()."/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=printers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=printers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=phones,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=phones,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=netdevices,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=netdevices,ou=systems,/i", "", $value["dn"]);
      }
      if ($tmp == ""){
        $tmp= $value["dn"];
      }
      if (preg_match ("/,ou=incoming,/i", $tmp)){
        if (in_array('gotoTerminal', $value['objectClass'])){
          $add= "+ "._("New terminal");
        }
        if (in_array('gotoWorkstation', $value['objectClass'])){
          $add= "+ "._("New workstation");
        }
      } else {
        $add= "";
      }

      if (in_array ($tmp, $responsible) || $add != ""){
        if (in_array('gotoTerminal', $value["objectClass"])){
          if (isset($value["macAddress"][0]) &&
              $value["macAddress"][0] != "-"){

            $this->terminals[$value["dn"]]= $add." ".
              $value["cn"][0].
              " [".$value["macAddress"][0]."]T";
          } else {
            $location= array_search($tmp, $this->config->departments);
            $this->terminals[$value["dn"]]= _("Thin client template for").
              " $location"."D";
          }
        } elseif (in_array('gotoWorkstation', $value["objectClass"])){
          if (isset($value["macAddress"][0]) &&
              $value["macAddress"][0] != "-"){

            $this->terminals[$value["dn"]]= $add." ".
              $value["cn"][0].
              " [".$value["macAddress"][0]."]L";
          } else {
            $location= array_search($tmp, $this->config->departments);
            $this->terminals[$value["dn"]]= _("Workstation template for").
              " $location"."D";
          }
        } elseif (in_array('gotoPrinter', $value["objectClass"])){
          if (isset($value['description'][0])){
            $this->terminals[$value['dn']]= $value['cn'][0]." [".$value['description'][0]."]P";
          } else {
            $this->terminals[$value['dn']]= $value['cn'][0]."P";
          }
        } elseif (in_array('goServer', $value["objectClass"])){
          if (isset($value['description'][0])){
            $this->terminals[$value['dn']]= $value['cn'][0]." [".$value['description'][0]."]S";
          } else {
            $this->terminals[$value['dn']]= $value['cn'][0]."S";
          }
        } elseif (in_array('goFonHardware', $value["objectClass"])){

          if (isset($value['description'][0])){
            $this->terminals[$value['dn']]= $value['cn'][0]." [".$value['description'][0]."]F";
          } else {
            $this->terminals[$value['dn']]= $value['cn'][0]."F";
          }
        } elseif (in_array('ieee802Device', $value["objectClass"])){

          if (isset($value['description'][0])){
            $this->terminals[$value['dn']]= $value['cn'][0]." [".$value['description'][0]."]C";
          } else {
            $this->terminals[$value['dn']]= $value['cn'][0]."C";
          }
        } else{
          $name= preg_replace('/\$$/', '', $value['cn'][0]);
          if (isset($value['sambaDomainName'])){
            $domain= " [".$value['sambaDomainName'][0]."]";
          } else {
            $domain= "";
          }
          $this->terminals[$value['dn']]= $name.$domain."W";
        }
      }
    }
    natcasesort ($this->terminals);
    reset ($this->terminals);
  }

  function remove_lock()
  {
    if (isset($this->systab->dn)){
      del_lock ($this->systab->dn);
    }
  }


  function get_system_type($classes)
  {
    $type= "";

    if (in_array('ieee802Device', $classes)){
      $type= "component";
    }
    if (in_array('gotoTerminal', $classes)){
      $type= "terminal";
    }
    if (in_array('gotoWorkstation', $classes)){
      $type= "workstation";
    }
    if (in_array('gotoPrinter', $classes)){
      $type= "printer";
    }
    if (in_array('goFonHardware', $classes)){
      $type= "phone";
    }
    if (in_array('goServer', $classes)){
      $type= "server";
    }
    if (in_array('sambaAccount', $classes) ||
        in_array('sambaSamAccount', $classes)){
      $type= "winstation";
    }

    return ($type);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
