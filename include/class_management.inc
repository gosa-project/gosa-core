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

  protected $tabClass = "";
  protected $tabType = "";
  protected $aclCategory = "";
  protected $objectName = "";

  protected $displayApplyBtn = "";
  protected $cpHandler = null;
  protected $cpPastingStarted = FALSE;
 
  // Private
  private $plugname = "";
  private $headpage = null;
  private $filter = null;
  private $actions = array();
  private $tabObject = null;
  
  function  __construct($config,$ui,$plugname, $headpage)
  {
    $this->plugname = $plugname;
    $this->headpage = $headpage;
    $this->ui = $ui;
    $this->config = $config;

    if($this->cpHandler) $this->headpage->setCopyPasteHandler($this->cpHandler);

    if(empty($this->plIcon)){
      $this->plIcon = "plugins/".$plugname."/images/plugin.png";
    }
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

    // Handle actions (POSTs and GETs)
    $str = $this->detectPostActions();
    if($str) return($this->getHeader().$str);

    // Display tab object.
    if($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug){
      $this->tabObject->save_object();
      $display = $this->tabObject->execute();
      $display.= $this->_getTabFooter();
      return($this->getHeader().$display);
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
        $this->closeDialogs();
        del_lock($this->dn);        
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), ERROR_DIALOG);
        new log("security","groups/".get_class($this),$dn,array(),"Tried to trick deletion.");
        return;
      }
    }

    $this->closeDialogs();
    $this->remove_lock();
  }


  function detectPostActions()
  {
    $action= $this->headpage->getAction();

    if(isset($_POST['edit_apply']))  $action['action'] = "apply";    
    if(isset($_POST['edit_finish'])) $action['action'] = "save";    
    if(isset($_POST['edit_cancel'])) $action['action'] = "cancel";    
    if(isset($_POST['delete_confirmed'])) $action['action'] = "removeConfirmed";    

    // Start action  
    if(isset($this->actions[$action['action']])){
      $func = $this->actions[$action['action']];
      if(!isset($action['targets']))$action['targets']= array(); 
      return($this->$func($action['action'],$action['targets'],$action));
    }
  } 

  function newEntry($action,$target,$all)
  {
    // Check locking & lock entry if required 
    $this->displayApplyBtn = FALSE;
    $this->dn = "new";
    $this->is_new = TRUE;
    $this->is_single_edit = FALSE;
    $this->is_multiple_edit = FALSE;

    set_object_info($this->dn);

    // Open object.
    if(empty($this->tabClass) || empty($this->tabType)){
      // No tab type defined
    }else{
      $tab = $this->tabClass;
      $this->tabObject= new $tab($this->config,$this->config->data['TABS'][$this->tabType], $this->dn, $this->aclCategory);
      $this->tabObject->set_acl_base($this->headpage->getBase());
    }
  }

  function editEntry($action,$target,$all)
  {
    if(count($target) == 0){
      //nothing 
    }elseif(count($target) == 1){

      // Check locking & lock entry if required 
      $this->displayApplyBtn = TRUE;

      $this->is_new = FALSE;
      $this->is_single_edit = TRUE;
      $this->is_multiple_edit = FALSE;

      $this->dn = array_pop($target);
      set_object_info($this->dn);
      $user = get_lock($this->dn);
      if ($user != ""){
        return(gen_locked_message ($user, $this->dn,TRUE));
      }
      add_lock ($this->dn, $this->ui->dn);

      // Open object.
      if(empty($this->tabClass) || empty($this->tabType)){
        // No tab type defined
      }else{
        $tab = $this->tabClass;
        $this->tabObject= new $tab($this->config,$this->config->data['TABS'][$this->tabType], $this->dn,$this->aclCategory);
        $this->tabObject->set_acl_base($this->dn);
      }
    }else{

      $this->is_new = FALSE;
      $this->is_singel_edit = FALSE;
      $this->is_multiple_edit = TRUE;

      $this->dns = $target;
      $tmp = new multi_plug($this->config,$this->tabClass,$this->config->data['TABS'][$this->tabType],
            $this->dns,$this->headpage->getBase(),$this->aclCategory);
      if ($tmp->entries_locked()){
        return($tmp->display_lock_message());
      }
      $tmp->lock_entries($this->ui->dn);
      if($tmp->multiple_available()){
        $this->tabObject = $tmp;
        set_object_info($this->tabObject->get_object_info());
      }

    }
  }

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
        $this->closeDialogs();
        $this->remove_lock();
      }
    }
  }

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
  
  protected function closeDialogs()
  {
    $this->dn = "";
    $this->dns = array();
    $this->tabObject = null;
    set_object_info();
  }

  protected function cancelEdit()
  {
    $this->closeDialogs();
    $this->remove_lock();
  }


 

  function registerAction($action,$target)
  {
    $this->actions[$action] = $target;
  }

  function remove_lock()
  {
    if(!empty($this->dn) && $this->dn != "new"){
      del_lock($this->dn);
    }
    if(count($this->dns)){
      del_lock($this->dns);
    }
  }

  function copyPasteHandler($s_action,$s_entry)
  {

    /* Check if Copy & Paste is disabled */
    if(!is_object($this->cpHandler)){
      return("");
    }

    $ui = get_userinfo();

    /* Add entries to queue */
    if($s_action == "copy" || $s_action == "cut"){

      /* Cleanup object queue */
      $this->cpHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($s_entry as $dn){
        if($s_action == "copy" && $ui->is_copyable($dn,$this->aclCategory,$this->aclPlugin)){
          $this->cpHandler->add_to_queue($dn,"copy",$this->tabClass,$this->tabType,$this->aclCategory);
        }
        if($s_action == "cut" && $ui->is_cutable($dn,$this->aclCategory,$this->aclPlugin)){
          $this->cpHandler->add_to_queue($dn,"cut",$this->tabClass,$this->tabType,$this->aclCategory);
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "paste"){
      $this->cpPastingStarted = TRUE;
    }

    /* Return C&P dialog */
    if($this->cpPastingStarted && $this->cpHandler->entries_queued()){

      /* Get dialog */
      $this->cpHandler->SetVar("base",$this->headpage->getBase());
      $data = $this->cpHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->cpHandler->entries_queued()){
      $this->cpPastingStarted = FALSE;
    }
    return("");
  }


  function save_object(){}
  function setDescription($str) {
    $this->plDescription = $str;
  } 
  function setHeadpage($str) {
    $this->headpage = $str;
  } 
  function setFilter($str) {
    $this->filter = $str;
  } 
  function setIcon($str) {
    $this->plIcon = $str;
  } 
  function setHeadline($str) {
    $this->plHeadline = $str;
  } 
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
