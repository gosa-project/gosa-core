<?php

class servdns extends goService
{
    /* attribute list for save action */
    var $ignore_account   = FALSE;
    var $attributes       = array(); 
    var $objectclasses    = array("whatever");

    var $RecordTypes      = array();
    var $Zones            = array();

    var $orig_dn          = "";

    var $initially_was_account;

    /* ServerService tab vars */
    var $conflicts        = array("servdns");
    var $DisplayName      = "";
    var $StatusFlag       = "";
    var $view_logged      = FALSE;

    var $dns_server_list   = array("ENTRIES"=> array(),"FOR_LIST"=> array());
    var $take_over_id       = -1;


    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config, $dn, $parent);

        $this->DisplayName = _("DNS service");

        $this->orig_dn = $dn;

        /* Get record types for zones
         */
        $this->RecordTypes = DNS::getDnsRecordTypes(true);

        /* Get all zone Informations
         */
        $this->Zones = DNS::getDNSZoneEntries($config,$dn);

        /* If there is at least one entry in this -> types, we have DNS enabled 
         */
        if(count($this->Zones) == 0){
            $this->is_account = false;
            $this->dns_server_list = $this->get_list_of_dns_servers();
        }else{
            $this->is_account = true;
        }
        $this->initially_was_account = $this->is_account;

        // Prepare lists
        $this->zoneList = new sortableListing();
        $this->zoneList->setDeleteable(true);
        $this->zoneList->setEditable(true);
        $this->zoneList->setWidth("100%");
        $this->zoneList->setHeight("300px");
        $this->zoneList->setHeader(array(_("Zone"),_("Reverse zone"),_("TTL"),_("Class")));
        $this->zoneList->setColspecs(array('*','*','*','*','40px'));
        $this->zoneList->setDefaultSortColumn(0);
    }


    function get_list_of_dns_servers()
    {
        $ret = array("ENTRIES"=> array(),"FOR_LIST"=> array());
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=dNSZone)(zoneName=*))",array("dn","zoneName"));
        $dns = array();
        while($attrs = $ldap->fetch()){
            /* Skip own config */
            if($this->dn != "new" && preg_match("/".preg_quote($this->dn, '/')."$/",$attrs['dn'])){
                continue;
            }
            $dn = preg_replace("/^zoneName=[^,]+,/","",$attrs['dn']);
            if(preg_match("/^cn=/",$dn) && !in_array_strict($dn,$dns)){
                $dns[] = $dn;
            }
        }
        $i = 0;
        foreach($dns as $dn){
            $ldap->cat($dn,array('*'));
            if($ldap->count()){
                $i ++;
                $attrs = $ldap->fetch();
                $ret['ENTRIES'][$i]   = $attrs;
                $ret['FOR_LIST'][$i] = $attrs['cn'][0];
            }
        }
        return($ret);
    }  


    function get_dns_info_string($id)
    {
        $ret="";
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dns_server_list['ENTRIES'][$id]['dn']);
        $ldap->search("(|(zoneName=*)(relativeDomainName=*))",array("dn"));
        while($attrs = $ldap->fetch()){
            $ret .= $attrs['dn']."\n";
        }
        return($ret);
    }


    function execute()
    {
        /* Call parent execute 
         */
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        /* Fill templating stuff 
         */
        $smarty= get_smarty();
        $smarty->assign("dns_take_over",FALSE);
        $smarty->assign("is_createable",$this->acl_is_createable());
        $display= "";


        $this->initially_was_account= $this->is_account;
        /*****************/
        /* Handle Take Over Actions
        /*****************/

        /* Give smarty the required informations */
        $smarty->assign("dns_server_list", $this->dns_server_list['FOR_LIST']);
        $smarty->assign("dns_server_list_cnt", count($this->dns_server_list['FOR_LIST']));

        /* Take over requested, save id */
        if(isset($_POST['take_over_src']) && isset($_POST['take_over'])){
            $id = get_post('take_over_src');
            if(isset($this->dns_server_list['ENTRIES'][$id])){
                $this->take_over_id = $id;
            }
        }

        /* Abort take over action */
        if(isset($_POST['cancel_take_over'])){
            $this->dialog =false;
            $this->take_over_id = -1;
            $this->dns_server_list = $this->get_list_of_dns_servers();
        }

        /* Display informartion about take over that will be started when saving this server
         *  and hide default dns output
         */
        if($this->take_over_id != -1){
            $this->dialog = FALSE;
            $id = $this->take_over_id;
            $info = $this->get_dns_info_string($id);
            $smarty->assign("dns_take_over",TRUE);
            $smarty->assign("info",$info);
            $warning = sprintf(_("You are going to migrate the DNS setup from server '%s'."),$this->dns_server_list['ENTRIES'][$id]['cn'][0]);
            $warning2 = _("The migration will be started when you save this system. To cancel this action, use the cancel button below.");
            $smarty->assign("warning",$warning);
            $smarty->assign("warning2",$warning2);
            return($smarty->fetch(get_template_path('servdns.tpl', TRUE, dirname(__FILE__))));
        }


        /* Do we need to flip is_account state? 
         */
        if (isset($_POST['modify_state'])){
            $this->is_account= !$this->is_account;
        }

        /* Edited or Added zone 
         */
        if(isset($_POST['SaveZoneChanges'])){
            $this->dialog->save_object();

            /* Check for errors  
             */
            if(count($this->dialog->check())){
                foreach($this->dialog->check() as $msgs){
                    msg_dialog::display(_("Error"), $msgs, ERROR_DIALOG);
                }
            }else{
                /* add new/edited zone 
                 */
                $ret = $this->dialog->save();
                if(!$this->dialog->isNew){
                    unset($this->Zones[$this->dialog->OldZoneName]);
                }
                $this->Zones[$ret['zoneName']] = $ret;
                $this->dialog = FALSE;
            }
        }

        /* Cancel zone edit / new 
         */
        if(isset($_POST['CancelZoneChanges'])){
            $this->dialog = FALSE;
        }

        /* Add empty new zone 
         */
        if(isset($_POST['AddZone'])){
            $this->dialog = new servdnseditZone($this->config,$this->dn);
            if($this->is_new){
                $this->dialog->acl_base = $this->acl_base;
                $this->dialog->acl_category = $this->acl_category;
            }
        }

        /* Check for edit zone request 
         */
        $this->zoneList->save_object();
        $action = $this->zoneList->getAction();
        if($action['action'] == 'delete'){
            $id = $this->zoneList->getKey($action['targets'][0]);
            $this->RemoveZone($id);
        }
        if($action['action'] == 'edit'){
            $id = $this->zoneList->getKey($action['targets'][0]);
            $this->dialog= new servdnseditZone($this->config,$this->dn,$this->Zones[$id]);
            $this->dialog->acl_base = $this->acl_base;
            $this->dialog->acl_category = $this->acl_category;
        }


        /* Show dialog 
         */
        if(is_object($this->dialog)){
            $this->dialog->save_object();
            $this->dialog->parent = $this;
            return($this->dialog->execute());
        }

        /* Create Listbox with existing Zones 
         */
        $this->zoneList->setAcl($this->getacl(""));

        $lData = array();
        foreach($this->Zones as $zone => $values ){
            $lData[$zone] = array('data' => array($zone,$values['ReverseZone'],$values['sOAttl'],$values['dNSClass']));
        }    
        $this->zoneList->setListData($this->Zones,$lData);
        $this->zoneList->update();

        /* Display tempalte 
         */
        $smarty->assign("ZoneList",$this->zoneList->render());
        $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE, dirname(__FILE__)));
        return($display);
    }


    /* Delete specified zone
     */
    function RemoveZone($id,$force = FALSE)
    {
        $zones =  $this->getUsedZoneNames();

        if(isset($this->Zones[$id]['InitialReverseZone'])){
            $rev = DNS::FlipIp($this->Zones[$id]['InitialReverseZone']);
        }else{
            $rev = DNS::FlipIp($this->Zones[$id]['ReverseZone']);
        }

        $zonename = "";
        if(isset($this->Zones[$id]['InitialzoneName'])){
            $zonename= $this->Zones[$id]['InitialzoneName'];
        }

        $used = array();

        /* Add Records which use this zoneName
         */
        if(isset($zones[$zonename])){
            $used = array_merge($used,$zones[$zonename]);
        }

        /* Add Records which uses this reverse zone
         */
        if(isset($zones[$rev.".in-addr.arpa."])){
            $used = array_merge($used,$zones[$rev.".in-addr.arpa."]);
        } 

        /* There are still entries using this configuration
         *  Abort deletion
         */
        if(count($used) && !$force){
            $i = 2;
            $str ="";
            foreach($used as $dn){
                if($i > 0 && !preg_match("/,relativeDomainName=/",$dn)){
                    $i --;
                    $name = preg_replace("/^[^=]+=([^,]*),.*$/","\\1",$dn);
                    $zone = preg_replace("/^.*zoneName=([^,]*),.*$/","\\1",$dn);
                    $str.= $name.".".$zone." ";
                }
            }

            /*  Only show 2 dns in the error message 
             */
            if(count($used)> 2) {
                $str .=" ... ";
            }
            msg_dialog::display(_("Error"), sprintf(_("Cannot delete the selected zone. It is still in use by '%s'"), trim($str)), ERROR_DIALOG);
            return(false);
        }else{
            unset($this->Zones[$id]);
            return(true);
        }
    } 


    /* This funtion returns all used Zonenames 
     */
    function getUsedZoneNames()
    {
        $ret = array();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=dNSZone)(!(relativeDomainName=@))(zoneName=*))",array("zoneName","relativeDomainName"));
        while($attr = $ldap->fetch()){
            $ret[$attr['zoneName'][0]][] = $attr['dn'];
        }
        return($ret);
    }


    /* Remove dns service 
     */
    function remove_from_parent()
    {
        if($this->initially_was_account){
            $bool = true;
            $this->is_account = FALSE;
            foreach($this->Zones as $key => $zone){
                $bool= $bool & $this->RemoveZone($key,TRUE);
            }
            if($bool){
                $this->save();
            }
            return($bool);
        }
    }


    /* Save to LDAP */
    function save()
    {

        /* Take over handling
         * - Create list of zones managed by source server 
         * - Copy ldap entries to destination server 
         * - Remove old zone entries from source
         */
        if($this->take_over_id != -1){
            $del = array();
            $id = $this->take_over_id;
            $src = $this->dns_server_list['ENTRIES'][$id]['dn'];
            $ldap = $this->config->get_ldap_link(); 
            $ldap->ls("(objectClass=dnsZone)",$src,array('cn'));
            while($attrs = $ldap->fetch()){
                $src_zone = $attrs['dn'];
                $dst_zone = preg_replace("/".preg_quote($src, '/')."$/",$this->dn,$src_zone);
                $res = plugin::recursive_move($src_zone, $dst_zone);

                if($res){
                    $del [] = $src_zone;
                }
            }
            foreach($del as $src_zone){
                $ldap->rmdir_recursive($src_zone);
            }
            return;
        }

        /* Save zone editor changes now */
        foreach($this->Zones as $name => $zone){
            if(isset($zone['zoneEditor'] ) && $zone['zoneEditor'] != NULL && is_object($zone['zoneEditor'])){
                $zone['zoneEditor']->save();
                unset($this->Zones[$name]['zoneEditor']);;
            }
        }

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);  

        /* Get differences 
         */
        $old_dn = $this->orig_dn;
        if($old_dn == "new"){
            $old_dn = $this->dn;
        }

        /* Update dns to current object dn */ 
        $tmp = DNS::getDNSZoneEntriesDiff($this->config,$this->Zones,$old_dn);
        $tmp2 = array();
        foreach($tmp as $key1 => $data1){
            $tmp2[$key1] = array();
            foreach($data1 as $key2 => $data2){
                $tmp2[$key1][preg_replace("/".preg_quote($old_dn, '/')."$/",$this->dn,$key2)] = $data2;
            }
        }
        $tmp = $tmp2;

        /* Updated zone entries if reverser or forward name has changed  
         * Must be done before moving entries, else the given dn is invalid
         */
        if(isset($tmp['zoneUpdates'])){
            foreach($tmp['zoneUpdates'] as $dn => $attrs){
                $ldap->cd($dn);
                $ldap->modify($attrs);
                new log("modify","unknown/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
                }
            }
        }

        /* Delete dns 
         */
        foreach($tmp['del'] as $dn => $del){

            $for = $del['InitialzoneName'];
            $rev = DNS::FlipIp($del['InitialReverseZone']).".in-addr.arpa.";

            $ldap->cd($dn);
            $ldap->rmdir_recursive($dn);
            new log("remove","unknown/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
            }

            /* Handle Post events */
            if(preg_match("/^zoneName=/",$dn)){
#        $this->handle_post_events("remove",array("dn" => $dn,"zoneName" => $for));
#        $this->handle_post_events("remove",array("dn" => $dn,"zoneName" => $rev));
            }
        }

        /* move follwoing entries
         */
        foreach($tmp['move'] as $src => $dst){
            $this->recursive_move($src,$dst);
        }

        /* Add || Update new DNS entries
         */
        foreach($tmp['add'] as $dn => $attrs){
            $ldap->cd($dn);
            $ldap->cat($dn, array('dn'));
            if($ldap->fetch()){
                $ldap->cd($dn);
                $ldap->modify ($attrs);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
                }

                /* Handle Post events */
                if(preg_match("/^zoneName=/",$dn)){
#          $this->handle_post_events("modify",array("dn" => $dn,"zoneName" => $attrs['zoneName']));
                }
            }else{
                $ldap->cd($dn);
                $ldap->add($attrs);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_ADD, get_class()));
                }

                /* Handle Post events */
                if(preg_match("/^zoneName=/",$dn)){
#          $this->handle_post_events("add",array("dn" => $dn,"zoneName" => $attrs['zoneName']));
                }
            }
        }
        $this->handle_post_events("modify");
    }


    /* Directly save new status flag */
    function setStatus($value)
    {
        if($value == "none") return;
        if(!$this->initially_was_account) return;
        if(empty($this->StatusFlag)) return;
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


    function getListEntry()
    {
        $fields               = goService::getListEntry(); 
        $fields['Message']    = _("DNS service");
#$fields['AllowEdit']  = true;
        return($fields);
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


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("DNS service"),
                    "plDescription" => _("DNS service")." ("._("Services").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 83,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("server"),

                    "plProvidedAcls"=> array(
                        "start"         => _("Start service"),  // Remove this to hide the start button at all.
                        "stop"          => _("Stop service"),   // Remove this to hide the stop button at all.
                        "restart"       => _("Restart service"),// Remove this to hide the restart button at all.

                        "zoneName"      =>_("Zone name"),
                        "ReverseZone"   =>_("Reverse zone"),
                        "NetworkClass"  =>_("Network class"),
                        "zoneEditor"    =>_("Zone entry editor"),
                        "sOAprimary"    =>_("Primary DNS server"),
                        "sOAmail"       =>_("Mail address"),
                        "sOAserial"     =>_("Serial"),
                        "sOArefresh"    =>_("Refresh"),
                        "sOAretry"      =>_("Retry"),
                        "sOAexpire"     =>_("Expire"),
                        "sOAttl"        =>_("TTL"),
                        "mXRecord"      =>_("MX records"),
                        "zoneRecords"   =>_("Zone records"))
                        ));
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
