<?php

class mailogroup extends plugin
{
  /* plugin specific values */
  var $mail= "";
  var $cn= "";

  /* attribute list for save action */
  var $attributes= array("mail");
  var $objectclasses= array("kolabGroupOfNames");
  var $members= array();
  var $view_logged = FALSE;

  function mailogroup (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin($config, $dn, $parent);

    /* Include config object */
    $this->config= $config;

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","ogroups/".get_class($this),$this->dn);
    }


    /* Do we need to flip is_account state? */
    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    /* Show tab dialog headers */
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display= $this->show_disable_header(_("Remove mail account"),
            _("This group has mail features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_enable_header(_("Create mail account"), _("This group has mail features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }
    
    /* Initialize templating engine */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl("mail"));
    }

    /* Assign mail attribute */
    $smarty->assign("mail", $this->mail);

    /* Show main page */
    return ($display.$smarty->fetch (get_template_path('mail.tpl', TRUE)));
  }


  /* Check formular input */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if ($this->is_account){
      $ldap= $this->config->get_ldap_link();

      /* Check if mail address is valid */
      if (!tests::is_email($this->mail) || $this->mail == ""){
        $message[]= _("Please enter a valid email address in 'Primary address' field.");
      }

      /* Check if mail address is already in use */
      $ldap->cd($this->config->current['BASE']);
      $ldap->search ("(&(!(objectClass=gosaUserTemplate))(|(mail=".$this->mail.")(gosaMailAlternateAddress=".$this->mail."))(!(cn=".$this->cn.")))", array("uid"));
      if ($ldap->count() != 0){
        $message[]= _("The primary address you've entered is already in use.");
      }
    }

    return ($message);
  }


  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of object group/mail with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }


  /* remove object from parent */
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

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    new log("remove","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of object group/mail with dn '%s' failed."),$this->dn));
  }


  function getCopyDialog()
  {
    $str = "";
    $smarty = get_smarty();
    $smarty->assign("mail",     $this->mail);
    $str = $smarty->fetch(get_template_path("paste_mail.tpl",TRUE,dirname(__FILE__)));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['mail'])){
      $this->mail = $_POST['mail'];
    }
  }


   static function plInfo()
  {
    return (array(
          "plShortName"   => _("Mail"),
          "plDescription" => _("Mail group"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 4,
          "plSection"     => array("administration"),
          "plCategory"    => array("ogroups"),
          "plProvidedAcls"=> array(
            "mail" => _("Mail address"))
          ));
  }




}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
