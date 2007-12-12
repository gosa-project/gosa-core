<?php

/* Sieve else tag */
class sieve_elsif extends sieve_if
{
  var $TYPE = "elsif";
}

class sieve_else
{
  var $object_id = -1;

  function check()
  {
    return(array());
  }

  function sieve_else($data,$object_id)
  {
    $this->object_id = $object_id;
  }

  function save_object()
  {
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container_clear.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_else.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }

  function get_sieve_script_part()
  {
    return("else");
  }
}

?>
