<?php


/* This code is part of GOsa-contrib (https://oss.gonicus.de/labs/gosa-contrib)
 * Copyright (C) 2011 Alejandro Escanero Blanco (aescanero@gmail.com)
 *
 * Ported to GOsa 2.7 by Fabian Hickert (hickert@gonicus.de)
 * Copyright (C) 2012 GONICUS GmbH
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

class netgroupManagement extends management {

    var $plHeadline = "NIS Netgroups";
    var $plDescription = "NIS Netgroups management";
    var $plIcon = "plugins/netgroups/images/plugin.png";
    var $matIcon= "connect_without_contact";
    // Tab definition
    protected $tabClass = "netgrouptabs";
    protected $tabType = "NETGROUPTABS";
    protected $aclCategory = "netgroups";
    protected $aclPlugin = "netgroup";
    protected $objectName = "netgroup";

    function __construct($config, $ui) {
        $this->config = $config;
        $this->ui = $ui;

		$this->storagePoints = array(get_ou("netgroupSystem", "netgroupSystemRDN"));

        // Build filter
        if (session::global_is_set(get_class($this) . "_filter")) {
            $filter = session::global_get(get_class($this) . "_filter");
        } else {
            $filter = new filter(get_template_path("netgroup-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("netgroup-list.xml", true));
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            //$this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            //$this->snapHandler = new SnapshotHandler($this->config);
        }
        parent::__construct($config, $ui, "netgroups", $headpage);

        $this->registerAction("edit_netgroup", "editEntry");
    }

    // Inject user actions
    function detectPostActions() {
        $action = management::detectPostActions();
        if (isset($_POST['save_event_dialog']))
            $action['action'] = "saveEventDialog";
        if (isset($_POST['abort_event_dialog']))
            $action['action'] = "abortEventDialog";
        return($action);
    }

    function editEntry($action="", $target=array(), $all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="") {
        $str = management::editEntry($action, $target);

        if (preg_match("/^edit_/", $action)) {
            $tab = preg_replace("/^edit_/", "", $action);
            if (isset($this->tabObject->by_object[$tab])) {
                $this->tabObject->current = $tab;
            } else {
                trigger_error("Unknown tab: " . $tab);
            }
        }
        if (!empty($str))
            return($str);
    }

}

?>
