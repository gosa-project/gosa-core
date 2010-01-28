<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: getbin.php 9255 2008-03-03 16:04:30Z cajus $$
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
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");

session::start();
session::global_set('errorsAlreadyPosted',array());

/* Logged in? Simple security check */
if (!session::global_is_set('ui')){
  new log("security","unknown","",array(),"Error: autocomplete.php called without session") ;
  header ("Location: index.php");
  exit;
}

/* Base completition or filter completition? */
if (isset($_GET['type']) && $_GET['type'] == "base") {
  if (session::is_set("pathMapping") && count($_POST) == 1) {
    $res= "";
    $pathMapping= session::get("pathMapping");
    $search= current($_POST);
    foreach ($pathMapping as $key => $value) {
      if (mb_stristr($value, $search) !== false) {
        $res.= "<li>$value</li>";
      }
    }

    /* Return results */
    if (!empty($res)) {
      echo "<ul>$res</ul>";
    }
  }

} else {

  $config = session::global_get('config');
  $ui = session::global_get('ui');

  /* Is there a filter object arround? */
  if (session::is_set("autocomplete")){
    $filter= session::get("autocomplete");
    $filter->processAutocomplete();
  }
}

?>
