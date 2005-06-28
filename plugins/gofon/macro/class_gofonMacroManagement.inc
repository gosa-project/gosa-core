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
require "tabs_macros.inc";

class gofonMacro extends plugin
{
  /* Definitions */
  var $plHeadline		= "Phone macros";
  var $plDescription	= "This does something";

  /* Dialog attributes */
  var $macrotabs		= NULL;
  var $macros		= array();
  var $ui				= NULL;
  var $acl				= "";

  function gofonMacro($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("macrofilter")){
      $base= get_base_from_people($ui->dn);
      $macrofilter= array("depselect" => $base,  "regex" => "*");
      register_global("macrofilter", $macrofilter);
    }
  }

  function execute()
  {
    /* Save data */
    $macrofilter= get_global("macrofilter");
    foreach( array("depselect", "regex") as $type){
      if (isset($_POST[$type])){
        $macrofilter[$type]= $_POST[$type];
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $macrofilter['regex']= $s;
    }
    /* Save (new) Filter */
    register_global("macrofilter", $macrofilter);

    /* Check sorting variable */
    if (!isset($this->macrotabs) &&
        !isset($_POST['new_app']) &&
        !isset($_POST['delete_app']) &&
        !isset($_POST['select_app'])){
      $this->reload();
    }
  
    
    $smarty= get_smarty();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* New macro? */
    if (isset($_POST['new_app'])){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn);
      $this->macrotabs->set_acl(array(':all'));
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->macrotabs->dn);
      unset ($this->macrotabs);
      $this->macrotabs= NULL;
      unset ($_SESSION['objectinfo']);
    }

    /* Finish apps edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if (isset($_POST['edit_finish'])){

      /* Check tabs, will feed message array */
      $this->macrotabs->last= $this->macrotabs->current;
      $this->macrotabs->save_object();
      $message= $this->macrotabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->macrotabs->save();
        gosa_log ("Macro object'".$this->dn."' has been saved");

        /* macro has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new apps at
           this point. */
        $this->reload ();
        unset ($this->macrotabs);
        $this->macrotabs= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* User wants to edit data? */
    if ((isset($_POST['select_app']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['macrolists']) && $_POST['macrolists'] != ""){

      /* Get 'dn' from posted 'macrolists', must be unique */
      $this->dn= trim($_POST['macrolists']);

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

      /* Register macrotabs to trigger edit dialog */
      $this->macrotabs= new macrotabs($this->config,
          $this->config->data['TABS']['MACROTABS'], $this->dn);
      $this->macrotabs->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Remove user was requested */
    if (isset($_POST['delete_app']) && isset($_POST['macrolists'])){

      /* Get 'dn' from posted 'uid' */
      $this->dn= trim($_POST['macrolists']);

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "macro", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the macro '%s'."), $this->dn));
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this macro!"));
      }
    }

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_app_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){
      
        /* Delete request is permitted, perform LDAP action */
        $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn);
        $this->macrotabs->set_acl(array($this->acl));
      
        $this->macrotabs->delete ();
        gosa_log ("macro object'".$this->dn."' has been removed");
        unset ($this->macrotabs);
        $this->macrotabs= NULL;

        /* Group list has changed, reload it. */
        $this->reload ();
      } else {
        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this macro!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }
      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /* Delete macro canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if ($this->macrotabs){
      $display= $this->macrotabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->macrotabs->by_object[$this->macrotabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Show main page */
    $smarty->assign("macros", $this->macros);
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    foreach( array("depselect", "regex") as $type){
      $smarty->assign("$type", $macrofilter[$type]);
    }

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function reload()
  {
    /* Get config */
    $macrofilter= get_global('macrofilter');

    /* Set base for all searches */
    $base= $macrofilter['depselect'];

    /* Regex filter? */
    if ($macrofilter['regex'] != ""){
      $regex= $macrofilter['regex'];
    } else {
      $regex= "*";
    }

    /* Generate macro list */
    $res= get_list($this->ui->subtreeACL, "(&(cn=$regex)(objectClass=goFonMacro))", TRUE, $base, array("cn", "description"), TRUE);
    $this->macros= array();
    foreach ($res as $value){
      $this->macros[$value["dn"]]= $value["cn"][0];
      if (isset($value["description"][0])){
        $this->macros[$value["dn"]]= $value["cn"][0]." (".
          $value["description"][0].")";
      }
    }
    natcasesort ($this->macros);
    reset ($this->macros);
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
    /* Optionally execute a command after we're done */
    $this->postcreate();
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
    if (isset($this->macrotabs->dn)){
      del_lock ($this->macrotabs->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
