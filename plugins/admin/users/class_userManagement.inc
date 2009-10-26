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

class userManagement extends management
{
  var $plHeadline     = "Users";
  var $plDescription  = "Manage users";
  var $plIcon  = "plugins/users/images/user.png";

  // Tab definition 
  protected $tabClass = "usertabs";
  protected $tabType = "USERTABS";
  protected $aclCategory = "users";
  protected $aclPlugin   = "user";
  protected $objectName   = "user";

  function __construct($config,$ui)
  {
    // Build filter
    $filter = new filter(get_template_path("user-filter.xml", true));
    $filter->setObjectStorage("ou=people,");

    // Build headpage
    $headpage = new listing(get_template_path("user-list.xml", true));
    $headpage->setFilter($filter);
    $this->cpHandler = new CopyPasteHandler($this->config);

    parent::__construct($config, $ui, "users", $headpage);

    $this->registerAction("new",    "newEntry");
    $this->registerAction("edit",   "editEntry");
    $this->registerAction("apply",  "applyChanges");
    $this->registerAction("save",   "saveChanges");
    $this->registerAction("cancel", "cancelEdit");
    $this->registerAction("remove", "removeEntryRequested");
    $this->registerAction("removeConfirmed", "removeEntryConfirmed");

    $this->registerAction("copy",   "copyPasteHandler");
    $this->registerAction("cut",    "copyPasteHandler");
    $this->registerAction("paste",  "copyPasteHandler");
  }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
