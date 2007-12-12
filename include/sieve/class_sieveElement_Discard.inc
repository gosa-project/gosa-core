<?php

class sieve_discard 
{
  var $data = array();
  var $object_id = -1;

  function sieve_discard($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function get_sieve_script_part()
  {
    return("discard;");
  } 
  function check()
  {
    return(array())  ;
  }

    
  function save_object()
  {
  
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object = $smarty->fetch(get_template_path("templates/element_discard.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
