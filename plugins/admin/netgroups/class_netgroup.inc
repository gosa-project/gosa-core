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

class netgroup extends plugin {

    var $cn = "";
    var $description = "";
	var $objectInfo = array();
	var $ignore_account = TRUE;

    var $netgroups = array();
    var $netgroups_used_by_some = array();

	var $triples = array();
    var $triples_used_by_some = array();
    
    var $nisNetgroupTriple = array();
    var $memberNisNetgroup = array();

    /* Helpers */
    var $base = "";
    var $orig_dn = "";
    var $orig_cn = "";
    var $orig_base = "";
    var $userSelect = FALSE;
    var $dialog;
    var $view_logged = FALSE;
    var $baseSelector;

    /* attribute list for save action */
    var $attributes = array("cn", "description", "nisNetgroupTriple", "memberNisNetgroup");
    var $objectclasses = array("top", "nisNetgroup");
    var $multiple_support = TRUE;

	//var $CopyPasteVars = array("");


	/* Constructor which load object attributes aso.
 	 * */
    function __construct(&$config, $dn= NULL) {

        plugin::__construct($config, $dn);
		$this->objectInfo = array("triple" => array(), "groups" => array());
        $this->orig_dn = $dn;
        $this->orig_cn = $this->cn;

		// Extract triple informations
		// . (machine, user, domain)
        if (isset($this->attrs['nisNetgroupTriple']["count"])) {
            $tmp = array();
            for ($i = 0; $i < $this->attrs['nisNetgroupTriple']['count']; $i++) {
				if (preg_match("/^\((\S+),\-?,(\S*)\)$/", $this->attrs['nisNetgroupTriple'][$i], $matches)) {

					// Skip non-fqdn entries
					if(strpos($matches[1], ".") !== FALSE){
						$this->addTriple($matches[1], NULL, $matches[2]);
					}
                }
                if (preg_match("/^\(\-?,(\S+),(\S*)\)$/", $this->attrs['nisNetgroupTriple'][$i], $matches)) {
					$this->addTriple(NULL, $matches[1], $matches[2]);
                }
            }
		}

        /* Get netgroups list  and put it in $this->netgroups */
		if (isset($this->attrs['memberNisNetgroup'][0])) {
            for ($i = 0; $i < $this->attrs['memberNisNetgroup']['count']; $i++) {
				$this->addNisGroup($this->attrs['memberNisNetgroup'][$i]);
            }
        }

        if ($this->dn == "new") {
            if (session::is_set('CurrentMainBase')) {
                $this->base = session::get('CurrentMainBase');
            } else {
                $ui = get_userinfo();
                $this->base = dn2base($ui->dn);
            }
        } else {

            /* Get object base */
            $this->base = preg_replace("/^[^,]+," . preg_quote(get_ou("netgroupSystem", "netgroupSystemRDN"),'/') . "/i", "", $this->dn);
        }
        $this->orig_base = $this->base;

        /* Instanciate base selector */
        $this->baseSelector = new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        // Prepare lists
        $this->memberList = new sortableListing(array(),array(), FALSE);
        $this->memberList->setDeleteable(true);
        $this->memberList->setInstantDelete(false);
        $this->memberList->setEditable(false);
        $this->memberList->setWidth("100%");
        $this->memberList->setHeight("300px");
        $this->memberList->setColspecs(array('*','*','*','*'));
        $this->memberList->setHeader(array("-",_("Name"),_("Description"),_("Domain")));
        $this->memberList->setDefaultSortColumn(1);
    }


	/* Generate the plugin gui, handle posts and fill lists.
 	 * */
    function execute() {
	
		// Call parent execute 
        plugin::execute();

		// Log view 
        if ($this->is_account && !$this->view_logged) {
            $this->view_logged = TRUE;
            new log("view", "netgroups/" . get_class($this), $this->dn);
        }

		// Set list Acls
        $this->memberList->setAcl($this->getacl("triples"));

        // Add selected objects
		if (isset($_POST["edit_membership"]) && preg_match("/w/", $this->getacl("triples"))) {
			$skip = array();
			foreach($this->triples as $key => $data){
				if(isset($this->objectInfo['triple'][$key]['data'])){
					$skip[] = $this->objectInfo['triple'][$key]['data']['dn'];
				}
			}
			foreach($this->netgroups as $key => $data){
				if(isset($this->objectInfo['groups'][$key]['data'])){
					$skip[] = $this->objectInfo['groups'][$key]['data']['dn'];
				}
			}
			$skip[] = $this->dn;
			session::set('filterBlacklist', array("dn" => $skip));
			$this->userSelect = new tripleSelect($this->config, get_userinfo(), $this->dn);
            $this->dialog = TRUE;
        }

        // Close add-dialog
        if (isset($_POST["add_triples_cancel"]) || isset($_POST['cancel-abort'])) {
            $this->userSelect = NULL;
            $this->dialog = FALSE;
        }

        // Add to netgroup
        if (isset($_POST['add_triples_finish']) || isset($_POST['ok-save']) && $this->userSelect) {
            $users = $this->userSelect->detectPostActions();
            if (isset($users['targets'])) {
                $headpage = $this->userSelect->getHeadpage();
                foreach ($users['targets'] as $dn) {
                    $attrs = $headpage->getEntry($dn);
                    $value = "";
					$objectClass = "posixAccount";
					if(in_array("posixAccount", $attrs['objectClass'])){
						$this->addTriple(NULL, $attrs['uid'][0], '', $attrs);
					}else if(in_array("nisNetgroup", $attrs['objectClass'])){
						$this->addNisGroup($attrs['cn'][0]);
					}else{
						$this->addTriple($attrs['cn'][0], NULL, '', $attrs);
					}
                }
            }
            $this->userSelect = NULL;
            $this->dialog = FALSE;
        }

		// Render the user selection
        if ($this->userSelect) {
            return($this->userSelect->execute());
        }

		// Assign smarty vars
		$smarty = get_smarty();
		$smarty->assign("base", $this->baseSelector->render());
		foreach ($this->attributes as $val) {
			$smarty->assign("$val", $this->$val);
		}
        $tmp = $this->plInfo();
        foreach ($tmp['plProvidedAcls'] as $name => $translation) {
            $smarty->assign($name . "ACL", $this->getacl($name));
        }
        if ($this->acl_is_writeable("base")) {
            $smarty->assign("baseSelect", true);
        } else {
            $smarty->assign("baseSelect", false);
        }
        $smarty->assign("multiple_support", $this->multiple_support_active);
        foreach ($this->attributes as $val) {
            if (in_array($val, $this->multi_boxes)) {
                $smarty->assign("use_" . $val, TRUE);
            } else {
                $smarty->assign("use_" . $val, FALSE);
            }
        }
        foreach (array("base") as $val) {
            if (in_array($val, $this->multi_boxes)) {
                $smarty->assign("use_" . $val, TRUE);
            } else {
                $smarty->assign("use_" . $val, FALSE);
            }
        }

		// Fill the list
		$data = $lData = array();
        foreach($this->triples_used_by_some as $key => $pattern){
			$s_key = "t_{$key}";
			$data[$s_key] = $pattern;
			$list_data = $this->objectInfo['triple'][$key]['list_data'];
			for($i=1;$i<4;$i++){
				$list_data[$i] = "<font color='grey'>".$list_data[$i]."</font>";
			}
            $lData[$s_key] = array('data'=> $list_data);
        }
        foreach($this->netgroups_used_by_some as $key => $pattern){
			$s_key = "g_{$key}";
			$data[$s_key] = $pattern;
			$list_data = $this->objectInfo['groups'][$key]['list_data'];
			for($i=1;$i<4;$i++){
				$list_data[$i] = "<font color='grey'>".$list_data[$i]."</font>";
			}
            $lData[$s_key] = array('data'=> $list_data);
        }
		foreach($this->netgroups as $key => $pattern){
			$s_key = "g_{$key}";
            $data[$s_key] = $pattern;
            $lData[$s_key] = array('data'=> $this->objectInfo['groups'][$key]['list_data']);
        }
		foreach($this->triples as $key => $pattern){
			$s_key = "t_{$key}";
            $data[$s_key] = $pattern;
            $lData[$s_key] = array('data'=> $this->objectInfo['triple'][$key]['list_data']);
		}
		$this->memberList->setListData($data, $lData);
		$this->memberList->update();
		$smarty->assign("memberList", $this->memberList->render());
        return($smarty->fetch(get_template_path('generic.tpl', TRUE)));
    }


	/* Returns an image for the given objectClass array.
 	 * This is used to generated the list enties
 	 * */
	function getImageForOc($objectClasses){
        $theme = getThemeName();

        switch ($theme) {
            case 'classic':
                if(in_array("goServer", $objectClasses)){
                    return(image("plugins/systems/images/select_server.png"));
                }
                if(in_array("gotoWorkstation", $objectClasses)){
                    return(image("plugins/systems/images/select_workstation.png"));
                }
                if(in_array("gotoTerminal", $objectClasses)){
                    return(image("plugins/systems/images/select_terminal.png"));
                }
                if(in_array("ieee802Device", $objectClasses)){
                    return(image("plugins/systems/images/select_component.png"));
                }
                if(in_array("posixAccount", $objectClasses)){
                    return(image("plugins/users/images/select_user.png"));
                }
                if(in_array("nisNetgroup", $objectClasses)){
                    return(image("plugins/netgroups/images/select_netgroup.png"));
                }
                break;
            
            default:
                if(in_array("goServer", $objectClasses)){
                    return(image("<i class='material-icons'>dns</i>"));
                }
                if(in_array("gotoWorkstation", $objectClasses)){
                    return(image("<i class='material-icons'>computer</i>"));
                }
                if(in_array("gotoTerminal", $objectClasses)){
                    return(image("<i class='material-icons'>monitor</i>"));
                }
                if(in_array("ieee802Device", $objectClasses)){
                    return(image("<i class='material-icons'>inventory_2</i>"));
                }
                if(in_array("posixAccount", $objectClasses)){
                    return(image("<i class='material-icons'>person</i>"));
                }
                if(in_array("nisNetgroup", $objectClasses)){
                    return(image("<i class='material-icons'>connect_without_contact</i>"));
                }
                break;
        }

	}


	/* This removes a triple from the list/object
 	 * */
	function removeTriple($name){
		if(isset($this->triples[$name])){
			unset($this->triples[$name]);
		}
		if(isset($this->triples_used_by_some[$name])){
			unset($this->triples_used_by_some[$name]);
		}
		unset($this->objectInfo['triple'][$name]);
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


	/* Adds a new triple the the list/object
 	 * This method also tries to resolve the hosts domain-name.
 	 * */
	function addTriple($host, $user, $nis_domain, $attrs = NULL)
	{
		$fqdn = "";
		$domain = "";
		$ldap = $this->config->get_ldap_link();
		$ldap->cd($this->config->current['BASE']);
		if($host){
			$attributes = array("dn", "cn", "description", "objectClass");

			// Split name into host and domain
			if(strpos($host, ".") === FALSE){
				$domain = $this->getDNSZoneForHost($host);
				if(!$domain){
					msg_dialog::display(_("Failed to add triple"), sprintf(_("Unable to detect FQDN for host '%s'!"), $host), ERROR_DIALOG);
					return;
				}
			}else{
				$domain = preg_replace("/^[^\.]*+\./", "", $host);
				$host = preg_replace("/\..*/", "", $host);
			}

			$fqdn = "{$host}.{$domain}";
			if(!$attrs){
				$ldap->search("(&(|(objectClass=gotoTerminal)(objectClass=gotoWorkstation)(objectClass=goServer)".
					"(objectClass=GOhard)(objectClass=ieee802Device))(|(cn=".normalizeldap($host).")(cn=".normalizeldap($fqdn).")))");
				if($ldap->count()){
					$attrs = $ldap->fetch();
				}
			}
			$name = $fqdn;
		}else if($user){
			$attributes = array("dn", "cn", "uid", "sn", "givenName", "objectClass");
			if(!$attrs){
				$ldap->search("(&(objectClass=posixAccount)(uid=".normalizeldap($user)."))");
				if($ldap->count()){
					$attrs = $ldap->fetch();
				}
			}
			$name = $user;
		}else{
			return "?";
		}

		$this->removeTriple($name);
		$this->triples[$name] = array(
			"host" => $host,
			"user" => $user,
			"domain" => $domain,
			"fqdn" => $fqdn,
			"nis_domain" => "");

		if($attrs){
			$type = $this->getImageForOc($attrs['objectClass']);
			$this->objectInfo['triple'][$name]["data"] = $attrs;
			if($user){
				$this->objectInfo['triple'][$name]["list_data"] = array($type, $attrs['uid'][0], $attrs['cn'][0], $domain);
			}else{
				$desc = "&nbsp";
				if(isset($attrs['description'][0])){
					$desc = $attrs['description'][0];
				}
				if(!$domain){
					$domain = "&nbsp;";
				}
				$this->objectInfo['triple'][$name]["list_data"] = array($type, $host, $desc, $domain);
			}
		}else{
			$this->objectInfo['triple'][$name]["list_data"] = array("?", $name, "&nbsp;", $domain);
		}
	}	


	/* Removes a nis-group from this object
 	 * */
	function removeNisGroup($name){
		if(isset($this->netgroups[$name])){
			unset($this->netgroups[$name]);
		}
		if(isset($this->netgroups_used_by_some[$name])){
			unset($this->netgroups_used_by_some[$name]);
		}
		unset($this->objectInfo['groups'][$name]);
	}


	/* Adds a new nis-group to this object
 	 * */
	function addNisGroup($name)
	{
		$ldap = $this->config->get_ldap_link();
		$ldap->cd($this->config->current['BASE']);
		$attributes = array("dn", "cn", "description", "objectClass");
		$ldap->search("(&(objectClass=nisNetgroup)(cn=".normalizeldap($name)."))");
		$this->removeNisGroup($name);
		$this->netgroups[$name] = $name;
		if($ldap->count()){
			while($attrs = $ldap->fetch()){
				$desc = "&nbsp";
				if(isset($attrs['description'][0])){
					$desc = $attrs['description'][0];
				}
				$type = $this->getImageForOc($attrs['objectClass']);
				$this->objectInfo['groups'][$name]["data"] = $attrs;
				$this->objectInfo['groups'][$name]["list_data"] = array($type, $attrs['cn'][0], $desc, "&nbsp;");
			}
		}else{
			$this->objectInfo['groups'][$name]["list_data"] = array("?", $name, "&nbsp;", "&nbsp;");
		}
	}


	/* Delete this object and its references
 	 * */
    function remove_from_parent() {
        plugin::remove_from_parent();

		// Remove this object
        $ldap = $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }

        new log("remove", "netgroups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());

        // Delete references to object netgroups
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaGroupOfNames)(memberNisNetgroup=" . LDAP::prepare4filter($this->dn) . "))", array("cn"));
        while ($ldap->fetch()) {
            $og = new ogroup($this->config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save();
        }

        // Remove ACL dependencies, too
        acl::remove_acl_for($this->dn);

        // Send signal to the world that we've done
        $this->handle_post_events("remove");
    }


	/* Act and HTML posts
 	 * */
    function save_object() {

        if (isset($_POST['nisnetgroupedit'])) {

			// Act on list actions (delete)
			$this->memberList->save_object();
			$action = $this->memberList->getAction();
			if(isset($action['action']) && $action['action'] == 'delete'){
				foreach($action['targets'] as $id){
					$entry = $this->memberList->getKey($id);
					if(preg_match("/^t_/", $entry)){
						$this->removeTriple(preg_replace("/^t_/","",$entry));
					}else{
						$this->removeNisGroup(preg_replace("/^g_/","",$entry));
					}
				}
			}

            /* Create a base backup and reset the
              base directly after calling plugin::save_object();
              Base will be set seperatly a few lines below */
            $base_tmp = $this->base;
            plugin::save_object();
            $this->base = $base_tmp;

            /* Refresh base */
            if ($this->acl_is_moveable($this->base)) {
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base = $this->baseSelector->getBase();
                    $this->is_modified = TRUE;
                }
            }
        }
    }


	/* Save modification back to the LDAP
 	 * */
    function save() {

        plugin::save();

		$this->attrs['memberNisNetgroup'] = array();
		$this->attrs['nisNetgroupTriple'] = array();
		foreach($this->triples as $key => $data){
			$host 		= $data['host'];
			$user 		= $data['user'];
			$nis_domain = $data['nis_domain'];
			$domain 	= $data['domain'];
			$fqdn 		= $data['fqdn'];
			if($host){
				$this->attrs['nisNetgroupTriple'][] = "({$host},{$user},{$nis_domain})";
				$this->attrs['nisNetgroupTriple'][] = "({$fqdn},{$user},{$nis_domain})";
			}else{
				$this->attrs['nisNetgroupTriple'][] = "({$host},{$user},{$nis_domain})";
			}
		}

        if (count($this->netgroups)) {
            $this->attrs['memberNisNetgroup'] = array_values(array_unique($this->netgroups));
        }

        /* New accounts need proper 'dn', propagate it to remaining objects */
        if ($this->dn == 'new') {
            $this->dn = 'cn='.$this->cn.','.get_ou("netgroupSystem", "netgroupSystemRDN").$this->base;
        }

        /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
          new entries. So do a check first... */
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($this->dn, array('dn'));
        if ($ldap->fetch()) {
            $mode = "modify";
        } else {
            $mode = "add";
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }

        /* Write back to ldap */
        $ldap->cd($this->dn);
        $this->cleanup();
		$ldap->$mode($this->attrs);

        // Update ACL dependencies
        if ($this->dn != $this->orig_dn && $this->orig_dn != "new") {
            $tmp = new acl($this->config, $this->parent, $this->dn);
            $tmp->update_acl_membership($this->orig_dn, $this->dn);
        }

		// Log action
        if ($this->initially_was_account) {
            new log("modify", "netgroups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        } else {
            new log("create", "netgroups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        }

		// Check ldap result
        $ret = 0;
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
            $ret = 1;
        }

        // Handle post events
        $this->handle_post_events($mode);

        return ($ret);
    }


	/* Check input values
 	 * */
    function check() {

        $message = plugin::check();
        if ($this->base != "") {
            $new_dn = 'cn='.$this->cn.','.get_ou("netgroupSystem", "netgroupSystemRDN").$this->base;
        } else {
            $new_dn = $this->dn;
        }

        if ($this->cn == "" && $this->acl_is_writeable("cn")) {
            $message[] = msgPool::required(_("Name"));
        }

        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[] = msgPool::check_base();
        }

        // Check for valid group name
        if (!tests::is_uid($this->cn)) {
            if (strict_uid_mode ()) {
                $message[] = msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/");
            } else {
                $message[] = msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/i");
            }
        }

        // Check if a wrong base was supplied
        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[] = msgPool::check_base();
        }

        // Check netgroup loops and if user is in any netgroup
        $nisnetmap = array();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(objectClass=nisNetgroup)", array("cn", "memberNisNetgroup", "nisNetgroupTriple"));
        while ($attrs = $ldap->fetch()) {
            $nisnetmap[$attrs['cn'][0]] = array();
            $nisnetmap[$attrs['cn'][0]]['memberNisNetgroup'] = array();
            $nisnetmap[$attrs['cn'][0]]['nisNetgroupTriple'] = array();
            if(isset($attrs['memberNisNetgroup'])){
                foreach ($attrs['memberNisNetgroup'] as $val) {
                    $nisnetmap[$attrs['cn'][0]]['memberNisNetgroup'][] = $val;
                }
            }
            if(isset($attrs['nisNetgroupTriple'])){
                foreach ($attrs['nisNetgroupTriple'] as $val) {
                    $nisnetmap[$attrs['cn'][0]]['nisNetgroupTriple'][] = $val;
                }
            }
		}

        $checkmsg = $this->checkNisNetgroup($nisnetmap);
        if (count($checkmsg) != 0)
            $message[] = $checkmsg[0];


        // Check if we are allowed to create or move this object
        if (!$this->orig_dn == "new" ||
                $this->orig_base != $this->base ||
                $this->cn != $this->orig_cn) {

            if ($this->orig_dn == "new" && !$this->acl_is_createable($this->base)) {
                $message[] = msgPool::permCreate();
            } elseif ($this->orig_dn != "new" && !$this->acl_is_moveable($this->base)) {
                $message[] = msgPool::permMove();
            }
        }
        return ($message);
    }

	/* Check for loops in nis-group assignment
 	 * */
	function checkNisNetgroup($nisnetmap, $timeline=FALSE, $checknng=FALSE) 
	{
        if ($timeline == FALSE) {
            $timeline = array();
            foreach ($this->netgroups as $key => $value) {
				$timeline[] = $key;
				if(isset($nisnetmap[$key])){
					foreach ($nisnetmap[$key]['memberNisNetgroup'] as $val) {
						if(isset($nisnetmap[$val])){
							$result = $this->checkNisNetgroup($nisnetmap, $timeline, $nisnetmap[$val]);
							if (count($result) != 0) {
								return($result);
							}
						}
					}
                }
            }
        } else {
            foreach ($nisnetmap[$val]['memberNisNetgroup'] as $key) {
                if (array_search($key, $timeline) !== FALSE) {
                    $result = array();
                    $result[] = _("NIS Netgroup Error") . "<b>" . _("Error: There are a loop with NIS Netgroups") . " [" . " " . $key .
                            "] " . "<br>" .
                            _("IMPORTANT: Check the members of NIS Netgroup") . "</b>";
                    return($result);
                }
                $timeline[] = $key;
                $result = $this->checkNisNetgroup($nisnetmap, $timeline, $nisnetmap[$val]);
                if (count($result) != 0) {
                    return($result);
                }
            }
        }
        return(array());
    }

//TODO: check
    function getCopyDialog() {
        $ret = array();
        return($ret);
    }

//TODO: check
    function saveCopyDialog() {
        if (isset($_POST['cn'])) {
            $this->cn = $_POST['cn'];
        }
    }

	//TODO: check ACL!
	/* Return plugin informations for acl handling  */
	static function plInfo() {
		return (array(
			"plShortName" => _("Generic"),
			"plDescription" => _("Generic NIS Netgroup settings"),
			"plSelfModify" => FALSE,
			"plDepends" => array(),
			"plPriority" => 0,
			"plSection" => array("administration"),
			"plCategory" => array("netgroups" => array("objectClass" => "nisNetgroup", "description" => _("NIS Netgroups"))),
			"plProvidedAcls" => array(
				"cn" => _("Name"),
				"description" => _("Description"),
				"base" => _("Base"),
				"memberCn" => _("NIS Netgroup members"))
			));
	}


	/* Act on HTML-post when in multiple-edit mode.
 	 * */
	function multiple_save_object() 
	{

        if (isset($_POST['nisnetgroup_mulitple_edit'])) {

			$this->memberList->save_object();
			$action = $this->memberList->getAction();
			if(isset($action['action']) && $action['action'] == 'delete'){
				foreach($action['targets'] as $id){
					$entry = $this->memberList->getKey($id);
					if(preg_match("/^t_/", $entry)){
						$this->removeTriple(preg_replace("/^t_/","",$entry));
					}else{
						$this->removeNisGroup(preg_replace("/^g_/","",$entry));
					}
				}
			}

            /* Create a base backup and reset the
              base directly after calling plugin::save_object();
              Base will be set seperatly a few lines below */
            $base_tmp = $this->base;
            plugin::multiple_save_object();
            plugin::save_object();
            $this->base = $base_tmp;

            foreach (array("base") as $attr) {
                if (isset($_POST['use_' . $attr])) {
                    $this->multi_boxes[] = $attr;
                }
            }

            /* Refresh base */
            if ($this->acl_is_moveable($this->base)) {
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base = $this->baseSelector->getBase();
                    $this->is_modified = TRUE;
                }
            }
        }
    }


	/* Returns the modified values after a multiple-edit session
 	 * */
	function get_multi_edit_values() 
	{
        $ret = plugin::get_multi_edit_values();
        foreach (array("base") as $attr) {
            if (in_array($attr, $this->multi_boxes)) {
                $ret[$attr] = $this->$attr;
            }
        }
        $ret['triples'] = $this->triples;
        $ret['triples_used_by_some'] = $this->triples_used_by_some;
        $ret['netgroups'] = $this->netgroups;
        $ret['netgroups_used_by_some'] = $this->netgroups_used_by_some;
        return($ret);
    }


    function multiple_execute() {
        return($this->execute());
    }
    
	
	/* Load values for multiple-edit
	 * */
    function init_multiple_support($attrs, $all) {
        plugin::init_multiple_support($attrs, $all);
        $this->members=array();
        $this->allusers=array();
        $this->users_used_by_some=array();
        $this->netgroups = array();
		$this->netgroups_used_by_some = array();

        // Load nis-groups used by some groups
        if (isset($all['memberNisNetgroup'][0])) {
            for ($i = 0; $i < $all['memberNisNetgroup']['count']; $i++) {
                $this->addNisGroup($all['memberNisNetgroup'][$i]);
            }
        }
		$this->netgroups_used_by_some = $this->netgroups;

		// Load nis-groups by all edited-groups 
		$this->netgroups = array();
        if (isset($attrs['memberNisNetgroup'][0])) {
            for ($i = 0; $i < $attrs['memberNisNetgroup']['count']; $i++) {
                $this->addNisGroup($attrs['memberNisNetgroup'][$i]);
            }
        }

        $this->triples = array();
        $this->triples_used_by_some = array();

		if (isset($all['nisNetgroupTriple'])) {
            $tmp = array();
            for ($i = 0; $i < $all['nisNetgroupTriple']['count']; $i++) {
				if (preg_match("/^\((\S+),\-?,(\S*)\)$/", $all['nisNetgroupTriple'][$i], $matches)) {

					// Skip non-fqdn entries
					if(strpos($matches[1], ".") !== FALSE){
						$this->addTriple($matches[1], NULL, $matches[2]);
					}
                }
                if (preg_match("/^\(\-?,(\S+),(\S*)\)$/", $all['nisNetgroupTriple'][$i], $matches)) {
					$this->addTriple(NULL, $matches[1], $matches[2]);
                }
            }
		}
		$this->triples_used_by_some = $this->triples;
		$this->triples = array();

        if (isset($attrs['nisNetgroupTriple'])) {
            $tmp = array();
            for ($i = 0; $i < $attrs['nisNetgroupTriple']['count']; $i++) {
				if (preg_match("/^\((\S+),\-?,(\S*)\)$/", $attrs['nisNetgroupTriple'][$i], $matches)) {

					// Skip non-fqdn entries
					if(strpos($matches[1], ".") !== FALSE){
						$this->addTriple($matches[1], NULL, $matches[2]);
					}
                }
                if (preg_match("/^\(\-?,(\S+),(\S*)\)$/", $attrs['nisNetgroupTriple'][$i], $matches)) {
					$this->addTriple(NULL, $matches[1], $matches[2]);
                }
            }
		}
    }


	/* Initialize plugin for multiple edit mode
 	 * */
	function set_multi_edit_values($attrs) 
	{
        $triples = array();
        $netgroups = array();

        // Update netgroupMembership, keep optinal netgroup
        foreach ($attrs['triples_used_by_some'] as $uid => $value) {
            if (in_array($uid, array_keys($this->triples))) {
                $triples[$uid] = $value;
            }
        }
        foreach ($attrs['netgroups_used_by_some'] as $uid => $value) {
            if (in_array($uid, array_keys($this->netgroups))) {
                $netgroups[$uid] = $value;
            }
        }

        // Update netgroupMembership, add forced netgroups
        foreach ($attrs['triples'] as $uid => $value) {
            $triples[$uid] = $value;
        }
        foreach ($attrs['netgroups'] as $uid => $value) {
            $netgroups[$uid] = $value;
		}
		plugin::set_multi_edit_values($attrs);
        $this->triples = $triples;
        $this->netgroups = $netgroups;
    }
}
?>
