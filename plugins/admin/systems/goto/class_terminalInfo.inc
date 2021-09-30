<?php

class terminfo extends plugin
{
    /* Generic terminal attributes */
    var $ghCpuType= "-";
    var $ghMemSize= "-";
    var $macAddress= "-";
    var $ghUsbSupport= "-";
    var $ghNetNic= array();
    var $ghIdeDev= array();
    var $ghScsiDev= array();
    var $ghGfxAdapter= "-";
    var $ghSoundAdapter= "-";
    var $ghInventoryNumber= "-";
    var $gotoLastUser= "-";
    var $gotoFloppyEnable= "";
    var $gotoCdromEnable= "";

    /* Needed values and lists */
    var $base= "";
    var $cn= "";
    var $view_logged = FALSE;

    /* attribute list for save action */
    var $ignore_account= TRUE;
    var $attributes= array("cn", "gotoMode", "gotoTerminalPath", "gotoFloppyEnable",
            "gotoCdromEnable", "ghInventoryNumber",
            "gotoSwapServer", "gotoSyslogServer", "gotoNtpServer",
            "ghCpuType", "ghMemSize", "macAddress", "ghUsbSupport",
            "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser");
    var $objectclasses= array("GOhard");

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config,$dn);

        /* Read arrays */
        foreach (array("ghNetNic", "ghIdeDev", "ghScsiDev") as $val){
            if (!isset($this->attrs[$val])){
                continue;
            }
            for ($i= 0; $i<$this->attrs[$val]['count']; $i++){
                array_push($this->$val, $this->attrs[$val][$i]);
            }
        }

        /* Fix USB entry */
        if ($this->ghUsbSupport == "true"){
            $this->ghUsbSupport= _("present");
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

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            return("<img alt=\"\" src=\"images/small-error.png\" align=middle>&nbsp;<b>".
                    msgPool::noValidExtension(_("terminal"))."</b>");
        }

        $smarty= get_smarty();
        $display= "";
        $smarty->assign("ACL",TRUE);
        if(!preg_match("/r/",$this->getacl(""))){
            $smarty->assign("ACL",FALSE);
        }elseif(!is_callable("snmpget")){

            $smarty->assign("load", progressbar(0,100,15,true));
            $smarty->assign("mem", progressbar(0,100,15,true));
            $smarty->assign("swap", progressbar(0,100,15,true));
            foreach(array("uptime", "sshd", "X", "saned", "artsd", "cupsd","status","ghNetNic", "ghIdeDev", "ghScsiDev","FloppyDevice", "CdromDevice","active") as $val){
                $smarty->assign("$val", "<i>"._("unknown status, SNMP support missing")."</i>");
            }


            $display ="";
        }else
            /* Default entry? */
            if ($this->cn == "default"){
                $display= "<div style='height:150px;'><br><b>";
                $display.= _("This is a virtual terminal which has no properties to show here.");
                $display.= "</b></div>";
            } else {

                /* Get template object */

                /* Prevent undefined variable .... */ 
                $smarty->assign("load", progressbar(0,100,15,true));
                $smarty->assign("mem", progressbar(0,100,15,true));
                $smarty->assign("swap", progressbar(0,100,15,true));

                /* Check if terminal is online - due to lack of the daemon consider offline */
                $smarty->assign("status", _("off-line"));
                $smarty->assign("active", "false");

                /* Set floppy and cdrom status */
                foreach(array("Floppy", "Cdrom") as $val){
                    $name= "goto".$val."Enable";
                    if ($this->$name == "YES"){
                        $status= _("present");
                    } else {
                        $status= "-";
                    }
                    $smarty->assign($val."Device", $status);
                }

                /* Show main page */
                foreach(array("ghNetNic", "ghIdeDev", "ghScsiDev") as $val){
                    if (!count($this->$val)){
                        $this->$val= "-";
                    }
                    $smarty->assign($val, $this->$val);
                }
            }
        foreach(array("ghCpuType", "ghMemSize", "macAddress", "ghUsbSupport",
                    "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser", "ghInventoryNumber") as $val){

            $smarty->assign($val, $this->$val);
        }
        $display= $smarty->fetch (get_template_path('info.tpl', TRUE, dirname(__FILE__)));

        return ($display);
    }

    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {
        plugin::save_object();
    }


    /* Save to LDAP */
    function save()
    {
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("System info"),
                    "plDescription" => _("System informations"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 33,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("workstation","server","terminal"),

                    "plProperties" => array(
                        array(
                            "name"          => "snmpCommunity",
                            "type"          => "string",
                            "default"       => "",
                            "description"   => _("Name of GOto SNMP community."),
                            "check"         => "gosaProperty::isString",
                            "migrate"       => "",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),

                    "plProvidedAcls"=> array()
                        ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
