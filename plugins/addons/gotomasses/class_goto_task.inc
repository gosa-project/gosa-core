<?php

class goto_task extends plugin
{
  /* Definitions */
  var $plHeadline     = "System mass deployment";
  var $plDescription  = "This does something";
 
  var $parent   = NULL;
  var $data     = array();

  var $attributes = array("MACADDRESS","ID","STATUS","HEADERTAG");

  function goto_task(&$config, &$parent, $data = array())
  {
    /* Set parent object */
    $this->parent = &$parent;
    $this->data = $data;

    /* Intialize plugin */
    $this->config = &$config;
  }


  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("Actions",$this->parent->get_actions());
    foreach($this->attributes as $attr){
      if(isset($this->data[$attr])){
        $smarty->assign($attr,$this->data[$attr]);
      }else{
        $smarty->assign($attr,"");
      }
    }

    $year   = substr($this->data['TIMESTAMP'],0,4);
    $month  = substr($this->data['TIMESTAMP'],4,2);
    $day    = substr($this->data['TIMESTAMP'],6,2);
    
    $hour   = substr($this->data['TIMESTAMP'],8,2);
    $minute = substr($this->data['TIMESTAMP'],10,2);
    $second = substr($this->data['TIMESTAMP'],12,2);

    $years = array();
    for($i = date("Y",time()); $i <= 2037 ;$i ++){
      $years[$i] = $i;
    }
    $months = array();
    for($i = 1; $i <= 12; $i ++){
      $e = str_pad($i,2,"0",STR_PAD_LEFT);
      $months[$e] = $e;
    }
    $days = array();
    for($i = 1; $i <= cal_days_in_month(CAL_GREGORIAN,$month,$year); $i ++){
      $e = str_pad($i,2,"0",STR_PAD_LEFT);
      $days[$e] = $e;
    }
    $hours = array();
    for($i = 0; $i < 24; $i ++){
      $e = str_pad($i,2,"0",STR_PAD_LEFT);
      $hours[$e] = $e;
    }
    $minutes = array();
    for($i = 0; $i < 60; $i ++){
      $e = str_pad($i,2,"0",STR_PAD_LEFT);
      $minutes[$e] = $e;
    }
    $seconds = array();
    for($i = 0; $i < 60; $i ++){
      $e = str_pad($i,2,"0",STR_PAD_LEFT);
      $seconds[$e] = $e;
    }

    $smarty->assign("years", $years);
    $smarty->assign("months", $months);
    $smarty->assign("days", $days);
    $smarty->assign("hours", $hours);
    $smarty->assign("minutes", $minutes);
    $smarty->assign("seconds", $seconds);
 
    $smarty->assign("time_year",$year); 
    $smarty->assign("time_month",$month); 
    $smarty->assign("time_day",$day); 
    $smarty->assign("time_hour",$hour); 
    $smarty->assign("time_minute",$minute); 
    $smarty->assign("time_second",$second); 

    return ($smarty->fetch (get_template_path('goto_task.tpl', TRUE)));
  }


  function save_object()
  {
    if(isset($_POST['HeaderTag'])){
      $possible_tags = $this->parent->get_actions();
      $tag = $_POST['HeaderTag'];
      if(isset($possible_tags[$tag])){
        $this->data['HEADERTAG'] = $tag;
      }
    }
    foreach(array("status","macaddress") as $attr){
      if(isset($_POST[$attr])){
        $this->data[strtoupper($attr)] = $_POST[$attr];
      }
    }
    $date = "";
    foreach(array("time_year","time_month","time_day","time_hour","time_minute","time_second") as $attr){
      if(isset($_POST[$attr])){
        $date .= $_POST[$attr];
      }
    }
    if(strlen($date) == 14){
      $this->data['TIMESTAMP'] = $date;
    }
  }

  function save()
  {
    $tmp = array();   
    foreach(array("STATUS","MACADDRESS","HEADERTAG","TIMESTAMP") as $attr){
      if(isset($this->data[$attr])){
        $tmp[$attr] = $this->data[$attr];
      }
    }
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
