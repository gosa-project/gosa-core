<?php

class ServiceAddDialog extends plugin{

  var $cli_summary      = "This dialog is used to add services";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("config"=>"Config object" , "dn"=>"Object dn");

  /* This plugin does not have any ocs */
  var $objectclasses    = array();
  var $parent           = NULL;

  function ServiceAddDialog(&$config,$dn,$parent)
  {
    plugin::plugin($config);
    $this->parent = $parent;
  }

  function execute()
  {
    $smarty = get_smarty();
    $services = $this->parent->getAllUnusedServices();
    natcasesort($services);
    $smarty->assign("Services",$services);
    return($smarty->fetch(get_template_path("ServiceAddDialog.tpl", TRUE,dirname(__FILE__))));
 }

  function check(){ return array();}
  function save_object(){;}
  function save(){}

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
