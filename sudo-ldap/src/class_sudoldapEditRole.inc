<?php

class sudoldapEditRole extends plugin
{
  /* Plugin specific values */
  var $attributes	= array("cn",
  				"description",
  				"sudoUser", 
				"sudoHost", 
				"sudoCommand", 
				"sudoRunAs", 
				"sudoOption");
  
  var $objectclasses	= array("sudoRole");

  var $cn		= "";
  var $description	= "";
  var $sudoUser		= "";
  var $sudoHost		= "";
  var $sudoCommand	= "";
  var $sudoRunAs	= "";
  var $sudoOption	= "";

  var $isNew		= true; 

  function sudoldapEditRole ($config, $dn= NULL, $attrs = array())
  {
    plugin::plugin ($config, $dn);

    echo '<pre>';
    print_r($attrs);
    echo '</pre>';

    if(!count($attrs)){
    	$this->isNew = true;
    }else{
	$this->isNew = false;
      	foreach($this->attributes as $value){
        	if(isset($attrs[$value])){
          		$this->$value = $attrs[$value];
        	}
      	}
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();


    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $smarty->assign("cn", 		$this->cn);
    $smarty->assign("description", 	$this->description);
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
    plugin::save();
    $ldap= $this->config->get_ldap_link();

    $this->dn = "cn=".$this->cn.",ou=SUDOers,".$this->config->current['BASE'];
    $this->attrs['objectClass'] = array("sudoRole");

    $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    $ldap->cd($this->dn);
    $ldap->add($this->attrs); 

    /* Display errors 
     */
    if($ldap->get_error() != "Success"){
    	show_ldap_error("Record:".$ldap->get_error(), _("Saving failed!")); 
    }

  }

  
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
