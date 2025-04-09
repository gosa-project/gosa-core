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

class reference extends plugin
{
    var $attributes = array('uid', 'modifyTimestamp');
    var $aclResolver = null;

    var $referenceFilters = [];
    var $objectList = "";
    var $modifyTimestamp = "";
    var $cn = '';

    var $lists = [];

    function __construct($config, $dn = null, $parent = null)
    {
        // Init the plugin
        parent::__construct($config, $dn, $parent);

        // Try to read the 'modifyTimestamp' - this has to be done separately.
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);
        $ldap->cat($this->dn, ['modifyTimestamp']);
        if ($ldap->count()) {
            $attrs = $ldap->fetch();
            if (isset($attrs['modifyTimestamp'][0])) {
                $this->modifyTimestamp = $attrs['modifyTimestamp'][0];
            }
        }

        // Initialize the ACL-resolver
        $this->aclResolver = new aclResolver($config, $this->dn, $this);

        // References we may have to other objects.
        $this->referenceFilters = [];

        // Check for group membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=posixGroup)(memberUid={$this->uid}))",
            'attrs'  => array('cn' => _("Name"), 'description' => _("Description")),
            'msg'    => _("Group membership"),
            'releaseAction' => 'removeUid',
            'listObject' => new sortableListing()
        );

        // Check for group membership in rfc 2307 bis mode
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=posixGroup)(member=" . LDAP::escapeValue($this->dn) . "))",
            'attrs'  => array('cn' => _("Name"), 'description' => _("Description")),
            'msg'    => _("Group membership") . " (rfc 2307 bis)",
            'listObject' => new sortableListing()
        );

        // Check for role membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=organizationalRole)(roleOccupant=" . LDAP::escapeValue($this->dn) . "))",
            'attrs'  => array('cn' => _("Name"), 'description' => _("Description")),
            'msg'    => _("Role membership"),
            'listObject' => new sortableListing()
        );

        // Check for objectGroup membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaGroupOfNames)(member=" . LDAP::escapeValue($this->dn) . "))",
            'attrs'  => array('cn' => _("Name"), 'description' => _("Description")),
            'msg'    => _("Object group membership"),
            'releaseAction' => 'removeMember',
            'listObject' => new sortableListing()
        );

        // Check for department manager
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaDepartment)(manager=" . LDAP::escapeValue($this->dn) . "))",
            'attrs'  => array('ou' => _("Name"), 'description' => _("Description")),
            'msg'    => _("Department manager"),
            'listObject' => new sortableListing()
        );

        // Check for user manager
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaAccount)(manager=" . LDAP::escapeValue($this->dn) . "))",
            'attrs'  => array('givenName' => _("Given name"), 'sn' => _("Surname"), 'uid' => _("UID")),
            'msg'    => _("User manager"),
            'listObject' => new sortableListing()
        );

        $this->refreshContent();
    }

    function refreshContent()
    {
        global $config;
        // Go through filters and detect possible references
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);
        $str = "";
        foreach ($this->referenceFilters as $filter) {
            $ldap->search($filter['filter'], array_merge(array_keys($filter['attrs']), ['dn']));
            if (!$ldap->success()) {
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_SEARCH, __CLASS__));
            } elseif ($ldap->count()) {
                $list = $filter['listObject'];
                if (isset($filter['releaseAction'])) {
                    $list->setDeleteable(true);
                } else {
                    $list->setDeleteable(false);
                }

                $list->setEditable(false);
                $list->setWidth("100%");
                $list->setHeight("80px");
                $list->setHeader(array_values($filter['attrs']));
                $list->setDefaultSortColumn(0);
                $list->setAcl('rwcdm');

                $data = [];
                $displayData = [];
                while ($attrs = $ldap->fetch()) {
                    $entry = [];
                    foreach ($filter['attrs'] as $name => $desc) {
                        $$name = "";
                        if (isset($attrs[$name][0])) $$name = $attrs[$name][0];
                        $entry['data'][] = $$name;
                    }
                    $displayData[] = $entry;
                    $entry['dn'] = $attrs['dn'];
                    $data[] = $entry;
                }
                $list->setListData($data, $displayData);

                $list->update();
                $str .= "<h2>" . $filter['msg'] . "</h2><div class='row'><div class='col s12'>";
                $str .= $list->render();
                $str .= "</div></div>";
                $str .= "<br>";
                $this->lists[] = $list;
            }
        }
        $this->objectList = $str;
    }

    function removeUid(string $posixGroupDn)
    {
        global $config;
        $ldap = $config->get_ldap_link();
        $ldap->cat($posixGroupDn, ["memberUid"]);

        if ($ldap->count() == 1) {
            $posixGroup = $ldap->fetch();
            if (isset($posixGroup['memberUid'])) {
                $updatedMemberUidList = [];
                for ($i = 0; $i < $posixGroup['memberUid']['count']; $i++) {
                    if ($posixGroup['memberUid'][$i] != $this->uid) {
                        $updatedMemberUidList[] = $posixGroup['memberUid'][$i];
                    }
                }
                $ldap->cd($posixGroupDn);
                $ldap->modify([
                    'memberUid' => $updatedMemberUidList
                ]);
                if (!$ldap->success()) {
                    echo $ldap->get_error();
                }
            }
        }
    }

    function removeMember(string $objectGroupDn)
    {
        global $config;
        $ldap = $config->get_ldap_link();
        $ldap->cat($objectGroupDn, ["member"]);

        if ($ldap->count() == 1) {
            $objectGroup = $ldap->fetch();
            if (isset($objectGroup['member'])) {
                $updatedMemberList = [];
                for ($i = 0; $i < $objectGroup['member']['count']; $i++) {
                    if ($objectGroup['member'][$i] != $this->dn) {
                        $updatedMemberList[] = $objectGroup['member'][$i];
                    }
                }
                $ldap->cd($objectGroupDn);
                $ldap->modify([
                    'member' => $updatedMemberList
                ]);
                if (!$ldap->success()) {
                    echo $ldap->get_error();
                }
            }
        }
    }

    function execute()
    {
        global $config;
        // Mark plugin as viewed
        parent::execute();

        foreach (array_keys($_POST) as $postParam) {
            if (strpos($postParam, 'del_') === 0) {
                foreach ($this->referenceFilters as $filter) {
                    $releaseAction = $filter['releaseAction'];
                    $list = $filter['listObject'];
                    if ($releaseAction !== null && $list !== null) {
                        if (strpos($postParam, $list->getListId())) {
                            // ATTENTION: WORKAROUND
                            // sortableListing is checking $_REQUEST['PID'] for being the active one
                            // but having more than one listing on one page will set the PID value
                            // to the latest sortableListing object that is displayed.
                            $_REQUEST['PID'] = $list->getListId();
                            $list->save_object();
                            $action = $list->getAction();
                            $this->$releaseAction($list->getData($action['targets'][0])['dn']);
                        }
                    }
                }
            }
        }

        $this->refreshContent();

        // Show ldif viewer
        if (isset($_POST['viewLdif'])) {
            $this->dialog = new ldifViewer($config, $this->dn);
        }
        if (isset($_POST['cancelLdifViewer'])) $this->dialog = null;
        if ($this->dialog instanceof ldifViewer) {
            return $this->dialog->execute();
        }

        $smarty = get_smarty();

        // Assign permissions
        $ui = get_userinfo();

        $category = preg_replace("/\/.*$/", "", $this->acl_category);
        $smarty->assign('aclREAD',  preg_match("/r/", $ui->get_category_permissions($this->dn, 'acl')));
        $smarty->assign('completeACL',  $ui->has_complete_category_acls($this->dn, $category));
        $smarty->assign('someACL',      $ui->get_category_permissions($this->dn, $category));

        // Convert the modifyTimestamp to a human readable value
        $tz = timezone::get_default_timezone();
        $smarty->assign('modifyTimestamp', set_post(date('d.m.Y H:i:s', strtotime($this->modifyTimestamp))));

        $smarty->assign('objectList', $this->objectList);
        $smarty->assign("acls", $this->aclResolver->getReadableACL());

        session::set("generic::aclResolver", $this->aclResolver);
        session::set('autocomplete', "generic::aclResolver");
        return $smarty->fetch(get_template_path("contents.tpl", TRUE, dirname(__FILE__)));
    }
}
