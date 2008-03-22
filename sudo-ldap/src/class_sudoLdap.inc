<?php
/*!
  \brief   sudoLdap plugin
  \author  Guido Serra <gserra@guidoserra.it>
  \version 1.00
  \date    22.03.2008

  This class provides the functionality to read and write all attributes
  relevant for sudo-ldap from/to the LDAP.
 */

class sudoLdap extends plugin
{
  /* Definitions */
  var $plHeadline= "sudo-ldap";
  var $plDescription= "This does something";

  /* Plugin specific values */
 
  var $attributes     = array("sudoUser", "sudoHost", "sudoCommand", "sudoRunAs", "sudoOption");
  var $objectclasses  = array("sudoRole");

  var $uid ="";

  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function sudoLdap ($config, $dn= NULL, $parent= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    /* Load bases attributes */
    plugin::plugin($config, $dn, $parent);

    /* set user id */    
    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }

    $ldap= $this->config->get_ldap_link();
    
  }


  /* execute generates the html output for this node */
  function execute($isCopyPaste = false)
  {
	/* Call parent execute */
	plugin::execute();
	$display= "";

	$display = "execute()";
	
	return($display);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
