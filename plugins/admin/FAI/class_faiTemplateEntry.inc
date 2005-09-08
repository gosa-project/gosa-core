<?php

class faiTemplateEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("Object_cn","Object_description","Object_FAItemplateFile","Object_FAItemplatePath","Object_FAIowner","Object_FAImode");
  var $objectclasses= array();

  var $orig_cn              = "";

  var $Object_dn            = "";
  var $Object_cn            = "";
  var $Object_FAItemplateFile   = "";
  var $Object_FAItemplatePath   = "";
  var $Object_FAIowner     = "";
  var $Object_FAImode    = "";
  var $Object_description   = "";
  var $Object_status        = "new";
  
  function faiTemplateEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = "Object_".$name;
        $this->$oname=$value;
      }
    }else{
      $this->Object_status = "new";
      $this->orig_cn       = false;
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,stripslashes($this->$attrs));
    }

    for($i =1 ; $i <= 100 ; $i++){
      $Object_FAIprioritys[$i]=$i;
    }
    $smarty->assign("Object_FAIprioritys",$Object_FAIprioritys);
    $display.= $smarty->fetch(get_template_path('faiTemplateEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['SubObjectFormSubmitted'])){
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = "";
        }
      }
    }
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
  
    
    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $attr = preg_replace("/^Object_/","",$attrs);
      $tmp[$attr] = $this->$attrs;
    }

    if(($this->orig_cn)&&($tmp['cn']!=$this->orig_cn)){
      $tmp['remove']['from']  = $this->orig_cn;
      $tmp['remove']['to']    = $tmp['cn'];
    }
  
    $tmp['dn']      = $this->dn;  
    $tmp['status']  = $this->Object_status;  
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
