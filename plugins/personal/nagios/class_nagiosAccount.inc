<?php

/*! \brief   mail plugin
  \author  Guillame Delecourt <guillaume@opensides.be>
  \author  Benoit Mortier <benoit.mortier@opensides.be>
  \author  Vincent Seynhaeve <vincent@opensides.be>  
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
  var $NagiosAlias  = "";
  var $NagiosPager  = "";
  var $NagiosMail   = "";

  var $HostNotificationOptions      = "";
  var $HostNotificationPeriod       = "";
  var $ServiceNotificationPeriod    = "";
  var $ServiceNotitificationPeriod  = "";
  var $HostNotificationCommands     = "";
  var $ServiceNotificationCommands  = "";
  var $HostNotitificationOptions    = "";
  var $ServiceNotificationOptions   = "";

  var $AuthorizedAllHosts                 = "unchecked";
  var $AuthorizedAllServices              = "unchecked";
  var $AuthorizedSystemCommands           = "unchecked";
  var $AuthorizedAllHostCommands          = "unchecked";
  var $AuthorizedAllServiceCommands       = "unchecked";
  var $AuthorizedConfigurationInformation = "unchecked";
  var $AuthorizedSystemInformation        = "unchecked";

  /* attribute list for save action */

  var $attributes= array("NagiosAlias", "NagiosPager", "NagiosMail", "HostNotificationPeriod",
      "ServiceNotificationPeriod", "HostNotificationCommands", "ServiceNotificationCommands",
      "HostNotificationOptions", "ServiceNotificationOptions", "AuthorizedAllHosts", "AuthorizedAllServices",
      "AuthorizedSystemCommands", "AuthorizedAllHostCommands", "AuthorizedAllServiceCommands",
      "AuthorizedConfigurationInformation", "AuthorizedSystemInformation");

  var $is_chk_box = array("AuthorizedAllHosts","AuthorizedAllServices","AuthorizedSystemCommands",
      "AuthorizedAllHostCommands","AuthorizedAllServiceCommands",
      "AuthorizedConfigurationInformation","AuthorizedSystemInformation");

  var $objectclasses= array("nagiosContact","nagiosAuth");

  var $uid = "";

  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function nagiosAccount ($config, $dn= NULL, $parent= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    plugin::plugin ($config, $dn, $parent);

    /* Set uid */    
    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Save initial account state */  
    $this->initially_was_account= $this->is_account;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Load templating engine */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){

      /* Onyl change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
      }
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

    /* Assign attributes an ACL to smarty */
    foreach($this->attributes as $val) { 
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
      if(in_array($val,$this->is_chk_box)){
        if($this->$val == "checked"){
          $smarty->assign($val."CHK", " checked ");
        }else{
          $smarty->assign($val."CHK", "");
        }
      }
    }
    $smarty->assign('ServiceNotificationOptionsValues'  ,array("d,u,r,v"=>"d,u,r,v" ,"d,u,r"=>"d,u,r" ,"d,u"=>"d,u"));
    $smarty->assign('HostNotificationOptionsValues'     ,array("w,u,c,r"=>"w,u,c,r" ,"w,u,c"=>"w,u,c" ,"c,w"=>"c,w"));
    $smarty->assign('ServiceNotificationPeriodValues'   ,array("24x7"=>"24x7" ,"24x5"=>"24x5" ,"8x5"=>"8x5"));
    $smarty->assign('HostNotificationPeriodValues'      ,array("24x7"=>"24x7" ,"24x5"=>"24x5" ,"8x5"=>"8x5"));
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
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving nagios account failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add", array("uid" => $this->uid));
    }
  }

  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    $ldap= $this->config->get_ldap_link();
    /* must: mail */
    if ($this->NagiosAlias == ""){
      $message[]= _("The required field 'NagiosAlias' is not set.");
    }
    if ($this->NagiosMail == ""){
      $message[]= _("The required field 'NagiosMail' is not set.");
    }
    if ($this->is_template){
      if (!is_email($this->NagiosMail, TRUE)){
        $message[]= _("Please enter a valid email address in 'NagiosMail' field.");
      }
    } else {
      if (!is_email($this->NagiosMail)){
        $message[]= _("Please enter a valid email address in 'NagiosMail' field.");
      }
    }
    return($message);
  }


  /* Use Save_object for every Post handling */
  function save_object()
  {
    if (isset($_POST['nagiosTab'])){
      /* Save ldap attributes */
      plugin::save_object();

      /* Specialhandling for checkboxes */
      foreach($this->is_chk_box as $val){
        if(isset($_POST[$val])){
          $this->$val = "checked";
        }else{
          $this->$val = "unchecked";
        }
      }
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
    //     $method= new $this->method($this->config);
    //     $method->fixAttributesOnRemove($this);

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing nagios account failed"));

    /* remove the entry from LDAP */
    unset ($this->attrs['uid']);

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove',array("uid"=>$this->uid));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:

?>
