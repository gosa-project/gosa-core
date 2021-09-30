<?php 
class ArpNewDevice extends plugin
{
    var $cn= "";
    var $description = "";
    var $netConfigDNS = NULL;
    var $attributes= array("cn","description");
    var $objectclasses = array("gotoHard");
    var $ignore_account = TRUE;

    var $ObjectGroups   = array();
    var $SystemTypes       = array();

    var $ObjectGroup    = "none";
    var $SystemType     = "gotoWorkstation";

    var $gotoIntegration = FALSE;

    function __construct($config, $dn= NULL, $parent= NULL)
    {
        plugin :: plugin($config,$dn);
        $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
        $this->acl = "#all#";
        $this->netConfigDNS->acl = $this->acl;
        $this->netConfigDNS->force_dns();

        /* Get object groups */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("cn"));
        $tmp= array();
        while($attrs = $ldap->fetch()){
            $tmp[$attrs['dn']]= $attrs['cn'][0];
        }
        asort($tmp, SORT_LOCALE_STRING);
        $this->ObjectGroups= $tmp;

        $this->SystemTypes =array("gotoWorkstation"=>_("Workstation"), "gotoTermminal"=>_("Terminal"), "goServer"=>_("Server"));
        if(class_available("opsi")){
            $this->SystemTypes["FAKE_OC_OpsiHost"]= _("Windows workstation");
        }
    }

    function execute()
    {
        plugin::execute();

        /* Get object groups */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaGroupOfNames)(cn=*))",array("gosaGroupObjects","cn"));
        $tmp= array("W" => array(),"T" => array(), "S" => array(),"O" => array());
        while($attrs = $ldap->fetch()){
            $tmp[preg_replace("/[\[\] ]/","",$attrs['gosaGroupObjects'][0])][$attrs['dn']] = $attrs['cn'][0];
        }

        $smarty = get_smarty();
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }
        $this->netConfigDNS->cn= $this->cn;

        $map = array("gotoWorkstation" => "W","gotoTerminal" => "T","goServer" => "S", "FAKE_OC_OpsiHost" => "O");
        $smarty->assign("netconfig", $this->netConfigDNS->execute());
        $smarty->assign("ogroups",         set_post($tmp[$map[$this->SystemType]]));
        $smarty->assign("SystemTypes",     set_post($this->SystemTypes));
        $smarty->assign("SystemTypeKeys",  set_post(array_flip($this->SystemTypes))); 
        $smarty->assign("ObjectGroup",     set_post($this->ObjectGroup));
        $smarty->assign("SystemType",      set_post($this->SystemType));

        $smarty->assign("gotoIntegration",$this->gotoIntegration);

        /* Display sub dialog from network settings */
        $this->netConfigDNS->acl = $this->acl;
        if($this->netConfigDNS->dialog){
            $this->dialog = TRUE;
            return($this->netConfigDNS->execute());
        }else{
            $this->dialog = FALSE;
        }

        return($smarty->fetch (get_template_path('ArpNewDevice.tpl', TRUE,dirname(__FILE__))));
    }

    function check()
    {
        $message= plugin::check();
        $message= array_merge($message, $this->netConfigDNS->check());
        if(empty($this->cn)){
            $message[] = msgPool::required(_("Name"));
        }
        return($message);
    }

    function save_object()
    {
        if(isset($_POST['ArpNewDevice_posted'])){
            plugin::save_object();
            $this->netConfigDNS->save_object();

            $this->gotoIntegration = isset($_POST['gotoIntegration']);
            if($this->gotoIntegration){
                if(isset($_POST['SystemType'])){
                    $this->SystemType = get_post('SystemType');
                }
                if(isset($_POST['ObjectGroup'])){
                    $this->ObjectGroup = get_post('ObjectGroup');
                }
            }
        }
    }

    function save()
    {

        $this->netConfigDNS->acl = $this->acl;
        plugin::save();
        $this->netConfigDNS->cn = $this->cn;    
        $this->netConfigDNS->save($this->dn);

        // Do not remove this Arp-Device if gotoIntegration was selected.
        if(!$this->gotoIntegration){ 

            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->rmdir_recursive($this->dn);
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $key->dn, LDAP_DEL, get_class()));
            }
        }
    }


    /*! \brief  !! Incoming dummy acls, required to defined acls for incoming objects
     */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Unknown incoming objects"),
                    "plDescription" => _("Unknown incoming objects"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 99,
                    "plSection"     => array("administration"),

                    "plProperties" =>
                    array(
                        array(
                            "name"          => "systemIncomingRDN",
                            "type"          => "rdn",
                            "default"       => "ou=incoming,",
                            "description"   => _("RDN for storing new devices."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_systemIncomingRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)),

                    "plCategory"    => array("incoming"   => array( "description"  => _("Incoming"))),
                    "plProvidedAcls"=> array(
                            "cn"  => _("Name"),
                            "description"  => _("Description")

                            )
                        ));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
