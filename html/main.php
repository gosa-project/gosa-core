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

/* Save start time */
$start = microtime();

/* Basic setup, remove eventually registered sessions */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
require_once ("functions_FAI.inc");

/* Set header */
header("Content-type: text/html; charset=UTF-8");

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
  new log("security","login","",array(),"main.php called without session - logging out") ;
  header ("Location: logout.php");
  exit;
} 

/* Check for uniqe ip address */
$ui= $_SESSION["ui"];
if ($_SERVER['REMOTE_ADDR'] != $ui->ip){
  new log("security","login","",array(),"main.php called with session which has a changed IP address.") ;
  header ("Location: logout.php");
  exit;
}
$config= $_SESSION['config'];
$config->check_and_reload();

/* Enable compressed output */
if (isset($config->data['MAIN']['COMPRESSED']) && preg_match('/^(true|on)$/i', $config->data['MAIN']['COMPRESSED'])){
  ob_start("ob_gzhandler");
}

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
    new log("security","login","",array(),"main.php called without session - logging out") ;
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

/* If last language != current force navi reload */
$lang= get_browser_language();
if($_SESSION['Last_init_lang'] != $lang){
  $reload_navigation = true;
}

/* Language setup */
$_SESSION['Last_init_lang'] = $lang;

/* Preset current main base */
if(!isset($_SESSION['CurrentMainBase'])){
  $_SESSION['CurrentMainBase']= get_base_from_people($ui->dn);
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
@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");

/* Prepare plugin list */
if (!isset($_SESSION['plist'])){
  /* Initially load all classes */
  $class_list= get_declared_classes();
  foreach ($class_mapping as $class => $path){
    if (!in_array($class, $class_list)){
        require_once("$BASE_DIR/$path");
    }
  }
  
  $_SESSION['plist']= new pluglist($config, $ui);

  /* Load ocMapping into userinfo */
  $tmp= new acl($config, NULL, $ui->dn);
  $ui->ocMapping= $tmp->ocMapping;
  $_SESSION['ui']= $ui;
}
$plist= $_SESSION['plist'];

/* Check for register globals */
if (isset($global_check) && $config->data['MAIN']['FORCEGLOBALS'] == 'true'){
  echo _("FATAL: Register globals is on. GOsa will refuse to login unless this is fixed by an administrator.");
  new log("security","login","",array(),"Register globals is on. For security reasons, this should be turned off.") ;
  session_destroy ();
  exit ();
}

/* Check Plugin variable */
if (isset($_SESSION['plugin_dir'])){
  $old_plugin_dir= $_SESSION['plugin_dir'];
} else {
  $old_plugin_dir= "";
}
if (isset($_GET['plug'])){
  $plug= validate($_GET['plug']);
  $plugin_dir= $plist->get_path($plug);
  $_SESSION['plugin_dir']= $plugin_dir;
  if ($plugin_dir == ""){
    new log("security","gosa","",array(),"main.php called with invalid plug parameter \"$plug\"") ;
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

/* reload navigation if language changed*/  
if($reload_navigation){
  $plist->menu="";;
}
$plist->gen_headlines();
$plist->gen_menu();
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

/* check if we are using account expiration */
if((isset($config->data['MAIN']['ACCOUNT_EXPIRATION'])) &&
    preg_match('/true/i', $config->data['MAIN']['ACCOUNT_EXPIRATION'])){

  $expired= ldap_expired_account($config, $ui->dn, $ui->username);

  if ($expired == 2){
    new log("security","gosa","",array(),"password for user \"$ui->username\" is about to expire") ;
    print_red(_("Your password is about to expire, please change your password"));
  }
}

/* Load plugin */
if (is_file("$plugin_dir/main.inc")){
  require_once ("$plugin_dir/main.inc");
} else {
  echo sprintf(_("FATAL: Can't find any plugin definitions for plugin '%s'!"), $plug);
  exit();
}


/* Print_out last ErrorMessage repeated string. */
if(isset($_GET['add'])){
msg_dialog::display("Error"   ,"Kann datei nicht schreiben, bla",ERROR_DIALOG);
msg_dialog::display("Warning" ,"Kann datei nicht schreiben, bla",WARNING_DIALOG);
msg_dialog::display("Info"    ,"Kann datei nicht schreiben, bla",INFO_DIALOG);
msg_dialog::display("Confirm" ,"Kann datei nicht schreiben, bla",CONFIRM_DIALOG);
}

$smarty->assign("msg_dialogs", msg_dialog::get_dialogs());
$smarty->assign("contents", $display);

/* Assign erros to smarty */
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

$focus= '<script language="JavaScript" type="text/javascript">';
$focus.= 'next_msg_dialog();';
$focus.= '</script>';
$smarty->assign("focus", $focus);

$display= $header.$smarty->fetch(get_template_path('framework.tpl'));

/* Save dialog filters and selected base in a cookie. 
   So we may be able to restore the filter an base settings on reload.
*/
$cookie = array();

if(isset($_COOKIE['GOsa_Filter_Settings'])){
  $cookie = unserialize(base64_decode($_COOKIE['GOsa_Filter_Settings']));
}elseif(isset($HTTP_COOKIE_VARS['GOsa_Filter_Settings'])){
  $cookie = unserialize(base64_decode($HTTP_COOKIE_VARS['GOsa_Filter_Settings']));
}
if(isset($config->data['MAIN']['SAVE_FILTER']) && preg_match("/true/",$config->data['MAIN']['SAVE_FILTER'])){
  $cookie_vars = array("MultiDialogFilters","CurrentMainBase");
  foreach($cookie_vars as $var){
    if(isset($_SESSION[$var])){
      $cookie[$ui->dn][$var] = $_SESSION[$var];
    }
  }
  if(isset($_GET['plug'])){
    $cookie[$ui->dn]['plug'] = $_GET['plug'];
  }
  setcookie("GOsa_Filter_Settings",base64_encode(serialize($cookie)),time() + (60*60*24));
}

/* Show page... */
echo $display;

/* Save plist and config */
$_SESSION['plist']= $plist;
$_SESSION['config']= $config;
$_SESSION['errorsAlreadyPosted']= array();

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
