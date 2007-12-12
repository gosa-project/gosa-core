<?php

class goKrbServer extends goService{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports NTP service.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goKrbServer");
  var $attributes       = array("goKrbRealm", "goKrbAdmin","goKrbPassword");
  var $StatusFlag       = "goKrbServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goKrbServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $goKrbServerStatus= "";
  var $cn               ="";
  var $goKrbRealm     = "";
  var $goKrbAdmin     = "";
  var $goKrbPassword  ="";  
  var $view_logged  =FALSE;
 
  function goKrbServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("Kerberos service");
  }


  function execute()
  { 
    $smarty = get_smarty(); 

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    $tmp = $this->plinfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty->fetch(get_template_path("goKrbServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("Kerberos service (kadmin access informations)");
    $fields['AllowEdit']  = true;
    return($fields);
  }

  function check()
  { 
    $message = plugin::check();
    /* goKrbPassword is a must field, if goKrbServer is used as objectClass */
    if (empty($this->goKrbPassword)){
      $message[]= sprintf(_("The specified kerberos password is empty."), $attr);
    }
    if (empty($this->goKrbAdmin)){
      $message[]= sprintf(_("The specified kerberos admin is empty."), $attr);
    }
    if (empty($this->goKrbRealm)){
      $message[]= sprintf(_("The specified kerberos realm is empty."), $attr);
    }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goKrbServerPosted'])){
      plugin::save_object();
    }
  } 

    /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Kerberos"),
          "plDescription" => _("Kerberos access information")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 95,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "goKrbRealm"    => _("Realm"), 
            "goKrbAdmin"    => _("Admin"),
            "goKrbPassword" => _("Password"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
