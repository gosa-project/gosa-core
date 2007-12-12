<?php

class goImapServer extends goService {
	
  var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports mailqueue listings and so on.";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goImapServer");
  
  /* This class can't be assigned twice so it conflicts with itsself */

  var $DisplayName      = "";
  var $dn               = NULL;
  var $StatusFlag       = "goImapServerStatus";
  var $attributes       = array("goImapName","goImapConnect","goImapAdmin","goImapPassword",
                                "goImapSieveServer","goImapSievePort",
                                "cyrusImap","cyrusImapSSL","cyrusPop3","cyrusPop3SSL");

  var $cn                   = "";

  var $goImapName           = "";
  var $goImapConnect        = "";
  var $goImapAdmin          = "";
  var $goImapPassword       = "";
  
  var $goImapSieveServer    = "";
  var $goImapSievePort      = "";
  
  var $goImapServerStatus   = "";
  
  var $cyrusImap            = false;
  var $cyrusImapSSL         = false;
  var $cyrusPop3            = false;
  var $cyrusPop3SSL         = false;
  var $is_account           = false;
  var $view_logged  =FALSE;

  var $acl; 

  var $Actions              = array();  
  var $conflicts            = array("goImapServer","kolab");
 
  function goImapServer(&$config,$dn)
  {
    goService::goService($config,$dn);
  
    $this->DisplayName = _("IMAP/POP3 service");

    $this->Actions = array( SERVICE_STOPPED=>SERVICE_STOPPED,
                            SERVICE_STARTED => SERVICE_STARTED,
                            SERVICE_RESTARTED=>SERVICE_RESTARTED,
                            "repair_database"=>_("Repair database"));
    
  }

  function execute()
  { 
    $smarty = get_smarty();
 
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }
 
    /* set new status */
    if(isset($_POST['ExecAction'])){
      if(isset($this->Actions[$_POST['action']])){

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation) {
          if(preg_match("/^".$_POST['action']."$/i",$name)){
            if($this->acl_is_writeable($name)){
              $this->setStatus($_POST['action']);
            }
          }
        }

      }
    }

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);  
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    $smarty->assign("Actions",$this->Actions);
    $smarty->assign("is_new",$this->dn);
    $smarty->assign("is_acc",$this->initially_was_account);
    return($smarty->fetch(get_template_path("goImapServer.tpl",TRUE,dirname(__FILE__))));
  }

  function getListEntry()
  {
    $fields               = goService::getListEntry();
    $fields['Message']    = _("IMAP/POP3 (Cyrus) service");
    $fields['AllowRemove']= true;
    $fields['AllowEdit']  = true;
    return($fields);
  }

  function check()
  { 
    $message = plugin::check();
    if(empty($this->goImapName)){
      $message[] =_("Please specify a server identifier.");
    }  
    if(empty($this->goImapConnect)){
      $message[] =_("Please specify a connect url.");
    }  
    if(empty($this->goImapAdmin)){
      $message[] =_("Please specify an admin user.");
    }  
    if(empty($this->goImapPassword)){
      $message[] =_("Please specify a password for the admin user.");
    } 

    /* Check connect string */
    if (!preg_match('/^\{[^:]+:[0-9]+.*\}$/', $this->goImapConnect)){
      $message[]= sprintf(_("The imap connect string needs to be in the form '%s'."),
          '{server-name:port/options}');
    }
    if (!preg_match('/^[0-9]+$/', $this->goImapSievePort)){
      $message[]= _("The sieve port needs to be numeric.");
    }
 
    return ($message);
  }


  function save_object()
  {
    if(isset($_POST['goImapServerPosted'])){
      plugin::save_object(); 

      foreach(array("cyrusImap","cyrusImapSSL","cyrusPop3","cyrusPop3SSL") as $checkbox) { 

        if($this->acl_is_writeable($checkbox)){
          if(!isset($_POST[$checkbox])){
            $this->$checkbox = false;
          }else{
            $this->$checkbox = true;
          }
        }
      }
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("IMAP/POP3"),
          "plDescription" => _("IMAP/POP3")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 94,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(

            "start"           => _("Start"),
            "stop"            => _("Stop"),
            "restart"         => _("Restart"),
            "repair_database" => _("Repair database"),

            "goImapName"        =>_("Server identifier"),
            "goImapConnect"     =>_("Connect URL"),
            "goImapAdmin"       =>_("Admin user"),
            "goImapPassword"    =>_("Admin password"),
            "goImapSievePort"   =>_("Sieve port"),
            "cyrusImap"         =>_("Start IMAP service"),
            "cyrusImapSSL"      =>_("Start IMAP SSL service"),
            "cyrusPop3"         =>_("Start POP3 service"),
            "cyrusPop3SSL"      =>_("Start POP3 SSL service"))
          ));
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
