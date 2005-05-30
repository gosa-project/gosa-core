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

/* Include required files */
require_once ("class_ldap.inc");
require_once ("class_config.inc");
require_once ("class_userinfo.inc");
require_once ("class_plugin.inc");
require_once ("class_pluglist.inc");
require_once ("class_tabs.inc");
require_once ("class_mail-methods.inc");
require_once("class_password-methods.inc");
require_once ("debuglib.inc");

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

    /* Recurse through all "common" directories */
    if(is_dir($file) && $file!="." && $file!=".." && $file!="CVS"){
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

  if (!isset($config->data['MAIN']['THEME'])){
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
    print_red(sprintf(_("Error when connecting the LDAP. Server said '%s'."),
          $ldap->get_error()));
    echo $_SESSION['errors'];

    /* Hard error. We'd like to use the LDAP, anyway... */
    exit;
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
    echo $_SESSION['errors'];
    exit;
  }
  $ldap->cd($config->current['BASE']);
  $ldap->search("(&(uid=$username)(objectClass=gosaAccount))", array("uid"));

  /* get results, only a count of 1 is valid */
  switch ($ldap->count()){

    /* user not found */
    case 0:	return (NULL);
            break;

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
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaUser=$user)(gosaObject=$object))",
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
    $attrs["gosaObject"] = $object;
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
  $ldap->search ("(&(objectClass=gosaLockEntry)(gosaObject=$object))", array("gosaObject"));
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
  $ldap->search("(&(objectClass=gosaLockEntry)(gosaObject=$object))", array("gosaUser"));
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


function get_list($subtreeACL, $filter, $subsearch= TRUE, $base="", $attrs= array(), $flag= FALSE)
{
  global $config;

  /* Base the search on default base if not set */
  $ldap= $config->get_ldap_link($flag);
  if ($base == ""){
    $ldap->cd ($config->current['BASE']);
  } else {
    $ldap->cd ($base);
  }

  /* Perform ONE or SUB scope searches? */
  if ($subsearch) {
    $ldap->search ($filter, $attrs);
  } else {
    $ldap->ls ($filter);
  }

  /* Check for size limit exceeded messages for GUI feedback */
  if (preg_match("/size limit/i", $ldap->error)){
    $_SESSION['limit_exceeded']= TRUE;
  } else {
    $_SESSION['limit_exceeded']= FALSE;
  }

  /* Crawl through reslut entries and perform the migration to the
     result array */
  $result= array();
  while($attrs = $ldap->fetch()) {
    $dn= preg_replace("/[ ]*,[ ]*/", ",", $ldap->getDN());
    foreach ($subtreeACL as $key => $value){
      if (preg_match("/$key/", $dn)){
        $attrs["dn"]= $dn;
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

  /* Allow fallback to dialog */
  if (isset($_POST['edit_sizelimit'])){
    $_SESSION['size_ignore']= FALSE;
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
  while (preg_match('/,/', $tmp)){
    $tmp= ltrim(strstr($tmp, ","), ",");

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
      if (preg_match("/$module#/", $mod)){
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
  foreach (split (",", $dn) as $val){

    /* We're only interested in organizational units... */
    if (preg_match ("/ou=/", $val)){
      $dep= preg_replace("/ou=([^,]+)/", "\\1", $val)."/$dep";
    }

    /* ... and location objects */
    if (preg_match ("/l=/", $val)){
      $dep= preg_replace("/l=([^,]+)/", "\\1", $val)."/$dep";
    }
  }

  /* Return and remove accidently trailing slashes */
  return rtrim($dep, "/");
}


function get_ou($name)
{
  global $config;

  $ou= $config->current[$name];
  if ($ou != ""){
    if (!preg_match('/^[^=]+=[^=]+/', $ou)){
      return "ou=$ou,";
    } else {
      return "$ou,";
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

  $pattern= "/^[^,]+,".preg_quote(get_people_ou())."/";
  $base= preg_replace($pattern, '', $dn);

  /* Set to base, if we're not on a correct subtree */
  if (!isset($config->idepartments[$base])){
    $base= $config->current['BASE'];
  }

  return ($base);
}


function get_departments($ignore_dn= "")
{
  global $config;

  /* Initialize result hash */
  $result= array();
  $result['/']= $config->current['BASE'];

  /* Get list of department objects */
  $ldap= $config->get_ldap_link();
  $ldap->cd ($config->current['BASE']);
  $ldap->search ("(objectClass=gosaDepartment)", array("ou"));
  while ($attrs= $ldap->fetch()){
    $dn= $ldap->getDN();
    if ($dn == $ignore_dn){
      continue;
    }
    $result[convert_department_dn($dn)]= $dn;
  }

  return ($result);
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

  return preg_match ("/^[0-9 ()+*-]+$/", $nr);
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

  /* If DEBUGLEVEL is set, we're in web mode, use textual output in
     the other case... */
  if (isset($_SESSION['DEBUGLEVEL'])){
    $_SESSION['errors'].= "<div align=\"left\" style=\"border-width:5px;".
      "border-style:solid;border-color:red; background-color:black;".
      "margin-bottom:10px; padding:8px;\"><table><tr><td><img alt=\"\" src=\"".
      get_template_path('images/warning.png')."\"></td>".
      "<td width=\"100%\" align=\"center\"><font color=\"#FFFFFF\">".
      "<b style='font-size:16px;'>$string</b></font></td><td>".
      "<img alt=\"\"src=\"".get_template_path('images/warning.png').
      "\"></td></tr></table></div>\n";
  } else {
    echo "Error: $string\n";
  }
}


function gen_locked_message($user, $dn)
{
  global $plug, $config;

  $_SESSION['dn']= $dn;
  $ldap= $config->get_ldap_link();
  $ldap->cat ($user);
  $attrs= $ldap->fetch();
  $uid= $attrs["uid"][0];

  /* Prepare and show template */
  $smarty= get_smarty();
  $smarty->assign ("dn", $dn);
  $smarty->assign ("message", sprintf(_("You're going to edit the LDAP entry '%s' which appears to be used by '%s'. Please contact the person in order to clarify proceedings."), $dn, "<a href=\"main.php?plug=0&amp;viewid=$uid\">$uid</a>"));

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
    unset ($ar);
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


function show_ldap_error($message)
{
  if (!preg_match("/Success/i", $message)){
    print_red (_("LDAP error:")." $message");
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
    $dn= preg_replace('/,'.get_people_ou().'/' , ',', $dn);
  }
  if (get_groups_ou() != ""){
    $dn= preg_replace('/,'.get_groups_ou().'/' , ',', $dn);
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
  $display.= "  <img src=\"$image\" align=\"middle\" alt=\"*\">&nbsp;$headline\n";
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


function range_selector($dcnt,$start,$range=25)
{

  /* Entries shown left and right from the selected entry */
  $max_entries= 10;

  /* Initialize and take care that max_entries is even */
  $output="";
  if ($max_entries & 1){
    $max_entries++;
  }

  /* Prevent output to start or end out of range */
  if ($start < 0 ){
    $start= 0 ;
  }
  if ($start >= $dcnt){
    $start= $range * (int)(($dcnt / $range) + 0.5);
  }

  $numpages= (($dcnt / $range));
  if(((int)($numpages))!=($numpages))
    $numpages = (int)$numpages + 1;
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

  /* Draw decrement */
  if ($start > 0 ) {
    $output.="  <a href= \"main.php?plug=".validate($_GET['plug'])."&amp;start=".
      (($start-$range))."\">".
      "<img alt=\"\" src=\"images/back.png\" border=0 align=\"middle\"></a>";
  }

  /* Draw pages */
  for ($i= $begin; $i < $end; $i++) {
    if ($ppage == $i){
      $output.= "<a style=\"background-color:#D0D0D0;\" href=\"main.php?plug=".
        validate($_GET['plug'])."&amp;start=".
        ($i*$range)."\">&nbsp;".($i+1)."&nbsp;</a>";
    } else {
      $output.= "<a href=\"main.php?plug=".validate($_GET['plug']).
        "&amp;start=".($i*$range)."\">&nbsp;".($i+1)."&nbsp;</a>";
    }
  }

  /* Draw increment */
  if($start < ($dcnt-$range)) {
    $output.="  <a href= \"main.php?plug=".validate($_GET['plug'])."&amp;start=".
      (($start+($range)))."\">".
      "<img alt=\"\" src=\"images/forward.png\" border=0 align=\"middle\"></a>";
  }

  return($output);
}


function apply_filter()
{
  $apply= "";

  $apply= ''.
    '<table width="100%"  style="border-top:1px solid #B0B0B0;"><tr><td width="100%" align="right">'.
    '<input type="submit" name="apply" value="'._("Apply").'"></td></tr></table>';

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
  /* Variables filled in by subversion */
  $svn_path = '$HeadURL$';
  $svn_revision = '$Revision$';

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

function gosaRaiseError($errno, $errstr, $errfile, $errline)
{
  global $error_collector;

  /* Workaround for buggy imap_open error outputs */
  if (preg_match('/imap_open/', $errstr)){
    return;
  }

  /* FIXME: Workaround for PHP5 error message flooding. The new OOM
     code want's us to use public/protected/private instead of flat
     var declarations. For now I can't workaround this - let's ignore
     the messages till the next major release which may drop support
     for PHP4. */
  if (preg_match('/var: Deprecated./', $errstr)){
    return;
  }

  /* FIXME: Same as above. Compatibility does error flooding.*/
  if (preg_match('/zend.ze1_compatibility_mode/', $errstr)){
    return;
  }

  /* FIXME: Hide ldap size limit messages */
  if (preg_match('/ldap_error/', $errstr)){
    if (preg_match('/sizelimit/', $errstr)){
      return;
    }
  }

  /* Create header as needed */
  if ($error_collector == ""){
    if ($_SESSION['js']==FALSE){
      $error_collector= "<div>";
    } else {
      $error_collector= "<table width=\"100%\" style='background-color:#E0E0E0;border-bottom:1px solid black'><tr><td><img alt=\"\" align=\"middle\" src='".get_template_path('images/warning.png')."'>&nbsp;<font style='font-size:14px;font-weight:bold'>"._("Generating this page caused the PHP interpreter to raise some errors!")."</font></td><td align=right><button onClick='toggle(\"errorbox\")'>"._("Toggle information")."</button></td></tr></table><div id='errorbox' style='position:absolute; z-index:0; visibility: hidden'>";
    }
  }
 
  /* Extract traceback data */
  $trace= debug_backtrace();
  
  /* Create error header */
  $error_collector.= "<table width=\"100%\" cellspacing=0 style='background-color:#402005;color:white;border:2px solid red'><tr><td colspan=3><h1 style='color:white'>"._("PHP error")." \"$errstr\"</h1></td></tr>";
  
  /* Generate trace history */
  for ($index= 1; $index<count($trace); $index++){
    $ct= $trace[$index];
    $loc= "";
    if (isset($ct['class'])){
      $loc.= _("class")." ".$ct['class'];
      if (isset($ct['function'])){
        $loc.= " / ";
      }
    }
    if (isset($ct['function'])){
      $loc.= _("function")." ".$ct['function'];
    }
    if (isset($ct['type'])){
      switch ($ct['type']){
        case "::":
                $type= _("static");
                break;

        case "->":
                $type= _("method");
                break;
      }
    } else {
      $type= "-";
    }
    $args= "";
    foreach ($ct['args'] as $arg){
      $args.= htmlentities("\"$arg\", ");
    }
    $args= preg_replace("/, $/", "", $args);
    if ($args == ""){
      $args= "-";
    }
    $file= $ct['file'];
    $line= $ct['line'];
    $color= ($index&1)?'#404040':'606060';
    $error_collector.= "<tr style='background-color:$color'><td style='padding-left:20px' width=\"30%\">"._("Trace")."[$index]: $loc</td>";
    $error_collector.= "<td>"._("File").": $file ("._('Line')." $line)</td><td width=\"10%\">"._("Type").": $type</td></tr>";
    $error_collector.= "<tr style='background-color:$color'><td colspan=3 style='padding-left:20px;'>"._("Arguments").": $args</td></tr>";
  }

  /* Close error table */
  $error_collector.= "</table>";

  /* Write to syslog */
  gosa_log ("PHP error: $errstr ($errfile, line $errline)");
}

function dummy_error_handler()
{
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
