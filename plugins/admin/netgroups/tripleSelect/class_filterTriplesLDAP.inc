<?php

/* This code is part of GOsa-contrib (https://oss.gonicus.de/labs/gosa-contrib)
 * Copyright (C) 2011 Alejandro Escanero Blanco (aescanero@gmail.com)
 *
 * Ported to GOsa 2.7 by Fabian Hickert (hickert@gonicus.de)
 * Copyright (C) 2012 GONICUS GmbH
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

class filterTriplesLDAP extends filterLDAPBlacklist{

	static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
	{
		$result = filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);
		$filtered = filterLDAPBlacklist::filterByBlacklist($result);
		$filtered = filterTriplesLDAP::onlyHostsWithFQDN($filtered);
		return($filtered);
	}

	static function onlyHostsWithFQDN($entries)
	{
		global $config;
		$new_list = array();
		foreach($entries as $entry){
			if(array_intersect(array("goServer", "gotoTerminal", "gotoWorkstation"), $entry['objectClass'])){
				if(strpos($entry['cn'][0], ".") !== FALSE){
					$new_list[] = $entry;
				}else{

					// No FQDN given for this host, now try to find dns-domain information for this host and
					// append the dns-domain to the hostname.
				  	$ldap = $config->get_ldap_link();
				  	$ldap->cd($config->current['BASE']);
		          	$ldap->search("(&(objectClass=dNSZone)(relativeDomainName=".$entry['cn'][0].")(aRecord=*))", array("zoneName"));
                    while ($attrs = $ldap->fetch()) {
						$entry['cn'][0] = $entry['cn'][0].".".$attrs['zoneName'][0];
						$new_list[] = $entry;
                    }

				}
			}else{
				$new_list[] = $entry;
			}

		}
		return($new_list);
	}

}

?>
