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

class dhcpHost extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpHWAddress= "";
    var $realGosaHost = FALSE;

    /* attribute list for save action */
    var $objectclasses= array("top", "dhcpHost");

    function __construct($parent,$attrs,$host_exists_in_gosa = FALSE)
    {
        dhcpPlugin::__construct($parent,$attrs);

        /* Load attributes */
        if (!$this->new){
            $this->dhcpHWAddress= $attrs['dhcpHWAddress'][0];
        }

        $this->advanced->setAutoOptions(array("host-name"));
        $this->advanced->setAutoStatements(array("fixed-address"));

        $this->realGosaHost = $host_exists_in_gosa;
    }

    function execute()
    {
        $smarty= get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("dhcpHWAddress", set_post(preg_replace('/^[^ ]+ /', '', $this->dhcpHWAddress)));
        $smarty->assign("realGosaHost", set_post($this->realGosaHost));

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /* Create fixed address */
        $smarty->assign("fixedaddr", set_post($this->statements->get('fixed-address')));

        /* Prepare hw type selector */
        $hwtype= preg_replace('/\s.*$/', '', $this->dhcpHWAddress);
        $smarty->assign("hwtype", set_post($hwtype));
        $smarty->assign("hwtypes", 
                set_post(
                    array(
                        "ethernet" => _("Ethernet"),
                        "fddi" => _("FDDI"),
                        "token-ring" => _("Token Ring"))));
        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_host.tpl', TRUE,dirname(__FILE__))).$this->network->execute();

        /* Get advanced options */
        $display.= $this->advanced->execute();

        /* Add footer */
        $display.= "<div style='width:100%;text-align:right;margin-top:5px;'>";
        $display.=   "<button type='submit' name='save_dhcp'>".msgPool::saveButton()."</button>&nbsp;";
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
        /* Save remaining attributes */
        if (isset($_POST['dhcp_host_posted']) && preg_match("/w/",$this->parent->getacl(""))){

            /* Assemble hwAddress */
            if (isset($_POST['dhcpHWAddress'])){
                $this->dhcpHWAddress= get_post('hwtype')." ".get_post('dhcpHWAddress');
            }

            if(!$this->realGosaHost){
                $this->cn= get_post('cn');
            }

            /* Save fixed address */
            if(!$this->realGosaHost){
                if ($_POST['fixedaddr'] != ""){
                    $this->statements->set('fixed-address', get_post('fixedaddr'));
                } else {
                    $this->statements->removeAll('fixed-address');
                }
            }
            $this->options->set('host-name',$this->cn);
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

        /* Check syntax of MAC address */
        $check= preg_replace('/^[^\s]*\s/', '', $this->dhcpHWAddress);
        if (!preg_match('/^([0-9a-fA-F]{1,2}:){5}[0-9a-fA-F]{1,2}$/', $check)){
            $message[]= msgPool::invalid(_("Hardware address"));
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
        if ($this->dhcpHWAddress != ""){
            $this->attrs['dhcpHWAddress']= array($this->dhcpHWAddress);
        } else {
            $this->attrs['dhcpHWAddress']= array();
        }

        return ($this->attrs);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
