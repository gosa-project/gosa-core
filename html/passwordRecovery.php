<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
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

//include requirements for pdf generation
require_once("fpdf/fpdf.php");
require_once("../include/pdf/class_FormularPDF.inc");
require_once("../include/pdf/class_PasswortAendernPDF.inc");

/* Load required includes */
require_once ("../include/php_setup.inc");
require_once ("functions.inc");

/* if the Securimage class is unknown include it */
if(!class_exists("Securimage")){
  require_once ("securimage/securimage.php");
}

/* if the log class is unknwon include it */
if(!class_exists("log")){
  require_once("class_log.inc");
}

//start session if it wasn't already done
session_start();

/*
 * This function will display the given template
 */
function displayPWrecovery($pdf_uid, $pdf_password)
{
  global $smarty;
  global $pwChanged;

  $smarty->display(get_template_path('passwordRecovery.tpl'));

  if($pwChanged){
    $pdf= new PasswortAendernPDF($pdf_uid, $pdf_password);
    $pdf->output("PasswortAendern.pdf", "D");
  }

  exit();
}

// Generate a random character string
function rand_str($length = 128, $chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890')
{
  // Length of character list
  $chars_length = (strlen($chars) - 1);

  // Start our string
  $string = $chars{rand(0, $chars_length)};

  // Generate random string
  for ($i = 1; $i < $length; $i = strlen($string))
  {
    // Grab a random character from our list
    $r = $chars{rand(0, $chars_length)};

    // Make sure the same two characters don't appear next to each other
    if ($r != $string{$i - 1}) $string .=  $r;
  }

  // Return the string
  return $string;
}

//create the mail to send
function sendResetMail($receiver_address, $recoveryId){
  global $smarty;
  global $config;

  //verify parameters
  if($receiver_address == "" || $recoveryId == ""){
    return false;
  }

  //create messages
  $mailMessage = "Folgen Sie dem angegebenen Link um Ihr Password zurueckzusetzen ";
  $link = $_SERVER['SERVER_NAME'] . $_SERVER['REQUEST_URI'];
  $link = $link. "?pwToken=".$recoveryId;

  $mailMessage = $mailMessage . "\n\n" . "http://" . $link;
  $mailMessage = $mailMessage . "\n\n" . "Der Link ist " . $config->get_cfg_value("timeToLivePasswordToken", 30) . " Tage lange gueltig";

  //send the mail
  mail($receiver_address, "Password reset", $mailMessage); 
  //assign flag to smarty so we can display another page
  $smarty->assign("mailSend", true);
  return true;
}

header("Content-type: text/html; charset=UTF-8");

/* Reset errors */
session::global_set('js',true);
session::set('errors',"");
session::set('errorsAlreadyPosted',array());
session::set('LastError',"");

/* Check if CONFIG_FILE is accessible */
if (!is_readable(CONFIG_DIR."/".CONFIG_FILE)){
  msg_dialog::display(_("Fatal error"), sprintf(_("GOsa configuration %s/%s is not readable. Aborted."),CONFIG_DIR,CONFIG_FILE), FATAL_ERROR_DIALOG);
  exit;
}

/* Parse configuration file */
$config= new config(CONFIG_DIR."/".CONFIG_FILE, $BASE_DIR);
session::global_set('DEBUGLEVEL', $config->get_cfg_value("debuglevel"));
if ($_SERVER["REQUEST_METHOD"] != "POST"){
  @DEBUG (DEBUG_CONFIG, __LINE__, __FUNCTION__, __FILE__, $config->data, "config");
}

/* Set template compile directory */
$smarty->compile_dir= $config->get_cfg_value("templateCompileDirectory", '/var/spool/gosa');

/* Check for compile directory */
if (!(is_dir($smarty->compile_dir) && is_writable($smarty->compile_dir))){
  msg_dialog::display(_("Configuration error"), sprintf(_("Directory '%s' specified as compile directory is not accessible!"),
        $smarty->compile_dir), FATAL_ERROR_DIALOG);
  exit;
}

/* Check for old files in compile directory */
clean_smarty_compile_dir($smarty->compile_dir);

/* Language setup */
if ($config->get_cfg_value("language") == ""){
  $lang= get_browser_language();
} else {
  $lang= $config->get_cfg_value("language");
}

$lang.=".UTF-8";
putenv("LANGUAGE=");
putenv("LANG=$lang");
setlocale(LC_ALL, $lang);
$GLOBALS['t_language']= $lang;
$GLOBALS['t_gettext_message_dir'] = $BASE_DIR.'/locale/';

/* Set the text domain as 'messages' */
$domain = 'messages';
bindtextdomain($domain, LOCALE_DIR);
textdomain($domain);

/* Generate server list */
$servers= array();
foreach ($config->data['LOCATIONS'] as $key => $ignored){
	$servers[$key]= $key;
}

if (isset($_POST['server'])){
	$directory= validate($_POST['server']);
} else {
	$directory= $config->data['MAIN']['DEFAULT'];

  if(!isset($servers[$directory])){
    $directory = key($servers);
  }
}

if (isset($_GET['directory']) && isset($servers[$_GET['directory']])){
	$smarty->assign ("show_directory_chooser", false);
	$directory= validate($_GET['directory']);
} else {
	$smarty->assign ("server_options", $servers);
	$smarty->assign ("server_id", $directory);
	$smarty->assign ("show_directory_chooser", true);
}

/* Set config to selected one */
$config->set_current($directory);
session::global_set('config',$config);
$pwChanged = false;

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

$smarty->assign('tokenAccepted', false);

/* If SSL is forced, just forward to the SSL enabled site */
if ($config->get_cfg_value("forcessl") == 'true' && $ssl != ''){
  header ("Location: $ssl");
  exit;
}

$method= $config->get_cfg_value("hash", "crypt/md5");
$message= array();
$smarty->assign("mailSend", false);
$uid = null;
$pw_pdf = null;

/* Got a formular answer, validate and try to log in */
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['apply'])){
  $securimage = new Securimage();
  $ldap = $config->get_ldap_link();
  $attrs = array();

  /* Destroy old sessions, they cause a successfull login to relog again ...*/
  if(session::global_is_set('_LAST_PAGE_REQUEST')){
    session::global_set('_LAST_PAGE_REQUEST',time());
  }

  $uid= $_POST['uid'];
  if ($_POST['uid'] == ""){
    $message[]= msgPool::required(_("uid"));
  }

  //if we have the pw token and the new password we can set it 
  if(isset($_POST['pwToken']))
  {
    if(!isset($_POST['newPassword']) || !isset($_POST['newPasswordConfirm']) ||
       $_POST['newPassword'] == "" || $_POST['newPasswordConfirm'] == ""){
      $message[] = msgPool::required(_("new password and its confirmation"));
    } else {
      //first check that the new pw and its confirmation are equal
      if($_POST['newPassword'] == $_POST['newPasswordConfirm']){
        //search for the token in the ldap
        $ldap->search("(lostPasswordToken=".$_POST['pwToken'].")",array('dn', 'uid', 'lostPasswordTokenCreationDate'));
        //there has to be exact one match
        if($ldap->count() == 1){
          $attrs = $ldap->fetch();

          //check the given uid and the found ui are the same
          if($attrs['uid'][0] == $uid){
            //if the passwords have some restrictions check them....
            $pwAcceptable = true;
            if ($config->get_cfg_value("passwordMinLength") != ""){
              if (strlen($_POST['newPassword']) < $config->get_cfg_value("passwordMinLength")){
                $message[]= _("The password used as new is to short.");
                $pwAcceptable = false;
              }
            }

            //....and finally set the password when everything was fine

            $result;
            if($pwAcceptable){
              
              /* create the global userinfo object
               * the current dn is used because the person
               * who is about to change the password 
               * is hopefully the owner of this account
               * the changedBy - attribute will contain the user itself
               */
              global $ui;
              $ui= new userinfo($config, $attrs['dn']);

              //do the job change the password now or never 
              $result = uniBonnPassword::change_uni_password($attrs['dn'], $_POST['newPassword']);

              if(!$result){
                $message[] = _("Password change was not successful, token remains active, pleas try again later or consult the administrator");
              } else {

                //the default expiring value is 30 days
                //if a value is given in gosa.conf use it
                $expiringDate = $attrs['lostPasswordTokenCreationDate'][0] + ($config->get_cfg_value("timeToLivePasswordToken", 30) * 24 * 60 * 60);
                //if we exceeded the expiring date quit
                if($expiringDate < time()){
                  $message[] = _("Das Token zum zurücksetzen Ihres Passworts hat seine Gueltigkeit verloren! Fordern Sie ein neues an oder wenden Sie sich an den Administrator");
                } else {
                  $pw_pdf = $_POST['newPassword'];
                  $ldap->cd($attrs['dn']);
                  $ldap->rm(array('lostPasswordToken' => array()), $attrs['dn']);
                  $ldap->rm(array('lostPasswordTokenCreationDate' => array()), $attrs['dn']);
                  //after we deleted the token everything is done
                  //like we do when a new id is set to active we want to generate
                  //a new pdf with all the data the user needs
                  $pwChanged = true;
                  $smarty->assign("pw_changed", true);
                  //                $pdf= new PasswortAendernPDF($uid, $_POST['newPassword']);
                  //                $pdf->output("PasswortAendern.pdf", "D");

                }
              }
            }
          } else {
            $message[] = _("uni-id / password token mismatch");
          }
        } else {
          $message[] = _("multiple use of the token");
        }
      } else {
        $message[] = _("The password and its confirmation are not equal");
      }
    }
  } else {
    if (!isset($_POST['captcha_code']) || $_POST['captcha_code'] == ""){
      $message[]= msgPool::required(_("captcha_code"));
    }else{
      if ($securimage->check($_POST['captcha_code']) == false) {
        $message[]= "Die Sicherheitsabfrage ist falsch!";
      } else {
        //Search the ldap object by the given uid
        $ldap->search("(uid=".$uid.")",array('dn', 'uid', 'mailForward', 'mailForwardVerified', 'accountState'));

        //there must be extact one account
        if($ldap->count() == 1){
          $attrs = $ldap->fetch();

          //only proceed when the found accout
          //has a verified private mail address
          //and the account state is active
          if(isset($attrs['mailForward']) && 
            $attrs['mailForward'][0] != '' /* &&
            isset($attrs['mailForwardVerified'])  &&
            $attrs['mailForwardVerified'] == true*/){

              if(isset($attrs['accountState']) && $attrs['accountState'][0] == 'active'){
                do{
                  $newRecoveryId = rand_str();
                  $ldap->search("lostPasswordToken=".$newRecoveryId."", array('dn'));
                }while($ldap->count() != 0);

                $ldap->cd($attrs['dn']);
                $ldap->modify(array('lostPasswordToken' => $newRecoveryId ));
                $ldap->modify(array('lostPasswordTokenCreationDate' => time()));
                sendResetMail($attrs['mailForward'][0], $newRecoveryId);
              } else {
                $message[] = _("The given uni-id isn't in an active state please consult the administrator");
              }
            } else {
              $message[] = _("No validated private email was found for the given uni-id. Please consult the system administrator");
            }
        } else {
          $message[] = _("No account with the given uni-id / To many accounts with the given uni-id");
        }
      }
    }
  }
}

/* If the given password token is valid and in use go to the 
 * password change site
 */
if(isset($_GET['pwToken']) && $_GET['pwToken'] != ""){
  //get ldap link
  $ldap = $config->get_ldap_link();
  //search for ldap objects using the given token
  $ldap->search("(lostPasswordToken=".$_GET['pwToken'].")",array('dn', 'uid'));

  /* only proceed if there is exactly one object */
  if($ldap->count() == 1){
    //assign the given token to the smarty template 
    //so we get the token on the next post
    $smarty->assign('tokenAccepted', $_GET['pwToken']);
  } else {
    $message[] = _("The given token is used by more or less than one account please consult the administrator");
  }
}

/* Do we need to show error messages? */
if (count ($message) != 0){
 /* Show error message and continue editing */
 msg_dialog::displayChecks($message);
}

/* Fill template with required values */
$smarty->assign ('password_img', get_template_path('images/password.png'));

/* Displasy SSL mode warning? */
if ($ssl != "" && $config->get_cfg_value("warnssl") == 'true'){
  $smarty->assign ("ssl", "<b>"._("Warning").":</b> "._("Session will not be encrypted.")." <a style=\"color:red;\" href=\"".htmlentities($ssl)."\"><b>"._("Enter SSL session")."</b></a>!");
} else {
  $smarty->assign ("ssl", "");
}

/* show login screen */
if (session::is_set('errors')){
  $smarty->assign("errors", session::get('errors'));;
}
if ($error_collector != ""){
  $smarty->assign("php_errors", $error_collector."</div>");
} else {
  $smarty->assign("php_errors", "");
}

$smarty->assign("msg_dialogs", msg_dialog::get_dialogs());
displayPWrecovery($uid, $pw_pdf);

?>

</body>
</html>
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
