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

// Will be used in the "stats" plugin later, to be able calculate the elapsed render time.
$overallRenderTimer = microtime(TRUE);

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
session::global_set('runtime_cache',array());
session::set('limit_exceeded',FALSE);

// Count number of page reloads 
if(!session::is_set('clicks')){
    session::set('clicks', 0);
}
$clicks = session::get('clicks');
$clicks ++ ;
session::set('clicks', $clicks);

pathNavigator::clear();

if ($_SERVER["REQUEST_METHOD"] == "POST"){
  @DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_POST, "_POST");
}
@DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, session::get_all(), "_SESSION");

/* Logged in? Simple security check */
if (!session::global_is_set('config')){
  new log("security","login","",array(),"main.php called without session - logging out") ;
  header ("Location: logout.php");
  exit;
} 

/* Check for uniqe ip address */
$ui= session::global_get('ui');
if ($_SERVER['REMOTE_ADDR'] != $ui->ip){
  new log("security","login","",array(),"main.php called with session which has a changed IP address.") ;
  header ("Location: logout.php");
  exit;
}
$config= session::global_get('config');
$config->check_and_reload();
$config->configRegistry->reload();

/* Enable compressed output */
if ($config->get_cfg_value("core","sendCompressedOutput") == "true"){

  if(ob_get_length() > 0) ob_end_clean();
  ob_start("ob_gzhandler");
}

/* Check for invalid sessions */
if(session::global_get('_LAST_PAGE_REQUEST') == ""){
  session::global_set('_LAST_PAGE_REQUEST',time());
}else{

  /* check GOsa.conf for defined session lifetime */
  $max_life= $config->get_cfg_value("core","sessionLifetime");

  /* get time difference between last page reload */
  $request_time = (time()- session::global_get('_LAST_PAGE_REQUEST'));

  /* If page wasn't reloaded for more than max_life seconds 
   * kill session
   */
  if($request_time > $max_life){
    session::destroy();
    new log("security","login","",array(),"main.php called without session - logging out") ;
    header ("Location: logout.php");
    exit;
  }
  session::global_set('_LAST_PAGE_REQUEST',time());
}


@DEBUG (DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");

/* Set template compile directory */
$smarty->compile_dir= $config->get_cfg_value("core","templateCompileDirectory");
$smarty->error_unassigned= true;

/* Set default */
$reload_navigation = false;

/* Set last initialised language to current, browser settings */
if(!session::global_is_set('Last_init_lang')){
  $reload_navigation = true;
  session::global_set('Last_init_lang',get_browser_language());
}

/* If last language != current force navi reload */
$lang= get_browser_language();
if(session::global_get('Last_init_lang') != $lang){
  $reload_navigation = true;
}

/* Language setup */
session::global_set('Last_init_lang',$lang);

/* Preset current main base */
if(!session::global_is_set('CurrentMainBase')){
  session::global_set('CurrentMainBase',get_base_from_people($ui->dn));
}

putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

// Validate LDAP schema if not done already
if( $config->boolValueIsTrue('core','schemaCheck') && 
    !$config->configRegistry->schemaCheckFinished() && 
    !$config->configRegistry->validateSchemata($force=FALSE,$disableIncompatiblePlugins=TRUE)){
    $config->configRegistry->displayRequirementErrors();
}

/* Check if the config is up to date */
$config->check_config_version();

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);
@DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");

/* Prepare plugin list */
if (!session::global_is_set('plist')){
  /* Initially load all classes */
  $class_list= get_declared_classes();
  foreach ($class_mapping as $class => $path){
    if (!in_array_strict($class, $class_list)){
      if (is_readable("$BASE_DIR/$path")){
        require_once("$BASE_DIR/$path");
      } else {
        msg_dialog::display(_("Fatal error"),
            sprintf(_("Cannot locate file %s - please run %s to fix this"),
              bold("$BASE_DIR/$path"), bold("update-gosa")), FATAL_ERROR_DIALOG);
        exit;
      }
    }
  }

  session::global_set('plist', new pluglist($config, $ui));

  /* Load ocMapping into userinfo */
  $tmp= new acl($config, NULL, $ui->dn);
  $ui->ocMapping= $tmp->ocMapping;
  session::global_set('ui',$ui);
}
$plist= session::global_get('plist');

/* Check for register globals */
if (isset($global_check) && $config->boolValueIsTrue("core","forceGlobals")){
  msg_dialog::display(
            _("PHP configuration"),
            _("Fatal error: Register globals is active. Please fix this in order to continue."),
            FATAL_ERROR_DIALOG);

  new log("security","login","",array(),"Register globals is on. For security reasons, this should be turned off.") ;
  session::destroy ();
  exit;
}

/* Check Plugin variable */
if (session::global_is_set('plugin_dir')){
  $old_plugin_dir= session::global_get('plugin_dir');
} else {
  $old_plugin_dir= "";
}

// Generate menus 
$plist->gen_headlines();
$plist->gen_menu();
$plist->genPathMenu();
$plist->genBreadcrumb();

/* check if we are using account expiration */
$smarty->assign("hideMenus", FALSE);
if ($config->boolValueIsTrue("core","handleExpiredAccounts")){
    $expired= ldap_expired_account($config, $ui->dn, $ui->username);
 
    if ($expired == POSIX_WARN_ABOUT_EXPIRATION && !session::is_set('POSIX_WARN_ABOUT_EXPIRATION__DONE')){

        // The users password is about to xpire soon, display a warning message.
        new log("security","gosa","",array(),"password for user \"$ui->username\" is about to expire") ;
        msg_dialog::display(_("Password change"), _("Your password is about to expire, please change your password!"), INFO_DIALOG);
        session::set('POSIX_WARN_ABOUT_EXPIRATION__DONE', TRUE);

    } elseif ($expired == POSIX_FORCE_PASSWORD_CHANGE){

        // The password is expired, we are now going to enforce a new one from the user.

        // Hide the GOsa menus to avoid leaving the enforced password change dialog.
        $smarty->assign("hideMenus", TRUE);
        $plug = (isset($_GET['plug'])) ? $_GET['plug'] : null;

        // Detect password plugin id:
        $passId =  array_search('password', $plist->pluginList);
        if($passId !== FALSE){
            $_GET['plug'] = $passId;
        }
    }
}

$smarty->assign("noMenuMode", count($plist->getRegisteredMenuEntries()) == 0);
if (isset($_GET['plug']) && $plist->plugin_access_allowed($_GET['plug'])){
  $plug= validate($_GET['plug']);
  $plugin_dir= $plist->get_path($plug);
  $plugin= $plist->get_class($plug);
  session::global_set('currentPlugin',$plugin);
  session::global_set('plugin_dir',$plugin_dir);
  if ($plugin_dir == ""){
    new log("security","gosa","",array(),"main.php called with invalid plug parameter \"$plug\"") ;
    header ("Location: logout.php");
    exit;
  }
} else {
    session::global_set('plugin_dir',"welcome");
    session::global_set('currentPlugin','welcome');
    $plugin_dir= "$BASE_DIR/plugins/generic/welcome";
}

// Display the welcome page for admins (iconmenu) and an info page for those 
//  who are not allowed to adminstrate anything (user)
if(count($plist->getRegisteredMenuEntries()) == 0 && session::global_get('currentPlugin') == "welcome"){
    session::global_set('plugin_dir',"infoPage");
    session::global_set('currentPlugin','welcome');
    $plugin_dir= "$BASE_DIR/plugins/generic/infoPage";
} 

/* Handle plugin locks.
    - Remove the plugin from session if we switched to another. (cleanup) 
    - Remove all created locks if "reset" was posted.
    - Remove all created locks if we switched to another plugin.
*/
$cleanup    = FALSE;
$remove_lock= FALSE;

/* Check if we have changed the selected plugin 
*/
if(($old_plugin_dir != $plugin_dir && $old_plugin_dir != "") || 
    (isset($_GET['reset']) && $_GET['reset'] == 1)){
  if (is_file("$old_plugin_dir/main.inc")){
    $cleanup = $remove_lock = TRUE;
    require ("$old_plugin_dir/main.inc");
    $cleanup = $remove_lock = FALSE;
  }
}else // elseif

/* Reset was posted, remove all created locks for the current plugin
*/
if((isset($_GET['reset']) && $_GET['reset'] == 1) || isset($_POST['delete_lock'])){
  $remove_lock = TRUE;
}

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
}


/* Load department list when plugin has changed. That is some kind of
   compromise between speed and beeing up to date */
if (isset($_GET['reset'])){
  set_object_info();
}

/* show web frontend */
$smarty->assign ("title","GOsa");
$smarty->assign ("logo", image(get_template_path("images/logo.png")));
$smarty->assign ("logoutimage", get_template_path("images/btn-logout.png"));
$smarty->assign ("date", date("l, dS F Y H:i:s O"));
$smarty->assign ("lang", preg_replace('/_.*$/', '', $lang));
$smarty->assign ("must", "<span class='required'>*</span>");
if (isset($plug)){
  $plug= "?plug=$plug";
} else {
  $plug= "";
}
if (session::global_get('js')==FALSE){
  $smarty->assign("javascript", "false");
  $smarty->assign("help_method", "href='helpviewer.php$plug' target='_blank'");
} else {
  $smarty->assign("javascript", "true");
  $smarty->assign("help_method"," onclick=\"return popup('helpviewer.php$plug','GOsa help');\"");
}


$loggedin = sprintf(_("You're logged in as %s"), 
    "<span>".$ui->cn." [".$ui->username."] / ".$config->current['NAME']."</span> &nbsp;");
if($ui->ignore_acl_for_current_user()){
    $loggedin = "<font color='red'>"._("ACLs are disabled")."</font>&nbsp;".$loggedin;
}

$smarty->assign ("loggedin", $loggedin);
$smarty->assign ("go_logo", get_template_path('images/go_logo.png'));
$smarty->assign ("go_base", get_template_path('images/dtree.png'));
$smarty->assign ("go_home", get_template_path('images/gohome.png'));
$smarty->assign ("go_out", get_template_path('images/logout.png'));
$smarty->assign ("go_top", get_template_path('images/go_top.png'));
$smarty->assign ("go_corner", get_template_path('images/go_corner.png'));
$smarty->assign ("go_left", get_template_path('images/go_left.png'));
$smarty->assign ("go_help", get_template_path('images/help.png'));

/* reload navigation if language changed*/  
if($reload_navigation){
  $plist->menu="";
}
$smarty->assign ("menu", $plist->gen_menu());
$smarty->assign ("plug", "$plug");


/* React on clicks */
if ($_SERVER["REQUEST_METHOD"] == "POST"){
  if (isset($_POST['delete_lock']) || isset($_POST['open_readonly'])){

    /* Set old Post data */
    if(session::global_is_set('LOCK_VARS_USED_GET')){
      foreach(session::global_get('LOCK_VARS_USED_GET') as $name => $value){
        $_GET[$name]  = $value;
      } 
    } 
    if(session::global_is_set('LOCK_VARS_USED_POST')){
      foreach(session::global_get('LOCK_VARS_USED_POST') as $name => $value){
        $_POST[$name] = $value;
      } 
    }
    if(session::global_is_set('LOCK_VARS_USED_REQUEST')){
      foreach(session::global_get('LOCK_VARS_USED_REQUEST') as $name => $value){
        $_REQUEST[$name] = $value;
      } 
    }
  }
}

/* Load plugin */
if (is_file("$plugin_dir/main.inc")){
  $display ="";
  require ("$plugin_dir/main.inc");
} else {
  msg_dialog::display(
      _("Plug-in"),
      sprintf(_("Fatal error: Cannot find any plugin definitions for plugin %s!"), bold($plug)),
      FATAL_ERROR_DIALOG);
  exit();
}


/* Print_out last ErrorMessage repeated string. */
$smarty->assign("msg_dialogs", msg_dialog::get_dialogs());
$smarty->assign ("pathMenu", $plist->genPathMenu());
if (check_for_materilize_theme) {
  $smarty->assign ("breadcrumb", $plist->genBreadcrumb());
}
$smarty->assign("contents", $display);
$smarty->assign("sessionLifetime", $config->get_cfg_value('core','sessionLifetime'));

/* If there's some post, take a look if everything is there... */
if (isset($_POST) && count($_POST)){
  if (!isset($_POST['php_c_check'])){
    msg_dialog::display(
            _("Configuration Error"),
            sprintf(_("Fatal error: not all POST variables have been transfered by PHP - please inform your administrator!")),
            FATAL_ERROR_DIALOG);
    exit();
  }
}

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

/* Set channel if needed */
#TODO: * move all global session calls to global_
#      * create a new channel where needed (mostly management dialogues)
#      * remove regulary created channels when not needed anymore
#      * take a look at external php calls (i.e. get fax, ldif, etc.)
#      * handle aborted sessions (by pressing anachors i.e. Main, Menu, etc.)
#      * check lock removals, is "dn" global or not in this case?
#      * last page request -> global or not?
#      * check that filters are still global
#      * maxC global?
if (isset($_POST['_channel_'])){
	echo "DEBUG - current channel: ".$_POST['_channel_'];
	$smarty->assign("channel", $_POST['_channel_']);
} else {
	$smarty->assign("channel", "");
}

$display= "<!-- headers.tpl-->".$smarty->fetch(get_template_path('headers.tpl')).
          $smarty->fetch(get_template_path('framework.tpl'));

/* Save dialog filters and selected base in a cookie. 
   So we may be able to restore the filter an base settings on reload.
*/
$cookie = array();

if(isset($_COOKIE['GOsa_Filter_Settings'])){
  $cookie = json_decode(base64_decode($_COOKIE['GOsa_Filter_Settings']), true);
}elseif(isset($HTTP_COOKIE_VARS['GOsa_Filter_Settings'])){
  $cookie = json_decode(base64_decode($HTTP_COOKIE_VARS['GOsa_Filter_Settings']), true);
}
if(!is_array($cookie)) {
  $cookie = [];
}

/* Save filters? */
if($config->get_cfg_value("core","storeFilterSettings") == "true"){
  $cookie_vars = array("MultiDialogFilters","CurrentMainBase");
  foreach($cookie_vars as $var){
    if(session::global_is_set($var)){
      $cookie[$ui->dn][$var] = session::global_get($var);
    }
  }
  if(isset($_GET['plug'])){
    $cookie[$ui->dn]['plug'] = $_GET['plug'];
  }
  @setcookie("GOsa_Filter_Settings",base64_encode(json_encode($cookie)),time() + (60*60*24));
}

/* Show page... */
echo $display;

/* Save plist and config */
session::global_set('plist',$plist);
session::global_set('config',$config);
session::set('errorsAlreadyPosted',array());

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
