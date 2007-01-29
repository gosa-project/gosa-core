<?php

class servdhcp extends plugin
{
  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  function servdhcp ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove DHCP service"),
          _("This server has DHCP features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DHCP service"),
          _("This server has DHCP features disabled. You can enable them by clicking below."));
      return ($display);
    }

    $display.= $smarty->fetch(get_template_path('servdhcp.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Optionally execute a command after we're done */
    #$this->handle_post_events($mode);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
