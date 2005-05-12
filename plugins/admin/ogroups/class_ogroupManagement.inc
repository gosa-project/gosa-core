<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004 Cajus Pollmeier

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
require "tabs_ogroups.inc";

class ogroupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Object groups";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $obtypes= array();
  var $ogroup;

  function ogroupManagement ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;
    $this->ui= get_userinfo();

    /* Fill translation array */
    $this->obtypes= array(	"posixAccount" => _("UNIX accounts"),
        "posixGroup" => _("Groups"),
        "gosaDepartment" => _("Departments"),
        "gosaApplication" => _("Applications"),
        "goServer" => _("Servers"),
        "gotoTerminal" => _("Thin Clients"),
        "gotoWorkstation" => _("Workstations"),
        "goFonHardware" => _("Phone"),
        "gotoPrinter" => _("Printer"));
    asort($this->obtypes);

    /* Get global filter config */
    if (!is_global("ogroupfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $ogroupfilter= array(	"usergroups" => "checked",
          "groupgroups" => "checked",
          "appgroups" => "checked",
          "depgroups" => "checked",
          "servergroups" => "checked",
          "wsgroups" => "checked",
          "prtgroups" => "checked",
          "emptygroups" => "checked",
          "tcgroups" => "checked",
          "fongroups" => "checked",
          "depselect" => $base,
          "regex" => "*");
      register_global("ogroupfilter", $ogroupfilter);
    }
  }

  function execute()
  {
    /* Prepare template */
    $smarty= get_smarty();

    /* Save data */
    if (is_global("ogroupfilter")){
      $ogroupfilter= get_global("ogroupfilter");
    } else {
      $ogroupfilter= array();
    }
    foreach( array("depselect", "regex") as $type){
      if (isset($_POST[$type])){
        $ogroupfilter[$type]= $_POST[$type];
      }
    }
    if (isset($_POST['depselect'])){
      foreach( array("usergroups", "groupgroups", "appgroups", "depgroups", "emptygroups",
            "servergroups", "wsgroups", "prtgroups", "tcgroups", "fongroups") as $type){

        if (isset($_POST[$type])) {
          $ogroupfilter[$type]= "checked";
        } else {
          $ogroupfilter[$type]= "";
        }
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $ogroupfilter['regex']= $s;
    }
    register_global("ogroupfilter", $ogroupfilter);

    /* Prepare formular */
    if (!isset($this->ogroup) &&
        !isset($_POST['new_group']) &&
        !isset($_POST['delete_group']) &&
        !isset($_POST['select_group'])){
      $this->reload();
    }

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* New group? */
    if (isset($_POST['new_group'])){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->ogroup= new ogrouptabs($this->config,
          $this->config->data['TABS']['OGROUPTABS'], $this->dn);
      $this->ogroup->set_acl(array(':all'));
    }

    if (isset($_POST['delete_group']) && isset($_POST['grouplist'])){

      /* Get 'dn' from posted 'uid' */
      $this->dn= trim($_POST['grouplist']);

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "ogroup", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the object group '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this object group!"));
      }
    }

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->ogroup= new ogrouptabs($this->config,
            $this->config->data['TABS']['OGROUPTABS'], $this->dn);
        $this->ogroup->set_acl(array($this->acl));
        $this->ogroup->delete ();
        gosa_log ("Object group'".$this->dn."' has been removed");
        unset ($this->ogroup);
        $this->ogroup= NULL;

        /* Group list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this object group!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    if ((isset($_POST['select_group']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['grouplist']) && $_POST['grouplist'] != ""){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= trim($_POST['grouplist']);

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

      /* Register grouptab to trigger edit dialog */
      $this->ogroup= new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'],
          $this->dn);
      $this->ogroup->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Finish button has been pressed */
    if (isset($_POST['edit_finish'])){

      /* Check tabs, will feed message array */
      $message= $this->ogroup->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->ogroup->save();
        gosa_log ("Object group'".$this->dn."' has been saved");

        /* Group has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new users at
           this point. */
        $this->reload ();
        unset ($this->ogroup);
        $this->ogroup= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel'])){
      del_lock ($this->ogroup->dn);
      unset ($this->ogroup);
      $this->ogroup= NULL;
      unset($_SESSION['objectinfo']);
    }

    /* Show dialog if object is present */
    if ($this->ogroup){
      $display= $this->ogroup->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->ogroup->by_object[$this->ogroup->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Show main page */
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);

    $smarty->assign("groups", $this->convert_list($this->grouplist));

    foreach( array("usergroups", "groupgroups", "appgroups", "depgroups", "emptygroups",
          "servergroups", "wsgroups", "prtgroups", "tcgroups", "fongroups", "regex", "depselect") as $type){
      $smarty->assign("$type", $ogroupfilter[$type]);
    }

    $smarty->assign("hint", print_sizelimit_warning());
    $smarty->assign("apply", apply_filter());
    return ($smarty->fetch (get_template_path('headpage.tpl', TRUE)));
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(   "U" => "select_user.png",
        "G" => "select_groups.png",
        "A" => "select_application.png",
        "D" => "select_department.png",
        "S" => "select_server.png",
        "F" => "select_phone.png",
        "W" => "select_workstation.png",
        "T" => "select_terminal.png",
        "P" => "select_printer.png");

    foreach ($input as $key => $value){

      /* Assemble picture */
      $type= $value['type'];
      if (isset($type[0])){
        $p1= $conv[$type[0]];
      } else {
        $p1= "empty.png";
      }
      if (isset($type[1])){
        $p2= $conv[$type[1]];
      } else {
        $p2= "empty.png";
      }

      /* Generate output */
      $temp= "<option value=\"$key\" class=\"select\" style=\"background-image:url('combine.php?image1=$p1&image2=$p2');padding-left:35px;\">".$value['text']."</option>\n".$temp;
    }

    return ($temp);
  }


  function reload()
  {
    /* Get config */
    $ogroupfilter= get_global('ogroupfilter');

    /* Set base for all searches */
    $base= $ogroupfilter['depselect'];

    /* Regex filter? */
    if ($ogroupfilter['regex'] != ""){
      $regex= $ogroupfilter['regex'];
    } else {
      $regex= "*";
    }

    /* User filter? */
    $this->grouplist= array();

    /* Generate grouplist filter */
    $filter= "";

    if ($ogroupfilter['usergroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*U*)";
    }
    if ($ogroupfilter['groupgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*G*)";
    }
    if ($ogroupfilter['appgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*A*)";
    }
    if ($ogroupfilter['depgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*D*)";
    }
    if ($ogroupfilter['servergroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*S*)";
    }
    if ($ogroupfilter['wsgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*W*)";
    }
    if ($ogroupfilter['prtgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*P*)";
    }
    if ($ogroupfilter['tcgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*T*)";
    }
    if ($ogroupfilter['fongroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*F*)";
    }
    if ($ogroupfilter['emptygroups'] == "checked"){
      $filter.= "(gosaGroupObjects=[])";
    }

    if ($filter != ""){
      $filter= "(&(cn=$regex)(objectClass=gosaGroupOfNames)(|$filter))";
    } else {
      $filter= "(&(cn=$regex)(objectClass=gosaGroupOfNames))";
    }
    $res= get_list($this->ui->subtreeACL, "$filter", TRUE, $base, array("cn", "description", "gosaGroupObjects"), TRUE);
    foreach ($res as $value){
      $pictype= preg_replace('/[\[\]]/', '', $value["gosaGroupObjects"][0]);
      $sortstring= preg_split('//', $pictype, -1, PREG_SPLIT_NO_EMPTY);
      sort($sortstring);
      $pictype= implode("", $sortstring);
      if (isset($value["description"][0])){
        $this->grouplist[$value["dn"]]= array("text" => $value["cn"][0]." [".
            $value["description"][0]."]", "type" => $pictype);
      } else {
        $this->grouplist[$value["dn"]]= array("text" => $value["cn"][0],
            "type" => $pictype);
      }
    }
    ksort ($this->grouplist);
    reset ($this->grouplist);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
