<?php

class goTerminalServer extends goService{
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports asterisk management.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goTerminalServer");
  var $attributes       = array("goXdmcpIsEnabled", "goFontPath");
  var $StatusFlag       = "goTerminalServerStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goTerminalServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $acl;
  var $cn                      = "";
  var $goTerminalServerStatus  = "";
  var $goXdmcpIsEnabled        = false;  
  var $goFontPath              = "";
  var $view_logged  =FALSE;


  function goTerminalServer(&$config,$dn)
  {
    goService::goService($config,$dn);
    $this->DisplayName = _("Terminal service");
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
    return($smarty->fetch(get_template_path("goTerminalServer.tpl",TRUE,dirname(__FILE__))));
  }


  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("Terminal service");
    $fields['AllowEdit']  = true;
    return($fields);
  }


  function save()
  {
    plugin::save();

    if(!$this->goXdmcpIsEnabled){
      $this->attrs['goXdmcpIsEnabled'] = "0";
    }

    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving server services/terminalServer with dn '%s' failed."),$this->dn));
    if($this->initially_was_account){
      $this->handle_post_events("modify");
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      $this->handle_post_events("add");
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }
  }


  function check()
  { 
    $message = plugin::check();
    if(empty($this->goFontPath)){
      $message[]=_("Terminal server, must have fontpath specified.");
    }

    return($message);
  }


  function save_object()
  {
    if(isset($_POST['goTerminalServerPosted'])){
      plugin::save_object();
      if($this->acl_is_writeable("goXdmcpIsEnabled")){
        if(isset($_POST['goXdmcpIsEnabled'])){
          $this->goXdmcpIsEnabled = true;
        }else{
          $this->goXdmcpIsEnabled = false;
        }
      }
    }
  } 


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Terminal service"),
          "plDescription" => _("Terminal service")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 87,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
            "goXdmcpIsEnabled"  => _("Temporary disable login"),
            "goFontPath"        => _("Font path"))
          ));
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
