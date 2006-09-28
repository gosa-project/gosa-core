<?php

class openca extends plugin
{
  /* Definitions */
  var $plHeadline= "Certificates";
  var $plDescription= "This does something";

  function openca ($config, $dn= NULL)
  {
	/* Include config object */
	$this->config= $config;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

	$smarty= get_smarty();

	/* Show main page */
	return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

}

?>
