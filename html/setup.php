<?php

/* 
 This code is part of GOsa (https://gosa.gonicus.de)
 Copyright (C) 2007 Fabian Hickert

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



/* Get standard functions */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
require_once ("class_timezones.inc");

require_once("../setup/class_setup.inc");
require_once("../setup/class_setupStep.inc");
require_once("../setup/class_setupStep_Welcome.inc");
require_once("../setup/class_setupStep_Language.inc");
require_once("../setup/class_setupStep_Checks.inc");
require_once("../setup/class_setupStep_License.inc");
require_once("../setup/class_setupStep_Ldap.inc");
require_once("../setup/class_setupStep_Config1.inc");
require_once("../setup/class_setupStep_Config2.inc");
require_once("../setup/class_setupStep_Config3.inc");
require_once("../setup/class_setupStep_Schema.inc");
require_once("../setup/class_setupStep_Migrate.inc");
require_once("../setup/class_setupStep_Feedback.inc");
require_once("../setup/class_setupStep_Finish.inc");


/* Set header */
header("Content-type: text/html; charset=UTF-8");

/* Set cookie lifetime to one day (The parameter is in seconds ) */
session_set_cookie_params(24*60*60);
session_cache_expire(60*24);  // default is 180
ini_set("session.gc_maxlifetime",24*60*60);

/* Start session */
session_start ();
$_SESSION['DEBUGLEVEL']= 1;

/* Attribute initialization, reset errors */
$_SESSION['errors']             = "";
$_SESSION['errorsAlreadyPosted']= array();
$_SESSION['LastError']          = "";

/* Set template compile directory */
$smarty->compile_dir= '/var/spool/gosa/';

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  echo sprintf(_("Directory '%s' specified as compile directory is not accessible!"),
        $smarty->compile_dir);
  exit();
}
/* Get posted language */
if(!isset($_SESSION['lang'])){
  $_SESSION['lang'] = get_browser_language();
}
if(isset($_POST['lang_selected'])){
  if($_POST['lang_selected'] != ""){
    $_SESSION['lang'] = $_POST['lang_selected'];
  }else{
    $_SESSION['lang'] = get_browser_language();
  }
}

/* Check for js */
if (!isset($_GET['js']) && !isset($_SESSION['js'])){
  echo '<script language="JavaScript" type="text/javascript">';
  echo '  location = "setup.php?js=true";';
  echo '</script>';

  $_SESSION['js']= FALSE;
} elseif(isset($_GET['js'])) {
  $_SESSION['js']= TRUE;
}

$lang = $_SESSION['lang'];
/* Append .UTF-8 to language string if necessary */
if(!preg_match("/utf(-)8$/i",$lang)){
  $lang .= ".UTF-8";
}
putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, "$BASE_DIR/locale");
textdomain($domain);


/* Call setup */
$display = "";
require_once("../setup/main.inc");

/* Print_out last ErrorMessage repeated string. */
print_red(NULL);

$smarty->assign("date", date("l, dS F Y H:i:s O"));
$header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('setup_headers.tpl'));
/* show web frontend */
$smarty->assign("contents"  , $display);
$smarty->assign("navigation", $_SESSION['setup']->get_navigation_html());
$smarty->assign("header", $_SESSION['setup']->get_header_html());
$smarty->assign("bottom", $_SESSION['setup']->get_bottom_html());

if ($error_collector != ""){
  $smarty->assign("php_errors", preg_replace("/%BUGBODY%/",$error_collector_mailto,$error_collector)."</div>");
} else {
  $smarty->assign("php_errors", "");
}
if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}else{
  $smarty->assign("errors"    , "");
}

$smarty->assign("version",get_gosa_version());

echo $header.$smarty->fetch("../setup/setup_frame.tpl");
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
