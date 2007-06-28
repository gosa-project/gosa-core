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
$start = microtime();
$timing= array();
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
header("Content-type: text/html; charset=UTF-8");

/* Find all class files and include them */
get_dir_list("$BASE_DIR/plugins");

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, "$BASE_DIR/locale");
textdomain($domain);

/* Set cookie lifetime to one day (The parameter is in seconds ) */
session_set_cookie_params(24*60*60);

/* Set cache limter to one day (parameter is minutes !!)*/
session_cache_expire(60*24);  // default is 180

/* Set session max lifetime, to prevent the garbage collector to delete session before timeout.
    !! The garbage collector is a cron job on debian systems, the cronjob will fetch the timeout from 
    the php.ini, so if you use debian, you must hardcode session.gc_maxlifetime in your php.ini */
ini_set("session.gc_maxlifetime",24*60*60);

/* Remember everything we did after the last click */
session_start ();

$_SESSION['limit_exceeded'] =FALSE;

if ($_SERVER["REQUEST_METHOD"] == "POST"){
  @DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_POST, "_POST");
}
@DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_SESSION, "_SESSION");

/* Logged in? Simple security check */
if (!isset($_SESSION['config'])){
  gosa_log ("main.php called without session - logging out");
  header ("Location: logout.php");
  exit;
} 

/* Reset errors */
$_SESSION['errors']             = "";
$_SESSION['errorsAlreadyPosted']= array();
$_SESSION['LastError']          = "";

/* Check for uniqe ip address */
$ui= $_SESSION["ui"];
if ($_SERVER['REMOTE_ADDR'] != $ui->ip){
  gosa_log ("main.php called with session which has a changed IP address.", 3);
  header ("Location: logout.php");
  exit;
}
$config= $_SESSION['config'];


/* Check for invalid sessions */
if(empty($_SESSION['_LAST_PAGE_REQUEST'])){
  $_SESSION['_LAST_PAGE_REQUEST']= time();
}else{

  /* check GOsa.conf for defined session lifetime */
  if(isset($config->data['MAIN']['SESSION_LIFETIME'])){
    $max_life = $config->data['MAIN']['SESSION_LIFETIME'];
  }else{
    $max_life = 60*60*2;
  }

  /* get time difference between last page reload */
  $request_time = (time()-$_SESSION['_LAST_PAGE_REQUEST']);

  /* If page wasn't reloaded for more than max_life seconds 
   * kill session
   */
  if($request_time > $max_life){
    session_unset();
    gosa_log ("main.php called without session - logging out");
    header ("Location: logout.php");
    exit;
  }
  $_SESSION['_LAST_PAGE_REQUEST'] = time();
}


@DEBUG (DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");

/* Set template compile directory */
if (isset ($config->data['MAIN']['COMPILE'])){
  $smarty->compile_dir= $config->data['MAIN']['COMPILE'];
} else {
  $smarty->compile_dir= '/var/spool/gosa/';
}

/* Set default */
$reload_navigation = false;

/* Set last initialised language to current, browser settings */
if((!isset($_SESSION['Last_init_lang']))){
  $reload_navigation = true;
  $_SESSION['Last_init_lang'] = get_browser_language();
}

/* Language setup */
if ($config->data['MAIN']['LANG'] == ""){
  
  /* If last language != current force navi reload */
  if($_SESSION['Last_init_lang'] != get_browser_language()){
    $reload_navigation = true;
  }
  $lang= get_browser_language();
  $_SESSION['Last_init_lang'] = $lang;
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

/* Preset current main base */
if(!isset($_SESSION['CurrentMainBase'])){
  $_SESSION['CurrentMainBase']= get_base_from_people($ui->dn);
}

/* Prepare plugin list */
if (!isset($_SESSION['plist'])){
  $_SESSION['plist']= new pluglist($config, $ui);
}
$plist= $_SESSION['plist'];

/* Check for register globals */
if (isset($global_check) && $config->data['MAIN']['FORCEGLOBALS'] == 'true'){
  echo _("FATAL: Register globals is on. GOsa will refuse to login unless this is fixed by an administrator.");
  gosa_log ("Register globals is on. For security reasons, this should be turned off.");
  session_destroy ();
  exit ();
}

/* Check Plugin variable */
if (isset($_SESSION['plugin_dir'])){
  $old_plugin_dir= $_SESSION['plugin_dir'];
} else {
  $old_plugin_dir= "";
}

/* reload navigation if language changed*/  
if($reload_navigation){
  $plist->menu="";;
}
$plist->gen_headlines();
$plist->gen_menu();


if (isset($_GET['plug'])){
  $plug= validate($_GET['plug']);

  if(!in_array_ics($plug,$plist->allowed_plug_ids)){
    $plug = key($plist->allowed_plug_ids);
  }

  $plugin_dir= $plist->get_path($plug);
  $_SESSION['plugin_dir']= $plugin_dir;
  if ($plugin_dir == ""){
    gosa_log ("main.php called with invalid plug parameter \"$plug\"", 3);
    header ("Location: logout.php");
    exit;
  }
} else {
  /* set to welcome page as default plugin */
  $_SESSION['plugin_dir']= "welcome";
  $plugin_dir= "$BASE_DIR/plugins/generic/welcome";
}

/* Check if we need to delete a lock */
if ($old_plugin_dir != $plugin_dir){
  if (is_file("$old_plugin_dir/main.inc")){
    $remove_lock= true;
    require_once ("$old_plugin_dir/main.inc");
  }
}
$remove_lock= false;

/* Check for sizelimits */
eval_sizelimit();

/* Check for memory */
if (function_exists("memory_get_usage")){
  if (memory_get_usage() > (to_byte(ini_get('memory_limit')) - 2048000 )){
    print_red(_("Warning: memory is getting low - please increase the memory_limit!"));
  }
}

/* Redirect on back event */
if ($_SERVER["REQUEST_METHOD"] == "POST"){

  /* Look for button events that match /^back[0-9]+$/,
     extract the number and step the correct plugin. */
  foreach ($_POST as $key => $value){
    if (preg_match("/^back[0-9]+$/", $key)){
      $back= substr($key, 4);
      header ("Location: main.php?plug=$back");
      exit;
    }
  }
}

/* Redirect on password back event */
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['password_back'])){
  header ("Location: main.php");
  exit;
}

/* Check for multiple windows logout */
if ($_SERVER["REQUEST_METHOD"] == "POST"){
  if (isset($_POST['reset_session'])){
    header ("Location: logout.php");
    exit;
  }

  if (isset($_POST['cancel_lock'])){
    unset ($_SESSION['dn']);
  }
}


/* Load department list when plugin has changed. That is some kind of
   compromise between speed and beeing up to date */
if (isset($_GET['reset'])){
  if (isset($_SESSION['objectinfo'])){
    unset ($_SESSION['objectinfo']);
  }
}

/* Install eGOsa hooks, convert _POST to _SESSION['POST'] */
if(isset($_GET['explorer'])){
  $_SESSION{'eGosa'}=1;
}
if(isset($_SESSION['POST'])){
  $_SERVER["REQUEST_METHOD"] = "POST";
  foreach ($_SESSION['POST'] as $key => $dummy){
    $_POST[$key]=$_SESSION['POST'][$key];
  }
  unset($_SESSION['POST']);
}

/* show web frontend */
$smarty->assign ("date", date("l, dS F Y H:i:s O"));
$smarty->assign ("must", "<font class=\"must\">*</font>");
if (isset($plug)){
  $plug= "?plug=$plug";
} else {
  $plug= "";
}
if ($_SESSION['js']==FALSE){
  $smarty->assign("javascript", "false");
  $smarty->assign("help_method", "href='helpviewer.php$plug' target='_blank'");
} else {
  $smarty->assign("javascript", "true");
  $smarty->assign("help_method"," onclick=\"return popup('helpviewer.php$plug','GOsa help');\"");
}

$smarty->assign ("username", $ui->username);
$smarty->assign ("go_logo", get_template_path('images/go_logo.png'));
$smarty->assign ("go_base", get_template_path('images/dtree.png'));
$smarty->assign ("go_home", get_template_path('images/gohome.png'));
$smarty->assign ("go_out", get_template_path('images/stop.png'));
$smarty->assign ("go_top", get_template_path('images/go_top.png'));
$smarty->assign ("go_corner", get_template_path('images/go_corner.png'));
$smarty->assign ("go_left", get_template_path('images/go_left.png'));
$smarty->assign ("go_help", get_template_path('images/help.png'));

$smarty->assign ("menu", $plist->menu);
$smarty->assign ("plug", "$plug");

$header= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl'));

/* React on clicks */
if ($_SERVER["REQUEST_METHOD"] == "POST"){

  /* 'delete_lock' is set by the lock removal dialog. We should remove the
     lock at this point globally. Plugins do not need to remove it. */
  if (isset($_POST['delete_lock']) && isset($_SESSION['dn'])){
    del_lock ($_SESSION['dn']);

    /* Set old Post data */
    if(isset($_SESSION['LOCK_VARS_USED'])){
      foreach($_SESSION['LOCK_VARS_USED'] as $name => $value){
        $_GET[$name]  = $value;
        $_POST[$name] = $value;
      } 
    }
    sess_del ('dn');
  }


  /* I don't want multiple browser windows open. One way to check it is
     to count a hidden field and warn the user if SESSION and INPUT count
     differ. */
  if (isset($_POST['session_cnt'])){
    if ($_POST['session_cnt'] != $_SESSION['session_cnt']){
      $smarty->display(get_template_path('conflict.tpl'));
      exit ();
    }
    $_SESSION['session_cnt']= $_SESSION['session_cnt'] + 1;
    $_SESSION['post_cnt']= validate($_POST['session_cnt']) + 1;
  }
}

/* Only generate hidden click counter, if post_cnt is defined */
if (isset ($_SESSION['post_cnt'])){
  echo "<input type=\"hidden\" name=\"session_cnt\" value=\"".$_SESSION['post_cnt']."\">\n";
}

/* Load plugin */
if (is_file("$plugin_dir/main.inc")){
  require_once ("$plugin_dir/main.inc");
} else {
  echo sprintf(_("FATAL: Can't find any plugin definitions for plugin '%s'!"), $plug);
  exit();
}

/* Close div/tables */

  /* check if we are using account expiration */

  if((isset($config->data['MAIN']['ACCOUNT_EXPIRATION'])) &&
      preg_match('/true/i', $config->data['MAIN']['ACCOUNT_EXPIRATION'])){
    
      $expired= ldap_expired_account($config, $ui->dn, $ui->username);

      if ($expired == 2){
        gosa_log ("password for user \"$ui->username\" is about to expire");
        print_red(_("Your password is about to expire, please change your password"));
      }
  }
  
/* Print_out last ErrorMessage repeated string. */
print_red(NULL);

$smarty->assign("contents", $display);

if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}

if ($error_collector != ""){
  $smarty->assign("php_errors", preg_replace("/%BUGBODY%/",$error_collector_mailto,$error_collector)."</div>");
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

$display= $header.$smarty->fetch(get_template_path('framework.tpl'));

if ((isset($config->data['MAIN']['W3CTEST']) && preg_match('/true/i', $config->data['MAIN']['W3CTEST']))&&(!empty($display))&&(is_callable("tidy_parse_string"))) {
  tidy_parse_string(utf8_decode($display));
  $err = nl2br(htmlentities(tidy_get_error_buffer()));
  
  if($err){
    echo "<table summary=\"\" width=\"100%\" style='background-color:#E0E0E0;border-bottom:1px solid black'><tr><td><img alt=\"W3C\"            align=\"middle\" src='images/warning.png'>&nbsp;<font style='font-size:14px;font-weight:bold'>"._("Generating this page caused the W3C          conformance checker to raise some errors!")."</font></td><td align=right><button onClick='toggle(\"w3cbox\")'>"._("Toggle information")."</     button></td></tr></table><div id='w3cbox' style='width:100%; position:absolute; z-index:0; visibility: hidden; background-color:white; border-  bottom:1px solid black;'>";
    echo $err."</div>";
  }

  tidy_clean_repair($display);
}

/* Show page... */
echo $display;

/* Save plist and config */
$_SESSION['plist']= $plist;
$_SESSION['config']= $config;


/* Echo compilation time * /
$r = split(" ",$start);
$ms = $r[0];
$s= $r[1];

$re = split(" ",microtime());
$mse = $re[0];
$se= $re[1];

$add = 0;
if(($mse -$ms)<0){
  $se --;
  $add = 1;
}
echo ($se -$s).",";
echo (int)(($add+($mse -$ms))*1000)." s";
*/
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
