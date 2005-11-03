<?php

class mailqueue extends plugin
{
  /* Definitions */
  var $plHeadline= "Mail queue";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function mailqueue($config, $dn= NULL)
  {
	/* Include config object */
	$this->config= $config;

  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
	$smarty= get_smarty();
	return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

}

?>
