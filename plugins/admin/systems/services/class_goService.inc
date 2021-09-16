<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_goService.inc 19118 2010-07-26 15:46:29Z hickert $$
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

class goService extends plugin{

    /* This plugin only writes its objectClass */
    var $objectclasses    = array();
    var $attributes       = array();
    var $StatusFlag       = "";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts            = array();
    var $dn                   = NULL;
    var $cn                   = "";
    var $DisplayName          = "";
    var $view_logged          = FALSE;

    var $krb_service_prefix   = "";
    var $krb_host_key         = NULL; 

    /* Construcktion */ 
    function __construct(&$config,$dn,$parent = NULL)
    {
        plugin::__construct($config,$dn);
        $this->DisplayName = _("Empty service");
    }


    /* Create content */
    function execute()
    {
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        $str ="<div style='width:100%; text-align:right;'>".
            "  <input type='submit' name='SaveService' value='".msgPool::saveButton()."'>&nbsp;".
            "  <input type='submit' name='CancelService' value='".msgPool::cancelButton()."'>".
            "</div>";
        return($str);
    }


    /* Get service information for serverService plugin */
    function getListEntry()
    {

        $this->updateStatusState();

        /* Assign status flag */
        if(!empty($this->StatusFlag)){
            $flag                   = $this->StatusFlag;
            $fields['Status']       = $this->$flag;
        }else{
            $fields['Status']       = "";
        }

        /* Name displayed in service overview */
        $fields['Message']      = _("Empty service");

        /* Allow/disallow some functions */
        $sf = !empty($this->StatusFlag);
        $fields['AllowStart']   = $sf && $this->acl_is_writeable("start");
        $fields['AllowStop']    = $sf && $this->acl_is_writeable("stop");
        $fields['AllowRestart'] = $sf && $this->acl_is_writeable("restart");
        $fields['AllowRemove']  = $this->acl_is_removeable();
        $fields['AllowEdit']    = $this->acl_is_readable("");
        return($fields);
    }


    /* Remove service */
    function remove_from_parent()
    {
        if(!$this->initially_was_account || !$this->acl_is_removeable()){
            return;
        }

        plugin::remove_from_parent();

        /* Remove status flag, it is not a memeber of 
           this->attributes, so ensure that it is deleted too */
        if(!empty($this->StatusFlag)){
            $this->attrs[$this->StatusFlag] = array();
        }

        /* Check if this is a new entry ... add/modify */
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($this->dn,array("objectClass"));
        if($ldap->count()){
            $ldap->cd($this->dn);
            $ldap->modify($this->attrs);
        }else{
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);

        }

        new log("remove","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
        $this->handle_post_events("remove");
    }


    /* Save service */
    function save()
    {
        plugin::save();
        /* Check if this is a new entry ... add/modify */
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($this->dn,array("objectClass"));
        if($ldap->count()){
            $ldap->cd($this->dn);
            $ldap->modify($this->attrs);
        }else{
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
        }
        if($this->initially_was_account){
            new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            $this->handle_post_events("modify");
        }else{
            $this->handle_post_events("add");
            new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
    }


    /* Directly save new status flag */
    function setStatus($value)
    {
        if($value == "none") return;

        /* Can't set status flag for new services (Object doesn't exists in ldap tree) */
        if(!$this->initially_was_account) return;

        /* Can't set status flag, if no flag is specified  */
        if(empty($this->StatusFlag)){
            return;
        }

        /* Get object (server), update status flag and save changes */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->cat($this->dn,array("objectClass"));
        if($ldap->count()){

            $tmp = $ldap->fetch();
            for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
                $attrs['objectClass'][] = $tmp['objectClass'][$i];
            }
            $flag = $this->StatusFlag;
            $attrs[$flag] = $value;
            $this->$flag = $value;
            $ldap->modify($attrs);
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
            }
            $this->action_hook();
        }
    }


    function check()
    { 
        $message = plugin::check();
        return($message);
    }


    function save_object()
    {
        plugin::save_object();
    }  


    function action_hook($addAttrs= array())
    {
        $this->callHook('SERVICEACTIONHOOK', $addAttrs);
    }


    /* Get updates for status flag */
    function updateStatusState()
    {
        if(empty($this->StatusFlag)) return;

        $attrs = array();
        $flag = $this->StatusFlag;
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->cn);
        $ldap->cat($this->dn,array($flag));
        if($ldap->count()){
            $attrs = $ldap->fetch();
        }
        if(isset($attrs[$flag][0])){
            $this->$flag = $attrs[$flag][0];
        }
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
