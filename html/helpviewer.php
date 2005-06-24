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

session_start ();
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

$helpdir                      = HELP_BASEDIR."/en/manual_gosa_en/"; // Folder to use for help files
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

/* Define which tags musst be delete, header, navigation, banner */
$replacements=array();
$replacements['from']=array("@<!DOC.*<BODY >@si",
    "/border=\".*\"/i",
    "'<code.*code>'",
//    "/alt=\".*\"/i",
    "/<HR>/",
    "@<ADDRESS[^>]*?>.*?ADDRESS>@si",
    "@<\/BODY[^>]*?>.*?HTML>@si",
    "'<TABLE.*>'",
    "/src.*icons/i",
    "/src=\"/i",
    "/<H1 ALIGN=\"CENTER\">/",
 /* picture replacements */
 //  "",
    );


$replacements['to']=array("",
    " border=\"0\" ",
    "",
  //  "",
    "",
    "",
    "",
    "<table border=1 cellspacing=0 bgcolor=\"#E0E0E0\" width=\"95%\" align=\"center\" cellpadding=\"3\" summary=\"\">",
    "src=\"",
    "src=\"images/",
    "<H1>",
 /* picture replacements */
//    "",
  );


/* Default pages */
$backward =$defaultpage;
$index    =$defaultpage;
$forward  ="node1.html";

/*
   Here it begins, the real function, above only definitions
 */

if(!file_exists(HELP_BASEDIR."/en/manual_gosa_en/")){
  /* prevent php warning missing value ... or so*/
  $smarty->assign("backward","");
  $smarty->assign("index"   ,"");
  $smarty->assign("forward" ,"");
  $smarty->assign("search_string","");
  $smarty->assign("help_contents","<br>".sprintf(_("Can't read any helpfiles from ' %s ', possibly there is no help available."),HELP_BASEDIR."/en/manual_gosa_en/"));

  /* Output html ...*/
  $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
  $display= $header.$smarty->fetch(get_template_path('help.tpl'));
  echo $display;

}else{  

  /* We prepare to search, all Document for the given keyword */
  if(isset($_POST['search'])){

    /* Get Keyword */
    $keyword = $_POST['search_string'];  

    /* Save Keyword to be able to show last searched word in template */
    $_SESSION['search_string']= $keyword;

    /* Read all files with contents*/
    /*               |Folder="/var/ww...", 
                     |        |Fileprefix="node"
                     |        |       |Filesuffix=".html"
                     |        |       |       |WithoutContent=false(This means : read content)
                     |        |       |       |     |Singlepage=false(Means read all, if w want to read single, specify its filename)"*/
    $arr = readfiles($helpdir,$prefix,$suffix,false,$singlepage=false);

    /* Create Searchresult for our Keyword(s) */
    $res = search($arr,$keyword); 

    /* Tell smarty which pages to use for backward forwa.. */
    $smarty->assign("backward","");
    $smarty->assign("index"   ,$index);
    $smarty->assign("forward" ,"");

    /* Tell smarty the Keyword, to show it in the input field again */
    $smarty->assign("search_string",$keyword);

    /* Create result list */
    $smarty->assign("help_contents",searchlist($arr,$res,$maxresults));

    /* show some errors */
    if (isset($_SESSION['errors'])){
      $smarty->assign("errors", $_SESSION['errors']);
    }
    if ($error_collector != ""){
      $smarty->assign("php_errors", $error_collector."</div>");
    } else {
      $smarty->assign("php_errors", "");
    }

    /* Output html ...*/
    $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
    $display= $header.$smarty->fetch(get_template_path('help.tpl'));
    echo $display;

    /*
       Don't search, only show selected page
     */
  }else{

    /* present last searched word(s)*/
    if(!isset($_SESSION['search_string'])){
      $_SESSION['search_string']="";
    }


    $smarty->assign("search_string",$_SESSION['search_string']);

    /* Read all files, prepare to serach */
    $helppages = readfiles($helpdir,$prefix,$suffix,true);

    /* Get transmitted page */
    if(isset($_GET['pg'])){
      $page = $_GET['pg'];
    }else{
      $page = $defaultpage;
    }
    
    /* test if this page exists, in our array of files */
    if((!isset($helppages[$page]))&&($page!=$defaultpage))
    {
      //print "Requested helppage is unknown, redirekted to index"; // For debugging only
      $page = $defaultpage;
    }

    /* Check forward backward, funtionality*/
    if($page != $defaultpage)
    {
      /* Extract Number of node page */
      $number = str_replace($prefix,"",str_replace($suffix,"",$page));

      /* Check if we can switch forward and/or backward*/ 
      $bck = $prefix.($number-1).$suffix;
      $fck = $prefix.($number+1).$suffix;

      /* backward page exists ?, so allow it*/
      if((isset($helppages[$bck]))) {
        $backward = $bck;
      }

      $forward  = $fck;
    }

    $help_contents=readfiles($helpdir,$prefix,$suffix,false,$page);

    /* Mark last searched words */
    if(isset($_GET['mark'])){
      $marks = ($_SESSION['lastresults']) ; 
      $help_contents = markup_page($help_contents[$page]['content'],$marks[$page]);
      $help_contents=$help_contents;  
    }else{
      $help_contents=$help_contents[$page]['content'];
    }

    $smarty->assign("help_contents",$help_contents);

    /* Define our own navigation pages */
    if($page == $defaultpage){
      $smarty->assign("backward","");
    }else{
      $smarty->assign("backward",sprintf($backwardlink,$backward));
    }
    $smarty->assign("index"   ,$index);
  
    if(!(isset($helppages[$forward]))){
      $smarty->assign("forward","");
    }else{
      $smarty->assign("forward",sprintf($forwardlink,$forward));
    }

    /* show some errors */
    if (isset($_SESSION['errors'])){
      $smarty->assign("errors", $_SESSION['errors']);
    }
    if ($error_collector != ""){
      $smarty->assign("php_errors", $error_collector."</div>");
    } else {
      $smarty->assign("php_errors", "");
    }

    /* Fill page */
    $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
    $display= $header.$smarty->fetch(get_template_path('help.tpl'));
    echo $display;
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
