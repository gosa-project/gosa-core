<?php

class goLogDBServer extends goService{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goLogDBServer");
  var $attributes       = array("goLogAdmin", "goLogPassword");
  var $StatusFlag       = "goLogDBServerStatus";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goLogDBServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;
  var $cn                   = "";
  var $goLogDBServerStatus  = "";
  var $goLogAdmin           = "";
  var $goLogPassword        = "";  
  var $view_logged  =FALSE;


  function goLogDBServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("Syslog service database");
  }


  function execute()
  { 
    $smarty = get_smarty(); 

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty->fetch(get_template_path("goLogDBServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']      = _("Syslog service database");
    $fields['AllowEdit']    = true;
    return($fields);
  }


  function check()
  { 
    $message = plugin::check();
    if (empty($this->goLogAdmin)){
      $message[]= sprintf_("The attribute user is empty or contains invalid characters.");
    }
    if (empty($this->goLogPassword)){
      $message[]= sprintf_("The attribute password is empty or contains invalid characters.");
    }
    return($message);
  }


  function save_object()
  {
    if(isset($_POST['goLogDBServerPosted'])){
      plugin::save_object();
    }
  } 

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Logging database"),
          "plDescription" => _("Logging database")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 97,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "start"         => _("Start"),
            "stop"          => _("Stop"),
            "restart"       => _("Restart"),
            "goLogAdmin"    => _("Admin"),
            "goLogPassword" => _("Password"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
