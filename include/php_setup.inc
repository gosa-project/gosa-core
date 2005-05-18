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

/* Get base dir for reference */
$BASE_DIR= dirname(dirname(__FILE__));
$ROOT_DIR= $BASE_DIR."/html";
error_reporting (E_ALL);

$variables_order= "ES";
ini_set("register_globals",0);
ini_set("track_vars",1);
ini_set("include_path",".:$BASE_DIR/include");
ini_set("display_errors",0);

/* Do smarty setup */
require("smarty/Smarty.class.php");
$smarty = new Smarty;
$smarty->template_dir = $BASE_DIR.'/ihtml/';
$smarty->caching= false;
$smarty->php_handling= SMARTY_PHP_REMOVE;

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
