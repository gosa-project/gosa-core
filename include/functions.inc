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
define ("CONFIG_FILE", "gosa.conf-trunk");
define ("CONFIG_TEMPLATE_DIR", "../contrib/");
define ("HELP_BASEDIR", "/var/www/doc/");

/* Define get_list flags */
define("GL_NONE",         0);
define("GL_SUBSEARCH",    1);
define("GL_SIZELIMIT",    2);
define("GL_CONVERT",      4);
define("GL_NO_ACL_CHECK", 8);

/* Heimdal stuff */
define('UNIVERSAL',0x00);
define('INTEGER',0x02);
define('OCTET_STRING',0x04);
define('OBJECT_IDENTIFIER ',0x06);
define('SEQUENCE',0x10);
define('SEQUENCE_OF',0x10);
define('SET',0x11);
define('SET_OF',0x11);
define('DEBUG',false);
define('HDB_KU_MKEY',0x484442);
define('TWO_BIT_SHIFTS',0x7efc);
define('DES_CBC_CRC',1);
define('DES_CBC_MD4',2);
define('DES_CBC_MD5',3);
define('DES3_CBC_MD5',5);
define('DES3_CBC_SHA1',16);

/* Define globals for revision comparing */
$svn_path = '$HeadURL$';
$svn_revision = '$Revision$';

/* Include required files */
require_once("class_location.inc");
require_once ("functions_debug.inc");
require_once ("accept-to-gettext.inc");

/* Define constants for debugging */
define ("DEBUG_TRACE",   1);
define ("DEBUG_LDAP",    2);
define ("DEBUG_MYSQL",   4);
define ("DEBUG_SHELL",   8);
define ("DEBUG_POST",   16);
define ("DEBUG_SESSION",32);
define ("DEBUG_CONFIG", 64);
define ("DEBUG_ACL",    128);

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


/* Class autoloader */
function __autoload($class_name) {
    global $class_mapping, $BASE_DIR;

    if ($class_mapping === NULL){
	    echo sprintf(_("Fatal error: no class locations defined - please run '%s' to fix this"), "<b>update-gosa</b>");
	    exit;
    }

    if (isset($class_mapping[$class_name])){
      require_once($BASE_DIR."/".$class_mapping[$class_name]);
    } else {
      echo sprintf(_("Fatal error: cannot instantiate class '%s' - try running '%s' to fix this"), $class_name, "<b>update-gosa</b>");
      exit;
    }
}


/*! \brief Checks if a class is available. 
 *  @param  name String  The class name.
 *  @return boolean      True if class is available, else false.
 */
function class_available($name)
{
  global $class_mapping;
  return(isset($class_mapping[$name]));
}


/* Check if plugin is avaliable */
function plugin_available($plugin)
{
	global $class_mapping, $BASE_DIR;

	if (!isset($class_mapping[$plugin])){
		return false;
	} else {
		return is_readable($BASE_DIR."/".$class_mapping[$plugin]);
	}
}


/* Create seed with microseconds */
function make_seed() {
  list($usec, $sec) = explode(' ', microtime());
  return (float) $sec + ((float) $usec * 100000);
}


/* Debug level action */
function DEBUG($level, $line, $function, $file, $data, $info="")
{
  if (session::get('DEBUGLEVEL') & $level){
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


function get_browser_language()
{
  /* Try to use users primary language */
  global $config;
  $ui= get_userinfo();
  if (isset($ui) && $ui !== NULL){
    if ($ui->language != ""){
      return ($ui->language.".UTF-8");
    }
  }

  /* Check for global language settings in gosa.conf */
  if(isset($config->data['MAIN']['LANG']) && !empty($config->data['MAIN']['LANG'])) {
    $lang = $config->data['MAIN']['LANG'];
    if(!preg_match("/utf/i",$lang)){
      $lang .= ".UTF-8";
    }
    return($lang);
  }
 
  /* Load supported languages */
  $gosa_languages= get_languages();

  /* Move supported languages to flat list */
  $langs= array();
  foreach($gosa_languages as $lang => $dummy){
    $langs[]= $lang.'.UTF-8';
  }

  /* Return gettext based string */
  return (al2gt($langs, 'text/html'));
}


/* Rewrite ui object to another dn */
function change_ui_dn($dn, $newdn)
{
  $ui= session::get('ui');
  if ($ui->dn == $dn){
    $ui->dn= $newdn;
    session::set('ui',$ui);
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
      $nf= preg_replace("!^".$BASE_DIR."/!", "", session::get('plugin_dir'));
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
      return (session::get('plugin_dir')."/$filename");
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


function gosa_array_merge($ar1,$ar2)
{
  if(!is_array($ar1) || !is_array($ar2)){
    trigger_error("Specified parameter(s) are not valid arrays.");
  }else{
    return(array_values(array_unique(array_merge($ar1,$ar2))));
  }
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

  $ldap = new LDAP ($binddn, $pass, $server,
      isset($config->current['RECURSIVE']) && $config->current['RECURSIVE'] == "true",
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


function process_htaccess ($username, $kerberos= FALSE)
{
  global $config;

  /* Search for $username and optional @REALM in all configured LDAP trees */
  foreach($config->data["LOCATIONS"] as $name => $data){
  
    $config->set_current($name);
    $mode= "kerberos";
    if (isset($config->current['KRBSASL']) && preg_match('/^true$/i', $config->current['KRBSASL'])){
      $mode= "sasl";
    }

    /* Look for entry or realm */
    $ldap= $config->get_ldap_link();
    if (!preg_match("/Success/i", $ldap->error)){
      msg_dialog::display(_("LDAP error"), sprintf(_('User login failed.').'<br><br>'._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
      $smarty= get_smarty();
      $smarty->display(get_template_path('headers.tpl'));
      echo "<body>".session::get('errors')."</body></html>";
      exit();
    }
    $ldap->search("(&(objectClass=gosaAccount)(|(uid=$username)(userPassword={$mode}$username)))", array("uid"));

    /* Found a uniq match? Return it... */
    if ($ldap->count() == 1) {
      $attrs= $ldap->fetch();
      return array("username" => $attrs["uid"][0], "server" => $name);
    }
  }

  /* Nothing found? Return emtpy array */
  return array("username" => "", "server" => "");
}


function ldap_login_user_htaccess ($username)
{
  global $config;

  /* Look for entry or realm */
  $ldap= $config->get_ldap_link();
  if (!preg_match("/Success/i", $ldap->error)){
    msg_dialog::display(_("LDAP error"), sprintf(_('User login failed.').'<br><br>'._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
    $smarty= get_smarty();
    $smarty->display(get_template_path('headers.tpl'));
    echo "<body>".session::get('errors')."</body></html>";
    exit();
  }
  $ldap->search("(&(objectClass=gosaAccount)(uid=$username))", array("uid"));
  /* Found no uniq match? Strange, because we did above... */
  if ($ldap->count() != 1) {
    msg_dialog::display(_("LDAP error"), _("Username / UID is not unique inside the LDAP tree. Please contact your Administrator."), ERROR_DIALOG);
    return (NULL);
  }
  $attrs= $ldap->fetch();

  /* got user dn, fill acl's */
  $ui= new userinfo($config, $ldap->getDN());
  $ui->username= $attrs['uid'][0];

  /* No password check needed - the webserver did it for us */
  $ldap->disconnect();

  /* Username is set, load subtreeACL's now */
  $ui->loadACL();

  /* TODO: check java script for htaccess authentication */
  session::set('js',true);

  return ($ui);
}


function ldap_login_user ($username, $password)
{
  global $config;

  /* look through the entire ldap */
  $ldap = $config->get_ldap_link();
  if (!preg_match("/Success/i", $ldap->error)){
    msg_dialog::display(_("LDAP error"), sprintf(_("User login failed.")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
    $smarty= get_smarty();
    $smarty->display(get_template_path('headers.tpl'));
    echo "<body>".session::get('errors')."</body></html>";
    exit();
  }
  $ldap->cd($config->current['BASE']);
  $allowed_attributes = array("uid","mail");
  $verify_attr = array();
  if(isset($config->current['LOGIN_ATTRIBUTE'])){
    $tmp = split(",",$config->current['LOGIN_ATTRIBUTE']); 
    foreach($tmp as $attr){
      if(in_array($attr,$allowed_attributes)){
        $verify_attr[] = $attr;
      }
    }
  }
  if(count($verify_attr) == 0){
    $verify_attr = array("uid");
  }
  $tmp= $verify_attr;
  $tmp[] = "uid";
  $filter = "";
  foreach($verify_attr as $attr) {
    $filter.= "(".$attr."=".$username.")";
  }
  $filter = "(&(|".$filter.")(objectClass=gosaAccount))";
  $ldap->search($filter,$tmp);

  /* get results, only a count of 1 is valid */
  switch ($ldap->count()){

    /* user not found */
    case 0:	return (NULL);

            /* valid uniq user */
    case 1: 
            break;

            /* found more than one matching id */
    default:
            msg_dialog::display(_("Internal error"), _("Username / UID is not unique inside the LDAP tree. Please contact your Administrator."), ERROR_DIALOG);
            return (NULL);
  }

  /* LDAP schema is not case sensitive. Perform additional check. */
  $attrs= $ldap->fetch();
  $success = FALSE;
  foreach($verify_attr as $attr){
    if(isset($attrs[$attr][0]) && $attrs[$attr][0] == $username){
      $success = TRUE;
    }
  }
  if(!$success){
    return(FALSE);
  }

  /* got user dn, fill acl's */
  $ui= new userinfo($config, $ldap->getDN());
  $ui->username= $attrs['uid'][0];

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
    $ldap= $config->get_ldap_link();
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

  if(is_array($object)){
    foreach($object as $obj){
      add_lock($obj,$user);
    }
    return;
  }

  /* Just a sanity check... */
  if ($object == "" || $user == ""){
    msg_dialog::display(_("Internal error"), _("Error while adding a lock. Contact the developers!"), ERROR_DIALOG);
    return;
  }

  /* Check for existing entries in lock area */
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaUser=$user)(gosaObject=".base64_encode($object)."))",
      array("gosaUser"));
  if (!preg_match("/Success/i", $ldap->error)){
    msg_dialog::display(_("Configuration error"), sprintf(_("Cannot create locking information in LDAP tree. Please contact your administrator!")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
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
      msg_dialog::display(_("Internal error"), sprintf(_("Adding a lock failed.")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
      return;
    }
  }
}


function del_lock ($object)
{
  global $config;

  if(is_array($object)){
    foreach($object as $obj){
      del_lock($obj);
    }
    return;
  }

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
      msg_dialog::display(_("LDAP error"), sprintf(_("Removing a lock failed.")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
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
    msg_dialog::display(_("Internal error"), _("Error while adding a lock. Contact the developers!"), ERROR_DIALOG);
    return("");
  }

  /* Get LDAP link, check for presence of the lock entry */
  $user= "";
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaObject=".base64_encode($object)."))", array("gosaUser"));
  if (!preg_match("/Success/i", $ldap->error)){
    msg_dialog::display(_("LDAP error"), sprintf(_("Cannot get locking information from LDAP tree!")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
    return("");
  }

  /* Check for broken locking information in LDAP */
  if ($ldap->count() > 1){

    /* Hmm. We're removing broken LDAP information here and issue a warning. */
    msg_dialog::display(_("Warning"), _("Found multiple locks for object to be locked. This should not happen - cleaning up multiple references."), WARNING_DIALOG);

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


function get_multiple_locks($objects)
{
  global $config;

  if(is_array($objects)){
    $filter = "(&(objectClass=gosaLockEntry)(|";
    foreach($objects as $obj){
      $filter.="(gosaObject=".base64_encode($obj).")";
    }
    $filter.= "))";
  }else{
    $filter = "(&(objectClass=gosaLockEntry)(gosaObject=".base64_encode($objects)."))";
  }

  /* Get LDAP link, check for presence of the lock entry */
  $user= "";
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['CONFIG']);
  $ldap->search($filter, array("gosaUser","gosaObject"));
  if (!preg_match("/Success/i", $ldap->error)){
    msg_dialog::display(_("LDAP error"), sprintf(_("Cannot get locking information from LDAP tree!")."<br><br>"._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
    return("");
  }

  $users = array();
  while($attrs = $ldap->fetch()){
    $dn   = base64_decode($attrs['gosaObject'][0]);
    $user = $attrs['gosaUser'][0];
    $users[] = array("dn"=> $dn,"user"=>$user);
  }
  return ($users);
}


/* \!brief  This function searches the ldap database.
            It search in  $sub_bases,*,$base  for all objects matching the $filter.

    @param $filter    String The ldap search filter
    @param $category  String The ACL category the result objects belongs 
    @param $sub_bases  String The sub base we want to search for e.g. "ou=apps"
    @param $base      String The ldap base from which we start the search
    @param $attributes Array The attributes we search for.
    @param $flags     Long   A set of Flags
 */
function get_sub_list($filter, $category,$sub_deps, $base= "", $attributes= array(), $flags= GL_SUBSEARCH)
{
  global $config, $ui;
  $departments = array();

#  $start = microtime(TRUE);

  /* Get LDAP link */
  $ldap= $config->get_ldap_link($flags & GL_SIZELIMIT);

  /* Set search base to configured base if $base is empty */
  if ($base == ""){
    $base = $config->current['BASE'];
  }
  $ldap->cd ($base);

  /* Ensure we have an array as department list */
  if(is_string($sub_deps)){
    $sub_deps = array($sub_deps);
  }

  /* Remove ,.*$ ("ou=1,ou=2.." => "ou=1") */
  $sub_bases = array();
  foreach($sub_deps as $key => $sub_base){
    if(empty($sub_base)){

      /* Subsearch is activated and we got an empty sub_base.
       *  (This may be the case if you have empty people/group ous).
       * Fall back to old get_list(). 
       * A log entry will be written.
       */
      if($flags & GL_SUBSEARCH){
        $sub_bases = array();
        break;
      }else{
        
        /* Do NOT search within subtrees is requeste and the sub base is empty. 
         * Append all known departments that matches the base.
         */
        $departments[$base] = $base;
      }
    }else{
      $sub_bases[$key] = preg_replace("/,.*$/","",$sub_base);
    }
  }
  
   /* If there is no sub_department specified, fall back to old method, get_list().
   */
  if(!count($sub_bases) && !count($departments)){
    
    /* Log this fall back, it may be an unpredicted behaviour.
     */
    if(!count($sub_bases) && !count($departments)){
      // log($action,$objecttype,$object,$changes_array = array(),$result = "") 
      new log("debug","all",__FILE__,$attributes,
          sprintf("get_sub_list(): Falling back to get_list(), due to empty sub_bases parameter.".
            " This may slow down GOsa. Search was: '%s'",$filter));
    }
    $tmp = get_list($filter, $category,$base,$attributes,$flags);
    return($tmp);
  }

  /* Get all deparments matching the given sub_bases */
  $base_filter= "";
  foreach($sub_bases as $sub_base){
    $base_filter .= "(".$sub_base.")";
  }
  $base_filter = "(&(objectClass=organizationalUnit)(|".$base_filter."))";
  $ldap->search($base_filter,array("dn"));
  while($attrs = $ldap->fetch()){
    foreach($sub_deps as $sub_dep){

      /* Only add those departments that match the reuested list of departments.
       *
       * e.g.   sub_deps = array("ou=servers,ou=systems,");
       *  
       * In this case we have search for "ou=servers" and we may have also fetched 
       *  departments like this "ou=servers,ou=blafasel,..."
       * Here we filter out those blafasel departments.
       */
      if(preg_match("/".normalizePreg($sub_dep)."/",$attrs['dn'])){
        $departments[$attrs['dn']] = $attrs['dn'];
        break;
      }
    }
  }

  $result= array();
  $limit_exceeded = FALSE;

  /* Search in all matching departments */
  foreach($departments as $dep){

    /* Break if the size limit is exceeded */
    if($limit_exceeded){
      return($result);
    }

    $ldap->cd($dep);

    /* Perform ONE or SUB scope searches? */
    if ($flags & GL_SUBSEARCH) {
      $ldap->search ($filter, $attributes);
    } else {
      $ldap->ls ($filter,$dep,$attributes);
    }

    /* Check for size limit exceeded messages for GUI feedback */
    if (preg_match("/size limit/i", $ldap->error)){
      session::set('limit_exceeded', TRUE);
      $limit_exceeded = TRUE;
    }

    /* Crawl through result entries and perform the migration to the
     result array */
    while($attrs = $ldap->fetch()) {
      $dn= $ldap->getDN();

      /* Convert dn into a printable format */
      if ($flags & GL_CONVERT){
        $attrs["dn"]= convert_department_dn($dn);
      } else {
        $attrs["dn"]= $dn;
      }

      /* Skip ACL checks if we are forced to skip those checks */
      if($flags & GL_NO_ACL_CHECK){
        $result[]= $attrs;
      }else{

        /* Sort in every value that fits the permissions */
        if (is_array($category)){
          foreach ($category as $o){
            if ($ui->get_category_permissions($dn, $o) != ""){
              $result[]= $attrs;
              break;
            }
          }
        } else {
          if ( $ui->get_category_permissions($dn, $category) != ""){
            $result[]= $attrs;
          }
        }
      }
    }
  }
#  if(microtime(TRUE) - $start > 0.1){
#    echo sprintf("<pre>GET_SUB_LIST  %s .| %f  --- $base -----$filter ---- $flags</pre>",__LINE__,microtime(TRUE) - $start);
#  }
  return($result);
}


function get_list($filter, $category, $base= "", $attributes= array(), $flags= GL_SUBSEARCH)
{
  global $config, $ui;

#  $start = microtime(TRUE);

  /* Get LDAP link */
  $ldap= $config->get_ldap_link($flags & GL_SIZELIMIT);

  /* Set search base to configured base if $base is empty */
  if ($base == ""){
    $ldap->cd ($config->current['BASE']);
  } else {
    $ldap->cd ($base);
  }

  /* Perform ONE or SUB scope searches? */
  if ($flags & GL_SUBSEARCH) {
    $ldap->search ($filter, $attributes);
  } else {
    $ldap->ls ($filter,$base,$attributes);
  }

  /* Check for size limit exceeded messages for GUI feedback */
  if (preg_match("/size limit/i", $ldap->error)){
    session::set('limit_exceeded', TRUE);
  }

  /* Crawl through reslut entries and perform the migration to the
     result array */
  $result= array();

  while($attrs = $ldap->fetch()) {

    $dn= $ldap->getDN();

    /* Convert dn into a printable format */
    if ($flags & GL_CONVERT){
      $attrs["dn"]= convert_department_dn($dn);
    } else {
      $attrs["dn"]= $dn;
    }

    if($flags & GL_NO_ACL_CHECK){
      $result[]= $attrs;
    }else{

      /* Sort in every value that fits the permissions */
      if (is_array($category)){
        foreach ($category as $o){
          if ($ui->get_category_permissions($dn, $o) != ""){

            /* We found what we were looking for, break speeds things up */
            $result[]= $attrs;
          }
        }
      } else {
        if ($ui->get_category_permissions($dn, $category) != ""){

          /* We found what we were looking for, break speeds things up */
          $result[]= $attrs;
        }
      }
    }
  }
 
#  if(microtime(TRUE) - $start > 0.1){
#    echo sprintf("<pre>GET_LIST %s .| %f  --- $base -----$filter ---- $flags</pre>",__LINE__,microtime(TRUE) - $start);
#  }
  return ($result);
}


function check_sizelimit()
{
  /* Ignore dialog? */
  if (session::is_set('size_ignore') && session::get('size_ignore')){
    return ("");
  }

  /* Eventually show dialog */
  if (session::is_set('limit_exceeded') && session::get('limit_exceeded')){
    $smarty= get_smarty();
    $smarty->assign('warning', sprintf(_("The size limit of %d entries is exceed!"),
          session::get('size_limit')));
    $smarty->assign('limit_message', sprintf(_("Set the new size limit to %s and show me this message if the limit still exceeds"), '<input type="text" name="new_limit" maxlength="10" size="5" value="'.(session::get('size_limit') +100).'">'));
    return($smarty->fetch(get_template_path('sizelimit.tpl')));
  }

  return ("");
}


function print_sizelimit_warning()
{
  if (session::is_set('size_limit') && session::get('size_limit') >= 10000000 ||
      (session::is_set('limit_exceeded') && session::get('limit_exceeded'))){
    $config= "<input type='submit' name='edit_sizelimit' value="._("Configure").">";
  } else {
    $config= "";
  }
  if (session::is_set('limit_exceeded') && session::get('limit_exceeded')){
    return ("("._("incomplete").") $config");
  }
  return ("");
}


function eval_sizelimit()
{
  if (isset($_POST['set_size_action'])){

    /* User wants new size limit? */
    if (tests::is_id($_POST['new_limit']) &&
        isset($_POST['action']) && $_POST['action']=="newlimit"){

      session::set('size_limit', validate($_POST['new_limit']));
      session::set('size_ignore', FALSE);
    }

    /* User wants no limits? */
    if (isset($_POST['action']) && $_POST['action']=="ignore"){
      session::set('size_limit', 0);
      session::set('size_ignore', TRUE);
    }

    /* User wants incomplete results */
    if (isset($_POST['action']) && $_POST['action']=="limited"){
      session::set('size_ignore', TRUE);
    }
  }
  getMenuCache();
  /* Allow fallback to dialog */
  if (isset($_POST['edit_sizelimit'])){
    session::set('size_ignore',FALSE);
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
      if(session::is_set('maxC')){
        $b= session::get('maxC');
        $q= "";
        for ($m=0;$m<strlen($b);$m++) {
          $q.= $b[$m++];
        }
        msg_dialog::display(_("Internal error"), base64_decode($q), ERROR_DIALOG);
      }
    }
  }
}


function &get_userinfo()
{
  global $ui;

  return $ui;
}


function &get_smarty()
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

  $map = array( 
                "ogroupou"      => "ou=groups,",
                "applicationou" => "ou=apps,",
                "systemsou"     => "ou=systems,",
                "serverou"      => "ou=servers,ou=systems,",
                "terminalou"    => "ou=terminals,ou=systems,",
                "workstationou" => "ou=workstations,ou=systems,",
                "printerou"     => "ou=printers,ou=systems,",
                "phoneou"       => "ou=phones,ou=systems,",
                "componentou"   => "ou=netdevices,ou=systems,",
                "blocklistou"   => "ou=gofax,ou=systems,",
                "incomingou"    => "ou=incoming,",
                "aclroleou"     => "ou=aclroles,",
                "macroou"       => "ou=macros,ou=asterisk,ou=configs,ou=systems,",
                "conferenceou"  => "ou=conferences,ou=asterisk,ou=configs,ou=systems,",

                "faiou"         => "ou=fai,ou=configs,ou=systems,",
                "faiscriptou"   => "ou=scripts,",
                "faihookou"     => "ou=hooks,",
                "faitemplateou" => "ou=templates,",
                "faivariableou" => "ou=variables,",
                "faiprofileou"  => "ou=profiles,",
                "faipackageou"  => "ou=packages,",
                "faipartitionou"=> "ou=disk,",

                "deviceou"      => "ou=devices,",
                "mimetypeou"    => "ou=mime,");

  /* Preset ou... */
  if (isset($config->current[$name])){
    $ou= $config->current[$name];
  } elseif (isset($map[$name])) {
    $ou = $map[$name];
    return($ou);
  } else {
    trigger_error("No department mapping found for type ".$name);
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


function strict_uid_mode()
{
  return !(isset($config->current['STRICT']) && preg_match('/^(no|false)$/i', $config->current['STRICT']));
}


function get_uid_regexp()
{
  /* STRICT adds spaces and case insenstivity to the uid check.
     This is dangerous and should not be used. */
  if (strict_uid_mode()){
    return "^[a-z0-9_-]+$";
  } else {
    return "^[a-zA-Z0-9 _.-]+$";
  }
}


function print_red()
{
  trigger_error("Use of obsolete print_red");
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

  /* If DEBUGLEVEL is set, we're in web mode, use textual output in
     the other case... */
  if($string !== NULL){
    if (preg_match("/"._("LDAP error:")."/", $string)){
      $addmsg= _("Problems with the LDAP server mean that you probably lost the last changes. Please check your LDAP setup for possible errors and try again.");
    } else {
      if (!preg_match('/[.!?]$/', $string)){
        $string.= ".";
      }
      $string= preg_replace('/<br>/', ' ', $string);
      $addmsg= _("Please check your input and fix the error. Press 'OK' to close this message box.");
      $addmsg = "";
    }
    if(empty($addmsg)){
      $addmsg = _("Error");
    }
    msg_dialog::display($addmsg, $string,ERROR_DIALOG);
    return;
  }else{
    return;
  }

}


function gen_locked_message($user, $dn)
{
  global $plug, $config;

  session::set('dn', $dn);
  $remove= false;

  /* Save variables from LOCK_VARS_TO_USE in session - for further editing */
  if( session::is_set('LOCK_VARS_TO_USE') && count(session::get('LOCK_VARS_TO_USE'))){

    $LOCK_VARS_USED   = array();
    $LOCK_VARS_TO_USE = session::get('LOCK_VARS_TO_USE');

    foreach($LOCK_VARS_TO_USE as $name){

      if(empty($name)){
        continue;
      }

      foreach($_POST as $Pname => $Pvalue){
        if(preg_match($name,$Pname)){
          $LOCK_VARS_USED[$Pname] = $_POST[$Pname];
        }
      }

      foreach($_GET as $Pname => $Pvalue){
        if(preg_match($name,$Pname)){
          $LOCK_VARS_USED[$Pname] = $_GET[$Pname];
        }
      }
    }
    session::set('LOCK_VARS_TO_USE',array());
    session::set('LOCK_VARS_USED'  , $LOCK_VARS_USED);
  }

  /* Prepare and show template */
  $smarty= get_smarty();
  
  if(is_array($dn)){
    $msg = "<pre>";
    foreach($dn as $sub_dn){
      $msg .= "\n".$sub_dn.", ";
    }
    $msg = preg_replace("/, $/","</pre>",$msg);
  }else{
    $msg = $dn;
  }

  $smarty->assign ("dn", $msg);
  if ($remove){
    $smarty->assign ("action", _("Continue anyway"));
  } else {
    $smarty->assign ("action", _("Edit anyway"));
  }
  $smarty->assign ("message", sprintf(_("You're going to edit the LDAP entry/entries '%s'"), "<b>".$msg."</b>", ""));

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


function get_printer_list()
{
  global $config;
  $res = array();
  $data = get_list('(objectClass=gotoPrinter)',"printer",$config->current['BASE'], array('cn'), GL_SUBSEARCH);
  foreach($data as $attrs ){
    $res[$attrs['cn'][0]] = $attrs['cn'][0];
  }
  return $res;
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
  msg_dialog::display(_("Error"), $complete, ERROR_DIALOG);
}


function show_ldap_error($message, $addon= "")
{
  if (!preg_match("/Success/i", $message)){
    if ($addon == ""){
      msg_dialog::display(_("LDAP error:"), $message, ERROR_DIALOG);
    } else {
      if(!preg_match("/No such object/i",$message)){
        msg_dialog::display(_("LDAP error"), sprintf(_("Plugin '%s':%s"),"<i>".$addon."</i>", "<br><br>$message"),ERROR_DIALOG);
      }
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
  return ($display);
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
      $stripped.= "{".$pos."}";
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
    if (strcasecmp($item, $value) == 0) {
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
              msg_dialog::display(_("Internal error"), sprintf(_("File '%s' could not be deleted."), $directory."/".$file), ERROR_DIALOG);
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
    msg_dialog::display(_("Internal error"), _("Cannot write to revision file!"), ERROR_DIALOG);
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
      msg_dialog::display(_("Internal error"), _("Cannot write to revision file!"), ERROR_DIALOG);
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


function get_base_dir()
{
  global $BASE_DIR;

  return $BASE_DIR;
}


function obj_is_readable($dn, $object, $attribute)
{
  global $ui;

  return preg_match('/r/', $ui->get_permissions($dn, $object, $attribute));
}


function obj_is_writable($dn, $object, $attribute)
{
  global $ui;

  return preg_match('/w/', $ui->get_permissions($dn, $object, $attribute));
}


function gosa_ldap_explode_dn($dn,$config = NULL,$verify_in_ldap=false)
{
  /* Initialize variables */
  $ret  = array("count" => 0);  // Set count to 0
  $next = true;                 // if false, then skip next loops and return
  $cnt  = 0;                    // Current number of loops
  $max  = 100;                  // Just for security, prevent looops
  $ldap = NULL;                 // To check if created result a valid
  $keep = "";                   // save last failed parse string

  /* Check each parsed dn in ldap ? */
  if($config!==NULL && $verify_in_ldap){
    $ldap = $config->get_ldap_link();
  }

  /* Lets start */
  $called = false;
  while(preg_match("/,/",$dn) && $next &&  $cnt < $max){

    $cnt ++;
    if(!preg_match("/,/",$dn)){
      $next = false;
    }
    $object = preg_replace("/[,].*$/","",$dn);
    $dn     = preg_replace("/^[^,]+,/","",$dn);

    $called = true;

    /* Check if current dn is valid */
    if($ldap!==NULL){
      $ldap->cd($dn);
      $ldap->cat($dn,array("dn"));
      if($ldap->count()){
        $ret[]  = $keep.$object;
        $keep   = "";
      }else{
        $keep  .= $object.",";
      }
    }else{
      $ret[]  = $keep.$object;
      $keep   = "";
    }
  }

  /* No dn was posted */
  if($cnt == 0 && !empty($dn)){
    $ret[] = $dn;
  }

  /* Append the rest */
  $test = $keep.$dn;
  if($called && !empty($test)){
    $ret[] = $keep.$dn;
  }
  $ret['count'] = count($ret) - 1;

  return($ret);
}


function get_base_from_hook($dn, $attrib)
{
  global $config;

  if (isset($config->current['BASE_HOOK'])){
    
    /* Call hook script - if present */
    $command= $config->current['BASE_HOOK'];

    if ($command != ""){
      $command.= " '".LDAP::fix($dn)."' $attrib";
      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");
        exec($command, $output);
        if (preg_match("/^[0-9]+$/", $output[0])){
          return ($output[0]);
        } else {
          msg_dialog::display(_("Warning"), _("'base_hook' is not available. Using default base."), WARNING_DIALOG);
          return ($config->current['UIDBASE']);
        }
      } else {
        msg_dialog::display(_("Warning"), _("'base_hook' is not available. Using default base."), WARNING_DIALOG);
        return ($config->current['UIDBASE']);
      }

    } else {

      msg_dialog::display(_("Warning"), _("'base_hook' is not available. Using default base."), WARNING_DIALOG);
      return ($config->current['UIDBASE']);

    }
  }
}


function check_schema_version($class, $version)
{
  return preg_match("/\(v$version\)/", $class['DESC']);
}


function check_schema($cfg,$rfc2307bis = FALSE)
{
  $messages= array();

  /* Get objectclasses */
  $ldap = new LDAP($cfg['admin'],$cfg['password'],$cfg['connection'] ,FALSE,$cfg['tls']);
  $objectclasses = $ldap->get_objectclasses();
  if(count($objectclasses) == 0){
    msg_dialog::display(_("LDAP warning"), _("Cannot get schema information from server. No schema check possible!"), WARNING_DIALOG);
  }

  /* This is the default block used for each entry.
   *  to avoid unset indexes.
   */
  $def_check = array("REQUIRED_VERSION" => "0",
      "SCHEMA_FILES"     => array(),
      "CLASSES_REQUIRED" => array(),
      "STATUS"           => FALSE,
      "IS_MUST_HAVE"     => FALSE,
      "MSG"              => "",
      "INFO"             => "");#_("There is currently no information specified for this schema extension."));

  /* The gosa base schema */
  $checks['gosaObject'] = $def_check;
  $checks['gosaObject']['REQUIRED_VERSION'] = "2.4";
  $checks['gosaObject']['SCHEMA_FILES']     = array("gosa+samba3.schema","gosa.schema");
  $checks['gosaObject']['CLASSES_REQUIRED'] = array("gosaObject");
  $checks['gosaObject']['IS_MUST_HAVE']     = TRUE;

  /* GOsa Account class */
  $checks["gosaAccount"]["REQUIRED_VERSION"]= "2.4";
  $checks["gosaAccount"]["SCHEMA_FILES"]    = array("gosa+samba3.schema","gosa.schema");
  $checks["gosaAccount"]["CLASSES_REQUIRED"]= array("gosaAccount");
  $checks["gosaAccount"]["IS_MUST_HAVE"]    = TRUE;
  $checks["gosaAccount"]["INFO"]            = _("Used to store account specific informations.");

  /* GOsa lock entry, used to mark currently edited objects as 'in use' */
  $checks["gosaLockEntry"]["REQUIRED_VERSION"] = "2.4";
  $checks["gosaLockEntry"]["SCHEMA_FILES"]     = array("gosa+samba3.schema","gosa.schema");
  $checks["gosaLockEntry"]["CLASSES_REQUIRED"] = array("gosaLockEntry");
  $checks["gosaLockEntry"]["IS_MUST_HAVE"]     = TRUE;
  $checks["gosaLockEntry"]["INFO"]             = _("Used to lock currently edited entries to avoid multiple changes at the same time.");

  /* Some other checks */
  foreach(array(
        "gosaCacheEntry"        => array("version" => "2.4"),
        "gosaDepartment"        => array("version" => "2.4"),
        "goFaxAccount"          => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        "goFaxSBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        "goFaxRBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        "gosaUserTemplate"      => array("version" => "2.4", "class" => "posixAccount","file" => "nis.schema"),
        "gosaMailAccount"       => array("version" => "2.4", "class" => "mailAccount","file" => "gosa+samba3.schema"),
        "gosaProxyAccount"      => array("version" => "2.4", "class" => "proxyAccount","file" => "gosa+samba3.schema"),
        "gosaApplication"       => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),
        "gosaApplicationGroup"  => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),
        "GOhard"                => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
        "gotoTerminal"          => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
        "goServer"              => array("version" => "2.4","class" => "server","file" => "goserver.schema"),
        "goTerminalServer"      => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goShareServer"         => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goNtpServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goSyslogServer"        => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
        "goLdapServer"          => array("version" => "2.4"),
        "goCupsServer"          => array("version" => "2.4", "class" => array("posixAccount", "terminals"),),
        "goImapServer"          => array("version" => "2.4", "class" => array("mailAccount", "mailgroup"),"file" => "gosa+samba3.        schema"),
        "goKrbServer"           => array("version" => "2.4"),
        "goFaxServer"           => array("version" => "2.4", "class" => "gofaxAccount","file" => "gofax.schema"),
        ) as $name => $values){

          $checks[$name] = $def_check;
          if(isset($values['version'])){
            $checks[$name]["REQUIRED_VERSION"] = $values['version'];
          }
          if(isset($values['file'])){
            $checks[$name]["SCHEMA_FILES"] = array($values['file']);
          }
          $checks[$name]["CLASSES_REQUIRED"] = array($name);
        }
  foreach($checks as $name => $value){
    foreach($value['CLASSES_REQUIRED'] as $class){

      if(!isset($objectclasses[$name])){
        $checks[$name]['STATUS'] = FALSE;
        if($value['IS_MUST_HAVE']){
          $checks[$name]['MSG']    = sprintf(_("Missing required object class '%s'!"),$class);
        }else{
          $checks[$name]['MSG']    = sprintf(_("Missing optional object class '%s'!"),$class);
        }
      }elseif(!check_schema_version($objectclasses[$name],$value['REQUIRED_VERSION'])){
        $checks[$name]['STATUS'] = FALSE;

        if($value['IS_MUST_HAVE']){
          $checks[$name]['MSG'] = sprintf(_("Version mismatch for required object class '%s' (!=%s)!"), $class,                           $value['REQUIRED_VERSION']);
        }else{
          $checks[$name]['MSG'] = sprintf(_("Version mismatch for optional object class '%s' (!=%s)!"), $class,                           $value['REQUIRED_VERSION']);
        }
      }else{
        $checks[$name]['STATUS'] = TRUE;
        $checks[$name]['MSG'] = sprintf(_("Class(es) available"));
      }
    }
  }

  $tmp = $objectclasses;

  /* The gosa base schema */
  $checks['posixGroup'] = $def_check;
  $checks['posixGroup']['REQUIRED_VERSION'] = "2.4";
  $checks['posixGroup']['SCHEMA_FILES']     = array("gosa+samba3.schema","gosa.schema");
  $checks['posixGroup']['CLASSES_REQUIRED'] = array("posixGroup");
  $checks['posixGroup']['STATUS']           = TRUE;
  $checks['posixGroup']['IS_MUST_HAVE']     = TRUE;
  $checks['posixGroup']['MSG']              = "";
  $checks['posixGroup']['INFO']             = "";

  /* Depending on selected rfc2307bis mode, we need different schema configurations */
  if(isset($tmp['posixGroup'])){

    if($rfc2307bis && isset($tmp['posixGroup']['STRUCTURAL'])){
      $checks['posixGroup']['STATUS']           = FALSE;
      $checks['posixGroup']['MSG']              = _("You have enabled the rfc2307bis option on the 'ldap setup' step, but your schema    configuration do not support this option.");
      $checks['posixGroup']['INFO']             = _("In order to use rfc2307bis conform groups the objectClass 'posixGroup' must be      AUXILIARY");
    }
    if(!$rfc2307bis && !isset($tmp['posixGroup']['STRUCTURAL'])){
      $checks['posixGroup']['STATUS']           = FALSE;
      $checks['posixGroup']['MSG']              = _("Your schema is configured to support the rfc2307bis group, but you have disabled this option on the 'ldap setup' step.");
      $checks['posixGroup']['INFO']             = _("The objectClass 'posixGroup' must be STRUCTURAL");
    }
  }

  return($checks);
}


function get_languages($languages_in_own_language = FALSE,$strip_region_tag = FALSE)
{
  $tmp = array(
        "de_DE" => "German",
        "fr_FR" => "French",
        "it_IT" => "Italian",
        "es_ES" => "Spanish",
        "en_US" => "English",
        "nl_NL" => "Dutch",
        "pl_PL" => "Polish",
        "sv_SE" => "Swedish",
        "zh_CN" => "Chinese",
        "ru_RU" => "Russian");
  
  $tmp2= array(
        "de_DE" => _("German"),
        "fr_FR" => _("French"),
        "it_IT" => _("Italian"),
        "es_ES" => _("Spanish"),
        "en_US" => _("English"),
        "nl_NL" => _("Dutch"),
        "pl_PL" => _("Polish"),
        "sv_SE" => _("Swedish"),
        "zh_CN" => _("Chinese"),
        "ru_RU" => _("Russian"));

  $ret = array();
  if($languages_in_own_language){

    $old_lang = setlocale(LC_ALL, 0);
    foreach($tmp as $key => $name){
      $lang = $key.".UTF-8";
      setlocale(LC_ALL, $lang);
      if($strip_region_tag){
        $ret[preg_replace("/^([^_]*).*$/","\\1",$key)] = _($name)." (".$tmp2[$key].")";
      }else{
        $ret[$key] = _($name)." &nbsp;(".$tmp2[$key].")";
      }
    }
    setlocale(LC_ALL, $old_lang);
  }else{
    foreach($tmp as $key => $name){
      if($strip_region_tag){
        $ret[preg_replace("/^([^_]*).*/","\\1",$key)] = _($name);
      }else{
        $ret[$key] = _($name);
      }
    }
  }
  return($ret);
}


/* Returns contents of the given POST variable and check magic quotes settings */
function get_post($name)
{
  if(!isset($_POST[$name])){
    trigger_error("Requested POST value (".$name.") does not exists, you should add a check to prevent this message.");
    return(FALSE);
  }
  if(get_magic_quotes_gpc()){
    return(stripcslashes($_POST[$name]));
  }else{
    return($_POST[$name]);
  }
}


/* Return class name in correct case */
function get_correct_class_name($cls)
{
  global $class_mapping;
  if(isset($class_mapping) && is_array($class_mapping)){
    foreach($class_mapping as $class => $file){
      if(preg_match("/^".$cls."$/i",$class)){
        return($class);
      }
    }
  }
  return(FALSE);
}


// change_password, changes the Password, of the given dn
function change_password ($dn, $password, $mode=0, $hash= "")
{
  global $config;
  $newpass= "";

  /* Convert to lower. Methods are lowercase */
  $hash= strtolower($hash);

  // Get all available encryption Methods

  // NON STATIC CALL :)
  $tmp = new passwordMethod(session::get('config'));
  $available = $tmp->get_available_methods();

  // read current password entry for $dn, to detect the encryption Method
  $ldap       = $config->get_ldap_link();
  $ldap->cat ($dn, array("shadowLastChange", "userPassword", "uid"));
  $attrs      = $ldap->fetch ();

  // Check if user account was deactivated, indicated by ! after } ... {crypt}!###
  if(isset($attrs['userPassword'][0]) && preg_match("/^[^\}]*+\}!/",$attrs['userPassword'][0])){
    $deactivated = TRUE;
  }else{
    $deactivated = FALSE;
  }

  /* Is ensure that clear passwords will stay clear */
  if($hash == "" && isset($attrs['userPassword'][0]) && !preg_match ("/^{([^}]+)}(.+)/", $attrs['userPassword'][0])){
    $hash = "clear";
  }

  // Detect the encryption Method
  if ( (isset($attrs['userPassword'][0]) &&  preg_match ("/^{([^}]+)}(.+)/", $attrs['userPassword'][0], $matches)) ||  $hash != ""){

    /* Check for supported algorithm */
    mt_srand((double) microtime()*1000000);

    /* Extract used hash */
    if ($hash == ""){
      $hash= strtolower($matches[1]);
    }

    $test = new  $available[$hash]($config);

  } else {
    // User MD5 by default
    $hash= "md5";
    $test = new  $available['md5']($config);
  }

  /* Feed password backends with information */
  $test->dn= $dn;
  $test->attrs= $attrs;
  $newpass= $test->generate_hash($password);

  // Update shadow timestamp?
  if (isset($attrs["shadowLastChange"][0])){
    $shadow= (int)(date("U") / 86400);
  } else {
    $shadow= 0;
  }

  // Write back modified entry
  $ldap->cd($dn);
  $attrs= array();

  // Not for groups
  if ($mode == 0){

    if ($shadow != 0){
      $attrs['shadowLastChange']= $shadow;
    }

    // Create SMB Password
    $attrs= generate_smb_nt_hash($password);
  }

 /* Readd ! if user was deactivated */
  if($deactivated){
    $newpass = preg_replace("/(^[^\}]+\})(.*$)/","\\1!\\2",$newpass);
  }

  $attrs['userPassword']= array();
  $attrs['userPassword']= $newpass;

  $ldap->modify($attrs);

  new log("modify","users/passwordMethod",$dn,array_keys($attrs),$ldap->get_error());

  if ($ldap->error != 'Success') {
    msg_dialog::display(_("LDAP error"), sprintf(_('Setting the password failed!').'<br><br>'._('LDAP server returned: %s'), "<br><br><i>".$ldap->get_error()."</i>"), ERROR_DIALOG);
  } else {

    /* Run backend method for change/create */
    $test->set_password($password);

    /* Find postmodify entries for this class */
    $command= $config->search("password", "POSTMODIFY",array('menu'));

    if ($command != ""){
      /* Walk through attribute list */
      $command= preg_replace("/%userPassword/", $password, $command);
      $command= preg_replace("/%dn/", $dn, $command);

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");
        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as POSTMODIFY for plugin '%s' doesn't seem to exist."), $command, "password");
        msg_dialog::display(_("Configuration error"), $message, ERROR_DIALOG);
      }
    }
  }
}


// Return something like array['sambaLMPassword']= "lalla..."
function generate_smb_nt_hash($password)
{
  global $config;

  # Try to use gosa-si?
  if (isset($config->current['GOSA_SI'])){
	$res= gosaSupportDaemon::send("gosa_gen_smb_hash", "GOSA", array("password" => $password), TRUE);
	$hash= $res['XML']['HASH'];
  } else {
	  $tmp= $config->data['MAIN']['SMBHASH']." ".escapeshellarg($password);
	  @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $tmp, "Execute");

	  exec($tmp, $ar);
	  flush();
	  reset($ar);
	  $hash= current($ar);
  }

  if ($hash == "") {
	  msg_dialog::display(_("Configuration error"), _("Cannot generate samba hash!"), ERROR_DIALOG);
	  return ("");
  }

  list($lm,$nt)= split (":", trim($hash));

  if ($config->current['SAMBAVERSION'] == 3) {
	  $attrs['sambaLMPassword']= $lm;
	  $attrs['sambaNTPassword']= $nt;
	  $attrs['sambaPwdLastSet']= date('U');
	  $attrs['sambaBadPasswordCount']= "0";
	  $attrs['sambaBadPasswordTime']= "0";
  } else {
	  $attrs['lmPassword']= $lm;
	  $attrs['ntPassword']= $nt;
	  $attrs['pwdLastSet']= date('U');
  }
  return($attrs);
}


function crypt_single($string,$enc_type )
{
  return( passwordMethod::crypt_single_str($string,$enc_type));
}


function getEntryCSN($dn)
{
  global $config;
  if(empty($dn) || !is_object($config)){
    return("");
  }

  /* Get attribute that we should use as serial number */
  if(isset($config->current['UNIQ_IDENTIFIER'])){
    $attr = $config->current['UNIQ_IDENTIFIER'];
  }elseif(isset($config->data['MAIN']['UNIQ_IDENTIFIER'])){
    $attr = $config->data['MAIN']['UNIQ_IDENTIFIER'];
  }
  if(!empty($attr)){
    $ldap = $config->get_ldap_link();
    $ldap->cat($dn,array($attr));
    $csn = $ldap->fetch();
    if(isset($csn[$attr][0])){
      return($csn[$attr][0]);
    }
  }
  return("");
}


/* Add a given objectClass to an attrs entry */
function add_objectClass($classes, &$attrs)
{
  if (is_array($classes)){
    $list= $classes;
  } else {
    $list= array($classes);
  }

  foreach ($list as $class){
    $attrs['objectClass'][]= $class;
  }
}


/* Removes a given objectClass from the attrs entry */
function remove_objectClass($classes, &$attrs)
{
  if (isset($attrs['objectClass'])){
    /* Array? */
    if (is_array($classes)){
      $list= $classes;
    } else {
      $list= array($classes);
    }

    $tmp= array();
    foreach ($attrs['objectClass'] as $oc) {
      foreach ($list as $class){
        if ($oc != $class){
          $tmp[]= $oc;
        }
      }
    }
    $attrs['objectClass']= $tmp;
  }
}

/*! \brief  Initialize a file download with given content, name and data type. 
 *  @param  data  String The content to send.
 *  @param  name  String The name of the file.
 *  @param  type  String The content identifier, default value is "application/octet-stream";
 */
function send_binary_content($data,$name,$type = "application/octet-stream")
{
  header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
  header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
  header("Cache-Control: no-cache");
  header("Pragma: no-cache");
  header("Cache-Control: post-check=0, pre-check=0");
  header("Content-type: ".$type."");

  /* force download dialog */
  if (preg_match('/MSIE 5.5/', $HTTP_USER_AGENT) || preg_match('/MSIE 6.0/', $HTTP_USER_AGENT)) {
    header('Content-Disposition: filename="'.$name.'"');
  } else {
    header('Content-Disposition: attachment; filename="'.$name.'"');
  }

  echo $data;
  exit();
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
