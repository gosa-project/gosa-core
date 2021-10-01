<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007 Cajus Pollmeier

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

class dhcpDnsZone extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpKeyDN="";
    var $dhcpDnsZone="";
    var $dhcpDnsZoneServer="";
    var $cn="";

    var $drop_down_domain= array();
    var $drop_down_nameserver= array();
    var $drop_down_tsigkey= array();

    /* attribute list for save action */
    var $objectclasses= array("top", "dhcpDnsZone");


    function __construct($parent,$attrs,$host_exists_in_gosa = FALSE)
    {
        global $config;
        dhcpPlugin::__construct($parent,$attrs);

        /* Load attributes */
        if (!$this->new){
            $this->dhcpDnsZoneServer= $attrs['dhcpDnsZoneServer'][0];
            $this->dhcpKeyDN= base64_encode($attrs['dhcpKeyDN'][0]);
        }

        // Prepare LDAP link
        $ldap= $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);

        /* Search for all objects, recursive, that match the given ldap filter.
           each dyndns zoen have to depent of an alreday DNS Zone created so we search it
         */

        // Search for domain
        $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@))",array("zoneName"));
        while($attrs = $ldap->fetch()){
            $this->drop_down_domain[$attrs["zoneName"][0]]= $attrs["zoneName"][0];
        }

        $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@))",array("nSRecord"));
        while($attrs = $ldap->fetch()){
            $this->drop_down_nameserver[$attrs["nSRecord"][0]]=$attrs["nSRecord"][0];
        }

        $ldap->search("(objectClass=dhcpTSigKey)",array("dn","cn"));
        if ($ldap->count() != 0){
            while($attrs = $ldap->fetch()){
                $this->drop_down_tsigkey[base64_encode($attrs["dn"])]=$attrs["cn"][0];
            }
        }
    }


    function execute()
    {
        $smarty= get_smarty();
        $smarty->assign("cn",                   set_post($this->cn));
        $smarty->assign("cns",                  set_post($this->drop_down_domain));
        $smarty->assign("dhcpDnsZoneServer",    set_post($this->dhcpDnsZoneServer));
        $smarty->assign("dhcpDnsZoneServers",   set_post($this->drop_down_nameserver));
        $smarty->assign("dhcpKeyDN",            set_post($this->dhcpKeyDN));
        $smarty->assign("dhcpKeyDNs",           set_post($this->drop_down_tsigkey));

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_dnszone.tpl', TRUE,dirname(__FILE__)));

        /* Add footer */
        $display.= "<div style='width:100%;text-align:right;margin-top:5px;'>";
        if(preg_match("/w/",$this->parent->getacl(""))){
            $display.=   "<input type=submit name='save_dhcp' value='".msgPool::saveButton()."'>&nbsp;";
        }
        $display.=   "<input type=submit name='cancel_dhcp' value='".msgPool::cancelButton()."'>";
        $display.= "</div>";

        return ($display);
    }



    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {
        /* Save remaining attributes */
        if (isset($_POST['dhcp_dnszone_posted']) && preg_match("/w/", $this->parent->getacl(""))){

            if (isset($_POST['dhcpKeyDN'])){
                $this->dhcpKeyDN= get_post('dhcpKeyDN');
            }

            if (isset($_POST['dhcpDnsZoneServer'])){
                $this->dhcpDnsZoneServer= get_post('dhcpDnsZoneServer');
            }
            if (isset($_POST['cn'])){
                $this->cn= get_post('cn');
            }

        }

        dhcpPlugin::save_object();
    }


    /* Check values */
    function check()
    {
        $message= array();

        $cache = array();
        if(isset($this->parent) && isset($this->parent->dhcpObjectCache)){
            $cache = $this->parent->dhcpObjectCache;
        }

        /* All required fields are set? */
        if ($this->cn == ""){
            $message[]= msgPool::required(_("DNS zone"));
        }

        if ($this->dhcpDnsZoneServer == ""){
            $message[]= msgPool::required(_("DNS server"));
        }

        if ($this->dhcpKeyDN == ""){
            $message[]= msgPool::required(_("Key DN"));
        }


        /* cn already used? */
        if ($this->orig_cn != $this->cn || $this->new){

            foreach($cache as $dn => $dummy){
                if (preg_match("/^cn=".$this->cn.",/", $dn) && count($dummy)){
                    $message[]= msgPool::duplicated(_("Name"));
                    break;
                }
            }
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
        dhcpPlugin::save();

        if ($this->dhcpKeyDN != ""){
            $this->attrs['dhcpKeyDN']= array(base64_decode($this->dhcpKeyDN));
        } else {
            $this->attrs['dhcpKeyDN']= array();
        }

        if ($this->dhcpDnsZoneServer != ""){
            $this->attrs['dhcpDnsZoneServer']= array($this->dhcpDnsZoneServer);
        } else {
            $this->attrs['dhcpDnsZoneServer']= array();
        }

        return ($this->attrs);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
