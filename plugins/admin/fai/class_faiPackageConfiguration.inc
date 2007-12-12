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

  var $Debconf_handle = NULL;
  var $pkg_config ;
  var $obj;

  function faiPackageConfiguration (&$config, $dn= NULL,$obj,$path,$pkg_config)
  {
    plugin::plugin ($config, $dn);
    $this->obj = $obj;
    $this->Debconf_handle = new debconf();
    $this->Debconf_handle->set_template_directory($path);
    $this->Debconf_handle->set_package($this->obj);
    $this->Debconf_handle->set_language(preg_replace('/_.*$/', '', get_browser_language()));

    if( $this->Debconf_handle->load() ) {
      $this->pkg_config = $pkg_config;

      foreach($this->pkg_config as $var){
        $this->Debconf_handle->SetDefault($var['Name'],$var['Value']);
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";
    $smarty->assign("headline", sprintf(_("Debconf information for package '%s'"), $this->obj));

    $this->Debconf_handle->PostCheck();

    $smarty->assign("Config",$this->Debconf_handle->get_dialog());

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
    /* Call common method to give check the hook */
    $message= plugin::check();

    return ($message);
  }

  function save()
  {
    $this->Debconf_handle->PostCheck();
    $pkgs = $this->Debconf_handle->template;

    $tmp = array();
    foreach($pkgs as $pkg){
      $tmp[$this->obj][$pkg['Name']]['Value']= $pkg['Default'];
      $tmp[$this->obj][$pkg['Name']]['Name'] = $pkg['Name'];
      $tmp[$this->obj][$pkg['Name']]['Type'] = $pkg['Type'];
    }
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
