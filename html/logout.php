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

/* Basic setup, remove eventually registered sessions */
require_once("../include/php_setup.inc");
require_once("functions.inc");
header("Content-type: text/html; charset=UTF-8");

/* try to start session, so we can remove userlocks, 
  if the old session is still available */
session::start();
session::set('errorsAlreadyPosted', array());
if (session::global_is_set('ui')) {

  /* Get config & ui informations */
  $ui = session::global_get("ui");

  /* config used for del_user_locks & some lines below to detect the language */
  $config = session::global_get("config");

  /* Remove all locks of this user */
  del_user_locks($ui->dn);

  /* Write something to log */
  new log("security", "logout", "", array(), "User \"" . $ui->username . "\" logged out");
}

/* Language setup */
if ((!isset($config)) || $config->get_cfg_value("core", "language") == "") {
  $lang = get_browser_language();
} else {
  $lang = $config->get_cfg_value("core", "language");
}

// Try to keep track of logouts, this will fail if our session has already expired. 
// Nothing will be logged if config isn't present anymore.
stats::log('global', 'global', array(),  $action = 'logout', $amount = 1, 0);

putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language'] = $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR . '/locale/';

/* Set the text domain as 'core' */
$domain = 'core';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);

$smarty->assign("title", "GOsa");

/* If GET request is posted, the logout was forced by pressing the link */
if (isset($_POST['forcedlogout']) || isset($_GET['forcedlogout'])) {

  /* destroy old session */
  session::destroy();

  /* If we're not using htaccess authentication, just redirect... */
  if (isset($config) && $config->get_cfg_value("core", "htaccessAuthentication") == "true") {

    /* Else notice that the user has to close the browser... */
    $smarty->display(get_template_path('headers.tpl'));
    $smarty->display(get_template_path('logout-close.tpl'));
    exit;
  }

  header("Location: index.php");
  exit();
} else {  // The logout wasn't forced, so the session is invalid 


  $smarty->display(get_template_path('headers.tpl'));
  $smarty->display(get_template_path('logout.tpl'));
  exit;
}
?>

</html>