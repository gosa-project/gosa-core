<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: getfax.php 17460 2010-04-01 13:12:08Z hickert $$
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

/* Basic setup, remove eventually registered sessions */
@require_once ("../../../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (0);
session::start();
session::set('errorsAlreadyPosted',array());

/* Logged in? Simple security check */
if (!session::is_set('ui')){
  new log("security","users/viewFaxEntries","",array(),"Error: getfax.php called without session") ;
  header ("Location: ../../index.php");
  exit;
}
$ui= session::is_set("ui");

/* Get loaded servers */
foreach (array("FAX_SERVER", "FAX_LOGIN", "FAX_PASSWORD") as $val){
  if (session::is_set($val)){
    $$val= session::get($val);
  }
}

/* Load fax entry */
$config= session::get('config');
restore_error_handler();

$cfg= $config->data['SERVERS']['FAX'];
$link = mysqli_connect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD'], 'gofax')
                  or die(_("Could not connect to database server!"));

/* Permission to view? */
$query = "SELECT id,uid FROM faxlog WHERE id = '".validate(stripcslashes($_GET['id']))."'";
$result = mysqli_query($link, $query) or die(_("Database query failed!"));
$line = mysqli_fetch_array($result, MYSQL_ASSOC);

$query = "SELECT id,fax_data FROM faxdata WHERE id = '".validate(stripcslashes($_GET['id']))."'";
$result = mysqli_query($link, $query) or die(_("Database query failed!"));

/* Load pic */
$data = mysqli_result ($result, 0, "fax_data");
mysqli_close ($link);



if (!isset($_GET['download'])){

  /* display picture */
  header("Content-type: image/png");

  /* Fallback if there's no image magick support in PHP */
  if (!function_exists("imagick_blob2image")){

    /* Write to temporary file and call convert, because TIFF sucks */
    $tmpfname = tempnam ("/tmp", "GOsa");
    $temp= fopen($tmpfname, "w");
    fwrite($temp, $data);
    fclose($temp);

    /* Read data written by convert */
    $output= "";
    $query= "convert -size 420x594 $tmpfname -resize 420x594 +profile \"*\" png:- 2> /dev/null";
    $sh= popen($query, 'r');
    $data= "";
    while (!feof($sh)){
      $data.= fread($sh, 4096);
    }
    pclose($sh);

    unlink($tmpfname);

  } else {

    /* Loading image */
    if(!$handle  =  imagick_blob2image($data))	{
      new log("view","faxreport/faxreport","",array(), "Cannot load fax image") ;
    }

    /* Converting image to PNG */
    if(!imagick_convert($handle,"PNG")) {
      new log("view","faxreport/faxreport","",array(),"Cannot convert fax image to png") ;
    }

    /* Resizing image to 420x594 and blur */
    if(!imagick_resize($handle,420,594,IMAGICK_FILTER_GAUSSIAN,1)){
      new log("view","faxreport/faxreport","",array(),"Cannot resize fax image") ;
    }

    /* Creating binary Code for the Image */
    if(!$data = imagick_image2blob($handle)){
      new log("view","faxreport/faxreport","",array(),"Reading fax image image failed") ;
    }	
  }

} else {

  /* force download dialog */
  header("Content-type: application/tiff\n");
  if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) ||
      preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
    header('Content-Disposition: filename="fax.tif"');
  } else {
    header('Content-Disposition: attachment; filename="fax.tif"');
  }
  header("Content-transfer-encoding: binary\n");
  header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
  header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
  header("Cache-Control: no-cache");
  header("Pragma: no-cache");
  header("Cache-Control: post-check=0, pre-check=0");

}

// Get ALL valid FAX-Accounts and their dns, this allows us to perform correct
//  permissions checks later.
$filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(objectClass=goFaxAccount)(uid=*))";
$tmp= get_list($filter, "users/viewFaxEntries", $config->current['BASE'],
    array("uid"), GL_SUBSEARCH | GL_NO_ACL_CHECK);
$uidToDN = array();
$uid = $line['uid'];
foreach($tmp as $attrs){
    $uidToDN[$attrs['uid'][0]] = $attrs['dn'];
}

// Detect dn to check for
$dn = $config->current['BASE'];
if(isset($uidToDN[$uid])){
    $dn = $uidToDN[$uid];
}

// We do not have any ACLs for this entry, so continue.
$ui = session::get('ui');
$acls = $ui->get_permissions($dn,"users/viewFaxEntries","detailedView");
if(!preg_match("/r/",$acls)){
    $data = file_get_contents("../../images/lists/locked.png");
}
/* print the tiff image and close the connection */
echo "$data";

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>

