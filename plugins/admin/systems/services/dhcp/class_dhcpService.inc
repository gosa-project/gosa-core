<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class dhcpService extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpPrimaryDN= "";
    var $orig_dhcpPrimaryDN= "";
    var $ddns_styles= array('none', 'interim', 'ad-hoc');

    /* attribute list for save action */
    var $objectclasses= array('top', 'dhcpService');


    function __construct($parent,$attrs)
    {
        dhcpPlugin::__construct($parent,$attrs);

        /* Load statements / options */
        if (!$this->new){
            /* Load attributes */
            $this->dhcpPrimaryDN= $attrs['dhcpPrimaryDN'][0];
        } else {
            /* We keep the parent dn here if it's new */
            $this->statements->set('default-lease-time', 600);
            $this->statements->set('max-lease-time', 1700);
            $this->statements->set('authoritative', TRUE);
            $this->statements->set('ddns-update-style', 'none');
        }

        $this->advanced->setAutoOptions(array("server-name"));
        $this->advanced->setAutoStatements(array("default-lease-time", "max-lease-time", 
                    "authoritative", "server-identifier", "ddns-update-style"));

        /* Save for later action */
        $this->orig_dhcpPrimaryDN= $this->dhcpPrimaryDN;
    }


    function execute()
    {
        /* Show main page */
        $smarty= get_smarty();

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        $smarty->assign('ddns_styles', $this->ddns_styles);
        foreach (array('max_lease_time', 'default_lease_time', 'ddns_update_style') as $value){
            $smarty->assign("$value", set_post($this->statements->get(preg_replace('/_/', '-', $value))));
        }

        $smarty->assign("authoritative",set_post( $this->statements->exists('authoritative')));

        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_service.tpl', TRUE, dirname(__FILE__))).$this->network->execute();

        $display.= $this->advanced->execute();

        /* Add footer */
        $display.= "<div style='width:100%;text-align:right;margin-top:5px;'>";
        if(preg_match("/w/",$this->parent->getacl(""))){
            $display.=   "<button type='submit' name='save_dhcp'>".msgPool::saveButton()."</button>&nbsp;";
        }
        $display.=   "<button type='submit' name='cancel_dhcp'>".msgPool::cancelButton()."</button>";
        $display.= "</div>";

        return ($display);

    }

    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {
        /* No need to save in the first time */
        if (isset($_POST['ddns_update_style']) && preg_match("/w/",$this->parent->getacl(""))){

            /* Save remaining attributes */
            foreach (array('max_lease_time', 'default_lease_time', 'ddns_update_style') as $val){
                $tval= preg_replace('/_/', '-', $val);
                if ($_POST[$val] != ""){
                    $this->statements->set($tval,get_post($val));
                } else {
                    $this->statements->removeAll($tval);
                }
            }
            if (isset($_POST['authoritative'])){
                $this->statements->set('authoritative', "");
            } else {
                $this->statements->removeAll('authoritative');
            }
        }

        dhcpPlugin::save_object();
    }


    /* Check values */
    function check()
    {
        $message= array();

        if (!tests::is_id($this->statements->get('default-lease-time'))){
            $message[]= msgPool::invalid(_("Lease time"),$this->statements->get('default-lease-time'),"/[0-9]/");
        }
        if (!tests::is_id($this->statements->get('max-lease-time'))){
            $message[]= msgPool::invalid(_("Max lease time"),$this->statements->get('max-lease-time'),"/[0-9]/");
        }
        if ($this->statements->get('default-lease-time') > $this->statements->get('max-lease-time')){
            $message[]= msgPool::toobig(_("Default lease time"),_("Maximum lease time"));
        }

        /* Check external plugins */
        $net= $this->network->check();
        $adv= $this->advanced->check();
        $message= array_merge($message, $net, $adv);
        return $message;
    }


    /* Save to LDAP */
    function save()
    {
        global $config;
        $this->attrs= array();

        /* Get and set server name */
        $ldap= $config->get_ldap_link();
        $ldap->cat($this->dhcpPrimaryDN, array('cn'));
        $res= $ldap->fetch();
        if(isset($res['cn'][0])){
            $server_name= $res['cn'][0];
        }

        dhcpPlugin::save();

        $this->attrs['dhcpPrimaryDN']= array($this->dhcpPrimaryDN);
        $this->removeOption('server-name');
#    $this->attrs['dhcpOption'][]= "server-name $server_name";

        return ($this->attrs);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
