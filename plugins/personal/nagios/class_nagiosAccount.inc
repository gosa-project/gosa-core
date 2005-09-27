<?php

/*! \brief   mail plugin
  \author  Guillame Delecourt <guillaume@opensides.be>
  \author  Benoit Mortier <benoit.mortier@opensides.be>
  \version 1.00
  \date    25.09.2005

  This class provides the functionality to read and write all attributes
  relevant for nagiosAccount from/to the LDAP. It does syntax checking
  and displays the formulars required.
 */

class nagiosAccount extends plugin
{
	/* Definitions */
	var $plHeadline= "Nagios";
	var $plDescription= "This does something";
	
	/* CLI vars */
	var $cli_summary= "Manage users nagios account";
	var $cli_description= "Manage Account and autorization\nfor Nagios";
	var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* plugin specific values */
  var $NagiosAlias= "";
  var $NagiosPager= "";
  var $NagiosMail= "";
  var $HostNotificationPeriod= "";
  var $ServiceNotitificationPeriod= "";
  var $HostNotificationCommands= "";
  var $ServiceNotificationCommands= "";
  var $HostNotitificationOptions= "";
  var $ServiceNotificationOptions= "";

  var $AuthorizedAllHosts= "";
  var $AuthorizedAllServices= "";
  var $AuthorizedSystemCommands= "";
  var $AuthorizedAllHostCommands= "";
  var $AuthorizedAllServiceCommands= "";
  var $AuthorizedConfigurationInformation= "";
  var $AuthorizedSystemInformation= "";

  /* attribute list for save action */
  
	var $attributes= array("NagiosAlias", "NagiosPager", "NagiosMail", "HostNotificationPeriod",
      "ServiceNotificationPeriod", "HostNotificationCommands", "ServiceNotificationCommands",
      "HostNotificationOptions", "ServiceNotificationOptions", "AuthorizedAllHosts", "AuthorizedAllServices",
      "AuthorizedSystemCommands", "AuthorizedAllHostCommands", "AuthorizedAllServiceCommands",
      "AuthorizedConfigurationInformation", "AuthorizedSystemInformation");

  var $objectclasses= array("nagiosContact");

  /* constructor, if 'dn' is set, the node loads the given
       'dn' from LDAP */
  function nagiosAccount ($config, $dn= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;
    
  	plugin::plugin ($config, $dn);

    /* Save initial account state */  
	  $this->initially_was_account= $this->is_account;
  }

  function execute()
  {
    /* Load templating engine */
    $smarty= get_smarty();
    $display= "";
            
	  /* Do we need to flip is_account state? */
	  if (isset($_POST['modify_state'])){
	    $this->is_account= !$this->is_account;
	  }
  
	  /* Do we represent a valid account? */
	    if (!$this->is_account && $this->parent == NULL){
	      $display= "<img alt=\"\"src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
		      _("This account has no nagios extensions.")."</b>";
    
        $display.= back_to_main();
        return ($display);
	    }
  
	  /* Show tab dialog headers */
	  if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove nagios account"),
            _("This account has nagios features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_header(_("Create nagios account"), _("This account has nagios features disabled. You can enable them by clicking below."));
        return($display);
	    }
	  }
	  
    foreach(array("NagiosAlias", "NagiosPager", "NagiosMail", "HostNotificationPeriod",
                  "ServiceNotificationPeriod", "HostNotificationCommands", "ServiceNotificationCommands",
                  "HostNotificationOptions", "ServiceNotificationOptions", "AuthorizedAllHosts", "AuthorizedAllServices",
                  "AuthorizedSystemCommands", "AuthorizedAllHostCommands", "AuthorizedAllServiceCommands",
                  "AuthorizedConfigurationInformation", "AuthorizedSystemInformation") as $val) {

    $smarty->assign("$val", $this->$val);
    $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    	$display.= $smarty->fetch (get_template_path('nagios.tpl', TRUE, dirname(__FILE__)));
	    return ($display);

  }

  /* Save to LDAP */
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call parents save to prepare $this->attrs */
	  plugin::save();

    /* Adapt attributes if needed */
    //$method= new $this->method($this->config);
    //$id= $method->uattrib;
    //$method->fixAttributesOnStore($this);
                

	  /* Write back to ldap */
	  $ldap->cd($this->dn);
	  $ldap->modify($this->attrs);
	  show_ldap_error($ldap->get_error());
	
	  /* Optionally execute a command after we're done */
	  if ($this->initially_was_account == $this->is_account){
	    if ($this->is_modified){
		    $this->handle_post_events("modify");
	    }
	  } else {
	    $this->handle_post_events("add");
    }
	
  }

  function check()
  {
  }
  
  function save_object()
  {

    if (isset($_POST['nagiosTab'])){
      /* Save ldap attributes */
      plugin::save_object();
    }
  }

  function remove_from_parent()
  {
	  /* Cancel if there's nothing to do here */
	  if (!$this->initially_was_account){
	    return;
	  }

    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();

    /* Adapt attributes if needed */
    $method= new $this->method($this->config);
    $method->fixAttributesOnRemove($this);
    

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
      $this->attributes, "Save");
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());
                        
    /* remove the entry from LDAP */
    unset ($this->attrs['uid']);
                
    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:

?>
