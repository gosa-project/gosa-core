<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_termDNS.inc 21097 2012-01-12 09:36:53Z hickert $$
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

class termDNS extends plugin
{
    /* attribute list for save action */
    var $ignore_account = true;

    /* Basic informations 
     */
    var $attributes     = array("ipHostNumber","macAddress");
    var $objectclasses  = array("whatever");

    var $ipHostNumber          = "";    // IP address 
    var $additionalHostNumbers = array();
    var $macAddress            = "";    // Mac address 

    // Used in manufacturer detection
    var $last_macAddress       = "";      
    var $manufacturer          = "";

    var $orig_ipHostNumber   = "";    // IP address 
    var $orig_macAddress     = "";    // Mac address 

    var $cn             = "";    // CN of currently edited device 
    var $OrigCn         = "";    // Initial cn
    var $IPisMust       = false;
    var $MACisMust      = false;
    var $dialog         = false;

    /* DCHP Attributes 
     */
    var $dhcpAttributes           = array("dhcpParentNode");
    var $dhcpEnabled              = FALSE;
    var $dhcp_is_Account          = FALSE;
    var $dhcpParentNodes          = array();
    var $dhcpParentNode           = "";
    var $dhcpHostEntry            = array();
    var $initial_dhcpParentNode   = "";
    var $initial_dhcpHostEntry    = array();
    var $initial_dhcp_is_Account  = FALSE;

    var $used_ip_mac              = array();  

    /* DNS attributes  
     */
    var $DNSattributes            = array("zoneName","dNSTTL");
    var $DNS_is_account           = false;
    var $initially_was_account = false;
    var $dnsEntry                 = array();
    var $DNSenabled               = false;
    var $hide_dns_check_box       = FALSE;
    var $namingAttr               = "cn";

    /*  Terminal dns 
     */
    function __construct(&$config, $parent,$objectClasses,$IPisMust = false,$namingAttr = "cn")
    {
        /* We need to know which objectClasses are used, to store the ip/mac
         * Because of different type of devices   
         */ 
        $this->parent         = $parent;
        $this->objectclasses  =  $objectClasses;
        $this->IPisMust       = $IPisMust;
        $this->namingAttr     = $namingAttr;

        plugin::__construct ($config, $parent->dn, $this->parent);

        $this->attrs = &$this->parent->attrs;

        if(isset($this->attrs[$namingAttr][0])){
            $this->OrigCn = preg_replace("/\\\$\$/","",$this->attrs[$namingAttr][0]);
            $this->cn = preg_replace("/\\\$\$/","",$this->attrs[$namingAttr][0]);
        }

        /* Create list of additional ipHostNumber.
         */
        $this->additionalHostNumbers = array();
        if(isset($this->attrs['ipHostNumber']) && $this->attrs['ipHostNumber']['count'] > 1){
            for($i = 1 ; $i < $this->attrs['ipHostNumber']['count']; $i ++){
                $this->additionalHostNumbers[] = $this->attrs['ipHostNumber'][$i];
            }
        }

        /************
         * DHCP 
         ************/

        /* Hide all dhcp specific code, if dhcp plugin is not present in config */
        $this->dhcpEnabled = FALSE;
        if($this->config->pluginEnabled("servdhcp")){
            $this->dhcpEnabled = TRUE;
        }

        if(!class_available("dhcpHost")){
            $this->dhcpEnabled = FALSE;
        }

        if($this->dhcpEnabled){
            $this->dhcpParentNodes = $this->get_dhcp_parent_nodes();
            $this->dhcpParentNode  = $this->get_dhcp_parent_node();
            if($this->dhcpParentNode){
                $this->dhcp_is_Account = TRUE;
                $this->initial_dhcp_is_Account = TRUE;
                $this->dhcpHostEntry  = $this->get_dhcp_host_entry();    
            }
            $this->initial_dhcpHostEntry = $this->dhcpHostEntry;
            $this->initial_dhcpParentNode= $this->dhcpParentNode;
        }


        /************
         * DNS
         ************/

        /* Hide all dns specific code, if dns is not available 
         */
        $DNSenabled = false;
        foreach($this->config->data['TABS']['SERVERSERVICE'] as $tab){
            if(preg_match("/^servdns$/",$tab['CLASS'])){
                $this->DNSenabled = true;
            }
        }

        if(!class_available("DNS")){
            $this->DNSenabled = FALSE;
        }

        if(!$this->DNSenabled){
            $this->DNS_is_account = false;
            return;
        }

        if($this->DNSenabled){

            /* Get Zones  
             */
            $this->Zones        = DNS::getAvailableZones($config);

            /* Get Entry 
             */
            $this->dnsEntry     = DNS::getDNSHostEntries($config,$this->OrigCn);

            $ptr = DNS::FlipIp($this->get_pTRRecord());

            /* Remove A record which equals $this->ipHostNumber
             */
            foreach($this->dnsEntry['RECORDS'] as $key => $rec){
                if(($rec['type'] == "aRecord") && ($rec['value'] == $this->ipHostNumber)){
                    unset($this->dnsEntry['RECORDS'][$key]);
                }
                if(($rec['type'] == "pTRRecord") && ($rec['value'] == $ptr)){
                    unset($this->dnsEntry['RECORDS'][$key]);
                }
            }

            /* Get Record types 
             */
            $this->RecordTypes  = DNS::getDnsRecordTypes();

            /* If there is at least one entry in this -> types, we have DNS enabled 
             */
            if($this->dnsEntry['exists']){
                $this->DNS_is_account = true;
            }else{
                $this->DNS_is_account = false;
            }
        }

        /* Create a list of used mac and ip addresses.

           ! We use this optically huge amount of code to fetch all 
           Mac and IP addresses, because a simple search for mac and IP
           over the whole ldap server was 10 to 20 times slower.
         */
        $deps  = array();
        $ou = preg_replace("/,.*$/","",get_ou("systemManagement", "systemRDN"));
        $a_ous = array(get_ou("servgeneric", "serverRDN"),
                get_ou("termgeneric", "terminalRDN"),
                get_ou("workgeneric", "workstationRDN"),
                get_ou("printgeneric", "printerRDN"),
                get_ou("phoneGeneric", "phoneRDN"),
                get_ou("componentGeneric", "componentRDN"));

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=organizationalUnit)(".$ou."))",array("dn"));
        while($attrs = $ldap->fetch()){
            foreach($a_ous as $allowed){
                $deps[] = $allowed.$attrs['dn'];
            }
        }

        foreach($deps as $dep){
            $ldap->cd($dep);
            $ldap->search("(|(macAddress=*)(ipHostNumber=*))",array("macAddress","ipHostNumber"));
            while($attrs = $ldap->fetch()){
                if(isset($attrs['ipHostNumber'][0])){
                    $this->used_ip_mac["ip:".$attrs['ipHostNumber'][0]] = "ip:".$attrs['ipHostNumber'][0];
                }
                if(isset($attrs['macAddress'][0])){
                    $this->used_ip_mac["mac:".$attrs['macAddress'][0]] = "mac:".$attrs['macAddress'][0];
                }
            } 
        }

        /* Save initial ip and mac values, to be able 
           check if the used values are already in use */ 
        $this->orig_ipHostNumber   = $this->ipHostNumber;  
        $this->orig_macAddress     = $this->macAddress;

        /* Store initally account settings 
         */
        $this->initially_was_account = $this->DNS_is_account;

        if($this->DNS_is_account){
            new log("view","unknown/".get_class($this),$this->dn);
        }
    }


    function netmaskIsCoherent($idZone) 
    {
        $netmask = DNS::FlipIp(str_replace(".in-addr.arpa.","",DNS::getNameFromMix($idZone)));
        if(!strstr($this->ipHostNumber, $netmask)){
            return false;
        }else{
            return true;
        }
    }


    function getVarsForSaving($attrs) 
    {
        foreach($this->attributes as $attr){
            if(!empty($this->$attr)){
                $attrs[$attr] = $this->$attr;
            }
        }
        return($attrs); 
    }

    function execute()
    {
        plugin::execute();

        /* Call parent execute */
        $smarty= get_smarty();
        $smarty->assign("autonetACL",$this->acl_is_writeable("macAddress") && $this->acl_is_writeable("ipHostNumber")?"rw":"");

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        $display= "";

        /**********
         * Additional ipHostNumber handling 
         **********/

        /* Add a new one */
        if($this->acl_is_writeable("ipHostNumber")){
            foreach($_POST as $name => $value){
                if(preg_match("/^additionalHostNumbers_add/",$name)){
                    $this->additionalHostNumbers[] = "";
                    break;
                }

                /* Delete given entry */
                if(preg_match("/^additionalHostNumbers_del_/",$name)){
                    $id = preg_replace("/^^additionalHostNumbers_del_([0-9]*)$/","\\1",$name);
                    if(isset($this->additionalHostNumbers[$id])){
                        unset($this->additionalHostNumbers[$id]);
                        $this->additionalHostNumbers = array_values($this->additionalHostNumbers);
                    }
                    break;
                } 
            }
        }

        $smarty->assign("additionalHostNumbers",$this->additionalHostNumbers);
        $smarty->assign("MACisMust",$this->MACisMust);

        /**********
         * Autonet completion
         **********/

        /* Check for autonet button */
        if (isset($_POST['autonet']) && ($this->acl_is_writeable("ipHostNumber") || $this->acl_is_writeable("macAddress"))){

            if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){
                $rpc = $this->config->getRpcHandle();
                $res = $rpc->networkCompletion($this->cn);
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"), msgPool::rpcError($rpc->get_error()), ERROR_DIALOG);
                }else{
                    if(isset($res['ip']) && tests::is_ip($res['ip'])) $this->ipHostNumber= $res['ip'];
                    if(isset($res['mac']) && tests::is_mac($res['mac'])) $this->macAddress= $res['mac'];
                }
            }
        }


        /**********
         * DHCP Handling
         **********/

        if(isset($_POST['dhcpEditOptions']) && $this->acl_is_readable("dhcpSetup")){
            if(count($this->dhcpHostEntry) == 0){
                $this->dialog = new dhcpHost($this->parent,$this->dhcpParentNode,TRUE);
            }else{
                $this->dialog = new dhcpHost($this->parent,$this->dhcpHostEntry,TRUE);
            }
            $this->dialog->cn   = $this->cn;
            $this->dialog->read_only     = !$this->acl_is_writeable("dhcpSetup");
            $this->dialog->dhcpHWAddress = "ethernet ".$this->macAddress; 
            if(!empty($this->ipHostNumber)){
                $this->dialog->statements->set('fixed-address', $this->ipHostNumber); 
            }
        }

        if(isset($_POST['cancel_dhcp'])){
            $this->dialog = FALSE; 
        }

        if(isset($_POST['save_dhcp']) && $this->acl_is_writeable("dhcpSetup") && is_object($this->dialog)){
            $this->dialog->save_object();
            $msgs = $this->dialog->check(array());
            if(count($msgs)){
                foreach($msgs as $msg){
                    msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
                }
            }else{
                $this->dhcpHostEntry = $this->dialog->save();
                $this->dialog = FALSE; 
            }
        }

        if(is_object($this->dialog)){
            $this->dialog->save_object();
            return($this->dialog->execute());
        }

        $smarty->assign("dhcpEnabled",    $this->dhcpEnabled && $this->acl_is_readable("dhcpSetup"));
        $smarty->assign("dhcp_is_Account",$this->dhcp_is_Account);
        $smarty->assign("dhcpParentNode", $this->dhcpParentNode);
        $smarty->assign("dhcpParentNodes",$this->dhcpParentNodes);
        $smarty->assign("dhcpParentNodeCnt",count($this->dhcpParentNodes));

        // Assign network card manufacturer string
        if ($this->config->get_cfg_value("core","gosaRpcServer") != ""   && 
                $this->macAddress != $this->last_macAddress){
            $rpc = $this->config->getRpcHandle();
            $res = $rpc->getMacManufacturer($this->macAddress);
            if($rpc->success()){
                $this->manufacturer = $rpc->getMacManufacturer($this->macAddress);
            }
        }
        $smarty->assign("macManufacturer", set_post($this->manufacturer));

        /**********
         * DNS Handling
         **********/

        /* There is no dns available
         */
        $smarty->assign("DNS_is_account",$this->DNS_is_account);
        $smarty->assign("DNSenabled",$this->DNSenabled && $this->acl_is_readable("dnsSetup"));

        if($this->DNSenabled == false){

            /* Is IP address must ? */
            # $smarty->assign("DNS_is_account",false);
            $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));

            /* Assign smarty all non DNs attributes */
            foreach($this->attributes as $attr){
                $smarty->assign($attr,set_post($this->$attr));
            }

            $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
        }else{
            # $smarty->assign("DNS_is_account",true);

            /* Add new empty array to our record list */
            if(isset($_POST['AddNewRecord']) && $this->acl_is_writeable("dnsSetup")){
                $this->dnsEntry['RECORDS'][]  =array("type"=>"aRecord","value"=>"");
            }

            /* propose_ip */
            if(isset($_POST['propose_ip']) && $this->acl_is_writeable("ipHostNumber")){
                foreach($this->Zones as $key => $name){
                    if($name == $this->dnsEntry['zoneName']){
                        $net = DNS::FlipIp(str_replace(".in-addr.arpa.","",DNS::getNameFromMix($key)));
                        $this->ipHostNumber = $this->generateRandomIP($net);
                    }
                }
            }

            /* Handle all posts */
            $only_once =true;
            if($this->acl_is_writeable("dnsSetup")){
                foreach($_POST as $name => $value){

                    /* Check if we have to delete a record entry */
                    if((preg_match("/RemoveRecord_/",$name))&&($only_once)) {

                        /* Avoid performing this once again */
                        $only_once = false;

                        /* Extract id for specified entry */
                        $id = preg_replace("/RemoveRecord_/","",$name);
                        $id = preg_replace("/_.*$/","",$id);

                        /* Delete this record, mark edited entries to be able to delete them */
                        if(isset($this->dnsEntry['RECORDS'][$id])){
                            unset($this->dnsEntry['RECORDS'][$id]);
                        }
                    }
                }
            }
            /* Assign smarty all non DNs attributes */
            foreach($this->attributes as $attr){
                $smarty->assign($attr,set_post($this->$attr));
            }

            /* Assign smarty all DNS attributes */
            foreach($this->DNSattributes as $attr){
                $smarty->assign($attr,set_post($this->dnsEntry[$attr]));
            }

            /* Assign all needed vars */
#     $smarty->assign("DNSAccount",$this->DNS_is_account);
            $smarty->assign("hide_dns_check_box",$this->hide_dns_check_box);

            $smarty->assign("Zones",$this->Zones);
            $smarty->assign("ZoneCnt",count($this->Zones));
            $smarty->assign("ZoneKeys",($this->Zones));
            $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));

            /* Create zone array */
            $idZones = array();
            foreach($this->Zones as $id => $zone){
                if($this->netmaskIsCoherent($id)) {
                    $idZones[$id] = $zone;
                }else{
                    $idZones[$id] = $zone."&nbsp;("._("Not matching").")";
                }
            }
            $smarty->assign("Zones",$idZones);
            $smarty->assign("ZoneKeys", $this->Zones);

            $tmp = $this->generateRecordsList();

            $changeStateForRecords = $tmp['changeStateForRecords'];

            $smarty->assign("records",$tmp['str']);
            $smarty->assign("changeStateForRecords",$changeStateForRecords);

            $smarty->assign("autonetACL",$this->acl_is_writeable("macAddress") && $this->acl_is_writeable("ipHostNumber")?"rw":"");
            $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
        }

        return($display);
    }


    function remove_from_parent()
    {
        if($this->DNS_is_account){

            $ldap = $this->config->get_ldap_link();

            $tmp = array();
            $this->dnsEntry['exists'] = false;
            $tmp = DNS::getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);

            /* Delete dns */
            foreach($tmp['del'] as $dn => $del){
                $ldap->cd($dn);
                $ldap->rmdir_recursive($dn);
                new log("remove","unknown/".get_class($this),$dn);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $dn, LDAP_DEL, get_class()));
                }
            }
        }

        /************
         * DHCP Handling 
         ************/

        /* DHCP removed */
        if($this->dhcp_is_Account){
            $ldap = $this->config->get_ldap_link();
            $ldap->rmdir_recursive($this->dhcpHostEntry['dn']);
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dhcpHostEntry['dn'], LDAP_DEL,         get_class()));
            }
        }

    }


    /* Save data to object */
    function save_object()
    {

        if(isset($_POST['network_tpl_posted'])){

            $this->last_macAddress = $this->macAddress;

            /* Save all posted vars */
            plugin::save_object();

            /******
              Additional IP Host Numbers 
             ******/

            /* Get posts for all additionally added ipHostNumbers */
            if($this->acl_is_writeable("ipHostNumber")){
                foreach($this->additionalHostNumbers as $id => $value){
                    if(isset($_POST['additionalHostNumbers_'.$id])){
                        $this->additionalHostNumbers[$id] = get_post('additionalHostNumbers_'.$id);
                    }
                } 
            } 


            /******
              DHCP posts
             ******/

            if($this->acl_is_writeable("dhcpSetup") && $this->dhcpEnabled && isset($_POST['network_tpl_posted'])){
                foreach($this->dhcpAttributes as $attr){
                    if(isset($_POST[$attr])){
                        $this->$attr = get_post($attr);
                    }
                }
                if(isset($_POST['dhcp_is_Account'])){
                    $this->dhcp_is_Account = TRUE;
                }else{
                    $this->dhcp_is_Account = FALSE;
                }
            }

            /* Ge all non dns attributes (IP/MAC)*/
            foreach($this->attributes as $attr){
                if(isset($_POST[$attr]) && $this->acl_is_writeable($attr)){
                    $this->$attr = get_post($attr);
                }
            }


            /******
              DNS posts
             ******/

            /* Check if DNS should be enabled / disabled 
             *  -skip this, if the dns account is enforced.
             */
            if(!$this->hide_dns_check_box){
                if($this->DNS_is_account && $this->acl_is_writeable("dnsSetup") && !isset($_POST['DNS_is_account'])){
                    $this->DNS_is_account = false;
                }elseif(!$this->DNS_is_account && $this->acl_is_writeable("dnsSetup") && isset($_POST['DNS_is_account'])){
                    $this->DNS_is_account = true;
                }
            }

            /* Get dns attributes */
            if(($this->DNSenabled) && (isset($_POST['network_tpl_posted'])) && $this->acl_is_writeable("dnsSetup")){

                /* Check for posted record changes */
                if(is_array($this->dnsEntry['RECORDS'])){
                    foreach($this->dnsEntry['RECORDS'] as $key => $value){

                        /* Check if type has changed */
                        if(isset($_POST['RecordTypeSelectedFor_'.$key])){
                            $this->dnsEntry['RECORDS'][$key]['type'] = get_post('RecordTypeSelectedFor_'.$key);
                        }
                        /* Check if value has changed */
                        if(isset($_POST['RecordValue_'.$key])){
                            $this->dnsEntry['RECORDS'][$key]['value'] = get_post('RecordValue_'.$key);
                        }
                    }
                }

                /* Get all basic DNS attributes (TTL, Clas ..)*/
                foreach($this->DNSattributes as $attr){
                    if(isset($_POST[$attr])){
                        $this->dnsEntry[$attr] = get_post($attr);
                    }
                }
            }
            if($this->hide_dns_check_box){
                $this->DNS_is_account = true;
            }
        }
    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        /******
          check additional IP Host Numbers 
         ******/

        foreach($this->additionalHostNumbers as $id => $value){
            if(!tests::is_ip($value)){
                $message[]= msgPool::invalid(sprintf(_("IP address %s"),($id +2)), "", "", "192.168.1.10");
            }
        }


        /* Check if mac and ip are already used */
        if(!empty($this->ipHostNumber) && $this->DNS_is_account && 
                $this->ipHostNumber != $this->orig_ipHostNumber && 
                in_array_strict("ip:".$this->ipHostNumber,$this->used_ip_mac)){
            $message[]= msgPool::duplicated(_("IP address"));
        }
        if(!empty($this->macAddress) && $this->dhcp_is_Account && 
                $this->macAddress != $this->orig_macAddress && 
                in_array_strict("mac:".$this->macAddress,$this->used_ip_mac)){
            $message[]= msgPool::duplicated(_("MAC address"));
        }

        /* Check if ip must be given
         */  
        if(($this->IPisMust)||($this->DNS_is_account)){
            if (empty($this->ipHostNumber)){
                $message[]= msgPool::required(_("IP address"));
            }elseif (!tests::is_ip($this->ipHostNumber)){
                $message[]= msgPool::invalid(_("IP address"), "", "", "192.168.1.10");
            }
        }

        /* Check if mac is empty 
         */
        if($this->MACisMust || $this->dhcp_is_Account){
            if ($this->macAddress == "" ){
                $message[]= msgPool::required(_("MAC address"));
            }elseif(!tests::is_mac($this->macAddress)){
                $message[]= msgPool::invalid(_("MAC address"), "", "", "00:0C:7F:31:33:F1");
            }
        }

        /* only perfrom this checks if this is a valid DNS account */
        if($this->DNS_is_account){

            $checkArray = array();
            $onlyOnce   = array();

            //  $onlyOnce['cNAMERecord'] = 0;
            $tmp = array_flip($this->Zones);
            $tmp2 = $tmp[$this->dnsEntry['zoneName']];
            if(!$this->netmaskIsCoherent($tmp2)){ //this->dnsEntry['zoneName'])){
                $tmp2 = preg_replace("/^.*\//","",$tmp2);
                $message[] =sprintf(_("The IP address '%s' is not part of the selected reverse zone '%s'!"),$this->ipHostNumber,$tmp2);
            }

            /* Walk through all entries and detect duplicates or mismatches
             */  
            foreach($this->dnsEntry['RECORDS'] as $name => $values){

                /* Count record values, to detect duplicate entries for a specific record
                 */
                if(!isset($checkArray[$values['type']][$values['value']])){
                    $checkArray[$values['type']][$values['value']] = 0;
                }else{
                    $message[] = sprintf(_("Record type '%s' is duplicated!"),$values['type']);
                }

                /* Check if given entries in $onlyOnce are used more than once
                 */
                if(isset($onlyOnce[$values['type']])){
                    $onlyOnce[$values['type']] ++;
                    if($onlyOnce[$values['type']] > 1){
                        $message[] = sprintf(_("Unique record type '%s' is duplicated!"),$values['type']);
                    }
                }

                /* Skip txt record ... 
                 */
                if($values['type'] == "tXTRecord") continue;

                /* Check if there is an aRecord defined which uses the same IP as used in IPhostAddress 
                 */
                if(($values['type'] == "aRecord")&&($values['value'] == $this->ipHostNumber)){
#TODO: Where's the problem here?
                    $message[]=sprintf(_("The IP address '%s' will be added as 'A Record', this will be done automatically, please remove the record."), 
                            $this->ipHostNumber);
                }

                /* only lower-case is allowed in record entries ... 
                 */
                if($values['value'] != strtolower($values['value'])){
#TODO: What's in values['value']? Something for a propper message?
                    $message[] = sprintf(_("Only lowercase records are allowed, please check your '%ss'."),$values['type']);
                }
            }
            }
            return ($message);
        }


        /* Save to LDAP */
        function save()
        {
            $ldap= $this->config->get_ldap_link();

            $dn = $this->parent->dn;

            /*******************/ 
            /* IP-MAC HANDLING */
            /*******************/ 

            /* $dn was posted as parameter */
            $this->dn = $dn;

            /* Save DNS setting & ip/Mac*/
            plugin::save();

            /* Add all additional ipHostNumbers now 
             */
            if(!empty($this->ipHostNumber)){
                $this->attrs['ipHostNumber'] = array($this->ipHostNumber);
            }
            foreach($this->additionalHostNumbers as $value){
                $this->attrs['ipHostNumber'][] = $value;
            }

            /* Do not add the objectClass ipHost if no ip address is given */
            if(!isset($this->attrs['ipHostNumber'])){
                $this->attrs['objectClass'] = array_remove_entries(array("ipHost"),$this->attrs['objectClass']);
            }

            /* Write back to ldap */
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify ($this->attrs); 

            /****************/ 
            /* DHCP HANDLING */
            /****************/ 

            /* New entry */
            if($this->dhcpEnabled && $this->acl_is_writeable("dhcpSetup")) {

                if(count($this->dhcpHostEntry) == 0){
                    $this->dialog = new dhcpHost($this->parent,$this->dhcpParentNode,TRUE);
                    $this->dialog->cn = $this->cn;
                    $this->dialog->dhcpHWAddress = "ethernet ".$this->macAddress;
                    if(!empty($this->ipHostNumber)){
                        $this->dialog->statements->set('fixed-address', $this->ipHostNumber);
                    }
                    $this->dialog->execute();
                    $this->dialog->save_object(); 
                    $this->dhcpHostEntry = $this->dialog->save();
                    if(count($this->dhcpHostEntry['dhcpOption']) == 0){
                        $this->dhcpHostEntry['dhcpOption']= array("host-name ".$this->cn);
                    }
                }else{

                    // Updated Host-Name entry 
                    foreach($this->dhcpHostEntry['dhcpOption'] as $key => $entry){
                        if(preg_match("/^host-name/", $entry)){
                            $this->dhcpHostEntry['dhcpOption'][$key] = "host-name ".$this->cn;
                            break;
                        }
                    }
                }

                /* Write mac address to dhcp settings */
                if($this->dhcp_is_Account){
                    if(!isset($this->dhcpHostEntry['dhcpHWAddress'][0]) || 
                            !preg_match("/ethernet ".$this->macAddress."/",$this->dhcpHostEntry['dhcpHWAddress'][0])){
                        $this->dhcpHostEntry['dhcpHWAddress'] = array("ethernet ".$this->macAddress);
                        $this->dhcpHostEntry['MODIFIED'] = TRUE;
                    }
                }

                /* Updated IP host number */
                if($this->dhcp_is_Account && isset($this->dhcpHostEntry['dhcpStatements'])){
                    foreach($this->dhcpHostEntry['dhcpStatements'] as $id => $value){
                        if(preg_match("/^fixed-address/",$value)){
                            $this->dhcpHostEntry['dhcpStatements'][$id] = "fixed-address ".$this->ipHostNumber; 
                            $this->dhcpHostEntry['MODIFIED'] = TRUE;
                        }
                    }
                }

                /* Unset dhcpStatements if this attribute is empty  */
                if(isset($this->dhcpHostEntry['dhcpStatements']) && 
                        ($this->dhcpHostEntry['dhcpStatements'] == "" || count($this->dhcpHostEntry['dhcpStatements']) == 0) ){
                    unset($this->dhcpHostEntry['dhcpStatements']);
                }

                /* DHCP removed */
                if($this->initial_dhcp_is_Account && !$this->dhcp_is_Account){
                    $ldap->rmdir_recursive($this->dhcpHostEntry['dn']);
                    if (!$ldap->success()){
                        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dhcpHostEntry['dn'], LDAP_DEL, get_class()));
                    }

                    $tmp = new servdhcp($this->config,$this->dhcpParentNode);
                    $tmp->handle_post_events("remove");
                }

                /* DHCP Added */
                if(!$this->initial_dhcp_is_Account && $this->dhcp_is_Account){
                    $attrs = $this->dhcpHostEntry;
                    unset($attrs['MODIFIED']);
                    unset($attrs['dn']);
                    $ldap->cd("cn=".$this->cn.",".$this->dhcpParentNode);
                    $res = $ldap->add($attrs);

                    $tmp = new servdhcp($this->config,$this->dhcpParentNode);
                    $tmp->handle_post_events("add");

                    if (!$ldap->success()){
                        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), "cn=".$this->cn.",".$this->dhcpParentNode, LDAP_ADD, get_class()));
                    }
                }

                /* DHCP still activated */
                if($this->initial_dhcp_is_Account && $this->dhcp_is_Account){

                    /* DHCP node changed */
                    if(($this->initial_dhcpParentNode != $this->dhcpParentNode) || 
                            ($this->cn != $this->OrigCn)){
                        $attrs = $this->dhcpHostEntry;
                        $attrs[$this->namingAttr] = $this->cn;
                        unset($attrs['dn']);
                        unset($attrs['MODIFIED']);
                        $ldap->cd("cn=".$this->cn.",".$this->dhcpParentNode);
                        $res = $ldap->add($attrs);

                        $tmp = new servdhcp($this->config,$this->dhcpParentNode);
                        $tmp->handle_post_events("modify");

                        if (!$ldap->success()){
                            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), "cn=".$this->cn.",".$this->dhcpParentNode, LDAP_ADD, get_class()));
                        }
                        if($res){
                            $ldap->rmdir_recursive($this->dhcpHostEntry['dn']);
                            if (!$ldap->success()){
                                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dhcpHostEntry['dn'], LDAP_DEL, get_class()));
                            }
                        }
                    }

                    /* SAME node but modified */ 
                    if(isset($this->dhcpHostEntry['MODIFIED']) && $this->dhcpHostEntry['MODIFIED'] == 1  && 
                            $this->initial_dhcpParentNode == $this->dhcpParentNode){
                        $attrs = $this->dhcpHostEntry;
                        unset($attrs['dn']);
                        unset($attrs['MODIFIED']);
                        $ldap->cd($this->dhcpHostEntry['dn']);
                        $ldap->modify($attrs);

                        $tmp = new servdhcp($this->config,$this->dhcpParentNode);
                        $tmp->handle_post_events("modify");

                        if (!$ldap->success()){
                            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dhcpHostEntry['dn'], LDAP_MOD, get_class()));
                        }
                    }    
                }
            }
            $this->dialog = FALSE; 

            /****************/ 
            /* DNS HANDLING */
            /****************/ 

            /* If isn't DNS account but initially was DNS account 
               remove all DNS entries 
             */ 
            if((!$this->DNSenabled) || ((!$this->DNS_is_account)&&(!$this->initially_was_account))){
                return;
            }elseif($this->acl_is_writeable("dnsSetup")){

                /* Add ipHostNumber to aRecords
                 */
                $backup_dnsEntry = $this->dnsEntry;
                if(!empty($this->ipHostNumber)){
                    $this->dnsEntry['RECORDS'][] = array("type"=>"aRecord","value"=>$this->ipHostNumber);
                    $ptr = $this->get_pTRRecord();
                    if(!empty($ptr)){
                        $this->dnsEntry['RECORDS'][] = array("type"=>"pTRRecord","value"=>$ptr);
                    } 
                }

                /* Create diff and follow instructions 
                 * If Account was disabled, remove account by setting exists to false
                 */
                if((!$this->DNS_is_account)&&($this->initially_was_account)){  
                    $this->dnsEntry['exists'] = false;
                    $tmp = DNS::getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);
                }else{
                    $this->dnsEntry['exists'] = $this->DNS_is_account;
                    $tmp = DNS::getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);
                }   

                /* Update SOA record if needed
                 */
                $this->updateSOARecord();

                /* move follwoing entries 
                 */
                foreach($tmp['move'] as $src => $dst){
                    $this->recursive_move($src,$dst);
                }

                /* Delete dns */
                foreach($tmp['del'] as $dn => $del){
                    $ldap->cd($dn);
                    $ldap->rmdir_recursive($dn);
                    new log("modify","unknown/".get_class($this),$dn,array("*"),$ldap->get_error());
                }

                /* Add || Update new DNS entries 
                 */
                foreach($tmp['add'] as $dn => $attrs){
                    $ldap->cd($dn);
                    $ldap->cat($dn, array('dn'));
                    if(count($ldap->fetch())){
                        $ldap->cd($dn);
                        $ldap->modify ($attrs); 
                    }else{
                        $ldap->cd($dn);
                        $ldap->add($attrs);
                    }
                    new log("modify","unknown/".get_class($this),$dn,array_keys($attrs),$ldap->get_error());
                }


                /* Display errors 
                 */
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $dn, 0, get_class()));
                }

                $tmp2 = new servdns($this->config,$this->dn);
                $tmp2->handle_post_events("modify");

                $this->dnsEntry =  $backup_dnsEntry;
            }
        }

        /*  Create html table with all used record types
         */
        function generateRecordsList()
        {
            $changeStateForRecords = "";
            $str = "<table summary='' width='100%'>";
            foreach($this->dnsEntry['RECORDS'] as $key => $entry){

                $changeStateForRecords.= "changeState('RecordTypeSelectedFor_".$key."');\n";
                $changeStateForRecords.= "changeState('RecordValue_".$key."');\n";
                $changeStateForRecords.= "changeState('RemoveRecord_".$key."');\n";

                $str.=" <tr>".
                    "   <td>".$this->generateRecordListBox($entry['type'],"RecordTypeSelectedFor_".$key)."</td>".
                    "   <td><input type='text' value=\"".htmlentities($entry['value'])."\" name='RecordValue_".$key."' id='RecordValue_".$key."'></td>".
                    "   <td><button type='submit' name='RemoveRecord_".$key."' id='RemoveRecord_".$key."'>".msgPool::delButton()."</button>";
                "</tr>";
            }

            $str.= "  <tr>".
                "    <td colspan=2 width='50%'></td><td>".
                "      <button type='submit' name='AddNewRecord' id='AddNewRecord'>".msgPool::addButton()."</button>".
                "    </td>".
                "  </tr>".
                "</table>";
            $ret =  array("str" => $str, "changeStateForRecords" => $changeStateForRecords);

            return($ret);
        }


        /* Create a html select box which allows us to select different types of records 
         */
        function generateRecordListBox($selected,$name)
        {
            $str = "<select name='".$name."' id='".$name."'>";
            foreach($this->RecordTypes as $type => $value){
                $use = "";
                if($type == $selected){
                    $use = " selected ";
                }
                $str.="\n <option value='".$type."' ".$use.">".strtoupper(preg_replace("/record/i","",$type))."</option>";
            }
            $str.="</select>";
            return($str); 
        }


        /* Return plugin informations for acl handling  */ 
        static function plInfo()
        {
            $tmp =  array(
                    "plShortName"   => _("DNS"),
                    "plDescription" => _("DNS settings"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 5,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("workstation","terminal","phone","server","component","printer","winworkstation","opsi"),

                    "plProvidedAcls"=> array(
                        "ipHostNumber"  => _("IP address"),
                        "macAddress"    => _("MAC address"))
                    );

            /* Hide all dns/dhcp configurations if not available
             */
            if(class_available("servdns")){
                $tmp['plProvidedAcls']["dnsSetup"]    = _("DNS configuration");
            }
            if(class_available("servdhcp")){
                $tmp['plProvidedAcls']["dhcpSetup"]   = _("DHCP configuration");
            }
            return($tmp);
        }


        function get_dhcp_host_entry()
        {
            $attrs = array();
            $dn = $this->get_dhcp_host_entry_dn();
            if($dn){
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($this->config->current['BASE']);
                $ldap->cat($dn,array("*"));
                if($ldap->count()){
                    $attrs = $ldap->fetch();
                    foreach($attrs as $key => $value){
                        if(is_numeric($key) || ($key == "count")){
                            unset($attrs[$key]);
                        }
                        if(is_array($value) && isset($value['count'])){
                            unset($attrs[$key]['count']);
                        }
                    }
                }
            }
            return($attrs);
        }


        function get_dhcp_host_entry_dn()
        {
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=dhcpHost)(cn=".$this->cn."))",array("uid","cn","dn"));

            if($ldap->count()){
                $attr = $ldap->fetch();
                return($attr['dn']);
            }else{
                return("");
            }
        }  


        function get_dhcp_parent_node()
        {
            return(preg_replace("/^cn=".preg_quote($this->cn, '/').",/","",$this->get_dhcp_host_entry_dn()));
        }


        function get_dhcp_parent_nodes()
        {
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(objectClass=dhcpService)",array("dhcpPrimaryDN"));

            $dhcp_dns = array();
            while($attr = $ldap->fetch()){
                $dhcp_dns[$attr['dn']] = $attr['dhcpPrimaryDN'][0];
            }

            foreach($dhcp_dns as $key => $pri_dns){
                $ldap->cat($pri_dns,array("cn"));
                $tmp = $ldap->fetch();
                if(isset($tmp['cn'][0])){
                    $dhcp_dns[$key] = $tmp['cn'][0];
                }else{
                    unset($dhcp_dns[$key]);
                }
            }

            $tmp = $tmp2 = array();
            foreach($dhcp_dns as $dn => $cn){
                $ldap->cd($dn);
                $ldap->search("(|(objectClass=dhcpService)(objectClass=dhcpGroup)".
                        "(objectClass=dhcpSubnet)(objectClass=dhcpSharedNetwork))",array("cn"));
                while($attr = $ldap->fetch()){
                    $tmp[$attr['dn']] = $attr['cn'][0];
                }
                $tmp2 = array_merge($tmp2,$this->create_tree($tmp,preg_replace("/^[^,]+,/i","",$dn),"(".$cn.")&nbsp;"));
            }
            return($tmp2);
        }


        /* this function returns the default ptr record entry */
        function get_pTRRecord()
        {
            if(!empty($this->ipHostNumber) && isset($this->dnsEntry['zoneName']) && !empty($this->dnsEntry['zoneName'])){
                $ldap = $this->config->get_ldap_link();
                $ldap->cat(DNS::getDNSZoneDN($this->config,$this->dnsEntry['zoneName']));
                $attrs = $ldap->fetch();
                $tmp = array_flip($this->Zones);
                $tmp = preg_replace("/^[^\/]*+\//","",$tmp[$this->dnsEntry['zoneName']]);
                $tmp = trim(preg_replace("/\.in-addr.arpa\.$/","",$tmp));
                $ptr = preg_replace("/^".preg_quote(DNS::FlipIp($tmp), '/')."\./","",$this->ipHostNumber);
                return($ptr);
            }else{
                return(FALSE);
            }
        }


        function generateRandomIP($net = "")
        {
            $str = $net;
            $cnt = 4;

            // first gather all IPs 
            $ldap = $this->config->get_ldap_link();
            $ocs = 
                "(objectClass=goFonHardware)".
                "(objectClass=goServer)".
                "(objectClass=GOhard)".
                "(objectClass=gotoTerminal)".
                "(objectClass=gotoWorkstation)".
                "(objectClass=gotoPrinter)".
                "(objectClass=ipHost)";
            $list = array();
            $ldap->search("(&(|{$ocs})(ipHostNumber=*))",array("ipHostNumber"));
            while($attrs = $ldap->fetch()){
                if (preg_match("/^$net\./", $attrs['ipHostNumber'][0])) {
                    $list[] = $attrs['ipHostNumber'][0];
                }
            }

            // Set starting ip.
            $ip_data = preg_split("/\./",$net);
            for($i=0;$i<4;$i++){
                if(!isset($ip_data[$i])) $ip_data[$i] = 0;
            }

            // Search the next free and valid ip.
            while(in_array_strict(implode(".",$ip_data),$list) || $ip_data[3] <= 1){
                $ip_data[3] ++ ;
                if($ip_data[3] > 255){
                    $ip_data[3] = 1 ;
                    $ip_data[2] ++ ;
                }
                if($ip_data[2] > 255){
                    $ip_data[2] = 1 ;
                    $ip_data[1] ++ ;
                }
                if($ip_data[1] > 255){
                    $ip_data[1] = 1 ;
                    $ip_data[0] ++ ;
                }
                if($ip_data[0] > 255) break;
            }

            return(implode(".",$ip_data));
        }


        function create_tree($arr,$base,$current = "")
        {
            $ret = array();
            foreach($arr as $r => $name){
                $base_part = str_replace($base,"",$r);
                if(preg_match("/^[a-z]*=".preg_quote($name, '/')."(|,)$/i",$base_part)){
                    $ret[$r] = $current.$name;
                    $tmp = $this->create_tree($arr,$r,$current.".&nbsp;");
                    foreach($tmp as $sub_key => $sub_name){
                        $ret[$sub_key] = $sub_name;
                    }
                } 
            }
            return($ret);
        }

        function force_dns()
        {
            if($this->DNSenabled){

                /* Only force DNS account, if we have at least on dns Zone */
                if(count($this->Zones)){
                    $this->DNS_is_account  = TRUE;
                    $this->hide_dns_check_box = TRUE;
                }
            }
        }

        function updateSOARecord()
        {
            $oldEntries = DNS::getDNSHostEntries($this->config,$this->OrigCn);
            $newEntries = $this->dnsEntry;
            if(array_differs($newEntries['RECORDS'],$oldEntries['RECORDS'])){
                DNS::touchDNSZone($this->config,$this->dnsEntry['zoneName']);
            }
        }
    }

    // vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
    ?>
