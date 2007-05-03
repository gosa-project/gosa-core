<?php

/* This class handles the stop statement */
class sieve_stop 
{
  var $object_id = -1;

  function sieve_stop($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function save_object()
  {
  }

  function check()
  {
    return(array());
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_stop.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }

  function get_sieve_script_part()
  {
    return("stop; \n");
  } 
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
