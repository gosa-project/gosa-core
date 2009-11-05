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
  var $plDescription  = "Manage object groups";
  var $plIcon  = "plugins/ogroups/images/plugin.png";

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

    $this->storagePoints = array(get_ou("ogroupRDN"));

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
    if ($this->config->boolValueIsTrue("main", "copyPaste")){
      $this->cpHandler = new CopyPasteHandler($this->config);
    }
    if($this->config->get_cfg_value("enableSnapshots") == "true"){
      $this->snapHandler = new SnapshotHandler($this->config);
    }
    parent::__construct($config, $ui, "ogroups", $headpage);

    $this->registerAction("edit_ogroup","editEntry");
    $this->registerAction("edit_phonequeue","editEntry");
    $this->registerAction("edit_workstartup","editEntry");
    $this->registerAction("edit_termgroup","editEntry");
  }

  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    management::editEntry($action,$target);

    if(preg_match("/^edit_/",$action)){
      $tab = preg_replace("/^edit_/","",$action);
      if(isset($this->tabObject->by_object[$tab])){
        $this->tabObject->current = $tab;
      }else{
        trigger_error("Unknown tab: ".$tab);
      }
    }
  }

  static function filterProperties($row, $gosaGroupObjects)
  {

    $conv= array(
        "Y" => array("plugins/users/images/select_template.png",_("Templates") , "ogroup"),
        "U" => array("plugins/generic/images/head.png"        ,_("User")        , "ogroup"),
        "G" => array("plugins/groups/images/groups.png"       ,_("Group")       , "ogroup"),
        "A" => array("plugins/ogroups/images/application.png" ,_("Application") , "ogroup"),
        "D" => array("plugins/departments/department.png"     ,_("Department")  , "ogroup"),
        "S" => array("plugins/ogroups/images/server.png"      ,_("Server")      , "ogroup"),
        "F" => array("plugins/ogroups/images/asterisk.png"    ,_("Phone")       , "phonequeue"),
        "W" => array("plugins/ogroups/images/workstation.png" ,_("Workstation") , "workstartup"),
        "O" => array("plugins/ogroups/images/winstation.png"  ,_("Windows Install") , "ogroup"),
        "T" => array("plugins/ogroups/images/terminal.png"    ,_("Terminal")    , "termgroup"),
        "P" => array("plugins/ogroups/images/printer.png"     ,_("Printer")     , "ogroup"));

    $types = preg_replace("/[^a-z]/i","",$gosaGroupObjects[0]);
    $result ="";
    for($i = 0 ; $i < strlen($types); $i++){
      $type = $types[$i];
      $result.="<input class='center' type='image' src='".$conv[$type][0]."' ".
        "alt='".$conv[$type][1]."' title='".$conv[$type][1].
        "' name='listing_edit_".$conv[$type][2]."_$row' style='padding:1px'>";

    }
    return($result);
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
