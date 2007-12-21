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
require_once ("../include/php_setup.inc");
require_once ("functions.inc");
header("Content-type: text/html; charset=UTF-8");

/* try to start session, so we can remove userlocks, 
  if the old session is still available */
@session_start();
if(isset($_SESSION['ui'])){
  
  /* Get config & ui informations */
  $ui= $_SESSION["ui"];
  
  /* config used for del_user_locks & some lines below to detect the language */  
  $config= $_SESSION["config"];

  /* Remove all locks of this user */
  del_user_locks($ui->dn);
  
  /* Write something to log */  
  new log("security","logout","",array(),"User \"".$ui->username."\" logged out") ;
}

/* Language setup */
if ((!isset($config))||(empty($config->data['MAIN']['LANG']))){
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

/* Create smarty & Set template compile directory */
$smarty= new smarty();
if (isset ($config->data['MAIN']['COMPILE'])){
  $smarty->compile_dir= $config->data['MAIN']['COMPILE'];
} else {
  $smarty->compile_dir= '/var/spool/gosa/';
}

    
/* If GET request is posted, the logout was forced by pressing the link */
if (isset($_GET['request'])){
  
  /* destroy old session */
  @session_unset ();
  @session_destroy ();
  
  /* If we're not using htaccess authentication, just redirect... */
  if (!isset($config->data['MAIN']['HTACCESS_AUTH']) && !isset($_SERVER['REMOTE_USER'])){
    header ("Location: index.php");
    exit();
  }

  /* Else notice that the user has to close the browser... */
  $smarty->display (get_template_path('headers.tpl'));
  $smarty->display (get_template_path('logout-close.tpl'));
  exit;

}else{  // The logout wasn't forced, so the session is invalid 
  
  $smarty->display (get_template_path('headers.tpl'));
  $smarty->display (get_template_path('logout.tpl'));
  exit;
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
</html>
