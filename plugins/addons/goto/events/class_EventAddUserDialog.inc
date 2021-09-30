<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_EventAddSystemDialog.inc 15243 2010-01-22 08:53:18Z hickert $$
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

class EventAddUserDialog extends management
{
    // Tab definition 
    protected $skipFooter = TRUE;
    protected $skipHeader = TRUE;

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;
        $this->storagePoints = array(
                get_ou("core", "userRDN"), 
                get_ou("core", "groupRDN")); 

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("../../addons/goto/events/eventTargetUser-filter.xml", TRUE));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("../../addons/goto/events/eventTargetUser-list.xml", TRUE));
        $headpage->setFilter($filter);
        parent::__construct($config, $ui, "object", $headpage);
    }

    function save_object() {}

    function save()
    {
        $act = $this->detectPostActions();
        $headpage = $this->getHeadpage();
        if(!isset($act['targets'])) return(array());
        $ret = array();
        foreach($act['targets'] as $dn){
            $ret[] = $headpage->getEntry($dn);
        }
        return($ret);
    }

    function get_selected_targets()
    {
        $entries = $this->save();
        $a_targets = array("USERS" => array(),"GROUPS" => array());
        foreach($entries as $entry){
            if(in_array_strict("posixGroup",$entry['objectClass'])){
                $a_targets['GROUPS'][] = $entry['cn'][0];
            }
            if(in_array_strict("gosaAccount",$entry['objectClass'])){
                $a_targets['USERS'][] = $entry['uid'][0];
            }
        }
        return($a_targets);
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
