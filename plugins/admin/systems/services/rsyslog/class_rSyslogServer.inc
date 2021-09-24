<?php

class rSyslogServer extends goService{

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goSyslogServer");
    var $attributes       = array();
    var $db_attributes    = array("gosaLogDB","goLogAdmin","goLogPassword");
    var $StatusFlag       = "";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("rSyslogServer");

    var $DisplayName      = "";
    var $dn               = NULL;
    var $acl;
    var $cn                 = "";
    var $use_database = FALSE;
    var $initial_use_database = FALSE;
    var $gosaLogDB     = "Syslog";
    var $goLogAdmin    = "rsyslog";
    var $goLogPassword = "";  
    var $view_logged   = FALSE;


    function __construct(&$config,$dn)
    {
        goService::__construct($config,$dn);
        $this->DisplayName = _("Logging");
        $this->initial_use_database = $this->use_database = 
            isset($this->attrs['objectClass']) &&in_array_strict("goLogDBServer", $this->attrs['objectClass']);

        if($this->use_database){
            foreach($this->db_attributes as $attr){
                if(isset($this->attrs[$attr][0])){
                    $this->$attr = $this->attrs[$attr][0];
                }
            }
        }
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

        foreach($this->db_attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }
        $smarty->assign("use_database" , $this->use_database);
        return($smarty->fetch(get_template_path("rSyslogServer.tpl",TRUE,dirname(__FILE__))));
    }


    function getListEntry()
    {
        $fields = goService::getListEntry();
        $fields['Message']      = _("Logging service");
#$fields['AllowEdit']    = true;
#$fields['AllowStart']   = FALSE;
#$fields['AllowStop']    = FALSE;
#$fields['AllowRestart'] = FALSE;
        return($fields);
    }


    function check()
    { 
        $message = plugin::check();
        if($this->use_database){
            if (empty($this->goLogAdmin)){
                $message[]= msgPool::required(_("User"));
            }
            if (empty($this->gosaLogDB)){
                $message[]= msgPool::required(_("Database"));
            }
            if (empty($this->goLogPassword)){
                $message[]= msgPool::required(_("Password"));
            }
        }
        return($message);
    }


    function save_object()
    {
        if(isset($_POST['rSyslogServerPosted'])){
            plugin::save_object();

            foreach($this->db_attributes as $attr){
                if(isset($_POST[$attr])){
                    $this->$attr = get_post($attr);
                }
            }
            $this->use_database = isset($_POST['use_database']);
        }
    }  


    function save()
    {
        // Store rSyslog database connection info if enabled.
        if($this->use_database){
            $this->attributes = array('gosaLogDB','goLogAdmin','goLogPassword');
            $this->objectclasses = array('goSyslogServer','goLogDBServer');
        }else{
            $this->attributes = array();
            $this->objectclasses = array('goSyslogServer');

        }

        plugin::save();

        if(!$this->use_database && $this->initial_use_database){
            $this->attrs['objectClass'] = array_remove_entries(array('goLogDBServer'),$this->attrs['objectClass']);
            $this->attrs['objectClass'] = array_values($this->attrs['objectClass']);
            foreach($this->db_attributes as $attr){
                $this->attrs[$attr] = array();
            }
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


    /* Remove service */
    function remove_from_parent()
    {
        // Store rSyslog database connection info if enabled.
        if($this->initial_use_database){
            $this->attributes = array('gosaLogDB','goLogAdmin','goLogPassword');
            $this->objectclasses = array('goSyslogServer','goLogDBServer');
        }else{
            $this->attributes = array();
            $this->objectclasses = array('goSyslogServer');
        }

        goService::remove_from_parent();
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Logging service"),
                    "plDescription" => _("Logging service")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 99,
                    "plRequirements"=> array(
                        'ldapSchema' => array('goSyslogServer' => '>=2.7','goLogDBServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(),'rsyslog'),
                        ),
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),

                    "plProvidedAcls"=> array(
                        "gosaLogDB" => _("Database"),
                        "goLogAdmin"     => _("User"),
                        "goLogPassword" => _("Password"), 
                        "viewEntries"     => _("Allow view of entries on this server")) 
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
