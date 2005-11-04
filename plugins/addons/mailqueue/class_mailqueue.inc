<?php

class mailqueue extends plugin
{
  /* Definitions */
  var $plHeadline= "Mail queue";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  var $QueryCommand = "";
  var $RemoveCommand= "";

  function mailqueue($config, $dn= NULL)
  {
	/* Include config object */
	$this->config= $config;

	$this->QueryCommand = search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_QUERY_COMMAND");
	$this->RemoveCommand= search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_REMOVE_COMMAND");
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();
	$smarty= get_smarty();
	print search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_QUERY_COMMAND");
	print search_config($this->config->data['MENU'], "mailqueue", "MAILQUEUE_REMOVE_COMMAND");
	return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

}

?>
