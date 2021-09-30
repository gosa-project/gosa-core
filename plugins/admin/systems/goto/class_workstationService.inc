<?php

class workservice extends plugin
{
    var $gotoScannerEnable;

    /* Generic terminal attributes */
    var $gotoXMonitor= "";
    var $gotoXDriver= "";
    var $gotoXResolution= "";
    var $gotoXColordepth= "";
    var $gotoXHsync= "";
    var $gotoXVsync= "";
    var $AutoSync = false;
    var $gotoXKbModel= "";
    var $gotoXKbLayout= "";
    var $gotoXKbVariant= "";
    var $gotoXMouseType= "";
    var $gotoXMouseport= "";
    var $gotoScannerClients= "";
    var $gotoScannerBackend= "";
    var $goFonHardware= "automatic";
    var $view_logged = FALSE;

    /* Needed values and lists */
    var $ignore_account= TRUE;
    var $base= "";
    var $cn= "";
    var $orig_dn= "";
    var $XDrivers= array();
    var $XResolutions = array();
    var $MouseTypes= array();
    var $MousePorts= array();
    var $hardware_list= array();
    var $used_hardware= array();


    /* attribute list for save action */
    var $attributes= array("gotoXMonitor", "gotoXDriver", "gotoXResolution", "gotoXColordepth",
            "gotoXHsync", "gotoXVsync",
            "gotoScannerEnable", "gotoScannerClients",
            "gotoScannerBackend", "gotoXKbModel", "gotoXKbLayout", "gotoXKbVariant",
            "gotoXMouseType", "gotoXMouseport", "goFonHardware");
    var $objectclasses= array("GOhard");

    var $XColordepths     =array();
    var $XKbModels        =array();
    var $XKbLayouts       =array();
    var $XKbVariants      =array();

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config, $dn, $parent);

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
                //natcasesort($this->gotoXResolutions);
            }else{
                msg_dialog::display(_("Configuration error"), msgPool::cannotReadFile($file), WARNING_DIALOG);
            }
        }

        /* Get list of available xdrivers */
        $this->XDrivers = $this->getListOfXDrivers();

        array_unshift($this->XDrivers, "["._("unknown")."]");

        $this->XColordepths= array( 
                "8"        => "8 " ._("bit"), 
                "15"       => "15 "._("bit"),      
                "16"       => "16 "._("bit"),   
                "24"       => "24 "._("bit"));

        foreach(array ("btc9000", "chicony", "compaq", "dell", "dell101", "everex",
                    "flexpro", "geniuscomfy", "hp", "itouch", "jp106", "logicordless",
                    "logiinetnav", "logiinternet", "macintosh", "microsoft",
                    "microsoftpro", "omnikey101", "pc101", "pc102", "pc104",
                    "pc105", "rapidaccess", "rapidaccess2", "winbook") as $type){
            $this->XKbModels[$type] = $type;
        }

        $this->MouseTypes= array("auto" => "auto",
                "explorerps/2" => "explorerps/2",
                "ImPS/2" => "ImPS/2",
                "PS/2" => "PS/2",
                "Microsoft" => "Microsoft",
                "Logitech" => "Logitech");

        $this->MousePorts= array("/dev/input/mice" => "/dev/input/mice",
                "/dev/mouse" => "/dev/mouse",
                "/dev/psaux" => "/dev/psaux",
                "/dev/ttyS0" => "/dev/ttyS0",
                "/dev/ttyS1" => "/dev/ttyS1");

        /* Additional values will be extracted from CONFIG_DIR./keyboardLayouts */
        $this->XKbLayouts = array("de"=> "de","en" =>"en", "es" => "es", "us" =>"us", "fr" => "fr");
        $this->XKbVariants= array ("nodeadkeys"=>"nodeadkeys", "basic"=>"basic");

        /* try to read additional keyboard layouts 
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

        $this->orig_dn= $this->dn;

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

        if(preg_match("/\+/",$this->gotoXHsync)){
            $this->AutoSync = true;
            $this->gotoXHsync = preg_replace("/\+/","-",$this->gotoXHsync);
            $this->gotoXVsync = preg_replace("/\+/","-",$this->gotoXVsync);
        }

        /* Load hardware list */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".LDAP::prepare4filter($this->dn)."))");
        if ($ldap->count() == 1){
            $map= array("gotoXResolution", "gotoXColordepth", "gotoXKbModel", "gotoXKbLayout",
                    "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport", "gotoXDriver");
            $attrs= $ldap->fetch();

            foreach ($map as $name){
                if (!isset($attrs[$name][0])){
                    continue;
                }

                switch ($name){
                    case 'gotoXDriver':
                        $this->XDrivers= array('default' => _("inherited").' ['.$attrs[$name][0].']' ) + $this->XDrivers;
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

        /* Workaround to fill in inherited values if we've specified an objectclass */
        $SelectedSystemType = session::get("SelectedSystemType");
        if (isset($SelectedSystemType['ogroup']) && $SelectedSystemType['ogroup'] != 'none'){
            $this->XResolutions= array('default' => _("inherited"));
            $this->XColordepths= array('default' => _("inherited"));
            $this->XKbModels= array('default' => _("inherited"));
            $this->XKbLayouts= array('default' => _("inherited"));
            $this->XKbVariants= array('default' => _("inherited"));
            $this->MouseTypes= array('default' => _("inherited"));
            $this->MousePorts= array('default' => _("inherited"));
        }
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","workstation/".get_class($this),$this->dn);
        }

        /* Do we need to flip is_account state? */
        if (isset($_POST['modify_state'])){
            $this->is_account= !$this->is_account;
        }

        /* Do we need to flip is_account state? */
        if(isset($_POST['modify_state'])){
            if($this->is_account && $this->acl_is_removeable()){
                $this->is_account= FALSE;
            }elseif(!$this->is_account && $this->acl_is_createable()){
                $this->is_account= TRUE;
            }
        }

        /* Show main page */
        $smarty= get_smarty();

        /* Assign ACLs */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translated){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        /* Arrays */ 
        foreach(array("XDrivers", "XResolutions", "XColordepths",
                    "XKbModels", "XKbVariants",
                    "MouseTypes", "MousePorts") as $val){
            $smarty->assign("$val", set_post($this->$val));
        }
        $smarty->assign("XKbLayouts"    ,set_post($this->XKbLayouts));
        $smarty->assign("XKbLayoutKeys" ,set_post(array_flip($this->XKbLayouts)));

        $smarty->assign("xdmcpservers",set_post($this->config->data['SERVERS']['TERMINAL']));
        $smarty->assign("nfsservers",  set_post($this->config->data['SERVERS']['NFS']));

        /* Variables - select */
        foreach(array(
                    "gotoXDriver", "gotoXResolution", "gotoXColordepth", 
                    "gotoXKbModel", "gotoXKbLayout","gotoScannerEnable",
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
        $smarty->assign ("goFonHardware", $this->goFonHardware);

        $perms = "";
        if(!$this->acl_is_writeable("goFonHardware")){
            $perms = " disabled ";
        }

        $hl= "<select size=\"1\" name=\"goFonHardware\" ".$perms." title=\"".
            _("Choose the phone located at the current terminal")."\" >";
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

        if($this->AutoSync){
            $smarty->assign("AutoSyncCHK"," checked ");
            $smarty->assign("hiddenState"," disabled ");
        }else{
            $smarty->assign("AutoSyncCHK"," ");
            $smarty->assign("hiddenState","");
        }

        /* Show main page */
        return($smarty->fetch (get_template_path('workstationService.tpl',TRUE,dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        $this->handle_post_events("remove");
        new log("remove","workstation/".get_class($this),$this->dn);
    }

    /* Save data to object */
    function save_object()
    {
        plugin::save_object();

        if((isset($_POST['workservicePosted'])) && $this->acl_is_writeable("AutoSync")) {
            if(isset($_POST['AutoSync'])){
                $this->AutoSync = true;
            }else{
                $this->AutoSync = false;
            }
        }
    }

    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        /* Default entries can use blank hsync/vsync entries */
        if ($this->dn != "" && $this->cn != "default" && $this->cn != "wdefault"){

            /* But only if no auto sync is enabled... */
            if (!$this->AutoSync){

                /* Check vsync for correct usage */
                $val= preg_replace ("/\s/", "", $this->gotoXVsync);

                if($this->acl_is_writeable("gotoXVsync")){
                    if(empty($val)){
                        $message[]= msgPool::required(_("Vertical synchronization"));
                    }elseif (!preg_match ("/^\d+(\.\d+)?([+-]\d+(\.\d+)?)?$/", $val)){
                        $message[]= msgPool::invalid(_("Vertical synchronization"));
                    } else{
                        if (!preg_match('/^\d+$/', $val)){
                            list($v1,$v2)= preg_split ("/[-+]/", $val);
                            if ($v2 != ""){
                                if ($v1 > $v2){
                                    $message[]= msgPool::invalid(_("Vertical synchronization"));
                                }
                            }
                        }
                    }
                }

                /* Check hsync for correct usage */
                $val= preg_replace ("/\s/", "", $this->gotoXHsync);
                if($this->acl_is_writeable("gotoXHsync")){
                    if(empty($val)){
                        $message[]= msgPool::required(_("Horizontal synchronization"));
                    }elseif (!preg_match ("/^\d+(\.\d+)?([-]\d+(\.\d+)?)?$/", $val)){
                        $message[]= msgPool::invalid(_("Horizontal synchronization"));
                    } else{
                        if (!preg_match('/^\d+$/', $val)){
                            list($v1,$v2)= preg_split ("/[-+]/", $val);
                            if ($v2 != ""){
                                if ($v1 > $v2){
                                    $message[]= msgPool::invalid(_("Horizontal synchronization"));
                                }
                            }
                        }
                    }
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
        foreach(array(
                    "gotoXDriver", "gotoXResolution", "gotoXColordepth",
                    "gotoXKbModel", "gotoXKbLayout",
                    "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport") as $val){

            if ($this->attrs[$val] == "default"){
                $this->attrs[$val]= array();
            }
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
        new log("modify","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }
        $this->handle_post_events("modify");

        /* Daemon isn't available anymore */
        if(count($this->attrs)){
            $this->send_goto_reload(); 
        }

    }


    function getListOfXDrivers()
    {
        /* Generate a list of xdrivers from CONFIG_DIR./xdrivers */
        $drivers = array();
        if (file_exists(CONFIG_DIR.'/xdrivers')){
            $xdrivers = file (CONFIG_DIR.'/xdrivers');
            foreach ($xdrivers as $line){
                if (!preg_match ("/^#/", $line)){
                    $drivers[]= trim($line);
                }
            }
        } else {
            $drivers = array("ati", "atimisc", "chips", "cirrus", "cyrix", "fbdev", "fglrx",
                    "i128", "i740", "i810", "intel", "imstt", "mga", "neomagic", "newport", "nsc",  "nv", "nvidia",
                    "r128", "radeonhd", "radeon", "rendition", "s3", "s3virge", "savage", "siliconmotion",
                    "sis", "tdfx", "tga", "trident", "tseng", "vesa", "vga", "vmware");
        } 
        $tmp = array();
        foreach($drivers as $name){
            $tmp[$name] = $name;
        }

        return($tmp);
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Service"),
                    "plDescription" => _("Workstation service"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 10,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("workstation","ogroups"), 

                    "plProvidedAcls"=> array(
                        "gotoXMonitor"          => _("Monitor"),
                        "gotoXDriver"           => _("Graphic driver"),
                        "gotoXResolution"       => _("Graphic resolution"),
                        "gotoXColordepth"       => _("Graphic color depth"),
                        "gotoXHsync"            => _("Horizontal synchronization"),
                        "gotoXVsync"            => _("Vertical synchronization"),
                        "AutoSync"              => _("Use DDC"),
                        "gotoScannerEnable"     => _("Scanner enabled"),
                        "gotoXKbModel"          => _("Keyboard model"),
                        "gotoXKbLayout"         => _("Keyboard layout"),
                        "gotoXKbVariant"        => _("Keyboard variant"),
                        "gotoXMouseType"        => _("Mouse type"),
                        "gotoXMouseport"        => _("Mouse port"),
                        "goFonHardware"         => _("Telephone hardware")) 
                        ));
    }


    /*! \brief  Send goto_reload event to support daemon
     */
    function send_goto_reload()
    {
        if(count($this->attrs) && class_available("DaemonEvent")){
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
        }
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        if(preg_match("/\+/",$this->gotoXHsync)){
            $this->AutoSync = true;
            $this->gotoXHsync = preg_replace("/\+/","-",$this->gotoXHsync);
            $this->gotoXVsync = preg_replace("/\+/","-",$this->gotoXVsync);
        }
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
