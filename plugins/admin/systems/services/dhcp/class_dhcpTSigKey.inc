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

class dhcpTSigKey extends dhcpPlugin
{
    /* Used attributes */
    var $dhcpKeyAlgorithm="";
    var $dhcpKeySecret="";

    /* attribute list for save action */
    var $objectclasses= array("top", "dhcpTSigKey");

    function __construct($parent,$attrs)
    {
        dhcpPlugin::__construct($parent,$attrs);

        /* Load attributes */
        if (!$this->new){
            $this->dhcpKeyAlgorithm= $attrs['dhcpKeyAlgorithm'][0];
            $this->dhcpKeySecret= $attrs['dhcpKeySecret'][0];
        }

    }

    function execute()
    {
        $smarty= get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $dhcpKeyAlgorithm=preg_replace('/\s.*$/', '', $this->dhcpKeyAlgorithm);
        $smarty->assign("dhcpKeyAlgorithm",$dhcpKeyAlgorithm);
        $smarty->assign("dhcpKeyAlgorithms",
            set_post(
                array(
                    "HMAC-MD5" => _("HMAC-MD5"),
                    "RSAMD5" => _("RSAMD5"),
                    "RSASHA1" => _("RSASHA1"),
                    "DSA" => _("DSA"),
                    "DH" => _("DH"))));
        $smarty->assign("dhcpKeySecret", set_post($this->dhcpKeySecret));

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));


        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_tsigkey.tpl', TRUE,dirname(__FILE__)));


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
        if (isset($_POST['dhcp_tsigkey_posted']) && preg_match("/w/",$this->parent->getacl(""))){

            if (isset($_POST['dhcpKeyAlgorithm'])){
                $this->dhcpKeyAlgorithm=get_post('dhcpKeyAlgorithm');
            }

            if (isset($_POST['dhcpKeySecret'])){
                $this->dhcpKeySecret=get_post('dhcpKeySecret');
            }
            if (isset($_POST['cn'])){
                $this->cn=get_post('cn');
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
            $message[]= msgPool::required(_("Key name"));
        }

        if ($this->dhcpKeySecret == ""){
            $message[]= msgPool::required(_("Key"));
        }


        /* cn already used? */
        if ($this->orig_cn != $this->cn || $this->new){

            foreach($cache as $dn => $dummy){
                if (preg_match("/^cn=".$this->cn.",/", $dn) && count($dummy)){
                    $message[]= msgPool::duplicated(_("Key name"));
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

        if ($this->dhcpKeyAlgorithm != ""){
            $this->attrs['dhcpKeyAlgorithm']= array($this->dhcpKeyAlgorithm);
        } else {
            $this->attrs['dhcpKeyAlgorithm']= array();
        }


        if ($this->dhcpKeySecret != ""){
            $this->attrs['dhcpKeySecret']= array($this->dhcpKeySecret);
        } else {
            $this->attrs['dhcpKeySecret']= array();
        }

        return ($this->attrs);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
