<?php

class goCupsServer extends goService{

    var $cli_summary      = "This plugin is used within the ServerService Pluign \nand indicates that this server supports cups management.";
    var $cli_description  = "Some longer text\nfor help";
    var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goCupsServer");
    var $attributes       = array();
    var $StatusFlag       = "goCupsServerStatus";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("goCupsServer");

    var $DisplayName      = "";
    var $dn               = NULL;
    var $acl;
    var $cn                  = "";
    var $goCupsServerStatus  = "";
    var $view_logged  =FALSE;

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("Print service");
    }


    function execute()
    { 
        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        $smarty = get_smarty(); 
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }
        return($smarty->fetch(get_template_path("goCupsServer.tpl",TRUE,dirname(__FILE__))));
    }

    function getListEntry()
    {
        $fields = goService::getListEntry();
        $fields['Message']    = _("Print service");
#$fields['AllowEdit']  = false;
        return($fields);
    }

    function check()
    { 
        $message = plugin::check();
        return($message);
    }

    function save_object()
    {
        plugin::save_object();
    }  

    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Cups"),
                    "plDescription" => _("Print service")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 100,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('goCupsServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),

                    "plProvidedAcls"=> array(
                        "start"         => _("Start"),
                        "stop"          => _("Stop"),
                        "restart"       => _("Restart"),
                        )
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
