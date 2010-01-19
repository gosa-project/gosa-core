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

class baseSelectDialog extends management
{
  // The plugin description
  public $plugname      = "Base";
  public $plIcon        = "plugins/departments/images/plugin.png";
  public $plDescription = "Choose a base";
  public $plHeadline    = "Base";

  protected $skipHeader = TRUE;
  protected $allowedBases = array();

  // #FIXME  -  Updated plugins to use a function instead of the public class var. 
  public $BaseToUse = "";
  public $dialogClose = FALSE;

  function __construct (&$config,$parent,$onlyAllowThisBases = array())
  { 
    $this->config = $config;
    $this->ui = get_userinfo();
    $this->allowedBases = $onlyAllowThisBases;
    session::set('filterBaseSelect_WhiteList', &$this->allowedBases);

#    // Build filter
#    if (session::global_is_set(get_class($this)."_filter")){
#      $filter= session::global_get(get_class($this)."_filter");
#    } else {
  $filter = new filter(get_template_path("baseSelect-filter.xml"));
#    }
  $this->setFilter($filter);

  // Build headpage
  $headpage = new listing(get_template_path("baseSelect-list.xml"));
  $headpage->registerElementFilter("depLabel", "baseSelectDialog::filterDepLabel");
  $headpage->setFilter($filter);
  $this->registerAction("open","openEntry");
  $this->registerAction("cancelBaseSelect","cancelBaseSelect");
  parent::__construct($config, $this->ui, "departments", $headpage);
  }


  // An action handler which enables to switch into deparmtment by clicking the names.
  function openEntry($action,$entry)
  {
    $headpage = $this->getHeadpage();
    $headpage->setBase(array_pop($entry));
  }

  function execute()
  {
    $str = management::execute();
    $str.= "<p class='separator'>&nbsp;</p>
      <p style=\"text-align:right\">
      <input type=submit name=\"cancelBaseSelect\" value=\"".msgPool::cancelButton()."\">
      </p>";
    return($str);
  }


  // A filter which allows to open a department by clicking on the departments name.
  static function filterDepLabel($row,$dn,$params,$ou,$pid,$base)
  {
    $ou = $ou[0];
    if($dn == $base){
      $ou =".";
    }
    $dn= LDAP::fix(func_get_arg(1));
    return("<a href='?plug=".$_GET['plug']."&amp;PID=$pid&amp;act=listing_open_$row' title='$dn'>$ou</a>");
  }


  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    if(count($target) == 1){
      $this->BaseToUse = array_pop($target);
    }
  }

  function save_object()
  {
    // Damn ... , we've to call post detection manually & thus twice ...
    // #FIXME - We should fix the class handling in the plugins to match the class_management style.
    //        - save_object isn't needed anymore, just call execute...
    $this->handleActions($this->detectPostActions());
  }


  function isClosed()
  {
    return($this->dialogClose);
  }

  function isSelected() 
  {
    return($this->BaseToUse);
  }

  function setCurrentBase($base)
  {
    $headpage = $this->getHeadpage();
    $headpage->setBase = $base;
  }

  function cancelBaseSelect()
  {
    $this->dialogClose = TRUE;
  }

  function detectPostActions()
  {
    $action = management::detectPostActions();
    if(isset($_POST['cancelBaseSelect']))  $action['action'] = "cancelBaseSelect";
    return($action);
  } 
}

class filterBaseSelect extends filterLDAP
{
  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    $res= filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);
    return(filterBaseSelect::filterEntries($res));
  }

  static function filterEntries($res)
  {
    if(!session::is_set('filterBaseSelect_WhiteList')) return $res;
    $list = session::get('filterBaseSelect_WhiteList');
    foreach($res as $key => $entry){
      if(!isset($list[$entry['dn']])) unset($res[$key]);
    }
    return(array_values($res));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
