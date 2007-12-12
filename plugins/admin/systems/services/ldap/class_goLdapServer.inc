<?php

class goLdapServer extends goService{

  var $cli_summary      = "This plugin is used within the ServerService Pluign \nand indicates that this server has goLdapServer defined.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goLdapServer");
  var $attributes       = array("goLdapBase");
  var $StatusFlag       = "goLdapServerStatus";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goLdapServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn                  = "";
  var $goLdapServerStatus  = "";
  var $goLdapBase          = ""; 
  var $view_logged  =FALSE;

  function goLdapServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("LDAP service");
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
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    return($smarty->fetch(get_template_path("goLdapServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields  = goService::getListEntry();
    $fields['Message']    = _("LDAP Service");
    $fields['AllowEdit']  = true;
    return($fields);
  }



  function check()
  { 
    $message = plugin::check();
    if(empty($this->goLdapBase)){
      $message[] = _("The given base is empty or contains invalid characters.");
    }
    return($message);
  }


  function save_object()
  {
    if(isset($_POST['goLdapServerPosted'])){
      plugin::save_object();
    }
  } 


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Ldap"),
          "plDescription" => _("Ldap")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 96,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "goLdapBase" => _("Ldap base"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
