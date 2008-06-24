<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_departmentGeneric.inc 11085 2008-05-28 10:54:49Z hickert $$
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

class country extends department
{
	/* attribute list for save action */
	var $attributes     = array( "c", "description");
	var $objectclasses  = array("top", "country");
  var $type   ="c";
  var $c      ="";
  var $orgi_c ="";

  var $namingAttr = "c";

  function check()
  {
    $message = plugin::check();
    return($message);
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array("plShortName"   => _("Country"),
          "plDescription" => _("Country"),
          "plSelfModify"  => FALSE,
          "plPriority"    => 2,
          "plDepends"     => array(),
          "plSection"     => array("admin"),
          "plCategory"    => array("department"),

          "plProvidedAcls" => array(
#            "searchGuide"                => _("Search guide"),
#            "seeAlso"                    => _("See also"),
            "street"            => _("Street"),
            "base"              => _("Base"),
            "gosaUnitTag"       => _("Administrative settings"),
            "description"       => _("Description"),
            "c"                 => _("Country name")),
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
