<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_plugin.inc 14584 2009-10-12 14:04:22Z hickert $$
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

class management
{
  // Public 
  public $config = null;
  public $ui     = null;

  public $plIcon        = "";
  public $plDescription = "";
  public $plHeadline    = "";

  // Protected 
  protected $dn = "";
  protected $dns = array();

  protected $storagePoints = array();

  protected $last_dn = "";
  protected $last_dns = array();

  protected $tabClass = "";
  protected $tabType = "";
  protected $aclCategory = "";
  protected $objectName = "";
  protected $tabObject = null;
  protected $dialogObject = null;

  protected $last_tabObject = null;
  protected $last_dialogObject = null;

  protected $displayApplyBtn = "";
  protected $cpHandler = null;
  protected $cpPastingStarted = FALSE;
 
  protected $snapHandler = null;

  // Private
  protected $plugname = "";
  protected $headpage = null;
  protected $filter = null;
  protected $actions = array();
  
  function  __construct($config,$ui,$plugname, $headpage)
  {
    $this->plugname = $plugname;
    $this->headpage = $headpage;
    $this->ui = $ui;
    $this->config = $config;

    if($this->cpHandler) $this->headpage->setCopyPasteHandler($this->cpHandler);
    if($this->snapHandler) $this->headpage->setSnapshotHandler($this->snapHandler);

    if(empty($this->plIcon)){
      $this->plIcon = "plugins/".$plugname."/images/plugin.png";
    }

    // Register default actions
    $this->registerAction("new",    "newEntry");
    $this->registerAction("edit",   "editEntry");
    $this->registerAction("apply",  "applyChanges");
    $this->registerAction("save",   "saveChanges");
    $this->registerAction("cancel", "cancelEdit");
    $this->registerAction("remove", "removeEntryRequested");
    $this->registerAction("removeConfirmed", "removeEntryConfirmed");

    $this->registerAction("copy",   "copyPasteHandler");
    $this->registerAction("cut",    "copyPasteHandler");
    $this->registerAction("paste",  "copyPasteHandler");

    $this->registerAction("snapshot",    "createSnapshotDialog");
    $this->registerAction("restore",     "restoreSnapshotDialog");
    $this->registerAction("saveSnapshot","saveSnapshot");
    $this->registerAction("restoreSnapshot","restoreSnapshot");
    $this->registerAction("cancelSnapshot","closeDialogs");

  }

  function execute()
  {
    // Ensure that html posts and gets are kept even if we see a 'Entry islocked' dialog.
    $vars = array('/^act$/','/^listing/','/^PID$/','/^FILTER_PID$/');
    session::set('LOCK_VARS_TO_USE',$vars);

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandler("",array());
    if($ret){
      return($this->getHeader().$ret);
    }

    // Update filter
    if ($this->filter) {
      $this->filter->update();
      session::global_set(get_class($this)."_filter", $this->filter);
      session::set('autocomplete', $this->filter);
      if (!$this->filter->isValid()){
        msg_dialog::display(_("Filter error"), _("The filter is incomplete!"), ERROR_DIALOG);
      }
    }

    // Handle actions (POSTs and GETs)
    $str = $this->handleActions($this->detectPostActions());
    if($str) return($this->getHeader().$str);

    // Open single dialog objects
    if(is_object($this->dialogObject)){
      if(method_exists($this->dialogObject,'save_object')) $this->dialogObject->save_object(); 
      if(method_exists($this->dialogObject,'execute')){
        $display = $this->dialogObject->execute(); 
        $display.= $this->_getTabFooter();
        return($this->getHeader().$display);
      } 
    }

    // Display tab object.
    if($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug){
      $this->tabObject->save_object();
      $display = $this->tabObject->execute();
      $display.= $this->_getTabFooter();
      return($this->getHeader().$display);
    }

    // Set current restore base for snapshot handling.
    if(is_object($this->snapHandler)){
      $bases = array();
      foreach($this->storagePoints as $sp){
        $bases[] = $sp.$this->headpage->getBase();
      }
      $this->snapHandler->setSnapshotBases($bases);
    }
    
    $this->headpage->update();
    $display = $this->headpage->render();
    return($this->getHeader().$display);
  }

  protected function getHeader()
  {
    if (get_object_info() != ""){
      $display= print_header(get_template_path($this->plIcon),_($this->plDescription),
      "<img alt=\"\" class=\"center\" src=\"".get_template_path('images/lists/locked.png')."\">".
        LDAP::fix(get_object_info()));
    } else {
      $display= print_header(get_template_path($this->plIcon),_($this->plDescription));
    }
    return($display);
  }


  protected function _getTabFooter()
  { 
    if(!($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug)){
      return("");
    }
  
    if($this->tabObject->by_object[$this->tabObject->current]){
      $current = $this->tabObject->by_object[$this->tabObject->current];  
      if(is_object($current->dialog)){
        return("");
      }
    }

    $str = "";
    if(isset($this->tabObject->read_only) && $this->tabObject->read_only == TRUE){
      $str.= "<p style=\"text-align:right\">
        <input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">
        </p>";
      return($str);
    }else{
      $str.= "<p style=\"text-align:right\">\n";
      $str.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\"".msgPool::okButton()."\">\n";
      $str.= "&nbsp;\n";
      if($this->displayApplyBtn){
        $str.= "<input type=submit name=\"edit_apply\" value=\"".msgPool::applyButton()."\">\n";
        $str.= "&nbsp;\n";
      }
      $str.= "<input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";
      $str.= "</p>";
    }
    return($str);
  }


  protected function removeEntryRequested($action,$entry,$all)
  {
    $disallowed = array();
    $this->dns = array();
    foreach($entry as $dn){
      $acl = $this->ui->get_permissions($dn, $this->aclCategory."/".$this->aclPlugin);
      if(preg_match("/d/",$acl)){
        $this->dns[] = $dn;
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
        $dns_names[] =LDAP::fix($dn);
      }
      add_lock ($this->dns, $this->ui->dn);

      /* Lock the current entry, so nobody will edit it during deletion */
      $smarty = get_smarty();
      $smarty->assign("info", msgPool::deleteInfo($dns_names,_($this->objectName)));
      $smarty->assign("multiple", true);
      return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
    }
  }  


  protected function removeEntryConfirmed()
  {
    foreach($this->dns as $key => $dn){

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl = $this->ui->get_permissions($dn, $this->aclCategory."/".$this->aclPlugin);
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->dn = $dn;
        $tab = $this->tabClass;
        $this->tabObject= new $tab($this->config,$this->config->data['TABS'][$this->tabType], $this->dn, $this->aclCategory);
        $this->tabObject->set_acl_base($this->dn);
        $this->tabObject->delete ();
        del_lock($this->dn);        
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
        new log("security","groups/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }
    }

    $this->remove_lock();
    $this->closeDialogs();
  }


  function detectPostActions()
  {
    $action= $this->headpage->getAction();

    if(isset($_POST['edit_apply']))  $action['action'] = "apply";    
    if(isset($_POST['edit_finish'])) $action['action'] = "save";    
    if(isset($_POST['edit_cancel'])) $action['action'] = "cancel";    
    if(isset($_POST['delete_confirmed'])) $action['action'] = "removeConfirmed";   

    // Detect Snapshot actions
    if(isset($_POST['CreateSnapshot'])) $action['action'] = "saveSnapshot";   
    if(isset($_POST['CancelSnapshot'])) $action['action'] = "cancelSnapshot";   
    foreach($_POST as $name => $value){
      $once =TRUE;
      if(preg_match("/^RestoreSnapShot_/",$name) && $once){
        $once = FALSE;
        $entry = base64_decode(preg_replace("/^RestoreSnapShot_([^_]*)_[xy]$/i","\\1",$name));
        $action['action'] = "restoreSnapshot";
        $action['targets'] = array($entry);
      }
    }

    return($action);
  }

  function handleActions($action)
  {
    // Start action  
    if(isset($this->actions[$action['action']])){
      $func = $this->actions[$action['action']];
      if(!isset($action['targets']))$action['targets']= array(); 
      return($this->$func($action['action'],$action['targets'],$action));
    }
  } 

  function createSnapshotDialog($action="",$target=array(),$all=array())
  {
    foreach($target as $entry){
      if(!empty($entry) && $this->ui->allow_snapshot_create($entry,$this->aclCategory)){
        $this->dialogObject = new SnapShotDialog($this->config,$entry,$this);
        $this->dialogObject->aclCategories = array($this->aclCategory);
        
      }else{
        msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to create a snapshot for %s."),$entry),
            ERROR_DIALOG);
      }
    }
  }


  /*! \brief  Creates a snapshot new entry - This method is called when the somebody
   *           clicks 'save' in the "Create snapshot dialog" (see ::createSnapshotDialog).
   *      
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function saveSnapsho($action="",$target=array(),$all=array())
  {
    $this->dialogObject->save_object();
    $msgs = $this->dialogObject->check();
    if(count($msgs)){
      foreach($msgs as $msg){
        msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
      }
    }else{
      $this->dn =  $this->dialogObject->dn;
      $this->snapHandler->create_snapshot( $this->dn,$this->dialogObject->CurrentDescription);
      $this->closeDialogs();
    }
  }


  /*! \brief  Restores a snapshot object.
   *          The dn of the snapshot entry has to be given as ['target'] parameter.  
   *      
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function restoreSnapshot($action="",$target=array(),$all=array())
  {
    $entry = array_pop($target);
    if(!empty($entry) && $this->ui->allow_snapshot_restore($entry,$this->aclCategory)){
      $this->snapHandler->restore_snapshot($entry);
      $this->closeDialogs();
    }else{
      msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to restore a snapshot for %s."),$entry),
          ERROR_DIALOG);
    }
  }


  /*! \brief  Displays the "Restore snapshot dialog" for a given target. 
   *          If no target is specified, open the restore removed object 
   *           dialog.
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function restoreSnapshotDialog($action="",$target=array(),$all=array())
  {
    // Set current restore base for snapshot handling.
    if(is_object($this->snapHandler)){
      $bases = array();
      foreach($this->storagePoints as $sp){
        $bases[] = $sp.$this->headpage->getBase();
      }
    }

    // No target, open the restore removed object dialog.
    if(!count($target)){ 
      $entry = $this->headpage->getBase();
      if(!empty($entry) && $this->ui->allow_snapshot_restore($entry,$this->aclCategory)){
        $this->dialogObject = new SnapShotDialog($this->config,$entry,$this);
        $this->dialogObject->set_snapshot_bases($bases);
        $this->dialogObject->display_all_removed_objects = true;
        $this->dialogObject->display_restore_dialog = true;
      }else{
        msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to restore a snapshot for %s."),$entry),
            ERROR_DIALOG);
      } 
    }else{

      // Display the restore points for a given object.
      $entry = array_pop($target);
      if(!empty($entry) && $this->ui->allow_snapshot_restore($entry,$this->aclCategory)){
        $this->dialogObject = new SnapShotDialog($this->config,$entry,$this);
        $this->dialogObject->set_snapshot_bases($bases);
        $this->dialogObject->display_restore_dialog = true;
      }else{
        msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to restore a snapshot for %s."),$entry),
            ERROR_DIALOG);
      } 
    }
  }


  /*! \brief  This method intiates the object creation.
   *          
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory)
  {
    /* To handle mutliple object types overload this method.
     * ...
     *   registerAction('newUser', 'newEntry');
     *   registerAction('newGroup','newEntry');
     * ... 
     * 
     * function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory)
     * {
     *   switch($action){
     *     case 'newUser' : {
     *       mangement::newEntry($action,$target,$all,"usertabs","USERTABS","users");
     *     }
     *     case 'newGroup' : {
     *       mangement::newEntry($action,$target,$all,"grouptabs","GROUPTABS","groups");
     *     }
     *   }
     * }
     **/ 
    $tabType = $this->tabType;
    $tabClass = $this->tabClass;
    $aclCategory = $this->aclCategory;
    if(!empty($altTabClass)) $tabClass = $altTabClass;
    if(!empty($altTabType)) $tabType = $altTabType;
    if(!empty($altAclCategory)) $aclCategory = $altAclCategory;
  {
    // Check locking & lock entry if required 
    $this->displayApplyBtn = FALSE;
    $this->dn = "new";
    $this->is_new = TRUE;
    $this->is_single_edit = FALSE;
    $this->is_multiple_edit = FALSE;

    set_object_info($this->dn);

    // Open object.
    if(empty($tabClass) || empty($tabType)){
      // No tab type defined
    }else{
      $this->tabObject= new $tabClass($this->config,$this->config->data['TABS'][$tabType], $this->dn, $aclCategory);
      $this->tabObject->set_acl_base($this->headpage->getBase());
    }
  }


  /*! \brief  This method opens an existing object or a list of existing objects to be edited. 
   *                  
   * 
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory)
  {
    /* To handle mutliple object types overload this method.
     * ...
     *   registerAction('editUser', 'editEntry');
     *   registerAction('editGroup','editEntry');
     * ... 
     * 
     * function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory)
     * {
     *   switch($action){
     *     case 'editUser' : {
     *       mangement::editEntry($action,$target,$all,"usertabs","USERTABS","users");
     *     }
     *     case 'editGroup' : {
     *       mangement::editEntry($action,$target,$all,"grouptabs","GROUPTABS","groups");
     *     }
     *   }
     * }
     **/ 
    $tabType = $this->tabType;
    $tabClass = $this->tabClass;
    $aclCategory = $this->aclCategory;
    if(!empty($altTabClass)) $tabClass = $altTabClass;
    if(!empty($altTabType)) $tabType = $altTabType;
    if(!empty($altAclCategory)) $aclCategory = $altAclCategory;

    // Single edit - we only got one object dn.
    if(count($target) == 1){
      $this->displayApplyBtn = TRUE;
      $this->is_new = FALSE;
      $this->is_single_edit = TRUE;
      $this->is_multiple_edit = FALSE;

      // Get the dn of the object and creates lock
      $this->dn = array_pop($target);
      set_object_info($this->dn);
      $user = get_lock($this->dn);
      if ($user != ""){
        return(gen_locked_message ($user, $this->dn,TRUE));
      }
      add_lock ($this->dn, $this->ui->dn);

      // Open object.
      if(empty($tabClass) || empty($tabType)){
        trigger_error("We can't edit any object(s). 'tabClass' or 'tabType' is empty!");
      }else{
        $tab = $tabClass;
        $this->tabObject= new $tab($this->config,$this->config->data['TABS'][$tabType], $this->dn,$aclCategory);
        $this->tabObject->set_acl_base($this->dn);
      }
    }else{

      // We've multiple entries to edit.
      $this->is_new = FALSE;
      $this->is_singel_edit = FALSE;
      $this->is_multiple_edit = TRUE;

      // Open multiple edit handler.
      if(empty($tabClass) || empty($tabType)){
        trigger_error("We can't edit any object(s). 'tabClass' or 'tabType' is empty!");
      }else{
        $this->dns = $target;
        $tmp = new multi_plug($this->config,$tabClass,$this->config->data['TABS'][$tabType],
            $this->dns,$this->headpage->getBase(),$aclCategory);

        // Check for locked entries
        if ($tmp->entries_locked()){
          return($tmp->display_lock_message());
        }
        
        // Now lock entries.
        $tmp->lock_entries($this->ui->dn);
        if($tmp->multiple_available()){
          $this->tabObject = $tmp;
          set_object_info($this->tabObject->get_object_info());
        }
      }
    }
  }


  /*! \brief  Save object modifications and closes dialogs (returns to object listing).
   *          - Calls '::check' to validate the given input.
   *          - Calls '::save' to save back object modifications (e.g. to ldap).
   *          - Calls '::remove_locks' to remove eventually created locks.
   *          - Calls '::closeDialogs' to return to the object listing.
   */
  protected function saveChanges()
  {
    if($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug){
      $this->tabObject->save_object();
      $msgs = $this->tabObject->check();
      if(count($msgs)){
        msg_dialog::displayChecks($msgs); 
        return("");
      }else{
        $this->tabObject->save();
        $this->remove_lock();
        $this->closeDialogs();
      }
    }
  }


  /*! \brief  Save object modifications and keep dialogs opened. 
   *          - Calls '::check' to validate the given input.
   *          - Calls '::save' to save back object modifications (e.g. to ldap).
   */
  protected function applyChanges()
  {
  if($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug){
    $this->tabObject->save_object();
      $msgs = $this->tabObject->check();
      if(count($msgs)){
        msg_dialog::displayChecks($msgs); 
        return("");
      }else{
        $this->tabObject->save();
        $this->tabObject->re_init();
      }
    }
  }

  
  /*! \brief  This method closes dialogs
   *           and cleans up the cached object info and the ui.
   */
  protected function closeDialogs()
  {
    $this->last_dn = $this->dn;
    $this->last_dns = $this->dns;
    $this->last_tabObject = $this->tabObject;
    $this->last_dialogObject = $this->dialogObject;
    $this->dn = "";
    $this->dns = array();
    $this->tabObject = null;
    $this->dialogObject = null;
    set_object_info();
  }


  /*! \brief  Editing an object was caneled. 
   *          Close dialogs/tabs and remove locks.
   */
  protected function cancelEdit()
  {
    $this->remove_lock();
    $this->closeDialogs();
  }

 
  /*! \brief  Every click in the list user interface sends an event
   *           here can we connect those events to a method. 
   *          eg.  ::registerEvent('new','createUser')
   *          When the action/event new is send, the method 'createUser' 
   *           will be called.
   */
  function registerAction($action,$target)
  {
    $this->actions[$action] = $target;
  }

  
  /*! \brief  Removes ldap object locks created by this class.
   *          Whenever an object is edited, we create locks to avoid 
   *           concurrent modifications.
   *          This locks will automatically removed here.
   */
  function remove_lock()
  {
    if(!empty($this->dn) && $this->dn != "new"){
      del_lock($this->dn);
    }
    if(count($this->dns)){
      del_lock($this->dns);
    }
  }


  /*! \brief  This method is used to queue and process copy&paste actions. 
   *          Allows to copy, cut and paste mutliple entries at once.
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function copyPasteHandler($action="",$target=array(),$all=array())
  {
    // Return without any actions while copy&paste handler is disabled.
    if(!is_object($this->cpHandler))  return("");

    // Save user input
    $this->cpHandler->save_object();

    // Add entries to queue 
    if($s_action == "copy" || $s_action == "cut"){
      $this->cpHandler->cleanup_queue();
      foreach($s_entry as $dn){
        if($s_action == "copy" && $this->ui->is_copyable($dn,$this->aclCategory,$this->aclPlugin)){
          $this->cpHandler->add_to_queue($dn,"copy",$this->tabClass,$this->tabType,$this->aclCategory);
        }
        if($s_action == "cut" && $this->ui->is_cutable($dn,$this->aclCategory,$this->aclPlugin)){
          $this->cpHandler->add_to_queue($dn,"cut",$this->tabClass,$this->tabType,$this->aclCategory);
        }
      }
    }

    // Initiate pasting
    if($s_action == "paste"){
      $this->cpPastingStarted = TRUE;
    }

    // Display any c&p dialogs, eg. object modifications required before pasting.
    if($this->cpPastingStarted && $this->cpHandler->entries_queued()){
      $this->cpHandler->SetVar("base",$this->headpage->getBase());
      $data = $this->cpHandler->execute();
      if(!empty($data)){
        return($data);
      }
    }

    // Automatically disable pasting process since there is no entry left to paste.
    if(!$this->cpHandler->entries_queued()){
      $this->cpPastingStarted = FALSE;
    }
    return("");
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
