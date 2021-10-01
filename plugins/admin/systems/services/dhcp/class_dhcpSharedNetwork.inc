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

class dhcpSharedNetwork extends dhcpPlugin
{
    /* attribute list for save action */
    var $objectclasses= array('top', 'dhcpSharedNetwork');

    function __construct($parent,$attrs)
    {
        dhcpPlugin::__construct($parent,$attrs);

        $this->advanced->setAutoStatements(array("server-identifier", "default-lease-time",
                    "max-lease-time", "min-lease-time", "authoritative", "deny unknown-clients",
                    "deny bootp", "deny booting", "allow unknown-clients",
                    "allow bootp", "allow booting"));
    }


    function execute()
    {
        $smarty= get_smarty();

        /* Assign ACLs */
        $smarty->assign("acl",$this->parent->getacl(""));

        /* Fill template */
        $smarty->assign ("cn", set_post($this->cn));
        foreach (array("server-identifier", "default-lease-time", "max-lease-time", "min-lease-time") as $attr){
            $name = preg_replace('/-/', '_', $attr);
            $smarty->assign($name, set_post($this->statements->get($attr)));
        }
        if ($this->statements->exists("authoritative")){
            $smarty->assign("authoritative", "checked");
        } else {
            $smarty->assign("authoritative", "");
        }

        $allow = $this->statements->get("allow"); 
        if(!is_array($allow)) $allow=array($allow);
        foreach(array(
                    "unknown-clients" => "allow_unknown_state",
                    "bootp" => "allow_bootp_state",
                    "booting" => "allow_booting_state") as $state => $target){
            if(in_array_strict($state,$allow)){
                $smarty->assign($target,"checked");
            }else{
                $smarty->assign($target,"");
            }
        }

        /* Show main page */
        $display= $smarty->fetch(get_template_path('dhcp_sharedNetwork.tpl', TRUE,dirname(__FILE__))).$this->network->execute();

        /* Remove states configured by checkboxes. 
         */
        foreach(array("deny unknown-clients",
                    "deny bootp", "deny booting", "allow unknown-clients",
                    "allow bootp", "allow booting") as $name){
            if($this->statements->exists($name)){
                $this->statements->remove($name);
            }
        }

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
        /* Check permissions, don't touch anything if we do not have write permissions 
         */
        if (!preg_match("/w/",$this->parent->getacl(""))){
            dhcpPlugin::save_object();
        }elseif(isset($_POST['cn'])){

            $this->cn= get_post('cn');
            dhcpPlugin::save_object();

            foreach (array("server-identifier", "default-lease-time",
                        "max-lease-time", "min-lease-time") as $attr){
                if (isset($_POST[$attr]) && $_POST[$attr] != ""){
                    $this->statements->set($attr,get_post($attr));
                } else {
                    $this->statements->removeAll($attr);
                }
            }

            if (isset($_POST["authoritative"])){
                $this->statements->set("authoritative", "");
            } else {
                $this->statements->removeAll("authoritative");
            }

            $this->statements->removeAll("deny");
            $this->statements->removeAll("allow");
            foreach(array("unknown-clients", "bootp", "booting") as $name){
                if (isset($_POST[$name])){
                    $this->statements->add("allow",$name);
                } else {
                    $this->statements->add("deny",$name);
                }
            }
        }
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

        /* Check lease times */
        foreach (array("default-lease-time" => _("Default lease time"),
                    "max-lease-time" => _("Max. lease time"),
                    "min-lease-time" => _("Min. lease time")) as $key => $val){
            if ($this->statements->exists($key) && $this->statements->get($key) != "" && 
                    !tests::is_id($this->statements->get($key))){
                $message[]= msgPool::invalid($val,$this->statements->get($key),"/[0-9]/");
            }
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

        return ($this->attrs);
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
