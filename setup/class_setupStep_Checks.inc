<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

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


class Step_Checks extends setup_step
{
  var $basic_checks = array();
  var $config_checks= array();
  var $is_writeable = array();
  var $header_image = "images/system.png";

  function Step_Checks()
  {
    $this->update_strings();
  }
 

  function update_strings()
  {
    $this->s_title      = _("Installation check");
    $this->s_title_long = _("Installation check");
    $this->s_info       = _("Basic checks for PHP version and required extensions.");
  }
  
  
  /* Execute and display template */ 
  function execute()
  {
    $this->run_checks();
    $smarty = get_smarty();
    $smarty->assign("basic"       ,$this->basic_checks);
    $smarty->assign("config"      ,$this->config_checks);
    $smarty->assign("is_writeable",$this->is_writeable);
    return($smarty->fetch(get_template_path("setup_checks.tpl",TRUE,dirname(__FILE__))));
  }


  /* Execute all checks */ 
  function run_checks()
  {
    $this->basic_checks = array();
    $this->config_checks = array();
    $this->is_writeable = array();
 
    /* PHP version check */
    $N = _("Checking PHP version"); 
    $D = sprintf(_("PHP must be of version %s or above."), "5.2.0");
    $S = _("GOsa requires functionality that is not available (or buggy) in older PHP versions. Please update to a supported version.");
    $R = $this->check_php_version();
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checking imap extension */
    $N = _("Checking for LDAP support");
    $D = _("This is the main extension used by GOsa and therefore really required.");
    $S = _("The ldap extension (php5-ldap) is required to communicate with your LDAP server.");
    $R = is_callable("ldap_bind");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Check for gettext support */
    $N = _("Checking for gettext support");
    $D = _("Gettext support is required for internationalization.");
    $S = _("Please make sure that the extension is activated.");
    $R = is_callable("bindtextdomain");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checing for iconv */
    $N = _("Checking for iconv support");
    $D = _("This module is used by GOsa to convert samba munged dial informations and is therefore required. ");
    $S = _("Please make sure that the extension is activated.");
    $R = is_callable("iconv");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Check for installed mhash module */
    $N = _("Checking for mhash support");
    $D = _("You'll need this module to make use of SSHA encryption");
    $S = _("The mhash module for PHP 5 is not available.Please install php5-mhash.");
    $R = is_callable("mhash");
    $M = FALSE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Is imap module available */
    $N = _("Checking for IMAP support");
    $D = _("The IMAP module is needed to communicate with the IMAP server. GOsa retrieves status information, creates and deletes mail users, etc.");
    $S = _("This module is used to communicate with your mail server. Please install php5-imap.");
    $R = is_callable("imap_open");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Is imap module available */
    $N = _("Checking for multi byte support");
    $D = _("The multi byte string support is required by some plugins.");
    $S = _("Please compile php with enabled multi byte support '--enable-mbstring'.");
    $R = is_callable("mb_strlen");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Check getacl in imap module */
    $N = _("Checking for getacl in IMAP implementation");
    $D = _("The getacl support is needed to handle shared folder permissions. Old IMAP extensions are not capable of reading acl's. You need a recent PHP version to use this feature.");
    $S = $D;
    $R = is_callable("imap_getacl");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Mysql module loaded ? */
    $N = _("Checking for MySQL support");
    $D = _("MySQL support is needed to communicate with several supported databases.");
    $S = _("This module is required to communicate with database servers (GOfax, asterisk, GLPI, etc.). Please install php5-mysql");
    $R = is_callable("mysql_query");
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checking for kadm5 module */
    $N = _("Checking for kadm5 support");
    $D = _("Managing users in kerberos requires the kadm5 module which is downloadable via PEAR network.");
    $S = _("This module is required to manage user in kerberos, it is downloadable via PEAR network");
    $R = is_callable("kadm5_init_with_password");
    $M = FALSE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Snmp module available ? */
    $N = _("Checking for SNMP support");
    $D = _("The simple network management protocol is needed to get status information from clients.");
    $S = _("This module is required for client monitoring. Please install php5-snmp.");
    $R = is_callable("snmpget");
    $M = FALSE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checking for Cups module */
    $N = _("Checking for CUPS support");
    $D = _("In order to read available printers via the IPP protocol instead of printcap files, you've to install the CUPS module.");
    $S = $D; 
    $R = is_callable("cups_get_dest_list");
    $M = FALSE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checking for F ping utility */
    $query= "LC_ALL=C LANG=C fping -v 2>&1";
    $output= shell_exec ($query);
    $N = _("Checking for fping utility");
    $D = _("The fping utility is used if you've got a thin client based terminal environment.");
    $S = _("The fping utility is only used in thin client based terminal environment.");
    $R = preg_match("/^fping:/", $output);
    $M = FALSE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Checking generate LM/NT password hashes */
    $query= "mkntpwd 2>&1";
    $output= shell_exec ($query);
    $have_mkntpwd= preg_match("/^Usage: mkntpwd /", $output);
    if (!$have_mkntpwd){
      $query= 'LC_ALL=C LANG=C perl -MCrypt::SmbHash -e "print join(q[:], ntlmgen $ARGV[0]), $/;" &>/dev/null';
      system ($query, $ret);
      $have_mkntpwd |= ($ret == 0);
    }

    $N = _("SAMBA password hash generation");
    $D = _("In order to use SAMBA 2/3 passwords, you've to install additional packages to generate password hashes.");
    $S = _("In order to use SAMBA 2/3 you've to install additional perl libraries. Take a look at mkntpasswd.");
    $R = $have_mkntpwd;
    $M = TRUE;
    $this->basic_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );


    /* PHP Configuration checks 
     */

    /* Register_globals off */
    $N = "register_globals = <b>"._("Off")."</b>";
    $D = _("register_globals is a PHP mechanism to register all global variables to be accessible from scripts without changing the scope. This may be a security risk.");
    $S = _("Search for 'register_globals' in your php.ini and switch it to 'Off'.");
    $R = ini_get("register_globals") == 0;
    $M = FALSE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );


    /* session lifetime set to >=86400 seconds ? */
    $N = "session.gc_maxlifetime &gt;= <b>86400</b>";
    $D = _("PHP uses this value for the garbage collector to delete old sessions.")." ".
         _("Setting this value to one day will prevent loosing session and cookies before they really timeout.");
    $S = _("Search for 'session.gc_maxlifetime' in your php.ini and set it to 86400 or higher.");
    $R = ini_get("session.gc_maxlifetime") >= 86400;
    $M = FALSE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Session auto start must be turned off */
    $session_auto_start = ini_get('session.auto_start');
    $N = "session.auto_start = <b>"._("Off")."</b>";
    $D = _("In Order to use GOsa without any trouble, the session.auto_register option in your php.ini should be set to 'Off'.");
    $S = _("Search for 'session.auto_start' in your php.ini and set it to 'Off'.");
    $R = !$session_auto_start['local_value'];
    $M = TRUE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Check if memory limit is set to 32 or > */
    $N = "memory_limit &gt;= <b>32</b>";
    $D = _("GOsa needs at least 32MB of memory. Setting it below this limit may cause errors that are not reproducable! Increase it for larger setups.");
    $S = _("Search for 'memory_limit' in your php.ini and set it to '32M' or higher.");
    $R = ini_get('memory_limit') >= 32 ;
    $M = TRUE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Implicit Flush disabled can increase performance */
    $N = "implicit_flush = <b>"._("Off")."</b>";
    $D = _("This option influences the PHP output handling. Turn this Option off, to increase performance.");
    $S = _("Search for 'implicit_flush' in your php.ini and set it to 'Off'."); 
    $R = !ini_get('implicit_flush');
    $M = FALSE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Check if execution time is set to 30 */
    $N = "max_execution_time &gt;= <b>30</b>";
    $D = _("The Execution time should be at least 30 seconds.");
    $S = _("Search for 'max_execution_time' in your php.ini and set it to '30' or higher.");
    $R = ini_get("max_execution_time") >= 30 ;
    $M = TRUE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Expose php should be set to off */
    $N = "expose_php = <b>"._("Off")."</b>";
    $D = _("Increase the server security by setting expose_php to 'off'. PHP won't send any information about the server you are running in this case.");
    $S = _("Search for 'expose_php' in your php.ini and set if to 'Off'.");
    $R = !ini_get("expose_php");
    $M = FALSE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Automatic quoting must be turned on */
    $N = "magic_quotes_gpc = <b>"._("On")."</b>";
    $D = _("Increase your server security by setting magic_quotes_gpc to 'on'. PHP will escape all quotes in strings in this case."); 
    $S = _("Search for 'magic_quotes_gpc' in your php.ini and set it to 'On'."); 
    $R = ini_get('magic_quotes_gpc'); 
    $M = TRUE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* Emulating old stuff? */
    $N = "zend.ze1_compatibility_mode = <b>"._("Off")."</b>";
    $D = _("Increase your server performance by setting magic_quotes_gpc to 'off'."); 
    $S = _("Search for 'zend.ze1_compatibility_mode' in your php.ini and set it to 'Off'."); 
    $R = !ini_get('zend.ze1_compatibility_mode'); 
    $M = FALSE;
    $this->config_checks[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );


    /* Configuration file  
     */

    /* Check if we can create a config file.*/
    $N = _("Configuration writeable");
    $D = _("The configuration file can't be written");
    $S = sprintf(_("GOsa reads its configuration from a file located in (%s/%s). The setup can write the configuration directly if it is writeable."),CONFIG_DIR,CONFIG_FILE); 
    $R =    ( file_exists(CONFIG_DIR."/".CONFIG_FILE) && is_writeable(CONFIG_DIR."/".CONFIG_FILE)) // is there a config file ? 
        ||  (!file_exists(CONFIG_DIR."/".CONFIG_FILE) && is_writeable(CONFIG_DIR));  // There is non, but can we create a file there ?
    $M = FALSE;
    $this->is_writeable[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );

    /* If we have writeaccess to the config dir, check if there is already a config file */
#    if($R){
#
#      /* check if there is already a config file. */
#      $N = _("No old configuration file.");
#      $D = "";//_("");
#      $S = _("If there is already a configuration file, this file will be overwritten when GOsa setup finishes. Please move your old config file away.");
#      $R =  !file_exists(CONFIG_DIR."/".CONFIG_FILE);
#      $M = FALSE;
#      $this->is_writeable[] = array("NAME" => $N , "DESC" => $D , "RESULT" => $R , "SOLUTION" => $S , "MUST" => $M );
#    }
  }
 

  function save_object()
  {
     /* If everything is fine, set this step to completed
     *  and allow switching to next setup step
     */
    $failed = false;
    foreach(array("basic_checks","config_checks","is_writeable") as $type){
      foreach($this->$type as $obj){

        if($obj['MUST'] && !$obj['RESULT']){
          $failed = true;
          break;
        }
      }
    }
    $this->is_completed = !$failed;
  }
 

  /* Check if current PHP version is compatible 
      with the current version of GOsa */
  function check_php_version()
  {
    return(version_compare(phpversion(),"5.2.0",">=")); 
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
