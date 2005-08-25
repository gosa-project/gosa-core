<?php
class logonManagementDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  

  var $baseDir              = "../kioskProfiles/";
  var $filePrefix           = ".kioskProfile";

  function logonManagementDialog ($config, $dn= NULL,$use_existing=false )
  {
    $this->use_existing = $use_existing;
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    $smarty= get_smarty();
    $display= "";
    $display.= $smarty->fetch(get_template_path('logonManagement.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
