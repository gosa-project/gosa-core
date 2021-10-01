<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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

class dhcpPool extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpRange= "";
    var $range_start= "";
    var $range_stop= "";

    /* attribute list for save action */
    var $objectclasses= array('top', 'dhcpPool');

    function __construct($parent,$attrs)
    {
        dhcpPlugin::__construct($parent,$attrs);

        /* Load attributes */
        if (!$this->new){
            $this->dhcpRange= $attrs['dhcpRange'][0];
            list($this->range_start, $this->range_stop)= preg_split('/\s+/', $this->dhcpRange);
        }

        $this->advanced->setAutoOptions(array("host-name"));
        $this->advanced->setAutoStatements(array("fixed-address"));
    }

    function execute()
    {
        $smarty= get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("range_start", set_post($this->range_start));
        $smarty->assign("range_stop", set_post($this->range_stop));

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_pool.tpl', TRUE, dirname(__FILE__))).$this->network->execute();

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
        if (isset($_POST['cn'])){
            $this->cn= get_post('cn');
            $this->range_start= get_post('range_start');
            $this->range_stop= get_post('range_stop');
        }

        dhcpPlugin::save_object();

        /* Move range to internal variable */
        $this->dhcpRange= $this->range_start." ".$this->range_stop;
    }


    /* Check values */
    function check()
    {
        $message= array();

        $cache = $this->parent->dhcpObjectCache;

        /* All required fields are set? */
        if ($this->cn == ""){
            $message[]= msgPool::required(_("Name"));
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

        if ($this->dhcpRange == ""){
            $message[]= msgPool::required(_("Range"));
        }

        if (!tests::is_ip($this->range_start) || !tests::is_ip($this->range_stop)){
            $message[]= msgPool::invalid(_("Range"));
        }

        if(!tests::is_ip_range($this->range_start,$this->range_stop)){
            $message[]= msgPool::invalid(_("Range"));
        }

        /* Check if range is in the network */
        $dn= $this->dn;
        while (preg_match('/,/', $dn)){
            $type= $this->objectType($cache, $dn);

            /* Check for subnet */
            if ($type == 'dhcpSubnet'){
                $network= $cache[$dn]['cn'][0];
                $netmask= normalize_netmask($cache[$dn]['dhcpNetMask'][0]);
                if (!tests::is_in_network($network, $netmask, $this->range_start) ||
                        !tests::is_in_network($network, $netmask, $this->range_stop)){
                    $message[] = _("'Range' is not inside the configured network.");
                }
            }

            /* Stop if we've examined the service base object */
            if ($type == 'dhcpService'){
                break;
            }
            $dn= preg_replace('/^[^,]+,/', '', $dn);
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
        $this->attrs['dhcpRange']= array($this->dhcpRange);

        return ($this->attrs);
    }


    function objectType($cache, $dn)
    {
        $type= "";
        $types= array("dhcpService", "dhcpClass", "dhcpSubClass", "dhcpHost",
                "dhcpGroup", "dhcpPool", "dhcpSubnet", "dhcpSharedNetwork");

        foreach ($cache[$dn]['objectClass'] as $oc){
            if (in_array_strict($oc, $types)){
                $type= $oc;
                break;
            }
        }

        return ($type);
    }


}

?>
