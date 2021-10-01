<?php

class goFaxServer extends goService {

    var $cli_summary      = "This pluign is used within the ServerService Pluign \nand indicates that this server supports fax informations.";
    var $cli_description  = "Some longer text\nfor help";
    var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goFaxServer");
    var $attributes       = array("goFaxAdmin", "goFaxPassword");
    var $StatusFlag       = "goFaxServerStatus";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("goFaxServer");

    var $DisplayName      = "";
    var $dn               = NULL;
    var $acl;

    var $goFaxAdmin         = "";
    var $goFaxPassword      = "";
    var $goFaxServerStatus  = "";
    var $cn                 = "";
    var $view_logged  =FALSE;

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("FAX database");
    }


    function execute()
    { 
        $smarty = get_smarty(); 

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        /* Assign acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation ){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }
        return($smarty->fetch(get_template_path("goFaxServer.tpl",TRUE,dirname(__FILE__))));
    }


    function getListEntry()
    {
        $fields               = goService::getListEntry();
        $fields['Message']    = _("FAX database configuration");
#$fields['AllowEdit']  = true;
        return($fields);
    }

    function check()
    { 
        $message = plugin::check();
        if (empty($this->goFaxAdmin)){
            $message[]= msgPool::required(_("User"));
        }
        if (empty($this->goFaxPassword)){
            $message[]= msgPool::required(_("Password"));
        }
        return($message);
    }


    function save_object()
    {
        if(isset($_POST['goFaxServerPosted'])){
            plugin::save_object();
        }
    } 

    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Fax database"),
                    "plDescription" => _("Fax database")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 99,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('goFaxServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(),'faxreport','gofaxAccount')
                        ),

                    "plProvidedAcls"=> array(
                        "goFaxAdmin"     => _("Login name"),
                        "goFaxPassword"  => _("Password"),

                        "start"         => _("Start"),
                        "stop"          => _("Stop"),
                        "restart"       => _("Restart"))
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
