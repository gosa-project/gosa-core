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

  var $sort_by  = "Schedule";
  var $sort_dir = "down";

  var $range    = 25;
  var $start    = 0;

  function gotomasses(&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $this->o_queue = new gosaSupportDaemon(TRUE,10);
  }


  function execute()
  {
    $smarty = get_smarty();

    /************
     * Handle posts 
     ************/
    
    $s_entry = $s_action = "";
    $arr = array( 
        "/^stop_/"      => "stop",
        "/^stop_all/"  => "stop_all",
        "/^start_/"      => "start",
        "/^start_all/"  => "start_all",

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

    if(preg_match("/^start/",$s_action) || preg_match("/^stop/",$s_action)){

      switch($s_action){
        case 'start_all'   : $this->start_queue_entries($this->list_get_selected_items());break;
        case 'start'       : $this->start_queue_entries(array($s_entry));break; 
        case 'stop_all'    : $this->stop_queue_entries ($this->list_get_selected_items());break;
        case 'stop'        : $this->stop_queue_entries (array($s_entry));break; 
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
        if(count($ids)){
          $this->ids_to_remove = $ids;
          $ret = $this->o_queue->ids_exist($this->ids_to_remove);
          $ret = $this->o_queue->get_entries_by_id($ret);

          $tmp = "";
          foreach($ret as $task){
            $tmp.= "\n".$task['ID']." - ".$task['HEADERTAG']."&nbsp;".$task['MACADDRESS'];
          }
          $smarty->assign("multiple", TRUE); 
          $smarty->assign("info",sprintf(_("Your are about to delete the following tasks: %s"),"<pre>".$tmp."</pre>"));
          $this->current = $s_entry;
          return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
        }
      }
    }

    /* Remove specified tasks */
    if(count($this->ids_to_remove) && isset($_POST['delete_multiple_confirm'])){
      $this->o_queue->remove_entries($this->ids_to_remove);
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
      $entry = $this->o_queue->get_entry_by_id($s_entry);
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
          $this->o_queue->update_entries(array($this->current),$this->dialog->save());
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

    $divlist = new MultiSelectWindow($this->config,"gotoMasses",array("gotomasses"));
    $divlist->SetInformation(_("This menu allows you to remove and change the properties of GOsa deamon tasks."));
    $divlist->SetSummary(_("List of queued deamon jobs."));
    $divlist->EnableCloseButton(FALSE);
    $divlist->EnableSaveButton(FALSE);
    $divlist->SetHeadpageMode();

    if($this->sort_dir == "up"){
      $sort_img = "<img src='images/sort_up.png' alt='/\' border=0>";
    }else{
      $sort_img = "<img src='images/sort_down.png' alt='\/' border=0>";
    }

    if($this->sort_by == "TargetName"){ $sort_img_1 = $sort_img; } else { $sort_img_1 = "" ;}
    if($this->sort_by == "TaskID"){ $sort_img_2 = $sort_img; } else { $sort_img_2 = "" ;}
    if($this->sort_by == "Schedule"){ $sort_img_3 = $sort_img; } else { $sort_img_3 = "" ;}
    if($this->sort_by == "Action"){ $sort_img_4 = $sort_img; } else { $sort_img_4 = "" ;}

    /* Display add button if allowed */
    $header = "<div style='padding:5px'>";
    if($this->acl_is_removeable()){
      $header .= "<input type='image' src='images/edittrash.png' name='remove_multiple_task' class='center'>&nbsp;";
    }
    if(preg_match("/w/",$this->getacl(""))){
      $header .= "<input type='image' src='images/status_stop_all.png' name='stop_all' class='center'>&nbsp;";
      $header .= "<input type='image' src='images/status_start_all.png' name='start_all' class='center'>&nbsp;";
    }
    $header .= "</div>";

    /* Create divlist */
    $divlist->SetListHeader($header);

    $plug  = $_GET['plug'];
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $divlist->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=TargetName'>"._("Target").$sort_img_1."</a>&nbsp;/&nbsp;".
                                     "<a href='?plug=".$plug."&amp;sort=TaskID'>"._("Task").$sort_img_2."</a>"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=Schedule'>"._("Schedule").$sort_img_3."</a>",
                                      "attach"=>"style='width:100px;'"));
    $divlist->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=Action'>"._("Status").$sort_img_4."</a>",
                                      "attach"=>"style='width:80px;'"));
    $divlist->AddHeader(array("string"=>_("Action"),
                                      "attach"=>"style='border-right:0px;width:120px;'"));


    $entries = $this->get_queued_entries();

    foreach($entries as $key => $task){

      $prio_actions = "<input class='center' type='image' src='images/status_stop_all.png' name='stop_".$task['ID']."'>&nbsp;";
      $prio_actions.= "<input class='center' type='image' src='images/status_start_all.png' name='start_".$task['ID']."'>&nbsp;";

      $action = "<input type='image' src='images/edit.png' name='edit_task_".$task['ID']."' 
        class='center' alt='"._("Edit")."'>";

      if($this->acl_is_removeable()){
        $action.= "<input type='image' src='images/edittrash.png' name='remove_task_".$task['ID']."' 
          class='center' alt='"._("Reomve")."'>";
      }

      $color = "";
      $display = $task['MACADDRESS'];
      $display2= $task['HEADERTAG'];

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$task['ID']."' name='item_selected_".$task['ID']."'>" ,
                      "attach" => "style='width:20px;".$color."'");
      $field1 = array("string" => $display,
                      "attach" => "style='".$color."'");
      $field1a= array("string" => $display2,
                      "attach" => "style='".$color.";width:80px;'");
      $field2 = array("string" => date("d.m.Y H:i:s",strtotime($task['TIMESTAMP'])),"attach" => "style='".$color.";width:100px;'");
      $field3 = array("string" => $task['STATUS'],"attach" => "style='".$color.";width:80px;'");
      $field4 = array("string" => $prio_actions.$action,"attach" => "style='".$color.";text-align:right;width:120px;border-right:0px;'");
      $divlist->AddElement(array($field0,$field1,$field1a,$field2,$field3,$field4));
    }


    $smarty = get_smarty();
    $smarty->assign("start",$this->start);
    $smarty->assign("start_real", ($this->start + 1));
    $smarty->assign("ranges", array("10" => "10",
                                    "20" => "20",
                                    "25" => "25",
                                    "50" => "50",
                                    "100"=> "100",
                                    "200"=> "200",
                                    "9999" => "*"));

    $count = $this->o_queue->number_of_queued_entries();
    $smarty->assign("range_selector", range_selector($count, $this->start, $this->range,"range"));
    $smarty->assign("range",$this->range);
    $smarty->assign("div",$divlist->Draw());
    return($smarty->fetch (get_template_path('gotomasses.tpl', TRUE, dirname(__FILE__))));
  }


  /*! \brief  Force queue job to be done as far as possible.
   *  @return Boolean TRUE in case of success, else FALSE. 
   */
  private function start_queue_entries($ids)
  {
    if(!count($ids)){
      return;
    }
  
    $data = array("timestamp" => date("YmdHis"),
                  "status"    => "-");
    if(!$this->o_queue->update_entries($ids,$data)){
      msg_dialog::display(_("Error"), sprintf(_("Could not update queue entry: %s"),$id) , ERROR_DIALOG);
      return(FALSE);
    }
    return(TRUE);
  }


  /*! \brief Stops the specified queue entry from execution.
   *  @return Boolean TRUE in case of success, else FALSE. 
   */
  private function stop_queue_entries($ids)
  {
    if(!count($ids)){
      return;
    }
    $data = array("timestamp" => "20490101010101",
                  "status"    => "Stoppped");
    if(!$this->o_queue->update_entries($ids,$data)){
      msg_dialog::display(_("Error"), sprintf(_("Could not update queue entry: %s"),$id) , ERROR_DIALOG);
      return(FALSE);
    }
    return(TRUE);
  }


  /*! \brief  Request list of queued jobs.
   *  @return Returns an array of all queued jobs.
   */
  function get_queued_entries()
  {
    $map = array(
        "QueuePosition" => "id",
        "Action"        => "status",
        "TaskID"        => "headertag",
        "TargetName"    => "macaddress",
        "Schedule"      => "timestamp");

    if(!isset($map[$this->sort_by])){
      $sort = "id DESC";
    }else{
      $sort   = $map[$this->sort_by]; 
      if($this->sort_dir == "up"){
        $sort.= " ASC";
      }else{
        $sort.= " DESC";
      }
    }
      
    $start  = $this->start; 
    $stop   = $this->range;
    $entries = $this->o_queue->get_queued_entries($start,$stop,$sort);
    if(!is_array($entries) || !isset($entries['XML']) || !is_array($entries['XML'])){
      if ($this->o_queue->get_error()){
        msg_dialog::display(_("Error"), sprintf(_("Cannot load queue entries: %s"), "<br><br>".$this->o_queue->get_error()), ERROR_DIALOG);
      }
      return(array());
    }

    $ret = array();
    foreach($entries['XML'] as $entry){
      $task = $entry['ID']; 
      $ret[]= $entry;
    }
    return($ret);
  }


  /*! \brief  Handle post jobs, like sorting.
   */
  function save_object()
  {
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
    if(isset($_POST['range']) && is_numeric($_POST['range'])){
      $this->range = $_POST['range'];
    }
    if(isset($_GET['start'])){
      $start = $_GET['start'];
      if(is_numeric($start) || $start == 0){
        $this->start = $start;
      }
    }

    /* Check start stop and reset if necessary */
    $count = $this->o_queue->number_of_queued_entries();
    if($this->start >= $count){
      $this->start = $count -1;
    }
    if($this->start < 0){
      $this->start = 0;
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
