<?php

class sieve_block_start 
{
  var $object_id= -1;
  
  function sieve_block_start($data = NULL,$object_id)
  {
    $this->object_id = $object_id;
  }

  function execute()
  {
    $smarty = get_smarty();
    return($smarty->fetch(get_template_path("templates/element_block_start.tpl",TRUE,dirname(__FILE__))));
  }

  function check()
  {
    return(array());
  }

  function save_object()
  {
  }

  function get_sieve_script_part()
  {
    return("{");
  } 
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
