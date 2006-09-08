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
@require_once ("class_glpi.inc");
session_start ();

/* Logged in? Simple security check */
if (!isset($_SESSION['ui'])){
  gosa_log ("Error: get_attachment.php called without session");
  header ("Location: index.php");
  exit;
}
$ui= $_SESSION["ui"];

$config = $_SESSION['config'];
/* Abort class construction, if no db is defined */
if(!isset($config->data['SERVERS']['GLPI'])){
  return;
}


// Get informations about databse connection
$data = $config->data['SERVERS']['GLPI'];

// Abort if mysql extension is missing
if(!is_callable("mysql_connect")){
  print _("Can't connect to glpi database, there is no mysl extension available in your php setup.");
  return;
}

// Create handle of glpi class, and check if database connection is established
$handle = new glpiDB($data['SERVER'],$data['LOGIN'],$data['PASSWORD'],$data['DB']);

if(!$handle->is_connected){
  print _("Can't connect to specified database, please check your glpi configuration.");
  return;
}

$att =array();
$atts = $handle->getAttachments();
$att = $atts[$_GET['id']];

if(count($att)== 0){
  print _("Can't get specified attachment file, there is no entry with this id.");
  return;
}

if(!is_readable("/etc/gosa/glpi/".$att['filename'])){
  print sprintf(_("Can't open file '%s'."),"/etc/gosa/glpi/".$att['filename']);
}
$data = file_get_contents("/etc/gosa/glpi/".$att['filename']);

/* force download dialog */
header("Content-type: ".$att['mime']."\n");
if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
    preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
  header('Content-Disposition: filename="'.$att['filename'].'"');
} else {
  header('Content-Disposition: attachment; filename="'.$att['filename'].'"');
}

header("Content-transfer-encoding: binary\n");
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");


/* print the tiff image and close the connection */
echo "$data";

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>

