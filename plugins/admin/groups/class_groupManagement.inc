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
require "tabs_group.inc";

class groupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Groups";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $grouptab= NULL;
  var $grouplist= array();
  var $ui= NULL;
  var $acl= "";

  var $CopyPasteHandler  = NULL;
  var $DivListGroup      = NULL;

  var $ShowPrimaryCheckBox = false; 

  function groupManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Copy & Paste enabled ?*/
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE']))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Detect if we have to display the primary group checkbox */
    if (isset($this->config->data['MAIN']['NOPRIMARYGROUP'])){
      $this->ShowPrimaryCheckBox = false;
    } else {
      $this->ShowPrimaryCheckBox = true;
    }

    /* Create dialog object */
    $this->DivListGroup = new divListGroup($this->config,$this);
    $this->DivListGroup->DisableCheckBox("ShowPrimaryGroups",$this->ShowPrimaryCheckBox);
  }

  function execute()
  {
  	/* Call parent execute */
  	plugin::execute();

    /* Store these posts if the current object is locked (used by somebody else)*/
    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^group_edit_/","/^group_del_/");

    /* Save data */
    $s_action   = "";
    $s_entry    = "";

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/^group_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/group_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/^group_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/group_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^group_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/^group_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/^group_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/group_chgpw_/i","",$key);
      }elseif(preg_match("/_group_edit_/",$key)){
        $type = preg_replace("/_group_edit_.*$/","",$key);
        $s_action="edit";
        $s_entry  = preg_replace("/".$type."_group_edit_/i","",$key); 
        $_POST['arg'] = $type;
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry); 

    /* Check for posted gets */
    if((isset($_GET['act'])) && ($_GET['act'] == "edit_entry")){
      $s_entry    = $_GET['id'];
      $s_action = "edit";
    }

    $smarty= get_smarty();

    /********************
      Copy & Paste Handling  ...
     ********************/

    /* Only perform copy&paste requests if it is enabled
     */
    if($this->CopyPasteHandler){
      if($str = $this->copyPasteHandling($s_action,$s_entry)){
        return $str;
      }
    }


    /********************
      Create a new group  ...
     ********************/

    /* New group? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->grouptab= new grouptabs($this->config, $this->config->data['TABS']['GROUPTABS'], $this->dn);

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->DivListGroup->selectedBase, $this->ui->subtreeACL);
      $this->grouptab->set_acl($acl);
    }


    /********************
      Save Group Tab/Object Changes
     ********************/

    /* Finish group edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->grouptab->config)) ){

      /* Check tabs, will feed message array 
         Save, or display error message? */
      $message= $this->grouptab->check();
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->grouptab->save();
        gosa_log ("Group object'".$this->dn."' has been saved");

        /* Group has been saved successfully, remove lock from LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new users at this point. */
        //$this->reload ();
        unset ($this->grouptab);
        $this->grouptab= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /********************
      Edit existing group 
     ********************/

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->grouptab-> config))){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->grouplist[trim($s_entry)]['dn'];

      /* Check locking & lock entry if required */
      $user = get_lock($this->dn);
      if ($user != ""){
        return(gen_locked_message ($user, $this->dn));
      }
      add_lock ($this->dn, $this->ui->dn);

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Register grouptab to trigger edit dialog */
      $this->grouptab= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $this->dn);
      $this->grouptab->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }


    /********************
      Delete group 
     ********************/

    /* Remove group was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->grouplist[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "group", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the group '%s'."), @LDAP::fix($this->dn)));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));

      } else {
        
        /* Obviously the user isn't allowed to delete. Show message and clean session. */
        print_red (_("You are not allowed to delete this group!"));
      }
    }


    /********************
      Delete group confirmed  
     ********************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->grouptab= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $this->dn);
        $this->grouptab->set_acl(array($this->acl));
        $this->grouptab->delete ();
        gosa_log ("Group object'".$this->dn."' has been removed");
        unset ($this->grouptab);
        $this->grouptab= NULL;

        /* Group list has changed, reload it. */
        //$this->reload ();

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this group!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /********************
      Delete group canceled  
     ********************/

    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }
    

    /********************
      A dialog was canceled  
     ********************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->grouptab->dn);
      unset ($this->grouptab);
      $this->grouptab= NULL;
      unset($_SESSION['objectinfo']);
    }


    /********************
      If there is currently a dialog open, display it
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->grouptab->config)){
      $display= $this->grouptab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->grouptab->by_object[$this->grouptab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Display dialog with group list */
    $this->DivListGroup->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListGroup->SubSearch){
      $this->DivListGroup->AddDepartments($this->DivListGroup->selectedBase,4);
    }
    $this->reload ();
    $this->DivListGroup->setEntries($this->grouplist);
    return($this->DivListGroup->Draw());
  }

  function reload($CreatePosixsList=false)
  {
    $this->grouplist        = array();
    $primaries              = array();
    $functional             = array();
    $error= $error2         = "";
    $filter                 = "(objectclass=posixGroup)";

    $base                   = $this->DivListGroup->selectedBase;
    $Regex                  = $this->DivListGroup->Regex;
    $UserRegex              = $this->DivListGroup->UserRegex;
    $SubSearch              = $this->DivListGroup->SubSearch;
    $ShowPrimaryGroups      = $this->DivListGroup->ShowPrimaryGroups;
    $ShowSambaGroups        = $this->DivListGroup->ShowSambaGroups;
    $ShowApplicationGroups  = $this->DivListGroup->ShowApplicationGroups;
    $ShowMailGroups         = $this->DivListGroup->ShowMailGroups;
    $ShowFunctionalGroups   = $this->DivListGroup->ShowFunctionalGroups;

    /* Prepare ldap class */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($base);


    /********************
      Create filter depending on selected checkboxes 
     ********************/

    /* Add Mail Groups */
    if ($ShowMailGroups){
      $filter.= "(objectClass=gosaMailAccount)";
    }

    /* Add application groups */
    if ($ShowApplicationGroups){
      $filter.= "(objectClass=gosaApplicationGroup)";
    }

    /*
       #FIXME Why is "sfilter" or "filter" set ?
        Please verify && add comment 
     */
    $sfilter= "";
    if ($this->config->current['SAMBAVERSION'] == 3){
      if (!$ShowPrimaryGroups){
        $sfilter= "(objectClass=sambaGroupMapping)";
      } elseif ($ShowSambaGroups){
        $filter.= "(objectClass=sambaGroupMapping)";
      }
    }

    /* Prepare filter for given Regex && UserRegex */
    if ($filter != ""){
      $filter= "(&(cn=$Regex)(objectClass=posixGroup)(|$filter))";
      if ($UserRegex != ""){
        $filter= "(&(|(memberUID=".$UserRegex.")(cn=".$UserRegex."))$filter)";
      }
    }


    /********************
      Collect some groupids to be able to skip primary & functional groups 
     ********************/

    /* Collect primary groupIDs to show primary groups 
        if this option is enabled in gosa conf && the checkbox is checked */  
    if(($this->ShowPrimaryCheckBox) && ($ShowPrimaryGroups)){
      $ldap->search("(&(uid=$Regex)(!(uid=*$))(objectClass=posixAccount)(gidNumber=*))", array("gidNumber", "cn"));
      show_ldap_error($ldap->get_error());
      while ($attrs= $ldap->fetch()){
        $primaries[$attrs['gidNumber'][0]]= $attrs['cn'][0];
      }
    }
  
    /* Collect all GroupIDs from those groups which are functional.
        Only perfrom this search if  ShowFunctionalGroups  is unchecked, else leave arre empty  */ 
    if(!$ShowFunctionalGroups){ 
      $ldap->cd(get_groups_ou().$base);
      $ldap->set_size_limit($_SESSION['size_limit']);
      $ldap->search("(&(cn=$Regex)(objectClass=posixGroup)(!(|(objectClass=gosaMailAccount)(objectClass=gosaApplicationGroup)$sfilter)))", array("cn", "gidNumber", "description"));
      $error2= $ldap->error;
      while ($attrs= $ldap->fetch()){
        if (!isset($primaries[$attrs['gidNumber'][0]])){
          $functional[$attrs['gidNumber'][0]]= $attrs['gidNumber'][0];
        }
      }
    }
    

    /********************
      Search for the prepared filter 
     ********************/
    
    /* If subsearch is activated search for subobjects too */
    $attrs = array("cn", "description", "gidNumber", "objectClass");

    if ($SubSearch){
      $res= get_list($filter, $this->ui->subtreeACL, $base, $attrs, GL_SIZELIMIT| GL_SUBSEARCH);
    } else {
      $res= get_list($filter, $this->ui->subtreeACL, get_groups_ou().$base, $attrs, GL_SIZELIMIT);
    }

    /* Sort values into grouplist*/
    $tmp = $tmp2 = array();
    foreach ($res as $value){

      /* Skip functional groups if checkbox isn't checked */
      if (!$ShowFunctionalGroups && isset($functional[$value['gidNumber'][0]])){
        continue;
      }
      
      /* If gidNumber is in $primaries skip this entry */
      if (!isset($primaries[$value['gidNumber'][0]])){
        $tmp2[$value['cn'][0]] = $value;
        $tmp [$value['cn'][0]] = $value['cn'][0];
      }
    }
    natcasesort($tmp);
    foreach($tmp as $name){
      $this->grouplist[] = $tmp2[$name]; 
    }
    reset ($this->grouplist);
  }



  /* Perform copy & paste requests
     If copy&paste is in progress this returns a dialog to fix required attributes 
   */ 
  function copyPasteHandling($s_action,$s_entry)
  {
    /* Paste copied/cutted object in here
     */
    if(($s_action == "editPaste") || ($this->CopyPasteHandler->stillOpen())){
      $this->CopyPasteHandler->save_object();
      $this->CopyPasteHandler->SetVar("base",$this->DivListGroup->selectedBase);
      if($str = $this->CopyPasteHandler->execute()){
        return( $str);
      };
      /* Ensure that the new object is shown in the list now */
      $this->relaod();
    }


    /* Copy current object to CopyHandler
     */
    if($s_action == "copy"){
      $this->CopyPasteHandler->Clear();

      $dn     = $this->grouplist[trim($s_entry)]['dn'];
      $acl    = get_permissions ($dn, $this->ui->subtreeACL);

      $obj    = new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $dn);
      $objNew = new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], "new");
      $obj->set_acl($acl);
      $objNew->set_acl($acl);
      $this->CopyPasteHandler->Copy($obj,$objNew);
    }


    /* Copy current object to CopyHandler
     */
    if($s_action == "cut"){
      $this->CopyPasteHandler->Clear();
      $dn = $this->grouplist[trim($s_entry)]['dn'];
      $acl= get_permissions ($dn, $this->ui->subtreeACL);

      $obj= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $dn);
      $obj->set_acl($acl);

      $this->CopyPasteHandler->Cut($obj);
    }
  }

  /* Save data to object */
  function save_object()
  {
    $this->DivListGroup->save_object();
  }


  function remove_lock()
  {
    if (isset($this->grouptab->dn)){
      del_lock ($this->grouptab->dn);
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
  function adapt_from_template($dn) { }
  function password_change_needed()  { }
  function show_header($button_text, $text, $disabled= FALSE)  { }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
