<?php

class gosaLogServer extends goService{
	
  /* This plugin only writes its objectClass */
  var $objectclasses    = array("gosaLogServer");
  var $attributes       = array("goLogDB","goLogDBUser","goLogDBPassword");
  var $StatusFlag       = "";

  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("gosaLogServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;
  var $cn                 = "";
  var $goLogDBServerStatus= "";
  var $goLogDBUser        = "";
  var $goLogDBPassword    = "";  
  var $goLogDB            = "";  
  var $view_logged  =FALSE;


  function gosaLogServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("GOsa MySQL logging service");
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
    return($smarty->fetch(get_template_path("gosaLogServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']      = _("GOsa MySQL logging service");
    $fields['AllowEdit']    = true;
    $fields['AllowStart']   = FALSE;
    $fields['AllowStop']    = FALSE;
    $fields['AllowRestart'] = FALSE;
    return($fields);
  }


  function check()
  { 
    $message = plugin::check();
    if (empty($this->goLogDBUser)){
      $message[]= sprintf_("The attribute user is empty or contains invalid characters.");
    }
    if (empty($this->goLogDBPassword)){
      $message[]= sprintf_("The attribute password is empty or contains invalid characters.");
    }
    if (empty($this->goLogDB)){
      $message[]= sprintf_("The attribute database is empty or contains invalid characters.");
    }
    return($message);
  }


  function save_object()
  {
    if(isset($_POST['gosaLogServerPosted'])){
      plugin::save_object();
    }
  } 

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("GOsa logging service"),
          "plDescription" => _("GOsa logging service")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 98,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "goLogDB"         => _("Database"),
            "goLogDBUser"     => _("User"),
            "goLogDBPassword" => _("Password"), 
            "viewEntries"    => _("Allow view of entries on this server")) 
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
