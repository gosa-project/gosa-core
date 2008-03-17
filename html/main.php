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

/* Save start time */
$start = microtime();

/* Basic setup, remove eventually registered sessions */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");

/* Set header */
header("Content-type: text/html; charset=UTF-8");

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);

/* Remember everything we did after the last click */
session::start();
session::set('errorsAlreadyPosted',array());
session::set('runtime_cache',array());
session::set('limit_exceeded',FALSE);

if ($_SERVER["REQUEST_METHOD"] == "POST"){
  @DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_POST, "_POST");
}
@DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, session::get_all(), "_SESSION");

/* Logged in? Simple security check */
if (!session::is_set('config')){
  new log("security","login","",array(),"main.php called without session - logging out") ;
  header ("Location: logout.php");
  exit;
} 

/* Check for uniqe ip address */
$ui= session::get('ui');
if ($_SERVER['REMOTE_ADDR'] != $ui->ip){
  new log("security","login","",array(),"main.php called with session which has a changed IP address.") ;
  header ("Location: logout.php");
  exit;
}
$config= session::get('config');
$config->check_and_reload();

/* Enable compressed output */
if (isset($config->data['MAIN']['COMPRESSED']) && preg_match('/^(true|on)$/i', $config->data['MAIN']['COMPRESSED']) && !isset($_GET['no_output_compression'])){
  ob_start("ob_gzhandler");
}

/* Check for invalid sessions */
if(session::get('_LAST_PAGE_REQUEST') == ""){
  session::set('_LAST_PAGE_REQUEST',time());
}else{

  /* check GOsa.conf for defined session lifetime */
  if(isset($config->data['MAIN']['SESSION_LIFETIME'])){
    $max_life = $config->data['MAIN']['SESSION_LIFETIME'];
  }else{
    $max_life = 60*60*2;
  }

  /* get time difference between last page reload */
  $request_time = (time()- session::get('_LAST_PAGE_REQUEST'));

  /* If page wasn't reloaded for more than max_life seconds 
   * kill session
   */
  if($request_time > $max_life){
    session::destroy();
    new log("security","login","",array(),"main.php called without session - logging out") ;
    header ("Location: logout.php");
    exit;
  }
  session::set('_LAST_PAGE_REQUEST',time());
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
if(!session::is_set('Last_init_lang')){
  $reload_navigation = true;
  session::set('Last_init_lang',get_browser_language());
}

/* If last language != current force navi reload */
$lang= get_browser_language();
if(session::get('Last_init_lang') != $lang){
  $reload_navigation = true;
}

/* Language setup */
session::set('Last_init_lang',$lang);

/* Preset current main base */
if(!session::is_set('CurrentMainBase')){
  session::set('CurrentMainBase',get_base_from_people($ui->dn));
}

putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);
@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");

/* Prepare plugin list */
if (!session::is_set('plist')){
  /* Initially load all classes */
  $class_list= get_declared_classes();
  foreach ($class_mapping as $class => $path){
    if (!in_array($class, $class_list)){
      if (is_readable("$BASE_DIR/$path")){
        require_once("$BASE_DIR/$path");
      } else {
        msg_dialog::display(_("Fatal error"),
            sprintf(_("Cannot locate file '%s' - please run '%s' to fix this"),
              "$BASE_DIR/$path", "<b>update-gosa</b>"), FATAL_ERROR_DIALOG);
        exit;
      }
    }
  }

  session::set('plist', new pluglist($config, $ui));

  /* Load ocMapping into userinfo */
  $tmp= new acl($config, NULL, $ui->dn);
  $ui->ocMapping= $tmp->ocMapping;
  session::set('ui',$ui);
}
$plist= session::get('plist');
/* Check for register globals */
if (isset($global_check) && $config->data['MAIN']['FORCEGLOBALS'] == 'true'){
  msg_dialog::display(
            _("PHP configuration"),
            _("FATAL: Register globals is on. GOsa will refuse to login unless this is fixed by an administrator."),
            FATAL_ERROR_DIALOG);

  new log("security","login","",array(),"Register globals is on. For security reasons, this should be turned off.") ;
  session::destroy ();
  exit;
}

/* Check Plugin variable */
if (session::is_set('plugin_dir')){
  $old_plugin_dir= session::get('plugin_dir');
} else {
  $old_plugin_dir= "";
}
if (isset($_GET['plug'])){
  $plug= validate($_GET['plug']);
  $plugin_dir= $plist->get_path($plug);
  session::set('plugin_dir',$plugin_dir);
  if ($plugin_dir == ""){
    new log("security","gosa","",array(),"main.php called with invalid plug parameter \"$plug\"") ;
    header ("Location: logout.php");
    exit;
  }
} else {

  /* set to welcome page as default plugin */
  session::set('plugin_dir',"welcome");
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
    msg_dialog::display(_("Configuration error"), _("Running out of memory!"), WARNING_DIALOG);
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
    session::un_set('dn');
  }
}


/* Load department list when plugin has changed. That is some kind of
   compromise between speed and beeing up to date */
if (isset($_GET['reset'])){
  if (session::is_set('objectinfo')){
    session::un_set('objectinfo');
  }
}

/* Install eGOsa hooks, convert _POST to session */
if(isset($_GET['explorer'])){
  session::set('eGosa',TRUE);
}
if(session::is_set('POST')){
  $_SERVER["REQUEST_METHOD"] = "POST";
  foreach (session::get('POST') as $key => $dummy){
    $_POST[$key]= $dummy;
  }
  session::un_set('POST');
}

/* show web frontend */
$smarty->assign ("date", date("l, dS F Y H:i:s O"));
$smarty->assign ("must", "<font class=\"must\">*</font>");
if (isset($plug)){
  $plug= "?plug=$plug";
} else {
  $plug= "";
}
if (session::get('js')==FALSE){
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
  $plist->menu="";
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
  if (isset($_POST['delete_lock']) && session::is_set('dn')){
    del_lock (session::get('dn'));

    /* Set old Post data */
    if(session::is_set('LOCK_VARS_USED')){
      foreach(session::get('LOCK_VARS_USED') as $name => $value){
        $_GET[$name]  = $value;
        $_POST[$name] = $value;
      } 
    }
    session::un_set ('dn');
  }


  /* I don't want multiple browser windows open. One way to check it is
     to count a hidden field and warn the user if SESSION and INPUT count
     differ. */
  if (isset($_POST['session_cnt'])){
    if ($_POST['session_cnt'] != session::get('session_cnt')){
      $smarty->display(get_template_path('conflict.tpl'));
      exit ();
    }
    session::set('session_cnt', (session::get('session_cnt') + 1));
    session::set('post_cnt' , validate($_POST['session_cnt']) + 1);
  }
}

/* Only generate hidden click counter, if post_cnt is defined */
if (session::is_set('post_cnt')){
  echo "<input type=\"hidden\" name=\"session_cnt\" value=\"".session::get('post_cnt')."\">\n";
}

/* check if we are using account expiration */
if((isset($config->data['MAIN']['ACCOUNT_EXPIRATION'])) &&
    preg_match('/true/i', $config->data['MAIN']['ACCOUNT_EXPIRATION'])){

  $expired= ldap_expired_account($config, $ui->dn, $ui->username);

  if ($expired == 2){
    new log("security","gosa","",array(),"password for user \"$ui->username\" is about to expire") ;
    msg_dialog::display(_("Password change"), _("Your password is about to expire, please change your password!"), INFO_DIALOG);
  }
}

/* Load plugin */
if (is_file("$plugin_dir/main.inc")){
  require_once ("$plugin_dir/main.inc");
} else {
  msg_dialog::display(
            _("Plugin"),
            sprintf(_("FATAL: Cannot find any plugin definitions for plugin '%s'!"), $plug),
            FATAL_ERROR_DIALOG);
  exit();
}


/* Print_out last ErrorMessage repeated string. */
$smarty->assign("msg_dialogs", msg_dialog::get_dialogs());
$smarty->assign("contents", $display);

/* Assign erros to smarty */
if (session::is_set('errors')){
  $smarty->assign("errors", session::get('errors'));
}
if ($error_collector != ""){
  $smarty->assign("php_errors", preg_replace("/%BUGBODY%/",$error_collector_mailto,$error_collector)."</div>");
} else {
  $smarty->assign("php_errors", "");
}

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
    if(session::is_set($var)){
      $cookie[$ui->dn][$var] = session::get($var);
    }
  }
  if(isset($_GET['plug'])){
    $cookie[$ui->dn]['plug'] = $_GET['plug'];
  }
  @setcookie("GOsa_Filter_Settings",base64_encode(serialize($cookie)),time() + (60*60*24));
}

/* Show page... */
echo $display;

/* Save plist and config */
session::set('plist',$plist);
session::set('config',$config);
session::set('errorsAlreadyPosted',array());

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
