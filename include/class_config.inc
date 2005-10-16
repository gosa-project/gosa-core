<?php
/*
 * This code is part of GOsa (https://gosa.gonicus.de)
 * Copyright (C) 2003  Cajus Pollmeier
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
		'MAIN' => array( 'LANGUAGES' => array(), 'FAXFORMATS' => array() ),
		'MENU' => array(), 'SERVICE' => array());
  var $basedir= "";

  /* Keep a copy of the current deparment list */
  var $departments= array();
  var $idepartments= array();

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

  function parse($filename)
  { 
	$fh= fopen($filename, "r"); 
	$xmldata= fread($fh, 100000);
	fclose($fh); 
	if(!xml_parse($this->parser, chop($xmldata))){
		print_red(sprintf(_("XML error in gosa.conf: %s at line %d"),
			xml_error_string(xml_get_error_code($this->parser)),
			xml_get_current_line_number($this->parser)));
		echo $_SESSION['errors'];
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

		/* Handle language */
		case 'LANGUAGE':
				if ($this->tags[$this->level-2] == 'MAIN'){
					/* Add languages */
					$this->data['MAIN']['LANGUAGES'][$attrs['NAME']]= 
						$attrs['TAG'];
				}
				break;

		/* Handle faxformat */
		case 'FAXFORMAT':	
				if ($this->tags[$this->level-2] == 'MAIN'){
					/* Add fax formats */
					$this->data['MAIN']['FAXFORMATS'][]= $attrs['TYPE'];
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
	/* Build new connection */
	$this->ldap= ldap_init ($this->current['SERVER'], $this->current['BASE'],
			$this->current['ADMIN'], $this->current['PASSWORD']);

	/* Check for connection */
	if (is_null($this->ldap) || (is_int($this->ldap) && $this->ldap == 0)){
		print_red (_("Can't bind to LDAP. Please contact the system administrator."));
		echo $_SESSION['errors'];
		exit;
	}

	if (!isset($_SESSION['size_limit'])){
		$_SESSION['size_limit']= $this->current['SIZELIMIT'];
		$_SESSION['size_ignore']= $this->current['SIZEIGNORE'];
	}

	if ($sizelimit){
		$this->ldap->set_size_limit($_SESSION['size_limit']);
	} else {
                $this->ldap->set_size_limit(0);
        }

	/* Move referrals */
	if (!isset($this->current['REFERRAL'])){
		$this->ldap->referrals= array();
	} else {
		$this->ldap->referrals= $this->current['REFERRAL'];
	}

	return ($this->ldap);
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
	if ($this->current == NULL){
		return;
	}

	/* Fill imap servers */
	$ldap= $this->get_ldap_link();
	$ldap->cd ($this->current['BASE']);
	$ldap->search ("(objectClass=goImapServer)");

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
	if ($ldap->count()){
		$attrs= $ldap->fetch();
		$this->data['SERVERS']['FON']= array( 
						'SERVER' 	=> $attrs['cn'][0],
						'LOGIN' 	=> $attrs['goFonAdmin'][0],
						'PASSWORD' 	=> $attrs['goFonPassword'][0],
						'DB'		=> "gophone",
						'SIP_TABLE'		=> "sip_users",
						'EXT_TABLE' 	=> "extensions",
						'VOICE_TABLE'	=> "voicemail_users",
						'QUEUE_TABLE'	=> "queues",
						'QUEUE_MEMBER_TABLE'	=> "queue_members");
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

	/* Get NFS server lists */
	$tmp= array("default");
	$ldap->cd ($this->current['BASE']);
	$ldap->search ("(&(objectClass=goShareServer)(goExportEntry=*))");
	while ($attrs= $ldap->fetch()){
		for ($i= 0; $i<$attrs["goExportEntry"]["count"]; $i++){
			$path= preg_replace ("/\s.*$/", "", $attrs["goExportEntry"][$i]);
			$tmp[]= $attrs["cn"][0].":$path";
		}
	}
	$this->data['SERVERS']['NFS']= $tmp;


	/* Load Terminalservers */
	$ldap->cd ($this->current['BASE']);
	$ldap->search ("(objectClass=goTerminalServer)");
	$this->data['SERVERS']['TERMINAL'][]= "default";
	$this->data['SERVERS']['FONT'][]= "default";
	while ($attrs= $ldap->fetch()){
		$this->data['SERVERS']['TERMINAL'][]= $attrs["cn"][0];
		for ($i= 0; $i<$attrs["goFontPath"]["count"]; $i++){
			$this->data['SERVERS']['FONT'][]= $attrs["goFontPath"][$i];
		}
	}

	/* Ldap Server */
	$this->data['SERVERS']['LDAP']= array("default");
	$ldap->cd ($this->current['BASE']);
	$ldap->search ("(objectClass=goLdapServer)");
	while ($attrs= $ldap->fetch()){
		for ($i= 0; $i<$attrs["goLdapBase"]["count"]; $i++){
			$this->data['SERVERS']['LDAP'][]= $attrs["cn"][0].":".$attrs["goLdapBase"][$i];
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
			$this->data['SERVERS']['SAMBA'][$attrs['sambaDomainName'][0]]= array(
				"SID" => $attrs["sambaSID"][0],
				"RIDBASE" => $attrs["sambaAlgorithmicRidBase"][0]);
		}

		/* If no samba servers are found, look for configured sid/ridbase */
		if (count($this->data['SERVERS']['SAMBA']) == 0){
			if (!isset($this->current["SID"]) || !isset($this->current["RIDBASE"])){
				print_red(_("SID and/or RIDBASE missing in your configuration!"));
				echo $_SESSION['errors'];
				exit;
			} else {
				$this->data['SERVERS']['SAMBA']['DEFAULT']= array(
					"SID" => $this->current["SID"],
					"RIDBASE" => $this->current["RIDBASE"]);
			}
		}
	}
  }

  function make_idepartments($max_size= 28)
  {
  	$this->idepartments= array();
	foreach ($this->departments as $key => $val){
		if (strlen($key) > $max_size){
			$this->idepartments[$val]= substr($key, 0, $max_size/2 - 3)."...".substr($key, -$max_size/2);
		} else {
			$this->idepartments[$val]= $key;
		}
	}

	asort($this->idepartments);
  }

  /* This function returns all available Shares defined in this ldap
  * There are two ways to call this function, if listboxEntry is true
  *  only name and path are attached to the array, in it is false, the whole
  *  entry will be parsed an atached to the result.
  */
  function getShareList($listboxEntry = false)
  {
    $ldap= $this->get_ldap_link();
    $a_res = $ldap->search("(objectClass=goShareServer)",array("goExportEntry","cn"));
    $return= array();
    while($entry = $ldap->fetch($a_res)){
      unset($entry['goExportEntry']['count']);
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
    return($return);
  }

  /* This function returns all available ShareServer
  */
  function getShareServerList()
  {
    $ldap= $this->get_ldap_link();
    $a_res = $ldap->search("(objectClass=goShareServer)",array("goExportEntry","cn"));
    $return= array();
    while($entry = $ldap->fetch($a_res)){
      unset($entry['goExportEntry']['count']);
      foreach($entry['goExportEntry'] as $share){
        $a_share = split("\|",$share);
        $sharename = $a_share[0];
        $return[$entry['cn'][0]."|".$sharename] = $entry['cn'][0]." [".$sharename."]";
      }
    }
    return($return);
  }

}

?>
