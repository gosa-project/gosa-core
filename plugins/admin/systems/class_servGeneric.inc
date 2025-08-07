<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_servGeneric.inc 21103 2012-01-16 09:00:07Z hickert $$
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

class servgeneric extends plugin
{
    /* Generic terminal attributes */
    var $ignore_account= TRUE;
    var $interfaces= array();

    /* Needed values and lists */
    var $base= "";
    var $cn= "";
    var $l= "";
    var $description= "";
    var $orig_dn= "";
    var $orig_cn= "";
    var $orig_base= "";

    /* attribute list for save action */
    var $attributes= array("cn", "description","gotoMode");
    var $objectclasses= array("top", "GOhard", "goServer");

    var $gotoMode= "locked";

    var $netConfigDNS;
    var $modes = array();
    var $ui   ;
    var $validActions   = array("reboot" => "", "update" => "", "localboot" => "", "reinstall" => "", "rescan" => "",
            "wakeup" => "", "memcheck" => "", "sysinfo" => "");

    var $fai_activated  =FALSE;
    var $view_logged = FALSE;

    var $currently_installing = FALSE;
    var $currently_installing_warned = FALSE;

    var $kerberos_key_service = NULL;
    var $baseSelector;

    var $mapActions   = array("reboot"          => "",
            "instant_update"  => "softupdate",
            "localboot"       => "localboot",
            "update"          => "sceduledupdate",
            "reinstall"       => "install",
            "rescan"          => "",
            "wakeup"            => "",
            "memcheck"        => "memcheck",
            "sysinfo"         => "sysinfo");


    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        /* Check if FAI is activated */
        $this->fai_activated = $config->pluginEnabled("faiManagement");

        plugin::__construct ($config, $dn, $parent);

        /* Initialize */
        $this->ui = get_userinfo();
        $this->modes["active"]= _("Activated");
        $this->modes["locked"]= _("Locked");

        /* Set base */
        if ($this->dn == "new"){
            $ui= get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
            $this->cn= "";
        } elseif(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $this->dn)){
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", "", $this->dn);
        } else {
            $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("servgeneric", "serverRDN"), '/')."/i", "", $this->dn);
        }
        $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
        $this->netConfigDNS->set_acl_category("server");
        $this->netConfigDNS->set_acl_base($this->base);
        $this->netConfigDNS->MACisMust =TRUE;

        /* Initialize kerberos host key plugin */
        if(class_available("krbHostKeys")){
            $this->kerberos_key_service = new krbHostKeys($this->config,$this);
        }

        /* Check if this host is currently in installation process*/
        if($this->dn != "new" && class_available("DaemonEvent")){
            $e_types = DaemonEvent::get_event_types(USER_EVENT | SYSTEM_EVENT | HIDDEN_EVENT);
        }

        /* Save dn for later references */
        $this->orig_dn   = $this->dn;
        $this->orig_cn   = $this->cn;
        $this->orig_base = $this->base;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
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


    function execute()
    {


        /* Call parent execute */
        plugin::execute();

        $theme = getThemeName();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        /* Do we represent a valid server? */
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

        if ((isset($_POST['action'])) && ($this->acl_is_writeable("FAIstate")) && isset($this->mapActions[$_POST['saction']]) ){
            $action = get_post('saction');

            /* Check if we have an DaemonEvent for this action */
            if(class_available("DaemonEvent")){
                $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
                if(isset($events['TRIGGERED']["DaemonEvent_".$action])){
                    $evt = $events['TRIGGERED']["DaemonEvent_".$action];
                    $tmp = new $evt['CLASS_NAME']($this->config);
                    $tmp->add_targets(array($this->netConfigDNS->macAddress));
                    $tmp->set_type(TRIGGERED_EVENT);
                }
            }else{
                msg_dialog::display(_("Event error"),
                        sprintf(_("Event '%s' is not available!"),$action),ERROR_DIALOG);
            }
        }

        /* Fill templating stuff */
        $smarty= get_smarty();


        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translated){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        /* Assign base ACL */
        $smarty->assign("base", $this->baseSelector->render());

        /* Assign attributes */
        foreach ($this->attributes as $attr){
            $smarty->assign("$attr", set_post($this->$attr));
        }

        /* Assign status */

        $smarty->assign("actions", 
                set_post(
                    array(
                        "wakeup" => _("Wake up"),
                        "reinstall" => _("Reinstall"),
                        "update" => _("System update"),
                        "memcheck" => _("Memory test"),
                        "localboot" => _("Force local boot"),
                        "sysinfo"  => _("System analysis"))));

        /* Show main page */
        $smarty->assign("fai_activated",$this->fai_activated);
        $si_url = $this->config->get_cfg_value("core","gosaSupportURI");
        $smarty->assign("si_activated",!empty($si_url));

        $str = $this->netConfigDNS->execute();
        $this->dialog = FALSE;
        if(is_object($this->netConfigDNS->dialog)){
            $this->dialog = TRUE;
            return($str);
        }
        $smarty->assign("netconfig", $str);
        $smarty->assign("modes", $this->modes);
        $smarty->assign("currently_installing", $this->currently_installing);

        $smarty->assign("host_key","");
        if(is_object($this->kerberos_key_service)){
            $smarty->assign("host_key",$this->kerberos_key_service->execute_by_prefix("host/"));
        }

        return($smarty->fetch (get_template_path('server.tpl', TRUE)));
    }

    function remove_from_parent()
    {
        /* Cancel if there's nothing to do here */
        if ((!$this->initially_was_account) || (!$this->acl_is_removeable())){
            return;
        }

        /* Remove kerberos key dependencies too */
        if(is_object($this->kerberos_key_service)){
            $this->kerberos_key_service->remove_from_parent_by_prefix("host/");
        }

        $this->netConfigDNS->remove_from_parent();
        $ldap= $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);

        update_accessTo($this->orig_cn,"");

        new log("remove","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, self::class));
        }

        /* Delete references to object groups */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
        while ($ldap->fetch()){
            $og= new ogroup($this->config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save ();
        }


        $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }


    /* Save data to object */
    function save_object()
    {
        /* Save current base, to be able to revert to last base, 
           if new base is invalid or not allowed to be selected */
        $base_tmp = $this->base;
        plugin::save_object();
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


        /* Hanle kerberos host key plugin */
        if(is_object($this->kerberos_key_service)){
            $this->kerberos_key_service->save_object_by_prefix("host/");
        }
    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();
        $message= array_merge($message, $this->netConfigDNS->check());
        $this->dn= "cn=".$this->cn.",".get_ou("servgeneric", "serverRDN").$this->base;

        /* must: cn */
        if ($this->cn == ""){
            msgPool::required(_("Server name"));
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

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        if ($this->orig_dn != $this->dn){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($this->base);
            $ldap->search ("(cn=".$this->cn.")", array("cn"));
            if ($ldap->count() != 0){
                while ($attrs= $ldap->fetch()){
                    if ($attrs['dn'] != $this->orig_dn){

                        $matchesArp = FALSE;
                        if(class_available("ArpNewDevice")){
                            $matchesArp = preg_match("/,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i",$attrs['dn']);
                        }
                        $matchesServer = preg_match("/,".preg_quote(get_ou("servgeneric", "serverRDN"), '/')."/i",$attrs['dn']);

                        if(!preg_match("/cn=dhcp,/",$attrs['dn']) && !$matchesArp && $matchesServer){
                            $message[]= msgPool::duplicated(_("Server name"));
                            break;
                        }
                    }
                }
            }
        }

        /* Warn the user, that this host is currently installing */
        if($this->currently_installing && 
                !$this->currently_installing_warned && 
                !preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $this->orig_dn)){

            /* Force aborting without message dialog */
            $message[] = "";
            $this->currently_installing_warned = TRUE;
            msg_dialog::display(_("Software deployment"), 
                    _("This host is currently installing, if you really want to save it, press 'OK'."),
                    CONFIRM_DIALOG);
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        /* Detect mode changes */
        $activate= (isset($this->saved_attributes['gotoMode']) &&
                $this->gotoMode != $this->saved_attributes['gotoMode'] &&
                $this->gotoMode == "active" &&
                tests::is_ip($this->netConfigDNS->ipHostNumber));

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

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        if ($this->orig_dn == 'new'){
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
            new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            $mode= "add";
        } else {

            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 
            new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

            /* Update all accessTo/trust dependencies */
            if($this->orig_cn != $this->cn){
                update_accessTo($this->orig_cn,$this->cn);
            }

            $mode= "modify";
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, self::class));
        }
        $this->netConfigDNS->cn = $this->cn;
        $this->netConfigDNS->save();

        /* Optionally execute a command after we're done */
        $this->handle_post_events($mode,array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

        if ($activate){

            /* Send installation activation 
             */
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
            if(isset($events['TRIGGERED']['DaemonEvent_installation_activation'])){
                $evt = $events['TRIGGERED']['DaemonEvent_installation_activation'];
                $tmp = new $evt['CLASS_NAME']($this->config);
                $tmp->set_type(TRIGGERED_EVENT);
                $tmp->add_targets(array($this->netConfigDNS->macAddress));
            }
        }

    }


    /* Display generic part for server copy & paste */
    function getCopyDialog()
    {
        $vars = array("cn");

        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $smarty->assign("object","server");
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



    /* Return plugin informations for acl handling */ 
    static function plInfo()
    {
        return (array( 
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Server generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),           
                    "plRequirements"=> array(
                        'ldapSchema' => array('goServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(self::class)
                        ),
                    "plCategory"    => array("server" => array("description"  => _("Server"),
                            "objectClass"  => "goServer")),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "serverRDN",
                            "type"          => "rdn",
                            "default"       => "ou=servers,ou=systems,",
                            "description"   => _("RDN for server storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_serverRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),

            "plProvidedAcls"=> array(
                    "cn"            => _("Name"),
                    "description"   => _("Description"),
                    "base"          => _("Base"),

                    "gotoMode"      => _("Goto mode"),
                    "userPassword"=> _("Root password"),
                    "FAIstate"      => _("Action flag"))
                ));
    }


    function is_modal_dialog()
    {
        return((isset($this->dialog) && $this->dialog) || (isset($this->netConfigDNS->dialog) && $this->netConfigDNS->dialog));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
