<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_gotomasses.inc 10967 2008-05-20 13:18:12Z hickert $$
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

class dak_repository extends plugin
{
  public function __construct()
  {

  }

  public function execute()
  {
    $smarty= get_smarty();
    return($smarty->fetch (get_template_path('dak_queue.tpl', TRUE, dirname(__FILE__))));
  }


  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Repository"),
          "plDescription" => _("DAK repository"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"       => array("addon"),
          "plCategory"      => array("dak"       => array("objectClass" => "none", "description" => _("DAK"))),
          "plProvidedAcls"  => array("Comment"   => _("Description")) 
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
