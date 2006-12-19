<?php

class sambaLogonHours extends plugin
{

  var $sambaLogonHours = "";
  var $Matrix          = array();
  
  var $config;

  function sambaLogonHours ($config, $dn, $slh)
  {
    plugin::plugin($config,$dn);

    /* Convert to bin */
    $tmp = '';
    for($i = 0 ; $i < strlen($slh) ; $i ++){
      $tmp .= str_pad(base_convert($slh[$i],16,2),4,'0',STR_PAD_LEFT);
    }
    $slh = $tmp;
    $this->sambaLogonHours = $slh;

    /* Set to default if value is empty or seams to be broken */
    if(strlen($slh) != 168){
      $slh = str_pad('',168 , "1");
    }

    /* Create matrix */ 
    $this->Matrix = array();
    for($day = 0 ; $day < 7 ; $day ++ ){
      for($hour  = 0 ; $hour < 24 ; $hour ++){
        $bitpos  = ($day * 24 + $hour) % 168;
        $this->Matrix[$day][$hour] = $slh[$bitpos]; 
      }
    }
  }

  function execute()
  {
    $week_days = array();
    $week_days[0]= _("Sunday");
    $week_days[1]= _("Monday");
    $week_days[2]= _("Tuesday");
    $week_days[3]= _("Wednesday");
    $week_days[4]= _("Thursday");
    $week_days[5]= _("Friday");
    $week_days[6]= _("Saturday");

    $hours = array();
    for($i = 0 ; $i <24 ; $i++ ){
      $hours[$i] = $i ;
    }

    $smarty = get_smarty();
    $smarty->assign("Matrix",$this->Matrix);
    $smarty->assign("Days"  ,$week_days);
    $smarty->assign("Hours" ,$hours);
    return($smarty->fetch(get_template_path("sambaLogonHours.tpl",TRUE,dirname(__FILE__))));
  }

  function save_object()
  {
    /* Check if dialog was opened and if there were any changes */
    if(isset($_POST['sambaLogonHoursPosted'])){
      foreach($this->Matrix as $day_key => $days){
        foreach($days as $hour_key => $hour){
          if(isset($_POST['day_'.$day_key.'_'.$hour_key])){
            $this->Matrix[$day_key][$hour_key] = 1;
          }else{
            $this->Matrix[$day_key][$hour_key] = 0;
          }
        }
      }
    }
  }
  
  function save()
  {
    /* Convert Matrix to Hex */
    $ret = "";
    foreach($this->Matrix as $day_key => $days){
      $four_bit = '';
      foreach($days as $hour_key => $hour){
        $four_bit .= $hour;
        if(strlen($four_bit) == 4){
          $ret .= base_convert($four_bit,2,16);
          $four_bit ='';
        }
      }
    }
    
    /* All hours and days selected */
    if(substr_count($ret,'f') == 42){
      $ret ="";
    }
  
    return($ret);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
