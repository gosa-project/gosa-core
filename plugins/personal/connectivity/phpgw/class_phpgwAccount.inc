<?php
class phpgwAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PHPGroupware";
  var $plDescription= "Manage PHPGroupware user settings";

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
  
  var $multiple_support = TRUE;

  function __construct(&$config, $dn= NULL)
  {
    plugin::__construct ($config, $dn);

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
      $smarty->assign("$val", set_post( $this->$val));
    }
    if ($this->is_account){
      $smarty->assign("phpgwState", "checked");
    } else {
      $smarty->assign("phpgwState", "");
    }

    $smarty->assign('phpgwAccountACL', $this->getacl("",$this->ReadOnly));
    $smarty->assign("multiple_support",$this->multiple_support_active);
    $smarty->assign("use_phpgw",in_array_strict("phpgw",$this->multi_boxes));
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

      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
      }

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

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

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
          "plShortName"     => _("PHPGroupware"),
          "plDescription"   => _("PHPGroupware account settings")."&nbsp;("._("Connectivity add-on").")",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 24,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),
          "plRequirements"=> array(
              'ldapSchema' => array('phpgwAccount' => ''),
              'onFailureDisablePlugin' => array(get_class())
              ),

          "plProvidedAcls"  => array()
          ));
  }


  function multiple_save_object()
  {
    if (isset($_POST['connectivityTab'])){
      plugin::multiple_save_object();

      /* Do we need to flip is_account state? */
      if(isset($_POST['use_phpgw'])){
        $this->multi_boxes[] = "phpgw";
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
      }
    }
  }

  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    if(in_array_strict("phpgw",$this->multi_boxes)){
      $ret['is_account'] = $this->is_account;
    }
    return($ret);
  }

  function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);
    if(isset($values['is_account'])){
      $this->is_account = $values['is_account'];
    }
  }

  function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);
    if(isset($attrs['objectClass']) && in_array_strict("phpgwAccount",$attrs['objectClass'])){
      $this->is_account = TRUE;
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
