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
require "tabs_application.inc";

class applicationManagement extends plugin
{
  /* Definitions */
  var $plHeadline     = "Applications";
  var $plDescription  = "This does something";

  /* Dialog attributes */
  var $apptabs                  = NULL;
  var $ui                       = NULL;
  var $CopyPasteHandler         = NULL;
  var $DivListApplication       = NULL;
  var $applications             = array();
  var $acl                      = "";
  var $enableReleaseManagement  = false;

  function IsReleaseManagementActivated()
  {
    /* Check if we should enable the release selection */
    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if(!empty($tmp)){
      return(true);
    }
    return(false);
  }

  function applicationManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config   = $config;
    $this->ui       = $ui;

    /* Check if copy & paste is activated */
    if($this->config->boolValueIsTrue("MAIN","ENABLECOPYPASTE")){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Creat dialog object */
    $this->DivListApplication = new divListApplication($this->config,$this);

    if($this->IsReleaseManagementActivated()){
    /* Check if we should enable the release selection */
      $this->enableReleaseManagement = true;

      /* Hide SubSearch checkbox */
      $this->DivListApplication->DisableCheckBox("SubSearch");
    }
  }

  function getReleases($base)
  {
    $ldap                   = $this->config->get_ldap_link();
    $dn                     = "ou=apps,".$base;
    $ret                    = array();
    $ret [$dn] = "/";

    $ldap->cd($dn);
    $ldap->search("objectClass=organizationalUnit",array("ou"));

    while($attrs = $ldap->fetch()){
      $str = str_replace($dn,"",$attrs['dn']);
      $tmp = array_reverse( split("ou=",$str));
      $str = "";
      foreach($tmp as $val){
        $val = trim(preg_replace("/,/","",$val));
        if(empty($val)) break;
        $str .= "/".$val;
      } 
      if(!empty($str)){
        $ret[$attrs['dn']]= preg_replace("/^\//","",$str);
      }
    }
    asort($ret);
    return($ret);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();


    /**************** 
      Variable init 
     ****************/

    /* These vars will be stored if you try to open a locked app, 
        to be able to perform your last requests after showing a warning message */
    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^appl_edit_/","/^appl_del_/");

    $smarty       = get_smarty();             // Smarty instance
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    
    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/appl_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/appl_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/appl_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/appl_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
        // Post for new
      }elseif(preg_match("/^appl_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /**************** 
      Copy & Paste handling  
     ****************/

    /* Only perform copy / paste if it is enabled 
     */
    if($this->CopyPasteHandler){
      if($str = $this->copyPasteHandling($s_action,$s_entry)){
       return($str);
      };
    }


    /**************** 
      Create a new app 
     ****************/

    /* New application? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->apptabs= new apptabs($this->config,
          $this->config->data['TABS']['APPSTABS'], $this->dn);
      $this->apptabs->set_acl(array(':all'));
    }


    /**************** 
      Edit entry canceled 
     ****************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->apptabs->dn);
      unset ($this->apptabs);
      $this->apptabs= NULL;
      unset ($_SESSION['objectinfo']);
    }


    /**************** 
      Edit entry finished 
     ****************/

    /* Finish apps edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->apptabs->config))){

      /* Check tabs, will feed message array */
      $this->apptabs->last= $this->apptabs->current;
      $this->apptabs->save_object();
      $message= $this->apptabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->apptabs->set_release($this->DivListApplication->selectedRelease);
        $this->apptabs->save();
        gosa_log ("Application object'".$this->dn."' has been saved");

        /* Application has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }
        unset ($this->apptabs);
        $this->apptabs= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /**************** 
      Edit entry  
     ****************/

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->apptabs->config))){

      /* Get 'dn' from posted 'applist', must be unique */
      $this->dn= $this->applications[$s_entry]['dn'];

      /* Get faistate to check if this applications is frezzed. 
         Freezed applications can't be modified anyway */
      $state = "";
      if(isset($this->applications[$s_entry]['FAIstate'])){
        $state = $this->applications[$s_entry]['FAIstate'][0];
      }

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

      /* Register apptabs to trigger edit dialog */
      $this->apptabs= new apptabs($this->config,$this->config->data['TABS']['APPSTABS'], $this->dn);

      /* If this is a freeze assign acl none  */
      if(preg_match("/freeze/",$state)){
        $this->apptabs->set_acl(array());
      }else{
        $this->apptabs->set_acl($acl);
      }
      $_SESSION['objectinfo']= $this->dn;
    }


    /**************** 
      Delete app 
     ****************/

    /* Remove user was requested */
    if ($s_action == "del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->applications[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "application", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the application '%s'."), @LDAP::fix($this->dn)));
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this application!"));
      }
    }


    /**************** 
      Delete app confirmed 
     ****************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_app_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->apptabs= new apptabs($this->config,
            $this->config->data['TABS']['APPSTABS'], $this->dn);
        $this->apptabs->set_acl(array($this->acl));
        $this->apptabs->delete ();
        gosa_log ("Application object'".$this->dn."' has been removed");
        unset ($this->apptabs);
        $this->apptabs= NULL;

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this application!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /**************** 
      Delete app canceled 
     ****************/

    /* Delete application canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if (($this->apptabs) && (isset($this->apptabs->config))){
      $display= $this->apptabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->apptabs->by_object[$this->apptabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /****************
      Dialog display
     ****************/

    /* Display dialog with system list */
    $this->DivListApplication->parent = $this;
    $this->DivListApplication->execute();
    $this->DivListApplication->AddDepartments($this->DivListApplication->selectedBase);
    $this->reload();
    $this->DivListApplication->setEntries($this->applications);
    return($this->DivListApplication->Draw());
  }


  function reload()
  {
    $this->applications= array();

    /* Set base for all searches */
    $base       = $this->DivListApplication->selectedBase;
    $release    = $this->DivListApplication->selectedRelease;
    $Regex      = $this->DivListApplication->Regex;
    $SubSearch  = $this->DivListApplication->SubSearch; 
    $Flags      =  GL_NONE | GL_SIZELIMIT;
    $Filter     = "(&(cn=".$Regex.")(objectClass=gosaApplication))";
    $tmp        = array();
    $Releases   = $this->getReleases($base);

    if(!$this->enableReleaseManagement){
      $use_base = "ou=apps,".$base;
    }else{
      if(isset($Releases[$release])){
        $use_base  = $release;
      }else{
        $use_base  = "ou=apps,".$base;
      }
    }

    if($SubSearch){
      $Flags    |= GL_SUBSEARCH;  
    }
   
    $res= get_list($Filter, $this->ui->subtreeACL,$use_base, array("cn","description","dn","objectClass","FAIstate"), $Flags);
    $tmp2 = array();
    foreach ($res as $val){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['cn'][0]]=strtolower($val['cn'][0]).$val['cn'][0];
    }

    natcasesort($tmp2);
    $this->applications=array();
    foreach($tmp2 as $val){
      $this->applications[]=$tmp[$val];
    }
    reset ($this->applications);
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  function copyPasteHandling($s_action,$s_entry)
  {
    /* Paste copied/cutted object in here
     */
    if(($s_action == "editPaste") || ($this->CopyPasteHandler->stillOpen())){
      $this->CopyPasteHandler->save_object();
      $this->CopyPasteHandler->SetVar("base", $this->DivListApplication->selectedBase);

      if($str = $this->CopyPasteHandler->execute()) {
        return($str);
      }
    }

    /* Copy current object to CopyHandler
     */
    if($s_action == "copy"){
      $this->CopyPasteHandler->Clear();
      $dn = $this->applications[$s_entry]['dn'];
      $obj    = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], $dn);
      $objNew = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], "new");
      $this->CopyPasteHandler->Copy($obj,$objNew);
    }

    /* Copy current object to CopyHandler
     */
    if($s_action == "cut"){
      $this->CopyPasteHandler->Clear();
      $dn = $this->applications[$s_entry]['dn'];
      $obj = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], $dn);
      $this->CopyPasteHandler->Cut($obj);
    }
  }

  /* Save to LDAP */
  function save()
  {
    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

  function remove_lock()
  {
    if (isset($this->apptabs->dn)){
      del_lock ($this->apptabs->dn);
    }
  }

  function save_object() {
    $this->DivListApplication->save_object();
  }

  function check() {}
  function adapt_from_template($dn) {}
  function password_change_needed() {}
  function show_header($button_text, $text, $disabled= FALSE) {}
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
