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
    var $conflicts            = array("goImapServer");

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);

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
            if(isset($this->Actions[get_post('action')])){

                $tmp = $this->plInfo();
                foreach($tmp['plProvidedAcls'] as $name => $translation) {
                    if(preg_match("/^".get_post('action')."$/i",$name)){
                        if($this->acl_is_writeable($name)){
                            $this->setStatus(set_post('action'));
                        }
                    }
                }

            }
        }

        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));  
        }

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        $smarty->assign("Actions", set_post($this->Actions));
        $smarty->assign("is_new",$this->dn);
        $smarty->assign("is_acc",$this->initially_was_account);
        return($smarty->fetch(get_template_path("goImapServer.tpl",TRUE,dirname(__FILE__))));
    }

    function getListEntry()
    {
        $fields               = goService::getListEntry();
        $fields['Message']    = _("IMAP/POP3 (Cyrus) service");
#$fields['AllowRemove']= true;
#$fields['AllowEdit']  = true;
        return($fields);
    }

    function check()
    { 
        $message = plugin::check();
        if(empty($this->goImapName)){
            $message[] = msgPool::required(_("Server identifier"));
        } 

        if(empty($this->goImapConnect)){
            $message[] = msgPool::required(_("Connect URL"));
        }elseif(!preg_match('/^\{[^:]+:[0-9]*\/.*\}$/', $this->goImapConnect)){
            $message[]= msgPool::invalid(_("Connect URL"),"","","{server-name:port/options}");
        } 

        if(empty($this->goImapSieveServer)){
            $message[] = msgPool::required(_("Sieve connect URL"));
        }elseif(!preg_match('/^\{[^:]+:[0-9]*\/(no|)tls\}$/', $this->goImapSieveServer)){
            $message[]= msgPool::invalid(_("Sieve connect URL"),"","","{server-name:port/options}"."&nbsp;".
                    sprintf(_("Valid options are: %s"),"tls,notls"));
        } 

        if(empty($this->goImapAdmin)){
            $message[] = msgPool::required(_("Administrator"));
        }  
        if(empty($this->goImapPassword)){
            $message[] = msgPool::required(_("Password"));
        } 

#   if(empty($this->goImapSievePort)){
#     $message[] = msgPool::required(_("Sieve port"));
#   }elseif (!preg_match('/^[0-9]+$/', $this->goImapSievePort)){
#     $message[]= msgPool::invalid(_("Sieve port"),$this->goImapSievePort,"/[0-9]/");
#   }

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
            $this->goImapConnect = trim($this->goImapConnect);
            $this->goImapSieveServer = trim($this->goImapSieveServer);
        }
    }


    /* Save service */
    function save()
    {

        plugin::save();

        $this->attrs['goImapSievePort']  = preg_replace("/^\{[^:]+:([0-9]*)\/.*$/","\\1",$this->goImapSieveServer);

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
        if($this->initially_was_account){
            new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            $this->handle_post_events("modify");
        }else{
            $this->handle_post_events("add");
            new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
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
                    "plRequirements"=> array(
                        'ldapSchema' => array('goImapServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(),'mailAccount','mailogroup','mailgroup')
                        ),

                    "plProvidedAcls"=> array(

                        "start"           => _("Start"),
                        "stop"            => _("Stop"),
                        "restart"         => _("Restart"),
                        "repair_database" => _("Repair database"),

                        "goImapName"        =>_("Server identifier"),
                        "goImapConnect"     =>_("Connect URL"),
                        "goImapAdmin"       =>_("Administrator"),
                        "goImapPassword"    =>_("Administrator password"),
                        //            "goImapSievePort"   =>_("Sieve port"),
                        "goImapSieveServer"=>_("Sieve connect URL"),
                        "cyrusImap"         =>_("Start IMAP service"),
                        "cyrusImapSSL"      =>_("Start IMAP SSL service"),
                        "cyrusPop3"         =>_("Start POP3 service"),
                        "cyrusPop3SSL"      =>_("Start POP3 SSL service"))
                        ));
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
