<?php

class gotomasses extends plugin
{
  /* Definitions */
  var $plHeadline     = "System deployment";
  var $plDescription  = "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Queue tasks */
  var $current        = FALSE;
  var $dialog         = FALSE;
  var $ids_to_remove  = array();
  var $divlist        = NULL;

  var $sort_by  = "QueuePosition";
  var $sort_dir = "up";

  function gotomasses(&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $this->divlist = new divListMasses($this->config,$this);
    $this->o_queue = new gosaSupportDaemon("10.3.67.111","20081","secret-gosa-password",TRUE,0.2);
  }


  function execute()
  {
    $smarty = get_smarty();

    /************
     * Handle posts 
     ************/

    $s_entry = $s_action = "";
    $arr = array( 
        "/^prio_top_/"      => "prio_top",
        "/^prio_increase_/" => "prio_increase",
        "/^prio_decrease_/" => "prio_decrease",
        "/^prio_bottom_/"   => "prio_bottom",

        "/^multiple_prio_top_/"      => "mprio_top",
        "/^multiple_prio_increase_/" => "mprio_increase",
        "/^multiple_prio_decrease_/" => "mprio_decrease",
        "/^multiple_prio_bottom_/"   => "mprio_bottom",

        "/^edit_task_/"             =>  "edit",
        "/^remove_task_/"           =>  "remove",
        "/^new_task_/"              =>  "new_task",
        "/^remove_multiple_task_/"  => "remove_multiple");

    foreach($arr as $regex => $action){
      foreach($_POST as $name => $value){
        if(preg_match($regex,$name)){
          $s_action = $action;
          $s_entry  = preg_replace($regex,"",$name);
          $s_entry  = preg_replace("/_(x|y)$/","",$s_entry);
        }
      }
    }

    /* Edit posted from list link */
    if(isset($_GET['act']) && $_GET['act'] == "edit" && isset($_GET['id']) && isset($this->tasks[$_GET['id']])){
      $s_action = "edit";
      $s_entry = $_GET['id'];
    }


    /************
     * Handle Priority modifications  
     ************/

    if(preg_match("/^mprio_/",$s_action) || preg_match("/^prio_/",$s_action)){

      switch($s_action){
        case 'mprio_top'       : foreach($this->list_get_selected_items() as $id ){
                                   $this->o_queue->max_entry_priority($id);
                                 }
                                 break;
        case 'mprio_increase'  : foreach($this->list_get_selected_items() as $id ){
                                   $this->o_queue->increase_entry_priority($id);
                                 }
                                 break;
        case 'mprio_decrease'  : foreach($this->list_get_selected_items() as $id ){
                                   $this->o_queue->decrease_entry_priority($id);
                                 }
                                 break;
        case 'mprio_bottom'    : foreach($this->list_get_selected_items() as $id ){
                                   $this->o_queue->min_entry_priority($id);
                                 }
                                 break;
        case 'prio_top'       : $this->o_queue->max_entry_priority($s_entry);break;
        case 'prio_increase'  : $this->o_queue->increase_entry_priority($s_entry);break;
        case 'prio_decrease'  : $this->o_queue->decrease_entry_priority($s_entry);break;
        case 'prio_bottom'    : $this->o_queue->min_entry_priority($s_entry);break;
        default : trigger_error("Undefined priority setting used.");
      }
      if($this->o_queue->is_error()){
        msg_dialog::display(_("Error"), $this->o_queue->get_error(), ERROR_DIALOG);
      }
    }

    /************
     * REMOVE 
     ************/

    /* Remove multiple */
    if($s_action == "remove_multiple" || $s_action == "remove"){

      if(!$this->acl_is_removeable()){
        msg_dialog::display(_("Permission error"), _("You have no permission to delete this entry!"), ERROR_DIALOG);
      }else{

        if($s_action == "remove"){
          $ids = array($s_entry);
        }else{
          $ids = $this->list_get_selected_items();
        }

        $this->ids_to_remove = $ids;
        $tmp = "";
        foreach($this->ids_to_remove as $key => $id){
          if($this->o_queue->id_exists($id)){
            $task = $this->o_queue->get_entry($id);
            $tmp.= "\n".$task['5']."&nbsp;".$task['3'];
          }else{
            unset($this->ids_to_remove[$key]);
          }
        }
        $smarty->assign("multiple", TRUE); 
        $smarty->assign("info",sprintf(_("Your are about to delete the following tasks: %s"),"<pre>".$tmp."</pre>"));
        $this->current = $s_entry;
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }

    /* Remove specified tasks */
    if(count($this->ids_to_remove) && isset($_POST['delete_multiple_confirm'])){
      foreach($this->ids_to_remove as $id){
        $this->o_queue->remove_entry($id);
      }
      $this->save();
    }

    /* Remove aborted */
    if(isset($_POST['delete_cancel'])){
      $this->ids_to_remove = array();;
    }


    /************
     * EDIT 
     ************/

    /* Edit selected entry */
    if($s_action == "edit"){
      $entry = $this->o_queue->get_entry($s_entry);
      if($entry){
        $this->dialog = new goto_task($this->config,$this,$entry);
        $this->current = $s_entry;
      }
    }

    /* New entry */
    if($s_action== "new_task" && $this->acl_is_createable()){
      $this->dialog = new goto_task($this->config,$this);
      $this->current = -1;
    }

    /* Close dialog */
    if(isset($_POST['close_goto_task'])){
      $this->dialog = FALSE;
      $this->current = -1;
    }

    /* Close dialog */
    if((isset($_POST['save_goto_task']) || isset($_POST['apply_goto_task'])) && is_object($this->dialog) ){
      $this->dialog->save_object();
      $msgs = $this->dialog->check();
      if(count($msgs)){
        foreach($msgs as $msg){
          msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
        }
      }else{  

        if($this->o_queue->id_exists($this->current)){
          $this->o_queue->update_entry($this->current,$this->dialog->save());
        }else{
          $tmp = $this->dialog->save();
          $tmp2= array();
          $targets =$tmp['Target'];
          foreach($targets as $target){
            $tmp['Target'] = array($target);
            $tmp2[] = $tmp;
          }
          if(!$this->o_queue->add_multiple($tmp2)){
            msg_dialog::display(_("Error"), $this->o_queue->get_error(), ERROR_DIALOG);
          }
        }
        if(!isset($_POST['apply_goto_task'])){
          $this->dialog = FALSE;
          $this->current = -1;
        }
        $this->save();
      }
    }

    /* Display dialogs if currently opened */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    /************
     * Handle Divlist 
     ************/

    $this->divlist->execute();
    $entries = $this->get_queue_entries();
    $this->divlist->SetEntries($entries);
    return($this->divlist->Draw());
  }


  /*! \brief  Request list of queued jobs.
   *  @return Returns an array of all queued jobs.
   */
  function get_queue_entries()
  {
    if(!$this->o_queue->load()){
      msg_dialog::display(_("Error"), sprintf(_("Cannot load queue entries: %s"), "<br><br>".$this->o_queue->get_error()), ERROR_DIALOG);
      return(array());
    }
    $tasks = array();
    $ret = array();
    while($entry = $this->o_queue->fetch()){
      $task = $entry['ID']; 
      $ret[]= $entry;
    }
    $map = array("QueuePosition" => "ID",
        "Action"        => "STATUS",
        "TaskID"        => "HEADERTAG",
        "TargetName"    => "MACADDRESS",
        "Schedule"      => "TIMESTAMP");
    $sort_tmp = array();
    foreach($ret as $entry_id => $entry){
      $sort_tmp[$entry_id] = $entry[$map[$this->sort_by]];
    } 
    natcasesort($sort_tmp);
    $return = array();
    foreach($sort_tmp as $entry_id => $value){
      $return[] = $ret[$entry_id];
    } 
    if($this->sort_dir != "up"){
      $return = array_reverse($return);
    } 
    return($return);
  }


  /*! \brief  Handle post jobs, like sorting.
   */
  function save_object()
  {
    $this->divlist->save_object();
    $sort_vals = array("Action","QueuePosition","TargetName","Schedule","TaskID");
    if(isset($_GET['sort']) && in_array($_GET['sort'],$sort_vals)){
      $sort = $_GET['sort'];
      if($this->sort_by == $sort){
        if($this->sort_dir == "up"){
          $this->sort_dir = "down";
        }else{
          $this->sort_dir = "up";
        }
      }
      $this->sort_by = $sort;
    }
  }


  /* Return list of object groups */
  function get_object_groups()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
    while($attrs = $ldap->fetch()){
      $ret[$attrs['cn'][0]] = $attrs['cn'][0];
    }
    return($ret); 
  }


  function save()
  {
    // We do not save anything here.
  }


  /*! \brief  Return a list of all selected items.
    @return Array   Returns an array containing all selected item ids.
   */
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


  function get_actions()
  {
    /* Prepare list of available actions */
    $actions = array(      
        "gosa_ping"       => _("GOsa ping"),
        "ping"            => _("Ping"),
        "sayHello"        => _("Say hello"));
    return($actions);
  }


  static function plInfo()
  {
    return (array(
          "plShortName"   => _("System mass deployment"),
          "plDescription" => _("Provide a mechanism to automatically activate a set of systems"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("addon"),
          "plCategory"    => array("gotomasses" => array("objectClass" => "none", "description" => _("System mass deployment"))),
          "plProvidedAcls" => array("Comment"   => _("Description"), 
            "Action"    => _("Action"),
            "Day"       => _("Day"),
            "Minute"    => _("Minute"),
            "Hour"      => _("Hour"),
            "Month"     => _("Month"),
            "Weekday"   => _("Week day"),
            "Target"    => _("Target"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
