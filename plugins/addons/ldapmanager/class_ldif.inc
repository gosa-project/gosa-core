<?php
require_once("tabs_ldif.inc");

class ldif extends plugin
{
  /* Definitions */
  var $plHeadline= "LDAP manager";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $ldif;

  function ldif ($config)
  {
    /* Include config object */
    $this->ldif= new ldiftab($config, $config->data['TABS']['LDIFTAB'], "");
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Show main page */
    return ($this->ldif->execute());
  }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
