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

class phoneConferenceManagment extends plugin
{
  /* Definitions */
  var $plHeadline				  = "Phone conferences";
  var $plDescription			= "Management";

  /* CLI vars */
  var $cli_summary			  = "Handling of LDAP subtrees";
  var $cli_description		= "Some longer text\nfor help";
  var $cli_parameters			= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes				  = array();
  var $objectclasses			= array();

  var $conferences			  = array();
  var $conftab            = false;
  var $ui                 = NULL;
  var $DivListConference  = NULL;

  var $CopyPasteHandler = NULL;
  var $start_pasting_copied_objects = FALSE;

  /* Initialise Class */
  function phoneConferenceManagment (&$config, $ui)
  {
    $this->ui			  = $ui;
    $this->dn			  = "";
    $this->config		= $config;
    $this->DivListConference = new divListConference($this->config,$this);

    /* Copy & Paste enabled ?*/
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE']))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }
  }


  /* Execute class and display something */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^id$/","/^act$/","/^conference_/","/^item_selected/","/^remove_multiple_conferences/");

    /***************
      Variable initialisation
     ***************/
  
    /* Reload departments */
    $smarty						          = get_smarty();
    $display 					          = "";
    $s_action 					        = "";  // Will contain an action, like del or edit
    $s_entry  					        = "";  // The entry name for edit delete -...


    /***************
      Check posts  
     ***************/
  
    foreach($_POST as $key => $post){
      if(preg_match("/^conference_new.*/i",$key)){
        $s_action = "new";
        // Post for delete
      }elseif(preg_match("/^conference_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/^conference_del_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
        // Post for edit
      }elseif(preg_match("/conference_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/conference_edit_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
      }elseif(preg_match("/^remove_multiple_conferences/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
        $s_entry  = preg_replace("/_.$/","",$s_entry);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
        $s_entry  = preg_replace("/_.$/","",$s_entry);
      }elseif(preg_match("/^multiple_copy_objects/",$key)){
        $s_action = "copy_multiple";
      }elseif(preg_match("/^multiple_cut_objects/",$key)){
        $s_action = "cut_multiple";
      } 
    }

    /* Edit Entry */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action = "edit";
      $s_entry  = $_GET['id'];
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
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "conference_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }


    /***************
      Cancel dialogs 
     ***************/
  
    /* Reset requested? */
    if (isset($_POST['edit_cancel'])){
      if (isset($this->conftab)){
        del_lock ($this->conftab->dn);
        unset ($this->conftab);
      }
      $this->conftab= NULL;
      $this->lognames= array();;
      $this->cn= "";
      session::un_set('objectinfo');
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
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->conferences[$id]['dn'];
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
        $smarty->assign("info",     sprintf(_("You're about to delete the following user(s) %s"), @LDAP::fix($dns_names)));
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_conference_confirm'])){

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){
        $this->dn = $dn;
        $acl = $this->ui->get_permissions($this->dn,"gofonconference/conference");
        if(preg_match("/d/",$acl)){
          $this->remove_from_parent();
        } else {
          print_red (_("You have no permission to remove this department."));
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
    if(isset($_POST['delete_multiple_conference_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /***************
      Delete 
     ***************/
  
    /* Delete Entry if Posted action (s_action) == del
     * The entry which will be deleted is defined in $s_entry
     */
    if ($s_action =="del"){

      $acl = $this->ui->get_permissions($this->conferences[$s_entry]['dn'],"gofonconference/conference"); 
      if(preg_match("/d/",$acl)){
        $this->dn= $this->conferences[$s_entry]['dn'];

        /* Check locking */
        if (($conf= get_lock($this->dn)) != ""){
          $_SESSION['dn']= $this->dn;
          return(gen_locked_message($conf, $this->dn));
        } else {
          add_lock ($this->dn, $this->ui->dn);
          $smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), $this->dn));
          $smarty->assign("multiple", false);
          $display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
          return ($display);
        }
      }
    }


    /***************
      Delete confirmed  
     ***************/
  
    /* If department deletion is accepted ...
     * Finally delete department
     */
    if (isset($_POST['delete_department_confirm'])){
      $acl = $this->ui->get_permissions($this->dn,"gofonconference/conference"); 
      if(preg_match("/d/",$acl)){
        $this->remove_from_parent();
      } else {
        print_red (_("You have no permission to remove this department."));
      }
    }


    /***************
      Edit
     ***************/
  
    /* Edit Entry if Posted action (s_action) == edit
     * The entry which will be edited is defined in $s_entry
     */
    if (($s_action=="edit") && (!isset($this->conftab->config))){

      $this->dn= $this->conferences[$s_entry]['dn'];

      if (($conf= get_lock($this->dn)) != ""){
        return(gen_locked_message ($conf, $this->dn));
      }

      /* Lock the current entry, so everyone will get the  above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Register conftab to trigger edit dialog */
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn,"gofonconference");
      $this->conftab->set_acl_base($this->dn);
      $_SESSION['objectinfo']= $this->dn;
    }


    /***************
      Create new  
     ***************/
  
    /* Insert new entry*/    
    if($s_action == "new" ){

      $dummy_dn = "cn=dummy,".get_ou('conferenceou').$this->DivListConference->selectedBase;      
      $acl = $this->ui->get_permissions($dummy_dn,"gofonconference/conference"); 
      if(preg_match("/c/",$acl)){
        /* Set up the users ACL's for this 'dn' */
        $this->dn= "new";
        $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn,"gofonconference");
        $this->conftab->set_acl_base($dummy_dn);
        session::un_set('objectinfo');
      }
    }


    /***************
      Save entry  
     ***************/
  
    /* Edit finished, check and save changes */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->conftab->config))){
      /* Check tabs, will feed message array */
      $this->conftab->last= $this->conftab->current;
      $this->conftab->save_object();
      $message= $this->conftab->check();

      if (count($message) == 0){
        if($this->conftab->save() == 1){
          gosa_log ("goFonConference object '".$this->dn."' saving failed.");
          return;
        }
        gosa_log ("goFonConference object '".$this->dn."' has been saved");

        if (!isset($_POST['edit_apply'])){
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }
        }

        del_lock ($this->conftab->dn);;
        unset ($this->conftab);
        $this->conftab= NULL;
        session::un_set('objectinfo');
      } else {
        show_errors($message);
      }
    }


    /***************
      Display dialogs   
     ***************/
  
    /* if edit or new, show dialog */
    if(($this->conftab) && (isset($this->conftab->config))){
      $display= $this->conftab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->conftab->by_object[$this->conftab->current]->dialog){
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


    /***************
      display divlist
     ***************/

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListConference->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Return rendered main page */
    /* Display dialog with system list */
    $this->DivListConference->parent = $this;
    $this->DivListConference->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListConference->SubSearch){
      $this->DivListConference->AddDepartments($this->DivListConference->selectedBase,5,1);
    }
    $this->reload();
    $this->DivListConference->setEntries($this->conferences);
    return($this->DivListConference->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_ou('conferenceou').$this->DivListConference->selectedBase));
  }


  /* Reload entries for divlist.
   * reload all conferences for the current base, with the given regex
   */
  function reload()
  {
    $Base             = get_ou('conferenceou').$this->DivListConference->selectedBase;
    $SubSearch        = $this->DivListConference->SubSearch;
    $Regex            = $this->DivListConference->Regex;
    $Flags            = GL_SIZELIMIT ; 
    $Filter           = "(&(|(cn=".$Regex.")(description=".$Regex."))(objectClass=goFonConference))";
    $Attrs            = array("cn","goFonConferenceOwner","goFonPIN","telephoneNumber");
 
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }

    $this->conferences= get_list($Filter, "gofonconference", $Base, $Attrs, $Flags);
  }

  function remove_from_parent()
  {
    /* Ehm what are we doinf here ? */
  
    $cfg = new conference($this->config, $this->dn);
    $cfg->set_acl_category("gofonconference");
    $cfg->set_acl_base($this->dn);

    $cfg->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->recursive_remove();

    /* Optionally execute a command after we're done */
    $this->postremove();

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new conftab($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

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
      $dn = $this->conferences[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"conferencetabs","CONFERENCETABS","gofonconference");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->conferences[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","conferencetabs","CONFERENCETABS","gofonconference");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","conferencetabs","CONFERENCETABS","gofonconference");
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
      $this->CopyPasteHandler->SetVar("base",$this->DivListConference->selectedBase);

      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();

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


  function save_object()
  {
    $this->DivListConference->save_object();
  }

  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
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


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
