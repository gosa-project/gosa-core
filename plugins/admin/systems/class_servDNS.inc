<?php

class servdns extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array("whatever");

  function servdns ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove DNS service"),
          _("This server has DNS features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DNS service"),
          _("This server has DNS features disabled. You can enable them by clicking below."));
      return ($display);
    }

    $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE));
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
    $message= array();

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
