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

/* Basic setup, remove eventually registered sessions */
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (0);
session_start ();

/* Logged in? Simple security check */
if (!isset($_SESSION['ui'])){
  gosa_log ("Error: getvcard.php called without session");
  header ("Location: index.php");
  exit;
}

/* Uid parameter set? */
if (!isset($_GET['dn']) || $_GET['dn'] == ""){
  print_red (_("Error: getcvard.php needs a parameter to export a vcard!"));
  exit;
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");
header("Content-type: text/x-vcard; charset=utf-8");
header("Content-type: text/plain");
if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
    preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {

  header('Content-Disposition: filename="'.$name.'.vcf"');
} else {
  header('Content-Disposition: attachment; filename="'.$name.'.vcf"');
}

/* Get entry */
$config= $_SESSION['config'];
$ldap= $config->get_ldap_link();
$ldap->cat(base64_decode(validate($_GET['dn'])));

/* 
 * Generate vcard for specified IDs
 */
while ($attrs= $ldap->fetch()){
  /* Header / Name */
  echo "BEGIN:VCARD\n";
  echo "VERSION:3.0\n";
  echo "FN:".preg_replace('/,/', '\\,', $attrs['cn'][0])."\n";

  /* Assemble titles for N attribute */
  $titles= "";
  if (isset($attrs['personalTitle'])){
    $titles= $attrs['personalTitle'][0];
  }
  if (isset($attrs['academicTitle'])){
    if ($titles != ""){
      $titles.= ",".$attrs['academicTitle'][0];
    } else {
      $titles= $attrs['academicTitle'][0];
    }
  }
  echo "N:".$attrs['sn'][0].";".$attrs['givenName'][0].";;;$titles\n";

  /* Generate random UID */
  $uid= "";
  srand(make_seed());
  $chars= '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
  for ($i= 0; $i<16; $i++){
    $uid.= $chars[rand(0, strlen($chars)-1)];
  }
  echo "UID:$uid\n";

  /* Mail addresses */
  if (isset($attrs['mail'][0])){
    echo "EMAIL;TYPE=internet,pref:".$attrs['mail'][0]."\n";
    if (isset($attrs['gosaMailAlternateAddress'])){
      for ($i= 0; $i<$attrs['gosaMailAlternateAddress']['count']; $i++){
        echo "EMAIL;TYPE=internet:".
          $attrs['gosaMailAlternateAddress'][$i]."\n";
      }
    }
  }

  /* Export date */
  echo "REV:".date("Y-m-d")."\n";
  echo "CLASS:PUBLIC\n";

  /* Fill address */
  if (isset($attrs['homePostalAddress'])){
    @list($street,$town,$country)= preg_split('/\n/', $attrs['homePostalAddress'][0]);
    echo "ADR;TYPE=home:;;".trim($street).";".trim($town).";;;".
      trim($country)."\n";
  }
  if (isset($attrs['postalAddress'])){
    @list($street,$town,$country)= preg_split('/\n/', $attrs['postalAddress'][0]);
    echo "ADR;TYPE=work,pref:;;".trim($street).";".trim($town).";;;".
      trim($country)."\n";
  }

  /* Telephone numbers */
  if (isset($attrs['homePhone'])){
    echo "TEL;TYPE=home:".$attrs['homePhone'][0]."\n";
  }
  if (isset($attrs['telephoneNumber'])){
    echo "TEL;TYPE=work,pref:".$attrs['telephoneNumber'][0]."\n";
  }
  if (isset($attrs['mobile'])){
    echo "TEL;TYPE=cell:".$attrs['mobile'][0]."\n";
  }
  if (isset($attrs['pager'])){
    echo "TEL;TYPE=pager:".$attrs['pager'][0]."\n";
  }

  /* Set organization */
  if (isset($attrs['o'])){
    echo "ORG:".$attrs['o'][0]."\n";
  }

  echo "NOTE:Exported by GOsa - https://gosa.gonicus.de\n";
  echo "SORT-STRING:".$attrs['sn'][0]."\n";
  if (isset($attrs['labeledURI'][0])){
    echo "URL:".$attrs['labeledURI'][0]."\n";
  }

  /* Add user certificate */
#if (isset($attrs['userCertificate;binary'])){
#	$cert= $ldap->get_attribute($ldap->getDN(), "userCertificate;binary");
#	$cert= preg_replace('/\r\n/', chr(10).' ', chunk_split(base64_encode($cert)));
#	echo "KEY;ENCODING=b:".preg_replace('/\n $/', '', $cert)."\n";
#}

  /* Add picture */
  if (isset($attrs['jpegPhoto'])){
    $photodata= $ldap->get_attribute($ldap->getDN(), "jpegPhoto");
    $photodata= preg_replace('/\r\n/', chr(10).' ', chunk_split(base64_encode($photodata)));
    echo "PHOTO;ENCODING=b;TYPE=JPEG:".preg_replace('/\n $/', '', $photodata)."\n";
  }	

  /* Day of birth */
  if (isset($attrs['dayOfBirth'][0])){
    echo "BDAY:".$attrs['dayOfBirth'][0]."\n";
  }

  echo "END:VCARD\n\n";
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
