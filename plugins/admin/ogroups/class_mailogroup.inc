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


  function mailogroup ($config, $dn= NULL)
  {
    plugin::plugin($config, $dn);

    /* Include config object */
    $this->config= $config;

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;
  }


  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove mail account"),
            _("This group has mail features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_header(_("Create mail account"), _("This group has mail features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }
    
    /* Initialize templating engine */
    $smarty= get_smarty();

    /* Assign mail attribute */
    $smarty->assign("mail", $this->mail);
    $smarty->assign("mailACL", chkacl($this->acl, "mail"));

    /* Show main page */
    return ($display.$smarty->fetch (get_template_path('mail.tpl', TRUE)));
  }


  /* Check formular input */
  function check()
  {
    $message= array();

    if ($this->is_account){
      $ldap= $this->config->get_ldap_link();

      /* Check if mail address is valid */
      if (!is_email($this->mail) || $this->mail == ""){
        $message[]= _("Please enter a valid email address in 'Primary address' field.");
      }

      /* Check if mail address is already in use */
      $ldap->cd($this->config->current['BASE']);
      $ldap->search ("(&(!(objectClass=gosaUserTemplate))(|(mail=".$this->mail.         ")(gosaMailAlternateAddress=".$this->mail."))(!(cn=".$this->cn.")))", array("uid"));
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

    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
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

    show_ldap_error($ldap->get_error());
  }


  function getCopyDialog()
  {
    $str  = "";
    $str .= _("Phone number");
    $str .= "&nbsp;<input type='text' name='mail' value='".$this->mail."'>";
    return($str);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['mail'])){
      $this->mail = $_POST['mail'];
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
