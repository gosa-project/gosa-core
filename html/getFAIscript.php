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
restore_error_handler();
function getFAIScript ($id)
{
restore_error_handler();
  $ldap = $_SESSION['config']->get_ldap_link();
  $ldap->cd($_SESSION['config']->current['BASE']);

  $sr= $ldap->cat($id, array("FAItemplateFile", "FAIscript", "cn", "objectClass"));
  $ei= ldap_first_entry($ldap->cid, $sr);
  if(isset($_GET['is_template'])){
    $tmp = ldap_get_values_len($ldap->cid, $ei,"FAItemplateFile");
  }else{
    $tmp = ldap_get_values_len($ldap->cid, $ei,"FAIscript");
  }
  $tmp2 = $ldap->fetch();

  $name= $tmp2['cn'][0];
  if(in_array("FAIhookEntry",$tmp2['objectClass'])){
    $name.= ".FAIhook";
  }elseif(in_array("FAIscriptEntry",$tmp2['objectClass'])){
    $name .= ".FAIscript";
  }elseif(isset($_GET['is_template'])){
    if(preg_match("/\//",$name)){
      $name = preg_replace("/^.*\//","",$name); 
    }
  }

  if(isset($tmp[0])){
    header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
    header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
    header("Cache-Control: no-cache");
    header("Pragma: no-cache");
    header("Content-transfer-encoding: binary\n");
    header("Cache-Control: post-check=0, pre-check=0");
    header("Content-type: application/octet-stream");
    header("Content-Disposition: attachment; filename=".$name);
    echo $tmp[0];
  }else{
    echo sprintf("Can't query for this item '%s'",$id);
  }
}

/* Basic setup, remove eventually registered sessions */
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (E_ALL);
session_start ();

/* Logged in? Simple security check */
if (!isset($_SESSION['ui'])){
  gosa_log ("Error: getFAIScript.php called without session");
  header ("Location: index.php");
  exit;
}
$ui= $_SESSION["ui"];
$config= $_SESSION['config'];

/* Check ACL's */
$acl= get_permissions ($config->current['BASE'], $ui->subtreeACL);
$acl= get_module_permission($acl, "all", $config->current['BASE']);
if (chkacl($acl, "all") != ""){
  header ("Location: index.php");
  exit;
}
$dir = search_config($config->data,"environment", "KIOSKPATH");
getFAIScript(base64_decode($_GET['id']));

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
