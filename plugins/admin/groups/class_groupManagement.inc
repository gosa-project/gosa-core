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

class groupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Groups";
  var $plDescription= "Manage POSIX groups";

  /* Dialog attributes */
  var $grouptab           = NULL;
  var $grouplist          = array();
  var $ui                 = NULL;
  var $CopyPasteHandler   = NULL;
  var $DivListGroup       = NULL;
  var $ShowPrimaryCheckBox= false; 
  var $start_pasting_copied_objects = FALSE;

  function groupManagement (&$config, &$ui)
  {
    /* Save configuration for internal use */
    $this->config = &$config;
    $this->ui     = &$ui;

    /* Copy & Paste enabled ?*/
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE']))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }
    
    /* Detect if we have to display the primary group checkbox */
    $tmp = "";
    if (isset($this->config->data['MAIN']['NOPRIMARYGROUP'])){
      $tmp = $this->config->data['MAIN']['NOPRIMARYGROUP'];
    }
    if(preg_match("/true/i",$tmp)|| (preg_match("/yes/",$tmp))){ 
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
    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^group_edit_/","/^group_del_/","/^item_selected/","/^remove_multiple_groups/","/^multiple_edit/","/menu_action/"));

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
      }elseif(preg_match("/^remove_multiple_groups/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^multiple_copy_groups/",$key)){
        $s_action = "copy_multiple";
      }elseif(preg_match("/^multiple_cut_groups/",$key)){
        $s_action = "cut_multiple";
      }
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry); 

    /* Check for posted gets */
    if((isset($_GET['act'])) && ($_GET['act'] == "edit_entry")){
      $s_entry    = $_GET['id'];
      $s_action = "edit";
    }

    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
      $s_action = "copy_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
      $s_action = "cut_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^editPaste/",$_POST['menu_action'])){
      $s_action = "editPaste";
    }

    /* Create options */
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "group_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "multiple_edit"){
      $s_action = "multiple_edit";
    }

    if(isset($_POST['menu_action']) && preg_match("/^trigger_event_DaemonEvent_notify/",$_POST['menu_action'])){
      $s_action = "notify";
    }

    $smarty= get_smarty();



    /********************
      Create notification event
     ********************/

    if($s_action == "notify"){

      $ids = $this->list_get_selected_items();
      $uids = array();
      foreach($ids as $id){
        $uids[] = $this->grouplist[$id]['cn'][0];
      }
      if(count($uids)){
        $this->grouptab = new DaemonEvent_notify($this->config);
        $this->grouptab->add_targets($uids);
      }
    }

    /* Abort event dialog */
    if(isset($_POST['abort_event_dialog'])){
      $this->grouptab = FALSE;
    }

    /* Save event */
    if(isset($_POST['save_event_dialog'])){
      $o_queue = new gosaSupportDaemon();
      $o_queue->append($this->grouptab);
      if($o_queue->is_error()){
        msg_dialog::display(_("Daemon"),sprintf(_("Something went wrong while talking to the daemon: %s."),
              $o_queue->get_error()),ERROR_DIALOG);
      }else{
        $this->grouptab = FALSE;
      }
    }

    /* Display event */
    if($this->grouptab instanceof DaemonEvent){
      $this->grouptab->save_object();
      return($this->grouptab->execute());
    }



    /********************
      Copy & Paste Handling  ...
     ********************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }


    /********************
      Create a new group  ...
     ********************/

    /* New group? */
    if ($s_action=="new"){

      /* Check create permissions */
      $acl = $this->ui->get_permissions($this->DivListGroup->selectedBase,"groups/group");
      if(preg_match("/c/",$acl)){

        /* By default we set 'dn' to 'new', all relevant plugins will
           react on this. */
        $this->dn= "new";

        /* Create new grouptab object */
        $this->grouptab= new grouptabs($this->config, $this->config->data['TABS']['GROUPTABS'], $this->dn);

        /* Set up the group ACL's for this 'dn' */
        $this->grouptab->set_acl_base($this->DivListGroup->selectedBase);
      }
    }


    /********************
      Save Group Tab/Object Changes
     ********************/

    /* Finish group edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->grouptab->config)) ){

      /* Check tabs, will feed message array 
         Save, or display error message? */
      $message= $this->grouptab->check();
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->grouptab->save();

        if (!isset($_POST['edit_apply'])){
          /* Group has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          /* There's no page reload so we have to read new groups at this point. */
          //$this->reload ();
          unset ($this->grouptab);
          $this->grouptab= NULL;
          session::un_set('objectinfo');
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /********************
      Edit multiple entries
     ********************/

    /* User wants to edit data? */
    if ($s_action == "multiple_edit" && !isset($this->grouptab->config)){

      $this->dn = array();
      foreach($this->list_get_selected_items() as $id){
        $this->dn[] = $this->grouplist[$id]['dn'];;
      }
      $tmp = new multi_plug($this->config,"grouptabs",$this->config->data['TABS']['GROUPTABS'],
            $this->dn,$this->DivListGroup->selectedBase,"groups");
      if ($tmp->entries_locked()){
        return($tmp->display_lock_message());
      }
      $tmp->lock_entries($this->ui->dn);
      if($tmp->multiple_available()){
        $this->grouptab = $tmp;
        session::set('objectinfo',$this->grouptab->get_object_info());
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

      /* Register grouptab to trigger edit dialog */
      $this->grouptab= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $this->dn);
      $this->grouptab->set_acl_base($this->dn);
      session::set('objectinfo',$this->dn);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->grouplist[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn;
        }

        $dns_names = "<br><pre>";
        foreach($this->dns as $dn){
          add_lock ($dn, $this->ui->dn);
          $dns_names .= $dn."\n";
        }
        $dns_names .="</pre>";

        /* Lock the current entry, so nobody will edit it during deletion */
        if (count($this->dns) == 1){
          $smarty->assign("info",     sprintf(_("You're about to delete the following entry %s"), @LDAP::fix($dns_names)));
        } else {
          $smarty->assign("info",     sprintf(_("You're about to delete the following entries %s"), @LDAP::fix($dns_names)));
        }
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

      /* Confirmation for deletion has been passed. Groups should be deleted. */
      if (isset($_POST['delete_multiple_groups_confirm'])){

        /* Remove user by user and check acls before removeing them */
        foreach($this->dns as $key => $dn){

          /* Load permissions for selected 'dn' and check if
             we're allowed to remove this 'dn' */
          $acl = $this->ui->get_permissions($dn,"groups/group");
          if(preg_match("/d/",$acl)){

            /* Delete request is permitted, perform LDAP action */
            $this->grouptab= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $dn);
            $this->grouptab->set_acl_base($dn);
            $this->grouptab->delete ();
            unset ($this->grouptab);
            $this->grouptab= NULL;

          } else {

            /* Normally this shouldn't be reached, send some extra
               logs to notify the administrator */
            msg_dialog::display(_("Permission error"), _("You have no permission to delete this entry!"), ERROR_DIALOG);
            new log("security","groups/".get_class($this),$dn,array(),"Tried to trick deletion.");
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
      Delete group 
     ********************/

    /* Remove group was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->grouplist[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl = $this->ui->get_permissions($this->dn,"groups/group");
      if(preg_match("/d/",$acl)){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the group '%s'."), @LDAP::fix($this->dn)));
        $smarty->assign("multiple", false);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));

      } else {
        
        /* Obviously the user isn't allowed to delete. Show message and clean session. */
        msg_dialog::display(_("Permission error"), _("You have no permission to delete this entry!"), ERROR_DIALOG);
      }
    }


    /********************
      Delete group confirmed  
     ********************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      $acl = $this->ui->get_permissions($this->dn,"groups/group");
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->grouptab= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $this->dn);
        $this->grouptab->set_acl_base($this->dn);
        $this->grouptab->delete ();
        unset ($this->grouptab);
        $this->grouptab= NULL;

        /* Group list has changed, reload it. */
        //$this->reload ();

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Permission error"), _("You have no permission to delete this entry!"), ERROR_DIALOG);
        new log("security","groups/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }


    /********************
      Delete group canceled  
     ********************/

    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }
    

    /********************
      A dialog was canceled  
     ********************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      if(isset($this->grouptab->dn)){
        del_lock ($this->grouptab->dn);
      }
      unset ($this->grouptab);
      $this->grouptab= NULL;
      session::un_set('objectinfo');
    }


    /********************
      If there is currently a dialog open, display it
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->grouptab->config)){
      $display= $this->grouptab->execute();

      /* Don't show buttons if tab dialog requests this */
      if(isset($this->grouptab->by_object)){
        if (!$this->grouptab->by_object[$this->grouptab->current]->dialog){
          $display.= "<p style=\"text-align:right\">\n";
          $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
          $display.= "&nbsp;\n";
          if ($this->dn != "new"){
            $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
            $display.= "&nbsp;\n";
          }
          $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
          $display.= "</p>";
        }
      }
      return ($display);
    }


    /* Check if there is a snapshot dialog open */
    $base = $this->DivListGroup->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }
   
    /* Display dialog with group list */
    $this->DivListGroup->parent = $this;
    $this->DivListGroup->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListGroup->SubSearch){
      $this->DivListGroup->AddDepartments($this->DivListGroup->selectedBase,4,1);
    }
    $this->reload ();
    $this->DivListGroup->setEntries($this->grouplist);
    return($this->DivListGroup->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_groups_ou().$this->DivListGroup->selectedBase));
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
    $ldap->set_size_limit(session::get('size_limit'));


    /********************
      Create filter depending on selected checkboxes 
     ********************/

    /* Add application groups */
    if ($ShowApplicationGroups){
      $filter.= "(objectClass=gosaApplicationGroup)";
    }
    
    /* Add Mail Groups */
    if ($ShowMailGroups){
      $filter.= "(objectClass=gosaMailAccount)";
    }

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
    if ($this->ShowPrimaryCheckBox){
      $res = get_list("(&(uid=$Regex)(!(uid=*$))(objectClass=posixAccount)(gidNumber=*))",
            "groups", $base,array("gidNumber", "cn"),  GL_SUBSEARCH);
      foreach ($res as $attrs){
        $primaries[$attrs['gidNumber'][0]]= $attrs['cn'][0];
      }
    }

    /* Collect all GroupIDs from those groups which are functional.
       Only perfrom this search if  ShowFunctionalGroups  is unchecked, else leave arre empty  */ 
    $ff = "(&(cn=$Regex)(objectClass=posixGroup)(!(|(objectClass=gosaMailAccount)(objectClass=gosaApplicationGroup)$sfilter)))";
    if ($SubSearch){
      $res = get_sub_list($ff, "groups",get_groups_ou(), $base,array("gidNumber", "cn", "description"), GL_SUBSEARCH);
    } else {
      $res = get_sub_list($ff, "groups",get_groups_ou(), $base,array("gidNumber", "cn", "description"), GL_NONE);
    }
    foreach($res as $attrs){
      if (!isset($primaries[$attrs['gidNumber'][0]])){
        $functional[$attrs['gidNumber'][0]]= $attrs['gidNumber'][0];
      }
    }
 
    /********************
      Search for the prepared filter 
     ********************/
   
    /* Attributes to search for */    
    $attrs = array("cn", "description", "gidNumber", "objectClass");
 
    /* If subsearch is activated search for subobjects too */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $attrs [] = "FAIrelease";
    }

    if ($SubSearch){
      $res= get_sub_list($filter, "groups",get_groups_ou(), $base, $attrs, GL_SIZELIMIT| GL_SUBSEARCH);
    } else {
      $res= get_sub_list($filter, "groups",get_groups_ou(), get_groups_ou().$base, $attrs, GL_SIZELIMIT);
    }

    /* Sort values into grouplist*/
    $tmp = $tmp2 = array();
    foreach ($res as $value){
      /* Skip functional groups if checkbox isn't checked */
      if (!$ShowFunctionalGroups && isset($functional[$value['gidNumber'][0]])){
        continue;
      }
      
      /* If gidNumber is in $primaries skip this entry */
      if (($ShowPrimaryGroups)   ||   (!$ShowPrimaryGroups && !isset($primaries[$value['gidNumber'][0]]))){
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
      $dn = $this->grouplist[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"grouptabs","GROUPTABS","groups");
    }


    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->grouplist[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","grouptabs","GROUPTABS","groups");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","grouptabs","GROUPTABS","groups");
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
      $this->CopyPasteHandler->SetVar("base",$this->DivListGroup->selectedBase);

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
