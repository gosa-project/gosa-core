<?php

class phpscheduleitAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "PHPscheduleit Account";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users phpscheduleit account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("phpscheduleitAccount");

  function phpscheduleitAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    if ($this->is_account){
      $smarty->assign("phpscheduleitState", "checked");
    } else {
      $smarty->assign("phpscheduleitState", "");
      $smarty->assign("wstate", "disabled");
    }

    if ($this->parent != NULL){
      $smarty->assign("tabbed", 1);
    }

    $smarty->assign('gosaphpscheduleitACL', chkacl($this->acl, 'gosaphpscheduleit'));

    $display.= $smarty->fetch (get_template_path('phpscheduleit.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    plugin::remove_from_parent();
    $ldap= $this->config->get_ldap_link();

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['phpscheduleit'])){
        if (!$this->is_account && $_POST['phpscheduleit'] == "B"){
          $this->is_account= TRUE;
        }
      } else {
        $this->is_account= FALSE;
      }
    }

    plugin::save_object();
    if (isset($_POST["phpscheduleitStatus"])){
      $this->pptpStatus = "disabled";
    } else {
      $this->pptpStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
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

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
