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

class aclManagement extends management
{
  var $plHeadline     = "Acl";
  var $plDescription  = "Manage access control lists";
  var $plIcon  = "plugins/acl/images/plugin.png";

  // Tab definition 
  protected $tabClass = "acltab";
  protected $tabType = "ACLTABS";
  protected $aclCategory = "acl";
  protected $aclPlugin   = "acl";
  protected $objectName   = "acl";

  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;
   
    $this->storagePoints = array(); // ACLs are attached to department containers
 
    // Build filter
#    if (session::global_is_set(get_class($this)."_filter")){
#      $filter= session::global_get(get_class($this)."_filter");
#    } else {
      $filter = new filter(get_template_path("acl-filter.xml", true));
      $filter->setObjectStorage($this->storagePoints);
#    }
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("acl-list.xml", true));
    $headpage->registerElementFilter("filterLabel", "aclManagement::filterLabel");
    $headpage->setFilter($filter);

    // Add copy&paste and snapshot handler.
    if ($this->config->boolValueIsTrue("main", "copyPaste")){
      $this->cpHandler = new CopyPasteHandler($this->config);
    }
    if($this->config->get_cfg_value("enableSnapshots") == "true"){
      $this->snapHandler = new SnapshotHandler($this->config);
    }

    parent::__construct($config, $ui, "acl", $headpage);

  }

  // A filter which allows to open a department by clicking on the departments name.
  static function filterLabel($row,$dn,$params,$ou= array(),$cn=array(),$pid=0,$base="")
  {
    if(!isset($ou[0])){
      $ou = $cn[0];
    }else{
      $ou = $ou[0];
    }
    if($dn == $base){
      $ou =".";
    }
    $dn= LDAP::fix(func_get_arg(1));
    return("<a href='?plug=".$_GET['plug']."&amp;PID=$pid&amp;act=listing_open_$row' title='$dn'>$ou</a>");
  }


} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
