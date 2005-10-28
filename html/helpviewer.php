<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier, Fabian Hickert

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
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
require_once ("functions_helpviewer.inc");
header("Content-type: text/html; charset=UTF-8");

/* Start session */
session_start ();

/* Display all Errors/Warnings*/
error_reporting(E_ALL);

/* Language setup */
if ($config->data['MAIN']['LANG'] == ""){
  $lang= get_browser_language();
} else {
  $lang= $config->data['MAIN']['LANG'];
}

$lang.=".UTF-8";
putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, "$BASE_DIR/locale");
textdomain($domain);
@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");


/* Logged in? Simple security check */
if (!isset($_SESSION['ui'])){
  gosa_log ("Error: helpviewer.php called without session");
  echo "<b>"._("Help is not available if you are not logged in.")."</b>";
  exit;
}

/* Set template compile directory */
$config= $_SESSION['config'];
if (isset ($config->data['MAIN']['COMPILE'])){
  $smarty->compile_dir= $config->data['MAIN']['COMPILE'];
} else {
  $smarty->compile_dir= '/var/spool/gosa/';
}

/* Get plugin list */
$plist= $_SESSION['plist'];

/*
##################
My PART ^^
##################
 */

$helpdir                      = "/home/hickert/gosa/doc/guide/user/fr/html/"; // Folder to use for help files
$defaultpage                  = "index.html";                       // alternative file, shown on error, or on first call
$prefix                       = "node";                             // Prefix of the generated help files 
$suffix                       = ".html";                            // Suffix of the generated helpfiles
$maxresults                   = 10;                                 // max number of results shown in result list
$minwordlength                = 3;                                  // Word less than 3 chars will be dropped in search
$allowed_chars_in_searchword  = "'[^a-z0-9 %_-]'i";                 // Remove all chars that would disturb our search like < or > ...
$pre_mark                     = "<span style=\"background-color: #FFFC35;\">" ;                       // Sign words with this
$suf_mark                     = "</span>";                          //  and this
$backwardlink                 = "<a href=\"?pg=%s\"  class=\"maintitlebar\">
                                  <img src='images/back.png' align=\"middle\" alt=\""._("previous")."\" border=\"0\">
                                 </a>";

$forwardlink                 = "<a href=\"?pg=%s\"  class=\"maintitlebar\">
                                  <img src='images/forward.png' align=\"middle\" alt=\""._("next")."\" border=\"0\">
                                 </a>";


/* Default pages */
$backward =$defaultpage;
$index    =$defaultpage;
$forward  ="node1.html";

/*
   Here it begins, the real function, above only definitions
 */
/* Path exists ? */
if((is_dir($helpdir))&&($fp = opendir($helpdir))){

  /* read all available directories */
  while($dir = readdir($fp)){
    
    /* Skip . / .. */
    if(!in_array($dir,array(".","..","images"))){
      $arr[$dir] = readfiles($helpdir."/".$dir."/",$prefix,$suffix,false,$singlepage=false);
    }
  }
    
  /* Handle posts */
  if((isset($_GET['folder']))&&(is_dir($helpdir."/".$_GET['folder']."/"))){
    $_SESSION['helpbrowser']['folder'] = $_GET['folder'];
  }

  if((isset($_GET['folder']))&&(empty($_GET['folder']))){
    $_SESSION['helpbrowser']['folder']  = "";
    $_SESSION['helpbrowser']['file']    = "";
  }

  if((isset($_GET['pg']))&&(empty($_GET['pg']))){
    $_SESSION['helpbrowser']['folder']  = "";
    $_SESSION['helpbrowser']['file']    = "";
  }

  if((isset($_GET['pg']))&&(is_file($helpdir."/".$_SESSION['helpbrowser']['folder']."/".$_GET['pg']))){
    $_SESSION['helpbrowser']['file'] = $_GET['pg'];
  }

  /* Open helppage */
  if(!empty($_SESSION['helpbrowser']['folder'])){
    $folder = $_SESSION['helpbrowser']['folder'];
    if(!empty($_SESSION['helpbrowser']['file'])){
      $file = $_SESSION['helpbrowser']['file'];
      $index = readfiles($helpdir."/".$folder."/",$prefix,"",false,$file);
      $smarty->assign("help_contents",$index[$file]['content']);
    }else{
      $index = readfiles($helpdir."/".$folder."/",$prefix,"",false,$singlepage="index.html");
      $smarty->assign("help_contents",$index['index.html']['content']);
    }
  }else{
    $smarty->assign("help_contents",genIndex($arr));
  }
  $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
  $display= utf8_encode(  $header.$smarty->fetch(get_template_path('help.tpl')));
  echo $display;
}else{
  $smarty->assign("help_contents","<h2>".sprintf(_("Helpdir '%s' is not accessible, can't read any helpfiles."),$helpdir))."</h2><br>";
  $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
  $display= utf8_encode(  $header.$smarty->fetch(get_template_path('help.tpl')));
  echo $display;
}

function genIndex($arr)
{
  $str = "<h2>"._("Index")."</h2>";
  foreach($arr as $index => $obj){    
    $nr   = preg_replace("/_.*$/i","",$index);
    $name = preg_replace("/^.*_/i","",$index);
    if(empty($name)){
      $name = preg_replace("/_/","",$index);
      $nr = "";
    }
    $str .= "<p style='padding-left:20px;'><a href='?folder=".$index."'><b>".$nr." - ".$name."</b></a></p>";
  }
  return $str;
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
