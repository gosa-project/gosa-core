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

function getkiosk ($id)
{

  if(!file_exists($id)){
    echo sprintf(_("Can't open file '%s', possibly the file does not exist."),$id);
    exit();
  }

  if(!is_readable($id)){
    echo sprintf(_("Can't read file '%s', check permissions."),$id);
    exit();
  }

  $display = file_get_contents($id);

  $nn = preg_replace("/^.*\//","",$id);

  header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
  header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
  header("Cache-Control: no-cache");
  header("Pragma: no-cache");
  header("Cache-Control: post-check=0, pre-check=0");
  header("Content-type: application/octet-stream");
  header("Content-Disposition: attachment; filename=".$nn);
	echo $display;
}


/* Basic setup, remove eventually registered sessions */
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (E_ALL | E_STRICT);
session_start ();

/* Logged in? Simple security check */
if (!session::is_set('ui')){
  new log("security","fai","",array(),"Error: getkiosk.php called without session") ;
  header ("Location: index.php");
  exit;
}
$ui= session::get("ui");
$config= session::get('config');

/* Check ACL's */
#FIXME Use more specific acl categories instead of all/all
$ui = get_userinfo();

$tmp = $ui->get_module_departments("server/goKioskService");
$found = FALSE;
foreach($tmp as $dir){
  if(preg_match("/r/",$ui->get_permissions($dir,"server/goKioskService"))){
    $found = TRUE;
    break;
  }
}
if(!$found){
  header ("Location: index.php");
  exit;
}
$dir= $config->search('environment', 'kioskpath', array('tabs','menu'));
getkiosk($dir."/".$_GET['id']);

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
