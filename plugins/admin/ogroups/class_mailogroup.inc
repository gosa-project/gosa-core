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
  }

  function execute()
  {
    /* Initialize templating engine */
    $smarty= get_smarty();

    /* Assign mail attribute */
    $smarty->assign("mail", $this->mail);
    $smarty->assign("mailACL", chkacl($this->acl, "mail"));

    /* Show main page */
    return ($smarty->fetch (get_template_path('mail.tpl', TRUE)));
  }

  /* Check formular input */
  function check()
  {
    $ldap= $this->config->get_ldap_link();

    $message= array();

    /* Check if mail address is valid */
    if (!is_email($this->mail)){
      $message[]= _("Please enter a valid email address in 'Primary address' field.");
    }

    /* Check if mail address is already in use */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search ("(&(!(objectClass=gosaUserTemplate))(|(mail=".$this->mail.         ")(gosaMailAlternateAddress=".$this->mail."))(!(cn=".$this->cn.")))", array("uid"));
    if ($ldap->count() != 0){
      $message[]= _("The primary address you've entered is already in use.");
    }
 

    return ($message);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
