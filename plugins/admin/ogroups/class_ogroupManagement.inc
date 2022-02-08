<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class ogroupManagement extends management
{
    var $plHeadline     = "Object groups";
    var $plDescription  = "Combine different types of objects to make use of this relationship";
    var $plIcon         = "plugins/ogroups/images/plugin.png";
    var $matIcon        = "group_work";

    // Tab definition 
    protected $tabClass = "ogrouptabs";
    protected $tabType = "OGROUPTABS";
    protected $aclCategory = "ogroups";
    protected $aclPlugin   = "ogroup";
    protected $objectName   = "object group";

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        $this->storagePoints = array(get_ou("group", "ogroupRDN"));

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("ogroup-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("ogroup-list.xml", true));
        $headpage->registerElementFilter("filterProperties", "ogroupManagement::filterProperties");
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }
        parent::__construct($config, $ui, "ogroups", $headpage);

        $this->registerAction("edit_ogroup","editEntry");
        $this->registerAction("edit_phonequeue","editEntry");
        $this->registerAction("edit_workstartup","editEntry");
        $this->registerAction("edit_termgroup","editEntry");

        $this->registerAction("sendMessage", "sendMessage");
        $this->registerAction("saveEventDialog", "saveEventDialog");
        $this->registerAction("abortEventDialog", "closeDialogs");
    }


    // Inject user actions
    function detectPostActions()
    {
        $action = management::detectPostActions();

        if(isset($_POST['save_event_dialog'])) $action['action'] = "saveEventDialog";
        if(isset($_POST['abort_event_dialog'])) $action['action'] = "abortEventDialog";
        return($action);
    }


    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $str = management::editEntry($action,$target);
        if(preg_match("/^edit_/",$action)){
            $tab = preg_replace("/^edit_/","",$action);
            if(isset($this->tabObject->by_object[$tab])){
                $this->tabObject->current = $tab;
            }else{
                trigger_error("Unknown tab: ".$tab);
            }
        }
        if(!empty($str)) return($str);
    }


    /*! \brief  Sends a message to a set of users using gosa-si events.
     */
    function saveEventDialog()
    {
        $this->dialogObject->save_object();
        $msgs = $this->dialogObject->check();
        if(count($msgs)){
            msg_dialog::displayChecks($msgs);
        }
    }


    /*! \brief  Sends a message to a set of users using gosa-si events.
     */
    function sendMessage($action="",$target=array(),$all=array())
    {
      return;
    }

    static function filterProperties($row, $gosaGroupObjects)
    {
        $theme = getThemeName();

        switch ($theme) {
            case 'classic':
                $conv = array(
                    "Y" => array("plugins/users/images/select_template.png",    _("Templates"),         "ogroup"),
                    "U" => array("plugins/generic/images/head.png",             _("User"),              "ogroup"),
                    "G" => array("plugins/groups/images/select_group.png",      _("Group"),             "ogroup"),
                    "A" => array("plugins/ogroups/images/application.png",      _("Application"),       "ogroup"),
                    "D" => array("plugins/departments/department.png",          _("Department"),        "ogroup"),
                    "S" => array("plugins/ogroups/images/server.png",           _("Server"),            "ogroup"),
                    "F" => array("plugins/ogroups/images/asterisk.png",         _("Phone"),             "phonequeue"),
                    "W" => array("plugins/ogroups/images/workstation.png",      _("Workstation"),       "workstartup"),
                    "O" => array("plugins/ogroups/images/winstation.png",       _("Windows Install"),   "ogroup"),
                    "T" => array("plugins/ogroups/images/terminal.png",         _("Terminal"),          "termgroup"),
                    "P" => array("plugins/ogroups/images/printer.png",          _("Printer"),           "ogroup")
                );
                break;
            
            default:
                $conv = array(
                    "Y" => array("<i class='material-icons input-icons'>web_asset</i>",         _("Templates"),         "ogroup"),
                    "U" => array("<i class='material-icons input-icons'>person</i>",            _("User"),              "ogroup"),
                    "G" => array("<i class='material-icons input-icons'>groups</i>",            _("Group"),             "ogroup"),
                    "A" => array("<i class='material-icons input-icons'>apps</i>",              _("Application"),       "ogroup"),
                    "D" => array("<i class='material-icons input-icons'>domain</i>",            _("Department"),        "ogroup"),
                    "S" => array("<i class='material-icons input-icons'>dns</i>",               _("Server"),            "ogroup"),
                    "F" => array("<i class='material-icons input-icons'>phone</i>",             _("Phone"),             "phonequeue"),
                    "W" => array("<i class='material-icons input-icons'>computer</i>",          _("Workstation"),       "workstartup"),
                    "O" => array("<i class='material-icons input-icons'>desktop_windows</i>",   _("Windows Install"),   "ogroup"),
                    "T" => array("<i class='material-icons input-icons'>web_asset</i>",         _("Terminal"),          "termgroup"),
                    "P" => array("<i class='material-icons input-icons'>printer</i>",           _("Printer"),           "ogroup") 
                );
                break;
        }

        $types = preg_replace("/[^a-z]/i","",$gosaGroupObjects[0]);
        $result ="";
        for($i = 0 ; $i < strlen($types); $i++){
            $type = $types[$i];
            switch ($theme) {
                case 'classic':
                    $result .= image($conv[$type][0], 'listing_edit_'.$conv[$type][2].'_'.$row,$conv[$type][1]);
                    break;
                
                default:
                    $result .= image($conv[$type][0], 'listing_edit_'.$conv[$type][2].'_'.$row,$conv[$type][1]);
                    break;
            }
        }
        return($result);
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
