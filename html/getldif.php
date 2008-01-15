<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

function dump_ldap ($mode= 0)
{
  global $config;
  $ldap= $config->get_ldap_link();

  $display = "";

  if($mode == 2){	// Single Entry Export !
    $dn =  base64_decode($_GET['dn']);
    $display = $ldap->gen_one_entry($dn);
    echo $display;
  }
  elseif($mode == 3){ // Full LDIF Export !
    $dn =  base64_decode($_GET['dn']);
    $display = $ldap->gen_ldif($dn);
    echo $display;
  }
  elseif($mode == 4){ // IVBB LDIF Export
    $dn =  base64_decode($_GET['dn']);
    $display= $ldap->gen_ldif($dn,"(objectClass=ivbbentry)",array(
          "GouvernmentOrganizationalUnit","houseIdentifier","vocation",
          "ivbbLastDeliveryCollective","gouvernmentOrganizationalPersonLocality",
          "gouvernmentOrganizationalUnitDescription","gouvernmentOrganizationalUnitSubjectArea",
          "functionalTitle","role","certificateSerialNumber","userCertificate","publicVisible",
          "telephoneNumber","seeAlso","description","title","x121Address","registeredAddress",
          "destinationIndicator","preferredDeliveryMethod","telexNumber","teletexTerminalIdentifier",
          "telephoneNumber","internationaliSDNNumber","facsimileTelephoneNumber","street",
          "postOfficeBox","postalCode","postalAddress","physicalDeliveryOfficeName","ou",
          "st","l","audio","businessCategory","carLicense","departmentNumber","displayName",
          "employeeNumber","employeeType","givenName","homePhone","homePostalAddress",
          "initials","jpegPhoto","labeledURI","mail","manager","mobile","o","pager","photo",
          "roomNumber","secretary","userCertificate","x500uniqueIdentifier","preferredLanguage",
          "userSMIMECertificate","userPKCS12"));

    echo $display;
  }
}


/* Basic setup, remove eventually registered sessions */
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (E_ALL | E_STRICT);
session::start();

/* Logged in? Simple security check */
if (!session::is_set('ui')){
  new log("security","all/all","",array(),"Error: getldif.php called without session") ;
  header ("Location: index.php");
  exit;
}
$ui= session::get("ui");
$config= session::get('config');

/* Check ACL's */
$dn =  base64_decode($_GET['dn']);
$acl = $ui->get_permissions($dn,"ldapmanager/ldifexport");
if(!preg_match("/r/",$acl)){
	echo _("Permission denied!");
	exit();
}


header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");

header("Content-type: text/plain");


switch ($_GET['ivbb']){
  case 2: dump_ldap (2);
          break;

  case 3: dump_ldap (3);
          break;

  case 4: dump_ldap (4);
          break;

  default:
          echo _("Error in ivbb parameter!");
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
