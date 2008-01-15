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
  var $plHeadline		    = "Phone macros";
  var $plDescription	  = "This does something";

  /* Dialog attributes */
  var $macrotabs		    = NULL;
  var $macros		        = array();
  var $ui				        = NULL;
  var $acl				      = "";
  var $DivListMacro     = NULL;

  function gofonMacro($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goFonMacro", $ui->dn);

    $this->DivListMacro = new divListMacro($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /*****************
      Variable initialisation
     *****************/

    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty= get_smarty();


    /*****************
      Check Posts 
     *****************/

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
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/user_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_chgpw_/i","",$key);
      }
    }
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /*****************
      Create a new Macro 
     *****************/

    /* New macro? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn);
    }


    /*****************
      Edit || Password canceled  
     *****************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['delete_cancel'])){
      del_lock ($this->macrotabs->dn);
      unset ($this->macrotabs);
      $this->macrotabs= NULL;
      unset ($_SESSION['objectinfo']);
    }


    /*****************
      Edit finised
     *****************/

    /* Finish mac edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->macrotabs->config))){

      /* Check tabs, will feed message array */
      $this->macrotabs->last= $this->macrotabs->current;
      $this->macrotabs->save_object();
      $message= $this->macrotabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->macrotabs->save();
        gosa_log ("Macro object'".$this->dn."' has been saved");

        if (!isset($_POST['edit_apply'])){
          /* macro has been saved successfully, remove lock from
             LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          unset ($this->macrotabs);
          $this->macrotabs= NULL;
          unset ($_SESSION['objectinfo']);
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /*****************
      Edit macro 
     *****************/

    /* User wants to edit data */
    if (($s_action=="edit") && (!isset($this->macrotabs->config))){

      /* Get 'dn' from posted 'macrolists', must be unique */
      $this->dn= $this->macros[$s_entry]['dn']; 

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);
      /* Register macrotabs to trigger edit dialog */
      $this->macrotabs= new macrotabs($this->config,
          $this->config->data['TABS']['MACROTABS'], $this->dn);
      $_SESSION['objectinfo']= $this->dn;
    }


    /*****************
      Remove marco
     *****************/

    /* Remove user was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->macros[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "goFonMacro", $this->dn);
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


    /*****************
      Remove macro
     *****************/

    /* Confirmation for deletion has been passed. Macro should be deleted. */
    if (isset($_POST['delete_macro_confirm'])){

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
      } else {
        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this macro!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }
      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /*****************
      Display open dialogs  
     *****************/

    /* Show tab dialog if object is present */
    if (($this->macrotabs) && (isset($this->macrotabs->config))){
      $display= $this->macrotabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->macrotabs->by_object[$this->macrotabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
        $display.= "&nbsp;\n";
        if ($this->dn != "new"){
          $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /*****************
      Display entry list 
     *****************/

    /* Return rendered main page */
    /* Display dialog with system list */
    $this->DivListMacro->parent = $this;
    $this->DivListMacro->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListMacro->SubSearch){
      $this->DivListMacro->AddDepartments($this->DivListMacro->selectedBase,4);
    }
    $this->reload();
    $this->DivListMacro->setEntries($this->macros);
    return($this->DivListMacro->Draw());
  }


  function reload()
  {
    /* Set base for all searches */
    $base       = $this->DivListMacro->selectedBase;
    $SubSearch  = $this->DivListMacro->SubSearch;
    $Regex      = $this->DivListMacro->Regex;
    $Filter     = "(&(cn=".$Regex.")(objectClass=goFonMacro))";
    $Flags      = GL_SIZELIMIT;
    $Attrs      = array("cn","description","displayName","goFonMacroVisible");

    /* Prepare for ls or search*/        
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }else{
      $base = "ou=macros,ou=asterisk,ou=configs,ou=systems,".$base;
    }

    /* Generate macro list */
    $res= get_list($Filter, $this->ui->subtreeACL,$base, $Attrs, $Flags);

    $tmp=array();
    foreach($res as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->macros=array();
    foreach($tmp as $val){
      $this->macros[]=$val;
    }

    reset ($this->macros);
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListMacro->save_object();
  }


  function adapt_from_template($dn)  { }
  function check() { }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
