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

function displayLogin()
{
  global $smarty,$message,$config,$ssl,$error_collector;
  error_reporting(E_ALL);
    /* Fill template with required values */
    $username = "";
    if(isset($_POST["username"])){
      $username= $_POST["username"];
    }

    /* Place ie workaround if needed */
    if (isset($config->data['MAIN']['IE_PNG_WORKAROUND']) && preg_match("/true/i",$config->data['MAIN']['IE_PNG_WORKAROUND'])){
      $smarty->assign('ieworkaround', 1);
    }

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
    $smarty->assign ("PHPSESSID", session_id());
    if (isset($_SESSION['errors'])){
      $smarty->assign("errors", $_SESSION['errors']);
    }
    if ($error_collector != ""){
      $smarty->assign("php_errors", preg_replace("/%BUGBODY%/",$error_collector_mailto,$error_collector)."</div>");
    } else {
      $smarty->assign("php_errors", "");
    }

    $smarty->display(get_template_path('headers.tpl'));
    $smarty->display(get_template_path('login.tpl'));
    exit();
}

session_start();

/* Destroy old session if exists.
    Else you will get your old session back, if you not logged out correctly. */
if(is_array($_SESSION) && count($_SESSION)){
  session_destroy();
  session_start();
}

$username= "";

/* Reset errors */
$_SESSION['errors']             = "";
$_SESSION['errorsAlreadyPosted']= array();
$_SESSION['LastError']          = "";

/* Check if we need to run setup */
if (!file_exists(CONFIG_DIR."/".CONFIG_FILE)){
  header("location:setup.php");
  exit();
}

/* Check for java script */
if(isset($_POST['javascript']) && $_POST['javascript'] == "true") {
  $_SESSION['js']= TRUE;
}elseif(isset($_POST['javascript'])) {
  $_SESSION['js']= FALSE;
}

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
$smarty->assign ('nextfield', 'username');

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  echo sprintf(_("Directory '%s' specified as compile directory is not accessible!"),
        $smarty->compile_dir);
  exit();
}

/* Check for old files in compile directory */
clean_smarty_compile_dir($smarty->compile_dir);

$lang= get_browser_language();
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

  $server= validate($_POST["server"]);
  $config->set_current($server);

  /* Admin-logon and verify */
  $ldap = $config->get_ldap_link();
  if (is_null($ldap) || (is_int($ldap) && $ldap == 0)){
    print_red (_("Can't bind to LDAP. Please contact the system administrator."));
    displayLogin();
    exit();
  }

  /* Check for schema file presence */
  if(!isset($config->data['MAIN']['SCHEMA_CHECK'])){
    $config->data['MAIN']['SCHEMA_CHECK'] = "true";
  }
  if(isset($config->data['MAIN']['SCHEMA_CHECK'])&&preg_match("/true/i",$config->data['MAIN']['SCHEMA_CHECK'])){
    $recursive = (isset($config->current['RECURSIVE']) && $config->current['RECURSIVE'] == "true");
    $tls =       (isset($config->current['TLS'])       && $config->current['TLS'] == "true");

    if(!count($ldap->get_objectclasses())){

      print_red(_("GOsa cannot retrieve information about the installed schema files. Please make sure, that this is possible."));
      displayLogin();
      exit();
    }else{
 
      $cfg = array(); 
      $cfg['admin']     = $config->current['ADMIN'];
      $cfg['password']  = $config->current['PASSWORD'];
      $cfg['connection']= $config->current['SERVER'];
      $cfg['tls']       = $tls;
  
      $str = check_schema($cfg,isset($config->current['RFC2307BIS']) && preg_match("/(true|yes|on|1)/i",$config->current['RFC2307BIS']));

      $checkarr = array();
      foreach($str as $tr){
        if(isset($tr['IS_MUST_HAVE']) && !$tr['STATUS']){
          print_red($tr['MSG']."<br>"._("Your ldap setup contains old schema definitions. Please re-run the setup."));
          displayLogin();
          exit();
        }
      }
    }
  }
  /* Check for locking area */
  $ldap->cat($config->current['CONFIG'], array("dn"));
  $attrs= $ldap->fetch();
  if (!count ($attrs)){
    $ldap->cd($config->current['BASE']);
    $ldap->create_missing_trees($config->current['CONFIG']);
  }

  /* Check for at least one subtreeACL in the complete tree */
  $ldap->cd($config->current['BASE']);
  $ldap->search("(&(objectClass=gosaObject)(gosaSubtreeACL=:all))");
  if ($ldap->count() < 1){
    print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
    displayLogin();
    exit();
  }

  /* Check for valid input */
  $username= $_POST["username"];
  if (!ereg("^[@A-Za-z0-9_.-]+$", $username)){
    $message= _("Please specify a valid username!");
  } elseif (mb_strlen($_POST["password"], 'UTF-8') == 0){
    $message= _("Please specify your password!");
    $smarty->assign ('nextfield', 'password');
  } else {

    /* Login as user, initialize user ACL's */
    $ui= ldap_login_user($username, $_POST["password"]);
    if ($ui === NULL || !$ui ){
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
      $config->get_departments();
      $config->make_idepartments();
      $_SESSION['config']= $config;

      /* Take care about zend.ze1_compatiblity_mode */
      if (ini_get("zend.ze1_compatibility_mode") != 0){
        $_SESSION['PHP4COMPATIBLE']= TRUE;
      }

      /* are we using accountexpiration */
      if((isset($config->data['MAIN']['ACCOUNT_EXPIRATION'])) && 
          preg_match('/true/i', $config->data['MAIN']['ACCOUNT_EXPIRATION'])){
      
        $expired= ldap_expired_account($config, $ui->dn, $ui->username);

        if ($expired == 1){
          $message= _("Account locked. Please contact your system administrator.");
          $smarty->assign ('nextfield', 'password');
          gosa_log ("Account for user \"$username\" has expired");
        } elseif ($expired == 3){
            $plist= new pluglist($config, $ui);
            foreach ($plist->dirlist as $key => $value){
              if (preg_match("/\bpassword\b/i",$value)){
                $plug=$key;
                gosa_log ("User \"$username\" password forced to change");
                header ("Location: main.php?plug=$plug&reset=1");
                exit;
              }
            }
          }

        /* Not account expired or password forced change go to main page */
        gosa_log ("User \"$username\" logged in successfully");
        header ("Location: main.php?global_check=1");
        exit;
        
      } else {
        /* Go to main page */
        gosa_log ("User \"$username\" logged in successfully");
        header ("Location: main.php?global_check=1");
        exit;
      }
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
  $smarty->assign ("ssl", "<b>"._("Warning").":<\/b> "._("Session will not be encrypted.")." <a style=\"color:red;\" href=\"$ssl\"><b>"._("Enter SSL session")."<\/b></a>!");
} else {
  $smarty->assign ("ssl", "");
}

/* Translation of cookie-warning. Whether to display it, is determined by JavaScript */
$smarty->assign ("cookies", "<b>"._("Warning").":<\/b> "._("Your browser has cookies disabled. Please enable cookies and reload this page before logging in!"));


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
$smarty->assign ("PHPSESSID", session_id());
if (isset($_SESSION['errors'])){
  $smarty->assign("errors", $_SESSION['errors']);
}
if ($error_collector != ""){
  $smarty->assign("php_errors", $error_collector."</div>");
} else {
  $smarty->assign("php_errors", "");
}

displayLogin();

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:

?>

</body>
</html>
