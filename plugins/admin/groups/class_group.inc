<?php

/**
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2025 GONICUS GmbH
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

class group extends plugin
{
    // Group attributes
    var $cn = '';
    var $description = '';
    var $gidNumber = '';
    var $memberUid = [];
    var $memberUid_used_by_some = [];
    var $member = [];
    var $member_used_by_some = [];

    // Helpers
    var $base = '';
    var $force_gid = false;
    var $fon_group = false;
    var $smbgroup = false;
    var $groupType = false;
    var $sambaSID = '';
    var $sambaDomainName = 'DEFAULT';
    var $SID = '';
    var $ridBase = 0;
    var $members = [];
    var $users = [];
    var $allusers = [];
    var $saved_gidNumber = '';
    var $oldgroupType = '';
    var $orig_dn = '';
    var $orig_cn = '';
    var $orig_base = '';
    var $has_mailAccount = false;
    var $userSelect;
    var $nagios_group = false;
    var $sambaGroupType;
    var $dialog;
    var $rfc2307bis = false;
    var $OnlyShowFirstEntries = 200;
    var $dnMapping = [];
    var $view_logged = false;
    var $allowGroupsWithSameNameInOtherSubtrees = true;
    var $baseSelector;
    var $dns = [];

    // attribute list for save action
    var $attributes = ['cn', 'description', 'gidNumber', 'memberUid', 'sambaGroupType', 'sambaSID'];
    var $objectclasses = ['top', 'posixGroup'];
    var $CopyPasteVars  = array('force_gid', 'fon_group', 'smbgroup', 'groupType', 'sambaSID', 'sambaDomainName', 'SID', 'nagios_group', 'sambaGroupType');
    var $multiple_support = true;

    // Lists 
    private $memberList;    // Single group editing
    private $commonList;    // (Multiple support active)
    private $partialList;   // (Multiple support active)

    var $trustModeDialog;
    var $rfc2307bisDisableNestedGroups = false;

    function __construct($config, $dn = null)
    {
        // Set rfc2307bis flag
        if ($config->get_cfg_value('core', "rfc2307bis") == "true") {
            $this->rfc2307bis = true;
            $this->attributes[] = "member";
            $this->objectclasses = ['top', 'groupOfNames', 'posixGroup'];
        }

        $this->rfc2307bisDisableNestedGroups = $config->get_cfg_value('core', 'rfc2307bisDisableNestedGroups');

        parent::__construct($config, $dn);

        $this->trustModeDialog = new trustModeDialog($this->config, $this->dn, null);
        $this->trustModeDialog->setAcl('groups/group');

        // Load attributes depending on the samba version
        $this->orig_dn = $dn;
        $this->orig_cn = $this->cn;

        // Get member list
        $this->loadGroupMembers();

        // Save gidNumber for later use
        if (isset($this->attrs['gidNumber'])) {
            $this->saved_gidNumber = $this->attrs['gidNumber'][0];
        }

        // Is a samba group?
        if (isset($this->attrs['objectClass'])) {
            if (array_search('sambaGroupMapping', $this->attrs['objectClass']) == false) {
                $this->smbgroup = false;
            } else {
                $this->smbgroup = true;
                if (isset($this->attrs['sambaSID'])) {
                    $this->sambaSID = $this->attrs['sambaSID'][0];
                }
            }
            if (array_search('goFonPickupGroup', $this->attrs['objectClass']) == false) {
                $this->fon_group = false;
            } else {
                $this->fon_group = true;
            }
            if (array_search('nagiosContactGroup', $this->attrs['objectClass']) == false) {
                $this->nagios_group = false;
            } else {
                $this->nagios_group = true;
            }
        }

        // Set mail flag
        if (isset($this->attrs['objectClass']) && in_array_strict('gosaMailAccount', $this->attrs['objectClass'])) {
            $this->has_mailAccount = true;
        }

        // Get samba Domain in case of samba 3
        if ($this->sambaSID != '') {
            $this->SID = preg_replace("/-[^-]+$/", '', $this->sambaSID);
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(objectClass=sambaDomain)(sambaSID=$this->SID))", array('sambaAlgorithmicRidBase'));
            if ($ldap->count() != 0) {
                $attrs = $ldap->fetch();
                if (isset($attrs['sambaAlgorithmicRidBase'])) {
                    $this->ridBase = $attrs['sambaAlgorithmicRidBase'][0];
                } else {
                    $this->ridBase = $this->config->get_cfg_value('core', 'sambaRidBase');
                }

                // Get domain name for SID
                $this->sambaDomainName = 'DEFAULT';
                foreach ($this->config->data['SERVERS']['SAMBA'] as $key => $val) {
                    if ($val['SID'] == $this->SID) {
                        $this->sambaDomainName = $key;
                        break;
                    }
                }
            } else {
                if ($this->config->get_cfg_value('core', "sambaRidBase") != '') {
                    $this->sambaDomainName = 'DEFAULT';
                    $this->ridBase = $this->config->get_cfg_value('core', "sambaRidBase");
                    $this->SID = $this->config->get_cfg_value('core', 'sambaSID');
                } else {
                    msg_dialog::display(_("Configuration error"), _("Cannot find group SID in your configuration!"), ERROR_DIALOG);
                }
            }

            // Get group type
            $this->groupType = (int)substr(strrchr($this->sambaSID, "-"), 1);
            if ($this->groupType < 500 || $this->groupType > 553) {
                $this->groupType = 0;
            }
            $this->oldgroupType = $this->groupType;
        }

        if ($this->dn === 'new') {
            if (session::is_set('CurrentMainBase')) {
                $this->base = session::get('CurrentMainBase');
            } else {
                $ui = get_userinfo();
                $this->base = dn2base($ui->dn);
            }
        } else {
            // Get object base 
            $this->base = preg_replace("/^[^,]+," . preg_quote(get_groups_ou(), '/') . "/i", '', $this->dn);
        }
        $this->orig_base = $this->base;

        // This is always an account 
        $this->is_account = true;

        // Instanciate base selector 
        $this->baseSelector = new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        // Prepare lists
        $this->memberList = new sortableListing();
        $this->memberList->setDeleteable(true);
        $this->memberList->setInstantDelete(false);
        $this->memberList->setEditable(false);
        $this->memberList->setWidth('100%');
        $this->memberList->setHeight('300px');
        $this->memberList->setHeader(array('~', _('Surname'), _('Given name'), _('UID')));
        $this->memberList->setColspecs(array('20px', '*', '*', '*', '20px'));
        $this->memberList->setDefaultSortColumn(1);

        // Set the dns of selected Group(s)
        if (!$this->dns[0]) {
            $this->dns[] = $this->dn;
        }

        $this->reload();
    }

    function execute()
    {
        global $config;
        // Call parent execute 
        parent::execute();

        $smarty = get_smarty();

        // Log view posixGroup
        if ($this->is_account && !$this->view_logged) {
            $this->view_logged = true;
            new log('view', 'groups/' . get_class($this), $this->dn);
        }

        // Do we represent a valid group? 
        if (!$this->is_account && $this->parent === null) {
            $smarty->assign('errorMsg', msgPool::noValidExtension(_('No valid group')));
            return $smarty->fetch(get_template_path('invalid-group.tpl', true));
        }

        // Act on list events 
        foreach (array('memberList', 'commonList', 'partialList') as $list) {

            // Check if list is available, depends on multi- or sinlge- group editing. 
            if ($this->$list) {
                $this->$list->save_object();
                $action = $this->$list->getAction();
                if ($action['action'] === 'delete' && $this->getMemberAcl("/w/")) {
                    foreach ($action['targets'] as $id) {
                        $value = $this->$list->getKey($id);
                        $this->removeGroupMember($value);
                    }
                }
            }
        }

        // Add objects?
        if (isset($_POST['edit_membership']) && $this->getMemberAcl("/w/")) {
            $this->userSelect = new userSelect($config, get_userinfo());
        }

        // Add objects finished? 
        if (isset($_POST['add_users_cancel']) || isset($_POST['cancel-abort'])) {
            $this->userSelect = null;
        }

        // Add user to group 
        if (isset($_POST['add_users_finish']) || isset($_POST['ok-save']) && $this->userSelect) {
            $users = $this->userSelect->detectPostActions();
            if (isset($users['targets'])) {
                $headpage = $this->userSelect->getHeadpage();
                foreach ($users['targets'] as $dn) {
                    $attrs = $headpage->getEntry($dn);
                    $value = $attrs['uid'][0];
                    $this->addUser($value);
                    $this->members["$value"] = $this->allusers[$value];
                    // is this correct reloading $attrs?
                    $this->reload();
                }
            }
            $this->userSelect = null;
        }

        // Handle trust mode dialog
        $this->dialog = false;
        $trustModeDialog = $this->trustModeDialog->execute();
        if ($this->trustModeDialog->trustSelect) {
            $this->dialog = true;
            return ($trustModeDialog);
        }

        $smarty->assign('trustModeDialog', $trustModeDialog);
        $smarty->assign('nagios', $config->pluginEnabled('nagiosAccount') && class_available('nagiosAccount'));
        $smarty->assign('pickupGroup', $config->pluginEnabled("phoneAccount") && class_available("phoneAccount"));

        // Manage object add dialog 
        if ($this->userSelect) {
            $this->dialog = true;
            return $this->userSelect->execute();
        }

        // Create base acls 
        $smarty->assign('base', $this->baseSelector->render());

        $domains = [];
        foreach ($config->data['SERVERS']['SAMBA'] as $name => $content) {
            $domains[$name] = $name;
        }
        $smarty->assign('sambaDomains',     set_post($domains));
        $smarty->assign('sambaDomainName',  set_post($this->sambaDomainName));
        $groupTypes = array(
            0 => _('Samba group'),
            512 => _('Domain administrators'),
            513 => _('Domain users'),
            514 => _('Domain guests')
        );

        // Don't loose special groups! If not key'ed above, add it to the combo box... 	
        if ($this->groupType >= 500 && $this->groupType <= 553 && !isset($groupTypes[$this->groupType])) {
            $groupTypes[$this->groupType] = sprintf(_('Special group (%d)'), $this->groupType);
        }

        $smarty->assign('groupTypes', set_post($groupTypes));
        $smarty->assign('groupType',  set_post($this->groupType));

        // Members and users 
        if (!$this->multiple_support_active) {
            $this->memberList->setAcl($this->getacl('memberUid'));
            $data = $lData = [];
            foreach ($this->members as $uid => $member) {
                $data[$uid] = $member;
                $givenName = $sn = _('Unknown');
                if (isset($member['sn']) && isset($member['sn'][0])) $sn = $member['sn'][0];
                if (isset($member['givenName']) && isset($member['givenName'][0])) $givenName = $member['givenName'][0];
                $image = image('images/false.png');
                if (isset($member['sn'])) {
                    $image = '<i class="material-icons">person</i>';
                }
                $lData[$uid] = array('data' => array($image, $sn, $givenName, $uid));
            }
            $this->memberList->setListData($data, $lData);
            $bool = $this->isRestrictedByDynGroup();
            $this->memberList->setDeleteable(!$bool);
            $this->memberList->update();
            $smarty->assign('memberList', $this->memberList->render());
        } else {

            $this->commonList->setAcl($this->getacl('memberUid'));
            $this->partialList->setAcl($this->getacl('memberUid'));

            $data = $lData = [];
            foreach ($this->memberUid as $uid => $member) {
                $member = $this->members[$member];
                $data[$uid] = $member;
                $givenName = $sn = _('Unknown');
                if (isset($member['sn'][0])) $sn = $member['sn'][0];
                if (isset($member['givenName'][0])) $givenName = $member['givenName'][0];
                $image = image('images/false.png');
                if (isset($member['sn'])) {
                    $image = '<i class="material-icons">person</i>';
                }
                $lData[$uid] = array('data' => array($image, $sn, $givenName, $uid));
            }
            $this->commonList->setListData($data, $lData);
            $this->commonList->update();
            $smarty->assign('commonList', $this->commonList->render());

            $data = $lData = [];
            foreach ($this->memberUid_used_by_some as $uid => $member) {
                $member = (isset($this->members[$member])) ? $this->members[$member] : null;
                $data[$uid] = $member;
                $givenName = $sn = _('Unknown');
                if (isset($member['sn']) && isset($member['sn'][0])) $sn = $member['sn'][0];
                if (isset($member['givenName'][0]) && isset($member['givenName'][0])) $givenName = $member['givenName'][0];
                $image = image('images/false.png');
                if (isset($member['sn'])) {
                    $image = image('plugins/users/images/select_user.png');
                }
                $lData[$uid] = array('data' => array($image, $sn, $givenName, $uid));
            }
            $this->partialList->setListData($data, $lData);
            $this->partialList->update();
            $smarty->assign("partialList", $this->partialList->render());
        }

        // Checkboxes 
        foreach (array('force_gid', 'smbgroup') as $val) {
            if ($this->$val == "1") {
                $smarty->assign("$val", 'checked');
            } else {
                $smarty->assign("$val", '');
            }
        }
        if ($this->force_gid != "1") {
            $smarty->assign('forceMode', 'disabled');
        } else {
            $smarty->assign('forceMode', '');
        }
        if ($this->fon_group) {
            $smarty->assign('fon_group', 'checked');
        } else {
            $smarty->assign('fon_group', '');
        }

        if ($this->nagios_group) {
            $smarty->assign('nagios_group', 'checked');
        } else {
            $smarty->assign('nagios_group', '');
        }

        // Fields 
        foreach (array('cn', 'description', 'gidNumber') as $val) {
            $smarty->assign("$val", set_post($this->$val));
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

        // Multiple edit handling 
        $smarty->assign('multiple_support', $this->multiple_support_active);

        foreach ($this->attributes as $val) {
            if (in_array_strict($val, $this->multi_boxes)) {
                $smarty->assign("use_" . $val, true);
            } else {
                $smarty->assign("use_" . $val, false);
            }
        }
        foreach (array("base", 'smbgroup', 'groupType', 'sambaDomainName', 'fon_group', 'nagios_group') as $val) {
            if (in_array_strict($val, $this->multi_boxes)) {
                $smarty->assign("use_" . $val, true);
            } else {
                $smarty->assign("use_" . $val, false);
            }
        }

        $bool = $this->isRestrictedByDynGroup();
        $smarty->assign("restrictedByDynGroup", $bool);
        if ($bool) {
            $smarty->assign("memberUidACL", preg_replace("/[^r]/", '', $this->getacl('memberUid')));
        }

        $smarty->assign('rfc2307bisMode', ($this->rfc2307bis) ? '1' : '0');
        return ($smarty->fetch(get_template_path('generic.tpl', true)));
    }

    function isRestrictedByDynGroup(): bool
    {
        $bool = false;
        if (isset($this->parent->by_object['DynamicLdapGroup'])) {
            $bool = $this->parent->by_object['DynamicLdapGroup']->isAttributeDynamic('memberUid') ||
                $this->parent->by_object['DynamicLdapGroup']->isAttributeDynamic('member');
        }
        return $bool;
    }

    /**
     * Add user defined by $uid to this group.
     *
     * \param $uid user id
     * \return bool
     */
    function addUser(string $uid): bool
    {
        global $config;
        /** 
         * In mutliple edit we have to handle two arrays.
         * members               : Containing users used in all groups
         * members_used_by_some  : Those which are not used in all groups
         * So we have to remove the given $dn from the ..used_by_some array first.
         */
        if ($this->multiple_support_active) {
            if (isset($this->memberUid_used_by_some[$uid])) {
                unset($this->memberUid_used_by_some[$uid]);
            }
        }

        // Ensure that the requested object is known to the group class 
        if (!isset($this->dnMapping[$uid])) {
            $ldap = $config->get_ldap_link();
            $ldap->cd($config->current['BASE']);
            $ldap->search("(&(objectClass=gosaAccount)(uid=" . $uid . "))", array('dn', 'uid', 'sn', 'givenName'));
            if ($ldap->count() === 0) {
                msg_dialog::display(
                    _("Error"),
                    sprintf(
                        _("Adding UID '%s' to group '%s' failed: cannot find user object!"),
                        $uid,
                        $this->cn
                    ),
                    ERROR_DIALOG
                );
                return false;
            } elseif ($ldap->count() >= 2) {
                msg_dialog::display(
                    _("Error"),
                    sprintf(
                        _("Add UID '%s' to group '%s' failed: UID is used more than once!"),
                        $uid,
                        $this->cn
                    ),
                    ERROR_DIALOG
                );
                return false;
            } else {
                while ($attrs = $ldap->fetch()) {
                    $this->dnMapping[$attrs['uid'][0]] = $attrs['dn'];
                    $this->members[$attrs['uid'][0]] = $attrs;
                    $this->allusers[$attrs['uid'][0]] = $attrs;
                }
            }
        }

        $this->memberUid[$uid] = $uid;
        return true;
    }


    function removeUser($uid)
    {
        if (isset($this->memberUid[$uid])) {
            unset($this->memberUid[$uid]);
        }

        if (isset($this->members[$uid])) {
            unset($this->members[$uid]);
        }

        /** 
         * We have two array contianing group members in multiple edit.
         * members             : Groups used by all currently edited groups 
         * members_used_by_some: Used by some 
         * So we have to remove the specified member from both arrays.
         */
        if ($this->multiple_support_active) {
            if (isset($this->memberUid_used_by_some[$uid])) {
                unset($this->memberUid_used_by_some[$uid]);
            }
        }
    }

    // reload and receive data from the LDAP
    function reload()
    {
        global $config;
        // Prepare ldap link
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);


        /**
         * Resolve still unresolved memberuids to fill the list with sn/giveName attributes 
         * (Store gathered sn/givenName informations in $this->allusers too, 
         * to be prepared when adding/deleting users)
         */
        $filter = '';

        // Merge in partial uids in multiple edit
        $allUids = array_keys($this->memberUid);
        if ($this->multiple_support_active) {
            $allUids = array_merge($allUids,  array_keys($this->memberUid_used_by_some));
        }

        // Do not request data for users we've already fetched before.
        foreach ($allUids as $key => $uid) {
            if (isset($this->dnMapping[$uid])) unset($allUids[$key]);
        }
        $allUids = array_values($allUids);

        // To resolve the usernames out of the 'uid' we've to perform ldap queries. 
        // To keep the amount of queries as short as possible, we combine the query 
        // for 'sn','givenName','..' for serveral users in one single query:
        //      (|(uid=hans)(uid=peter)(uid=hubert)(..))
        // 
        // Unfortunately there is a filter length limit which causes the query to be invalid,
        // we've to split these huge query again into shorter query strings. 
        // 
        // maxPerRound specifies the amount of queries we can combine into a 
        // single query.
        $maxPerRound = $config->get_cfg_value('core', 'ldapFilterNestingLimit');
        if ($maxPerRound === '') {
            $maxPerRound = count($allUids);
        }

        for ($added = 0; $added < count($allUids); $added += $maxPerRound) {
            // First build the query....
            $start = $added;
            $end = $added + $maxPerRound;
            $filter = '';
            for ($done = $start; $done < $end; $done++) {
                if (!isset($allUids[$done])) break;
                $value = $allUids[$done];
                $filter .= '(uid=' . LDAP::escapeValue($value) . ')';
            }

            // Retrieve the data to LDAP
            $ldap->cd($config->current['BASE']);
            $ldap->search('(&(objectClass=gosaAccount)(|' . $filter . '))', ['dn', 'uid', 'sn', 'givenName']);
            while ($attrs = $ldap->fetch()) {
                $this->dnMapping[$attrs['uid'][0]] = $attrs['dn'];
                $this->members[$attrs['uid'][0]] = $attrs;
                $this->allusers[$attrs['uid'][0]] = $attrs;
            }
        }

        // check if all uids are resolved
        foreach ($this->memberUid as $value) {
            if (!isset($this->members[$value])) {
                $this->members[$value] = '';
            }
        }

        if ($this->rfc2307bis) {
            // member dn processing
            foreach ($this->member as $memberDn) {
                // Retrieve the data to LDAP
                $ldap->cd($config->current['BASE']);
                $ldap->cat($memberDn);
                while ($attrs = $ldap->fetch()) {
                    $this->members[$memberDn] = $attrs;
                }
            }

            // ensure all member are initialized in members array
            foreach ($this->member as $memberDn) {
                if (!isset($this->members[$memberDn])) {
                    $this->members[$memberDn] = '';
                }
            }
        }
    }

    function remove_from_parent()
    {
        global $config;
        parent::remove_from_parent();

        $ldap = $config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, __CLASS__));
        }

        new log("remove", "groups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());

        // Delete references to object groups 
        $ldap->cd($config->current['BASE']);
        $ldap->search("(&(objectClass=gosaGroupOfNames)(member=" . LDAP::prepare4filter($this->dn) . "))", array('cn'));
        while ($ldap->fetch()) {
            $og = new ogroup($config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save();
        }

        // Remove ACL dependencies too,
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);
        $ldap->search("(&(objectClass=gosaAcl)(gosaAclEntry=*" . base64_encode($this->dn) . "*))", array("gosaAclEntry", 'dn'));
        while ($attrs = $ldap->fetch()) {
            $acl = new acl($config, $this->parent, $attrs['dn']);
            foreach ($acl->gosaAclEntry as $id => $entry) {
                foreach ($entry['members'] as $m_id => $member) {
                    if ($m_id == "G:" . $this->dn || $m_id == "U:" . $this->dn) {
                        unset($acl->gosaAclEntry[$id]['members'][$m_id]);
                        new log("modify", "groups/acl", $attrs['dn'], [], sprintf("Removed acl for %s on object %s.", $this->dn, $attrs['dn']));
                    }
                }
            }
            $acl->save();
        }

        // Remove ACL dependencies, too 
        acl::remove_acl_for($this->dn);

        // Send signal to the world that we've done 
        $this->handle_post_events("remove");
    }


    /**
     * Save data to object 
     */
    function save_object()
    {
        // Save additional values for possible next step 
        if (isset($_POST['groupedit'])) {

            // Create a base backup and reset the base directly after calling parent::save_object();  
            // Base will be set seperatly a few lines below 
            $base_tmp = $this->base;
            parent::save_object();
            $this->trustModeDialog->save_object();
            $this->base = $base_tmp;

            // Refresh base 
            if ($this->acl_is_moveable($this->base)) {
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base = $this->baseSelector->getBase();
                    $this->is_modified = true;
                }
            }

            $this->force_gid = 0;

            // Only reset sambagroup flag if we are able to write this flag 
            if ($this->acl_is_writeable('sambaGroupType')) {
                $this->smbgroup = 0;
            }

            foreach (
                array(
                    'force_gid'  => 'gidNumber',
                    'smbgroup'   => 'sambaGroupType'
                ) as $val => $aclname
            ) {
                if ($this->acl_is_writeable($aclname)  && isset($_POST["$val"])) {
                    $this->$val = get_post($val);
                }
            }

            // Save sambaDomain attribute 
            if ($this->acl_is_writeable('sambaDomainName') && isset($_POST['sambaDomainName'])) {
                $this->sambaDomainName = get_post('sambaDomainName');
                $this->groupType = get_post('groupType');
            }

            // Save fon attribute 
            if ($this->acl_is_writeable('fonGroup')) {
                if (isset($_POST['fon_group'])) {
                    $this->fon_group = true;
                } else {
                    $this->fon_group = false;
                }
            }
            if ($this->acl_is_writeable('nagiosGroup')) {
                if (isset($_POST['nagios_group'])) {
                    $this->nagios_group = true;
                } else {
                    $this->nagios_group = false;
                }
            }
        }
    }


    // Save to LDAP 
    function save()
    {
        global $config;

        // ID handling 
        if ($this->force_gid == 0) {
            if ($this->saved_gidNumber != '') {
                $this->gidNumber = $this->saved_gidNumber;
            } else {
                // Calculate new, lock gids 
                $wait = 10;
                while (get_lock('gidnumber') != '') {
                    sleep(1);

                    // timed out? 
                    if ($wait-- == 0) {
                        break;
                    }
                }
                add_lock('gidnumber', "gosa");
                $this->gidNumber = get_next_id('gidNumber', $this->dn);
            }
        }

        parent::save();

        // Remove objectClass for samba/phone support 
        $tmp = [];
        for ($i = 0; $i < count($this->attrs["objectClass"]); $i++) {
            if (
                $this->attrs['objectClass'][$i] != 'sambaGroupMapping' &&
                $this->attrs['objectClass'][$i] != 'sambaIdmapEntry' &&
                $this->attrs['objectClass'][$i] != 'goFonPickupGroup' &&
                $this->attrs['objectClass'][$i] != 'nagiosContactGroup'
            ) {
                $tmp[] = $this->attrs['objectClass'][$i];
            }
        }
        $this->attrs['objectClass'] = $tmp;
        $ldap = $config->get_ldap_link();

        // Add samba group functionality 
        if ($this->smbgroup) {
            // Fixed undefined index ...    
            $this->SID = $this->ridBase = '';
            if (isset($config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])) {
                $this->SID    = $config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
            } else {
                msg_dialog::display(_("Error"), sprintf(_("Cannot find any SID for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
            }
            if (isset($config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'])) {
                $this->ridBase = $config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'];
            } else {
                msg_dialog::display(_("Error"), sprintf(_("Cannot find any RIDBASE for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
            }

            $this->attrs['objectClass'][] = 'sambaGroupMapping';
            $this->attrs['sambaGroupType'] = "2";

            // Check if we need to create a special entry 
            if ($this->groupType == 0) {
                if ($this->sambaSID == '' || $this->oldgroupType != $this->groupType) {
                    $sid = $this->getSambaSID();
                    $this->attrs['sambaSID'] = $sid;
                    $this->sambaSID = $sid;
                }
            } else {
                $this->attrs['sambaSID'] = $this->SID . "-" . $this->groupType;
            }

            /** 
             * User wants me to fake the idMappings? This is useful for
             * making winbind resolve the group names in a reasonable amount
             * of time in combination with larger databases. 
             */
            if ($config->boolValueIsTrue('core', "sambaIdMapping")) {
                $this->attrs['objectClass'][] = "sambaIdmapEntry";
            }
        }

        // Add phone functionality 
        if ($this->fon_group) {
            $this->attrs['objectClass'][] = "goFonPickupGroup";
        }

        // Add nagios functionality 
        if ($this->nagios_group) {
            $this->attrs['objectClass'][] = "nagiosContactGroup";
        }

        /* Take members array */

        if (!$this->isRestrictedByDynGroup() && count($this->memberUid)) {
            $this->attrs['memberUid'] = array_values(array_unique($this->memberUid));
        }

        /* New accounts need proper 'dn', propagate it to remaining objects */
        if ($this->dn === 'new') {
            $this->dn = 'cn=' . $this->cn . ',' . get_groups_ou() . $this->base;
        }

        /* Add member dn's for RFC2307bis Support */
        if ($this->rfc2307bis) {
            $this->attrs['member'] = [];
            if (count($this->memberUid)) {
                foreach ($this->attrs['memberUid'] as $uid) {
                    if (isset($this->dnMapping[$uid])) {
                        $this->attrs['member'][] = $this->dnMapping[$uid];
                    }
                }
            } else {
                $this->attrs['member'][] = $this->dn;
            }
        }

        // Save data. Using 'modify' implies that the entry is already present, 
        // use 'add' for new entries. So do a check first... 
        $ldap->cat($this->dn, array('dn'));
        if ($ldap->fetch()) {
            /* Modify needs [] to remove values :-( */
            if (!count($this->memberUid)) {
                $this->attrs['memberUid'] = [];
            }
            if (!$this->smbgroup) {
                $this->attrs['sambaGroupType'] = [];
                $this->attrs['sambaSID'] = [];
            }
            $mode = "modify";
        } else {
            $mode = "add";
            $ldap->cd($config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }

        // check generated gidNumber, it may be used by another group. 
        if ($this->gidNumber != '') {
            $ldap->cd($config->current['BASE']);
            $chk_attr = "orig_cn";
            if ($this->cn != $this->orig_cn) {
                $chk_attr = 'cn';
            }
            $ldap->search("(&(!(cn=" . $this->$chk_attr . "))(objectClass=posixGroup)(gidNumber=" . $this->gidNumber . "))", array('cn'));
            if ($ldap->count()) {
                $cns = '';
                while ($attrs = $ldap->fetch()) {
                    $cns .= $attrs['cn'][0] . ", ";
                }
                $cns = rtrim($cns, ", ");
                msg_dialog::display(_("Warning"), sprintf(_("The gidNumber '%s' is already in use by %s!"), $this->gidNumber, $cns), WARNING_DIALOG);
            }
        }

        // check if members are added to the group and delete redundancies
        $this->attrs['memberUid'] = array_unique($this->attrs['memberUid'] ?? []);

        // if the member array is empty nothing is saved or updated
        if (count($this->attrs['memberUid'])) {
            // Write back to ldap
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->$mode($this->attrs);
        }

        // Remove ACL dependencies too,
        if ($this->dn != $this->orig_dn && $this->orig_dn != "new") {
            $tmp = new acl($config, $this->parent, $this->dn);
            $tmp->update_acl_membership($this->orig_dn, $this->dn);
        }

        if ($this->initially_was_account) {
            new log("modify", "groups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        } else {
            new log("create", "groups/" . get_class($this), $this->dn, array_keys($this->attrs), $ldap->get_error());
        }

        $ret = 0;
        if (!$ldap->success()) {
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, __CLASS__));
            $ret = 1;
        }

        $this->trustModeDialog->dn = $this->dn;
        $this->trustModeDialog->save();

        // Remove gid lock 
        del_lock('gidnumber');

        // Post that we've done
        $this->handle_post_events($mode);

        return ($ret);
    }

    function check()
    {
        global $config;

        // Call common method to give check the hook 
        $message = parent::check();

        // Permissions for that base? 
        if ($this->base != '') {
            $new_dn = 'cn=' . $this->cn . ',' . get_groups_ou() . $this->base;
        } else {
            $new_dn = $this->dn;
        }

        // must: cn 
        if ($this->cn == '' && $this->acl_is_writeable('cn')) {
            $message[] = msgPool::required(_("Name"));
        }

        // Check if a wrong base was supplied
        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[] = msgPool::check_base();;
        }

        // Check for valid input 
        if (!tests::is_uid($this->cn)) {
            if (strict_uid_mode()) {
                $message[] = msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/");
            } else {
                $message[] = msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9 _.-]/i");
            }
        }

        // Check if a wrong base was supplied
        if (!$this->baseSelector->checkLastBaseUpdate()) {
            $message[] = msgPool::check_base();;
        }

        if ($this->allowGroupsWithSameNameInOtherSubtrees == true) {
            // Check for used 'cn' 
            $ldap = $config->get_ldap_link();
            if (($this->cn  != $this->orig_cn) || ($this->orig_dn == "new")) {
                $ldap->cd(get_groups_ou() . $this->base);
                $ldap->ls("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))", get_groups_ou() . $this->base, array('cn'));
                if ($ldap->count() != 0) {
                    $message[] = msgPool::duplicated(_("Name"));
                }
            }
        } else {
            // Check for used 'cn' 
            $ldap = $config->get_ldap_link();
            $ldap->cd($config->current['BASE']);
            $ldap->search("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))", array('cn'));
            if ($ldap->count() != 0) {
                // New entry? 
                if ($this->dn == 'new') {
                    $message[] = msgPool::duplicated(_("Name"));
                }
                // Moved? 
                elseif ($new_dn != $this->orig_dn) {
                    $ldap->fetch();
                    if ($ldap->getDN() != $this->orig_dn) {
                        $message[] = msgPool::duplicated(_("Name"));
                    }
                }
            }
        }

        // Check ID 
        if ($this->force_gid == "1") {
            if (!tests::is_id($this->gidNumber)) {
                $message[] = msgPool::invalid(_("GID"), $this->gidNumber, "/[0-9]/");
            } else {
                if ($this->gidNumber < $config->get_cfg_value('core', "minId")) {
                    $message[] = msgPool::toosmall(_("GID"), $config->get_cfg_value('core', "minId"));
                }
            }
        }

        // Check if we are allowed to create or move this object 
        if (
            !$this->orig_dn === "new" ||
            $this->orig_base != $this->base ||
            $this->cn != $this->orig_cn
        ) {
            if ($this->orig_dn === "new" && !$this->acl_is_createable($this->base)) {
                $message[] = msgPool::permCreate();
            } elseif ($this->orig_dn != "new" && !$this->acl_is_moveable($this->base)) {
                $message[] = msgPool::permMove();
            }
        }
        return $message;
    }

    function getCopyDialog(): array
    {
        if ($this->force_gid) {
            $used = " checked ";
            $dis  = '';
        } else {
            $used = '';
            $dis  = " disabled ";
        }

        $smarty = get_smarty();
        $smarty->assign("used",     set_post($used));
        $smarty->assign("dis",     set_post($dis));
        $smarty->assign('cn',      set_post($this->cn));
        $smarty->assign('gidNumber', set_post($this->gidNumber));
        $str = $smarty->fetch(get_template_path("paste_generic.tpl", true));
        $ret = [];
        $ret['string'] = $str;
        $ret['status'] = '';
        return $ret;
    }

    function saveCopyDialog()
    {
        if (isset($_POST['cn'])) {
            $this->cn = get_post('cn');
        }
        if (isset($_POST['force_gid'])) {
            $this->force_gid  = 1;
            $this->gidNumber = get_post('gidNumber');
        } else {
            $this->force_gid  = 0;
            $this->gidNumber  = false;
        }
    }

    // Return plugin informations for acl handling   
    static function plInfo()
    {
        return (array(
            "plShortName" => _('Generic'),
            "plDescription" => _("Generic group settings"),
            "plSelfModify"  => false,
            "plDepends"     => [],
            "plPriority"    => 0,
            "plSection"     => array("administration"),
            "plCategory"    => array("groups" => array("objectClass" => 'posixGroup', 'description' => _("Groups"))),

            "plProperties" =>
            array(
                array(
                    "name"          => "ogroupRDN",
                    "type"          => "rdn",
                    'DEFAULT'       => "ou=groups,",
                    'description'   => _("RDN for object group storage."),
                    "check"         => "gosaProperty::isRdn",
                    "migrate"       => "migrate_ogroupRDN",
                    "group"         => "plugin",
                    "mandatory"     => false
                )
            ),

            "plProvidedAcls"    => array(
                'cn'                => _("Name"),
                'description'       => _("Description"),
                "base"              => _("Base"),

                'gidNumber'         => _("GID"),

                'sambaGroupType'    => _("Samba group type"),
                'sambaDomainName'   => _("Samba domain name"),
                "accessTo"          => _("System trust"),
                'fonGroup'          => _("Phone pickup group"),
                'nagiosGroup'       => _("Nagios group"),

                'memberUid'         => _("Group member")
            )
        ));
    }

    function multiple_save_object()
    {
        if (isset($_POST['group_mulitple_edit'])) {
            /** 
             * Create a base backup and reset the base directly
             * after calling parent::save_object(); 
             * Base will be set seperatly a few lines below 
             */
            $base_tmp = $this->base;
            parent::multiple_save_object();
            parent::save_object();
            $this->trustModeDialog->multiple_save_object();
            $this->base = $base_tmp;

            foreach (array("base", 'smbgroup', 'groupType', 'sambaDomainName', 'fon_group', 'nagios_group') as $attr) {
                if (isset($_POST['use_' . $attr])) {
                    $this->multi_boxes[] = $attr;
                }
            }

            // Refresh base 
            if ($this->acl_is_moveable($this->base)) {
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base = $this->baseSelector->getBase();
                    $this->is_modified = true;
                }
            }

            foreach (array('smbgroup'   => 'sambaGroupType', 'nagios_group' => 'nagiosGroup') as $val => $aclname) {
                if ($this->acl_is_writeable($aclname)) {
                    if (isset($_POST["$val"])) {
                        $this->$val =  true;
                    } else {
                        $this->$val =  false;
                    }
                }
            }

            // Save sambaDomain attribute 
            if ($this->acl_is_writeable('sambaDomainName') && isset($_POST['sambaDomainName'])) {
                $this->sambaDomainName = get_post('sambaDomainName');
                $this->groupType = get_post('groupType');
            }

            // Save fon attribute 
            if ($this->acl_is_writeable('fonGroup')) {
                if (isset($_POST['fon_group'])) {
                    $this->fon_group = true;
                } else {
                    $this->fon_group = false;
                }
            }
        }
    }

    function get_multi_edit_values()
    {
        $ret = parent::get_multi_edit_values();
        $ret = array_merge($ret, $this->trustModeDialog->get_multi_edit_values());

        foreach (array("base", 'smbgroup', 'groupType', 'sambaDomainName', 'fon_group', 'nagios_group') as $attr) {
            if (in_array_strict($attr, $this->multi_boxes)) {
                $ret[$attr] = $this->$attr;
            }
        }

        $ret['memberUid'] = $this->memberUid;
        $ret['memberUid_used_by_some'] = $this->memberUid_used_by_some;
        return ($ret);
    }

    function multiple_execute()
    {
        return ($this->execute());
    }

    // Initialize plugin with given atribute arrays 
    function init_multiple_support($attrs, $all)
    {
        parent::init_multiple_support($attrs, $all);
        $this->trustModeDialog->init_multiple_support($attrs, $all);

        $this->memberUid = [];
        $this->memberUid_used_by_some = [];
        if (isset($attrs['memberUid'])) {
            for ($i = 0; $i < $attrs['memberUid']['count']; $i++) {
                $this->memberUid[$attrs['memberUid'][$i]] = $attrs['memberUid'][$i];
            }
            ksort($this->memberUid);
        }

        if (isset($all['memberUid'])) {
            for ($i = 0; $i < $all['memberUid']['count']; $i++) {
                if (!in_array_strict($all['memberUid'][$i], $this->memberUid)) {
                    $this->memberUid_used_by_some[$all['memberUid'][$i]] = $all['memberUid'][$i];
                }
            }
            ksort($this->memberUid_used_by_some);
        }

        // We've two lists in mutliple support  
        //  - one which represents those users which are part of ALL groups.
        //  - ond one which represents those users which are only part of SOME groups.
        $this->commonList = new sortableListing();
        $this->commonList->setDeleteable(true);
        $this->commonList->setInstantDelete(false);
        $this->commonList->setEditable(false);
        $this->commonList->setWidth("100%");
        $this->commonList->setHeight("120px");
        $this->commonList->setHeader(array('~', _("Common Name"), _("UID")));
        $this->commonList->setColspecs(array('20px', '*', '*', '20px'));
        $this->commonList->setDefaultSortColumn(1);

        $this->partialList = new sortableListing();
        $this->partialList->setDeleteable(true);
        $this->partialList->setInstantDelete(false);
        $this->partialList->setEditable(false);
        $this->partialList->setWidth("100%");
        $this->partialList->setHeight("120px");
        $this->partialList->setHeader(array('~', _("Common Name"), _("UID")));
        $this->partialList->setColspecs(array('20px', '*', '*', '20px'));
        $this->partialList->setDefaultSortColumn(1);
    }

    function PrepareForCopyPaste($source)
    {
        global $config;
        parent::PrepareForCopyPaste($source);

        $this->trustModeDialog->PrepareForCopyPaste($source);

        // Get samba Domain in case of samba 3 
        if ($this->sambaSID != '') {
            $this->SID = preg_replace("/-[^-]+$/", '', $this->sambaSID);
            $ldap = $config->get_ldap_link();
            $ldap->cd($config->current['BASE']);
            $ldap->search("(&(objectClass=sambaDomain)(sambaSID=$this->SID))", array("sambaAlgorithmicRidBase"));
            if ($ldap->count() != 0) {
                $attrs = $ldap->fetch();
                if (isset($attrs['sambaAlgorithmicRidBase'])) {
                    $this->ridBase = $attrs['sambaAlgorithmicRidBase'][0];
                } else {
                    $this->ridBase = $config->get_cfg_value('core', "sambaRidBase");
                }

                // Get domain name for SID 
                $this->sambaDomainName = 'DEFAULT';
                foreach ($config->data['SERVERS']['SAMBA'] as $key => $val) {
                    if ($val['SID'] == $this->SID) {
                        $this->sambaDomainName = $key;
                        break;
                    }
                }
            } else {
                if ($config->get_cfg_value('core', "sambaRidBase") != '') {
                    $this->sambaDomainName = 'DEFAULT';
                    $this->ridBase = $config->get_cfg_value('core', "sambaRidBase");
                    $this->SID = $config->get_cfg_value('core', 'sambaSID');
                } else {
                    msg_dialog::display(_("Configuration error"), _("Cannot find group SID in your configuration!"), ERROR_DIALOG);
                }
            }

            // Get group type 
            $this->groupType = (int)substr(strrchr($this->sambaSID, "-"), 1);
            if ($this->groupType < 500 || $this->groupType > 553) {
                $this->groupType = 0;
            }
            $this->oldgroupType = $this->groupType;
        }

        // Detect samba groups and adapt its values.
        $this->smbgroup = in_array_strict('sambaGroupMapping', $source['objectClass']);
        if ($this->smbgroup) {
            $this->sambaSID = $this->getSambaSID();
        }

        $this->memberUid = [];
        if (isset($source['memberUid'])) {
            for ($i = 0; $i < $source['memberUid']['count']; $i++) {
                $this->memberUid[] = $source['memberUid'][$i];
            }
        }
    }

    function set_acl_base($base)
    {
        parent::set_acl_base($base);
        $this->trustModeDialog->set_acl_base($base);
    }

    // Enables multiple support for this plugin
    function enable_multiple_support()
    {
        parent::enable_multiple_support();
        $this->trustModeDialog->enable_multiple_support();
    }

    // Set all members of edited groups in $members 
    function set_multi_edit_values($attrs)
    {
        $users = [];

        // Update groupMembership, keep optinal group
        foreach ($attrs['memberUid_used_by_some'] as $uid) {
            if (in_array_strict($uid, $this->memberUid)) {
                $users[$uid] = $uid;
            }
        }

        // Update groupMembership, add forced groups
        foreach ($attrs['memberUid'] as $uid) {
            $users[$uid] = $uid;
        }

        parent::set_multi_edit_values($attrs);
        $this->trustModeDialog->set_multi_edit_values($attrs);
        $this->memberUid = $users;
    }

    // Get a new SambaSID for a group
    function getSambaSID()
    {
        global $config;
        $ldap = $config->get_ldap_link();
        $gidNumber = $this->gidNumber;
        while (true) {
            $sid = $this->SID . "-" . ($gidNumber * 2 + $this->ridBase + 1);
            $ldap->cd($config->current['BASE']);
            $ldap->search("(sambaSID=$sid)", array('sambaSID'));
            if ($ldap->count() == 0) {
                break;
            }
            $gidNumber++;
        }
        return $sid;
    }

    /**
     * Load group members from LDAP by $this->dn
     * 
     * we ignore if rfc2307bis is enabled for now and
     * just read all entries that are available.
     */
    function loadGroupMembers()
    {
        if ($this->dn !== 'new') {
            global $config;
            $ldap = $config->get_ldap_link();
            $ldap->cat($this->dn);
            $ldapGroup = $ldap->fetch();

            // if available get group member by memberUid
            if (isset($ldapGroup['memberUid'][0])) {
                $tmp = [];
                for ($i = 0; $i < $ldapGroup['memberUid']['count']; $i++) {
                    $tmp[$ldapGroup['memberUid'][$i]] = $ldapGroup['memberUid'][$i];
                }
                $this->memberUid = $tmp;
                ksort($this->memberUid);
            }

            // if available get group members dn by member
            if (isset($ldapGroup['member'][0])) {
                $tmp = [];
                for ($i = 0; $i < $ldapGroup['member']['count']; $i++) {
                    // "remove" the groups own dn from member list
                    // having the own dn added to member array is a common
                    // behavior if there is no other member available
                    if ($ldapGroup['member'][$i] !== $this->dn) {
                        $tmp[$ldapGroup['member'][$i]] = $ldapGroup['member'][$i];
                    }
                }
                $this->member = $tmp;
                ksort($this->member);
            }
        } else {
            // if we "refresh" group members on a new group with its DN is still 'new'
            // reset memberUid and member to empty arrays
            $this->memberUid = [];
            $this->member = [];
        }
    }

    function removeGroupMember(string $member)
    {
        if ($this->rfc2307bis) {
            $memberRemoved = $this->removeMemberEntry($member);

            // if rfc2307bis compatibility mode is enabled
            // check if deleting the dn in the first place was successful
            if ($this->rfc2307bisDisableNestedGroups && $memberRemoved) {
                // if removing the member dn was successful remove the old memberUid also
                $uid = $this->getUidByDn($member);
                if ($uid !== '') {
                    $this->removeUser($uid);
                } else {
                    msg_dialog::display(_("Group error"), _("Matching memberUid not found after removing the member dn. That may indicate some serious problems!"), ERROR_DIALOG);
                }
            }
        } else {
            // remove memberUid
            $this->removeUser($member);
        }
    }

    /**
     * Remove member DN from member array
     */
    function removeMemberEntry(string $member): bool
    {
        if (isset($this->member[$member])) {
            unset($this->member[$member]);
        } else {
            return false;
        }

        /** 
         * \todo - ignore this for now
         * 
         * We have two array contianing group members in multiple edit.
         * members             : Groups used by all currently edited groups 
         * members_used_by_some: Used by some 
         * So we have to remove the specified member from both arrays.
         */
        if ($this->multiple_support_active) {
            if (isset($this->member_used_by_some[$member])) {
                unset($this->member_used_by_some[$member]);
            }
        }

        return true;
    }

    private function getUidByDn(string $dn): string
    {
        global $config;
        $ldap = $config->get_ldap_link();
        $ldap->cat($dn);
        $ldapMember = $ldap->fetch();

        return $ldapMember['uid'][0] ?? '';
    }

    /**
     * If rfc2307bis is enabled we have to check the attribute "member"
     * otherwise "memberUid". Since we may still write/read "memberUid"
     * in rfc2307bis compatibility mode we ignore seperate ACL values
     * for these two attributes.
     */
    private function getMemberAcl(string $aclMatch)
    {
        if ($this->rfc2307bis) {
            return preg_match($aclMatch, $this->getacl('member'));
        }

        return preg_match($aclMatch, $this->getacl('memberUid'));
    }

    /**
     * Add member by memberUid or member dn depending
     * on the gosa configuration. If rfc2307bis is enabled
     * we also check the compatibilty flag "rfc2307bisDisableNestedGroups".
     * If this is set we will write both attributes.
     */
    function addGroupMember(string $member)
    {
        $success = false;

        if ($this->rfc2307bis) {
            $success = $this->addMemberEntry($member);

            if ($this->rfc2307bisDisableNestedGroups) {
                $uid = $this->getUidByDn($member);

                if ($uid !== '') {
                    // ignore return value here since adding the $dn may already be successful
                    // so we have to reload anyway
                    $this->addUser($member);
                } else {
                    msg_dialog::display(_("Group error"), _("Trying to add a new member that is not an user account while rfc2307bis compatibility support is enabled. This is not supported and may damage your LDAP!"), ERROR_DIALOG);
                }
            }
        } else {
            // add memberUid
            $success = $this->addUser($member);
        }

        if ($success) {
            $this->reload();
        }
    }

    /**
     * simply add the new dn entry to $this->member array
     */
    function addMemberEntry(string $member): bool
    {
        if (!isset($this->member[$member])) {
            $this->member[$member] = $member;
            return true;
        }
        return false;
    }
}
