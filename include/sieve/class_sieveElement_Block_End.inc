<?php

class sieve_block_end 
{
  var $object_id= -1;
  
  function sieve_block_end($data = NULL,$object_id)
  {
    $this->object_id = $object_id;
  }

  function execute()
  {
    $smarty = get_smarty();
    return($smarty->fetch(get_template_path("templates/element_block_end.tpl",TRUE,dirname(__FILE__))));
  }
  function check()
  {
    return(array());
  }

  function get_sieve_script_part()
  {
    return("}");
  } 
  function save_object()
  {
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
