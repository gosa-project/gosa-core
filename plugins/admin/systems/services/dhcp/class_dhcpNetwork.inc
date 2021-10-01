<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004-2007  Cajus Pollmeier

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

class dhcpNetwork extends plugin
{
    /* Used attributes */
    var $options= null;
    var $statements= null;

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();

    function __construct()
    {
        /* This is always an account */
        $this->initTime = microtime(TRUE);
        $this->is_account= TRUE;

        // Create statistic table entry
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));
    }

    function execute()
    {
        plugin::execute();

        $acl_writeable = preg_match("/w/",$this->parent->getacl(""));

        /* Check for iteraction */
        if ($acl_writeable && isset($_POST['add_dns']) && $_POST['addserver'] != ""){
            if (!preg_match('/^[0-9a-z.-]+$/', get_post('addserver'))){
                msg_dialog::display(_("Error"), msgPool::invalid(_("Name"),
                            htmlentities(get_post('addserver')),"/[0-9a-z.-]/"), ERROR_DIALOG);
            } else {
                $servers= array();
                if($this->options->exists('domain-name-servers')){
                    foreach(explode(",", $this->options->get('domain-name-servers')) as $val){
                        $servers[$val]= $val;
                    }
                }
                $servers[get_post('addserver')]= get_post('addserver');

                $tmp= "";
                foreach($servers as $val){
                    $tmp.= $val.",";
                }
                $this->options->removeAll('domain-name-servers');
                $this->options->set('domain-name-servers',preg_replace('/,$/', '', $tmp));
            }
        }

        if ($acl_writeable && isset($_POST['delete_dns']) && isset($_POST['dnsserver'])){
            $tmp= preg_replace("/(\s*,\s*)?".get_post('dnsserver')."/i", '', 
                    $this->options->get('domain-name-servers'));
            $tmp= trim(preg_replace("/(\s*)?,(\s*)?$/", '', $tmp),",");



            if ($tmp != ""){
                $this->options->set('domain-name-servers', $tmp);
            } else {
                $this->options->remove('domain-name-servers');
            }
        }

        /* Show main page */
        $smarty= get_smarty();

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /*
         * Assemble options
         */

        /* Router */
        $smarty->assign("routers", set_post($this->options->get('routers')));

        /* DNS */
        $smarty->assign("domain", set_post($this->options->get('domain-name')));

        if($this->options->exists('domain-name-servers')){
            $servers= array();
            foreach(explode(",", $this->options->get('domain-name-servers')) as $val){
                $servers[$val]= $val;
            }
            $smarty->assign("dnsservers", set_post($servers));
        } else {
            $smarty->assign("dnsservers", array());
        }

        /* Netmask / Broadcast */
        if ($this->options->exists('subnet-mask')){
            $this->options->set('subnet-mask',normalize_netmask($this->options->get('subnet-mask')));
        }
        $smarty->assign("subnet_mask",      set_post($this->options->get('subnet-mask')));
        $smarty->assign("broadcast_address",set_post($this->options->get('broadcast-address')));

        /* Boot stuff */
        $smarty->assign("filename",    set_post($this->statements->get('filename')));
        $smarty->assign("nextserver",  set_post($this->statements->get('next-server')));

        /* Set flags */
        $smarty->assign("autohost", "");
        if ($this->statements->exists('get-lease-hostnames')){
            if (preg_match('/^(true|on|yes)$/', $this->statements->get('get-lease-hostnames'))){
                $smarty->assign("autohost", "checked");
            }
        }
        $smarty->assign("autohostdecl", "");
        if ($this->statements->exists('use-host-decl-names')){
            if (preg_match('/^(true|on|yes)$/', $this->statements->get('use-host-decl-names'))){
                $smarty->assign("autohostdecl", "checked");
            }
        }

        return $smarty->fetch(get_template_path('dhcp_network.tpl', TRUE,dirname(__FILE__)));
    }

    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {
        /* Only save, if we are "active" */
        if (isset($_POST['routers']) && preg_match("/w/",$this->parent->getacl(""))){

            /*
             * Assemble options
             */

            /* Options */
            foreach (array("routers" => "routers", "domain-name" => "domain", "subnet-mask" => "subnet_mask",
                        "broadcast-address" => "broadcast_address") as $key => $val){
                if ($_POST["$val"] == ''){
                    $this->options->remove($key);
                } else {
                    $this->options->set($key,get_post($val));
                }
            }

            /* Statements */
            foreach (array("filename" => "filename", "next-server" => "nextserver") as $key => $val){
                if ($_POST["$val"] == ''){
                    $this->statements->removeAll($key);
                } else {

                    /* Only quote filename values */
                    if(in_array_strict($key,array("filename"))){
                        $this->statements->set($key,'"'.trim(get_post($val),"\"").'"');
                    }else{
                        $this->statements->set($key,get_post($val));
                    }
                }
            }

            /* Flags */
            if (isset ($_POST['autohost'])){
                $this->statements->set('get-lease-hostnames',"true");
            } else {
                $this->statements->removeAll('get-lease-hostnames');
            }
            if (isset ($_POST['autohostdecl'])){
                $this->statements->set('use-host-decl-names', "on");
            } else {
                $this->statements->removeAll('use-host-decl-names');
            }
        }
    }


    /* Check values */
    function check()
    {
        $message= array();

        /* Check netmask and broadcast */
        foreach(array("subnet-mask" => _("Net mask"), "broadcast-address" => _("Broadcast address")) as $key => $typ){
            if (!$this->options->exists("$key")){
                continue;
            }
            if (!tests::is_ip($this->options->get($key))){
                $message[]= sprintf(_("Error in definition of '%s'!"), $typ);
            }
        }
        return $message;
    }


    /* Save to LDAP */
    function save()
    {
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
