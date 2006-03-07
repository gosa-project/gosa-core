<?php

class servDNSeditZoneEntries extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  function servDNSeditZoneEntries ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
	  plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
    $display.= $smarty->fetch(get_template_path('servDNSeditZoneEntries.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
  }

  function save_object()
  {
    plugin::save_object();
  }

  function check()
  {
    $message= array();
    return ($message);
  }

  function save()
  {
    plugin::save();
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
