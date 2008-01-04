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

class mimetypeManagement extends plugin
{
  /* Definitions */
  var $plHeadline     = "Mime types";
  var $plDescription  = "Manage mime types";

  /* Dialog attributes */
  var $ui                             = NULL;
  var $DivListMimeTypes               = NULL;
  var $enableReleaseManagement        = false;
  var $mimetabs                       = NULL;
  var $snapDialog                     = NULL;
  var $CopyPasteHandler               = NULL;
  var $start_pasting_copied_objects = FALSE;


  function mimetypeManagement (&$config, &$ui)
  {
    /* Save configuration for internal use */
    $this->config   = &$config;
    $this->ui       = &$ui;

    /* Check if copy & paste is activated */
    if($this->config->boolValueIsTrue("MAIN","ENABLECOPYPASTE")){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Creat dialog object */
    $this->DivListMimeTypes = new divListMimeTypes($this->config,$this);

    if($this->IsReleaseManagementActivated()){

      /* Check if we should enable the release selection */
      $this->enableReleaseManagement = true;

      /* Hide SubSearch checkbox */
      $this->DivListMimeTypes->DisableCheckBox("SubSearch");
    }

  }


  /* Get all releases */
  function getReleases($base)
  {
    $ldap                   = $this->config->get_ldap_link();
    $dn                     = get_ou('mimetypeou').$base;
    $ret                    = array();
    $ret [get_ou('mimetypeou').$base] = "/";    

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

    /* These vars will be stored if you try to open a locked mime, 
        to be able to perform your last requests after showing a warning message */
    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^mime_edit_/","/^mime_del_/","/^item_selected/","/^remove_multiple_mimetypes/"));

    $smarty       = get_smarty();             // Smarty instance
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    
    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/mime_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/mime_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/mime_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/mime_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
        // Post for new
      }elseif(preg_match("/^mime_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^remove_multiple_mimetypes/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^multiple_copy_mimetypes/",$key)){
        $s_action = "copy_multiple";
      }elseif(preg_match("/^multiple_cut_mimetypes/",$key)){
        $s_action = "cut_multiple";
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);


 
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
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "mime_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }

    /**************** 
      Copy & Paste handling  
     ****************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }


    /**************** 
      Create a new mime type 
     ****************/

    /* New mime type? */
    $ui = get_userinfo();
    $acl = $ui->get_permissions($this->DivListMimeTypes->selectedBase,"mimetypes/mimetype");
    if (($s_action=="new") && preg_match("/c/",$acl)){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->mimetabs= new mimetabs($this->config, $this->config->data['TABS']['MIMETABS'], $this->dn,"mimetypes");
      $this->mimetabs->set_acl_base($this->DivListMimeTypes->selectedBase);
    }   


    /**************** 
      Edit entry canceled 
     ****************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel'])){
      del_lock ($this->mimetabs->dn);
      unset ($this->mimetabs);
      $this->mimetabs= NULL;
      session::un_set('objectinfo');
    }


    /**************** 
      Edit entry finished 
     ****************/

    /* Finish mime edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply']) ) && (isset($this->mimetabs->config))){

      /* Check tabs, will feed message array */
      $this->mimetabs->save_object();
      $message= $this->mimetabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->mimetabs->set_release($this->DivListMimeTypes->selectedRelease);
        $this->mimetabs->save();

        if (!isset($_POST['edit_apply'])){
          /* Mime type has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }
          unset ($this->mimetabs);
          $this->mimetabs= NULL;
          session::un_set('objectinfo');
        }
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
    if (($s_action=="edit") && (!isset($this->mimetabs->config))){

      /* Get 'dn' from posted 'mimelist', must be unique */
      $this->dn= $this->mimetypes[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);


      /* Register mimetabs to trigger edit dialog */
      $this->mimetabs= new mimetabs($this->config,$this->config->data['TABS']['MIMETABS'], $this->dn,"mimetypes");
      $this->mimetabs->set_acl_base($this->dn);
      session::set('objectinfo',$this->dn);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      $this->dns = array();
      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->mimetypes[$id]['dn'];
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
          $smarty->assign("intro",     sprintf(_("You're about to delete the following entry %s"), @LDAP::fix($dns_names)));
        } else {
          $smarty->assign("intro",     sprintf(_("You're about to delete the following entries %s"), @LDAP::fix($dns_names)));
        }
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }




    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_mimetype_confirm'])){

      $ui = get_userinfo();

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        $acl = $ui->get_permissions($dn,"mimetypes/mimetype");
        if(preg_match("/d/",$acl)){

          /* Delete request is permitted, perform LDAP action */
          $this->mimetabs= new mimetabs($this->config, $this->config->data['TABS']['MIMETABS'], $dn,"mimetypes");
          $this->mimetabs->set_acl_base($dn);
          $this->mimetabs->delete ();
          unset ($this->mimetabs);
          $this->mimetabs= NULL;

        } else {
          /* Normally this shouldn't be reached, send some extra
             logs to notify the administrator */
          print_red (_("You are not allowed to delete this mime type!"));
          new log("security","mimetypes/".get_class($this),$dn,array(),"Tried to trick deletion.");
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
    if(isset($_POST['delete_multiple_mimetype_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /**************** 
      Delete mime type 
     ****************/

    /* Remove user was requested */
    if ($s_action == "del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->mimetypes[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $ui = get_userinfo();
      $acl = $ui->get_permissions($this->dn,"mimetypes/mimetype");
      if (preg_match("/d/",$acl)){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the mime type '%s'."), @LDAP::fix($this->dn)));
        $smarty->assign("multiple", false);
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this mime type!"));
      }
    }


    /**************** 
      Delete mime confirmed 
     ****************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_mime_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      $ui = get_userinfo();
      $acl = $ui->get_permissions($this->dn,"mimetypes/mimetype");
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->mimetabs= new mimetabs($this->config, $this->config->data['TABS']['MIMETABS'], $this->dn,"mimetypes");
        $this->mimetabs->set_acl_base($this->dn);
        $this->mimetabs->delete ();
        unset ($this->mimetabs);
        $this->mimetabs= NULL;

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this mime type!"));
        new log("security","mimetypes/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /**************** 
      Delete mime canceled 
     ****************/

    /* Delete mime type canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      session::un_set('objectinfo');
    }

    /* Show tab dialog if object is present */
    if (($this->mimetabs) && (isset($this->mimetabs->config))){
      $display= $this->mimetabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->mimetabs->by_object[$this->mimetabs->current]->dialog){
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


    /****************
      Dialog display
     ****************/

        /* Check if there is a snapshot dialog open */
    $base = $this->DivListMimeTypes->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListMimeTypes->parent = $this;
    $this->DivListMimeTypes->execute();
    $this->DivListMimeTypes->AddDepartments($this->DivListMimeTypes->selectedBase,3,1);
    $this->reload();
    $this->DivListMimeTypes->setEntries($this->mimetypes);
    return($this->DivListMimeTypes->Draw());
  }

    
  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array($this->DivListMimeTypes->selectedRelease));
  }



  function reload()
  {
    $this->mimetypes= array();

    /* Set base for all searches */
    $base       = $this->DivListMimeTypes->selectedBase;
    $release    = $this->DivListMimeTypes->selectedRelease;
    $Regex      = $this->DivListMimeTypes->Regex;
    $SubSearch  = $this->DivListMimeTypes->SubSearch; 
    $Flags      =  GL_NONE | GL_SIZELIMIT;
    $Filter     = "(&(|(cn=".$Regex.")(description=".$Regex."))(objectClass=gotoMimeType))";
    $tmp        = array();
    $Releases   = $this->getReleases($base);


    /* If release management is enabled, use release as base. */
    if(!$this->enableReleaseManagement){
      $use_base = $base;
    }else{
      if(isset($Releases[$release])){
        $use_base  = $release;
      }else{
        $use_base  = $base;
      }
    }

    /* In case of subsearch, add the subsearch flag */
    if($SubSearch){
      $Flags    |= GL_SUBSEARCH;  
    }else{
      if(!$this->enableReleaseManagement){
        $use_base = get_ou('mimetypeou').$use_base;
      }
    }
  
    /* Get results and create index */ 
    $res= get_sub_list($Filter, "mimetypes",get_ou('mimetypeou'), $use_base, array("cn","description","dn","objectClass"), $Flags);
    $tmp2 = array();
    foreach ($res as $val){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0].$val['dn']]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['cn'][0].$val['dn']] = strtolower($val['cn'][0]).$val['cn'][0].$val['dn'];
    }

    /* sort entries */
    natcasesort($tmp2);
    $this->mimetypes=array();
    foreach($tmp2 as $val){
      $this->mimetypes[]=$tmp[$val];
    }
    reset ($this->mimetypes);
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
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
      $this->start_pasting_copied_objects = FALSE;
      $dn = $this->mimetypes[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"mimetabs","MIMETABS","mimetypes");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $this->start_pasting_copied_objects = FALSE;
  
      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->mimetypes[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","mimetabs","MIMETABS","mimetypes");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","mimetabs","MIMETABS","mimetypes");
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }

    /* Return C&P dialog */
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){

      /* Load entry from queue and set base */
      $this->CopyPasteHandler->load_entry_from_queue();
      $this->CopyPasteHandler->SetVar("base",$this->DivListMimeTypes->selectedBase);

      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    #if(!$this->CopyPasteHandler->entries_queued()){
    #  $this->start_pasting_copied_objects = FALSE;
    #}
    return("");
  }


  /* Check if the release management is activated. */
  function IsReleaseManagementActivated()
  {
    /* Check if we should enable the release selection */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      return(true);
    }
    return(false);
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


  /* Save to LDAP */
  function save()
  {
    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

  function remove_lock()
  {
    if (isset($this->mimetabs->dn)){
      del_lock ($this->mimetabs->dn);
    }
  }

  function save_object() {
    $this->DivListMimeTypes->save_object();
  }

  function check() {}
  function adapt_from_template($dn) {}
  function password_change_needed() {}
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
