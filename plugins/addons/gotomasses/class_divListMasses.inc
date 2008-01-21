<?php

class divListMasses extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  var $parent             ;
  var $ui                 ;

  var $wake = TRUE;  
  
  function divListMasses (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "gotoMasses", array("gotomasses"));

    $this->parent             = $parent;
    $this->ui                 = get_userinfo();
    $this->SetSummary(_("List of queued deamon jobs."));
    $this->SetHeadpageMode();
    $this->EnableCloseButton(FALSE);
    $this->EnableSaveButton(FALSE);
    $this->SetInformation(_("This menu allows you to remove and change the properties of GOsa deamon tasks."));

    foreach($this->parent->get_actions() as $name => $desc){
      $this->$name = TRUE;
      $this->AddCheckBox($name,sprintf(_("Display entries with action %s."),$desc),
                               sprintf(_("Display entries with action %s."),$desc),TRUE);
    }
    $plug  = $_GET['plug'];

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=QueuePosition'>"._("#")."</a>",
                           "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=TargetName'>"._("Target")."</a>&nbsp;/&nbsp;".
                                     "<a href='?plug=".$plug."&amp;sort=TaskID'>"._("Task")."</a>"));
    $this->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=Schedule'>"._("Schedule")."</a>",
                                      "attach"=>"style='width:100px;'"));
    $this->AddHeader(array("string"=>"<a href='?plug=".$plug."&amp;sort=Action'>"._("Status")."</a>",
                                      "attach"=>"style='width:80px;'"));
    $this->AddHeader(array("string"=>_("Action"),
                                      "attach"=>"style='border-right:0px;width:120px;'"));
  }

  function GenHeader()
  {
    
    /* Display add button if allowed */
    $header = "<div style='padding:5px'>";
#    if($this->parent->acl_is_createable()){
#      $header .= "<input type='image' src='images/gotomasses_new_task.png' name='new_task' class='center'>&nbsp;";
#    }

    /* Display remove button if allowed */
    if($this->parent->acl_is_removeable()){
      $header .= "<input type='image' src='images/edittrash.png' name='remove_multiple_task' class='center'>&nbsp;";
    }

    /* Add priority options */
    if(preg_match("/w/",$this->parent->getacl(""))){
      $header .= "<input type='image' src='images/status_stop_all.png' name='stop_all' class='center'>&nbsp;";
      $header .= "<input type='image' src='images/status_start_all.png' name='start_all' class='center'>&nbsp;";
    }
    $header .= "</div>";
    
    /* Create divlist */
    $this->SetListHeader($header);
  }


  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }


  function setEntries($tasks)
  {
    /* Create edit link */
    $plug = $_GET['plug'];
#    $edit_link = "<div style='width:100%;overflow:hidden;'><nobr><a href='?plug=".$_GET['plug']."&act=edit&id=%id%'>%str%</nobr></div>";
    $edit_link = "%str%";

    /* Create action filter array, to sort out those actions we do not want to see */
    $allowed_action = array();
    foreach($this->parent->get_actions() as $name => $desc){
      if($this->$name){
        $allowed_action[] = $name;
      }
    }

    $colors[0] = "#DDDDDD";
    $colors[1] = "#EEEEEE";
    $c_keys = array();
    $last_c = 0;
    if($this->parent->sort_dir=="down"){
      $last_c = !$last_c; 
    }
  
    foreach($tasks as $key => $task){

      $color = "";
      /* Skip unchecked actions */ 
#      if(!in_array($task['Action'],$allowed_action)){
#        continue;
#      }

      /* Create actions */
      $id         = $task['ID'];
      $queue_pos  = $id;
      $prio_actions = "<input class='center' type='image' src='images/status_stop_all.png' name='stop_".$id."'>&nbsp;";
      $prio_actions.= "<input class='center' type='image' src='images/status_start_all.png' name='start_".$id."'>&nbsp;";
      $action = "<input type='image' src='images/edit.png' name='edit_task_".$id."' class='center' alt='"._("Edit")."'>";
      if($this->parent->acl_is_removeable()){
        $action.= "<input type='image' src='images/edittrash.png' name='remove_task_".$id."' class='center' alt='"._("Reomve")."'>";
      }

      $display = preg_replace(array("/%id%/","/%str%/"),array($id,$task['MACADDRESS']),$edit_link);
      $display2= $task['HEADERTAG'];

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$id."' name='item_selected_".$id."'>" ,
                      "attach" => "style='width:20px;".$color."'");
      $field0a= array("string" => $queue_pos ,"attach" => "style='width:20px;".$color."'");
      $field1 = array("string" => $display,
                      "attach" => "style='".$color."'");
      $field1a= array("string" => $display2,
                      "attach" => "style='".$color.";width:80px;'");
      $field2 = array("string" => $this->stamp_to_date($task['TIMESTAMP']),"attach" => "style='".$color.";width:100px;'");
      $field3 = array("string" => $task['STATUS'],"attach" => "style='".$color.";width:80px;'");
      $field4 = array("string" => $prio_actions.$action,"attach" => "style='".$color.";text-align:right;width:120px;border-right:0px;'");
      $this->AddElement(array($field0,$field0a,$field1,$field1a,$field2,$field3,$field4));
    }
  }


  function stamp_to_date($stamp)
  {
    return(date("d.m.Y H:i:s",strtotime($stamp)));
  }


  function Save()
  {
    MultiSelectWindow :: Save();  
  }


  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow :: save_object();  
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
