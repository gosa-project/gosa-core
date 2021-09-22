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


class netgroupAccount extends plugin {
	/* Definitions */

	var $plHeadline = "NIS Netgroup";
	var $plDescription = "Choose NIS Netgroups for the user";

	/* Plugin specific values */
	var $view_logged = FALSE;

	/* attribute list for save action */
	var $CopyPasteVars = array();
	var $objectInfo = array();
	var $attributes = array();
	var $objectclasses = array("whatever");
	var $uid = "";
	var $netgroups = array();
	var $initial_netgroups = array();
	var $netgroupSelect = FALSE;
	var $multiple_support = TRUE;
	var $is_account = TRUE;
	var $dn = "";
	var $attrs = array();

	var $netgroups_all = array();
	var $netgroups_some = array();


	/* Constructor: Prepare values, lists, ...
 	 * */
	function __construct(&$config, $dn= NULL) 
	{
		$this->config = $config;

		/* Load bases attributes */
		plugin::__construct($config, $dn);

		/* Setting uid to default */
		if (isset($this->attrs['uid'][0])) {
			$this->uid = $this->attrs['uid'][0];
		}

		if ($this->dn != "new") {
			$ldap = $this->config->get_ldap_link();
			$ldap->cd($this->config->current['BASE']);
			$ldap->search("(&(objectClass=nisNetgroup)(nisNetgroupTriple=*))", array("cn", "nisNetgroupTriple"));
			while ($attrs = $ldap->fetch()) {
				foreach ($attrs['nisNetgroupTriple'] as $val) {
					if (preg_match("/^\(\-?," . $this->uid . ",(\S*)\)$/", $val, $matches)) {
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


	/* Removes a netgroup from this object
 	 * */
	function removeGroup($name)
	{
		unset($this->objectInfo[$name]);
		unset($this->netgroups[$name]);
		if(isset($this->netgroups_all[$name])){
			unset($this->netgroups_all[$name]);
		}
		if(isset($this->netgroups_some[$name])){
			unset($this->netgroups_some[$name]);
		}
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
				$this->objectInfo[$name] = array(image("<i class='material-icons'>connect_without_contact</i>"), $name, $desc);
				break;
		}
		$this->netgroups[$name] = $name;
		$this->netgroups_all[$name] = $name;
	}


	/* Render the gui.
 	 * */
	function multiple_execute() {
		return($this->execute());
	}
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
				if ($this->multiple_support_active) {
					unset($this->netgroups_all["$value"]);
					unset($this->netgroups_some["$value"]);
				}
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

		foreach($this->netgroups_some as $key => $name){
            $data[$key] = $name;
            $list_data = $this->objectInfo[$key];
            for($i=1;$i<3;$i++){
                $list_data[$i] = "<font color='grey'>".$list_data[$i]."</font>";
            }
            $lData[$key] = array('data'=> $list_data);
        }

        $this->memberList->setListData($data, $lData);
        $this->memberList->update();

		// Assign smarty variables
		$smarty = get_smarty();
		$smarty->assign("memberList", $this->memberList->render());
		$smarty->assign("netgroupsACL", $this->getacl("netgroups"));
		$smarty->assign("memberCn_All", $this->netgroups_all);
		$smarty->assign("memberCn_Some", $this->netgroups_some);
		$display.= $smarty->fetch(get_template_path('netgroupAccount.tpl', TRUE, dirname(__FILE__)));
		return($display);
	}


	/* Take care of HTML POST actions
 	 * */
	function multiple_save_object(){
		if (isset($_POST['nisnetgroupedit'])) {
			$this->save_object();
		}
	}
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
		// If nothing has changed, just return
		if(! (array_diff(array_keys($this->initial_netgroups), array_keys($this->netgroups)) ||
			array_diff(array_keys($this->netgroups), array_keys($this->initial_netgroups)))){
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
			$ng->addTriple(NULL, $this->uid, "", $this->attrs);
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
			$ng->removeTriple($this->uid);
			$ng->save();
		}
	}


	/* Returns object modification after multi-edit process
 	 * */
	function get_multi_edit_values() 
	{
		$ret = plugin::get_multi_edit_values();
		foreach (array("base") as $attr) {
			if (in_array($attr, $this->multi_boxes)) {
				$ret[$attr] = $this->$attr;
			}
		}
		$ret['netgroups'] = $this->netgroups;
		$ret['netgroups_some'] = $this->netgroups_some;
		return($ret);
	}


	/* Set collected multi-edit changes to this plugin
 	 * */
	function set_multi_edit_values($attrs) 
	{
		$netgroups = array();
		foreach ($attrs['netgroups_some'] as $uid => $value) {
			if (in_array($uid, array_keys($this->initial_netgroups))) {
				$netgroups[$uid] = $uid;
			}
		}
		foreach ($attrs['netgroups'] as $uid => $value) {
			$netgroups[$uid] = $uid;
		}
		plugin::set_multi_edit_values($attrs);
		$this->netgroups = $netgroups;
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
			"plOptions" => array(),
			"plProvidedAcls" => array(
				"netgroups" => _("NIS Netgroup"))
			));
	}


	/* Prepare this pkugin for multiple-edit.
 	 * */
	function init_multiple_support($attrs, $all) 
	{
		plugin::init_multiple_support($attrs, $all);
		$multiuid=$all["uid"];
		$netgroup_map=array();

		// Collect all groups
		$ldap = $this->config->get_ldap_link();
		$ldap->cd($this->config->current['BASE']);
		$ldap->search("(&(objectClass=nisNetgroup)(nisNetgroupTriple=*))", array("cn", "nisNetgroupTriple", "description"));
		$group_map = array();
		while ($group = $ldap->fetch()) {
				$name = $group['cn'][0];
				$group_map[$name] = array('uid' => array());
				foreach ($group['nisNetgroupTriple'] as $value) {
					if (preg_match("/^\(\-?,(\S+),(\S*)\)$/", $value, $matches)) {
						if(in_array($matches[1], $multiuid)){
							$group_map[$name]['uid'][] = $matches[1];
							$group_map[$name]['attrs'] = $group;
						}
					}
				}
		}

		// Get groups used by some users
		$this->netgroups = array();
		foreach($group_map as $name => $data){
			$uids = $data['uid'];
			$inter = array_intersect($multiuid, $uids);
			if(count($inter) && $multiuid["count"] != count($inter)){
				$this->addGroup($name, $data['attrs']);
			}
		}
		$this->netgroups_some = $this->netgroups;

		// Get groups used by all users
		$this->netgroups = array();
        foreach($group_map as $name => $data){
			$uids = $data['uid'];
			$inter = array_intersect($multiuid, $uids);
			if(count($inter) && $multiuid["count"] == count($inter)){
				$this->addGroup($name, $data['attrs']);
			}
		}
	}
}

?>
