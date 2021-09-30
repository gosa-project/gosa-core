<?php

class termgeneric extends plugin
{
    /* Generic terminal attributes */
    var $gotoMode= "locked";
    var $gotoTerminalPath= "";
    var $gotoSwapServer= "";
    var $gotoSyslogServer= "";
    var $gotoSyslogServers = array();
    var $gotoNtpServer= array();
    var $gotoNtpServers= array();
    var $gotoSndModule= "";
    var $gotoFloppyEnable= "";
    var $gotoCdromEnable= "";
    var $ghCpuType= "-";
    var $ghMemSize= "-";
    var $ghUsbSupport= "-";
    var $ghNetNic= array();
    var $ghIdeDev= array();
    var $ghScsiDev= array();
    var $ghGfxAdapter= "-";
    var $ghSoundAdapter= "-";
    var $gotoLastUser= "-";
    var $netConfigDNS;
    var $auto_activate= FALSE;

    /* Needed values and lists */
    var $base= "";
    var $cn= "";
    var $description= "";
    var $orig_dn= "";
    var $orig_cn= "";
    var $orig_base= "";

    var $inheritTimeServer = true;

    /* Plugin side filled */
    var $modes= array();
    var $baseSelector;

    /* attribute list for save action */
    var $ignore_account= TRUE;
    var $attributes= array("gotoMode", "gotoTerminalPath", 
            "gotoSwapServer", "gotoSyslogServer", "gotoNtpServer",
            "gotoFloppyEnable", "gotoCdromEnable", "cn", "gotoSndModule",
            "ghCpuType", "ghMemSize","ghUsbSupport", "description",
            "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser");
    var $objectclasses= array("top", "gotoTerminal", "GOhard");

    var $validActions   = array("reboot" => "", "rescan" => "", "wake" => "", "memcheck" => "", "sysinfo" => "");

    var $fai_activated = FALSE;
    var $view_logged = FALSE;

    var $member_of_ogroup = FALSE;

    var $kerberos_key_service = NULL;


    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        /* Check if FAI is activated */
        $this->fai_activated = $config->pluginEnabled("faiManagement");

        plugin::__construct ($config, $dn, $parent);

        if(class_available("krbHostKeys")){
            $this->kerberos_key_service = new krbHostKeys($this->config,$this);
        }

        if(!isset($this->parent->by_object['ogroup'])){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd ($this->config->current['BASE']);
            $ldap->search("(&(|(objectClass=gotoTerminalTemplate)(objectClass=gotoWorkstationTemplate))(member=".LDAP::prepare4filter($this->dn)."))",array("cn"));
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
            for($i = 0 ; $i < $this->attrs['gotoNtpServer']['count']; $i++ ){
                $server = $this->attrs['gotoNtpServer'][$i];
                $this->gotoNtpServer[$server] = $server;
            }
        }

        /* Set inherit checkbox state */
        if((in_array_strict("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer)==0)){
            $this->inheritTimeServer = true;
            $this->gotoNtpServer=array();
        }

        /* You can't inherit the NTP service, if we are not member in an object group */
        if(!$this->member_of_ogroup){
            $this->inheritTimeServer = FALSE;
        }

        /* Create available ntp options */
        $this->gotoNtpServers = $this->config->data['SERVERS']['NTP'];
        foreach($this->gotoNtpServers as $key => $server){
            if($server == "default"){
                unset($this->gotoNtpServers[$key]);
            }
        }

        $this->modes["locked"]= _("Locked");
        $this->modes["active"]= _("Activated");

        /* Set base */
        if ($this->dn == "new"){
            $ui= get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } elseif(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $this->dn)){
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", "", $this->dn);
        } else {
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i", "", $this->dn);
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

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","terminal/".get_class($this),$this->dn);
        }

        /* Do we need to flip is_account state? */
        if (isset($_POST['modify_state'])){
            $this->is_account= !$this->is_account;
        }

        if (isset($_POST['action']) && $this->acl_is_writeable("FAIstate") && isset($this->validActions[$_POST['saction']])){
            $action = get_post('saction');

            /* Check if we have an DaemonEvent for this action */
            if(class_available("DaemonEvent_".$action)){
                $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
                if(isset($events['TRIGGERED']["DaemonEvent_".$action])){
                    $evt = $events['TRIGGERED']["DaemonEvent_".$action];
                    $tmp = new $evt['CLASS_NAME']($this->config);
                    $tmp->add_targets(array($this->netConfigDNS->macAddress));
                    $tmp->set_type(TRIGGERED_EVENT);
                }
            }else{
                msg_dialog::display(_("Event error"),
                        sprintf(_("Event '%s' is not available!"),$action),ERROR_DIALOG);
            }

        }

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            $display= "<img alt=\"\" src=\"images/small-error.png\" align=middle>&nbsp;<b>".
                msgPool::noValidExtension(_("terminal"))."</b>";
            return($display);
        }

        /* Add new ntp Server to our list */
        if((isset($_POST['addNtpServer'])) && (isset($_POST['gotoNtpServers'])) && $this->acl_is_writeable("gotoNtpServer")){
            $this->gotoNtpServer[get_post('gotoNtpServers')] = get_post('gotoNtpServers');
        }

        /* Delete selected NtpServer for list of used servers  */
        if((isset($_POST['delNtpServer'])) && (isset($_POST['gotoNtpServerSelected'])) && $this->acl_is_writeable("gotoNtpServer")){
            foreach(get_post('gotoNtpServerSelected') as $name){
                unset($this->gotoNtpServer[$name]);
            } 
        }

        /* Fill templating stuff */
        $smarty= get_smarty();


        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        $smarty->assign("cn",set_post($this->cn));
        $smarty->assign("description", set_post($this->description));

        /* tell smarty the inherit checkbox state */
        $smarty->assign("inheritTimeServer",$this->inheritTimeServer);

        /* Check if terminal is online */
        $smarty->assign("actions", set_post(array("wake" => _("Wake up"))));

        /* Arrays */
        $smarty->assign("modes", set_post($this->modes));

        $tmp2 = array(); 
        $tmp2['!']= _("Local swap");
        foreach($this->config->data['SERVERS']['NBD'] as $server){
            if($server != "default"){
                $tmp2[$server]= $server;
            }else{
                if($this->member_of_ogroup){
                    $tmp2[$server]="["._("inherited")."]";
                }
            }
        }

        $smarty->assign("swapservers", set_post($tmp2));
        $tmp2 = array(); 
        foreach($this->config->data['SERVERS']['NFS'] as $server){
            if($server != "default"){
                $tmp2[$server]= $server;
            }else{
                if($this->member_of_ogroup){
                    $tmp2[$server]="["._("inherited")."]";
                }
            }
        }

        $smarty->assign("nfsservers",    set_post($tmp2));
        $smarty->assign("syslogservers", set_post($this->gotoSyslogServers));

        $tmp = array();
        foreach($this->gotoNtpServers as $server){
            if(!in_array_strict($server,$this->gotoNtpServer)){
                $tmp[$server] = $server;
            }
        }

        $smarty->assign("ntpservers",     set_post($tmp));
        $smarty->assign("fai_activated",$this->fai_activated);

        $smarty->assign("si_activated",FALSE);

        /* Variables */
        foreach(array("gotoMode", "gotoTerminalPath", "gotoSwapServer","gotoSyslogServer", "gotoNtpServer") as $val){
            $smarty->assign($val."_select", set_post($this->$val));
        }
        $smarty->assign("base", $this->baseSelector->render());

        $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

        /* Show main page */
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

        return($smarty->fetch (get_template_path('terminal.tpl', TRUE, dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        if($this->acl_is_removeable()){   
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->dn);
            $ldap->cat($this->dn, array('dn'));
            if($ldap->count()){
                $this->netConfigDNS->remove_from_parent();
                $ldap->rmDir($this->dn);

                new log("remove","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
                }

                /* Remove kerberos key dependencies too */
                if(is_object($this->kerberos_key_service)){
                    $this->kerberos_key_service->remove_from_parent_by_prefix("host/");
                }

                /* Optionally execute a command after we're done */
                $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

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

        /* Save terminal path to parent since it is used by termstartup, too */
        if(isset($this->parent->by_object['termstartup'])){
            $this->parent->by_object['termstartup']->gotoTerminalPath= $this->gotoTerminalPath;
        }

        if(isset($_POST['termgeneric_posted'])){
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
        if($this->cn != "default"){
            $message= array_merge($message, $this->netConfigDNS->check());
        }

        /* Permissions for that base? */
        $this->dn= "cn=".$this->cn.",".get_ou("termgeneric", "terminalRDN").$this->base;

        if ($this->cn == ""){
            $message[]= msgPool::required(_("Name"));
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();
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

        if ($this->orig_dn == 'new'){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($this->base);

            /* It is possible to have a 'default' terminal on every base */
            if($this->cn == "default"){
                $ldap->cat($this->dn);
            }else{
                $ldap->search ("(&(objectClass=gotoTerminal)(cn=".$this->cn."))", array("cn"));
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

        /* Find proper terminal path for tftp configuration
FIXME: This is suboptimal when the default has changed to
another location! */
        if ($this->gotoTerminalPath == "default"){
            $ldap= $this->config->get_ldap_link();

            /* Strip relevant part from dn, keep trailing ',' */
            $tmp= preg_replace("/^cn=[^,]+,".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i", "", $this->dn);
            $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

            /* Walk from top to base and try to load default values for
               'gotoTerminalPath'. Abort when an entry is found. */
            while (TRUE){
                $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

                $ldap->cat("cn=default,".get_ou("termgeneric", "terminalRDN").$tmp.
                        $this->config->current['BASE'], array('gotoTerminalPath'));
                $attrs= $ldap->fetch();
                if (isset($attrs['gotoTerminalPath'])){
                    $this->gotoTerminalPath= $attrs['gotoTerminalPath'][0];
                    break;
                }

                /* Nothing left? */
                if ($tmp == ""){
                    break;
                }
            }
        }

        plugin::save();

        /* Strip out 'default' values */
        foreach (array("gotoTerminalPath", "gotoSwapServer", "gotoSyslogServer") as $val){
            if(isset($this->attrs[$val])){
                if ($this->attrs[$val] == "default"){
                    $this->attrs[$val]= array();
                }
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

        /* Set ntpServers */
        $this->attrs['gotoNtpServer'] = array();
        if(!$this->inheritTimeServer){
            foreach($this->gotoNtpServer as $server){
                $this->attrs['gotoNtpServer'][] = $server;
            }
        }

        /* cn=default and macAddress=- indicates that this is a template */
        if($this->cn == "default"){
            $this->netConfigDNS->macAddress = "-";
        }

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        if ($this->orig_dn == 'new'){
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            if (!count($this->attrs['gotoNtpServer'])){
                unset($this->attrs['gotoNtpServer']);
            }
            $ldap->add($this->attrs);
            new log("create","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

            $this->netConfigDNS->cn = $this->cn;
            $this->netConfigDNS->save();

            $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
        } else {
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 
            new log("modify","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

            $this->netConfigDNS->cn = $this->cn;
            $this->netConfigDNS->save();

            $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

            /* Update all accessTo/trust dependencies */
            if($this->orig_cn != $this->cn){
                update_accessTo($this->orig_cn,$this->cn);
            }
        }

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

        /* Send installation activation
         */
        if ($activate && class_available("DaemonEvent")){
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
        $smarty->assign("cn" , set_post($this->cn));
        $smarty->assign("object","terminal");
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


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Terminal"),
                    "plDescription" => _("Terminal generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoTerminal' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),
                    "plCategory"    => array("terminal" => array( "description"  => _("Terminal"),
                            "objectClass"  => "gotoTerminal")),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "terminalRDN",
                            "type"          => "rdn",
                            "default"       => "ou=terminals,ou=systems,",
                            "description"   => _("RDN for terminal storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_terminalRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),

            "plProvidedAcls"=> array(
                    "cn"                  => _("Name"),
                    "description"         => _("Description"),
                    "base"                => _("Base"),

                    "gotoMode"            => _("Mode"),
                    "gotoSyslogServer"    => _("Syslog server enabled"),

                    "gotoTerminalPath"    => _("Root server"),
                    "gotoSwapServer"      => _("Swap server"),

                    "gotoNtpServer"       => _("NTP server settings"),
                    "userPassword"      => _("Root password"),

                    "FAIstate"            => _("Action flag"))
                ));
    }


    function set_everything_to_inherited()
    {
        $this->gotoTerminalPath  = "default";
        $this->gotoSwapServer    = "default" ;
        $this->gotoSyslogServer  = "default";
        $this->inheritTimeServer = TRUE;

        /* Set workstation service attributes to inherited */
        if($this->member_of_ogroup && isset($this->parent->by_object['termservice'])){
            foreach(array("gotoXKbLayout","gotoXKbModel","gotoXKbVariant","gotoXDriver",
                        "gotoXResolution","gotoXColordepth","gotoXMouseType","gotoXMouseport") as $name){
                $this->parent->by_object['termservice']->$name = "default";
            }
        }

        /* Set workstation startup attributes to inherited */
        if($this->member_of_ogroup && isset($this->parent->by_object['termstartup'])){
            $this->parent->by_object['termstartup']->gotoBootKernel = "default-inherited";
            $this->parent->by_object['termstartup']->gotoLdapServer = "default-inherited";
            $this->parent->by_object['termstartup']->gotoLdap_inherit = TRUE;
            $this->parent->by_object['termstartup']->gotoLdapServers = array();
        }
    }


    function is_modal_dialog()
    {
        return((isset($this->dialog) && $this->dialog) || (isset($this->netConfigDNS->dialog) && $this->netConfigDNS->dialog));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
