<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004  Cajus Pollmeier

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
  gosa_log ("Error: combine.php called without session");
  header ("Location: index.php");
  exit;
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");
header("Content-type: image/png");

$image1Path= $BASE_DIR."/html/".get_template_path("images/".validate($_GET['image1']));
$image2Path= $BASE_DIR."/html/".get_template_path("images/".validate($_GET['image2']));

$image1 = @imagecreatefrompng($image1Path); /* Attempt to open */
if (!$image1) {
  exit;
}
imageAlphaBlending($image1, true);
imageSaveAlpha($image1, true);
$image2 = @imagecreatefrompng($image2Path); /* Attempt to open */
if (!$image2) {
  exit;
}
imageAlphaBlending($image2, true);
imageSaveAlpha($image2, true);

/* Create final picture to display */
$pic=imagecreate(imagesx($image1)+imagesx($image2), imagesy($image1));
imagecopy ($pic, $image1, 0, 0, 0, 0, imagesx($image1), imagesy($image1));
imagecopy ($pic, $image2, imagesx($image1), 0, 0, 0, imagesx($image2), imagesy($image2));
imagePng($pic); 

error_reporting (E_ALL);

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
