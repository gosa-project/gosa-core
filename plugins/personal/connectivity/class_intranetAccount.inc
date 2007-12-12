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
  var $ReadOnly = false;  

  var $uid ="";

  var $view_logged = FALSE;

  /*! \brief Konstructor  

    \param $config 	The Config Object used to initialise plugin
    \param $dn 		The DN of the currently edited entry	
    \author  Harald Falk <hf@doellken-weimar.de>
    \version 1.00
    \date    1.07.2005
   */
  function intranetAccount (&$config, $dn= NULL, $parent=NULL)
  {
    plugin::plugin ($config, $dn,$parent);

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

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

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
    if ($this->parent !== NULL){
      $smarty->assign("tabbed", 1);
    }else{
      $smarty->assign("tabbed", 0);	
    }


    $smarty->assign('gosaIntranetACL', " disabled ");
    if((!$this->ReadOnly) && (    ($this->is_account && $this->acl_is_removeable()) 
                               || (!$this->is_account && $this->acl_is_createable()))){
      $smarty->assign('gosaIntranetACL', "");
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
    if($this->acl_is_removeable() && $this->initially_was_account){ 
      plugin::remove_from_parent();
      $ldap= $this->config->get_ldap_link();

      $ldap->cd($this->dn);
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      /* Log last action */
      new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/intranet account with dn '%s' failed."),$this->dn));

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
      if (isset($_POST['intranet'])){
        if (!$this->is_account && $_POST['intranet'] == "B"){
          if($this->acl_is_createable()){
            $this->is_account= TRUE;
          }
        }
      } else {
        if($this->acl_is_removeable()){
          $this->is_account= FALSE;
        }
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
      plugin::save();

      /* Write back to ldap */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      /* Log last action */
      if($this->initially_was_account){
        new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }else{
        new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      }

      show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/intranet account with dn '%s' failed."),$this->dn));

      /* Optionally execute a command after we're done */
      $this->postcreate(array("uid" => $this->uid));
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array(	
          "plShortName"     => _("Intranet"),
          "plDepends"       => array("user"),
          "plPriority"      => 25,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plDescription"       => _("Intranet account settings")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"        => TRUE,

          "plProvidedAcls"  => array()
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
