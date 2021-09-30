<?php

class termservice extends plugin
{
    /* Generic terminal attributes */
    var $gotoXMonitor= "";
    var $gotoXMethod= "default";
    var $gotoXdmcpServer= "";
    var $gotoXDriver= "";
    var $gotoXResolution= "";
    var $gotoXColordepth= "";
    var $gotoXHsync= "";
    var $gotoXVsync= "";
    var $gotoXKbModel= "";
    var $gotoXKbLayout= "";
    var $gotoXKbVariant= "";
    var $gotoXMouseType= "";
    var $gotoXMouseport= "";
    var $gotoLpdServer= "";
    var $gotoScannerEnable= "";
    var $gotoScannerModel= "";
    var $gotoScannerClients= "";
    var $gotoScannerBackend= "";
    var $goFonHardware= "automatic";

    var $gotoLpdEnable = array();

    var $AutoSync = false;
    var $view_logged = FALSE;

    /* Needed values and lists */
    var $ignore_account= TRUE;
    var $base= "";
    var $cn= "";
    var $orig_dn= "";
    var $XMethods= array();
    var $XDrivers= array();
    var $XResolutions= array();
    var $XColordepths= array();
    var $XKbModels= array ();
    var $XKbLayouts= array ();
    var $XKbVariants= array ();
    var $MouseTypes= array();
    var $MousePorts= array();
    var $hardware_list= array();
    var $used_hardware= array();


    /* attribute list for save action */
    var $attributes= array("gotoLpdEnable", "gotoXMonitor", "gotoXMethod", "gotoXdmcpServer",
            "gotoXDriver", "gotoXResolution", "gotoXColordepth",
            "gotoXHsync", "gotoXVsync", "gotoLpdServer",
            "gotoScannerEnable", "gotoScannerModel", "gotoScannerClients",
            "gotoScannerBackend", "gotoXKbModel", "gotoXKbLayout", "gotoXKbVariant",
            "gotoXMouseType", "gotoXMouseport", "goFonHardware");
    var $objectclasses= array("GOhard");

    var $is_ogroup        = FALSE;
    var $is_ogroup_member = FALSE;

    var $selected_xdmcp_servers = array();
    var $inherited_xdmcp_servers = array();

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config, $dn, $parent);
        $this->orig_dn= $this->dn;

        /* Check if we are a part of an ogroup.
         * In this case, we have to hide all the inherit stuff.
         */ 
        if(isset($parent) && get_class($parent) == "ogroup"){
            $this->is_ogroup = TRUE;
        }

        /* Check if we are member of an object group.
         */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(|(objectClass=gotoTerminalTemplate)(objectClass=gotoWorkstationTemplate))(member=".LDAP::prepare4filter($this->dn)."))");
        if($ldap->count()){
            $this->is_ogroup_member = TRUE;
        }

        /* Load gotoLpdEnable
         */
        $this->gotoLpdEnable = array();
        if(isset($this->attrs['gotoLpdEnable']['count'])){
            for($i = 0 ; $i < $this->attrs['gotoLpdEnable']['count']; $i ++){
                $this->gotoLpdEnable[] = $this->attrs['gotoLpdEnable'][$i];
            }
        }
        $this->gotoLpdEnable_dialog = new gotoLpdEnable($this->config,$this->gotoLpdEnable,$this); 

        /* Get list of available xdrivers */
        $this->XDrivers = $this->getListOfXDrivers();

        /* Create a list of available resolutions.
         */     
        $this->XResolutions= array(
                "640x480"   =>  "640x480",
                "800x600"   =>  "800x600",
                "1024x768"  =>  "1024x768",
                "1152x864"  =>  "1152x864",
                "1280x1024" =>  "1280x1024",
                "1400x1050" =>  "1400x1050",
                "1600x1200" =>  "1600x1200");

        if($this->config->get_cfg_value("environment","resolutions") != ""){
            $file = $this->config->get_cfg_value("environment","resolutions");
            if(is_readable($file)){
                $str = file_get_contents($file);
                $lines = preg_split("/\n/",$str);
                foreach($lines as $line){
                    $line = trim($line);
                    if(!empty($line)){
                        $this->XResolutions[$line]=$line;
                    }
                }
            }else{
                msg_dialog::display(_("Configuration error"), msgPool::cannotReadFile($file), WARNING_DIALOG);
            }
        }

        /* Create a set of selectable color depths
         */
        $this->XColordepths= array(
                "8"        => "8 " ._("bit"),
                "15"       => "15 "._("bit"),
                "16"       => "16 "._("bit"),
                "24"       => "24 "._("bit"));


        /* Create a set of selectable keyboard models
         */
        $this->XKbModels = array();
        foreach(array ("btc9000", "chicony", "compaq", "dell", "dell101", "everex",
                    "flexpro", "geniuscomfy", "hp", "itouch", "jp106", "logicordless",
                    "logiinetnav", "logiinternet", "macintosh", "microsoft",
                    "microsoftpro", "omnikey101", "pc101", "pc102", "pc104",
                    "pc105", "rapidaccess", "rapidaccess2", "winbook") as $type){
            $this->XKbModels[$type] = $type;
        }

        /* Additional values will be extracted from CONFIG_DIR.keyboardLayouts */
        $this->XKbLayouts = array("de"=> "de","en" =>"en", "es" => "es", "us" =>"us", "fr" => "fr");
        $this->XKbVariants= array("nodeadkeys"=>"nodeadkeys", "basic"=>"basic");

        $this->MouseTypes= array("auto" => "auto", 
                "explorerps/2" => "explorerps/2",
                "ImPS/2" => "ImPS/2",
                "PS/2" => "PS/2", 
                "Microsoft" => "Microsoft",
                "Logitech" => "Logitech",);

        $this->MousePorts= array("/dev/input/mice" => "/dev/input/mice",
                "/dev/mouse" => "/dev/mouse",
                "/dev/psaux" => "/dev/psaux",
                "/dev/ttyS0" => "/dev/ttyS0",
                "/dev/ttyS1" => "/dev/ttyS1");

        /* Try to read additional keyboard layouts
         */
        if(file_exists(CONFIG_DIR."/keyboardLayouts")){
            if(is_readable(CONFIG_DIR."/keyboardLayouts")){
                $str = file_get_contents(CONFIG_DIR."/keyboardLayouts");
                $tmp = preg_split("/\n/",$str);
                foreach($tmp as $entry){
                    if((!empty($entry)) && (!preg_match("/^#/",$entry))){
                        $entry = trim($entry);
                        $tmp2 = explode(":",$entry);
                        $la =   trim($tmp2[0]);   // What would be saved to ldap
                        $da =   trim($tmp2[1]);   // This wis displayed in the listbox
                        $this->XKbLayouts [ $la] = $da;
                    }
                }
            }
        }

        /* Terminal server methods supported by LTSP */
        if($this->is_ogroup_member){
            $this->XMethods["default"]= _("inherited");
        }
        $this->XMethods["xdmcp"]   = _("XDMCP");
        $this->XMethods["ldm"]     = _("LDM");
        $this->XMethods["shell"]   = _("Shell");
        $this->XMethods["telnet"]  = _("Telnet");
        $this->XMethods["rdp"]     = _("Windows RDP");
#$this->XMethods["citrix"] = _("ICA client");

        /* Get selected gotoXdmcpServer */
        $this->selected_xdmcp_servers = array();
        if(isset($this->attrs['gotoXdmcpServer'])){
            for($i = 0 ; $i < $this->attrs['gotoXdmcpServer']['count'] ; $i++){
                $this->selected_xdmcp_servers[] = $this->attrs['gotoXdmcpServer'][$i];
            }
        }

        /* Load phone hardware list 
         */
        $tmp = get_sub_list("(objectClass=goFonHardware)","",array(get_ou("phoneGeneric", "phoneRDN")), 
                $this->config->current['BASE'],array("cn","description"), GL_NO_ACL_CHECK);
        foreach($tmp as $attrs){
            $cn= $attrs['cn'][0];
            $description= "";
            if (isset($attrs['description'])){
                $description= " - ".$attrs['description'][0];
            }
            $this->hardware_list[$cn]= "$cn$description";
        }
        $this->hardware_list["automatic"]= _("automatic");
        ksort($this->hardware_list);

        /* These departments may contain objects that have 
           goFonHardware set. 
         */
        $deps_a = array(
                get_people_ou(),
                get_ou("group", "ogroupRDN"),
                get_ou("servgeneric", "serverRDN"),
                get_ou("termgeneric", "terminalRDN"),
                get_ou("workgeneric", "workstationRDN"),
                get_ou("printgeneric", "printerRDN"),
                get_ou("componentGeneric", "componentRDN"),
                get_ou("phoneGeneric", "phoneRDN"));

        $tmp = get_sub_list("(goFonHardware=*)","",$deps_a,$this->config->current['BASE'],
                array('cn','dn','goFonHardware'),GL_NO_ACL_CHECK);
        foreach($tmp as $attrs){
            $cn = $attrs['goFonHardware'][0];
            if(isset($this->hardware_list[$cn])){
                $this->used_hardware[$cn]= $cn;
            }
        }

        /* Load hardware list */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $map= array(
                "gotoXResolution", 
                "gotoXColordepth", 
                "gotoXKbModel", 
                "gotoXKbLayout",
                "gotoXDriver",
                "gotoXdmcpServer",
                "gotoXKbVariant",
                "gotoXMouseType", 
                "gotoXMethod",
                "gotoXMouseport");
        $ldap->search("(&(|(objectClass=gotoTerminalTemplate)(objectClass=gotoWorkstationTemplate))(member=".LDAP::prepare4filter($this->dn)."))",$map);
        if ($ldap->count() == 1){
            $attrs= $ldap->fetch();
            foreach ($map as $name){
                if (!isset($attrs[$name][0])){
                    continue;
                }

                switch ($name){
                    case 'gotoXDriver': 
                        $this->XDrivers = array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XDrivers;
                        break;
                    case 'gotoXMethod': 
                        $this->XMethods = array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XMethods;
                        if(isset($attrs['gotoXdmcpServer'])){
                            for($i = 0 ; $i < $attrs['gotoXdmcpServer']['count'] ; $i++){
                                $this->inherited_xdmcp_servers[] = $attrs['gotoXdmcpServer'][$i];
                            }
                        }
                        break;
                    case 'gotoXResolution':
                        $this->XResolutions= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XResolutions;
                        break;
                    case 'gotoXColordepth':
                        $this->XColordepths= array('default' => _("inherited").' ['.$attrs[$name][0].' '._('Bit').']') + $this->XColordepths;
                        break;
                    case 'gotoXKbModel':
                        $this->XKbModels= array('default' => _("inherited").' ['.$attrs[$name][0].']') + $this->XKbModels;
                        break;
                    case 'gotoXKbLayout':
                        $this->XKbLayouts= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XKbLayouts;
                        break;
                    case 'gotoXKbVariant':
                        $this->XKbVariants= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XKbVariants;
                        break;
                    case 'gotoXMouseType':
                        $this->MouseTypes= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->MouseTypes;
                        break;
                    case 'gotoXMouseport':
                        $this->MousePorts= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->MousePorts;
                        break;
                }
            }
        }

        if(preg_match("/\+/",$this->gotoXHsync)){
            $this->AutoSync = true;
            $this->gotoXHsync = preg_replace("/\+/","-",$this->gotoXHsync);
            $this->gotoXVsync = preg_replace("/\+/","-",$this->gotoXVsync);
        }
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

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            $display= "<img alt=\"\" src=\"images/small-error.png\" align=\"middle\">&nbsp;<b>".
                _("This DN has no terminal features.")."</b>";
            return ($display);
        }

        /* Show main page */
        $smarty= get_smarty();
        $smarty->assign("gotoLpdEnable_dialog", $this->gotoLpdEnable_dialog->execute());

        /* Assign acls */
        $tmp= $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        if(isset($_POST['gotoXdmcpServer_add']) && isset($_POST['XdmcpAddServer'])){
            if(!in_array_strict($_POST['gotoXdmcpServer_add'],$this->selected_xdmcp_servers)){
                $this->selected_xdmcp_servers[] =get_post('gotoXdmcpServer_add');
            }
        }

        if(isset($_POST['selected_xdmcp_servers']) && isset($_POST['XdmcpDelServer'])){
            $tmp = array();
            foreach($this->selected_xdmcp_servers as $name){
                if(!in_array_strict($name,$_POST['selected_xdmcp_servers'])){
                    $tmp[] =  $name;
                }
            }
            $this->selected_xdmcp_servers = $tmp;
        }

        /* Arrays */ 
        foreach(array("XMethods", "XDrivers", "XResolutions", "XColordepths",
                    "XKbModels","XKbVariants","MouseTypes", "MousePorts") as $val){
            $smarty->assign("$val", set_post($this->$val));
        }
        $smarty->assign("XKbLayouts",   set_post($this->XKbLayouts));
        $smarty->assign("XKbLayoutKeys",set_post(array_flip($this->XKbLayouts)));
        $smarty->assign("nfsservers",   set_post($this->config->data['SERVERS']['NFS']));


        /* Create a list of useable servers for the currently selected 
         *  connection type.
         */ 
        $xdmcp_types =  $this->config->data['SERVERS']['TERMINAL_SESSION_TYPES'];
        $available_servers = array();
        foreach($xdmcp_types as $servername =>$supported_types){
            if(in_array_strict(strtoupper($this->gotoXMethod),$supported_types)){
                $available_servers[]  = $servername;
            }
        }

        /* Append additional information to invalid server selections.
         */
        $tmp = array();
        foreach($this->selected_xdmcp_servers as $server){
            if(in_array_strict($server,$available_servers)){
                $tmp[$server] = $server;
            }else{
                $tmp[$server] = $server."&nbsp;-&nbsp;"._("Unsupported");
            }
        }

        /* Remove already selected servers from available list. 
         */
        foreach($available_servers as $key => $server){
            if(isset($tmp[$server])){
                unset($available_servers[$key]);
            }
        }

        $smarty->assign("selected_xdmcp_servers", set_post($tmp));
        $smarty->assign("inherited_xdmcp_servers",set_post($this->inherited_xdmcp_servers));
        $smarty->assign("available_xdmcp_servers",set_post($available_servers));

        /* Variables - select */
        foreach(array("gotoXMethod","gotoXDriver", "gotoXResolution", "gotoXColordepth", 
                    "gotoLpdServer", "gotoXKbModel", "gotoXKbLayout",
                    "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport") as $val){

            $smarty->assign($val."_select", set_post($this->$val));
        }

        /* Variables */
        foreach(array("gotoXHsync", "gotoXVsync") as $val){
            $smarty->assign($val, set_post($this->$val));
        }

        /* Checkboxes */
        foreach(array("gotoScannerEnable") as $val){
            if ($this->$val == TRUE) {
                $smarty->assign("$val", "checked");
            } else {
                $smarty->assign("$val", "");
            }
        }

        /* Phone stuff */
        $smarty->assign ("goFonHardware", set_post($this->goFonHardware));
        $hl= "<select size=\"1\" name=\"goFonHardware\" title=\"".
            _("Choose the phone located at the current terminal")."\" >\n";
        foreach ($this->hardware_list as $cn => $description){
            if ($cn == $this->goFonHardware){
                $selected= "selected";
            } else {
                $selected= "";
            }
            if (isset($this->used_hardware[$cn])){
                $color= "style=\"color:#A0A0A0\"";
            } else {
                $color= "";
            }
            $hl.= "  <option $color label=\"$cn\" value=\"$cn\" $selected>$description</option>\n";
        }
        $hl.= "</select>\n";
        $smarty->assign ("hardware_list", $hl);
        $smarty->assign ("gotoXMonitor", set_post($this->gotoXMonitor));

        $smarty->assign("AutoSyncACL",$this->getacl("AutoSync"));

        $smarty->assign("AutoSyncCHK"," ");
        if($this->AutoSync){
            $smarty->assign("AutoSyncCHK"," checked ");
            $smarty->assign("gotoXVsyncACL", preg_replace("/w/","",$this->getacl("gotoXVsync")));
            $smarty->assign("gotoXHsyncACL", preg_replace("/w/","",$this->getacl("gotoXHsync")));
        }

        /* Show main page */
        return($smarty->fetch (get_template_path('terminalService.tpl', TRUE,dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        new log("remove","terminal/".get_class($this),$this->dn,array_keys($this->attrs));
        $this->handle_post_events("remove");
    }


    /* Save data to object */
    function save_object()
    {
        plugin::save_object();

        $this->gotoLpdEnable_dialog->save_object();
        if($this->gotoLpdEnable_dialog->is_account){
            $this->gotoLpdEnable = $this->gotoLpdEnable_dialog->save();
        }else{
            $this->gotoLpdEnable = array();
        }

        /* Save checkbox state */
        if (isset ($_POST['gotoXMethod'])){
            foreach (array("gotoScannerEnable") as $val){

                if($this->acl_is_writeable($val)){
                    if (!isset ($_POST["$val"])){
                        $this->$val= FALSE;
                    } else {
                        $this->$val= TRUE;
                    }
                }
            }
        } 

        if(isset($_POST['gotoXDriver'])){
            if(isset($_POST['AutoSync'])){
                $this->AutoSync = true;
            }else{
                $this->AutoSync = false;
            }
        }

        /* Default entries can use blank hsync/vsync entries */
        if ($this->dn != "" && $this->cn != "default" && $this->cn != "default"){

            /* But only if no auto sync is enabled... */
            if (!$this->AutoSync){

                /* Check vsync for correct usage */
                $val= preg_replace ("/\s/", "", $this->gotoXVsync);
                if (!preg_match ("/^\d+(\.\d+)?([-]\d+(\.\d+)?)?$/", $val) && $this->acl_is_writeable("gotoXVsync")){
                    $message[]= msgPool::invalid(_("Vertical synchronization range"));
                } elseif ($this->acl_is_writeable("gotoXVsync")){
                    list($v1,$v2)= preg_split ("/[-+]/", $val);
                    if ($v2 != ""){
                        if ($v1 > $v2){
                            $message[]= msgPool::invalid(_("Vertical synchronization range"));
                        }
                    }
                }

                /* Check hsync for correct usage */
                $val= preg_replace ("/\s/", "", $this->gotoXHsync);
                if (!preg_match ("/^\d+(\.\d+)?([-]\d+(\.\d+)?)?$/", $val) && $this->acl_is_writeable("gotoXHsync")){
                    $message[]= msgPool::invalid(_("Horizontal synchronization range"));
                } elseif ($this->acl_is_writeable("gotoXHsync")){
                    list($v1,$v2)= preg_split ("/[-+]/", $val);
                    if ($v2 != ""){
                        if ($v1 > $v2){
                            $message[]= msgPool::invalid(_("Horizontal synchronization range"));
                        }
                    }
                }
            }
        }
    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();
        $message = array_merge($message,$this->gotoLpdEnable_dialog->check());

        if($this->gotoXMethod != "default"){
            $xdmcp_types =  $this->config->data['SERVERS']['TERMINAL_SESSION_TYPES'];
            $available_servers = array();
            foreach($xdmcp_types as $servername =>$supported_types){
                if(in_array_strict(strtoupper($this->gotoXMethod),$supported_types)){
                    $available_servers[]  = $servername;
                }
            }
            foreach($this->selected_xdmcp_servers as $server){
                if(!in_array_strict($server,$available_servers)){
                    $message[] = _("Remote desktop settings contains servers that do not support the selected connection method.");
                    break;
                }
            }
        }

        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        /* remove objectclass GOhard if this is an ogroup tab */
        if(isset($this->parent->by_object['ogroup'])){
            $this->objectclasses = array();
        }

        plugin::save();

        /* Strip out 'default' values */
        foreach(array("gotoXMethod","gotoXDriver", "gotoXResolution", "gotoXColordepth",
                    "gotoLpdServer", "gotoXKbModel", "gotoXKbLayout",
                    "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport") as $val){

            if ($this->attrs[$val] == "default"){
                $this->attrs[$val]= array();
            }
        }

        if($this->gotoXMethod == "default"){
            $this->attrs['gotoXdmcpServer'] = array();
            $this->attrs['gotoXMethod'] = array();
        }else{
            $this->attrs['gotoXdmcpServer'] = array_values($this->selected_xdmcp_servers);
        }


        if($this->AutoSync){
            $this->attrs['gotoXHsync'] = "30+55";
            $this->attrs['gotoXVsync'] = "50+70";
        }

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify ($this->attrs); 
        new log("modify","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }
        $this->handle_post_events("modify");

        /* Send goto reload event */
        if(count($this->attrs)){
            $this->send_goto_reload();
        }
    }


    function getListOfXDrivers()
    {
        $drivers = array("default" => "["._("unknown")."]");

        /* Generate a list of xdrivers from CONFIG_DIR/xdrivers */
        if (file_exists(CONFIG_DIR.'/xdrivers')){
            $xdrivers = file (CONFIG_DIR.'/xdrivers');
            foreach ($xdrivers as $line){
                if (!preg_match ("/^#/", $line)){
                    $drivers[]= trim($line);
                }
            }
        } else {
            foreach( array(
                        "ati", "atimisc", "chips", "cirrus", "cyrix", "fbdev", "fglrx",
                        "i128", "i740", "i810", "intel", "imstt", "mga", "neomagic", "newport", "nsc",  "nv", "nvidia",
                        "r128", "radeonhd", "radeon", "rendition", "s3", "s3virge", "savage", "siliconmotion",
                        "sis", "tdfx", "tga", "trident", "tseng", "vesa", "vga", "vmware") as $driver){
                $drivers[] = $driver;
            }
        }

        $tmp = array();
        foreach($drivers as $name){
            $tmp[$name] = $name;
        }
        return($tmp);
    }

    function send_goto_reload()
    {
        if(count($this->attrs) && class_available("DaemonEvent")){
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
            if(isset($events['TRIGGERED']['DaemonEvent_goto_reload'])){
                $evt = $events['TRIGGERED']['DaemonEvent_goto_reload'];
                $macs = array();

                /* Get list of macAddresses
                 */
                if(isset($this->parent->by_object['ogroup'])){

                    /* If we are an object group, add all member macs
                     */
                    $p = $this->parent->by_object['ogroup'];
                    foreach($p->memberList as $dn => $obj){
                        if(preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/",$dn)) continue;
                        if(isset($p->objcache[$dn]['macAddress']) && !empty($p->objcache[$dn]['macAddress'])){
                            $macs[] = $p->objcache[$dn]['macAddress'];
                        }
                    }
                }elseif(isset($this->parent->by_object['workgeneric']->netConfigDNS->macAddress)){

                    /* We are a workstation. Add current mac.
                     */
                    $mac = $this->parent->by_object['workgeneric']->netConfigDNS->macAddress;
                    if(!empty($mac) && !preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/",$this->orig_dn)){
                        $macs[] = $mac;
                    }
                }

                /* Trigger event for all member objects
                 */
                if(count($macs)){
                    $tmp = new $evt['CLASS_NAME']($this->config);
                    $tmp->set_type(TRIGGERED_EVENT);
                    $target = ":"."0";
                    $tmp->add_targets(array($target));
                    $tmp->set_macs($macs);
                }
            }
        }
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $source_o = new termservice ($this->config, $source['dn']);

        foreach(array("gotoXdmcpServer","selected_xdmcp_servers") as $attr){
            $this->$attr = $source_o->$attr;
        }
    }



    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Service"),
                    "plDescription" => _("Terminal service"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 3,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("terminal"),

                    "plProvidedAcls"=> array(

                        "gotoXMonitor"            => _("Monitor"),
                        "gotoXMethod"             => _("Method"),
                        "gotoXdmcpServer"         => _("Remote desktop"),
                        "gotoXDriver"             => _("Graphic driver"),
                        "gotoXResolution"         => _("Graphic resolution"),
                        "gotoXColordepth"         => _("Graphic color depth"),
                        "gotoXHsync"              => _("Horizontal synchronization"),
                        "gotoXVsync"              => _("Vertical synchronization"),
                        "AutoSync"                => _("Auto-Sync"),
                        "gotoScannerEnable"       => _("Scanner enabled"),
                        "gotoLpdEnable"           => _("Printer enabled"),
                        "gotoXKbModel"            => _("Keyboard model"),
                        "gotoXKbLayout"           => _("Keyboard layout"),
                        "gotoXKbVariant"          => _("Keyboard variant"),
                        "gotoXMouseType"          => _("Mouse type"),
                        "gotoXMouseport"          => _("Mouse port"),
                        "gotoLpdEnable"         => _("Printer enabled"),
                        "goFonHardware"           => _("Telephone hardware"))
                        ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
