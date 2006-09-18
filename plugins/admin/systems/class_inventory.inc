<?php

class inventory extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  function inventory ($config, $dn= NULL, $parent= NULL)
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
  }

  function save_object()
  {
  }

  function check()
  {
  }

  function save()
  {
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
