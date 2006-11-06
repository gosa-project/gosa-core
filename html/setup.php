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

// Starting the Session to save Form Information 
session_start();
$_SESSION['DEBUGLEVEL']= 1;


if (!isset($_GET['js']) && !isset($_SESSION['js'])){
  echo '<script language="JavaScript" type="text/javascript">';
  echo '  location = "setup.php?js=true";';
  echo '</script>';

  $_SESSION['js']= FALSE;
} else {
  $_SESSION['js']= TRUE;
}

/* Load required includes */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
require_once ("functions_setup.inc");

// Save the Post Data (back and forward button)
foreach ($_POST as $key => $val){
  $_SESSION['ldapconf'][$key] = $val;
}

/* Don't call setup if gosa.conf exists */
if (file_exists(CONFIG_DIR."/gosa.conf")){	
  header("location:index.php");	
  exit();
}

// Check if theres the example
if (!file_exists(CONFIG_TEMPLATE_DIR."/gosa.conf")){
	echo "error example of gosa.conf not present!";
}

// No Errors occured yet
$_SESSION['errors']= "";

// Print out gosa.conf 
//if(isset($_SESSION['classes'])) print "\$_SESSION['classes']=ok";
//if(isset($_SESSION['ldapconf'])) print "\$_SESSION['ldapconf']=ok";
//if(isset($_POST['getconf']))  print "\$_POST['getconf']=ok";

if ((
    isset($_SESSION['classes']) &&  
    isset($_SESSION['ldapconf']) && 
      (
      isset($_POST['getconf']) || isset($_GET['getconfig']) ))){
  echo parse_contrib_conf();
  exit;
}

/* Set detected samba version */
if (isset($classes['samba2'])) {
  $samba= "3";
} else {
  $samba= "2";
}

/* Set template compile directory */
$smarty->compile_dir= '/var/spool/gosa';

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  print_red(_("Directory '%s' specified as smarty compile directory is not accessible, please check existence and rights of this directory!"), $smarty->compile_dir);
  echo $_SESSION['errors'];
  exit();
}

/* Language setup */
$lang= get_browser_language();
header("Content-type: text/html; charset=UTF-8");
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

if ($_SERVER["REQUEST_METHOD"] != "POST"){
	// @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");
}

/* Fill template with required values */
$smarty->assign ('date', gmdate("D, d M Y H:i:s"));

if(isset ($_POST['next'])){
  $next = $_POST['next'];
} else {
  $next = 1;
}

if(isset($_POST['back'])) {
  $next = $next -2 ;     
}

if($next < 1){
  $next = 1;
}

// How this works

// Every functioon called below, has two modes
// If the parameter is false, we only test if this function 
//  is called witout an error
// Is the return value = false, then there was an error
//  and we call this func again, to output the error

// I hope this will work fine ^^
if((!show_setup_page1(false))||($next == 1)) {
  show_setup_page1();  
} elseif((!show_setup_page2(false))||($next==2)) {
  show_setup_page2();
} elseif((!show_setup_page3(false))||($next==3)) {
  show_setup_page3();
} elseif((!show_setup_page4(false))||($next==4)) {
  show_setup_page4();
}
// This is called to test if we have an administrative Group with a User in it
elseif((!create_user_for_setup(false))) {
  create_user_for_setup();
}
// this is the last Page which shows the downloadable conf file
else {
  show_setup_page5();
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
</body>
</html>
