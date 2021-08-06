<?php

class sieve_require 
{
  var $data = array();
  var $object_id = -1;
  var $parent = NULL;  
  var $skip_save_object =FALSE;

  function __construct($data,$object_id,$parent)
  {
    $this->parent = $parent;
    $this->object_id = $object_id;
    if($data !== NULL){

      for($i = 0 ; $i < count($data['ELEMENTS']) ; $i++){
        $tmp = sieve_get_strings($data['ELEMENTS'],$i);
        $i  = $i + $tmp['OFFSET'];
        foreach($tmp['STRINGS'] as $str){
          $this->data[]= $str;
        }
      }
    }
  }


  /* Add a new require statement and ensure 
   *  that it is not specified twice 
   */
  function Add_Require($str)
  {
    $current = array();
    foreach($this->data as $dat){
      $current[] = $dat;
    }
    if(!in_array_strict($str,$current)){
      $this->data[] = $str;
    }
    $this->data = array_unique($this->data);
    $this->skip_save_object = TRUE;
  }

  function save_object()
  {
    if($this->skip_save_object){
      $this->skip_save_object = FALSE;
      return;
    }

    /* Get the values should check for, they are seperated by , */
    if(isset($_POST['require_'.$this->object_id])){
      $vls = stripslashes($_POST['require_'.$this->object_id]);
      $tmp = array();

      $tmp2 = explode(",",$vls);
      foreach($tmp2 as $val){
        
        $val = trim($val);
    
        if(empty($val)) continue;        
  
        $tmp[] = $val;
      }
      $this->data = $tmp;
    }
  }

  function check()
  {
    $msgs = array();
  
    if(!count($this->data)){
      $msgs[] = _("Please specify at least one valid requirement.");
    }
    return($msgs);
  }

  function get_sieve_script_part()
  {
    if(count($this->data)){
    $tmp = sieve_create_strings($this->data);
    return("require ".$tmp.";\n");
    }else{
      return("");
    }
  } 
    
  function execute()
  {
    $Require = "";
    foreach($this->data as $key){
      $Require .= $key.", ";
    }
    $Require = preg_replace("/,$/","",trim($Require));

    $smarty = get_smarty();
    $smarty->assign("Require",$Require);
    $tmp = $this->check();
    $smarty->assign("LastError",$tmp);
    $smarty->assign("LastErrorCnt",count($tmp));
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_require.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
