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
  var $current =false;
  var $dialog = FALSE;
  var $ids_to_remove = array();
  var $divlist = NULL;

  var $sort_by = "QueuePosition";
  var $sort_dir= "up";

  function gotomasses(&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $this->divlist = new divListMasses($this->config,$this);
    $this->o_queue = new hostActionQueue(&$config);
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

                  "/^edit_task_/"=>"edit","/^remove_task_/"=>"remove",
                  "/^new_task_/"=>"new_task","/^remove_multiple_task_/" => "remove_multiple");
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
     * REMOVE 
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
        print_red($this->o_queue->get_error());
      }
    }

    /************
     * REMOVE 
     ************/

    /* Remove multiple */
    if($s_action == "remove_multiple" || $s_action == "remove"){

      if(!$this->acl_is_removeable()){
        print_red(_("You are not allowed to remove a task."));
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
            $tmp.= "\n".$this->target_to_string($task);
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
        if($this->o_queue->id_exists($id)){
          $this->o_queue->remove_entry($id);
        }
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
          print_red($msg);  
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
            print_red($this->o_queue->get_error());
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
    $this->divlist->SetEntries($this->get_queue_entries()); 
    $str = 
      "<script type=\"text/javascript\">
      <!--
        var time = null
        function move() {
          window.location = '?plug=".$_GET['plug']."'
        }
        setInterval(\"move()\",20000);
        //-->
        </script>";
    return($str.$this->divlist->Draw());
  }

    
  function get_queue_entries()
  {
    if(!$this->o_queue->load()){
      print_red("ERROR:".$this->o_queue->get_error());
      return(array());
    }

    $queue_pos = 1;
    $tasks = array();
    $ret = array();
    while($entry = $this->o_queue->fetch()){
      $task = $entry['TASK_ID']; 
      if(!isset($tasks[$task])){
        $tasks[$task] = dechex(rand(100,240)).dechex(rand(100,240)).dechex(rand(100,240));
      }
      $entry['Queue_Position'] = $queue_pos;
      $entry['color'] = $tasks[$task]; 
      $ret[]= $entry;
      $queue_pos ++;
    }
  
    /* Sort entries */
    $simple_map = array("QueuePosition" => "Queue_Position",
                        "Action"        => "Action",
                        "TaskID"        => "TASK_ID");
    $sort_arr = array();
    if(isset($simple_map[$this->sort_by])){
      foreach($ret as $id => $entry){
        $sort_arr[$entry[$simple_map[$this->sort_by]]][] = $id;
      }
    }
    if($this->sort_by == "TargetName"){
      foreach($ret as $id => $entry){
        $name = $this->target_to_string($entry);
        $sort_arr[$name][] = $id;
      }
    }
    if($this->sort_by == "Schedule"){
      foreach($ret as $id => $entry){
        $name = $entry['Minute'].$entry['Hour'].$entry['Day'].$entry['Month'].$entry['Weekday'];
        $sort_arr[$name][] = $id;
      }
    }
    if($this->sort_dir == "up"){
      ksort($sort_arr);
    }else{
      krsort($sort_arr);
    }
    $ret2 = array();
    foreach($sort_arr as $key => $entries){
      foreach($entries as $entry_id){
        $ret2[] = $ret[$entry_id];
      }
    }
    return($ret2);
  }


  function get_additional_entry_informations($data)
  {
    $i = rand(0,3);
    if($i == 0){
      return("<div style='width:80px;background-color:#999999;'><div style='background-color:#00FF00;width:40%'>&nbsp;</div></div>");
    }
    if($i == 1){
      return("<font style='color: #FF0000;'>Failed</font>");
    }
    if($i == 1){
      return("<font style='color: #00FF00;'>OK</font>");
    }
    return("-");
  }


  function target_to_string($data)
  {
    $ret = "";
    if($data['Action'] == "initial_install"){
      foreach($data['Initial_Target'] as $target){
        $ret .= $target['MAC'].", ";
      } 
    }else{
      foreach($data['Target'] as $target){
        $ret .= $target.", ";
      } 
    }
    return(preg_replace("/, $/","",$ret));
  }

  
  function time_to_string($data)
  {
    return($data['Minute']." ".$data['Hour']." ".$data['Day']." ".$data['Month']." ".$data['Weekday']);
  }

  
  function action_to_string($data)
  {
    $tmp = $this->get_actions();
    if(isset($tmp[$data['Action']])){
      return($tmp[$data['Action']]);
    }else{
      return(_("Unknown"));
    }
  }

  
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


  function get_actions()
  {
    /* Prepare list of available actions */
    $actions = array(       "reboot"          => _("Reboot"),
                            "localboot"       => _("Localboot"),
                            "halt"            => _("Halt system"),
                            "initial_install" => _("Initial installation"),
                            "update"          => _("Update"),
                            "reinstall"       => _("(Re)Install"),
                            "rescan"          => _("Rescan"),
                            "wake"            => _("Wake"),
                            "memcheck"        => _("Memory check"));
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
