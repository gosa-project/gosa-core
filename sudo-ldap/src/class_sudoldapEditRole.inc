<?php

class sudoldapEditRole extends plugin
{
  /* Plugin specific values */
  var $attributes	= array("sudoUser", 
				"sudoHost", 
				"sudoCommand", 
				"sudoRunAs", 
				"sudoOption");
  
  var $objectclasses	= array("sudoRole");

  function sudoldapEditRole ($config, $dn= NULL,$attrs = array())
  {
    plugin::plugin ($config, $dn);

  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();


    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Display template */
    $display.= $smarty->fetch(
    	get_template_path('sudoldapeditrole.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
  }


  /* Check supplied data */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
  }

  
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
