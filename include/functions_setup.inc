<?php

function check_schema_version($description, $version)
{
  $desc= preg_replace("/^.* DESC\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $description);

  return preg_match("/\(v$version\)/", $desc);
}


function view_schema_check($table)
{
  $message="<table summary=\"\" class=\"check\">";

  foreach ($table as $key => $values){
    $msg = $values['msg'];
    $message.= "<tr><td class=\"check\">$msg";

    if($values['status']) {
      $message.="</td><td style='text-align:center' >
        <img src=images/true.png alt='true' /></td></tr>";
    } else {
      $message.="</td><td style='text-align:center' >
        <img src=images/button_cancel.png alt='false' /></td></tr>";
    }
  }
  $message.="</table>";

  return $message;
}


function is_schema_readable($server, $admin, $password)
{
  $ldap = new LDAP($admin,$password,$server);
  $tmp = $ldap->get_objectclasses();

  if(count($tmp)){
    return(true);
  }
  return(false);
} 

function schema_check($server, $admin, $password, $aff=0,$CalledByIndexPhP=false)
{
  global $config;

  $messages= array();
  $required_classes= array(
      "gosaObject"            => array("version" => "2.4"),
      "gosaAccount"           => array("version" => "2.4"),
      "gosaLockEntry"         => array("version" => "2.4"),
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
      "goShareServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
      "goNtpServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
      "goSyslogServer"        => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
      "goLdapServer"          => array("version" => "2.4"),
      "goCupsServer"          => array("version" => "2.4", "class" => array("posixAccount", "terminals"),),
      "goImapServer"          => array("version" => "2.4", "class" => array("mailAccount", "mailgroup"),"file" => "gosa+samba3.schema"),
      "goKrbServer"           => array("version" => "2.4"),
      "goFaxServer"           => array("version" => "2.4", "class" => "gofaxAccount","file" => "gofax.schema"),
      );

  /* Get objectclasses */
  $ldap = new LDAP($admin,$password, $server);
  $objectclasses = $ldap->get_objectclasses(); 
  if(count($objectclasses) == 0){
    return (array(array("msg" => _("Can't get schema information from server. No schema check possible!"), "status" => FALSE)));
  }

  /* Walk through objectclasses and check if they are needed or not */
  foreach ($required_classes as $key => $value){
    if (isset($value['class'])){
      if (!is_array($value['class'])){
        $classes= array($value['class']);
      } else {
        $classes= $value['class'];
      }

      /* Check if we are using the class that requires */
      foreach($classes as $class){
        if (!isset($objectclasses[$key])){
          $messages[$key]['msg']= sprintf(_("Optional objectclass '%s' required by plugin '%s' is not present in LDAP setup"), $key, $class);
          $messages[$key]['status'] = FALSE;
        } else {
          if (!check_schema_version($objectclasses[$key], $value['version'])){
            $messages[$key]['msg']= sprintf(_("Optional objectclass '%s' required by plugin '%s' does not have version %s"), $key, $class, $value['version']);
            $messages[$key]['needonstartup'] = TRUE;
            $messages[$key]['status'] =FALSE;
          }else {
            if(!isset($affich2[$class])){
              $affich2[$class]['msg']   = sprintf(_("Support for '%s' enabled"), $class)."<td class=\"check\"> ".$value['file']."</td>";
              $affich2[$class]['status']= TRUE; 
            }
          }
        }

      }
    } else {
      /* Required class */
      if (!isset($objectclasses[$key])){
        $messages[$key]['msg']= sprintf(_("Required objectclass '%s' is not present in LDAP setup"), $key);
        $messages[$key]['status'] = FALSE;  
      } else {
        if (!check_schema_version($objectclasses[$key], $value['version'])){
          $messages[$key]['msg']= sprintf(_("Required objectclass '%s' does not have version %s"), $key, $value['version']);
          $messages[$key]['status'] = FALSE;  
          $messages[$key]['needonstartup'] = TRUE;
        }
    
      }
    }
  }

  /* Check for correct samba parameters */
  if (!isset($objectclasses['sambaSamAccount'])){
    $messages['samba3']['msg']= _("SAMBA 3 support disabled, no schema seems to be installed");
    $affich['samba3']['msg']= $messages['samba3']['msg']."<td class=\"check\">gosa+samba3.schema</td>";
    $messages['samba3']['status']= FALSE;
    $affich['samba3']['status']= FALSE;
  }else{
    $affich['samba3']['msg']= _("SAMBA 3 support enabled")."<td class=\"check\">gosa+samba3.schema</td>";
    $affich['samba3']['status']= TRUE;
  }

  if (!isset($objectclasses['sambaAccount'])){
    $messages['samba2']['msg']= _("SAMBA 2 support disabled, no schema seems to be installed");
    $affich['samba2']['msg']= $messages['samba2']['msg']."<td class=\"check\">samba.schema</td>";
    $messages['samba2']['status']= FALSE;
    $affich['samba2']['status']= FALSE;
  }else{
    $affich['samba2']['msg']= _("SAMBA 2 support enabled")."<td class=\"check\">samba.schema</td>";
    $affich['samba2']['status']= TRUE;
  }

  /* Check pureftp/dns/ */
  if (!isset($objectclasses['PureFTPdUser'])){
    $messages['pureftp']['msg']= _("Support for pureftp disabled, no schema seems to be installed");
    $affich['pureftp']['msg']= $messages['pureftp']['msg']."<td class=\"check\">pureftpd.schema</td>";
    $messages['pureftp']['status']= FALSE;
    $affich['pureftp']['status']= FALSE;
  }else{
    $affich['pureftp']['msg']= _("Support for pureftp enabled")."<td class=\"check\">pureftpd.schema</td>";
    $affich['pureftp']['status']= TRUE;
  }

  if (!isset($objectclasses['gosaWebdavAccount'])){
    $messages['webdav']['msg']= _("Support for WebDAV disabled, no schema seems to be installed");
    $affich['webdav']['msg']= $messages['webdav']['msg']."<td class=\"check\"></td>";
    $messages['webdav']['status']= FALSE;
    $affich['webdav']['status']= FALSE;
  }else{
    $affich['webdav']['msg']=_("Support for WebDAV enabled")."<td class=\"check\">gosa+samba3.schema</td>";
    $affich['webdav']['status']= TRUE;
  }

  if (!isset($objectclasses['phpgwAccount'])){
    $messages['phpgroupware']['msg']= _("Support for phpgroupware disabled, no schema seems to be installed");
    $affich['phpgroupware']['msg']= $messages['phpgroupware']['msg']."<td class=\"check\">phpgwaccount.schema</td>";
    $messages['phpgroupware']['status']= FALSE;
    $affich['phpgroupware']['status']= FALSE;
  }else{
    $affich['phpgroupware']['msg']= _("Support for phpgroupware enabled")."<td class=\"check\">phpgwaccount.schema</td>";
    $affich['phpgroupware']['status']= TRUE;
  }
  
  if (!isset($objectclasses['trustAccount'])){
    $messages['trustAccount']['msg']= _("Support for trustAccount disabled, no schema seems to be installed");
    $affich['trustAccount']['msg']= $messages['trustAccount']['msg']."<td class=\"check\">trust.schema</td>";
    $messages['trustAccount']['status']= FALSE;
    $affich['trustAccount']['status']= FALSE;
  }else{
    $affich['trustAccount']['msg']= _("Support for trustAccount enabled")."<td class=\"check\">trust.schema</td>";
    $affich['trustAccount']['status']= TRUE;
  }

  if (!isset($objectclasses['goFonAccount'])){
    $messages['phoneaccount']['msg']= _("Support for gofon disabled, no schema seems to be installed");
    $affich['phoneaccount']['msg']= $messages['phoneaccount']['msg']."<td class=\"check\">gofon.schema</td>";
    $messages['phoneaccount']['status']= FALSE;
    $affich['phoneaccount']['status']= FALSE;
  }else{
    $affich['phoneaccount']['msg']= _("Support for gofon enabled")."<td class=\"check\">gofon.schema</td>";
    $affich['phoneaccount']['status']= true;
  }

  if (!isset($objectclasses['nagiosContact'])){
    $messages['nagioscontact']['msg']= _("Support for nagios disabled, no schema seems to be installed");
    $affich['nagioscontact']['msg']= $messages['nagioscontact']['msg']."<td class=\"check\">nagios.schema</td>";
    $messages['nagioscontact']['status']= FALSE;
    $affich['nagioscontact']['status']= FALSE;
  }else{
    $affich['nagioscontact']['msg']= _("Support for nagios enabled")."<td class=\"check\">nagios.schema</td>";
    $affich['nagioscontact']['status']= true;
  }
  
  if ((!isset($objectclasses['apple-user'])) || (!isset($objectclasses['mount'])) ){
    $messages['netatalk']['msg']= _("Support for netatalk disabled, no schema seems to be installed");
    $affich['netatalk']['msg']= $messages['netatalk']['msg']."<td class=\"check\">apple.schema</td>";
    $messages['netatalk']['status']= FALSE;
    $affich['netatalk']['status']= FALSE;
  }else{
    $affich['netatalk']['msg']= _("Support for netatalk enabled")."<td class=\"check\">apple.schema</td>";
    $affich['netatalk']['status']= true;
  }
  
  /* Fix for PHP Fehler "Undefined index: ldapconf"
   * Ablaufverfolgung[1]: Funktion schema_check	Datei: /home/hickert/gosa/include/functions_setup.inc (Zeile 230)
   */
  if((isset($_SESSION['ldapconf']['mail_methods']))&&(isset($_SESSION['ldapconf']))){
	if(($_SESSION['ldapconf']['mail_methods'][$_SESSION['ldapconf']['mail']] == "kolab")&&(!$CalledByIndexPhP)){
	  if(!isset($objectclasses['kolabInetOrgPerson']))  {
	    $messages['kolab']['msg']= _("Support for Kolab disabled, no schema seems to be installed, setting mail-method to cyrus");
	    $affich['kolab']['msg']=$messages['kolab']['msg']."<td class=\"check\">kolab2.schema</td>";
	    $tmp= array_flip($_SESSION['ldapconf']['mail_methods']);
	    $_SESSION['ldapconf']['mail']=$tmp['cyrus'];
	    $messages['kolab']['status']= FALSE;
	    $affich['kolab']['status']= FALSE;
	  }else{
	    $affich['kolab']['msg']=_("Support for Kolab enabled")."<td class=\"check\">gofon.schema</td>";
	    $affich['kolab']['status']= TRUE;
	  }
	}
  }
  if($aff==0){
    return ($messages);
  } else {
    return(array_merge($affich,$affich2));
  }
}


function check(&$faults, $message, $description, $test, $required= TRUE)
{
  $msg= "<table summary=\"\" class='check'><tr><td class='check' style='font-size:14px;'>$message</td>
    <td rowspan=2 style='vertical-align:middle; text-align:center;width:45px;'>";
  if ($test){
    $msg.= _("OK")."<br>";
  } else {
    if (!$required){
      $msg.="<font color=red>"._("Ignored")."</font><br>";
    } else {
      $msg.="<font color=red>"._("Failed")."</font><br>";
      $faults++;
    }
  }
  $msg.= "</td></tr><tr><td class='check' style='padding-left:20px;".
    "background-color:#F0F0F0;'>$description</td></tr></table><br>";

  return $msg;
}

function perform_php_checks(&$faults)
{
  global $check_globals;

  $faults= 0;
  $msg= "";

  $msg.= "<h1>"._("PHP setup inspection")."</h1>";

  $msg.= check (	$faults, _("Checking for PHP version (>=4.1.0)"),
      _("PHP must be of version 4.1.0 or above for some functions and known bugs in PHP language."),
      version_compare(phpversion(), "4.1.0")>=0);

  $msg.= check (	$faults, _("Checking if register_globals is set to 'off'"),
      _("register_globals is a PHP mechanism to register all global varibales to be accessible from scripts without changing the scope. This may be a security risk. GOsa will run in both modes."),
      $check_globals == 0, FALSE);
  
  $msg.= check (  $faults, _("PHP session.gc_maxlifetime (>= 86400 seconds)."),
      _("PHP uses this value for the garbage collector to delete old sessions, setting this value to one day will prevent loosing session and cookie  before they really timeout."),
      ini_get("session.gc_maxlifetime") >= 86400,FALSE);

  $msg.= check (	$faults, _("Checking for ldap module"),
      _("This is the main module used by GOsa and therefore really required."),
      is_callable('ldap_bind'));

  $msg.= check (  $faults, _("Checking for XML functions"),
      _("XML functions are required to parse the configuration file."),
      is_callable('xml_parser_create'));

  $msg.= check (	$faults, _("Checking for gettext support"),
      _("Gettext support is required for internationalized GOsa."),
      is_callable('bindtextdomain'));

  $msg.= check (	$faults, _("Checking for iconv support"),
      _("This module is used by GOsa to convert samba munged dial informations and is therefore required."),
      is_callable('iconv'));

  $msg.= check (	$faults, _("Checking for mhash module"),
      _("To use SSHA encryption, you'll need this module. If you are just using crypt or md5 encryption, ignore this message. GOsa will run without it."),
      is_callable('mhash'), FALSE);

  $msg.= check (	$faults, _("Checking for imap module"),
      _("The IMAP module is needed to communicate with the IMAP server. It gets status informations, creates and deletes mail users."),
      is_callable('imap_open'));

  $msg.= check (	$faults, _("Checking for getacl in imap"),
      _("The getacl support is needed for shared folder permissions. The standard IMAP module is not capable of reading acl's. You need a recend PHP version for this feature."),
      is_callable('imap_getacl'), FALSE);

  $msg.= check (	$faults, _("Checking for mysql module"),
      _("MySQL support is needed for reading GOfax reports from databases."),
      is_callable('mysql_query'), FALSE);

  $msg.= check (	$faults, _("Checking for cups module"),
      _("In order to read available printers from IPP protocol instead of printcap files, you've to install the CUPS module."),
      is_callable('cups_get_dest_list'), FALSE);

  $msg.= check (	$faults, _("Checking for kadm5 module"),
      _("Managing users in kerberos requires the kadm5 module which is downloadable via PEAR network."),
      is_callable('kadm5_init_with_password'), FALSE);

  $msg.= check (  $faults, _("Checking for snmp Module"),
      _("Simple Network Management Protocol (SNMP) is required for client monitoring."),
      is_callable('snmpget'), FALSE);

  return ($msg);
}

function get_link($function_name) {
  $result= "<a href='http://de.php.net/manual/en/function.";

  /* Replace all underscores with hyphens (phpdoc convention) */
  /* hjb: added alternative check for GraphicsMagick >= 1.1.2 */
  $function_name= str_replace("_", "-", $function_name);

  /* Append to base URL */
  $result.= $function_name.".php'>$function_name</a>";

  return $result;
}

function perform_additional_function_checks(&$faults) {
  global $check_globals;

  $faults= 0;
  $msg= "";
  $functions= array();
  
  $functions_list= '../include/functions_list.inc';

  /* Make sure that we can read the file */
  if(is_readable($functions_list)) {
    /* Open filehandle */
    $fh= fopen($functions_list,'rb');
    if($fh!=null) {
      $functions= eval(fread($fh,filesize($functions_list)));
    }
  }

  $msg.= "<h1>"._("PHP detailed function inspection")."</h1>";
  /* Only print message, if function is not callable */
  foreach($functions as $key => $fn_name) {
    if(!is_callable($fn_name)) {
      $msg.= check ($faults, sprintf(_("Checking for function %s"), "<b>".get_link($fn_name)."</b>"),
        sprintf(_("The function %s is used by GOsa. There is no information if it's optional or required yet."), "<b>".get_link($fn_name)."</b>"),
        is_callable($fn_name), false);
    }
  }
  return $msg;
}

function perform_additional_checks(&$faults)
{
  $ret = NULL;
  /* Programm check */
  $msg= "<h1>"._("Checking for some additional programms")."</h1>";

  /* Image Magick */
  $query= "LC_ALL=C LANG=C convert -help";
  $output= shell_exec ($query);
  if ($output != ""){
    $lines= split ("\n", $output);
    $version= preg_replace ("/^Version: (.+Magick) ([^\s]+).*/", "\\1 \\2", $lines[0]);
    list($prog, $version) = split(" ", $version);
    list($major, $minor,$minor2)= split("\.", $version);
    if (preg_match('/GraphicsMagick/', $prog)) {
      $msg.= check (	$faults, _("Checking for GraphicsMagick (>=1.1.2)"),
          _("GraphicsMagick is used to convert user supplied images to fit the suggested size and the unified JPEG format."),
          ($major > 1 || ($major == 1 && $minor >= 1) || ($major == 1 && $minor == 1 && $minor2 >= 2) ) );
    } else {
      $msg.= check (	$faults, _("Checking for ImageMagick (>=5.4.0)"),
          _("ImageMagick is used to convert user supplied images to fit the suggested size and the unified JPEG format."),
          ($major > 5 || ($major == 5 && $minor >= 4)));
        }
  } else {
    $msg.= check (	$faults, _("Checking imagick module for PHP"),
        _("Imagick is used to convert user supplied images to fit the suggested size and the unified JPEG format from PHP script."), function_exists('imagick_blob2image'), TRUE);
  }

  /* Check for fping */
  $query= "LC_ALL=C LANG=C fping -v 2>&1";
  $output= shell_exec ($query);
  $have_fping= preg_match("/^fping:/", $output);
  $msg.= check (	$faults, _("Checking for fping utility"),
      _("The fping utility is only used if you've got a thin client based terminal environment running."),
      $have_fping, FALSE);

  /* Check for smb hash generation tool */
  $query= "mkntpwd 2>&1";
  $output= shell_exec ($query);
  $have_mkntpwd= preg_match("/^Usage: mkntpwd /", $output);
  $alt = 0;

  if (!$have_mkntpwd){
    $query= "LC_ALL=C LANG=C perl -MCrypt::SmbHash -e 'ntlmgen \"PASSWD\", \$lm, \$nt; print \"\${lm}:\${nt}\\n\";' &>/dev/null";
    system ($query, $ret);
    $alt= ($ret == 0);
  }

  $msg.= check (	$faults, _("Checking for a way to generate LM/NT password hashes"),
      _("In order to use SAMBA 2/3, you've to install some additional packages to generate password hashes."),
      ($have_mkntpwd || $alt));

  /* seesio.auto_start should be off, in order to without trouble*/
  $session_auto_start = ini_get('session.auto_start');
  $implicit_flush     = ini_get('implicit_flush');
  $max_execution_time = ini_get('max_execution_time');
  $memory_limit       = ini_get('memory_limit');
  $expose_php         = ini_get('expose_php');
  $magic_quotes_gpc   = ini_get('magic_quotes_gpc');
  $register_globals   = ini_get('register_globals');

  /* auto_register */
  $msg.= check (  $faults, _("php.ini check -> session.auto_register"),
      _("In Order to use GOsa without any trouble, the session.auto_register option in your php.ini must be set to 'Off'."), (!$session_auto_start['local_value']));

  /* implicit_flush */
  $msg.= check (  $faults, _("php.ini check -> implicit_flush"),
      _("This option influences the Output handling. Turn this Option off, to increase performance."),
      !$implicit_flush['local_value'],0,false);

  /* max_execution_time */
  if($max_execution_time['local_value'] < 30 ){
    $max_execution_time['local_value']=false;
  }
  $msg.= check (  $faults, _("php.ini check -> max_execution_time"),
      _("The Execution time should be at least 30 seconds, because some actions may consume more time."),
      $max_execution_time['local_value'],0,false);

  /* memory_limit */
  if($memory_limit['local_value'] < 16 ){
    $memory_limit['local_value']=false;
  }
  $msg.= check (  $faults, _("php.ini check -> memory_limit"),
      _("GOsa needs at least 16MB of memory, less will cause unpredictable errors! Increase it for larger setups."),
      !$implicit_flush['local_value'],0,false);

  /* expose_php */
  $msg.= check (  $faults, _("php.ini check -> expose_php"),
      _("Increase the server security by setting expose_php to 'off'. PHP won't send any Information about the server you are running in this case."),
      !$implicit_flush['local_value'],0,false);

  /* magic_quotes_gpc */
  $msg.= check (  $faults, _("php.ini check -> magic_quotes_gpc"),
      _("Increase your server security by setting magic_quotes_gpc to 'on'. PHP will escape all quotes in strings in this case."),
      $magic_quotes_gpc['local_value'],0,false);

  return $msg;
}


function parse_contrib_conf()
{

  $str                = "";
  $used_samba_version = 0;
  $query              = ""; 
  $fp                 = false;
  $output             = "";
  $needridbase_sid    = false;
  $pwdhash            = "";
  $replacements       = array();
  $ldapconf           = $_SESSION['ldapconf']; // The Installation information
  $classes            = $_SESSION['classes'];  // Class information needed to define which features are enabled
  $possible_plugins   = array();

  /* Which samba version do we use? */
  if(isset($classes['samba3'])){
    $used_samba_version = 2;
  } else {
    $used_samba_version = 3;
  }

  /* Look for samba password generation method */
  if(file_exists("/usr/bin/mkntpasswd")){
    $pwdhash  = "/usr/bin/mkntpasswd";
  } elseif (preg_match("/^Usage: mkntpwd /", shell_exec ("mkntpwd 2>&1"))){
    $pwdhash= "mkntpwd";
  } else {
    $pwdhash=('perl -MCrypt::SmbHash -e "ntlmgen \"\$ARGV[0]\", \$lm, \$nt; print \"\${lm}:\${nt}\n\";" $1');
  }


  /* Define which variables will be replaced */
  $replacements['{LOCATIONNAME}']  = $ldapconf['location'];
  $replacements['{SAMBAVERSION}']  = $used_samba_version;
  $replacements['{LDAPBASE}']      = $ldapconf['base'];
  $replacements['{LDAPADMIN}']     = $ldapconf['admin'];
  $replacements['{UIDBASE}']       = $ldapconf['uidbase'];
  $replacements['{DNMODE}']        = $ldapconf['peopledn'];
  $replacements['{LDAPHOST}']      = $ldapconf['uri'];
  $replacements['{PASSWORD}']      = $ldapconf['password'];
  $replacements['{CRYPT}']         = $ldapconf['arr_cryptkeys'][$ldapconf['arr_crypts']];
  $replacements['{SID}']         = "";
  $replacements['{RIDBASE}']     = "";
  if($ldapconf['mail'] != "disabled"){
    $replacements['{MAILMETHOD}']    = $ldapconf['mail_methods'][$ldapconf['mail']];
  }   
  $replacements['{SMBHASH}']       = $pwdhash;
  $replacements['{GOVERNMENTMODE}']= "false"; 
  $replacements['{kolabAccount}']  = "";
  $replacements['{servKolab}']     = "";
  $replacements['{errorlvl}']     = $ldapconf['errorlvl'];

  /* This array contains all preg_replace syntax to delete all unused plugins
     THE kEY MUST BE THE CLASSNAME so we can check it with $ldapconf['classes'] */

  $possible_plugins['fonreport'][]   = "'\n.*<plugin.*fonreport+.*\n.*>.*\n'i";
  $possible_plugins['phoneaccount'][]= "'\n.*<tab.*phoneAccount.*>.*\n'i";
  $possible_plugins['logview'][]     = "'\n.*<plugin.*logview+.*\n.*>.*\n'i";
  $possible_plugins['pureftp'][]     = "'\n.*<tab.*pureftp.*>.*\n'i";
  $possible_plugins['webdav'][]      = "'\n.*<tab.*webdav.*>.*\n'i";
  $possible_plugins['phpgroupware'][]= "'\n.*<tab.*phpgroupware.*>.*\n'i";
  $possible_plugins['netatalk'][0]    = "'\n.*<plugin.*netatalk+.*\n.*>.*\n'i";
  $possible_plugins['netatalk'][1]    = "'\n.*<tab.*netatalk.*>.*\n'i";

  /*Header information
     Needed to send the generated gosa.conf to the browser */
  header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
  header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
  header("Cache-Control: no-cache");
  header("Pragma: no-cache");
  header("Cache-Control: post-check=0, pre-check=0");
  header("Content-type: text/plain");

  if (preg_match('/MSIE 5.5/', $_SERVER['HTTP_USER_AGENT']) ||
      preg_match('/MSIE 6.0/', $_SERVER['HTTP_USER_AGENT'])){
    header('Content-Disposition: filename="gosa.conf"');
  } else {
    header('Content-Disposition: attachment; filename="gosa.conf"');
  }

  if(!$fp=fopen(CONFIG_TEMPLATE_DIR."/gosa.conf","r")) {
    echo "Can't open file ".CONFIG_TEMPLATE_DIR."/gosa.conf";
  } else {
    while(!feof($fp)) {
      $str.= fread($fp,512);
    }

    if($ldapconf['mail_methods'][$ldapconf['mail']]=="kolab") {
      $replacements['{kolabAccount}']  ="<tab class=\"kolabAccount\" />\n     ";
      $replacements['{servKolab}']     ="<tab class=\"servkolab\" name=\"Kolab\" />";
    }

    if($used_samba_version == 2) {
      /* Do nothing for samba 2... */
    } else {
      /* Create LDAP connection, to check if there's a domain
         object defined in the LDAP schema */
      $ldap= new LDAP($ldapconf['admin'], $ldapconf['password'], $ldapconf['uri']);

      /* Try to find a Samba Domain Objekt */
      $ldap->search("(objectClass=sambaDomain)");
    
      /* Something found ??? so we need to define ridbase an SID by ourselfs */
      if($ldap->count()< 1) {
        $replacements['{SID}']= "sid=\"123412-11\"";
        $replacements['{RIDBASE}']= "ridbase=\"1000\"";  
      } 
    }

    /* Data readed, types replaced, samba version detected and checked if
       we need to add SID and RIDBASE. Check if there is an ivbbentry in
       the LDAP tree, in this case we will set the governmentmode to true.
       Create LDAP connection, to check if theres a domain Objekt definen
       in the LDAP schema. */
    if(!isset($ldap)){
      $ldap= new LDAP($ldapconf['admin'], $ldapconf['password'], $ldapconf['uri']);
    }

    /* Try to find a Samba Domain Objekt */
    $ldap->search("(objectClass=ivbbentry)");

    /* Something found ??? so we need to define ridbase an SID by ourselfs */
    if($ldap->count()> 0) {
      $replacements['{GOVERNMENTMODE}']= "true";
    }

    /* Replace all colleted information with placeholder */
    foreach($replacements as $key => $val) {
      $str = preg_replace("/".$key."/",$val,$str);
    }    

    if($ldapconf['mail'] == "disabled"){
      $str = str_replace("mailMethod=\"{MAILMETHOD}\"","",$str);
    }

    /* Remove all unused plugins */
    foreach(array_keys($possible_plugins) as $akey) {
      if(array_key_exists($akey,$classes)) {
        foreach($possible_plugins[$akey] as $key=>$val) {
          $str = preg_replace($val,"\n",$str);
        }
      }
    }
  }

  return ((($str)));
}


/* Show setup_page 1 */
function show_setup_page1($withoutput = true)
{
  $faults   = false;
  $faults2  = false;
  $smarty = get_smarty();  
  $smarty->assign ("content", get_template_path('setup_introduction.tpl'));
  $smarty->assign ("tests", perform_php_checks($faults));
  $smarty->assign ("detailed_tests", perform_additional_function_checks($faults2));

  $faults = $faults || $faults2;

  /* This var is true if anything went wrong */
  if ($faults){
    $smarty->assign("mode", "disabled");
  }

  /* This line displays the template only if (withoutput is set) */
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }

  if (isset($_SESSION['errors'])){
    $smarty->assign("errors", $_SESSION['errors']);
  }

  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }

  return ($faults);
}


/* Show setup_page 2 */
function show_setup_page2($withoutput = true)
{
  $faults = false;
  $smarty = get_smarty();
  $smarty->assign ("content", get_template_path('setup_step2.tpl'));
  $smarty->assign ("tests", perform_additional_checks($faults));

  if ($faults) {
    $smarty->assign("mode", "disabled");
  }
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }
  if (isset($_SESSION['errors']))  {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }

  return ($faults);                               
}


function show_setup_page3($withoutput = true)
{
  $ds = NULL;
  $smarty = get_smarty();

  /* Take the Post oder the Sessioin saved data */
  if(isset($_POST['uri'])){
    $uri = $_POST['uri'];
  } elseif(isset($_SESSION['ldapconf']['uri'])){
    $uri = $_SESSION['ldapconf']['uri'];
  }

  /* If Page called first time, field is empty */
  if((!isset($uri))||(empty($uri))){
    $uri = "ldap://localhost:389";
  }

  /* if isset $uri save it to session */
  if(isset($uri)) {
    $_SESSION['ldapconf']['uri'] = $uri;
    $smarty->assign ("uri", validate($uri));
  }

  /* No error till now */
  $fault = false;

  if (!$ds = @ldap_connect (validate($uri))) {
    $fault =true;

    /* Output the Error */
    if($withoutput) {
      print_red (_("Can't connect to the specified LDAP server! Please make sure that is reachable for GOsa."));
      $smarty->assign ("uri", validate($uri));
      $smarty->assign ("content", get_template_path('setup_step3.tpl'));
    }
  } else {
    /* Try to bind the connection */    
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);

    /* if we can't bind , print error */
    if (!$r  =  @ldap_bind ($ds)) {
      $fault = true;

      /* Output the Error */
      if($withoutput) {
        print_red (_("Can't bind to the specified LDAP server! Please make sure that it is reachable for GOsa."));
        $smarty->assign ("content", get_template_path('setup_step3.tpl'));
        $smarty->assign ("uri", validate($uri));
      }
    } else {
      $fault = false;
    }
  }

  $smarty->assign ("content", get_template_path('setup_step3.tpl'));

  /* Load Header */
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }

  /* Set Errors to Smarty */
  if (isset($_SESSION['errors'])) {
    $smarty->assign("errors", $_SESSION['errors']);
  }

  /* Print out Template */ 
  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }

  return ($fault);                             
}


function show_setup_page4($withoutput = true)
{
  $smarty= get_smarty();      

  /* check POST data */
  if(isset($_POST['check'])) {

    /* Check if all needed vars are submitted */
    foreach($checkvars as $key) {
      if($key == "peopleou"){
        continue;
      }
      if($key == "groupou"){
        continue;
      }

      if((isset($_POST[$key]))&&($_POST[$key]!="")) {
        $_SESSION['ldapconf'][$key] = $_POST[$key];
      } else {
        if($withoutput) {
          print_red(sprintf(_("You're missing the required attribute '%s' from this formular. Please complete!"), $key));
        }
        $fault = true;
      }
    }
  }

  /* Transfer base */
  if(isset($_POST['base'])){
    $_SESSION['ldapconf']['base']= $_POST['base'];
  }

	// ?
  if(!isset($_SESSION['ldapconf']['base'])){
    $_SESSION['ldapconf']['base']= $base;
  }

  if(!isset($_SESSION['ldapconf']['base'])){
    $_SESSION['ldapconf']['base']= $base;
  }
  require_once("class_password-methods.inc");

  $fault     = false;              
  $uri       = $_SESSION['ldapconf']['uri'];
  $ldapconf  = $_SESSION['ldapconf'];
  $arr_crypts= array();
  $temp      = "";
  $checkvars = array("location", "admin", "password", "peopleou", "base",
      "peopledn", "arr_crypts", "mail", "uidbase","errorlvl");

  if(!isset($_SESSION['ldapconf']['arr_cryptkeys'])) {
    require_once("class_password-methods.inc");
    $tmp= passwordMethod::get_available_methods_if_not_loaded();
    $_SESSION['ldapconf']['arr_cryptkeys']= $tmp['name'];
  }

  if(!isset($_SESSION['ldapconf']['mail_methods'])) {
    $_SESSION['ldapconf']['mail_methods']=array();
    $temp = get_available_mail_classes();
    $_SESSION['ldapconf']['mail_methods']= $temp['name'];
  }

  /* If there are some empty vars in ldapconnect -
     these values also represent out default values */
  if(!$ds =  @ldap_connect (validate($uri))){
    $fault = true;
    if($withoutput){
      print_red (_("Can't connect to the specified LDAP server! Please make sure that is reachable for GOsa."));
    }
  } elseif(!@ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3)){
    $fault = true;
    if($withoutput){
      print_red (_("Can't set ldap protocol version 3."));
    }
  } elseif(!$r= @ldap_bind ($ds)){
    $fault = true;
    if($withoutput){
      print_red (_("Could not bind to the specified LDAP server! Please make sure that it is reachable for GOsa."));
    }
  } else {
    $sr=   @ldap_search ($r, NULL, "objectClass=*", array("namingContexts"));
    $attr= @ldap_get_entries($ds,$sr);

    if((empty($attr))) {

      if($withoutput){
#        print_red(_("Bind to server successful, but the server seems to be completly empty, please check all information twice"));
      }

    } else {
      $base= $attr[0]['dn'];
    }
  }

  if(!isset($_SESSION['ldapconf']['base'])){
    $_SESSION['ldapconf']['base']= $base;
  }
  if(!isset($_SESSION['ldapconf']['admin'])){
    $_SESSION['ldapconf']['admin']= "cn=ldapadmin,".$base;
  }
  if(!isset($_SESSION['ldapconf']['peopleou'])){
    $_SESSION['ldapconf']['peopleou']= "ou=people";
  }
  if(!isset($_SESSION['ldapconf']['groupou'])){
    $_SESSION['ldapconf']['groupou']= "ou=groups";
  }
  if(!isset($_SESSION['ldapconf']['peopledn'])){
    $_SESSION['ldapconf']['peopledn']= "cn";
  }
  if(!isset($_SESSION['ldapconf']['password'])){
    $_SESSION['ldapconf']['password']= "";
  }
  if(!isset($_SESSION['ldapconf']['location'])){
    $_SESSION['ldapconf']['location']= "Example";
  }
  if(!isset($_SESSION['ldapconf']['uidbase'])){
    $_SESSION['ldapconf']['uidbase']= "1000";
  }
  if(!isset($_SESSION['ldapconf']['mail'])){
    $_SESSION['ldapconf']['mail']= 0;
  }
  $tmp= array_flip($_SESSION['ldapconf']['arr_cryptkeys']);
  if(!isset($_SESSION['ldapconf']['arr_crypts'])){
    $_SESSION['ldapconf']['arr_crypts']   = $tmp['md5'];
  }

  $smarty->assign("arr_cryptkeys",$_SESSION['ldapconf']['arr_cryptkeys']);
  $smarty->assign("mail_methods", $_SESSION['ldapconf']['mail_methods']);

  foreach($_SESSION['ldapconf'] as $key => $val) {
    $smarty->assign($key,$val);
  }

  if(isset($_POST['check']) || (isset($_POST['admin'])) && isset($_POST['password'])) {
    $ldap= new LDAP($_SESSION['ldapconf']['admin'],
        $_SESSION['ldapconf']['password'],
        $_SESSION['ldapconf']['uri']);

    $m= schema_check($_SESSION['ldapconf']['uri'],
        $_SESSION['ldapconf']['admin'],
        $_SESSION['ldapconf']['password']);
    $_SESSION['classes']= $m;

    if(!is_schema_readable($ldapconf['uri'],$ldapconf['admin'],$ldapconf['password'])){
      if($withoutput){
        print_red(_("Can't read schema informations, GOsa needs to know your schema setup. Please verify that it is readable for GOsa"));
      }
      $fault=true;
    }



    if ($ldap->error != "Success") {
      if($withoutput) {
        print_red(sprintf(_("Can't log into LDAP server. Reason was: %s."), $ldap->get_error()));
      }
      $fault = true;
    }
  }

  /* Set smarty output */
  $smarty->assign ("content", get_template_path('setup_step4.tpl'));
  $smarty->assign ("peopledns", array("cn", "uid"));
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }
  if(isset($_SESSION['errors'])) {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }

  return ($fault);
}


function show_setup_page5($withoutput=true)
{
  /* Get ldapconf */
  $ldapconf= $_SESSION['ldapconf'];

  /* get smarty */
  $smarty = get_smarty();

  if(isset($_SESSION['classes'])){
    $classes = $_SESSION['classes'];
  }

  $info= posix_getgrgid(posix_getgid());
  $smarty->assign("webgroup", $info['name']);
  $smarty->assign("path", CONFIG_DIR);
  $message= "<table summary=\"\" class=\"check\">";
  $m= schema_check($ldapconf['uri'], $ldapconf['admin'], $ldapconf['password'],1);

  if($withoutput) {
    $smarty->assign ("schemas", view_schema_check($m));
    $smarty->assign ("content", get_template_path('setup_finish.tpl'));
  }

  /* Output templates... */
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }
  if (isset($_SESSION['errors'])) {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }

  return(true);
}


function create_user_for_setup($withoutput=true)
{
  global $samba;

  $ldapconf = $_SESSION['ldapconf'];
  $smarty = get_smarty();
  
  $need_to_create_group = false;
  $need_to_create_user  = false;

  $str_there="";

  if(isset($_SESSION['classes'])){
    $classes= $_SESSION['classes'];
  }

  /* Everything runns perfect ...
     So we do a last test on this page
     is there a user with ACLs :all which will be able to adminsitrate GOsa
     We check that, if this user or group is missing we ask for creating them */
  $ldap= new LDAP($_SESSION['ldapconf']['admin'],    $_SESSION['ldapconf']['password'],   $_SESSION['ldapconf']['uri']);

  /* 
  Now we are testing for a group, with the rights :all 
  */
  
  $ldap->cd($ldapconf['base']);
  $ldap->search("(&(objectClass=gosaObject)(gosaSubtreeACL=:all))");

  $group_cnt  = $ldap->count();
  $data       = $ldap->fetch();

//  $str_there  = "Searching for Aminitrative users <br><br>";

  /* 
  We need to create administrative user and group  because theres no group found 
  */
  if($group_cnt < 1) {
    
    /* 
    Set var to create user 
    */
//    $str_there  =   "no group found<br>";

    $need_to_create_group = true;
    $need_to_create_user  = true;


    /* Output error */
    if(($withoutput)&&(!isset($_POST['new_admin']))){
      print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
    }
  } else {
    
//    $str_there = "Group found <br>".$data['dn'];    

    $need_to_create_group = false;
 
    $ldap->clearResult();
   
    /* We found an Administrative Group, is there a user, too */
    if(isset($data['memberUid'][0])) {
      $str = "uid=".$data['memberUid']['0'];
      $ldap->search("(&(objectClass=gosaAccount)(objectClass=person)(".$str."))");
      $data2   = $ldap->fetch();
  
      /* We must create a user */
      if (($ldap->count() < 1)||(!isset($data2))) {
//        $str_there.="Missing user";
        
        $need_to_create_user = true;
      
        if(($withoutput)&&(!isset($_POST['new_admin']))){
          print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
        }
      }else {
//        $str_there.="<br>User found <br>".$data2['dn'];
        $need_to_create_user = false;
      }
    } else {
      $need_to_create_user=true;
      if(($withoutput)&&(!isset($_POST['new_admin']))){
          print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
        }   
//      $str_there.="<br>No User found <br>";
    }
  }

  if(!($need_to_create_user&&$need_to_create_group))
    return(true);

  /* We need to create a new user with group */
  if(isset($_POST['new_admin']))
  {
  
    /* Adjust password attributes according to the samba version */
    if (isset($classes['samba3'])) {
      $samba= "2";
      $lmPassword = "lmPassword";
      $ntPassword = "ntPassword";
    } else {
      $samba= "3";
      $lmPassword = "sambaLMPassword";
      $ntPassword = "sambaNtPassword";
    }

    /* Nothing submitted */
    if(((empty($_POST['admin_name']))||(empty($_POST['admin_pass'])))) {
      return(true);
    }

    if($need_to_create_user) {
      /* We have the order to create an Admin */
      /* Define the user we are going to create */
      $dn= "cn=".$_POST['admin_name'].",".$ldapconf['peopleou'].",".$ldapconf['base'];
      $arr['objectClass'][0] ="person";
      $arr['objectClass'][1] ="organizationalPerson";
      $arr['objectClass'][2] ="inetOrgPerson";
      $arr['objectClass'][3] ="gosaAccount";
      $arr['uid']            = $_POST['admin_name'];
      $arr['cn']             = $_POST['admin_name'];
      $arr['sn']             = $_POST['admin_name'];
      $arr['givenName']      = "GOsa main administrator";
      $arr[$lmPassword]      = "10974C6EFC0AEE1917306D272A9441BB";
      $arr[$ntPassword]      = "38F3951141D0F71A039CFA9D1EC06378";
      $arr['userPassword']   = crypt_single($_POST['admin_pass'],"md5");
    

      if(!$ldap->dn_exists($dn)){ 
        $ldap->cd($dn); 
        $ldap->create_missing_trees($dn);
        $ldap->cd($dn);
        $ldap->add($arr);
        if($ldap->error!="Success"){
          print_red($ldap->error);
          print_red("Can't create user, and / or Group, possibly this problem  depends on an empty LDAP server. Check your configuration and try again!");
        }
      }    
    }

    /* There's already a group for administrator, so we only need to add the user */
    if(!$need_to_create_group) {
      if(!isset($data['memberUid'])) {
        $arrr['memberUid']= $_POST['admin_name'];
      } else {
        $data['memberUid'][$data['memberUid']['count']]=$_POST['admin_name'];
        $arrr['memberUid'] = $data['memberUid'];
        unset($arrr['memberUid']['count']);
  
        $tmp = array_reverse($arrr['memberUid']);    
        foreach($tmp as $tt){
          $tmp2[]=$tt;
        }
        $arrr['memberUid']= $tmp2;
//        $str_there="Group found<br>".$data['dn'];
      }

      $ldap->cd($data['dn']);
      $ldap->modify($arrr);

    } else {
      $dn                    = "cn=administrators,".$ldapconf['groupou'].",".$ldapconf['base'];
      $arrr['objectClass'][0]= "gosaObject";
      $arrr['objectClass'][1]= "posixGroup";
      $arrr['gosaSubtreeACL']= ":all";
      $arrr['cn']            = "administrators";
      $arrr['gidNumber']     = "999";
      $arrr['memberUid']     = $_POST['admin_name'];

      $ldap->cd($dn);
      $ldap->create_missing_trees($dn);
      $ldap->cd($dn);

      $ldap->add($arrr);
    }
    return(true);
  } else {

    if((!isset($create_user))||(!($create_user))) {
      $smarty->assign ("content", get_template_path('setup_useradmin.tpl'));
      $smarty->assign("exists",true);
    } else {
      $smarty->assign ("content", get_template_path('setup_useradmin.tpl'));
      $smarty->assign("exists",false);
    }

  }

  /* Smarty output */ 
  if($withoutput){
    $smarty->display (get_template_path('headers.tpl'));
  }
  if (isset($_SESSION['errors'])) {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  $smarty->assign("str_there",$str_there);
  if($withoutput){
    $smarty->display (get_template_path('setup.tpl'));
  }
  return(false);
}


/* Returns the classnames auf the mail classes */
function get_available_mail_classes()
{
  $dir = opendir( "../include");
  $methods = array();
  $suffix = "class_mail-methods-";
  $lensuf = strlen($suffix);
  $prefix = ".inc";
  $lenpre = strlen($prefix);

  $i = 0;
  while (($file = readdir($dir)) !== false){

    if(stristr($file,$suffix)) {
      $lenfile = strlen($file);
      $methods['name'][$i] = substr($file,$lensuf,($lenfile-$lensuf)-$lenpre);
      $methods['file'][$i] = $file;
      $methods[$i]['file'] = $file;
      $methods[$i]['name'] = substr($file,$lensuf,($lenfile-$lensuf)-$lenpre);
      $i++;
    }

  }

  return($methods);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
