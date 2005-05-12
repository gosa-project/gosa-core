<?php

class servservice extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function servservice ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Always is account... */
    $this->is_account= TRUE;
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();

    return($smarty->fetch (get_template_path('servservice.tpl', TRUE)));
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
