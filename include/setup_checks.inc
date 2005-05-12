<?php

function minimum_version($vercheck)
{
  $minver = (int)str_replace('.', '', $vercheck);
  $curver = (int)str_replace('.', '', phpversion());

  if($curver >= $minver){
    return (true);
  }

  return (false);
}


function check_schema_version($description, $version)
{
  $desc= preg_replace("/^.* DESC\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $description);

  return preg_match("/\(v$version\)/", $desc);
}

function view_schema_check($table)
{
  $message="<table class=\"check\">";
  foreach ($table as $key => $msg){
    $message.= "<tr><td class=\"check\">$msg";
    if(strstr($msg,"enabled")) {
      $message.="</td><td style='text-align:center' ><img src=images/true.png alt='true' /></td></tr>";
    }
    else
    {
      $message.="</td><td style='text-align:center' ><img src=images/button_cancel.png alt='false' /></td></tr>";}
  }
  $message.="</table>";
  return $message;
}

function schema_check($server, $admin, $password,$aff=0)
{
  global $config;
  

  $messages= array();
  $required_classes= array(
      "gosaObject"            => array("version" => "2.1"),
      "gosaAccount"           => array("version" => "2.1.2"),
      "gosaLockEntry"         => array("version" => "2.1"),
      "gosaCacheEntry"        => array("version" => "2.1"),
      "gosaDepartment"        => array("version" => "2.1"),

      "goFaxAccount"          => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
      "goFaxSBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
      "goFaxRBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),

      "gosaUserTemplate"      => array("version" => "2.1", "class" => "posixAccount","file" => "nis.schema"),
      "gosaMailAccount"       => array("version" => "2.1", "class" => "mailAccount","file" => "gosa+samba3.schema"),
      "gosaProxyAccount"      => array("version" => "2.1.2", "class" => "proxyAccount","file" => "gosa+samba3.schema"),
      "gosaApplication"       => array("version" => "2.1", "class" => "appgroup","file" => "gosa.schema"),
      "gosaApplicationGroup"  => array("version" => "2.1", "class" => "appgroup","file" => "gosa.schema"),

      "GOhard"                => array("version" => "2.1", "class" => "terminals","file" => "goto.schema"),
      "gotoTerminal"          => array("version" => "2.0", "class" => "terminals","file" => "goto.schema"),
      "goServer"              => array("version" => "2.1","class" => "server","file" => "goserver.schema"),
      "goTerminalServer"      => array("version" => "2.1", "class" => "terminals","file" => "goto.schema"),
      "goNfsServer"           => array("version" => "2.1", "class" => "terminals","file" => "goto.schema"),
      "goNtpServer"           => array("version" => "2.1", "class" => "terminals","file" => "goto.schema"),
      "goSyslogServer"        => array("version" => "2.1", "class" => "terminals","file" => "goto.schema"),
      "goLdapServer"          => array("version" => "2.1"),
      "goCupsServer"          => array("version" => "2.1", "class" => array("posixAccount", "terminals"),),
      "goImapServer"          => array("version" => "2.1", "class" => array("mailAccount", "mailgroup"),"file" => "gosa+samba3.schema"),
      "goKrbServer"           => array("version" => "2.1"),
      "goFaxServer"           => array("version" => "2.1", "class" => "gofaxAccount","file" => "gofax.schema"),
       
        );

  /* Build LDAP connection */
  $ds= ldap_connect ($server);
  if (!$ds) {
    return (array(_("Can't bind to LDAP. No schema check possible!")));
  }
  ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
  $r= ldap_bind ($ds, $admin, $password);

  /* Get base to look for schema */
  $sr  = @ldap_read ($ds, "", "objectClass=*", array("subschemaSubentry"));
  $attr= @ldap_get_entries($ds,$sr);
  if (!isset($attr[0]['subschemasubentry'][0])){
    return (array(_("Can't get schema information from server. No schema check possible!")));
  }

  /* Get list of objectclasses */
  $nb= $attr[0]['subschemasubentry'][0];
  $objectclasses= array();
  $sr= ldap_read ($ds, $nb, "objectClass=*", array("objectclasses"));
  $attrs= ldap_get_entries($ds,$sr);
  if (!isset($attrs[0])){
    return (array(_("Can't get schema information from server. No schema check possible!")));
  }
  foreach ($attrs[0]['objectclasses'] as $val){
    $name= preg_replace("/^.* NAME\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $val);
    if ($name != $val){
      $objectclasses[$name]= $val;
    }
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
          $messages[$key]= sprintf(_("Optional objectclass '%s' required by plugin '%s' is not present in LDAP setup"), $key, $class);
        } else {
          if (!check_schema_version($objectclasses[$key], $value['version'])){
            $messages[$key]= sprintf(_("Optional objectclass '%s' required by plugin '%s' does not have version %s"), $key, $class, $value['version']);
          }else {
            if(!isset($affich2[$class])){
              $affich2[$class]="Support for <b>$class</b> enabled <td class=\"check\"> ".$value['file']."</td>";
            }
          }
        }

      }
    } else {
      /* Required class */
      if (!isset($objectclasses[$key])){
        $messages[$key]= sprintf(_("Required objectclass '%s' is not present in LDAP setup"), $key);
      } else {
        if (!check_schema_version($objectclasses[$key], $value['version'])){
          $messages[$key]= sprintf(_("Required objectclass '%s' does not have version %s"), $key, $value['version']);
        }
      }
    }
  }

  /* Check for correct samba parameters */
  if (!isset($objectclasses['sambaSamAccount'])){
    $messages['samba3']= _("<b>SAMBA 3</b> support disabled, no schema seems to be installed");
    $affich['samba3']= $messages['samba3']."<td class=\"check\">gosa+samba3.schema</td>";
  }else{$affich['samba3']=_("<b>SAMBA 3</b> support enabled<td class=\"check\">gosa+samba3.schema</td>");}

  if (!isset($objectclasses['sambaAccount'])){
    $messages['samba2']= _("<b>SAMBA 2</b> support disabled, no schema seems to be installed");
    $affich['samba2']=$messages['samba2']."<td class=\"check\">samba.schema</td>";
  }else{$affich['samba2']=_("<b>SAMBA 2</b> support enabled<td class=\"check\">samba.schema</td>");}

  /* Check pureftp/dns/ */
  if (!isset($objectclasses['PureFTPdUser'])){
    $messages['pureftp']= _("Support for <b>pureftp</b> disabled, no schema seems to be installed");
    $affich['pureftp']= $messages['pureftp']."<td class=\"check\">pureftpd.schema</td>";
  }else{$affich['pureftp']=_("Support for <b>pureftp</b> enabled<td class=\"check\">pureftpd.schema</td>");}

  if (!isset($objectclasses['gosaWebdavAccount'])){
    $messages['webdav']= _("Support for <b>WebDAV</b> disabled, no schema seems to be installed");
    $affich['webdav']=$messages['webdav']."<td class=\"check\"></td>";
  }else{$affich['webdav']=_("Support for <b>WebDAV</b> enabled<td class=\"check\">gosa+samba3.schema</td>");}

  if (!isset($objectclasses['phpgwAccount'])){
    $messages['phpgroupware']= _("Support for <b>phpgroupware</b> disabled, no schema seems to be installed");
    $affich['phpgroupware']=$messages['phpgroupware']."<td class=\"check\">phpgwaccount.schema</td>";
  }else{$affich['phpgroupware']=_("Support for <b>phpgroupware</b> enabled<td class=\"check\">phpgwaccount.schema</td>");}

  if (!isset($objectclasses['goFonAccount'])){
    $messages['phoneaccount']= _("Support for <b>gofon</b> disabled, no schema seems to be installed");
    $affich['phoneaccount']=$messages['phoneaccount']."<td class=\"check\">gofon.schema</td>";
  }else{$affich['phoneaccount']=_("Support for <b>gofon</b> enabled<td class=\"check\">gofon.schema</td>");}

  
  if(($_SESSION['ldapconf']['mail_methods'][$_SESSION['ldapconf']['mail']] == "kolab"))
  if(!isset($objectclasses['kolabInetOrgPerson']))
    {
    $messages['kolab']= _("Support for <b>Kolab</b> disabled, no schema seems to be installed, setting mail-method to cyrus");
    $tmp = array_flip($_SESSION['ldapconf']['mail_methods']);
    $_SESSION['ldapconf']['mail']=$tmp['cyrus'];
    $affich['kolab']=$messages['kolab']."<td class=\"check\"></td>";
  }else{$affich['kolab']=_("Support for <b>Kolab</b> enabled<td class=\"check\">gofon.schema</td>");}


  if($aff==0)return ($messages);
  else return(array_merge($affich,$affich2));


}



function check(&$faults, $message, $description, $test, $required= TRUE)
{
  $msg= "<table class='check'><tr><td class='check' style='font-size:14px;'>$message</td><td rowspan=2 style='vertical-align:middle; text-align:center;width:45px;'>";
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
  $msg.= "</td></tr><tr><td class='check' style='padding-left:20px;background-color:#F0F0F0;'>$description</td></tr></table><br>";

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
      minimum_version('4.1.0'));

  $msg.= check (	$faults, _("Checking if register_globals is set to 'off'"),
      _("register_globals is a PHP mechanism to register all global varibales to be accessible from scripts without changing the scope. This may be a security risk. GOsa will run in both modes."),
      $check_globals == 0, FALSE);

  $msg.= check (	$faults, _("Checking for ldap module"),
      _("This is the main module used by GOsa and therefore really required."),
      function_exists('ldap_bind'));

  $msg.= check (	$faults, _("Checking for gettext support"),
      _("Gettext support is required for internationalized GOsa."), function_exists('bindtextdomain'));

  $msg.= check (	$faults, _("Checking for mhash module"),
      _("To use SSHA encryption, you'll need this module. If you are just using crypt or md5 encryption, ignore this message. GOsa will run without it."),
      function_exists('mhash'), FALSE);

  $msg.= check (	$faults, _("Checking for imap module"),
      _("The IMAP module is needed to communicate with the IMAP server. It gets status informations, creates and deletes mail users."),
      function_exists('imap_open'));
  $msg.= check (	$faults, _("Checking for getacl in imap"),
      _("The getacl support is needed for shared folder permissions. The standard IMAP module is not capable of reading acl's. You need a recend PHP version for this feature."),
      function_exists('imap_getacl'), FALSE);
  $msg.= check (	$faults, _("Checking for mysql module"),
      _("MySQL support is needed for reading GOfax reports from databases."),
      function_exists('mysql_query'), FALSE);
  $msg.= check (	$faults, _("Checking for cups module"),
      _("In order to read available printers from IPP protocol instead of printcap files, you've to install the CUPS module."),
      function_exists('cups_get_dest_list'), FALSE);
  $msg.= check (	$faults, _("Checking for kadm5 module"),
      _("Managing users in kerberos requires the kadm5 module which is downloadable via PEAR network."),
      function_exists('kadm5_init_with_password'), FALSE);
  return ($msg);
}


function perform_additional_checks(&$faults)
{
# Programm check
  $msg= "<h1>"._("Checking for some additional programms")."</h1>";

# Image Magick
  $query= "LC_ALL=C LANG=C convert -help";
  $output= shell_exec ($query);
  if ($output != ""){
    $lines= split ("\n", $output);
    $version= preg_replace ("/^Version:[^I]+ImageMagick ([^\s]+).*/", "\\1", $lines[0]);
    list($major, $minor)= split("\.", $version);
    $msg.= check (	$faults, _("Checking for ImageMagick (>=5.4.0)"),
        _("ImageMagick is used to convert user supplied images to fit the suggested size and the unified JPEG format."),
        ($major > 5 || ($major == 5 && $minor >= 4)));
  } else {
    $msg.= check (	$faults, _("Checking imagick module for PHP"),
        _("Imagick is used to convert user supplied images to fit the suggested size and the unified JPEG format from PHP script."), function_exists('imagick_blob2image'), TRUE);
  }

# Check for fping
  $query= "LC_ALL=C LANG=C fping -v 2>&1";
  $output= shell_exec ($query);
  $have_fping= preg_match("/^fping:/", $output);
  $msg.= check (	$faults, _("Checking for fping utility"),
      _("The fping utility is only used if you've got a thin client based terminal environment running."),
      $have_fping, FALSE);

# Check for smb hash generation tool
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
# checking for some PHP.ini Options

/* seesio.auto_start should be off, in order to without trouble*/

$arra = ini_get_all();

/* This array contains folling info now : 
  global_value  0
  local_value   0
  access        7               

-->Access types
PHP_INI_USER      1          Entry can be set in user scripts
PHP_INI_PERDIR    2          Entry can be set in php.ini, .htaccess or httpd.conf 
PHP_INI_SYSTEM    4          Entry can be set in php.ini or httpd.conf 
PHP_INI_ALL       7          Entry can be set anywhere 

*/

$session_auto_start =     ($arra['session.auto_start']);
$implicit_flush     =     ($arra['implicit_flush']);
$max_execution_time =     ($arra['max_execution_time']);
$memory_limit =           ($arra['memory_limit']);
$expose_php =             ($arra['expose_php']);
$magic_quotes_gpc =       ($arra['magic_quotes_gpc']);
$register_globals =       ($arra['register_globals']);


// auto_register
$msg.= check (  $faults, _("PHP.ini check -> session.auto_register"),
      _("In Order to use GOsa without any trouble, the session.auto_register option in your php.ini musst be 'Off'."),
      (!$session_auto_start['local_value']));



//implicit_flush
$msg.= check (  $faults, _("PHP.ini check -> implicit_flush"),
      _("This Option defines the Ouput handling, turn this Option off, to increase performance."),
        !$implicit_flush['local_value'],0,false);

//max_execution_time
if($max_execution_time['local_value'] < 30 )
  $max_execution_time['local_value']=false;
$msg.= check (  $faults, _("PHP.ini check -> max_execution_time"),
      _("The Execution time, should be 30 seconds minimun, cause some actions will need huge ammount of time ."),
        $max_execution_time['local_value'],0,false);

//memory_limit
if($memory_limit['local_value'] < 8 )
  $memory_limit['local_value']=false;
$msg.= check (  $faults, _("PHP.ini check -> memory_limit"),
      _("GOsa need at least 8M of memory, less will cause unpredictable errors, sometimes without error message!. Best would be 32 M here."),
        !$implicit_flush['local_value'],0,false);

//expose_php
$msg.= check (  $faults, _("PHP.ini check -> expose_php"),
      _("PHP won't send any Information about the Server you are running, should be a security fact."),
        !$implicit_flush['local_value'],0,false);

//magic_quotes_gpc
$msg.= check (  $faults, _("PHP.ini check -> magic_quotes_gpc"),
      _("Security option, php will escape all quotes in strings ."),
        $magic_quotes_gpc['local_value'],0,false);

  return $msg;
}



//! Added by Hickert
//
// Parse /contrib/gosa.conf to set user defined values
//This function may create the ldap.conf
// Lets try
function parse_contrib_conf()
{
  /* First gather all needed informations */



  /* Variables */
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

  if(isset($classes['samba3']))  // means Samba 3 is disabled
    $used_samba_version = 2;
  else
    $used_samba_version = 3;


  if(file_exists("/usr/lib/gosa/mkntpasswd"))    {
    $pwdhash  = "/usr/lib/gosa/mkntpasswd";
  }
  elseif (preg_match("/^Usage: mkntpwd /", shell_exec ("mkntpwd 2>&1")))    {
    $pwdhash= "mkntpwd";
  } else {
    $pwdhash=addslashes('  perl -MCrypt::SmbHash -e "ntlmgen \"\$ARGV[0]\", \$lm, \$nt; print \"\${lm}:\${nt}\n\";" $1');
    //    $pwdhash= 'perl -MCrypt::SmbHash -e \"ntlmgen \\"\\$ARGV[0]\\", \\$lm, \\$nt; print \\"\\${lm}:\\${nt}\\\";\"'; 
  }


  // Define which variables will be replaced
  $replacements['{LOCATIONNAME}']           =   $ldapconf['location'];
  $replacements['{SAMBAVERSION}']           =   $used_samba_version;
  $replacements['{LDAPBASE}']               =   $ldapconf['base'];
  $replacements['{LDAPADMIN}']              =   $ldapconf['admin'];
  $replacements['{DNMODE}']                 =   $ldapconf['peopledn'];
  $replacements['{LOCALHOST}']              =   $ldapconf['uri'];
  $replacements['{PASSWORD}']               =   $ldapconf['password'];
  $replacements['{CRYPT}']                  =   $ldapconf['arr_cryptkeys'][$ldapconf['arr_crypts']];
  $replacements['{SID}']                    =   "";
  $replacements['{RIDBASE}']                =   "";
  $replacements['{MAILMETHOD}']             =   $ldapconf['mail_methods'][$ldapconf['mail']];
  $replacements['{SMBHASH}']                =   $pwdhash;
  $replacements['{GOVERNMENTMODE}']         =   "false"; 




  // This array contains all preg_replace syntax to delete all unused plugins
  // THE kEY MUST BE THE CLASSNAME so we can check it with $ldapconf['classes']

  $possible_plugins['fonreport'][]      ="'\n.*<plugin.*fonreport+.*\n.*>.*\n'i";
  $possible_plugins['phoneaccount'][]   ="'\n.*<tab.*phoneAccount.*>.*\n'i";

  $possible_plugins['logview'][]        ="'\n.*<plugin.*logview+.*\n.*>.*\n'i";

  $possible_plugins['pureftp'][]        ="'\n.*<tab.*pureftp.*>.*\n'i";

  $possible_plugins['webdav'][]         ="'\n.*<tab.*webdav.*>.*\n'i";

  $possible_plugins['phpgroupware'][]   ="'\n.*<tab.*phpgroupware.*>'i";


  // Header information
  // Needed to send the generated gosa.conf to the browser
  header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
  header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
  header("Cache-Control: no-cache");
  header("Pragma: no-cache");
  header("Cache-Control: post-check=0, pre-check=0");
  header("Content-type: text/plain");

  if (preg_match('/MSIE 5.5/', $_SERVER['HTTP_USER_AGENT']) ||  preg_match('/MSIE 6.0/', $_SERVER['HTTP_USER_AGENT']))
  {
    header('Content-Disposition: filename="gosa.conf"');
  } else {
    header('Content-Disposition: attachment; filename="gosa.conf"');
  }


  if(!$fp=fopen(CONFIG_TEMPLATE_DIR."/gosa.conf","r"))
  {
    echo "Can't open file ".CONFIG_TEMPLATE_DIR."/gosa.conf";
    // Don't write anything else
  }
  else
  {
    // Read out Data .....  
    while(!feof($fp))
    {
      $str.= fread($fp,512);
    }


    // Lets check which samba version we will use

    // in case of samba 2 we don't need to add additional objets in gosa.conf
    // in case of samba 3 we musst detect if theres an objectType = SambaObjekt defined
    //   if theres is one, then do nothing, because the setup will detect the the SID themself
    //   if theres none defined add SID and RIDBASE to gosa.conf


    if($used_samba_version == 2)
    {
      // Do nothing ...
    }
    else
    {
      // Create LDAP connection, to check if theres a domain Objekt definen in the Ldap scheme
      $ldap= new LDAP($ldapconf['admin'], $ldapconf['password'], $ldapconf['uri']);


      // Try to find a Samba Domain Objekt
      $ldap->search("(objectClass=sambaDomain)");

      // Something found ??? so we need to define ridbase an SID by ourselfs
      if($ldap->count()< 1)
      {
        $replacements['{SID}']                  =   "sid=\"123412-11\"";
        $replacements['{RIDBASE}']              =   "ridbase=\"1000\"";  
      } 
    }// else --> $used_samba_version == 2

    // Data readed, types replaced, samba version detected and checked if we need to add SID and RIDBASE 


    // Check if there is an ivbbEntry in the LDAP tree, in this case we will set the governmentmode to true
    // Create LDAP connection, to check if theres a domain Objekt definen in the Ldap scheme

    if(!isset($ldap))
      $ldap= new LDAP($ldapconf['admin'], $ldapconf['password'], $ldapconf['uri']);


    // Try to find a Samba Domain Objekt
    $ldap->search("(objectClass=ivbbEntry)");

    // Something found ??? so we need to define ridbase an SID by ourselfs
    if($ldap->count()> 0)
    {
      $replacements['{GOVERNMENTMODE}']                  =   "true";
    }


    // Replace all colleted information with placeholder
    foreach($replacements as $key => $val)
    {
      $str = preg_replace("/".$key."/",$val,$str);
      //      $str = ereg_replace($key,$val,$str);
    }    

    // Remove all unused plugins
    foreach($possible_plugins as $plugin)
    {
      foreach($plugin as $key=>$val)
      {
        if(in_array($plugin,$classes))
        {
          $str = preg_replace($val,"\n",$str);
        }
      }
    }


  }// else -->  !$fp=fopen("../contrib/gosa.conf","r")

  return ( $str);
}


// This ist the first page shown in setup
// This page test some packages, like php version, ldap_module aso
// The funtion don't save anything, it tests only, when withoutput = false
// (called from setup.php);
function show_setup_page1($withoutput = true)
{
  $smarty = get_smarty();  

  $smarty->assign ("content", get_template_path('setup_introduction.tpl'));
  $smarty->assign ("tests", perform_php_checks($faults));



  // This var is true if there is anything went wrong
  if ($faults)
  {
    $smarty->assign("mode", "disabled");
  }

  // This line displays the template only if (withoutput is set)
  if($withoutput) 
    $smarty->display (get_template_path('headers.tpl'));

  if (isset($_SESSION['errors']))
  {
    $smarty->assign("errors", $_SESSION['errors']);
  }

  if($withoutput)
    $smarty->display (get_template_path('setup.tpl'));

  return (!$faults);
}







/* Shows Setup_page 2*/
function show_setup_page2($withoutput = true)
{
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
  return (!$faults);                               
}


/* Setup page 3 asks for the server address
 "Now we're going to include your LDAP server and create an initial configuration"*/
function show_setup_page3($withoutput = true)
{

  // Take the Post oder the Sessioin saved data  
  if(isset($_POST['uri']))
    $uri = $_POST['uri'];
  elseif(isset($_SESSION['ldapconf']['uri']))
    $uri = $_SESSION['ldapconf']['uri'];

  // If Page called first time, field is empty 
  if((!isset($uri))||(empty($uri)))
    $uri = "ldap://localhost:389";


  $smarty = get_smarty();

  // if isset $uri save it to session
  if(isset($uri))
  {
    $_SESSION['ldapconf']['uri'] = $uri;
    $smarty->assign ("uri", validate($uri));
  }


  // No error till now
  $fault = false;


  // If we pushed the Button continue
  if(isset($_POST['continue3']))
    if(!isset($uri))
    {
      $fault = true;
      // Output the Error
      if($withoutput)
      {
        print_red (_("You've to specify an ldap server before continuing!"));
        $smarty->assign ("content", get_template_path('setup_step3.tpl'));
      }
    }
  elseif (!$ds = @ldap_connect (validate($uri)))
  {
    $fault =true;
    // Output the Error
    if($withoutput)
    {
      print_red (_("Can't connect to the specified LDAP server! Please make sure that is reachable for GOsa."));
      $smarty->assign ("uri", validate($uri));
      $smarty->assign ("content", get_template_path('setup_step3.tpl'));
    }
  }
    else
    {
      // Try to bind the connection     
      ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);

      // if we can't bind , print error
      if (!$r  =  @ldap_bind ($ds))
      {
        $fault = true;
        // Output the Error
        if($withoutput)      
        {
          print_red (_("Can't bind to the specified LDAP server! Please make sure that is reachable for GOsa."));
          $smarty->assign ("content", get_template_path('setup_step3.tpl'));
          $smarty->assign ("uri", validate($uri));
        }
      }
      else
      {
        $fault = false;
      }
    }


  $smarty->assign ("content", get_template_path('setup_step3.tpl'));


  // Load Header
  if($withoutput)
    $smarty->display (get_template_path('headers.tpl'));

  // Set Errors to Smarty
  if (isset($_SESSION['errors']))
  {
    $smarty->assign("errors", $_SESSION['errors']);
  }

  // Print out Template 
  if($withoutput)
    $smarty->display (get_template_path('setup.tpl'));



  return (!$fault);                             
}



// Setup page 4
// This page asked for detailed info, like base dn or admin user
// if evrything is ok , but there's a missing user with ACL :all we show a a user creation page before we show page 5
function show_setup_page4($withoutput = true)
{


  require_once("class_password-methods.inc");

  error_reporting(E_ALL);




  $fault        = false;                        // If an error occures we set this var to true 
  $smarty       = get_smarty();                 // Our smarty instance
  $uri          = $_SESSION['ldapconf']['uri']; // This is the the connect path to the ldapserver like ldap://lo..
  $ldapconf     = $_SESSION['ldapconf'];        // The ldap Configuration informations, we collected while setup 
  $arr_crypts   = array();                      // array which includes contains all possible password crypting methods
  $temp         = "";                           // Temp
  $checkvars    = array("location","admin","password","peopleou","peopledn","arr_crypts","mail","uidbase");


  if(!isset($_SESSION['ldapconf']['arr_cryptkeys']))
  {
    require_once("class_password-methods.inc");
    $tmp = passwordMethod::get_available_methods_if_not_loaded();
    $_SESSION['ldapconf']['arr_cryptkeys']= $tmp['name'];
  }



  if(!isset($_SESSION['ldapconf']['mail_methods']))
  {
    $_SESSION['ldapconf']['mail_methods']=array();
    $temp = get_available_mail_classes();
    $_SESSION['ldapconf']['mail_methods']= $temp['name'];
  }




  // If there are some empty vars in ldapconnect
  // This values also represent out default values 

# first try to get $base 
  if(!$ds = ldap_connect (validate($uri)))    
  {
    $fault = true;
  }
  elseif(!ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3))    
  {
    $fault = true;
  }
  elseif(! $r =  ldap_bind ($ds))    
  {
    $fault = true;
  }
  else
  {
    $sr=      ldap_search ($ds, "", "objectClass=*", array("namingContexts"));


    $attr=    ldap_get_entries($ds,$sr);
    if((empty($attr)))
    {
      $base= "dc=example,dc=net";


      if($withoutput)
        print_red(_("Bind to server successfull, but the server seems to be completly empty, please check all informations twice"));

    }
    else
    {
      $base= $attr[0]['dn'];
    }
  }



  if(!isset($_SESSION['ldapconf']['base']))           $_SESSION['ldapconf']['base']         = $base;
  if(!isset($_SESSION['ldapconf']['admin']))          $_SESSION['ldapconf']['admin']        = "cn=ldapadmin,".$base;
  if(!isset($_SESSION['ldapconf']['peopleou']))       $_SESSION['ldapconf']['peopleou']     = "ou=people";
  if(!isset($_SESSION['ldapconf']['groupou']))        $_SESSION['ldapconf']['groupou']      = "ou=groups";
  if(!isset($_SESSION['ldapconf']['peopledn']))       $_SESSION['ldapconf']['peopledn']     = "cn";
  if(!isset($_SESSION['ldapconf']['password']))       $_SESSION['ldapconf']['password']     = "";
  if(!isset($_SESSION['ldapconf']['location']))       $_SESSION['ldapconf']['location']     = "Example";
  if(!isset($_SESSION['ldapconf']['uidbase']))        $_SESSION['ldapconf']['uidbase']      = "1000";
  if(!isset($_SESSION['ldapconf']['mail']))           $_SESSION['ldapconf']['mail']         = 0;
  $tmp = array_flip($_SESSION['ldapconf']['arr_cryptkeys']);
  if(!isset($_SESSION['ldapconf']['arr_crypts']))     $_SESSION['ldapconf']['arr_crypts']   = $tmp['md5'];


  // check Post data

  if(isset($_POST['check']))
  {
    // Check if all needed vars are submitted
    foreach($checkvars as $key)
    {
      if((isset($_POST[$key]))&&($_POST[$key]!=""))
      {
        $_SESSION['ldapconf'][$key] = $_POST[$key];
      }
      else
      {
        if($withoutput)
        {
          print_red(sprintf(_("You're missing the required attribute '%s' from this formular. Please complete!"), $key));
        }
        $fault = true;
      }
    }

    // check if another base is given ... (ldapadmin...dc=base,dc=de)   ..

    $base     =   $_SESSION['ldapconf']['admin'];
    $tmp      =   array_reverse ( split(",",$base));
    $base     =   $tmp[1].",".$tmp[0];
    $_SESSION['ldapconf']['base']         = $base;


  }



  $smarty->assign("arr_cryptkeys",$_SESSION['ldapconf']['arr_cryptkeys']);
  $smarty->assign("mail_methods", $_SESSION['ldapconf']['mail_methods']);

  foreach($_SESSION['ldapconf'] as $key => $val)
  {
    $smarty->assign($key,$val);
  }

  if(isset($_POST['check']))
  {
    $ldap= new LDAP($_SESSION['ldapconf']['admin'], $_SESSION['ldapconf']['password'], $_SESSION['ldapconf']['uri']);
  
    if($withoutput){
    $m= schema_check($_SESSION['ldapconf']['uri'], $_SESSION['ldapconf']['admin'], $_SESSION['ldapconf']['password']);
    $_SESSION['classes']= $m;
    }

    if ($ldap->error != "Success")
    {
      if($withoutput)
      {
        print_red(sprintf(_("Can't log into LDAP server. Reason was: %s."), $ldap->get_error()));
      }
      $fault = true;
    }
  }



  // Set smarty output
  $smarty->assign ("content", get_template_path('setup_step4.tpl'));

  $smarty->assign ("peopledns", array("cn", "uid"));
  if($withoutput)
    $smarty->display (get_template_path('headers.tpl'));

  if(isset($_SESSION['errors']))
  {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput)
    $smarty->display (get_template_path('setup.tpl'));


  return (!$fault);


}




// This page shows your configuration 
// and wants you to download the gosa.conf ....
function show_setup_page5($withoutput=true)
{
  // Get ldapconf
  $ldapconf= $_SESSION['ldapconf'];

  // get smarty
  $smarty = get_smarty();

  $classes = $_SESSION['classes'];

  $info= posix_getgrgid(posix_getgid());
  $smarty->assign ("webgroup", $info['name']);
  $smarty->assign ("path", CONFIG_DIR);
  $message = "";
  $message.="<table class=\"check\">";
  $m= schema_check($ldapconf['uri'], $ldapconf['admin'], $ldapconf['password'],1);
  
  if($withoutput)
  {
    $smarty->assign ("schemas", view_schema_check($m));
    $smarty->assign ("content", get_template_path('setup_finish.tpl'));
  }
  // Output templates ....

  if($withoutput)
    $smarty->display (get_template_path('headers.tpl'));

  if (isset($_SESSION['errors']))
  {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput)
    $smarty->display (get_template_path('setup.tpl'));
  return(true);
}












// this function is called by setup step 5, in order to create a missinf Administrator 
// and or Administrational user
// on success go on with setup_page5
// else show this page aggain
function create_user_for_setup($withoutput=true)
{

  error_reporting(E_ALL);

  global $samba;

  $ldapconf = $_SESSION['ldapconf'];
  $smarty = get_smarty();

  $classes= $_SESSION['classes'];

  // Everything runns perfect ...
  // So we do a last test on this page
  // is there a user with ACLs :all which will be able to adminsitrate GOsa
  // We check that, if this user or group is missing we ask for creating them

  $ldap= new LDAP($_SESSION['ldapconf']['admin'], $_SESSION['ldapconf']['password'], $_SESSION['ldapconf']['uri']);

  //  $ldap= new LDAP($ldapconf['admin'], $ldapconf['password'], $ldapconf['uri']);

  // Now we are testing for a group, with the rights :all
  $ldap->cd($ldapconf['base']);
  $ldap->search("(&(objectClass=gosaObject)(gosaSubtreeACL=:all))");

  $group_cnt       = $ldap->count();
  $data           = $ldap->fetch();
  $create_user    = false;

  // We need to create Administrative user and group
  // Because theres no Group found
  if($group_cnt < 1)
  {
    // Set var to create user
    $create_user    =   true;

    // Output error
    if(($withoutput)&&(!isset($_POST['new_admin'])))
      print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
  }
  else
  {

    // We found an Administrative Group, is there a user too
    if(isset($data['memberUid'][0]))
    {
      $ldap->search("(&(objectClass=gosaAccount)(objectClass=person))",array("uid=".$data['memberUid'][0]));
      $data2      = $ldap->fetch();
      $user_cnt   = $ldap->count();
    }

    // We must create a user
    if (($ldap->count() < 1)||(!isset($data2)))
    {
      $create_user = true;
      if(($withoutput)&&(!isset($_POST['new_admin'])))
        print_red(_("You're missing an administrative account for GOsa, you'll not be able to administrate anything!"));
    }
    else
    {
      // We don't need to add a user
      return(true);
    }

  }// if($group_cn)

  // We need to create a new user with group
  if(isset($_POST['new_admin']))
  {
    // Is there a running user ?
    // Then add additional

    if (isset($classes['samba3']))
    {
      $samba= "2";
      $lmPassword = "lmPassword";
      $ntPassword = "ntPassword";
    } else {
      $samba= "3";
      $lmPassword = "sambaLMPassword";
      $ntPassword = "sambaNtPassword";
    }


    // Nothing submitted
    if(( (empty($_POST['admin_name']))||(empty($_POST['admin_pass'])) )&&(!$create_user))
    {
      return(true);
    }

    // We have the order to create on Admin ^^
    // Detect Samba version to define the Attribute names shown below
    // go to base
    $ldap->cd($ldapconf['base']);

    // Define the user we are going to create
    $dn =  "cn=".$_POST['admin_name'].",".$ldapconf['peopleou'].",".$ldapconf['base'];


    $arr['objectClass'][0] ="person";
    $arr['objectClass'][1] ="organizationalPerson";
    $arr['objectClass'][2] ="inetOrgPerson";
    $arr['objectClass'][3] ="gosaAccount";
    $arr['uid']            = $_POST['admin_name'];
    $arr['cn']             = $_POST['admin_name'];
    $arr['sn']             = $_POST['admin_name'];

    $arr['givenName']     = "GOsa main administrator";
    $arr[$lmPassword]     = "10974C6EFC0AEE1917306D272A9441BB";
    $arr[$ntPassword]     = "38F3951141D0F71A039CFA9D1EC06378";
    $arr['userPassword']  = crypt_single($_POST['admin_pass'],"md5");
    if( ! $ldap->dn_exists ( $dn )) { 
      $ldap->cd($dn); 
      $ldap->create_missing_trees($dn);
      $ldap->add($arr);
      if($ldap->error!="Success")      {
        print_red("Can't create user, and / or Group, possibly this problem  depends on an empty LDAP server. Check your configuration and try again!");
      }
    }

    // theres already a group for administrator, so we only need to add the user
    if($group_cnt)
    {
      if(!isset($data['memberUid']))
      {
        $arrr['memberUid']= $_POST['admin_name'];
      }
      else
      {
        $data['memberUid'][$data['memberUid']['count']]=$_POST['admin_name'];
        $arrr['memberUid'] = $data['memberUid'];
        unset($arrr['memberUid']['count']);
      }
      $ldap->cd($data['dn']);
      $ldap->modify($arrr);
    }
    else
    {
      // there was no group defined, so we must create one
      $dn                       = "cn=administrators,".$ldapconf['groupou'].",".$ldapconf['base'];
      $arrr['objectClass'][0]   = "gosaObject";
      $arrr['objectClass'][1]   = "posixGroup";
      $arrr['gosaSubtreeACL']   = ":all";
      $arrr['cn']               = "administrators";
      $arrr['gidNumber']        = "999";
      $arrr['memberUid']        = $_POST['admin_name'];
      $ldap->cd($dn);
      $ldap->add($arrr);
    }


    // We created the Group and the user, so we can go on with the next setup step
    return(true);
  }
  else
  {
    if(!($create_user))
    {
      $smarty->assign ("content", get_template_path('setup_useradmin.tpl'));
      $smarty->assign("exists",true);
    }
    else
    {
      $smarty->assign ("content", get_template_path('setup_useradmin.tpl'));
      $smarty->assign("exists",false);
    }
  }


  // Smarty outout  

  if($withoutput)
    $smarty->display (get_template_path('headers.tpl'));

  if (isset($_SESSION['errors']))
  {
    $smarty->assign("errors", $_SESSION['errors']);
  }
  if($withoutput)
    $smarty->display (get_template_path('setup.tpl'));


  return(false);
}


// Returns the classnames auf the mail classes
function get_available_mail_classes()
{
  $dir = opendir( "../include");
  $methods = array();
  $suffix = "class_mail-methods-";
  $lensuf = strlen($suffix);
  $prefix = ".inc";
  $lenpre = strlen($prefix);


  $i = 0;
  while (($file = readdir($dir)) !== false) 
  {
    if(stristr($file,$suffix))
    {
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
