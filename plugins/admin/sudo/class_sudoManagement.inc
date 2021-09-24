<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_roleManagement.inc 14742 2009-11-04 13:18:33Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class sudoManagement extends management
{
    var $plHeadline     = "Sudo rules";
    var $plDescription  = "Manage all aspects of system wide sudoers definitions";
    var $plIcon  = "plugins/sudo/images/sudo.png";
    var $matIcon = "admin_panel_settings";
    // Tab definition 
    protected $tabClass = "sudotabs";
    protected $tabType = "SUDOTABS";
    protected $aclCategory = "sudo";
    protected $aclPlugin   = "sudo";
    protected $objectName   = "sudo";

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        $this->storagePoints = array(get_ou("sudo", "sudoRDN"));

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("sudo-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("sudo-list.xml", true));
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }
        $this->registerAction("new_default", "newEntry");
        parent::__construct($config, $ui, "sudo", $headpage);
    }

    function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $str = management::newEntry($action,$target,$all,$altTabClass,$altTabType,$altAclCategory);

        /* This entry will become the default entry */
        if($action == "new_default"){
            $this->tabObject->set_default(TRUE);
        }
        if(!empty($str)) return($str);
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
