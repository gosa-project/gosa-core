<?php

class faiPackageConfiguration extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage server basic objects";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account = TRUE;
  var $attributes     = array();
  var $objectclasses  = array();

  var $obj;

  function faiPackageConfiguration ($config, $dn= NULL,$obj)
  {
    plugin::plugin ($config, $dn);
    $this->obj = $obj;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";
    $display.= $smarty->fetch(get_template_path('faiPackageConfiguration.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
    return ($message);
  }

  function save()
  {
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
