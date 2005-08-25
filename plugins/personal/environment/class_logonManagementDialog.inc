<?php
class logonManagementDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array("LogonName","LogonPriority","LogonLast","LogonOverload","LogonData");
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  

  var $LogonName      ="";  // Name for the LogonScript
  var $LogonPriority  ="";  // Priority defines the order in which the scripts will be processed 
  var $LogonLast      ="";  // Is this script marked as the last, all following scripts will be skipped
  var $LogonOverload  ="";  // If Overload is activated this script is overlaodable by a group script with same prio
  var $LogonData      ="";  // The script itself

  function logonManagementDialog ($config, $dn= NULL,$use_existing=false )
  {
    $this->use_existing = $use_existing;
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    $smarty= get_smarty();
    $display= "";

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      if($this->$attr){
        $smarty->assign($attr."CHK"," checked ");
      }else{
        $smarty->assign($attr."CHK","");
      }
    }
    $prios=array(1,2,3,4,5,6,7,8,9,10);
    $smarty->assign("LogonPrioritys",$prios);
    $smarty->assign("LogonPriorityKeys",$prios);
    $display.= $smarty->fetch(get_template_path('logonManagement.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function save_object()
  {
    if(isset($_POST['dialogissubmitted'])){
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr= $_POST[$attr]; 
        }else{
          $this->$attr = false;
        }
      }
    }
  }

  function save()
  {
    $a_return= array();
    foreach($this->attributes as $attr){
      $a_return[$attr]=$this->$attr;
    }  
    return($a_return); 
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
