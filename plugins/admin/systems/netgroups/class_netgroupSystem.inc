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


class netgroupSystem extends plugin {

	var $plHeadline = "NIS Netgroup";
	var $plDescription = "Choose NIS Netgroups for the system";

	/* Plugin specific values */
	var $view_logged = FALSE;

	/* attribute list for save action */
	var $CopyPasteVars = array();
	var $objectInfo = array();
	var $attributes = array();
	var $objectclasses = array("whatever");
	var $cn = "";
	var $netgroups = array();
	var $initial_netgroups = array();
	var $netgroupSelect = FALSE;
	var $multiple_support = TRUE;
	var $is_account = TRUE;
	var $dn = "";
	var $attrs = array();
	var $fqdn_found = FALSE;


	/* Constructor: Prepare values, lists, ...
 	 * */
	function __construct(&$config, $dn= NULL) 
	{
		$this->config = $config;

		/* Load bases attributes */
		plugin::__construct($config, $dn);

		/* Setting cn to default */
		if (isset($this->attrs['cn'][0])) {
			$this->cn = $this->attrs['cn'][0];
		}

		// Extract host and domain name out of the cn
		$host = $this->cn;
		$domain = "";
		$this->fqdn_found = TRUE;
		if(strpos($host, ".") === FALSE){
			$domain = $this->getDNSZoneForHost($host);
			if(!$domain){
				$this->fqdn_found = FALSE;
			}
		}else{
			$domain = preg_replace("/^[^\.]*+\./", "", $host);
			$host = preg_replace("/\..*/", "", $host);
		}
		$this->host = $host;
		$this->domain = $domain;
		$this->fqdn = "{$host}.{$domain}";

		if ($this->fqdn_found && $this->dn != "new") {
			$ldap = $this->config->get_ldap_link();
			$ldap->cd($this->config->current['BASE']);
			$ldap->search("(&(objectClass=nisNetgroup)(nisNetgroupTriple=*))", array("cn", "nisNetgroupTriple"));
			while ($attrs = $ldap->fetch()) {
				foreach ($attrs['nisNetgroupTriple'] as $val) {
					if (preg_match("/^\(" . $this->fqdn . ",\-?,(\S*)\)$/", $val, $matches)) {
						$this->addGroup($attrs['cn'][0]);
					}
				}
			}
		}

		$this->initial_netgroups = $this->netgroups;

        // Prepare lists
        $this->memberList = new sortableListing(array(),array(), FALSE);
        $this->memberList->setDeleteable(true);
        $this->memberList->setInstantDelete(false);
        $this->memberList->setEditable(false);
        $this->memberList->setWidth("100%");
        $this->memberList->setHeight("300px");
        $this->memberList->setColspecs(array('20px','*','*'));
        $this->memberList->setHeader(array("-",_("Name"),_("Description")));
        $this->memberList->setDefaultSortColumn(1);
	}


    /* Returns the dns-domain name used by the given hostname
     * If no domainname could be identified, NULL is returned
     * */
    function getDNSZoneForHost($host)
    {
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=".$host.")(aRecord=*))", array("zoneName"));
        while ($attrs = $ldap->fetch()) {
            return($attrs['zoneName'][0]);
        }
        return(NULL);
    }


	/* Removes a netgroup from this object
 	 * */
	function removeGroup($name)
	{
		unset($this->objectInfo[$name]);
		unset($this->netgroups[$name]);
	}


	/* Adds a netgroup to this object
 	 * */
	function addGroup($name, $attrs=NULL)
	{
		$theme = getThemeName();

		if($attrs){
			$ldap = $this->config->get_ldap_link();
			$ldap->cd($this->config->current['BASE']);
			$ldap->search("(&(objectClass=nisNetgroup)(cn=".normalizeldap($name)."))", array("cn", "description"));
			if(!$ldap->count()){
				return;
			}
			$attrs= $ldap->fetch();
		}
		$desc = "&nbsp";
		if(isset($attrs['description'])){
			$desc = $attrs['description'][0];
		}
		$this->removeGroup($name);
		switch ($theme) {
			case 'classic':
				$this->objectInfo[$name] = array(image("plugins/netgroups/images/select_netgroup.png"), $name, $desc);
				break;
			
			default:
				$this->objectInfo[$name] = array(image("<i class='material-icons input-icons'>connect_without_contact</i>"), $name, $desc);
				break;
		}
		$this->netgroups[$name] = $name;
	}


	/* Render the gui.
 	 * */
	function execute($isCopyPaste = false) {
		/* Call parent execute */
		plugin::execute();
		$display = "";

		// Set list Acls
		$this->memberList->setAcl($this->getacl("netgroups"));

		/* Log view */
		if ($this->is_account && !$this->view_logged) {
			$this->view_logged = TRUE;
			new log("view", "users/" . get_class($this), $this->dn);
		}

		if (isset($_POST['del_netgroups']) && isset($_POST['netgroups']) && preg_match("/w/", $this->getacl("netgroups"))) {
			foreach ($_POST['netgroups'] as $value) {
				unset($this->netgroups["$value"]);
			}
		}

		/* Add objects? */
		if (isset($_POST["edit_membership"]) && preg_match("/w/", $this->getacl("netgroups"))) {
			$this->netgroupSelect = new memberNisnetgroupSelect($this->config, get_userinfo());
			$this->dialog = TRUE;
		}

		/* Add objects finished? */
		if (isset($_POST["add_memberNisnetgroups_cancel"]) || isset($_POST['cancel-abort'])) {
			$this->netgroupSelect = NULL;
			$this->dialog = FALSE;
		}

		/* Add to netgroup */
		if (isset($_POST['add_memberNisnetgroups_finish']) || isset($_POST['ok-save']) && $this->netgroupSelect) {

			/* Get all the dn from netgroupSelect */
			$users = $this->netgroupSelect->detectPostActions();
			if (isset($users['targets'])) {
				$headpage = $this->netgroupSelect->getHeadpage();
				foreach ($users['targets'] as $dn) {
					$attrs = $headpage->getEntry($dn);
					$this->addGroup($attrs['cn'][0]);
				}
			}
			$this->netgroupSelect = NULL;
			$this->dialog = FALSE;
		}

		/* Manage object add dialog */
		if ($this->netgroupSelect) {
			return($this->netgroupSelect->execute());
		}

		$data = $lData = array();
        foreach($this->netgroups as $key => $name){
            $data[$key] = $key;
            $lData[$key] = array('data'=> $this->objectInfo[$name]);
        }

        $this->memberList->setListData($data, $lData);
        $this->memberList->update();

		// Assign smarty variables
		$smarty = get_smarty();
		$smarty->assign("fqdn_found", $this->fqdn_found);
		$smarty->assign("memberList", $this->memberList->render());
		$smarty->assign("netgroupsACL", $this->getacl("netgroups"));
		$display.= $smarty->fetch(get_template_path('netgroupSystem.tpl', TRUE, dirname(__FILE__)));
		return($display);
	}


	/* Take care of HTML POST actions
 	 * */
	function save_object() 
	{
		if (isset($_POST['nisnetgroupedit'])) {
			plugin::save_object();

            $this->memberList->save_object();
            $action = $this->memberList->getAction();
            if(isset($action['action']) && $action['action'] == 'delete'){
                foreach($action['targets'] as $id){
                    $this->removeGroup($entry = $this->memberList->getKey($id));
                }
            }
		}
	}


	/* Save changes back to the ldap.
 	 * */
	function save() 
	{

		$orig_cn= $this->cn;

		/* Set cn if we have been created or renamed */
		if ($this->is_new){
			$this->cn= preg_replace('/^cn=([^,]+),.*/', '$1', $this->dn);
		}

		// If nothing has changed, just return
		if(
		   (! (array_diff(array_keys($this->initial_netgroups), array_keys($this->netgroups)) ||
		       array_diff(array_keys($this->netgroups), array_keys($this->initial_netgroups)))) &&
		    ($this->cn===$orig_cn)
		   )
		{
			return;
		}

		$to_add = $to_del = array();
		foreach(array_keys($this->netgroups) as $val){
			$ldap = $this->config->get_ldap_link();
			$ldap->cd($this->config->current['BASE']);
			$ldap->search("(&(objectClass=nisNetgroup)(cn=".$val."))", array("dn"));
			while ($g_data = $ldap->fetch()) {
				$to_add[] = $g_data['dn'];
			}
		}
		foreach($to_add as $dn){
			$ng = new netgroup($this->config, $dn);
			$ng->addTriple($this->cn, NULL, "", $this->attrs);
			if($orig_cn!==$this->cn) {
				/* The below code handles correct removal of NIS netgroup triples
				 * when a system's hostname gets renamed.
				 *
				* FIXME: This will fail if hostname and DNS zone get changed
				* at the same time.
				*/
				if(strpos($orig_cn, ".") === FALSE){
					$domain = $this->getDNSZoneForHost($this->cn);
					if(!$domain){
						msg_dialog::display(_("Failure when modifying NIS netgroup memberships for renamed system"), sprintf(_("Unable to detect FQDN for host '%s'!"), $this->cn), WARNING_DIALOG);
					}
					$host= $orig_cn;
				}else{
					$domain = preg_replace("/^[^\.]*+\./", "", $this->cn);
					$host = preg_replace("/\..*/", "", $orig_cn);
				}
				if($domain) {
					$ng->removeTriple($host.".".$domain);
				}
			}
			$ng->save();
		}

		$nonetgroups=array_diff(array_keys($this->initial_netgroups), array_keys($this->netgroups));
		foreach($nonetgroups as $val){
			$ldap = $this->config->get_ldap_link();
			$ldap->cd($this->config->current['BASE']);
			$ldap->search("(&(objectClass=nisNetgroup)(cn=".$val."))", array("dn"));
			while ($g_data = $ldap->fetch()) {
				$to_del[] = $g_data['dn'];
			}
		}
		foreach($to_del as $dn){
			$ng = new netgroup($this->config, $dn);
			$host= $this->cn;
			if(strpos($host, ".") === FALSE){
				$domain = $this->getDNSZoneForHost($host);
				if(!$domain){
					msg_dialog::display(_("Failed to remove triple"), sprintf(_("Unable to detect FQDN for host '%s'!"), $host), WARNING_DIALOG);
					continue;
				}
			}else{
				$domain = preg_replace("/^[^\.]*+\./", "", $host);
				$host = preg_replace("/\..*/", "", $host);
			}

			$ng->removeTriple($host.".".$domain);
			$ng->save();
		}
	}


	/* Return plugin information
 	 * */
	static function plInfo() {
		return (array(
			"plDescription" => _("NIS Netgroup member"),
			"plSelfModify" => FALSE,
			"plDepends" => array("user"),
			"plPriority" => 2,
			"plSection" => array("personal"),
			"plCategory" => array("users"),
			"plProperties" =>
			array(
				array(
					"name"          => "netgroupSystemRDN",
					"type"          => "rdn",
					"default"       => "ou=netgroups,",
					"description"   => _("RDN for netgroup storage."),
					"check"         => "gosaProperty::isRdn",
					"group"         => "plugin",
					"mandatory"     => FALSE)),

			"plOptions" => array(),
			"plProvidedAcls" => array(
				"netgroups" => _("NIS Netgroup"))
			));
	}
}

?>
