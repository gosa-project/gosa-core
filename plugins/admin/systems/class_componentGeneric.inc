<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_componentGeneric.inc 21104 2012-01-16 09:01:35Z hickert $$
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

class componentGeneric extends plugin
{
    /* Generic terminal attributes */
    var $interfaces= array();
    var $ignore_account= TRUE;

    /* Needed values and lists */
    var $base= "";
    var $cn= "";
    var $description= "";
    var $orig_dn= "";
    var $orig_cn= "";
    var $orig_base= "";

    /* attribute list for save action */
    var $attributes= array("cn", "description");
    var $objectclasses= array("top", "device", "ipHost", "ieee802Device");
    var $netConfigDNS;
    var $view_logged  = FALSE;
    var $baseSelector;

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config, $dn, $parent);

        /* Set base */
        if ($this->dn == "new"){
            $ui= get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
            $this->cn= "";
        } else {
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("componentGeneric", "componentRDN"), '/')."/i", "", $this->dn);
        }
        $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses, true);
        /* Save dn for later references */
        $this->orig_dn= $this->dn;

        $this->orig_base = $this->base;
        $this->orig_cn = $this->cn;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
    }


    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        $theme = getThemeName();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","component/".get_class($this),$this->dn);
        }

        /* Do we represent a valid phone? */
        if (!$this->is_account && $this->parent === NULL){
            switch ($theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
            }
            $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("component"))."</div>";
            return($display);
        }

        /* Fill templating stuff */
        $smarty= get_smarty();


        /* Set acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        /* Assign attributes */
        foreach ($this->attributes as $attr){
            $smarty->assign("$attr", set_post($this->$attr));
        }
        $smarty->assign("base", $this->baseSelector->render());

        /* Show main page */
        $str = $this->netConfigDNS->execute();
        if(is_object($this->netConfigDNS->dialog)){
            return($str);
        }
        $smarty->assign("netconfig", $str);
        return($smarty->fetch (get_template_path('component.tpl', TRUE)));
    }

    function set_acl_base($base)
    {
        plugin::set_acl_base($base);
        $this->netConfigDNS->set_acl_base($base);
    }

    function set_acl_category($cat)
    {
        plugin::set_acl_category($cat);
        $this->netConfigDNS->set_acl_category($cat);
    }

    function remove_from_parent()
    {
        $ldap= $this->config->get_ldap_link();
        $this->netConfigDNS->remove_from_parent();
        $ldap->rmdir($this->dn);

        new log("remove","component/".get_class($this),$this->dn,$this->attributes,$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }

        $this->handle_post_events(array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

        /* Delete references to object groups */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
        while ($ldap->fetch()){
            $og= new ogroup($this->config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save ();
        }
    }


    /* Save data to object */
    function save_object()
    {

        /* Create a base backup and reset the
           base directly after calling plugin::save_object();
           Base will be set seperatly a few lines below */
        $base_tmp = $this->base;
        plugin::save_object();
        $this->base = $base_tmp;

        $this->netConfigDNS->save_object();

        /* Refresh base */
        if ($this->acl_is_moveable($this->base)){
            if (!$this->baseSelector->update()) {
                msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
            }
            if ($this->base != $this->baseSelector->getBase()) {
                $this->base= $this->baseSelector->getBase();
                $this->is_modified= TRUE;
            }
        }

    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();
        $message= array_merge($message,$this->netConfigDNS->check());

        $this->dn= "cn=".$this->cn.",".get_ou("componentGeneric", "componentRDN").$this->base;

        if ($this->cn == "" ){
            $message[]= msgPool::required(_("Component name"));
        }

        /* Check if given name is a valid host/dns name */
        $regex = $reason = "";
        if(!tests::is_dns_name($this->cn, $reason, $regex)){
            if($reason == 1){
                $message[]= sprintf(_("The full qualified domain name is too long!"));
            }elseif($reason == 2){
                $message[]= sprintf(_("The hostname name is too long!"));
            }else{
                $message[]= msgPool::invalid(_("Server name"), $this->cn, "/{$regex}/i");
            }
        }

        /* To check for valid ip*/
        if($this->netConfigDNS->ipHostNumber == ""){
            $message[]= msgPool::required(_("IP address"));
        } else {
            if (!tests::is_ip($this->netConfigDNS->ipHostNumber)){
                $message[]= msgPool::invalid(_("IP address"), "", "", "192.168.1.2");
            }
        }

        if ($this->orig_dn != $this->dn){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($this->base);
            $ldap->search ("(cn=".$this->cn.")", array("cn"));
            if ($ldap->count() != 0){
                while ($attrs= $ldap->fetch()){
                    if(preg_match("/cn=dhcp,/",$attrs['dn'])){
                        continue;
                    }
                    if ($attrs['dn'] != $this->orig_dn && preg_match("/".preg_quote(get_ou("componentGeneric", "componentRDN"), '/')."/i",$attrs['dn'])){
                        $message[]= msgPool::duplicated(_("Component name"));
                        break;
                    }
                }
            }
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        plugin::save();

        /* Remove all empty values */
        if ($this->orig_dn == 'new'){
            $attrs= array();
            foreach ($this->attrs as $key => $val){
                if (is_array($val) && count($val) == 0){
                    continue;
                }
                $attrs[$key]= $val;
            }
            $this->attrs= $attrs;
        }

        /* If this is a new Object IP & Mac aren't set.
           IP & Mac are msut attributes, so we set this values by here. */
        if($this->orig_dn == 'new'){
            $this->attrs['ipHostNumber'] = $this->netConfigDNS->ipHostNumber;
            $this->attrs['macAddress']  = $this->netConfigDNS->macAddress;
        }

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        if ($this->orig_dn == 'new'){
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
            new log("create","component/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

        } else {
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 
            new log("modify","component/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
        }

        $this->netConfigDNS->cn = $this->cn;
        $this->netConfigDNS->save();

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
    }

    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Component generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("component" => array("description"  => _("Network device"),
                            "objectClass"  => array("device", "ipHost", "ieee802Device"))),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "componentRDN",
                            "type"          => "rdn",
                            "default"       => "ou=netdevices,ou=systems,",
                            "description"   => _("RDN for component storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_componentRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),

            "plProvidedAcls"=> array(
                    "cn"                  => _("Name"),
                    "base"                => _("Base"),
                    "description"         => _("Description"),
                    "userPassword"      => _("Root password"))
                ));
    }

    /* Display generic part for server copy & paste */
    function getCopyDialog()
    {
        $vars = array("cn");
        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("object","component");
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    function saveCopyDialog()
    {
        if(isset($_POST['cn'])){
            $this->cn = get_post('cn');
        }
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);
        if(isset($source['macAddress'][0])){
            $this->netConfigDNS->macAddress = $source['macAddress'][0];
        }
        if(isset($source['ipHostNumber'][0])){
            $this->netConfigDNS->ipHostNumber = $source['ipHostNumber'][0];
        }
    }


    function is_modal_dialog()
    {
        return((isset($this->dialog) && $this->dialog) || (isset($this->netConfigDNS->dialog) && $this->netConfigDNS->dialog));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
