<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2005  Cajus Pollmeier

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

/* Basic setup, remove eventually registered sessions */
@require_once ("../include/class_ldap.inc");
error_reporting (E_ALL);

/* Send header */
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");

/* Get requestor name */
if (!isset($_GET['requestor'])){
  echo "Bad request";
  exit;
}
if (!preg_match('/^[a-z0-9-_]+$/i', $_GET['requestor'])){
  echo "Bad request";
  exit;
}
$cn= $_GET['requestor'];

/* Trigger type */
switch ($_GET['type']){

	case "ppd":
            /* Perform LDAP query */
            $ldap= new LDAP ("", "", "ldap://se1--002:389");
            $ldap->search('(&(objectClass=gotoPrinter)(cn='.$cn.')(gotoPrinterPPD=*))');
            if ($ldap->count() != 1){
              echo "Bad request";
              exit;
            }

            /* Get result */
            $res= $ldap->fetch();
            $path= "/var/spool/".preg_replace('/^[^:]+:\/\/[^\/]+\//', '', $res['gotoPrinterPPD'][0]);
            header("Content-type: text/plain");
            echo file_get_contents($path);
            break;

	case "kiosk":
            /* Perform LDAP query */
            $ldap= new LDAP ("", "", "ldap://se1--002:389");
            $ldap->search('(&(objectClass=gotoEnvironment)(|(uid='.$cn.')(cn='.$cn.'))(gotoKioskProfile=*))');
            if ($ldap->count() != 1){
              echo "Bad request";
              exit;
            }

            # Normally we would do this:
            # Load groups we're member in
            # Look for kiosk settings in these groups, take the first that matches
            # Look for kiosk settings for the user

            /* Get result */
            $res= $ldap->fetch();
            $path= "/etc/goto/kiosk/".$res['gotoKioskProfile'][0];
            header("Content-type: octet-stream");
            echo file_get_contents($path);
            break;

  default:
            echo "Bad request";
            exit;
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
