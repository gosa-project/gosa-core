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
    var $attributes= array('uid');
    var $aclResolver = NULL;

    var $referenceFilters = array();
    var $objectList ="";

    function reference (&$config, $dn= NULL, $parent = NULL)
    {
        // Init the plugin
        plugin::plugin($config,$dn,$parent);

        // Initialize the ACL-resolver
        $this->aclResolver = new aclResolver($this->config, $this->dn, $this);

        // References we may have to other objects.
        $this->referenceFilters = array();

        // Check for group membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=posixGroup)(memberUid={$this->uid}))",
            'attrs'  => array('cn' => _("Name"),'description' => _("Description")),
            'msg'    => _("Group membership"));

        // Check for group membership in rfc 2307 bis mode
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=posixGroup)(member=".normalizeLdap($this->dn)."))",
            'attrs'  => array('cn' => _("Name"),'description' => _("Description")),
            'msg'    => _("Group membership")." (rfc 2307 bis)");

        // Check for role membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=organizationalRole)(roleOccupant=".normalizeLdap($this->dn)."))",
            'attrs'  => array('cn' => _("Name"),'description' => _("Description")),
            'msg'    => _("Role membership"));

        // Check for objectGroup membership
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaGroupOfNames)(member=".normalizeLdap($this->dn)."))",
            'attrs'  => array('cn' => _("Name"),'description' => _("Description")),
            'msg'    => _("Object group membership"));

        // Check for department manager 
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaDepartment)(manager=".normalizeLdap($this->dn)."))",
            'attrs'  => array('ou' => _("Name"),'description' => _("Description")),
            'msg'    => _("Department manager"));

        // Check for user manager 
        $this->referenceFilters[] = array(
            'filter' => "(&(objectClass=gosaAccount)(manager=".normalizeLdap($this->dn)."))",
            'attrs'  => array('givenName' => _("Given name"),'sn' => _("Surname"),'uid'=>_("Uid")),
            'msg'    => _("User manager"));

        // Go through filters and detect possible references  
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $str = "";
        foreach($this->referenceFilters as $filter){
            $ldap->search($filter['filter'], array_keys($filter['attrs']));
            if(!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_VIEW, get_class()));
            }elseif($ldap->count()){
                $list = new sortableListing();
                $list->setDeleteable(false);
                $list->setEditable(false);
                $list->setWidth("100%");
                $list->setHeight("80px");
                $list->setHeader(array_values($filter['attrs']));
                $list->setDefaultSortColumn(0);
                $list->setAcl('rwcdm');

                $data = array();
                while($attrs = $ldap->fetch()){

                    $entry = array();
                    foreach($filter['attrs'] as $name => $desc){
                        $$name = "";
                        if(isset($attrs[$name][0])) $$name = $attrs[$name][0];
                        $entry['data'][] = $$name;
                    }
                    $data[] = $entry;
                }
                $list->setListData($data, $data);

                $list->update();    
                $str .= "<h3>".$filter['msg']."</h3>";
                $str .= $list->render();
                $str .= "<div class='v-spacer'></div>";
            }
        }
        $this->objectList = $str;
    }

    function execute()
    {
        // Mark plugin as viewed
        plugin::execute();
        $smarty = get_smarty();        

        // Assign permissions
        $tmp = $this->plInfo();
        $ui = get_userinfo();
        foreach($tmp['plProvidedAcls'] as $name => $desc){
            $smarty->assign($name.'READ', $ui->get_permissions($this->dn, 'references/reference', $name));
        }

        $smarty->assign('objectList', $this->objectList);
        $smarty->assign("acls",$this->aclResolver->getReadableACL());
        $smarty->assign("usePrototype", "true");
        session::set('autocomplete', $this->aclResolver);
        return ($smarty->fetch (get_template_path('contents.tpl', TRUE, dirname(__FILE__))));
    }



    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Reference"),
                    "plDescription" => _("Object references"),
                    "plSelfModify"  => TRUE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("references" => array("description"  => _("References"),
                                             "objectClass"  => array())),
                    "plProvidedAcls"=> array(
                        "references"   => _("View object references"),
                        "acl"          => _("View acl trace"),
                        "info"         => _("View object info"),
                        "export"       => _("Allow entry export")
                        )

                    ));
    }


}

?>
