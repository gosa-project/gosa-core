<?php
/**
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

/* Load required includes */
require_once "../include/php_setup.inc";
require_once "functions.inc";
require_once "class_log.inc";
header("Content-type: text/html; charset=UTF-8");


/**
 * Display the login page and exit().
 *
 */
function displayLogin()
{
    global $smarty,$message,$config,$ssl,$error_collector, $BASE_DIR;
    error_reporting(E_ALL | E_STRICT);

    /* Check theme compatibility */
    $theme = $config->get_cfg_value("core", 'theme');
    if (file_exists("$BASE_DIR/ihtml/themes/$theme/blacklist")) {
        $blocks= file("$BASE_DIR/ihtml/themes/$theme/blacklist");
        foreach ($blocks as $block) {
            if (preg_match('/'.preg_quote($block).'/', $_SERVER['HTTP_USER_AGENT'])) {
                die(sprintf(_("Your browser (%s) is blacklisted for the current theme!"), $block));
            }
        }
    }

    /* Fill template with required values */
    $username = "";
    if (isset($_POST["username"])) {
        $username= get_post("username");
    }
    
    $smarty->assign("title", "GOsa");
    $smarty->assign("logo", get_template_path("images/logo.png"));
    $smarty->assign('date', gmdate("D, d M Y H:i:s"));
    $smarty->assign('username', set_post($username));
    $smarty->assign('personal_img', get_template_path('images/login-head.png'));
    $smarty->assign('password_img', get_template_path('images/password.png'));
    $smarty->assign('directory_img', get_template_path('images/ldapserver.png'));

    /* Some error to display? */
    if (!isset($message)) {
        $message= "";
    }
    $smarty->assign("message", $message);

    /* Displasy SSL mode warning? */
    if ($ssl != "" && $config->get_cfg_value("core", 'warnSSL') == 'true') {
        $smarty->assign("ssl", sprintf(_("This session is not encrypted. Click %s to enter an encrypted session."), "<a href=\"$ssl\">&nbsp;<b>".(_("here"))."</b>&nbsp;</a>"));
    } else {
        $smarty->assign("ssl", "");
    }

    if (!$config->check_session_lifetime()) {
        $smarty->assign("lifetime", _("The configured session lifetime will be overridden by php.ini settings!"));
    } else {
        $smarty->assign("lifetime", "");
    }

    /* Generate server list */
    $servers= array();
    if (isset($_POST['server'])) {
        $selected= get_post('server');
    } else {
        $selected= $config->data['MAIN']['DEFAULT'];
    }
    foreach ($config->data['LOCATIONS'] as $key => $ignored) {
        $servers[$key]= $key;
    }
    $smarty->assign("server_options", $servers);
    $smarty->assign("server_id", $selected);

    /* show login screen */
    $smarty->assign("PHPSESSID", session_id());
    if (session::is_set('errors')) {
        $smarty->assign("errors", session::get('errors'));
    }
    if ($error_collector != "") {
        $smarty->assign("php_errors", $error_collector."</div>");
    } else {
        $smarty->assign("php_errors", "");
    }
    $smarty->assign("msg_dialogs", msg_dialog::get_dialogs());

    $smarty->display(get_template_path('headers.tpl'));
    $smarty->assign("version", get_gosa_version());
    $smarty->display(get_template_path('login.tpl'));
    exit();
}



/*****************************************************************************
 *                               M   A   I   N                               *
 *****************************************************************************/

/* Set error handler to own one, initialize time calculation
and start session. */
session::start();
session::set('errorsAlreadyPosted', array());

/* Destroy old session if exists.
Else you will get your old session back, if you not logged out correctly. */
if (is_array(session::get_all()) && count(session::get_all())) {
    session::destroy();
    session::start();
}

$username= "";

/* Reset errors */
session::set('errors', "");
session::set('errorsAlreadyPosted', "");
session::set('LastError', "");

/* Check if we need to run setup */
if (!file_exists(CONFIG_DIR."/".CONFIG_FILE)) {
    header("location:setup.php");
    exit();
}

/* Reset errors */
session::set('errors', "");

/* Check for java script */
if (isset($_POST['javascript']) && $_POST['javascript'] == "true") {
    session::global_set('js', true);
} elseif (isset($_POST['javascript'])) {
    session::global_set('js', false);
}

/* Check if gosa.conf (.CONFIG_FILE) is accessible */
if (!is_readable(CONFIG_DIR."/".CONFIG_FILE)) {
    msg_dialog::display(_("Configuration error"), sprintf(_("GOsa configuration %s/%s is not readable. Aborted."), CONFIG_DIR, CONFIG_FILE), FATAL_ERROR_DIALOG);
    exit();
}

/* Parse configuration file */
$config= new config(CONFIG_DIR."/".CONFIG_FILE, $BASE_DIR);
session::global_set('debugLevel', $config->get_cfg_value("core", 'debugLevel'));
if ($_SERVER["REQUEST_METHOD"] != "POST") {
    @DEBUG(DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");
}

/* Enable compressed output */
if ($config->get_cfg_value("core", "sendCompressedOutput") != "") {
    if (ob_get_length() > 0) {
        ob_end_clean();
    }
    ob_start("ob_gzhandler");
}

/* Set template compile directory */
$smarty->compile_dir= $config->get_cfg_value("core", "templateCompileDirectory");
$smarty->error_unassigned= true;

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))) {
    msg_dialog::display(_("Smarty error"), sprintf(
        _("Compile directory %s is not accessible!"),
        $smarty->compile_dir
    ), FATAL_ERROR_DIALOG);
    exit();
}

/* Check for old files in compile directory */
clean_smarty_compile_dir($smarty->compile_dir);

/* Language setup */
$lang= get_browser_language();
putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);
$smarty->assign('nextfield', 'username');

/* Translation of cookie-warning. Whether to display it, is determined by JavaScript */
$smarty->assign("cookies", _("Your browser has cookies disabled: please enable cookies and reload this page before logging in!"));

if ($_SERVER["REQUEST_METHOD"] != "POST") {
    @DEBUG(DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__, $lang, "Setting language to");
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
if ($config->get_cfg_value("core", "forceSSL") == 'true' && $ssl != '') {
    header("Location: $ssl");
    exit;
}

/* Do we have htaccess authentification enabled? */
$htaccess_authenticated= false;
if ($config->get_cfg_value("core", "htaccessAuthentication") == "true") {
    if (!isset($_SERVER['REMOTE_USER'])) {
        msg_dialog::display(_("Configuration error"), _("Broken HTTP authentication setup!"), FATAL_ERROR_DIALOG);
        exit;
    }

    $tmp= process_htaccess($_SERVER['REMOTE_USER'], isset($_SERVER['KRB5CCNAME']));
    $username= $tmp['username'];
    $server= $tmp['server'];
    if ($username == "") {
        msg_dialog::display(_("Error"), _("Cannot find a valid user for the current HTTP authentication!"), FATAL_ERROR_DIALOG);
        exit;
    }
    if ($server == "") {
        msg_dialog::display(_("Error"), _("Cannot find a unique user for the current HTTP authentication!"), FATAL_ERROR_DIALOG);
        exit;
    }

    $htaccess_authenticated= true;
}

/* Got a formular answer, validate and try to log in */
if (($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['login'])) || $htaccess_authenticated) {

    /* Reset error messages */
    $message= "";

    /* Destroy old sessions, they cause a successfull login to relog again ...*/
    if (session::global_is_set('_LAST_PAGE_REQUEST')) {
        session::global_set('_LAST_PAGE_REQUEST', time());
    }

    if (!$htaccess_authenticated) {
        $server= get_post("server");
    }
    $config->set_current($server);

    /* Admin-logon and verify */
    $ldap = $config->get_ldap_link();
    if (is_null($ldap) || (is_int($ldap) && $ldap == 0)) {
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        displayLogin();
        exit();
    }

    /* Check for locking area */
    $ldap->cat($config->get_cfg_value("core", "config"), array("dn"));
    $attrs= $ldap->fetch();
    if (!count($attrs)) {
        $ldap->cd($config->current['BASE']);
        $ldap->create_missing_trees($config->get_cfg_value("core", "config"));
    }

    /* Check for valid input */
    $ok= true;
    if (!$htaccess_authenticated) {
        $username= get_post("username");
        if (!preg_match("/^[@A-Za-z0-9_.-]+$/", $username)) {
            $message= _("Please specify a valid user name!");
            $ok= false;
        } elseif (mb_strlen(get_post("password"), 'UTF-8') == 0) {
            $message= _("Please specify your password!");
            $smarty->assign('nextfield', 'password');
            $ok= false;
        }
    }

    if ($ok) {

        /* Login as user, initialize user ACL's */
        if ($htaccess_authenticated) {
            $ui= ldap_login_user_htaccess($username);
            if ($ui === null || !$ui) {
                msg_dialog::display(_("Authentication error"), _("Cannot retrieve user information for HTTP authentication!"), FATAL_ERROR_DIALOG);
                exit;
            }
        } else {
            $ui= ldap_login_user($username, get_post("password"));
        }
        if ($ui === null || !$ui) {
            $message= _("Please check the username/password combination!");
            $smarty->assign('nextfield', 'password');
            session::global_set('config', $config);
            if (isset($_SERVER['REMOTE_ADDR'])) {
                $ip= $_SERVER['REMOTE_ADDR'];
                new log("security", "login", "", array(), "Authentication failed for user \"$username\" [from $ip]");
            } else {
                new log("security", "login", "", array(), "Authentication failed for user \"$username\"");
            }
        } else {
            /* Remove all locks of this user */
            del_user_locks($ui->dn);

            /* Save userinfo and plugin structure */
            session::global_set('ui', $ui);
            session::global_set('session_cnt', 0);

            /* Let GOsa trigger a new connection for each POST, save
            config to session. */
            $config->get_departments();
            $config->make_idepartments();
            session::global_set('config', $config);

            /* Restore filter settings from cookie, if available */
            if ($config->get_cfg_value("core", "storeFilterSettings") == "true") {
                if (isset($_COOKIE['GOsa_Filter_Settings']) || isset($HTTP_COOKIE_VARS['GOsa_Filter_Settings'])) {
                    if (isset($_COOKIE['GOsa_Filter_Settings'])) {
                        $cookie_all = json_decode(base64_decode($_COOKIE['GOsa_Filter_Settings']), true);
                    } else {
                        $cookie_all = json_decode(base64_decode($HTTP_COOKIE_VARS['GOsa_Filter_Settings']), true);
                    }
                    if (!is_array($cookie_all)) {
                        $cookie_all = [];
                    }
                    if (isset($cookie_all[$ui->dn])) {
                        $cookie = $cookie_all[$ui->dn];
                        $cookie_vars= array("MultiDialogFilters","CurrentMainBase","plug");
                        foreach ($cookie_vars as $var) {
                            if (isset($cookie[$var])) {
                                session::global_set($var, $cookie[$var]);
                            }
                        }
                        if (isset($cookie['plug'])) {
                            $plug =$cookie['plug'];
                        }
                    }
                }
            }

            /* are we using accountexpiration */
            if ($config->boolValueIsTrue("core", "handleExpiredAccounts")) {
                $expired= ldap_expired_account($config, $ui->dn, $ui->username);

                if ($expired == POSIX_ACCOUNT_EXPIRED) {
                    $message= _("Account locked. Please contact your system administrator!");
                    $smarty->assign('nextfield', 'password');
                    new log("security", "login", "", array(), "Account for user \"$username\" has expired") ;
                    displayLogin();
                    exit();
                }
            }

            /* Not account expired or password forced change go to main page */
            new log("security", "login", "", array(), "User \"$username\" logged in successfully") ;
            $plist= new pluglist($config, $ui);

            stats::log('global', 'global', array(), $action = 'login', $amount = 1, 0);

            if (isset($plug) && isset($plist->dirlist[$plug])) {
                header("Location: main.php?plug=".$plug."&amp;global_check=1");
            } else {
                header("Location: main.php?global_check=1");
            }
            exit;
        }
    }
}

/* Fill template with required values */
$smarty->assign('date', gmdate("D, d M Y H:i:s"));
$smarty->assign('username', set_post($username));
$smarty->assign('personal_img', get_template_path('images/login-head.png'));
$smarty->assign('password_img', get_template_path('images/password.png'));
$smarty->assign('directory_img', get_template_path('images/ldapserver.png'));

/* Some error to display? */
if (!isset($message)) {
    $message= "";
}

$smarty->assign("message", $message);

/* Generate server list */
$servers= array();
if (isset($_POST['server'])) {
    $selected= get_post('server');
} else {
    $selected= $config->data['MAIN']['DEFAULT'];
}
foreach ($config->data['LOCATIONS'] as $key => $ignored) {
    $servers[$key]= $key;
}
$smarty->assign("server_options", $servers);
$smarty->assign("server_id", $selected);

/* show login screen */
$smarty->assign("PHPSESSID", session_id());
if (session::is_set('errors')) {
    $smarty->assign("errors", session::get('errors'));
}
if ($error_collector != "") {
    $smarty->assign("php_errors", preg_replace("/%BUGBODY%/", $error_collector_mailto, $error_collector)."</div>");
} else {
    $smarty->assign("php_errors", "");
}

/* Set focus to the error button if we've an error message */
$focus= "";
if (session::is_set('errors') && session::get('errors') != "") {
    $focus= '<script language="JavaScript" type="text/javascript">';
    $focus.= 'document.forms[0].error_accept.focus();';
    $focus.= '</script>';
}
$smarty->assign("focus", $focus);
displayLogin();

// vim:tabstop=2:expandtab:shiftwidth=2:softtabstop=2:filetype=php:syntax:ruler:
?>

</body>
</html>
