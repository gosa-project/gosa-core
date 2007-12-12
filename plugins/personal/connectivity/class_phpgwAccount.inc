<?php
class phpgwAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PHPGroupware";
  var $plDescription= "This does something";

  /* GW attributes */
  var $phpgwAccountExpires= "-1";
  var $phpgwAccountStatus= "A";
  var $phpgwAccountType= "u";

  /* attribute list for save action */
  var $attributes= array("phpgwAccountExpires", "phpgwAccountStatus", "phpgwAccountType");
  var $objectclasses= array("phpgwAccount");
  var $ReadOnly = false;

  var $uid  = "";
  var $view_logged = FALSE;

  function phpgwAccount (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }

  function execute()
  {
	/* Call parent execute */
//	plugin::execute();
 
    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }
 
    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
    }
    if ($this->is_account){
      $smarty->assign("phpgwState", "checked");
    } else {
      $smarty->assign("phpgwState", "");
    }

    if((!$this->ReadOnly)&&(($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable()))) {
      $smarty->assign('phpgwAccountACL', "");
    }else{
      $smarty->assign('phpgwAccountACL', " disabled ");
    }

    $display.= $smarty->fetch (get_template_path('phpgw.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if($this->acl_is_removeable()){
      /* Cancel if there's nothing to do here */
      if (!$this->initially_was_account){
        return;
      }

      plugin::remove_from_parent();
      $ldap= $this->config->get_ldap_link();

      $ldap->cd($this->dn);
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/PHPgw account with dn '%s' failed."),$this->dn));

      new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      /* Optionally execute a command after we're done */
      $this->handle_post_events('remove',array("uid" => $this->uid));
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['phpgw'])){
        if (!$this->is_account && $_POST['phpgw'] == "B"){
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
    if (isset($_POST["phpgwStatus"])){
      $this->phpgwStatus = "disabled";
    } else {
      $this->phpgwStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/PHPgw account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }

  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("PHP GW"),
          "plDescription"   => _("PHP GW account settings")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 24,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array()
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
