#!/usr/bin/php
<?php
require_once('../include/class_sambaMungedDial.inc');

/*
	* GOsa: fix_munged.php - Modify existings sambaMungedDial-Entries to work with latest Win2003SP1 
	*
	* Authors: Jan Wenzel    <jan.wenzel@GONICUS.de>
	*
	* Copyright (C) 2006 GONICUS GmbH
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
	* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
	* USA
	*
	* Contact information: GONICUS GmbH
	* Moehnestrasse 11-17
	* D-59755 Arnsberg
	* Germany
	* tel: ++49 2932 916 0
	* fax: ++49 2932 916 230
	* email: info@GONICUS.de
	* http://www.GONICUS.de
	* */

/* Modify these settings to your needs */
$ldap_host= "localhost";
$ldap_port= "389";
$ldap_base= "dc=gonicus,dc=de";
$ldap_admin= "cn=ldapadmin,".$ldap_base;
$ldap_password= "tester";

/* Internal Settings */
$ldap_protocol= "3";
$filter= "(&(objectClass=sambaSamAccount)(sambaMungedDial=*))";
$attributes= array("dn","sambaMungedDial");

print("This script will try to convert all ldap entries that have the sambaMungedDial-Attribute set, into the new \n".
	    "format that win2003sp1 and later requires. If an entry is already in the new format, it is not touched. \n".
			"BEWARE: This script is not widely tested yet, so use it at your own risk! Be sure to backup your complete LDAP \n".
			"before running.\n".
			"Do you want to continue (y/n)?\n");

$handle= fopen("php://stdin","r");
$input=(fgets($handle,16));
fclose($handle);
if(substr(strtolower($input),0,1)!="y") {
	exit(1);
}
/* Connect to server */
$connection= ldap_connect($ldap_host,$ldap_port) 
	or die ('Could not connect to server '.$ldap_host."\n!");
ldap_set_option($connection, LDAP_OPT_PROTOCOL_VERSION, $ldap_protocol);
ldap_bind($connection,$ldap_admin,$ldap_password)
	or die ('Could not bind to server '.$ldap_host."!\n");

$results= ldap_get_entries($connection, ldap_search($connection, $ldap_base, $filter, $attributes));

$count= 0;

if(array_key_exists('count', $results)) {
	$count= $results['count'];
}

if($count > 0) {
	print('We found '.$count.' matching '.(($count==1)?'entry':'entries').".\n");
}

for($i=0; $i<$count; $i++) {
	$entry= $results[$i];
	print('Converting '.$entry['dn'].'...'); 
	$mungedDial = new sambaMungedDial();
	$mungedDial->load($entry['sambamungeddial'][0]);
	$modify['sambaMungedDial'][0]= $mungedDial->getMunged();
	if(ldap_modify($connection,$entry['dn'],$modify)) {
		print("done.\n");
	} else {
		print("failed.\n");
	}
}

ldap_close($connection);
?>

