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
require_once ("../include/php_setup.inc");
require_once ("../include/functions.inc");
session_start ();
error_reporting(E_ALL);
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

/* Get plugin list */
$plist= $_SESSION['plist'];

/*
##################
My PART ^^
##################

 */
//set_error_handler("myone");
$helpdir      = "../doc/guide/admin/en/manual_gosa_en/";
$defaultpage  = "index.html";
$prefix       = "node";
$suffix       = ".html";
$maxresults   = 10;

function myone($par1,$par2,$par3,$par3)
{
  print "<br>Seite : ".$par1."<br>Name : ".$par2."<br>Seite : ".$par3."<br>Zeile : ".$par3;
}

/* Define which tags musst be delete, header, navigation, banner */
$i=0;
$replacements=array();
$replacements['range'][$i]['from']    = "/<!DOC.*<BODY >/";
$replacements['range'][$i]['to']      = "";
$i++;
$replacements['range'][$i]['from']  = "@<DIV[^>]*?>.*?DIV>@si";
$replacements['range'][$i]['to']      = "";
$i++;
$replacements['range'][$i]['from']  = "/<HR>/";
$replacements['range'][$i]['to']      = "";
$i++;
$replacements['range'][$i]['from']  = "@<ADDRESS[^>]*?>.*?ADDRESS>@si";
$replacements['range'][$i]['to']      = "";
$i++;
$replacements['range'][$i]['from']  = "/<\/BODY.*>/";
$replacements['range'][$i]['to']      = "";
$i++;

/* Bsp . : Replace  Table Head to specified tableheader */ 
$replacements['range'][$i]['from']  = "@<TABLE[^>]*?>.*?>@si";
$replacements['range'][$i]['to']    = "<table border=0 cellspacing=1 bgcolor=\"#999999\" width=\"95%\" align=\"center\" >" ;

/* Default pages */
$backward ="index.html";
$index    ="index.html";
$forward  ="node1.html";

/*
   Here it begins, the real function, above only definitions
 */

$smarty->assign("search_string","");


if(isset($_POST['search'])){

  $keyword = $_POST['search_string'];  

  $arr = readfiles($helpdir,$prefix,$suffix,false,$singlepage=false);

  $res = search($arr,$keyword); 

  $smarty->assign("backward",$backward);
  $smarty->assign("index"   ,$index);
  $smarty->assign("forward" ,$forward);


  $smarty->assign("search_string",$keyword);

  $smarty->assign("help_contents",searchlist($arr,$res,$maxresults));

  $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
  $display= $header.$smarty->fetch(get_template_path('help.tpl'));
  echo $display;


}else{

  /* Read all files, prepare to serach */
  $helppages = readfiles("../doc/guide/admin/en/manual_gosa_en/",$prefix,true,$suffix);

  if(isset($_GET['pg'])){
    $page = $_GET['pg'];
  }

  if((!isset($helppages[$page]))&&($page!="index.html"))
  {
    print "Requested helppage is unknown, redirekted to index";
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

    /* forward exists ?*/
    if((isset($helppages[$fck])))  {
      $forward  = $fck;
    }
  }

  $help_contents=readfiles($helpdir,$prefix,$suffix,false,$page);

  /* Mark last searched words */
  if($_GET['mark']){
    $marks = ($_SESSION['lastresults']) ; 
    $help_contents = markup_page($help_contents[$page]['content'],$marks[$page]);
    $help_contents=remove_unwanted_tags($help_contents,$replacements);  
  }else{
    $help_contents=remove_unwanted_tags($help_contents[$page]['content'],$replacements);
  }


  $smarty->assign("help_contents",$help_contents);

  /* Define our own navigation pages */
  $smarty->assign("backward",$backward);
  $smarty->assign("index"   ,$index);
  $smarty->assign("forward" ,$forward);

  /* Fill page */
  $header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));
  $display= $header.$smarty->fetch(get_template_path('help.tpl'));
  echo $display;
}



/* 
   Only function definition will follow here
/*



/* Reads all files in specified directory with contents an some inforations about the file */
function readfiles($basedir,$prefix,$suffix,$onlyIndex,$singlepage=false)
{
  global $replacements;

  $str    = array();  // Temporary variable
  $cnt    = 0;        // Array index creation
  $file   = "";       // Contains Filename

  $dir = opendir($basedir);

  $str['global']['start']       = $cnt;     // collect basic informations - Startpage
  $str['global']['basedir']     = $basedir; // collect basic informations - Basedirectory

  /* Startime for Benchmark */ 
  $start =   (time()+microtime());
  if(!$singlepage) {
    while (($file = readdir($dir)) !== false) {
      if((strstr($file,".html"))&&($file!=".")&&($file!="..")&&(strstr($file,$prefix))){
        $str[$file]=array();
        $str[$file]['name']   = $file;
        $str[$file]['size']   = filesize($basedir.$file);
        if(!$onlyIndex){
          $str[$file]['content']= remove_unwanted_tags(linkwrapper(getcontents($basedir.$file),""),$replacements);
        }
        $str[$file]['stat']   = stat($basedir.$file);
        $cnt++;
      }
    }
  }else{
    $file = $singlepage;
    $str[$file]=array();
    $str[$file]['name']   = $file;
    $str[$file]['size']   = filesize($basedir.$file);
    if(!$onlyIndex){
      $str[$file]['content']= remove_unwanted_tags(linkwrapper(getcontents($basedir.$file),""),$replacements);
    }
    $str[$file]['stat']   = stat($basedir.$file);
  }

  /* Create right order */
  asort($str);
  /* Endtime for Benchmark*/
  $end = (time()+microtime());
  $str['global']['cmptime'] = $end-$start;
  $str['global']['numpages']= $cnt;
  closedir($dir);
  return($str);
}

/* Read filecontent */
function getcontents($file)
{
  $str = "" ;   // Temporary variable for file contents 
  $tmp = "" ;   // Temporary varibale for partitial file contents
  $fp = fopen($file,"r");
  if($fp) {
    while($tmp = fread($fp,512))
    {
      $str.=  $tmp;
    }
  }else{
    return(false);
  }
  return($str);
}

function remove_unwanted_tags($str,$replacements)
{
  $str=str_replace("\n","||WasBr||",$str);
  foreach($replacements['range'] as $var)
  {
    $str=preg_replace($var['from'],$var['to'],$str);
  }

  $str=str_replace("||WasBr||","\n",$str);
  return($str);
}

/*Converts the all links to specified path, is needed to get simple navigation */
function linkwrapper($str,$link)
{
  $str=str_replace("HREF=\"","href=\"".$link."?pg=",$str);  
  return($str);
}



function search($arr,$word)
{
  /* Prepare Vars */ 
  $result                     =array(); // Search result, filename, + hits + hits per word + matches 
  $words                      =array(); // Temporary searchword handling
  $useablewords               =array(); // Temporary searchword handling
  $tryword                    = "";     // Temporary searchword handling
  $result['global']['maxhit'] = 0;
  unset($_SESSION['lastresults']);

  /* prepare searchwords */
  $word   = trim($word);

  /* Filter all unusable chars */
  $word   = preg_replace("[^a-z0-9_+% ]","",$word);
  $words  = split(" ",str_replace("+"," ",$word));

  /* Check all wordlengths */
  foreach($words as $tryword){
    $tryword = trim($tryword);

    if(strlen($tryword)>=3) {
      $useablewords[]=$tryword;
    }
  }

  /* Use words to search the content */
  foreach($arr as $key=>$val)
  {
    $overallhits=0;
    foreach($useablewords as $word)
    {
      if($key!="global")
      {
        preg_match_all("/".$word."/i",$arr[$key]['content'], $matches,PREG_OFFSET_CAPTURE);
        $overallhits=$overallhits + count($matches[0]);    

        $result[$key]['hits'][$word]    = count($matches[0]); 
        $result[$key]['hits']['overall']= $overallhits;  

        if($overallhits > $result['global']['maxhit']){
          $result['global']['maxhit']=$overallhits;  
        }

        $result[$key]['match'][$word]=array();
        $result[$key]['match'][$word]=$matches[0];
      }
    }
  }
  $_SESSION['lastresults'] = $result;
  return($result);
}

/* Detect 10 Best result entries, sort and call createResultEntry to create HTML output for  complete list */
function searchlist($arr,$res,$maxresults)
{
  $global = $res['global'];
  $topten = array();        // To detect 10 best solutions
  $ret    = "";             // return value
  unset($res['global']);

  /* Detect 10 best Sites */
  foreach($res as $key=>$val){
    $topten[$key] = $val['hits']['overall']; 
  }

  asort($topten);
  $topten = array_reverse($topten);
  $topten = (array_slice($topten,0,$maxresults));

  /* We have a result, an array with all content, an array with hits and position of hits and we have the 10 best hits */

  /* Foreach */  
  foreach($topten as $name => $hits)
  {
    $ret.= createResultEntry($arr[$name],$res[$name],$name,$global['maxhit']);    
  }  
  return($ret);
}

/* This function marks a string with the given search result for this string*/
function markup_page($arr,$res)
{
  $ret    = "";             // return value
  $pre    = "<b><u><i>" ;
  $suf    = "</i></u></b>"; 
  $repl   = array();
  $posadd = 0;

  foreach($res['match'] as $word => $matches)   {
    foreach($matches as $matchnr=>$match)   {
      $repl[$match[1]]=$match[0];
    }
  }

  ksort($repl);

  foreach($repl as $position=>$word)  {
    $pos1 = strlen($arr);
    $arr= markword($arr,($position+$posadd),$word,$pre,$suf);
    $pos2 = strlen($arr);
    $posadd =$posadd + ($pos2 - $pos1);
  }
  return($arr); 
}

/* This function marks a single word with the specified prefix and suffix */
function markword($string,$position,$word,$prefix,$suffix)
{
  $wordlength   = strlen($word);
  $wholelength  = strlen($string); 

  $first = substr($string,0,$position);
  $last  = substr($string,($position+$wordlength),$wholelength);  

  return($first.$prefix.$word.$suffix.$last);
} 


/* Creates HTML output for a single search result entry */
function createResultEntry($entry,$res,$name,$max)
{

  $percentage = (int)(($res['hits']['overall'] / $max) * 100) ;

  $str =  "<b><a href=\"?pg=".$name."&mark=1\">With ".$percentage."% hit in file ".$name."</a></b><br>" ;
  $str.=  substr(strip_tags($entry['content']),0,200);
  $str.=  "<hr>";

  return($str);
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
