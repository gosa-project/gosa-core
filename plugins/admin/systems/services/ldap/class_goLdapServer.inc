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
    var $oldBase             = ""; 
    var $view_logged  =FALSE;

    var $krb_service_prefix = "ldap/";

    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("LDAP service");

        /* Suggest a default value */
        if ($this->goLdapBase == ""){
            if ($this->dn == ""){
                $srv= _("enter your servers DNS name");
            } else {
                $srv= preg_replace("/^cn=([^,]+),.*$/", "$1", $this->dn);
            }
            $this->goLdapBase= "ldap://$srv:389/".$config->current['BASE'];
        }
        $this->oldBase = $this->goLdapBase;
    }


    function execute()
    { 

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        $smarty = get_smarty(); 
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
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
        $fields['Message']    = _("LDAP service");
        return($fields);
    }



    function check()
    { 
        $message = plugin::check();
        if(empty($this->goLdapBase)){
            $message[] = msgPool::required(_("LDAP URI"));
        }
        if(!preg_match("/^ldap[si]?:\/\/[0-9a-z_.-]{1,}+(:[0-9]+)?\/.+$/i", $this->goLdapBase)){
            $message[] = msgPool::invalid(_("LDAP URI"),"","","(ldap|ldaps|ldapi)://"._("Server")."(:"._("Port").")/"._("Base"));
        }
        return($message);
    }


    function save_object()
    {
        if(isset($_POST['goLdapServerPosted'])){
            plugin::save_object();
        }
    } 


    function save()
    {
        goService::save();

        /* Update object relation */
        if(!empty($this->oldBase) && $this->oldBase != $this->goLdapBase){
            $new = array();
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search('(&(objectClass=GOhard)(gotoLdapServer=*'.$this->oldBase.'))',array("cn","gotoLdapServer"));
            while($attrs = $ldap->fetch()){
                $ldapServer = array();
                for($i = 0 ; $i < $attrs['gotoLdapServer']['count'] ; $i ++ ){
                    $ldapServer[] = preg_replace("/".preg_quote($this->oldBase, '/')."/",$this->goLdapBase,$attrs['gotoLdapServer'][$i]);
                }
                $new[$attrs['dn']]['gotoLdapServer'] = $ldapServer;
            }
            foreach($new as $dn => $data){
                $ldap->cd($dn);
                $ldap->modify($data); 
                if(!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
                }
            }
        }
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("LDAP"),
                    "plDescription" => _("LDAP service").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 96,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('goLdapServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),

                    "plProvidedAcls"=> array(
                        "start"         => _("Start"),
                        "stop"          => _("Stop"),
                        "restart"       => _("Restart"),
                        "goLdapBase" => _("LDAP base"))
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
