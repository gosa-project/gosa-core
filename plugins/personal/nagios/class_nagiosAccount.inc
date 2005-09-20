<?php

/* Create a class (name must be unique inside GOsa) which extends plugin. The plugin base
    class contains all methods that are used by GOsa and it provides the mechanism to load data
    from LDAP to local variables and prepare the save to ldap routine for you. */
class nagiosAccount extends plugin
{
	/* Definitions */
	var $plHeadline= "Nagios";
	var $plDescription= "This does something";
	
	/* CLI vars */
	var $cli_summary= "Manage users nagios account";
	var $cli_description= "Some longer text\nfor help";
	var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");
	
	/* These contain attributes to be loaded. We're not doing an LDAP plugin currently, so we don't
	care... */
	var $attributes= array("NagiosAlias","NagiosPager","NagiosMail","HostNotificationPeriod","ServiceNotificationPeriod","HostNotificationCommands","ServiceNotificationCommands",
	"HostNotificationOptions",
	"ServiceNotificationOptions",
	"HostNotificationPeriod",
	"ServiceNotificationPeriod",
	"AuthorizedAllHosts",
	"AuthorizedAllServices",
	"AuthorizedSystemCommands",
	"AuthorizedAllHostCommands",
	"AuthorizedAllServiceCommands",
	"AuthorizedConfigurationInformation",
	"AuthorizedSystemInformation");
  var $objectclasses= array("nagiosContact");

  /* The constructor just saves a copy of the config. You may add what ever you need. */
  function nagiosAccount ($config, $dn= NULL)
  {
        /* Include config object */
        $this->config= $config;
  	plugin::plugin ($config, $dn);
	$this->initially_was_account= $this->is_account;
}

  /* Execute is the function all plugins need. It fills the plugin with life and produces the output. */
  function execute()
  {
        $display= "";
	/* Use the smarty templating engine here... */
        $smarty= get_smarty();

	//echo "<pre>";print_r($_POST);echo "</pre>";
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
	/* Load attributes */
	foreach($this->attributes as $val){
	$smarty->assign("$val", $this->$val);
	$smarty->assign($val."ACL", chkacl($this->acl, "$val"));
	}
	
	if ($this->is_account){
		$smarty->assign("nagiosAuth","checked");
		$smarty->assign("nagiosContact","checked");
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
    	$display.= $smarty->fetch (get_template_path('nagios.tpl', TRUE, dirname(__FILE__)));
	//echo "<pre>";print_r($_POST);echo "</pre>";
	return ($display);
  }
 /* Save to LDAP */
function save()
{
	/* Load uid and gid of this 'dn' */
	$ldap= $this->config->get_ldap_link();
	$ldap->cat($this->dn);
	$tmp= $ldap->fetch();
	
	plugin::save();

	//nagiosContact
	//ugly code but i will correct this after :)

	if(isset($_POST['NagiosAlias']))$this->attrs['NagiosAlias']=$_POST['NagiosAlias'];
	if(isset($_POST['NagiosPager']))$this->attrs['NagiosAlias']=$_POST['NagiosAlias'];
	if(isset($_POST['NagiosMail']))$this->attrs['NagiosMail']=$_POST['NagiosMail'];
	if(isset($_POST['ServiceNotificationPeriod']))$this->attrs['ServiceNotificationPeriod']=$_POST['ServiceNotificationPeriod'];
	if(isset($_POST['HostNotificationPeriod']))$this->attrs['HostNotificationPeriod']=$_POST['HostNotificationPeriod'];
	if(isset($_POST['ServiceNotificationOptions']))$this->attrs['ServiceNotificationOptions']=$_POST['ServiceNotificationOptions'];
	if(isset($_POST['HostNotificationOptions']))$this->attrs['HostNotificationOptions']=$_POST['HostNotificationOptions'];
	if(isset($_POST['ServiceNotificationCommands']))$this->attrs['ServiceNotificationCommands']=$_POST['ServiceNotificationCommands'];
	if(isset($_POST['HostNotificationCommands']))$this->attrs['HostNotificationCommands']=$_POST['HostNotificationCommands'];

	//nagiosAuth
	if(isset($_POST['AuthorizedSystemInformation']))$this->attrs['AuthorizedSystemInformation']="checked";else $this->attrs['AuthorizedSystemInformation']="unchecked";
	if(isset($_POST['AuthorizedConfigurationInformation']))$this->attrs['AuthorizedConfigurationInformation']="checked";else $this->attrs['AuthorizedConfigurationInformation']="unchecked";
	if(isset($_POST['AuthorizedSystemCommands']))$this->attrs['AuthorizedSystemCommands']="checked";else $this->attrs['AuthorizedSystemCommands']="unchecked";
	if(isset($_POST['AuthorizedAllServices']))$this->attrs['AuthorizedAllServices']="checked";else $this->attrs['AuthorizedAllServices']="unchecked";
	if(isset($_POST['AuthorizedAllHosts']))$this->attrs['AuthorizedAllHosts']="checked";else $this->attrs['AuthorizedAllHosts']="unchecked";
	if(isset($_POST['AuthorizedAllServiceCommands']))$this->attrs['AuthorizedAllServiceCommands']="checked";else $this->attrs['AuthorizedAllServiceCommands']="unchecked";
	if(isset($_POST['AuthorizedAllHostCommands']))$this->attrs['AuthorizedAllHostCommands']="checked";else $this->attrs['AuthorizedAllHostCommands']="unchecked";

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

function save_object()
{

	if (!$this->is_account && ($_POST['nagiosAuth'] == "B" || $_POST['nagiosContact'] == "B")) {
		$this->is_account= TRUE;
	}
	else {
		$this->is_account= FALSE;
	}
	plugin::save_object();
}

function remove_from_parent()
{
	/* Cancel if there's nothing to do here */
	if (!$this->initially_was_account){
	return;
	}
	
	plugin::remove_from_parent();
	$ldap= $this->config->get_ldap_link();
	
	$ldap->cd($this->dn);
	@DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
	$this->attributes, "Save");
	$ldap->modify($this->attrs);
	show_ldap_error($ldap->get_error());
	
	/* Optionally execute a command after we're done */
	$this->handle_post_events('remove');
}
}
?>