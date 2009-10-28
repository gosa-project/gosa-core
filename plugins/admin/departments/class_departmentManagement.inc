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

class departmentManagement extends management
{
  var $plHeadline     = "Departments";
  var $plDescription  = "Manage departmentw";

  // Tab definition 
  protected $tabClass = "deptabs";
  protected $tabType = "DEPTABS";
  protected $aclCategory = "departement";
  protected $aclPlugin   = "generic";
  protected $objectName   = "department";

  function __construct($config,$ui)
  {
    $this->config = $config;
    $this->ui = $ui;
   
    $this->storagePoints = array("");
 
    // Build filter
    #if (session::global_is_set(get_class($this)."_filter")){
      $filter= session::global_get(get_class($this)."_filter");
#    } else {
      $filter = new filter(get_template_path("dep-filter.xml", true));
 #     $filter->setObjectStorage($this->storagePoints);
 #   }
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("dep-list.xml", true));
    $headpage->registerElementFilter("depLabel", "departmentManagement::filterDepLabel");
    $headpage->setFilter($filter);

    // Add copy&paste and snapshot handler.
    if ($this->config->boolValueIsTrue("main", "copyPaste")){
      $this->cpHandler = new CopyPasteHandler($this->config);
    }
    if($this->config->get_cfg_value("enableSnapshots") == "true"){
      $this->snapHandler = new SnapshotHandler($this->config);
    }

    parent::__construct($config, $ui, "departments", $headpage);

    $this->registerAction("open","openEntry");
  }


  function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
  {
    $types= $this->get_support_departments();
    $type = $this->headpage->getType($target[0]);
    management::editEntry($action,$target,$all,$this->tabClass,$types[$type]['TAB'],$this->aclCategory);
  }

  

  function openEntry($action,$entry)
  {
    $this->headpage->setBase(array_pop($entry));
  }


  static function filterDepLabel($row,$dn,$params,$ou,$pid,$base)
  {
    $ou = $ou[0];
    if($dn == $base){
      $ou =".";
    }
    $dn= LDAP::fix(func_get_arg(1));
    return("<a href='?plug=".$_GET['plug']."&amp;PID=$pid&amp;act=listing_open_$row' title='$dn'>$ou</a>");
  }


  /*! \brief  Returns information about all container types that GOsa con handle.
    @return Array   Informations about departments supported by GOsa.
   */
  public static function get_support_departments()
  {
    /* Domain 
     */ 
    $types = array();
    $types['domain']['ACL']     = "domain"; 
    $types['domain']['CLASS']   = "domain"; 
    $types['domain']['ATTR']    = "dc"; 
    $types['domain']['TAB']     = "DOMAIN_TABS"; 
    $types['domain']['OC']      = "domain"; 
    $types['domain']['IMG']     = "plugins/departments/images/domain.png"; 
    $types['domain']['IMG_FULL']= "plugins/departments/images/domain.png"; 
    $types['domain']['TITLE']   = _("Domain"); 
    $types['domain']['TPL']     = "domain.tpl"; 

    /* Domain component 
     */
    $types['dcObject']['ACL']     = "dcObject";
    $types['dcObject']['CLASS']   = "dcObject";
    $types['dcObject']['ATTR']    = "dc";
    $types['dcObject']['TAB']     = "DCOBJECT_TABS";
    $types['dcObject']['OC']      = "dcObject";
    $types['dcObject']['IMG']     = "plugins/departments/images/dc.png";
    $types['dcObject']['IMG_FULL']= "plugins/departments/images/dc.png";
    $types['dcObject']['TITLE']   = _("Domain Component");
    $types['dcObject']['TPL']     = "dcObject.tpl";

    /* Country object
     */
    $types['country']['ACL']     = "country";
    $types['country']['CLASS']   = "country";
    $types['country']['TAB']     = "COUNTRY_TABS";
    $types['country']['ATTR']    = "c";
    $types['country']['OC']      = "country";
    $types['country']['IMG']     = "plugins/departments/images/country.png";
    $types['country']['IMG_FULL']= "plugins/departments/images/country.png";
    $types['country']['TITLE']   = _("Country");
    $types['country']['TPL']     = "country.tpl";

    /* Locality object
     */
    $types['locality']['ACL']     = "locality";
    $types['locality']['CLASS']   = "locality";
    $types['locality']['TAB']     = "LOCALITY_TABS";
    $types['locality']['ATTR']    = "l";
    $types['locality']['OC']      = "locality";
    $types['locality']['IMG']     = "plugins/departments/images/locality.png";
    $types['locality']['IMG_FULL']= "plugins/departments/images/locality.png";
    $types['locality']['TITLE']   = _("Locality");
    $types['locality']['TPL']     = "locality.tpl";

    /* Organization
     */
    $types['organization']['ACL']     = "organization";
    $types['organization']['CLASS']   = "organization";
    $types['organization']['TAB']     = "ORGANIZATION_TABS";
    $types['organization']['ATTR']    = "o";
    $types['organization']['OC']      = "organization";
    $types['organization']['IMG']     = "plugins/departments/images/organization.png";
    $types['organization']['IMG_FULL']= "plugins/departments/images/organization.png";
    $types['organization']['TITLE']   = _("Organization");
    $types['organization']['TPL']     = "organization.tpl";

    /* Department
     */
    $types['organizationalUnit']['ACL']     = "department";
    $types['organizationalUnit']['CLASS']   = "department";
    $types['organizationalUnit']['TAB']     = "DEPTABS";
    $types['organizationalUnit']['ATTR']    = "ou";
    $types['organizationalUnit']['OC']      = "organizationalUnit";
    $types['organizationalUnit']['IMG']     = "images/lists/folder.png";//plugins/departments/images/department.png";
    $types['organizationalUnit']['IMG_FULL']= "images/lists/folder-full.png";//:wplugins/departments/images/department.png";
    $types['organizationalUnit']['TITLE']   = _("Department");
    $types['organizationalUnit']['TPL']     = "generic.tpl";

#  /* Alias
#   */
#  $types['alias']['ACL']     = "alias";
#  $types['alias']['CLASS']   = "alias";
#  $types['alias']['TAB']     = "ALIASTABS";
#  $types['alias']['ATTR']    = "???";
#  $types['alias']['OC']      = "???";
#  $types['alias']['IMG']     = "plugins/departments/images/department_alias.png";
#  $types['alias']['IMG_FULL']= "plugins/departments/images/department_alias.png";
#  $types['alias']['TITLE']   = _("Alias");
#  $types['alias']['TPL']     = "alias.tpl";
#
#  /* Referal
#   */
#  $types['referal']['ACL']     = "referal";
#  $types['referal']['CLASS']   = "referal";
#  $types['referal']['TAB']     = "REFERALTABS";
#  $types['referal']['ATTR']    = "???";
#  $types['referal']['OC']      = "???";
#  $types['referal']['IMG']     = "plugins/departments/images/department_referal.png";
#  $types['referal']['IMG_FULL']= "plugins/departments/images/department_referal.png";
#  $types['referal']['TITLE']   = _("Referal");
#  $types['referal']['TPL']     = "referal.tpl";

    return($types);
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
