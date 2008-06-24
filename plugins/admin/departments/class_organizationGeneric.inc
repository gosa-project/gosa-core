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

class organization extends department
{
	/* attribute list for save action */
  var $attributes     = array( "o", "description","userPassword","searchGuide",
      "seeAlso","businessCategory","x121Address","registeredAddress",
      "destinationIndicator","preferredDeliveryMethod","telexNumber",
      "telephoneNumber","internationaliSDNNumber","facsimileTelephoneNumber","street",
      "postOfficeBox","postalCode", "postalAddress","physicalDeliveryOfficeName",
      "st","l","teletexTerminalIdentifier");

  var $objectclasses  = array("top", "organization");

  /* Attributes 
   */ 
  var $st                       ="c";
  var $l                        ="c";
  var $description              ="c";
  var $userPassword             ="c";
  var $searchGuide              ="c";
  var $seeAlso                  ="c";
  var $businessCategory         ="c";
  var $x121Address              ="c";
  var $registeredAddress        ="c";
  var $destinationIndicator     ="c";
  var $preferredDeliveryMethod  ="c";
  var $telexNumber              ="c";
  var $telephoneNumber          ="c";
  var $internationaliSDNNumber  ="c";
  var $facsimileTelephoneNumber ="c";
  var $street                   ="c";
  var $postOfficeBox            ="c";
  var $postalCode               ="c";
  var $postalAddress            ="c";
  var $teletexTerminalIdentifier="c";
  var $physicalDeliveryOfficeName="c";

  /* Naming attributes 
   */
  var $o            ="";
  var $type         = "o";
  var $orgi_o       = "";
  var $namingAttr   = "o";

  function check()
  {
    $message = plugin::check();
    return($message);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
