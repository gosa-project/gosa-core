<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2005  Cajus Pollmeier

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

/* Load required includes */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
header("Content-type: text/html; charset=UTF-8");

/* Reset error handler */
$error_collector= "";
set_error_handler('gosaRaiseError');

/* Set error handler to own one, initialize time calculation
   and start session. */
session_start ();
$username= "";

/* Check if we need to run setup */
if (!file_exists(CONFIG_DIR."/gosa.conf")){
  header("location:setup.php");
  exit();
}

/* Reset errors */
$_SESSION['errors']= "";

/* Check for java script */
if (!isset($_GET['js']) && !isset($_SESSION['js'])){
  echo '<script language="JavaScript" type="text/javascript">';
  echo '  location = "index.php?js=true";';
  echo '</script>';

  $_SESSION['js']= FALSE;
} else {
  $_SESSION['js']= TRUE;
}

/* Check if gosa.conf is accessable */
if (!is_readable(CONFIG_DIR."/gosa.conf")){
  print_red(sprintf(_("GOsa configuration %s/gosa.conf is not readable. Aborted."), CONFIG_DIR));
  echo $_SESSION['errors'];
  exit();
}

/* Parse configuration file */
$config= new config(CONFIG_DIR."/gosa.conf", $BASE_DIR);
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
$smarty->assign ('nextfield', 'username');

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  print_red(sprintf(_("Directory '%s' specified as compile directory is not accessable!"),
        $smarty->compile_dir));
  echo $_SESSION['errors'];
  exit();
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


if ($_SERVER["REQUEST_METHOD"] != "POST"){
  @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");
}


/* Check for SSL connection */
$ssl= "";
if (!isset($HTTP_SERVER_VARS['HTTPS']) ||
    !stristr($HTTP_SERVER_VARS['HTTPS'], "on")) {

  if (empty($HTTP_SERVER_VARS['REQUEST_URI'])) {
    $ssl= "https://".$HTTP_SERVER_VARS['HTTP_HOST'].
      $HTTP_SERVER_VARS['PATH_INFO'];
  } else {
    $ssl= "https://".$HTTP_SERVER_VARS['HTTP_HOST'].
      $HTTP_SERVER_VARS['REQUEST_URI'];
  }
}

/* If SSL is forced, just forward to the SSL enabled site */
if ($config->data['MAIN']['FORCESSL'] == 'true'){
  header ("Location: $ssl");
  exit;
}

/* Got a formular answer, validate and try to log in */
if ($_SERVER["REQUEST_METHOD"] == "POST"){

  /* Reset error messages */
  $message= "";

  $server= validate($_POST["server"]);
  $config->set_current($server);

  /* Admin-logon and verify */
  $ldap = $config->get_ldap_link();
  if (is_null($ldap) || (is_int($ldap) && $ldap == 0)){
    print_red (_("Can't bind to LDAP. Please contact the system administrator."));
    echo $_SESSION['errors'];
    exit();
  }

  /* Check for locking area */
  $ldap->cat($config->current['CONFIG']);
  $attrs= $ldap->fetch();
  if (!count ($attrs)){
    $ldap->cd($config->current['BASE']);
    $ldap->create_missing_trees($config->current['CONFIG']);
  }

  /* Check for at least one subtreeACL in the complete tree */
  $ldap->cd($config->current['BASE']);
  $ldap->search("(&(objectClass=gosaObject)(gosaSubtreeACL=:all))");
  if ($ldap->count() < 1){
#FIXME: -> ldapsetup.tpl would be better
    print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
    echo $_SESSION['errors'];
    exit;
  }

  /* Check for valid input */
  $username= $_POST["username"];
  if (!ereg("^[A-Za-z0-9_.-]+$", $username)){
    $message= _("Please specify a valid username!");
  } elseif (mb_strlen($_POST["password"], 'UTF-8') == 0){
    $message= _("Please specify your password!");
    $smarty->assign ('nextfield', 'password');
  } else {

    /* Login as user, initialize user ACL's */
    $ui= ldap_login_user($username, $_POST["password"]);
    if ($ui === NULL || $ui == 0){
      $message= _("Please check the username/password combination.");
      $smarty->assign ('nextfield', 'password');
      gosa_log ("Authentication failed for user \"$username\"");
    } else {
      /* Remove all locks of this user */
      del_user_locks($ui->dn);

      /* Save userinfo and plugin structure */
      $_SESSION['ui']= $ui;
      $_SESSION['session_cnt']= 0;

      /* Let GOsa trigger a new connection for each POST, save
         config to session. */
      $_SESSION['config']= $config;

      /* Go to main page */
      gosa_log ("User \"$username\" logged in successfully");
      header ("Location: main.php?global_check=1");
      exit;
    }
  }
}

/* Fill template with required values */
$smarty->assign ('date', gmdate("D, d M Y H:i:s"));
$smarty->assign ('username', $username);
$smarty->assign ('personal_img', get_template_path('images/personal.png'));
$smarty->assign ('password_img', get_template_path('images/password.png'));
$smarty->assign ('directory_img', get_template_path('images/ldapserver.png'));

/* Some error to display? */
if (!isset($message)){
  $message= "";
}
$smarty->assign ("message", $message);

/* Displasy SSL mode warning? */
if ($ssl != "" && $config->data['MAIN']['WARNSSL'] == 'true'){
  $smarty->assign ("ssl", "<b>"._("Warning").":</b> "._("Session will not be encrypted.")." <a style=\"color:red;\" href=\"$ssl\"><b>"._("Enter SSL session")."</b></a>!");
} else {
  $smarty->assign ("ssl", "");
}

/* Generate server list */
$servers= array();
if (isset($_POST['server'])){
  $selected= validate($_POST['server']);
} else {
  $selected= $config->data['MAIN']['DEFAULT'];
}
foreach ($config->data['LOCATIONS'] as $key => $ignored){
  $servers[$key]= $key;
}
$smarty->assign ("server_options", $servers);
$smarty->assign ("server_id", $selected);

/* show login screen */
$smarty->display (get_template_path('headers.tpl'));
$smarty->assign ("PHPSESSID", session_id());
if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}
$smarty->assign("php_error", $error_collector);
$smarty->display (get_template_path('login.tpl'));

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>

</body>
</html>
