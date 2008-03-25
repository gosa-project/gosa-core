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

  var $roleName		= ""; // cn

  var $sudoUser		= "";
  var $sudoHost		= "";
  var $sudoCommand	= "";
  var $sudoRunAs	= "";
  var $sudoOption	= "";

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

    $smarty->assign("roleName", 	$this->roleName);
    $smarty->assign("sudoUser", 	$this->sudoUser);
    $smarty->assign("sudoHost", 	$this->sudoHost);
    $smarty->assign("sudoCommand", 	$this->sudoCommand);
    $smarty->assign("sudoRunAs", 	$this->sudoRunAs);
    $smarty->assign("sudoOption", 	$this->sudoOption);

    /* Display template */
    $display.= $smarty->fetch(
    	get_template_path('sudoldapeditrole.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
     //plugin::save_object();
     foreach($this->attributes as $attr){
	if(isset($_POST[$attr])){
		$this->$attr = $_POST[$attr];
	}
     }
  }


  /* Check supplied data */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* $dn was posted as parameter */
    //$this->dn = $dn;

    /* Save */
    plugin::save();
    
    echo $this->dn;

    /* Write back to ldap */
    $ldap->cd($this->dn);
    //$this->cleanup();
    $ldap->add($this->attrs); 

    /* Display errors 
     */
    if($ldap->get_error() != "Success"){
    	show_ldap_error("Record:".$ldap->get_error(), _("Saving failed!")); 
    }

    //$ret =array();
    //foreach($this->attributes as $name){
	//$ret[$name] = $this->$name;
    //}

    //return($ret);
  }

  
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
