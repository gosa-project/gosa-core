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

class ogroupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Object groups";
  var $plDescription= "Manage object groups";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $obtypes= array();
  var $ogroup;

  var $CopyPasteHandler = NULL;
  var $DivListOGroup    = NULL;
  var $start_pasting_copied_objects = FALSE;

  var $dns = array();
  var $acl_module   = array("ogroups");  

  function ogroupManagement (&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $this->ui= get_userinfo();

    /* Copy & Paste enabled ?
     */
    if ($this->config->get_cfg_value("copyPaste") == "true"){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Div lsit management */
    $this->DivListOGroup = new divListOGroup($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /****************
      Variable intialisation && Check posts for commands  
     ****************/

    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^group_edit_/","/^group_del_/","/^item_selected/","/^remove_multiple_ogroups/","/^menu_action/"));

    $smarty     = get_smarty();
    $s_action   = "";
    $s_entry    = "";

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/^group_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/^group_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/^group_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/^group_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^group_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }elseif(preg_match("/^remove_multiple_ogroups/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^multiple_cut_ogroups/",$key)){
        $s_action = "cut_multiple";
      }elseif(preg_match("/^multiple_copy_ogroups/",$key)){
        $s_action = "copy_multiple";
      }elseif(preg_match("/_group_edit_/",$key)){
        $type = preg_replace("/_group_edit_.*$/","",$key);
        $s_action="edit";
        $s_entry  = preg_replace("/".$type."_group_edit_/i","",$key);
        $_POST['arg'] = $type;
      }
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    // Edit if
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
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
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "group_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }

    /* Hanlde notification event requests */
    if(isset($_POST['menu_action']) && preg_match("/^event/",$_POST['menu_action'])){
      $s_action = $_POST['menu_action'];
    }


    /********************
      Create notification event
     ********************/

    if(preg_match("/^event_/",$s_action) && class_available("DaemonEvent")){
      $ids = $this->list_get_selected_items();
      $uids = array();
      foreach($ids as $id){
        $uids[] = $this->ogrouplist[$id]['cn'][0];
      }
      if(count($uids)){
        $events = DaemonEvent::get_event_types(USER_EVENT);
        $event = preg_replace("/^event_/","",$s_action);
        if(isset($events['BY_CLASS'][$event])){
          $type = $events['BY_CLASS'][$event];
          $this->ogroup = new $type['CLASS_NAME']($this->config);
          $this->ogroup->add_targets($uids);
          $this->ogroup->set_type(TRIGGERED_EVENT);
        }
      }
    }

    /* Abort event dialog */
    if(isset($_POST['abort_event_dialog'])){
      $this->ogroup = FALSE;
    }

    /* Save event */
    if(isset($_POST['save_event_dialog'])){
      $o_queue = new gosaSupportDaemon();
      $o_queue->append($this->ogroup);
      if($o_queue->is_error()){
        msg_dialog::display(_("Infrastructure error"), msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
      }else{
        $this->ogroup = FALSE;
      }
    }

    /* Display event */
    if($this->ogroup instanceof DaemonEvent){
      $this->ogroup->save_object();
      return($this->ogroup->execute());
    }


    /****************
      Copy & Paste handling 
     ****************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }


    /***************
      Create a new object group 
     ****************/

    /* New group? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new ogroup- object */
      $this->ogroup= new ogrouptabs($this->config,$this->config->data['TABS']['OGROUPTABS'], $this->dn,"ogroups");
      $this->ogroup->set_acl_base($this->DivListOGroup->selectedBase);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/
    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){
        $this->dns = array();
        $disallowed = array();
        foreach($ids as $id){
          $dn = $this->ogrouplist[$id]['dn'];
          $acl = $this->ui->get_permissions($dn, "users/user");
          if(preg_match("/d/",$acl)){
            $this->dns[$id] = $dn;
          }else{
            $disallowed[] = $dn;
          }
        }

        if(count($disallowed)){
          msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
        }

        if(count($this->dns)){

          /* Check locks */
          if ($user= get_multiple_locks($this->dns)){
            return(gen_locked_message($user,$this->dns));
          }

          $dns_names = array();
          foreach($this->dns as $dn){
            $dns_names[] = @LDAP::fix($dn);
          }

          add_lock ($this->dns, $this->ui->dn);

          /* Lock the current entry, so nobody will edit it during deletion */
          $smarty->assign("info", msgPool::deleteInfo($dns_names,_("object group")));
          $smarty->assign("multiple", true);
          return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
        }
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_ogroup_confirm'])){

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        $acl = $this->ui->get_permissions($dn, "ogroups");
        if (preg_match('/d/', $acl)){

          /* Delete request is permitted, perform LDAP action */
          $this->ogroup= new ogrouptabs($this->config,$this->config->data['TABS']['OGROUPTABS'], $dn);
          $this->ogroup->delete ();
          unset ($this->ogroup);
          $this->ogroup= NULL;
        } else {

          /* Normally this shouldn't be reached, send some extra
             logs to notify the administrator */
          msg_dialog::display(_("Permission error"), msgPool::permDelete(), INFO_DIALOG);
          new log("security","ogroups/".get_class($this),$dn,array(),"Tried to trick deletion.");
        }
      }

      /* Remove lock file after successfull deletion */
      $this->remove_lock();
      $this->dns = array();
    }


    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
    if(isset($_POST['delete_multiple_ogroup_cancel'])){

      /* Remove lock file after successfull deletion */
      $this->remove_lock();
      $this->dns = array();
    }


    /****************
      Delete object group
     ****************/

    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->ogrouplist[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl = $this->ui->get_permissions($this->dn,"ogroups");
      if(preg_match("/d/",$acl)){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", msgPool::deleteInfo(@LDAP::fix($this->dn),_("object group")));
        $smarty->assign("multiple", false);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), INFO_DIALOG);
      }
    }


    /****************
      Delete confirmed 
     ****************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      $acl = $this->ui->get_permissions($this->dn,"ogroups");
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->ogroup= new ogrouptabs($this->config,
            $this->config->data['TABS']['OGROUPTABS'], $this->dn);
        $this->ogroup->delete ();
        unset ($this->ogroup);
        $this->ogroup= NULL;
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), INFO_DIALOG);
        new log("security","ogroups/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      $this->remove_lock();
      session::un_set('objectinfo');
    }


    /****************
      Delete object group Canceled
     ****************/

    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      $this->remove_lock();
      session::un_set('objectinfo');
    }


    /****************
      Edit group
     ****************/

    if (($s_action=="edit") && (! isset($this->ogroup->config)) ){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->ogrouplist[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Register grouptab to trigger edit dialog */
      $this->ogroup= new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'], $this->dn, "ogroups");
      $this->ogroup->set_acl_base($this->dn);
      session::set('objectinfo',$this->dn);
    }


    /****************
      Edit finished save 
     ****************/

    /* Finish button has been pressed */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->ogroup->config)) ){

      /* Check tabs, will feed message array */
      $message= $this->ogroup->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->ogroup->save();

        if (!isset($_POST['edit_apply'])){
          /* Group has been saved successfully, remove lock from
             LDAP. */
          if ($this->dn != "new"){
            $this->remove_lock();
          }

          unset ($this->ogroup);
          $this->ogroup= NULL;
          session::un_set('objectinfo');
        }else{

          /* Reinitialize tab */
          if($this->ogroup instanceof tabs){
            $this->ogroup->re_init();
          }
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        msg_dialog::displayChecks($message);
      }
    }


    /****************
      Cancel edit object group
     ****************/

    /* Cancel dialogs */
    if ((isset($_POST['edit_cancel']))  && (isset($this->ogroup->dn))){
      $this->remove_lock();
      $this->ogroup= NULL;
      session::un_set('objectinfo');
    }


    /****************
      Display open dialogs 
     ****************/

    /* Show dialog if object is present */
    if (isset($this->ogroup->config)){
      $display= $this->ogroup->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->ogroup->by_object[$this->ogroup->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\"".msgPool::okButton()."\">\n";
        $display.= "&nbsp;\n";
        if ($this->dn != "new"){
          $display.= "<input type=submit name=\"edit_apply\" value=\"".msgPool::applyButton()."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /****************
      Display list 
     ****************/

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListOGroup->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases(),$this)){
      return($str);
    }

    /* Display dialog with group list */
    $this->DivListOGroup->parent = $this;
    $this->DivListOGroup->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListOGroup->SubSearch){
      $this->DivListOGroup->AddDepartments($this->DivListOGroup->selectedBase,4,1);
    }
    $this->reload ();
    $this->DivListOGroup->setEntries($this->ogrouplist);
    return($this->DivListOGroup->Draw());
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_ou('ogroupou').$this->DivListOGroup->selectedBase));
  }

  

  function convert_list($input)
  {
    $temp= "";

    $conv= array(   
        "U" => array("plugins/generic/images/head.png"        ,_("User")        , "ogroup"),
        "G" => array("plugins/groups/images/groups.png"       ,_("Group")       , "ogroup"),
        "A" => array("plugins/ogroups/images/application.png" ,_("Application") , "ogroup"),
        "D" => array("plugins/departments/department.png"     ,_("Department")  , "ogroup"),
        "S" => array("plugins/ogroups/images/server.png"      ,_("Server")      , "ogroup"),
        "F" => array("plugins/ogroups/images/asterisk.png"    ,_("Phone")       , "phonequeue"),
        "W" => array("plugins/ogroups/images/workstation.png" ,_("Workstation") , "workstartup"),
        "O" => array("plugins/ogroups/images/winstation.png"  ,_("Windows Install") , "ogroup"),
        "T" => array("plugins/ogroups/images/terminal.png"    ,_("Terminal")    , "termgroup"),
        "P" => array("plugins/ogroups/images/printer.png"     ,_("Printer")     , "ogroup"));

    /* Assemble picture */
    $type= $input['gosaGroupObjects'][0];
    $type= preg_replace("/[^A-Z]/i","",$type);
    if (isset($type[0])){
      $p1['pic']= $conv[$type[0]][0];
      $p1['tit']= $conv[$type[0]][1];
      $p1['alt']= $type[0];
      $p1['lnk']= $conv[$type[0]][2];
      $html_object_1 = "<input type='image' name='".$p1['lnk']."_group_edit_%KEY' ";
    } else {
      $p1['pic']= "images/empty.png";
      $p1['tit']= "";
      $p1['alt']= "";
      $p1['lnk']= "";//$conv[$type[0]][2];
      $html_object_1 = "<img ";
    }

    if (isset($type[1])){
      $p2['pic']= $conv[$type[1]][0];
      $p2['alt']= $type[1];
      $p2['tit']= $conv[$type[1]][1];
      $p2['lnk']= $conv[$type[1]][2];
      $html_object_2 = "<input type='image' name='".$p2['lnk']."_group_edit_%KEY' ";
    } else {
      $p2['pic']= "images/empty.png";
      $p2['alt']= "";
      $p2['tit']= "";
      $p2['lnk']= ""; //$conv[$type[0]][2];
      $html_object_2 = "<img ";
    }
    
    $temp = $html_object_1." class='center' id='iconA".preg_replace("/_/","",$p1['lnk']."_group_edit_%KEY")."' 
      src='".$p1['pic']."' alt='".$p1['alt']."' title='".$p1['tit']."'>";
    $temp.= $html_object_2." class='center' id='iconB".preg_replace("/_/","",$p2['lnk']."_group_edit_%KEY")."' 
      src='".$p2['pic']."' alt='".$p2['alt']."' title='".$p2['tit']."'>";
    return ($temp);
  }


  function reload()
  {
    /* Set base for all searches && initialise some vars */
    $this->ogrouplist= array();
    $base     = $this->DivListOGroup->selectedBase;
    $filter   = "(gosaGroupObjects=[])";
    $Regex    = $this->DivListOGroup->Regex;

    $chk = array(
        "UserGroups"          => "(gosaGroupObjects=*U*)" ,
        "GroupGroups"         => "(gosaGroupObjects=*G*)" ,
        "ApplicationGroups"   => "(gosaGroupObjects=*A*)" ,
        "DepartmentGroups"    => "(gosaGroupObjects=*D*)" ,
        "ServerGroups"        => "(gosaGroupObjects=*S*)" ,
        "WorkstationGroups"   => "(gosaGroupObjects=*W*)" ,
        "WindowsGroups"       => "(gosaGroupObjects=*O*)" ,
        "TerminalGroups"      => "(gosaGroupObjects=*T*)" ,
        "PrinterGroups"       => "(gosaGroupObjects=*P*)" ,
        "PhoneGroups"         => "(gosaGroupObjects=*F*)" );
    /* Create filter */ 
    foreach($chk as $chkBox => $FilterPart){
      if($this->DivListOGroup->$chkBox){
        $filter .=  $FilterPart;
      }
    }
    $filter= "(&(cn=$Regex)(objectClass=gosaGroupOfNames)(|$filter))";

    if($this->DivListOGroup->SubSearch){
      $res= get_sub_list($filter, "ogroups",get_ou('ogroupou'), $base, array("cn","objectClass","gosaGroupObjects"), GL_SIZELIMIT | GL_SUBSEARCH);
    }else{
      $res= get_sub_list($filter, "ogroups",get_ou('ogroupou'), get_ou('ogroupou').$base, array("cn","objectClass","gosaGroupObjects"), GL_SIZELIMIT );
    }

    $this->ogrouplist= $res;
    ksort ($this->ogrouplist);
    reset ($this->ogrouplist);
    $tmp=array();
    $tmp2 = array();
    foreach($this->ogrouplist as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['cn'][0]] = strtolower($val['cn'][0]).$val['cn'][0];
    }
    natcasesort($tmp2);
    $this->ogrouplist=array();
    foreach($tmp2 as $val){
      $this->ogrouplist[]=$tmp[$val];
    }
    reset ($this->ogrouplist);
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


  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    $ui = get_userinfo();

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn = $this->ogrouplist[$s_entry]['dn'];
      if($s_action == "copy" && $ui->is_copyable($dn,"ogroups","ogroup")){
        $this->CopyPasteHandler->add_to_queue($dn,$s_action,"ogrouptabs","OGROUPTABS","ogroups");
      }
      if($s_action == "cut" && $ui->is_cutable($dn,"ogroups","ogroup")){
        $this->CopyPasteHandler->add_to_queue($dn,$s_action,"ogrouptabs","OGROUPTABS","ogroups");
      }
    }


    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->ogrouplist[$id]['dn'];

        if($s_action == "copy_multiple" && $ui->is_copyable($dn,"ogroups","ogroup")){ 
          $this->CopyPasteHandler->add_to_queue($dn,"copy","ogrouptabs","OGROUPTABS","ogroups");
        }
        if($s_action == "cut_multiple" && $ui->is_cutable($dn,"ogroups","ogroup")){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","ogrouptabs","OGROUPTABS","ogroups");
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
      $this->CopyPasteHandler->SetVar("base",$this->DivListOGroup->selectedBase);
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


  function remove_lock()
  {
    if (isset($this->apptabs->dn)){
      del_lock ($this->apptabs->dn);
    }
    if(isset($this->dn) && !empty($this->dn) && $this->dn != "new"){
      del_lock($this->dn);
    }
    if(isset($this->dns) && is_array($this->dns) && count($this->dns)){
      del_lock($this->dns);
    }
  }


  function save_object()
  {
    $this->DivListOGroup->save_object();
    if(is_object($this->CopyPasteHandler)){
      $this->CopyPasteHandler->save_object();
    }
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
