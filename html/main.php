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

/* Remember everything we did after the last click */
session_start ();
if ($_SERVER["REQUEST_METHOD"] == "POST"){
  @DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_POST, "_POST");
}
@DEBUG (DEBUG_POST, __LINE__, __FUNCTION__, __FILE__, $_SESSION, "_SESSION");

/* Logged in? Simple security check */
if (!isset($_SESSION['config'])){
  gosa_log ("main.php called without session");
  header ("Location: index.php");
  exit;
} 

/* Reset errors */
$_SESSION['errors']= "";

/* Check for uniqe ip address */
$ui= $_SESSION["ui"];
if ($_SERVER['REMOTE_ADDR'] != $ui->ip){
  gosa_log ("main.php called with session which has a changed IP address.", 3);
  header ("Location: logout.php");
  exit;
}
$config= $_SESSION['config'];
@DEBUG (DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");

/* Set template compile directory */
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

/* Prepare plugin list */
if (!isset($_SESSION['plist'])){
  $_SESSION['plist']= new pluglist($config, $ui);
}
$plist= $_SESSION['plist'];

/* Check for register globals */
if (isset($global_check) && $config->data['MAIN']['FORCEGLOBALS'] == 'true'){
  print_red (_("Register globals is on. GOsa will refuse to login unless this is fixed by an administrator."));
  echo $_SESSION['errors'];
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
if (isset($_GET['plug'])){
  $plug= validate($_GET['plug']);
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
  $config->departments= get_departments();
  $config->make_idepartments ();
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
  $smarty->assign("help_method", "href='helpviewer.php$plug' target='_new'");
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
  print_red(sprintf(_("Can't find any plugin definitions for plugin '%s'!"), $plug));
  echo $_SESSION['errors'];
  exit();
}

/* Close div/tables */
$smarty->assign("contents", $display);
if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}
if ($error_collector != ""){
  $smarty->assign("php_errors", $error_collector."</div>");
} else {
  $smarty->assign("php_errors", "");
}
$display= $header.$smarty->fetch(get_template_path('framework.tpl'));

/* For development, perform a W3C conformance check if specified in gosa.conf */
if (isset($config->data['MAIN']['W3CTEST']) && preg_match('/true/i', $config->data['MAIN']['W3CTEST'])) {

  /* Use PHP tidy for debugging */
  $tidy = new tidy();
  $config = array('indent' => TRUE,
               'output-xhtml' => TRUE,
               'wrap' => 200);
  $display = tidy_parse_string($display, $config, 'UTF8');
  tidy_clean_repair($display);
  $cnt =  (tidy_error_count($display))+(tidy_warning_count($display));
  if($cnt != 0){
    echo "<table summary=\"\" width=\"100%\" style='background-color:#E0E0E0;border-bottom:1px solid black'><tr><td><img alt=\"W3C\" align=\"middle\" src='images/warning.png'>&nbsp;<font style='font-size:14px;font-weight:bold'>"._("Generating this page caused the W3C conformance checker to raise some errors!")."</font></td><td align=right><button onClick='toggle(\"w3cbox\")'>"._("Toggle information")."</button></td></tr></table><div id='w3cbox' style='width:100%; position:absolute; z-index:0; visibility: hidden; background-color:white; border-bottom:1px solid black;'>";

    echo nl2br(htmlentities($display->errorBuffer))."</div>";
  }
  tidy_clean_repair($display);
}

/* Show page... */
echo $display;

/* Save plist */
$_SESSION['plist']= $plist;

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
