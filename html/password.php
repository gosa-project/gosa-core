<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007  Cajus Pollmeier

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


function displayPWchanger()
{
  global $smarty;
  $smarty->display(get_template_path('passwords.tpl'));
  exit();
}

/* Load required includes */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
header("Content-type: text/html; charset=UTF-8");

session_start();

/* Destroy old session if exists.
    Else you will get your old session back, if you not logged out correctly. */
if(is_array($_SESSION) && count($_SESSION)){
  session_destroy();
  session_start();
}

/* Reset errors */
$_SESSION['errors']             = "";
$_SESSION['errorsAlreadyPosted']= array();
$_SESSION['LastError']          = "";

/* Check if CONFIG_FILE is accessible */
if (!is_readable(CONFIG_DIR."/".CONFIG_FILE)){
  echo sprintf(_("GOsa configuration %s/%s is not readable. Aborted."), CONFIG_DIR,CONFIG_FILE);
  exit();
}

/* Parse configuration file */
$config= new config(CONFIG_DIR."/".CONFIG_FILE, $BASE_DIR);
$_SESSION['DEBUGLEVEL']= $config->data['MAIN']['DEBUGLEVEL'];
if ($_SERVER["REQUEST_METHOD"] != "POST"){
  @DEBUG (DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");
}

/* Set template compile directory */
if (isset ($config->data['MAIN']['COMPILE'])){
  $smarty->compile_dir= $config->data['MAIN']['COMPILE'];
} else {
  $smarty->compile_dir= '/var/spool/gosa';
}

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  echo sprintf(_("Directory '%s' specified as compile directory is not accessible!"),
        $smarty->compile_dir);
  exit();
}

/* Check for old files in compile directory */
clean_smarty_compile_dir($smarty->compile_dir);

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


if ($_SERVER["REQUEST_METHOD"] != "POST"){
  @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");
}


/* Check for SSL connection */
$ssl= "";
if (!isset($_SERVER['HTTPS']) ||
    !stristr($_SERVER['HTTPS'], "on")) {

  if (empty($_SERVER['REQUEST_URI'])) {
    $ssl= "https://".$_SERVER['HTTP_HOST'].
      $_SERVER['PATH_INFO'];
  } else {
    $ssl= "https://".$_SERVER['HTTP_HOST'].
      $_SERVER['REQUEST_URI'];
  }
}

/* If SSL is forced, just forward to the SSL enabled site */
if ($config->data['MAIN']['FORCESSL'] == 'true' && $ssl != ''){
  header ("Location: $ssl");
  exit;
}

/* Got a formular answer, validate and try to log in */
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['login'])){

  /* Reset error messages */
  $message= "";

  /* Destroy old sessions, they cause a successfull login to relog again ...*/
  if(isset($_SESSION['_LAST_PAGE_REQUEST'])){
    $_SESSION['_LAST_PAGE_REQUEST'] = time();
  }

  echo "Well - I'm lazy...";
}

/* Fill template with required values */
$smarty->assign ('date', gmdate("D, d M Y H:i:s"));
$smarty->assign ('username', $username);
$smarty->assign ('password_img', get_template_path('images/password.png'));

/* Some error to display? */
if (!isset($message)){
  $message= "";
}

$smarty->assign ("message", $message);

/* Displasy SSL mode warning? */
if ($ssl != "" && $config->data['MAIN']['WARNSSL'] == 'true'){
  $smarty->assign ("ssl", "<b>"._("Warning").":<\/b> "._("Session will not be encrypted.")." <a style=\"color:red;\" href=\"$ssl\"><b>"._("Enter SSL session")."<\/b></a>!");
} else {
  $smarty->assign ("ssl", "");
}

/* show login screen */
$smarty->assign ("PHPSESSID", session_id());
if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}
if ($error_collector != ""){
  $smarty->assign("php_errors", $error_collector."</div>");
} else {
  $smarty->assign("php_errors", "");
}

/* Set focus to the error button if we've an error message */
$focus= "";
if (isset($_SESSION['errors']) && $_SESSION['errors'] != ""){
  $focus= '<script language="JavaScript" type="text/javascript">';
  $focus.= 'document.forms[0].error_accept.focus();';
  $focus.= '</script>';
}
$smarty->assign("focus", $focus);

displayPWchanger();

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>

</body>
</html>
