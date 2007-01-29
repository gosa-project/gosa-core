<?php

/*!
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005

      \brief   Enables Apache authentification for Intranet through openldap with .htaccess files   
*/

class intranetAccount extends plugin
{
  /*! Definitions */
  var $plHeadline= "Intranet";
  /*! Definitions */
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  /* ObjectClasses list for save action */
  var $objectclasses= array("gosaIntranetAccount");

  var $uid ="";

  /*! \brief Konstructor  
		
	\param $config 	The Config Object used to initialise plugin
	\param $dn 		The DN of the currently edited entry	
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005
  */
  function intranetAccount ($config, $dn= NULL, $parent= NULL)
  {
	plugin::plugin ($config, $dn, $parent);
	
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }

  /*!
	\brief General execution 
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005
	
	Load smarty Template and assign needed smarty vars
	*/
	
	
  function execute()
  {
	/* Call parent execute */
//	plugin::execute();

	$display= "";

	$smarty= get_smarty();

	if ($this->is_account){
		$smarty->assign("intranetState", "checked");
		$smarty->assign("wstate", "");
	} else {
		$smarty->assign("wstate", "disabled");
		$smarty->assign("intranetState", "");
	}

	//! Ever assign vars to smarty in both cases, to avoid php errors (missing variable aso. ) 
	if ($this->parent != NULL){
		$smarty->assign("tabbed", 1);
	}else{
		$smarty->assign("tabbed", 0);	
	}

	/* Allow account status toogle?  */
	$smarty->assign("gosaIntranetACL", "disabled");
	if(!$this->is_account && chkacl($this->acl,"create") == ""){
		$smarty->assign("gosaIntranetACL", "");
	}elseif($this->is_account && $this->acl == "#all#" ){
		$smarty->assign("gosaIntranetACL", "");
	}

	$display.= $smarty->fetch (get_template_path('intranet.tpl', TRUE, dirname(__FILE__)));
	return ($display);
  }

  /*! 
	\brief Delete ext from User
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005
	Handles deletion of this object
  */
  function remove_from_parent()
  {
	  if(chkacl($this->acl, 'intranetAccount') == ""){
		  plugin::remove_from_parent();
		  $ldap= $this->config->get_ldap_link();

		  $ldap->cd($this->dn);
		  @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
				  $this->attributes, "Save");
		  $this->cleanup();
		  $ldap->modify ($this->attrs); 

		  show_ldap_error($ldap->get_error(), _("Removing intranet account failed"));

		  /* Optionally execute a command after we're done */
		  $this->postremove(array("uid"=>$this->uid));
	  }
  }


  /*!
	\brief handles Post data 
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005
	Save data to object 
	*/
  function save_object()
  {
        /* Do we need to flip is_account state? */
	  if (isset($_POST['connectivityTab'])){

		  /* Change state if needed */
		  if(!$this->is_account && isset($_POST["intranet"]) && chkacl($this->acl,"create") == ""){
			  $this->is_account = true;
		  }elseif($this->is_account && !isset($_POST["intranet"]) && $this->acl == "#all#"){
			  $this->is_account = false;
		  }
	  }

	plugin::save_object();
	if (isset($_POST["INTRANETStatus"])){
		$this->INTRANETStatus = "disabled";
	} else {
		$this->INTRANETStatus = "enabled";
	}
  }


  /*! \brief Handles LDAP saves 
      \author  Harald Falk <hf@doellken-weimar.de>
      \version 1.00
      \date    1.07.2005
	Save objectClass for User in LDAP 
	
	*/
  function save()
  {
	  if(chkacl($this->acl, 'intranetAccount') == ""){
		  plugin::save();
			
		  /* Write back to ldap */
		  $ldap= $this->config->get_ldap_link();
		  $ldap->cd($this->dn);
		  $this->cleanup();
		  $ldap->modify ($this->attrs); 

		  show_ldap_error($ldap->get_error(), _("Saving intranet account failed"));

		  /* Optionally execute a command after we're done */
		  $this->postcreate(array("uid"=> $this->uid));
	  }
  }

}

?>
