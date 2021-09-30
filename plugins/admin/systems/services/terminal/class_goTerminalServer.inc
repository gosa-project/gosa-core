<?php

class goTerminalServer extends goService{

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goTerminalServer");
    var $attributes       = array("goXdmcpIsEnabled","gotoSessionType");
    var $StatusFlag       = "goTerminalServerStatus";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("goTerminalServer");

    var $DisplayName      = "";
    var $dn               = NULL;
    var $acl;
    var $cn                      = "";
    var $goTerminalServerStatus  = "";
    var $goXdmcpIsEnabled        = false;  
    var $view_logged             = FALSE;

    var $gotoSessionType         = array();

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("Terminal service");

        $tmp = array();
        if(isset($this->attrs['gotoSessionType'])){
            for($i =0; $i < $this->attrs['gotoSessionType']['count'] ; $i++){
                $tmp[] = $this->attrs['gotoSessionType'][$i];
            }
        }
        $this->gotoSessionType = $tmp;
#$this->gotoSessionTypes= array("LDM","XDMCP","RDP","CITRIX"); 
        $this->gotoSessionTypes= array("LDM","XDMCP", "SHELL", "RDP", "TELNET"); 
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
            $smarty->assign($attr, set_post($this->$attr));
        }

        $tmp = array();
        foreach($this->gotoSessionTypes as $type){
            if(in_array_strict($type,$this->gotoSessionType)){
                $tmp[$type] = TRUE;
            }else{
                $tmp[$type] = FALSE;
            }
        }
        $smarty->assign("gotoSessionTypes", set_post($tmp));
        return($smarty->fetch(get_template_path("goTerminalServer.tpl",TRUE,dirname(__FILE__))));
    }


    function getListEntry()
    {
        $fields               = goService::getListEntry();
        $fields['Message']    = _("Terminal service");
#$fields['AllowEdit']  = true;
        return($fields);
    }


    function save()
    {
        plugin::save();

        if(!$this->goXdmcpIsEnabled){
            $this->attrs['goXdmcpIsEnabled'] = "0";
        }

        $this->attrs['gotoSessionType'] = array_values($this->gotoSessionType);

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
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
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
        return($message);
    }


    function save_object()
    {
        if(isset($_POST['goTerminalServerPosted'])){
            plugin::save_object();

            $this->gotoSessionType = array();
            foreach($this->gotoSessionTypes as $attr){
                if(isset($_POST['gotoSessionType_'.$attr])){
                    $this->gotoSessionType[] = $attr;
                }
            }

            if($this->acl_is_writeable("goXdmcpIsEnabled")){
                if(isset($_POST['goXdmcpIsEnabled'])){
                    $this->goXdmcpIsEnabled = true;
                }else{
                    $this->goXdmcpIsEnabled = false;
                }
            }
        }
    } 


    function PrepareForCopyPaste($obj)
    {
        plugin::PrepareForCopyPaste($obj);
        $tmp = array();
        if(isset($obj['gotoSessionType'])){
            for($i =0; $i < $obj['gotoSessionType']['count'] ; $i++){
                $tmp[] = $obj['gotoSessionType'][$i];
            }
        }
        $this->gotoSessionType = $tmp;
#$this->gotoSessionTypes= array("LDM","XDMCP","RDP","CITRIX"); 
        $this->gotoSessionTypes= array("LDM","XDMCP", "SHELL", "TELNET"); 
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
                    "plRequirements"=> array(
                        'ldapSchema' => array('goTerminalServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plCategory"    => array("server"),


                    "plProvidedAcls"=> array(
                        "start"           => _("Start"),
                        "stop"            => _("Stop"),
                        "restart"         => _("Restart"),
                        "goXdmcpIsEnabled"  => _("Temporary disable login"))
                    ));
    }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
