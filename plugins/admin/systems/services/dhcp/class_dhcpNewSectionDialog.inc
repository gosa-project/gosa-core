<?php

class dhcpNewSectionDialog extends plugin
{
    /* attribute list for save action */
    var $ignore_account     = TRUE;
    var $attributes         = array();
    var $objectclasses      = array("whatever");

    /* Mapping array */
    var $types= array();
    var $classtype= "";
    var $sectionMap= array(  "dhcpService" => array("dhcpSharedNetwork", "dhcpSubnet", "dhcpGroup", "dhcpHost", "dhcpClass", "dhcpTSigKey", "dhcpDnsZone"),
            "dhcpClass" => array("dhcpSubClass"),
            "dhcpSubClass" => array(),
            "dhcpHost" => array(),
            "dhcpGroup" => array("dhcpHost"),
            "dhcpPool" => array(),
            "dhcpSubnet" => array("dhcpPool", "dhcpGroup", "dhcpHost", "dhcpClass", "dhcpTSigKey", "dhcpDnsZone"),
            "dhcpSharedNetwork" => array("dhcpSubnet", "dhcpPool", "dhcpTsigKey", "dhcpDnsZone"),
            "dhcpTSigKey"=> array(),
            "dhcpDnsZone" => array());



    function __construct($type)
    {
        $this->initTime = microtime(TRUE);
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

        $this->types= array(  "dhcpService" => _("Global options"),
                "dhcpClass" => _("Class"),
                "dhcpSubClass" => _("Subclass"),
                "dhcpHost" => _("Host"),
                "dhcpGroup" => _("Group"),
                "dhcpPool" => _("Pool"),
                "dhcpSubnet" => _("Subnet"),
                "dhcpSharedNetwork" => _("Shared network"),
                "dhcpTSigKey" => _("DNS update key"),
                "dhcpDnsZone" => _("DNS update zone") );

        $this->classtype= $type;
    }

    function execute()
    {
        plugin::execute();

        /* Fill templating stuff */
        $smarty = get_smarty();
        $display= "";

        $sections= $this->sectionMap[$this->classtype];
        $t_sections= array();
        foreach ($sections as $section){
            $t_sections[$section]= $this->types[$section]; 
        }
        asort($t_sections);
        $ui = get_userinfo();
        $smarty->assign("sections", set_post($t_sections));
        $display.= $smarty->fetch(get_template_path('dhcpNewSection.tpl', TRUE, dirname(__FILE__)));
        return($display);
    }

    /* Get posts and set class name 
     */ 
    function save_object()
    {
    }

    /* Check given class name */
    function check()
    {
        /* Call common method to give check the hook */
        $message= "";

        return ($message);
    }


    /* Return the class name */
    function save()
    {
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
