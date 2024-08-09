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
require_once ("../include/php_setup.inc");

session_cache_limiter("private");
session::start();
session::global_set('errorsAlreadyPosted',array());

/* Logged in? Simple security check */
if (!session::global_is_set('ui')){
    new log("security","unknown","",array(),"Error: autocomplete.php called without session") ;
    header ("Location: index.php");
    exit;
}

/* Base completition or filter completition? */
if (isset($_GET['type']) && $_GET['type'] == "base" && isset($_GET['pid'])) {
    
    // Find dn based on name and description
    $pid = $_GET['pid'];
    if (session::is_set("pathMapping_{$pid}")){
        $res= "";
        $pathMapping= session::get("pathMapping_{$pid}");
        $department_info= session::get("department_info_{$pid}");


        $search= preg_replace('/&quot;/', '"', current($_POST));
        foreach ($department_info as $dn => $info) {

            if (!isset($pathMapping[$dn])) {


                continue;
            }
            if (mb_stristr($info['name'], $search) !== false) {
                $res.= "<li>".mark($search, $pathMapping[$dn]).($info['description']==''?""
                        :"<span class='informal'> [".mark($search, $info['description'])."]</span>")."</li>";
                continue;
            }
            if (mb_stristr($info['description'], $search) !== false) {
                $res.= "<li>".mark($search, $pathMapping[$dn]).($info['description']==''?""
                        :"<span class='informal'> [".mark($search, $info['description'])."]</span>")."</li>";
                continue;
            }
            if (mb_stristr($pathMapping[$dn], $search) !== false) {
                $res.= "<li>".mark($search, $pathMapping[$dn]).($info['description']==''?""
                        :"<span class='informal'> [".mark($search, $info['description'])."]</span>")."</li>";
                continue;
            }
        }

        /* Return results */
        if (!empty($res)) {
            echo "<ul>$res</ul>";
        }
    }

} else {

    $ui = session::global_get('ui');
    $config = session::global_get('config');

    /* Is there a filter object arround? */
    if (session::is_set("autocomplete")){
		$name= session::get("autocomplete");
		$filter = session::get($name);
        $filter->processAutocomplete();
    }
}

?>
