<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

/* Get standard functions */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");

// Do not use the GOsa default error handler.
// To do so we require a valid config object - and at this point 
//  we doesn't have one.
restore_error_handler();

require_once("../setup/class_setup.inc");
require_once("../setup/class_setupStep.inc");
require_once("../setup/class_setupStep_Welcome.inc");
require_once("../setup/class_setupStep_Language.inc");
require_once("../setup/class_setupStep_Checks.inc");
require_once("../setup/class_setupStep_License.inc");
require_once("../setup/class_setupStep_Ldap.inc");
require_once("../setup/class_setupStep_Schema.inc");
require_once("../setup/class_setupStep_Migrate.inc");
require_once("../setup/class_setupStep_Finish.inc");


/* Set header */
header("Content-type: text/html; charset=UTF-8");

/* Set cookie lifetime to one day (The parameter is in seconds ) */
session_set_cookie_params(24*60*60);
session_cache_expire(60*24);  // default is 180
ini_set("session.gc_maxlifetime",24*60*60);

/* Start session */
session::start();
session::global_set('debugLevel',1);
session::set('errorsAlreadyPosted',array());

/* Attribute initialization, reset errors */
session::set('errors',"");
session::set('errorsAlreadyPosted',array());
session::set('LastError',"");

$smarty->compile_dir= "/var/spool/gosa/";
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){

    if(isset($_SERVER['SCRIPT_FILENAME'])){
        $smarty->compile_dir= preg_replace("#/html/.*$#","",$_SERVER['SCRIPT_FILENAME']);
    }
}

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
    msg_dialog::display(_("Smarty"),sprintf( _("Compile directory %s is not accessible!"),
                bold('/var/spool/gosa/')),FATAL_ERROR_DIALOG);
    exit();
}

/* Get posted language */
if(!session::global_is_set('lang')){
  session::global_set('lang',get_browser_language());
}
if(isset($_POST['lang_selected'])){
  if($_POST['lang_selected'] != ""){
    session::global_set('lang',$_POST['lang_selected']);
  }else{
    session::global_set('lang',get_browser_language());
  }
}

/* Check for js */
if (!isset($_GET['js']) && !session::global_is_set('js')){
  echo '<script language="JavaScript" type="text/javascript">';
  echo '  location = "setup.php?js=true";';
  echo '</script>';

  session::global_set('js',FALSE);
} elseif(isset($_GET['js'])) {
  session::global_set('js',TRUE);
}


$lang = session::global_get('lang');
/* Append .UTF-8 to language string if necessary */
if($lang && !preg_match("/utf(-)8$/i",$lang)){
  $lang .= ".UTF-8";
}
putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

//set the template variable for setup
$GLOBALS['theme'] = 'default';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);


/* Call setup */
$display = "";
require_once("../setup/main.inc");

$smarty->assign ("title","GOsa");
$smarty->assign("date", date("l, dS F Y H:i:s O"));
$header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('setup_headers.tpl'));



/* Set focus to the error button if we've an error message */
$focus= "";
if (session::is_set('errors') && session::get('errors') != ""){
  $focus= '<script language="JavaScript" type="text/javascript">';
  $focus.= 'document.forms[0].error_accept.focus();';
  $focus.= '</script>';
}

$focus= '<script language="JavaScript" type="text/javascript">';
$focus.= 'next_msg_dialog();';
$focus.= '</script>';

/* show web frontend */
$setup = session::global_get('setup');
$smarty->assign("contents"  , $display);
$smarty->assign("navigation", $setup->get_navigation_html());
$smarty->assign("header", $setup->get_header_html());
$smarty->assign("bottom", $focus.$setup->get_bottom_html());
$smarty->assign("msg_dialogs", msg_dialog::get_dialogs());

if ($error_collector != ""){
  $smarty->assign("php_errors", preg_replace("/%BUGBODY%/",$error_collector_mailto,$error_collector)."</div>");
} else {
  $smarty->assign("php_errors", "");
}
if(function_exists("get_gosa_version")){
  $smarty->assign("version",get_gosa_version());
}else{
  $smarty->assign("version","");
}

echo $header.$smarty->fetch("../setup/$theme/setup_frame.tpl");
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
