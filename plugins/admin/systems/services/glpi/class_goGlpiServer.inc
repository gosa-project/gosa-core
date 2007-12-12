<?php

class goGlpiServer extends goService {
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goGlpiServer");
  var $attributes       = array("goGlpiAdmin","goGlpiDatabase","goGlpiPassword");
  var $StatusFlag       = "goGlpiServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goGlpiServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn               = "";
  var $acl;

  var $goGlpiServerStatus ="";
  var $goGlpiAdmin        ="";
  var $goGlpiDatabase     ="";
  var $goGlpiPassword     ="";
  var $view_logged  =FALSE;
 
 
  function goGlpiServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("Inventory database");
  }


  function execute()
  { 
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation ){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    return($smarty->fetch(get_template_path("goGlpiServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("Inventory database service");
    $fields['AllowEdit']  = true;
    return($fields);
  }

  function check()
  { 
    $message = plugin::check();
    if(empty($this->goGlpiAdmin)){
      $message[]= _("The attribute user is empty or contains invalid characters.");
    }
    if(empty($this->goGlpiDatabase)){
      $message[]= _("The attribute database is empty or contains invalid characters.");
    }
    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goGlpiServer_posted'])){
      plugin::save_object();
    }
  } 

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Inventory database"),
          "plDescription" => _("Inventory database")." ("._("Services").")" ,
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 93,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "start"         => _("Start"),
            "stop"          => _("Stop"),
            "restart"       => _("Restart"),

            "goGlpiAdmin"    => _("Admin"), 
            "goGlpiDatabase" => _("Database"),
            "goGlpiPassword" => _("Password"))
          ));
  }

 
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
