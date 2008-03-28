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

class config  {

  /* XML parser */
  var $parser;
  var $config_found= FALSE;
  var $tags= array();
  var $level= 0;
  var $gpc= 0;
  var $section= "";
  var $currentLocation= "";

  /* Selected connection */
  var $current= array();

  /* Link to LDAP-server */
  var $ldap= NULL;
  var $referrals= array();

  /* Configuration data */
  var $data= array( 'TABS' => array(), 'LOCATIONS' => array(), 'SERVERS' => array(),
      'MAIN' => array(),
      'MENU' => array(), 'SERVICE' => array());
  var $basedir= "";

  /* Keep a copy of the current deparment list */
  var $departments= array();
  var $idepartments= array();
  var $adepartments= array();
  var $tdepartments= array();
  var $filename = "";
  var $last_modified = 0;

  function config($filename, $basedir= "")
  {
    $this->parser = xml_parser_create();
    $this->basedir= $basedir;

    xml_set_object($this->parser, $this);
    xml_set_element_handler($this->parser, "tag_open", "tag_close");

    /* Parse config file directly? */
    if ($filename != ""){
      $this->parse($filename);
    }
  }

  
  function check_and_reload()
  {
    if($this->filename != "" && filemtime($this->filename) != $this->last_modified){

      $this->config_found= FALSE;
      $this->tags= array();
      $this->level= 0;
      $this->gpc= 0;
      $this->section= "";
      $this->currentLocation= "";

      $this->parser = xml_parser_create();
      xml_set_object($this->parser, $this);
      xml_set_element_handler($this->parser, "tag_open", "tag_close");
      $this->parse($this->filename);
      if(session::is_set('plist')){
        session::un_set('plist');
      }
      if(session::is_set('plug')){
        session::un_set('plug');
      }
      if(isset($_GET['plug'])){
        unset($_GET['plug']);
      }
    }
  }  


  function parse($filename)
  { 
    $this->last_modified = filemtime($filename);
    $this->filename = $filename;
    $fh= fopen($filename, "r"); 
    $xmldata= fread($fh, 100000);
    fclose($fh); 
    if(!xml_parse($this->parser, chop($xmldata))){
      $msg = sprintf(_("XML error in gosa.conf: %s at line %d"),
            xml_error_string(xml_get_error_code($this->parser)),
            xml_get_current_line_number($this->parser));
      msg_dialog::display(_("Configuration error"), $msg, FATAL_ERROR_DIALOG);
      exit;
    }
  }

  function tag_open($parser, $tag, $attrs)
  { 
    /* Save last and current tag for reference */
    $this->tags[$this->level]= $tag;
    $this->level++;

    /* Trigger on CONF section */
    if ($tag == 'CONF'){
      $this->config_found= TRUE;
    }

    /* Return if we're not in config section */
    if (!$this->config_found){
      return;
    }

    /* yes/no to true/false and upper case TRUE to true and so on*/
    foreach($attrs as $name => $value){
      if(preg_match("/^(true|yes)$/i",$value)){
        $attrs[$name] = "true";
      }elseif(preg_match("/^(false|no)$/i",$value)){
        $attrs[$name] = "false";
      } 
    }

    /* Look through attributes */
    switch ($this->tags[$this->level-1]){


      /* Handle tab section */
      case 'TAB':	$name= $this->tags[$this->level-2];

                  /* Create new array? */
                  if (!isset($this->data['TABS'][$name])){
                    $this->data['TABS'][$name]= array();
                  }

                  /* Add elements */
                  $this->data['TABS'][$name][]= $attrs;
                  break;

                  /* Handle location */
      case 'LOCATION':
                  if ($this->tags[$this->level-2] == 'MAIN'){
                    $name= $attrs['NAME'];
                    $this->currentLocation= $name;

                    /* Add location elements */
                      $this->data['LOCATIONS'][$name]= $attrs;
                    }
                  break;

                  /* Handle referral tags */
      case 'REFERRAL':
                  if ($this->tags[$this->level-2] == 'LOCATION'){
                    $url= $attrs['URL'];
                    $server= preg_replace('!^([^:]+://[^/]+)/.*$!', '\\1', $url);

                    /* Add location elements */
                    if (!isset($this->data['LOCATIONS'][$this->currentLocation]['REFERRAL'])){
                      $this->data['LOCATIONS'][$this->currentLocation]['REFERRAL']= array();
                    }

                    $this->data['LOCATIONS'][$this->currentLocation]['REFERRAL'][$server]= $attrs;
                  }
                  break;

                  /* Load main parameters */
      case 'MAIN':
                  $this->data['MAIN']= array_merge ($this->data['MAIN'], $attrs);
                  break;

                  /* Load menu */
      case 'SECTION':
                  if ($this->tags[$this->level-2] == 'MENU'){
                    $this->section= $attrs['NAME'];
                    $this->data['MENU'][$this->section]= array(); ;
                  }
                  break;

                  /* Inser plugins */
      case 'PLUGIN':
                  if ($this->tags[$this->level-3] == 'MENU' &&
                      $this->tags[$this->level-2] == 'SECTION'){

                    $this->data['MENU'][$this->section][$this->gpc++]= $attrs;
                  }
                  if ($this->tags[$this->level-2] == 'SERVICEMENU'){
                    $this->data['SERVICE'][$attrs['CLASS']]= $attrs;
                  }
                  break;
    }
  }

  function tag_close($parser, $tag)
  {
    /* Close config section */
    if ($tag == 'CONF'){
      $this->config_found= FALSE;
    }
    $this->level--;
  }

  function get_ldap_link($sizelimit= FALSE)
  {
    if($this->ldap === NULL || !is_resource($this->ldap->cid)){

      /* Build new connection */
      $this->ldap= ldap_init ($this->current['SERVER'], $this->current['BASE'],
          $this->current['ADMIN'], $this->current['PASSWORD']);

      /* Check for connection */
      if (is_null($this->ldap) || (is_int($this->ldap) && $this->ldap == 0)){
        $smarty= get_smarty();
        msg_dialog::display(_("LDAP error"), _("Cannot bind to LDAP. Please contact the system administrator."), FATAL_ERROR_DIALOG);
        exit();
      }

      if (!session::is_set('size_limit')){
        session::set('size_limit',$this->current['SIZELIMIT']);
        session::set('size_ignore',$this->current['SIZEIGNORE']);
      }

      if ($sizelimit){
        $this->ldap->set_size_limit(session::get('size_limit'));
      } else {
        $this->ldap->set_size_limit(0);
      }

      /* Move referrals */
      if (!isset($this->current['REFERRAL'])){
        $this->ldap->referrals= array();
      } else {
        $this->ldap->referrals= $this->current['REFERRAL'];
      }
    }

    return new ldapMultiplexer($this->ldap);
  }

  function set_current($name)
  {
    $this->current= $this->data['LOCATIONS'][$name];
    if (!isset($this->current['PEOPLE'])){
      $this->current['PEOPLE']= "ou=people";
    }
    if (!isset($this->current['GROUPS'])){
      $this->current['GROUPS']= "ou=groups";
    }

    if (isset($this->current['INITIAL_BASE'])){
      session::set('CurrentMainBase',$this->current['INITIAL_BASE']);
    }
  
    /* Remove possibly added ',' from end of group and people ou */
    $this->current['GROUPS'] = preg_replace("/,*$/","",$this->current['GROUPS']);
    $this->current['PEOPLE'] = preg_replace("/,*$/","",$this->current['PEOPLE']);

    if (!isset($this->current['WINSTATIONS'])){
      $this->current['WINSTATIONS']= "ou=winstations,ou=systems";
    }
    if (!isset($this->current['HASH'])){
      $this->current['HASH']= "crypt";
    }
    if (!isset($this->current['DNMODE'])){
      $this->current['DNMODE']= "cn";
    }
    if (!isset($this->current['MINID'])){
      $this->current['MINID']= 100;
    }
    if (!isset($this->current['SIZELIMIT'])){
      $this->current['SIZELIMIT']= 200;
    }
    if (!isset($this->current['SIZEINGORE'])){
      $this->current['SIZEIGNORE']= TRUE;
    } else {
      if (preg_match("/true/i", $this->current['SIZEIGNORE'])){
        $this->current['SIZEIGNORE']= TRUE;
      } else {
        $this->current['SIZEIGNORE']= FALSE;
      }
    }

    /* Sort referrals, if present */
    if (isset ($this->current['REFERRAL'])){
      $bases= array();
      $servers= array();
      foreach ($this->current['REFERRAL'] as $ref){
        $server= preg_replace('%^(.*)/[^/]+$%', '\\1', $ref['URL']);
        $base= preg_replace('%^.*/([^/]+)$%', '\\1', $ref['URL']);
        $bases[$base]= strlen($base);
        $servers[$base]= $server;
      }
      asort($bases);
      reset($bases);
    }

    /* SERVER not defined? Load the one with the shortest base */
    if (!isset($this->current['SERVER'])){
      $this->current['SERVER']= $servers[key($bases)];
    }

    /* BASE not defined? Load the one with the shortest base */
    if (!isset($this->current['BASE'])){
      $this->current['BASE']= key($bases);
    }

    /* Convert BASE to have escaped special characters */
    $this->current['BASE']= @LDAP::convert($this->current['BASE']);

    /* Parse LDAP referral informations */
    if (!isset($this->current['ADMIN']) || !isset($this->current['PASSWORD'])){
      $url= $this->current['SERVER'];
      $referral= $this->current['REFERRAL'][$url];
      $this->current['ADMIN']= $referral['ADMIN'];
      $this->current['PASSWORD']= $referral['PASSWORD'];
    }

    /* Load server informations */
    $this->load_servers();
  }

  function load_servers ()
  {
    /* Only perform actions if current is set */
    if ($this->current === NULL){
      return;
    }

    /* Fill imap servers */
    $ldap= $this->get_ldap_link();
    $ldap->cd ($this->current['BASE']);
    if (!isset($this->current['MAILMETHOD'])){
      $this->current['MAILMETHOD']= "";
    }
    if ($this->current['MAILMETHOD'] == ""){
      $ldap->search ("(objectClass=goMailServer)", array('cn'));
      $this->data['SERVERS']['IMAP']= array();
      error_reporting(0);
      while ($attrs= $ldap->fetch()){
        $name= $attrs['cn'][0];
        $this->data['SERVERS']['IMAP'][$name]= $name;
      }
      error_reporting(E_ALL);
    } else {
      $ldap->search ("(objectClass=goImapServer)", array('goImapName', 'goImapConnect', 'goImapAdmin', 'goImapPassword',
            'goImapSieveServer', 'goImapSievePort'));

      $this->data['SERVERS']['IMAP']= array();
      error_reporting(0);
      while ($attrs= $ldap->fetch()){
        $name= $attrs['goImapName'][0];
        $this->data['SERVERS']['IMAP'][$name]= array( "connect" => $attrs['goImapConnect'][0],
            "admin" => $attrs['goImapAdmin'][0],
            "password" => $attrs['goImapPassword'][0],
            "sieve_server" => $attrs['goImapSieveServer'][0],
            "sieve_port" => $attrs['goImapSievePort'][0]);
      }
      error_reporting(E_ALL);
    }

    /* Get kerberos server. FIXME: only one is supported currently */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goKrbServer)");
    if ($ldap->count()){
      $attrs= $ldap->fetch();
      $this->data['SERVERS']['KERBEROS']= array( 'SERVER' => $attrs['cn'][0],
          'REALM' => $attrs['goKrbRealm'][0],
          'ADMIN' => $attrs['goKrbAdmin'][0],
          'PASSWORD' => $attrs['goKrbPassword'][0]);
    }

    /* Get cups server. FIXME: only one is supported currently */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goCupsServer)");
    if ($ldap->count()){
      $attrs= $ldap->fetch();
      $this->data['SERVERS']['CUPS']= $attrs['cn'][0];	
    }

    /* Get fax server. FIXME: only one is supported currently */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goFaxServer)");
    if ($ldap->count()){
      $attrs= $ldap->fetch();
      $this->data['SERVERS']['FAX']= array( 'SERVER' => $attrs['cn'][0],
          'LOGIN' => $attrs['goFaxAdmin'][0],
          'PASSWORD' => $attrs['goFaxPassword'][0]);
    }


    /* Get asterisk servers */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goFonServer)");
    $this->data['SERVERS']['FON']= array();
    if ($ldap->count()){
      while ($attrs= $ldap->fetch()){

        /* Add 0 entry for development */
        if(count($this->data['SERVERS']['FON']) == 0){
          $this->data['SERVERS']['FON'][0]= array(
              'DN'      => $attrs['dn'],
              'SERVER'  => $attrs['cn'][0],
              'LOGIN'   => $attrs['goFonAdmin'][0],
              'PASSWORD'  => $attrs['goFonPassword'][0],
              'DB'    => "gophone",
              'SIP_TABLE'   => "sip_users",
              'EXT_TABLE'   => "extensions",
              'VOICE_TABLE' => "voicemail_users",
              'QUEUE_TABLE' => "queues",
              'QUEUE_MEMBER_TABLE'  => "queue_members");
        }

        /* Add entry with 'dn' as index */
        $this->data['SERVERS']['FON'][$attrs['dn']]= array(
            'DN'      => $attrs['dn'],
            'SERVER'  => $attrs['cn'][0],
            'LOGIN'   => $attrs['goFonAdmin'][0],
            'PASSWORD'  => $attrs['goFonPassword'][0],
            'DB'    => "gophone",
            'SIP_TABLE'   => "sip_users",
            'EXT_TABLE'   => "extensions",
            'VOICE_TABLE' => "voicemail_users",
            'QUEUE_TABLE' => "queues",
            'QUEUE_MEMBER_TABLE'  => "queue_members");
      }
    }


    /* Get glpi server */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(&(objectClass=goGlpiServer)(cn=*)(goGlpiAdmin=*)(goGlpiDatabase=*))",array("cn","goGlpiPassword","goGlpiAdmin","goGlpiDatabase"));
    if ($ldap->count()){
      $attrs= $ldap->fetch();
      if(!isset($attrs['goGlpiPassword'])){
        $attrs['goGlpiPassword'][0] ="";
      }
      $this->data['SERVERS']['GLPI']= array( 
          'SERVER' 	=> $attrs['cn'][0],
          'LOGIN' 	=> $attrs['goGlpiAdmin'][0],
          'PASSWORD' 	=> $attrs['goGlpiPassword'][0],
          'DB'		=> $attrs['goGlpiDatabase'][0]);
    }


    /* Get logdb server */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goLogDBServer)");
    if ($ldap->count()){
      $attrs= $ldap->fetch();
      $this->data['SERVERS']['LOG']= array( 'SERVER' => $attrs['cn'][0],
          'LOGIN' => $attrs['goLogAdmin'][0],
          'PASSWORD' => $attrs['goLogPassword'][0]);
    }


    /* GOsa logging databases */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=gosaLogServer)");
    if ($ldap->count()){
      while($attrs= $ldap->fetch()){
      $this->data['SERVERS']['LOGGING'][$attrs['cn'][0]]= 
          array(
          'DN'    => $attrs['dn'],
          'USER'  => $attrs['goLogDBUser'][0],
          'DB'    => $attrs['goLogDB'][0],
          'PWD'   => $attrs['goLogDBPassword'][0]);
      }
    }


    /* Get NFS server lists */
    $tmp= array("default");
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(&(objectClass=goShareServer)(goExportEntry=*))");
    while ($attrs= $ldap->fetch()){
      for ($i= 0; $i<$attrs["goExportEntry"]["count"]; $i++){
        if(!preg_match('/^[^|]+\|[^|]+\|NFS\|.*$/', $attrs["goExportEntry"][$i])){
          continue;
        }
        $path= preg_replace ("/^[^|]+\|[^|]+\|[^|]+\|[^|]+\|([^|]+).*$/", '\1', $attrs["goExportEntry"][$i]);
        $tmp[]= $attrs["cn"][0].":$path";
      }
    }
    $this->data['SERVERS']['NFS']= $tmp;

    /* Load Terminalservers */
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goTerminalServer)",array("cn","gotoSessionType"));
    $this->data['SERVERS']['TERMINAL']= array();
    $this->data['SERVERS']['TERMINAL'][]= "default";
    $this->data['SERVERS']['TERMINAL_SESSION_TYPES'] = array();


    while ($attrs= $ldap->fetch()){
      $this->data['SERVERS']['TERMINAL'][]= $attrs["cn"][0];
      if(isset( $attrs["gotoSessionType"]['count'])){
        for($i =0 ; $i < $attrs["gotoSessionType"]['count'] ; $i++){
          $this->data['SERVERS']['TERMINAL_SESSION_TYPES'][$attrs["cn"][0]][] = $attrs["gotoSessionType"][$i]; 
        }
      }
    }

    /* Ldap Server */
    $this->data['SERVERS']['LDAP']= array();
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goLdapServer)");
    while ($attrs= $ldap->fetch()){
      if (isset($attrs["goLdapBase"])){
        for ($i= 0; $i<$attrs["goLdapBase"]["count"]; $i++){
          $this->data['SERVERS']['LDAP'][]= $attrs["cn"][0].":".$attrs["goLdapBase"][$i];
        }
      }
    }

    /* Get misc server lists */
    $this->data['SERVERS']['SYSLOG']= array("default");
    $this->data['SERVERS']['NTP']= array("default");
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goNtpServer)");
    while ($attrs= $ldap->fetch()){
      $this->data['SERVERS']['NTP'][]= $attrs["cn"][0];
    }
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=goSyslogServer)");
    while ($attrs= $ldap->fetch()){
      $this->data['SERVERS']['SYSLOG'][]= $attrs["cn"][0];
    }

    /* Get samba servers from LDAP, in case of samba3 */
    if ($this->current['SAMBAVERSION'] == 3){
      $this->data['SERVERS']['SAMBA']= array();
      $ldap->cd ($this->current['BASE']);
      $ldap->search ("(objectClass=sambaDomain)");
      while ($attrs= $ldap->fetch()){
        $this->data['SERVERS']['SAMBA'][$attrs['sambaDomainName'][0]]= array( "SID" =>"","RIDBASE" =>"");
        if(isset($attrs["sambaSID"][0])){
          $this->data['SERVERS']['SAMBA'][$attrs['sambaDomainName'][0]]["SID"]  = $attrs["sambaSID"][0];
        }
        if(isset($attrs["sambaAlgorithmicRidBase"][0])){
          $this->data['SERVERS']['SAMBA'][$attrs['sambaDomainName'][0]]["RIDBASE"] = $attrs["sambaAlgorithmicRidBase"][0];
        }
      }

      /* If no samba servers are found, look for configured sid/ridbase */
      if (count($this->data['SERVERS']['SAMBA']) == 0){
        if (!isset($this->current["SID"]) || !isset($this->current["RIDBASE"])){
          msg_dialog::display(_("Configuration error"), _("SID and/or RIDBASE missing in the configuration!"), FATAL_ERROR_DIALOG);
          exit();
        } else {
          $this->data['SERVERS']['SAMBA']['DEFAULT']= array(
              "SID" => $this->current["SID"],
              "RIDBASE" => $this->current["RIDBASE"]);
        }
      }
    }
  }


  function get_departments($ignore_dn= "")
  {
    global $config;

    /* Initialize result hash */
    $result= array();
    $administrative= array();
    $result['/']= $this->current['BASE'];
    $this->tdepartments= array();

    /* Get list of department objects */
    $ldap= $this->get_ldap_link();
    $ldap->cd ($this->current['BASE']);
    $ldap->search ("(objectClass=gosaDepartment)", array("ou", "objectClass", "gosaUnitTag"));
    while ($attrs= $ldap->fetch()){
      $dn= $ldap->getDN();
      $this->tdepartments[$dn]= "";

      /* Save administrative departments */
      if (in_array_ics("gosaAdministrativeUnit", $attrs['objectClass']) &&
          isset($attrs['gosaUnitTag'][0])){
        $administrative[$dn]= $attrs['gosaUnitTag'][0];
        $this->tdepartments[$dn]= $attrs['gosaUnitTag'][0];
      }
    
      if (in_array_ics("gosaAdministrativeUnitTag", $attrs['objectClass']) &&
          isset($attrs['gosaUnitTag'][0])){
        $this->tdepartments[$dn]= $attrs['gosaUnitTag'][0];
      }
    
      if ($dn == $ignore_dn){
        continue;
      }

      /* Only assign non-root departments */
      if ($dn != $result['/']){
        $result[convert_department_dn($dn)]= $dn;
      }
    }

    $this->adepartments= $administrative;
    $this->departments= $result;
  }


  function make_idepartments($max_size= 28)
  {
    global $config;
    $base = $config->current['BASE'];

    $arr = array();
    $ui= get_userinfo();

    $this->idepartments= array();

    /* Create multidimensional array, with all departments. */
    foreach ($this->departments as $key => $val){

      /* When using strict_units, filter non relevant parts */
      if (isset($config->current['STRICT_UNITS']) && preg_match('/true/i', $config->current['STRICT_UNITS'])){
        if ($ui->gosaUnitTag != "" && isset($this->tdepartments[$val]) &&
            $this->tdepartments[$val] != $ui->gosaUnitTag){
#          continue;
        }
      }

      /* remove base from dn */
      $val2 = str_replace($base,"",$val);

      /* Get every single ou */
      $str = preg_replace("/ou=/","|ou=",$val2);	
      $elements = array_reverse(split("\|",$str)); 		

      /* Save last array position */
      $last = &$arr;

      /* Get array depth  */
      $cnt = count($elements);

      /* Add last ou element of current dn to our array */
      foreach($elements as $key => $ele){

        /* skip enpty */
        if(empty($ele)) continue;

        /* Extract department name */		
        $elestr = preg_replace("/^ou=/","", $ele);
        $elestr = preg_replace("/,$/","",$elestr);	

        /* Add to array */	
        if($key == ($cnt-2)){
          $last[$elestr]['ENTRY'] = $val;
        }

        /* Set next array appending position */
        $last = &$last[$elestr]['SUB'];
      }
    }

    /* Add base entry */
    $ret["/"]["ENTRY"] 	= $base;
    $ret["/"]["SUB"]	= $arr;

    $this->idepartments= $this->generateDepartmentArray($ret,-1,$max_size);
  }


  /* Creates display friendly output from make_idepartments */
  function generateDepartmentArray($arr,$depth = -1,$max_size){
    $ret = array();
    $depth ++;

    /* Walk through array */	
    ksort($arr);
    foreach($arr as $name => $entries){

      /* If this department is the last in the current tree position 
       * remove it, to avoid generating output for it */
      if(count($entries['SUB'])==0){
        unset($entries['SUB']);
      }

      /* Fix name, if it contains a replace tag */
      $name= @LDAP::fix($name);

      /* Check if current name is too long, then cut it */
      if(mb_strlen($name, 'UTF-8')> $max_size){
        $name = mb_substr($name,0,($max_size-3), 'UTF-8')." ...";
      }

      /* Append the name to the list */	
      if(isset($entries['ENTRY'])){
        $a = "";
        for($i = 0 ; $i < $depth ; $i ++){
          $a.=".";
        }
        $ret[$entries['ENTRY']]=$a."&nbsp;".$name;
      }	

      /* recursive add of subdepartments */
      if(isset($entries['SUB'])){
        $ret = array_merge($ret,$this->generateDepartmentArray($entries['SUB'],$depth,$max_size));
      }
    }

    return($ret);
  }

  /* This function returns all available Shares defined in this ldap
   * There are two ways to call this function, if listboxEntry is true
   *  only name and path are attached to the array, in it is false, the whole
   *  entry will be parsed an atached to the result.
   */
  function getShareList($listboxEntry = false)
  {
    $tmp = get_sub_list("(&(objectClass=goShareServer)(goExportEntry=*))","server",get_ou("serverou"),
        $this->current['BASE'],array("goExportEntry","cn"), GL_NONE);
    $return =array();
    foreach($tmp as $entry){

      if(isset($entry['goExportEntry']['count'])){
        unset($entry['goExportEntry']['count']);
      }
      if(isset($entry['goExportEntry'])){
        foreach($entry['goExportEntry'] as $export){
          $shareAttrs = split("\|",$export);
          if($listboxEntry) {
            $return[$shareAttrs[0]."|".$entry['cn'][0]] = $shareAttrs[0]." - ".$entry['cn'][0];
          }else{
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['server']       = $entry['cn'][0];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['name']         = $shareAttrs[0];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['description']  = $shareAttrs[1];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['type']         = $shareAttrs[2];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['charset']      = $shareAttrs[3];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['path']         = $shareAttrs[4];
            $return[$shareAttrs[0]."|".$entry['cn'][0]]['option']       = $shareAttrs[5];
          }
        }
      }
    }
    return($return);
  }


  /* This function returns all available ShareServer */
  function getShareServerList()
  {
    global $config;
    $return = array();
    $base = $config->current['BASE'];
    $res= get_sub_list("(&(objectClass=goShareServer)(goExportEntry=*))", "server",
          get_ou("serverou"), $base,array("goExportEntry","cn"),GL_NONE);

    foreach($res as $entry){
        if(isset($entry['goExportEntry']['count'])){
          unset($entry['goExportEntry']['count']);
        }
        foreach($entry['goExportEntry'] as $share){
          $a_share = split("\|",$share);
          $sharename = $a_share[0];
          $return[$entry['cn'][0]."|".$sharename] = $entry['cn'][0]." [".$sharename."]";
        }
    }
    return($return);
  }


  /* Check if there's the specified bool value set in the configuration */
  function boolValueIsTrue($section, $value)
  {
    $section= strtoupper($section);
    $value= strtoupper($value);
    if (isset($this->data[$section][$value])){
    
      $data= $this->data[$section][$value];
      if (preg_match("/^true$/i", $data) || preg_match("/yes/i", $data)){
        return TRUE;
      }

    }

    return FALSE;
  }


  function __search(&$arr, $name, $return)
  {
    $return= strtoupper($return);
    if (is_array($arr)){
      foreach ($arr as &$a){
        if (isset($a['CLASS']) && strcasecmp($name, $a['CLASS']) == 0){
          return(isset($a[$return])?$a[$return]:"");
        } else {
          $res= $this->__search ($a, $name, $return);
          if ($res != ""){
            return $res;
          }
        }
      }
    }
    return ("");
  }


  function search($class, $value, $categories= "")
  {
    if (is_array($categories)){
      foreach ($categories as $category){
        $res= $this->__search($this->data[strtoupper($category)], $class, $value);
        if ($res != ""){
          return $res;
        }
      }
    } else {
      if ($categories == "") {
        return $this->__search($this->data, $class, $value);
      } else {
        return $this->__search($this->data[strtoupper($categories)], $class, $value);
      }
    } 

    return ("");
  }


  /* On debian systems the session files are deleted with
   *  a cronjob, which detects all files older than specified 
   *  in php.ini:'session.gc_maxlifetime' and removes them.
   * This function checks if the gosa.conf value matches the range
   *  defined by session.gc_maxlifetime.
   */
  function check_session_lifetime()
  {
    $cfg_lifetime = $this->data['MAIN']['SESSION_LIFETIME'];
    $ini_lifetime = ini_get('session.gc_maxlifetime');
    $deb_system   = file_exists('/etc/debian_version');
    return(!($deb_system && ($ini_lifetime < $cfg_lifetime)));  
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
