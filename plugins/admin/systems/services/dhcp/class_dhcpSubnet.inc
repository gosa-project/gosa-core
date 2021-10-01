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

class dhcpSubnet extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpNetMask= 24;
    var $dhcpRange= "";
    var $range_start= "";
    var $range_stop= "";
    var $use_range= FALSE;

    /* attribute list for save action */
    var $objectclasses= array('top', 'dhcpSubnet', 'dhcpOptions');

    function __construct($parent,$attrs)
    {
        dhcpPlugin::__construct($parent,$attrs);

        if (!$this->new){
            /* Load attributes */
            foreach (array("dhcpNetMask", "dhcpRange") as $attr){
                if (isset($attrs[$attr][0])){
                    $this->$attr= $attrs[$attr][0];
                }
            }
            if (isset($attrs['dhcpRange']) && count($attrs['dhcpRange'])){
                $this->use_range= TRUE;
                list($this->range_start, $this->range_stop)= preg_split('/\s+/', $this->dhcpRange);
            }
        }

        $this->dhcpNetMask= normalize_netmask($this->dhcpNetMask);
    }


    function execute()
    {
        $smarty= get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("dhcp_netmask", set_post($this->dhcpNetMask));

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /* Prepare range */
        if ($this->use_range){
            $smarty->assign("use_range", "checked");
            $smarty->assign("range_disabled", "");
        } else {
            $smarty->assign("use_range", "");
            $smarty->assign("range_disabled", "disabled");
        }
        $smarty->assign("range_start", set_post($this->range_start));
        $smarty->assign("range_stop",  set_post($this->range_stop));

        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_subnet.tpl', TRUE, dirname(__FILE__))).$this->network->execute();

        $display.= $this->advanced->execute();

        /* Add footer */
        $display.= "<div style='width:100%;text-align:right;margin-top:5px;'>";
        if(preg_match("/w/",$this->parent->getacl(""))){
            $display.=   "<button type='submit' name='save_dhcp'>".msgPool::saveButton()."</button>&nbsp;";
        }
        $display.=   "<button type='submit' name='cancel_dhcp'>".msgPool::cancelButton()."</button>";
        $display.= "</div>";


        /* Show main page */
        return $display;
    }


    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {

        /* Skip here if do not have write permissions here 
           Still executed at bottom - dhcpPlugin::save_object();
         */
        if(isset($_POST['dhcp_subnet_posted']) && preg_match("/w/",$this->parent->getacl(""))){
            if (isset($_POST['cn'])){
                $this->cn= get_post('cn');
            } 
            if (isset($_POST['dhcp_netmask'])){
                $this->dhcpNetMask= get_post('dhcp_netmask');
            } 
            if (isset($_POST['use_range'])){
                $this->use_range= TRUE;
                $this->range_start= get_post('range_start');
                $this->range_stop= get_post('range_stop');
            } else {
                $this->use_range= FALSE;
            }

            /* Move range to internal variable */
            $this->dhcpRange= $this->range_start." ".$this->range_stop;
        }
        dhcpPlugin::save_object();
    }


    /* Check values */
    function check()
    {
        $message= array();

        $cache = $this->parent->dhcpObjectCache;

        /* All required fields are set? */
        if ($this->cn == ""){
            $message[]= msgPool::required(_("Network address"));
        }
        if ($this->dhcpNetMask == ""){
            $message[]= msgPool::required(_("Net mask"));
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

        /* IP's? */
        foreach(array(
                    'dhcpNetMask' => _("Net mask"), 
                    'cn'          => _("Network address"),
                    'range_start' => _("Range"),
                    'range_stop'  => _("Range")) as $attr => $str){
            if ($this->$attr != "" && !tests::is_ip($this->$attr)){
                $message[]= msgPool::invalid($str,"","","192.168.0.23");
            }
        }

        /* Check ip range */
        if ($this->use_range){
            if(!tests::is_ip_range($this->range_start,$this->range_stop)){
                $message[]= msgPool::invalid(_("Network range"));
            }

            /* Check if range is in the network */
            if (!tests::is_in_network($this->cn, $this->dhcpNetMask, $this->range_start) ||
                    !tests::is_in_network($this->cn, $this->dhcpNetMask, $this->range_stop)){
                $message[] = _("'Range' is not inside the configured network.");
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

        /* Move dn to the result */
        $this->attrs['dhcpNetMask']= array(netmask_to_bits($this->dhcpNetMask));
        if ($this->use_range && !empty($this->dhcpRange)){
            $this->attrs['dhcpRange']= array($this->range_start." ".$this->range_stop);
        } else {
            $this->attrs['dhcpRange']= array();
        }

        return ($this->attrs);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
