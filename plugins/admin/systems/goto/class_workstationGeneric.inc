<?php

class workgeneric extends plugin
{
    /* Generic terminal attributes */
    var $gotoMode= "locked";
    var $initial_gotoMode= "locked";
    var $gotoSyslogServer= "";
    var $gotoSyslogServers= array();
    var $gotoNtpServer= array();
    var $gotoNtpServers= array();
    var $gotoSndModule= "";
    var $gotoFloppyEnable= "";
    var $gotoCdromEnable= "";
    var $description= "";
    var $ghCpuType= "-";
    var $ghMemSize= "-";
    var $ghUsbSupport= "-";
    var $ghNetNic= array();
    var $ghIdeDev= array();
    var $ghScsiDev= array();
    var $ghGfxAdapter= "-";
    var $ghSoundAdapter= "-";
    var $gotoLastUser= "-";
    var $FAIscript= "";
    var $view_logged = FALSE;
    var $auto_activate= FALSE;

    /* Needed values and lists */
    var $base= "";
    var $cn= "";
    var $l= "";
    var $orig_dn= "";
    var $orig_cn= "";
    var $orig_base= "";

    /* Plugin side filled */
    var $modes= array();

    var $netConfigDNS;
    var $baseSelector;

    var $inheritTimeServer = true;

    /* attribute list for save action */
    var $ignore_account= TRUE;
    var $attributes= array("gotoMode", "gotoSyslogServer", "gotoNtpServer",
            "gotoFloppyEnable", "gotoCdromEnable", "cn", "gotoSndModule",
            "ghCpuType", "ghMemSize", "ghUsbSupport", "description",
            "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser", "l","FAIscript");
    var $objectclasses= array("top", "gotoWorkstation", "GOhard");

    var $validActions   = array("reboot" => "", "localboot" => "", "halt" => "", "update" => "", "reinstall" => "",
            "rescan" => "", "wakeup" => "", "memcheck" => "", "sysinfo" => "");

    var $fai_activated = FALSE;

    var $member_of_ogroup = FALSE;

    var $currently_installing = FALSE;
    var $currently_installing_warned = FALSE;

    var $kerberos_key_service = NULL;

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        $this->fai_activated = $config->pluginEnabled("faiManagement");

        plugin::__construct ($config, $dn, $parent);

        if(class_available("krbHostKeys")){
            $this->kerberos_key_service = new krbHostKeys($this->config,$this);
        }

        if(!isset($this->parent->by_object['ogroup'])){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd ($this->config->current['BASE']);
            $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".LDAP::prepare4filter($this->dn)."))",array("cn"));
            $this->member_of_ogroup = $ldap->count() >= 1;
        }

        $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
        $this->netConfigDNS->MACisMust =TRUE;

        /* Read arrays */
        foreach (array("ghNetNic", "ghIdeDev", "ghScsiDev") as $val){
            if (!isset($this->attrs[$val])){
                continue;
            }
            for ($i= 0; $i<$this->attrs[$val]['count']; $i++){
                array_push($this->$val, $this->attrs[$val][$i]);
            }
        }

        /* Create used ntp server array */
        $this->gotoNtpServer= array();
        if(isset($this->attrs['gotoNtpServer'])){
            $this->inheritTimeServer = false;
            unset($this->attrs['gotoNtpServer']['count']);
            foreach($this->attrs['gotoNtpServer'] as $server){
                $this->gotoNtpServer[$server] = $server;
            }
        }

        /* Set inherit checkbox state */
        if((in_array_strict("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer) == 0)){
            $this->inheritTimeServer = true;
            $this->gotoNtpServer=array();
        }

        /* You can't inherit the NTP service, if we are not member in an object group */
        if(!$this->member_of_ogroup){
            $this->inheritTimeServer = FALSE;
        }

        /* Create available ntp options */
        $tmp = $this->config->data['SERVERS']['NTP'];
        $this->gotoNtpServers = array();
        foreach($tmp as $key => $server){
            if($server == "default") continue;
            $this->gotoNtpServers[$server] = $server;
        }

        $this->modes["active"]= _("Activated");
        $this->modes["locked"]= _("Locked");

        /* Set base */
        if ($this->dn == "new"){
            $ui= get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } elseif(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $this->dn)){
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", "", $this->dn);
        }else{
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("workgeneric", "workstationRDN"), '/')."/i", "", $this->dn);
        }

        /* Create an array of all Syslog servers */
        $tmp = $this->config->data['SERVERS']['SYSLOG'];
        foreach($tmp as $server){
            $visible = $server;
            if($server == "default" && $this->member_of_ogroup) {
                $visible = "["._("inherited")."]";
            }
            $this->gotoSyslogServers[$server] = $visible;
        }

        $this->initial_gotoMode = $this->gotoMode;

        /* Save 'dn' for later referal */
        $this->orig_dn= $this->dn;
        $this->orig_cn= $this->cn;
        $this->orig_base= $this->base;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
    }


    function set_acl_base($base)
    {
        plugin::set_acl_base($base);
        $this->netConfigDNS->set_acl_base($base);
    }

    function set_acl_category($cat)
    {
        plugin::set_acl_category($cat);
        $this->netConfigDNS->set_acl_category($cat);
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        $theme = getThemeName();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","workstation/".get_class($this),$this->dn);
        }

        /* Do we need to flip is_account state? */
        if(isset($_POST['modify_state'])){
            if($this->is_account && $this->acl_is_removeable()){
                $this->is_account= FALSE;
            }elseif(!$this->is_account && $this->acl_is_createable()){
                $this->is_account= TRUE;
            }
        }

        if ((isset($_POST['action'])) && ($this->acl_is_writeable("FAIstate")) && isset($this->validActions[$_POST['saction']]) ){
            $action= get_post('saction');

            /* Check if we have an DaemonEvent for this action */ 
            if(class_available("DaemonEvent")){
                $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
                if(isset($events['TRIGGERED']["DaemonEvent_".$action])){
                    $evt = $events['TRIGGERED']["DaemonEvent_".$action];
                    $tmp = new $evt['CLASS_NAME']($this->config);
                    $tmp->add_targets(array($this->netConfigDNS->macAddress));
                    $tmp->set_type(TRIGGERED_EVENT);
                }
            } else {
                msg_dialog::display(_("Event error"),
                        sprintf(_("Event '%s' is not available!"),$action),ERROR_DIALOG);
            }


        }

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            switch ($theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
            }
            $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("workstation"))."</div>";
            return $display;
        }

        /* Add new ntp Server to our list */ 
        if((isset($_POST['addNtpServer'])) && (isset($_POST['gotoNtpServers'])) && $this->acl_is_writeable("gotoNtpServer")){
            $this->gotoNtpServer[$_POST['gotoNtpServers']] = get_post('gotoNtpServers');
        }

        /* Delete selected NtpServer for list of used servers  */
        if((isset($_POST['delNtpServer'])) && (isset($_POST['gotoNtpServerSelected'])) && $this->acl_is_writeable("gotoNtpServer")){
            foreach($_POST['gotoNtpServerSelected'] as $name){
                unset($this->gotoNtpServer[$name]);
            }
        }

        /* Fill templating stuff */
        $smarty= get_smarty();


        /* Set acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("description", set_post($this->description));
        $smarty->assign("l", set_post($this->l));

        $tmp = array();
        foreach($this->gotoNtpServers as $server){
            if(!in_array_strict($server,$this->gotoNtpServer)){
                $tmp[$server] = $server;
            }
        }
        $smarty->assign("gotoNtpServers", set_post($tmp));

        /* Check if workstation is online - actions won't to anything useful since the daemon isn't supported anymore */

	$smarty->assign("actions", 
			set_post(
				array(
					"wakeup" => _("Wake up"),
					"reinstall" => _("Reinstall"),
					"update" => _("Software update"),
					"localboot" => _("Force local boot"),
				     )
				)
		       );

        /* Arrays */
        $smarty->assign("modes",        set_post($this->modes));
        $smarty->assign("nfsservers",   set_post($this->config->data['SERVERS']['NFS']));
        $smarty->assign("syslogservers",set_post($this->gotoSyslogServers));
        $smarty->assign("fai_activated",set_post($this->fai_activated));

        $smarty->assign("si_activated",FALSE);

        $ntpser = array();
        foreach($this->gotoNtpServers as $server){
            if(!in_array_strict($server,$this->gotoNtpServer)){
                $ntpser[$server] = set_post($server);
            }
        }
        $smarty->assign("gotoNtpServers", $ntpser);

        /* Variables */
        foreach(array("gotoMode", "gotoSyslogServer", "gotoNtpServer") as $val){
            $smarty->assign($val."_select", set_post($this->$val));
        }
        $smarty->assign("base", $this->baseSelector->render());

        /* tell smarty the inherit checkbox state */
        $smarty->assign("inheritTimeServer",$this->inheritTimeServer);
        $smarty->assign("member_of_ogroup", $this->member_of_ogroup);

        $str = $this->netConfigDNS->execute();
        if(is_object($this->netConfigDNS->dialog)){
            return($str);
        }
        $smarty->assign("netconfig", $str);

        /* Display kerberos host key options */
        $smarty->assign("host_key","");
        if(is_object($this->kerberos_key_service)){
            $smarty->assign("host_key",$this->kerberos_key_service->execute_by_prefix("host/"));
        }

        /* Show main page */
        $smarty->assign("currently_installing", $this->currently_installing);
        return($smarty->fetch (get_template_path('workstation.tpl', TRUE, dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        if($this->acl_is_removeable()){

            $this->netConfigDNS->remove_from_parent();
            $ldap= $this->config->get_ldap_link();
            $ldap->rmdir($this->dn);
            new log("remove","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
            }

            /* Remove kerberos key dependencies too */
            if(is_object($this->kerberos_key_service)){
                $this->kerberos_key_service->remove_from_parent_by_prefix("host/");
            }

            /* Optionally execute a command after we're done */
            $this->handle_post_events("remove", array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

            /* Delete references to object groups */
            $ldap->cd ($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
            while ($ldap->fetch()){
                $og= new ogroup($this->config, $ldap->getDN());
                unset($og->member[$this->dn]);
                $og->save ();
            }

            /* Remove all accessTo/trust dependencies */
            update_accessTo($this->cn,"");
        }


        if(isset($_POST["inheritAll"])){
            $this->set_everything_to_inherited();
        }
    }


    /* Save data to object */
    function save_object()
    {

        /* Create a base backup and reset the
           base directly after calling plugin::save_object();
           Base will be set seperatly a few lines below */
        $base_tmp = $this->base;
        plugin::save_object();
        $this->base = $base_tmp;

        /* Refresh base */
        if ($this->acl_is_moveable($this->base)){
            if (!$this->baseSelector->update()) {
                msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
            }
            if ($this->base != $this->baseSelector->getBase()) {
                $this->base= $this->baseSelector->getBase();
                $this->is_modified= TRUE;
            }
        }

        $this->netConfigDNS->save_object();

        /* Set inherit mode */
        if((isset($_POST['workgeneric_posted'])) && ($this->acl_is_writeable("gotoNtpServer"))){
            if(isset($_POST["inheritTimeServer"]) && $this->member_of_ogroup){
                $this->inheritTimeServer = true;
            }else{
                $this->inheritTimeServer = false;
            }
        }

        if(isset($_POST["inheritAll"])){
            $this->set_everything_to_inherited();
        }

        /* Hanle kerberos host key plugin */
        if(is_object($this->kerberos_key_service)){
            $this->kerberos_key_service->save_object_by_prefix("host/");
        }
    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        /* Skip IP & Mac checks if this is a template */
        if($this->cn != "wdefault"){
            $message= array_merge($message, $this->netConfigDNS->check());
        }

        $this->dn= "cn=".$this->cn.",".get_ou("workgeneric", "workstationRDN").$this->base;

        if ($this->cn == ""){
            $message[]= msgPool::required(_("Name"));
        }

        /* Check if given name is a valid host/dns name */
        $regex = $reason = "";
        if(!tests::is_dns_name($this->cn, $reason, $regex)){
            if($reason == 1){
                $message[]= sprintf(_("The full qualified domain name is too long!"));
            }elseif($reason == 2){
                $message[]= sprintf(_("The hostname name is too long!"));
            }else{
                $message[]= msgPool::invalid(_("Name"), $this->cn, "/{$regex}/i");
            }
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();
        }

        if ($this->orig_dn != $this->dn){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($this->base);

            if($this->cn == "wdefault"){
                $ldap->cat($this->dn);
            }else{
                $ldap->search ("(&(cn=".$this->cn.")(objectClass=gotoWorkstation))", array("cn"));
            }
            if ($ldap->count() != 0){
                while ($attrs= $ldap->fetch()){

                    $matchArp = FALSE;
                    if(class_available("ArpNewDevice") &&
                            preg_match ("/,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $ldap->getDN())){
                        $matchArp = TRUE;
                    } 

                    if (preg_match("/cn=dhcp,/",$attrs['dn']) || $matchArp){
                        continue;
                    } else {
                        if ($attrs['dn'] != $this->orig_dn){
                            $message[]= msgPool::duplicated(_("Name"));
                            break;
                        }
                    }
                }
            }
        }

        /* Check for valid ntpServer selection */
        if((!$this->inheritTimeServer) && (!count($this->gotoNtpServer))){
            $message[]= msgPool::required(_("NTP server"));
        }

        /* Only systems with a valid ldap handle can be activated 
         */
        if($this->gotoMode == "active" && $this->initial_gotoMode != "active"){

            if(isset($this->parent->by_object['workstartup']) &&
                    !count($this->parent->by_object['workstartup']->gotoLdapServers) && 
                    !$this->parent->by_object['workstartup']->gotoLdap_inherit){

                $message[] = _("A valid LDAP server assignment is missing!");
            }
        }else{
            /* Warn the user, that this host is currently installing */
            if($this->currently_installing && !$this->currently_installing_warned && 
                    !preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i",$this->orig_dn)){

                /* Force aborting without message dialog */
                $message[] = "";
                $this->currently_installing_warned = TRUE;
                msg_dialog::display(_("Software deployment"), 
                        _("This host is currently installing. If you want to save it, press 'OK'."),
                        CONFIRM_DIALOG);
            }
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        /* Detect mode changes */
        $activate= (isset($this->saved_attributes['gotoMode']) &&
                $this->gotoMode != $this->saved_attributes['gotoMode'] &&
                $this->gotoMode == "active" &&
                tests::is_ip($this->netConfigDNS->ipHostNumber)) || $this->auto_activate;
        plugin::save();

        /* Strip out 'default' values */
        foreach (array("gotoSyslogServer") as $val){

            if (isset($this->attrs[$val]) && $this->attrs[$val] == "default"){
                $this->attrs[$val]= array();
            }
        }

        /* Add missing arrays */
        foreach (array("ghScsiDev", "ghIdeDev", "ghNetNic") as $val){
            if (isset ($this->$val) && count ($this->$val) != 0){
                $this->attrs["$val"]= $this->$val;
            }
        }

        /* Remove all empty values */
        if ($this->orig_dn == 'new'){
            $attrs= array();
            foreach ($this->attrs as $key => $val){
                if (is_array($val) && count($val) == 0){
                    continue;
                }
                $attrs[$key]= $val;
            }
            $this->attrs= $attrs;
        }

        /* Update ntp server settings */
        if($this->inheritTimeServer){
            if($this->is_new){
                if(isset($this->attrs['gotoNtpServer'])){
                    unset($this->attrs['gotoNtpServer']);
                }
            }else{
                $this->attrs['gotoNtpServer'] = array();
            }
        }else{
            /* Set ntpServers */
            $this->attrs['gotoNtpServer'] = array();
            foreach($this->gotoNtpServer as $server){
                $this->attrs['gotoNtpServer'][] = $server;
            }
        }

        /* cn=default and macAddress=- indicates that this is a template */
        if($this->cn == "wdefault"){
            $this->netConfigDNS->macAddress = "-";
        }

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        if ($this->orig_dn == 'new'){
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
            new log("create","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
            }

            $this->netConfigDNS->cn = $this->cn;
            $this->netConfigDNS->save();

            $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
        } else {
            if ($this->orig_dn != $this->dn){

                /* Remove all accessTo/trust dependencies */
                update_accessTo($this->orig_cn,$this->cn);
            }
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
            }
            new log("modify","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

            $this->netConfigDNS->cn = $this->cn;
            $this->netConfigDNS->save();

            $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
        }

        if ($activate && class_available("DaemonEvent")){

            /* Send installation activation
             */
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
            if(isset($events['TRIGGERED']['DaemonEvent_installation_activation'])){
                $evt = $events['TRIGGERED']['DaemonEvent_installation_activation'];
                $tmp = new $evt['CLASS_NAME']($this->config);
                $tmp->set_type(TRIGGERED_EVENT);
                $tmp->add_targets(array($this->netConfigDNS->macAddress));
            
            }
        }
    }


    /* Display generic part for server copy & paste */
    function getCopyDialog()
    {
        $vars = array("cn");
        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("object","workstation");
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    function saveCopyDialog()
    {
        if(isset($_POST['cn'])){
            $this->cn = get_post('cn');
        }
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);
        if(isset($source['macAddress'][0])){
            $this->netConfigDNS->macAddress = $source['macAddress'][0];
        }
        if(isset($source['ipHostNumber'][0])){
            $this->netConfigDNS->ipHostNumber = $source['ipHostNumber'][0];
        }

        /* Create used ntp server array */
        $this->gotoNtpServer= array();
        if(isset($source['gotoNtpServer'])){
            $this->inheritTimeServer = false;
            unset($source['gotoNtpServer']['count']);
            foreach($source['gotoNtpServer'] as $server){
                $this->gotoNtpServer[$server] = $server;
            }
        }

        /* Set inherit checkbox state */
        if((in_array_strict("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer)==0)){
            $this->inheritTimeServer = true;
            $this->gotoNtpServer=array();
        }
    }


    /* Return plugin informations for acl handling 
#FIXME FAIscript seams to ununsed within this class... */ 
    static function plInfo()
    {
        return (array(  
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Workstation generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoWorkstation' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plCategory"    => array("workstation" => array("description"  => _("Workstation"),
                            "objectClass"  => "gotoWorkstation")),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "workstationRDN",
                            "type"          => "rdn",
                            "default"       => "ou=workstations,ou=systems,",
                            "description"   => _("RDN for workstation storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_workstationRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            ),
                        array(
                            "name"          => "systemIsoHook",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => _("Script to create system ISO images."),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                            ),

                        "plProvidedAcls"=> array(
                                "cn"                  => _("Workstation name"),
                                "description"         => _("Description") ,
                                "l"                   => _("Location") ,
                                "base"                => _("Base") ,
                                "gotoMode"            => _("Goto mode"), 
                                "gotoSyslogServer"    => _("Syslog server"), 
                                "gotoNtpServer"       => _("NTP server"), 
                                "userPassword"      => _("Root password"),
                                "createFAICD"         => _("Create FAI CD"),
                                "FAIstate"            => _("Action flag"))
                            ));
    }

    function set_everything_to_inherited()
    {
        $this->gotoSyslogServer  = "default";
        $this->inheritTimeServer = TRUE;

        /* Set workstation service attributes to inherited */
        if($this->member_of_ogroup && isset($this->parent->by_object['workservice'])){
            foreach(array("gotoXKbLayout","gotoXKbModel","gotoXKbVariant","gotoXDriver",
                        "gotoXResolution","gotoXColordepth","gotoXMouseType","gotoXMouseport") as $name){
                if($this->parent->by_object['workservice']->acl_is_writeable($name)){
                    $this->parent->by_object['workservice']->$name = "default"; 
                }
            }
        }

        /* Set workstation startup attributes to inherited */
        if($this->member_of_ogroup && isset($this->parent->by_object['workstartup'])){
            $obj = $this->parent->by_object['workstartup'];
            if($obj->acl_is_writeable("gotoBootKernel")){
                $this->parent->by_object['workstartup']->gotoBootKernel = "default-inherited";
            }
            if($obj->acl_is_writeable("gotoLdapServer")){
                $this->parent->by_object['workstartup']->gotoLdapServer = "default-inherited";
                $this->parent->by_object['workstartup']->gotoLdap_inherit = TRUE;
                $this->parent->by_object['workstartup']->gotoLdapServers = array();
            }
            if($obj->acl_is_writeable("FAIdebianMirror")){
                $this->parent->by_object['workstartup']->FAIdebianMirror= "inherited";
            }
        }
    }


    function is_modal_dialog()
    {
        return((isset($this->dialog) && $this->dialog) || (isset($this->netConfigDNS->dialog) && $this->netConfigDNS->dialog));
    }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
