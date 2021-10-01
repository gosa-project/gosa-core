<?php

class servDNSeditZoneEntries extends plugin
{
    /* attribute list for save action */
    var $ignore_account     = TRUE;
    var $attributes         = array();
    var $objectclasses      = array("whatever");

    var $Devices            = array();

    var $zoneName           = "";   // ZoneName of currently edited Zone
    var $reverseName        = "";   // ReverseZone of the currently edited Zone

    var $RecordTypes        = array();  // Possible record type.
    var $acl                = "";
    var $disableDialog      = false; // Dialog will be disabled, if this zone is new 


    function __construct(&$config,$dn, &$zoneObject)
    {
        plugin::__construct ($config, $dn);

        /* Initialise class
         */
        $this->RecordTypes  = DNS::getDnsRecordTypes();
        $this->dn           = "zoneName=".$zoneObject['InitialzoneName'].",".$dn; 
        $this->zoneName     = $zoneObject['InitialzoneName'];
        $this->reverseName  = $zoneObject['InitialReverseZone'];

        /* Remove nSRecord from listed types */
        if(isset($this->RecordTypes['nSRecord'])){
            unset($this->RecordTypes['nSRecord']);
        }
        /* Remove nSRecord from listed types */
        if(isset($this->RecordTypes['pTRRecord'])){
            unset($this->RecordTypes['pTRRecord']);
        }

        /* Get ldap connection 
         */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);

        /* Get zone content
         */
        $ldap->ls("(&(objectClass=dNSZone)(!(relativeDomainName=@)))",$this->dn,array("relativeDomainName"));

        while($attrs = $ldap->fetch()){
            $this->Devices[$attrs['relativeDomainName'][0]] = DNS::getDNSHostEntries($config,$attrs['relativeDomainName'][0],true);
            $this->Devices[$attrs['relativeDomainName'][0]]['OrigCn'] = $attrs['relativeDomainName'][0];
        }

        $ldap->cat($this->dn,array("objectClass"));

        $this->disableDialog = true;
        if(count($this->Devices)|| $ldap->count()){
            $this->disableDialog = false;
        }
    }

    function execute()
    {
        plugin::execute();

        /* Fill templating stuff */
        $smarty= get_smarty();
        $display= "";

        $table = "";
        foreach($this->Devices as $key => $dev){
            $table .= $this->generateRecordConfigurationRow($key);
        }

        $smarty->assign("acl",$this->acl);
        $smarty->assign("disableDialog",$this->disableDialog);
        $smarty->assign("table",$table);;
        $display.= $smarty->fetch(get_template_path('servDNSeditZoneEntries.tpl', TRUE, dirname(__FILE__)));
        return($display);
    }


    function save_object()
    {
        /* Check posts for operations ...  
         */
        $once = true;
        $ptr_updates = array();
        if(!preg_match("/w/",$this->acl)) return;

        foreach($_POST as $name => $value){

            /* Extract informations out of post name 
             */
            $tmp    = preg_replace("/^[^_]*_/","\\1",$name);
            $tmp2   = explode("|",postDecode($tmp));      

            /* Add new host entry
             */
            if((preg_match("/^UserRecord_?/",$name)) && ($once)){
                $once = false;
                $entry = DNS::getDNSHostEntries($this->config,"",true);     
                $entry['exists']    = true;
                $entry['zoneName']  = strtoupper($this->attrs['cn'][0])."/".$this->zoneName; 
                $entry['RECORDS'][] = array("type" => "aRecord" , "value"=>"");
                $this->Devices[_("New entry")] = $entry;
            }

            if(count($tmp2) != 2) continue;

            $Name     = $tmp2[0];
            $RecordID = $tmp2[1];

            /* Add new REcord
             */
            if((preg_match("/^AddRecord_/",$name)) && ($once)){
                $once = false;
                $this->Devices[$Name]['RECORDS'][] = $this->Devices[$Name]['RECORDS'][$RecordID];
            }

            /* Remove record from given dn
             */
            if((preg_match("/^RemoveRecord_/",$name)) && ($once)){
                $once   = false;
                if(isset($this->Devices[$Name]['RECORDS'][$RecordID])){
                    unset($this->Devices[$Name]['RECORDS'][$RecordID]);
                }

                /* Check if there is at least one visible record. Else remove complete entry */
                $visible = false;
                foreach($this->Devices[$Name]['RECORDS'] as $rec){
                    if(in_array_strict($rec['type'],$this->RecordTypes)){
                        $visible = true;  
                        break;
                    }
                }
                if(!$visible && isset($this->Devices[$Name]['RECORDS'])){
                    $this->Devices[$Name]['RECORDS'] = array();
                }
            }
        }

        /* Possible attributes posted 
         */
        foreach($_POST as $name => $value){

            $value = get_post($name);

            /* Extract informations out of post name 
             */
            $tmp    = preg_replace("/^[^_]*_/","\\1",$name);
            $tmp2   = explode("|",postDecode($tmp));      

            if(count($tmp2) != 2) continue;

            $Name     = $tmp2[0];
            $RecordID = $tmp2[1];

            /* Check for value change 
             */          
            if(preg_match("/ValueSelection_/",$name)){
                if(isset($this->Devices[$Name]['RECORDS'][$RecordID])){

                    /* Update value */ 
                    $old = $this->Devices[$Name]['RECORDS'][$RecordID]['value'];
                    $this->Devices[$Name]['RECORDS'][$RecordID]['value'] = get_post($name);

                    /* Handle pTRRecord */
                    if(!isset($ptr_updates[$Name]) && $this->Devices[$Name]['RECORDS'][$RecordID]['type'] == "aRecord"){

                        $found = false;
                        $ip = $value;
                        $match = preg_replace("/^[^\/]*+\//","",$this->reverseName);
                        $ip = preg_replace("/^".preg_quote($match)."/","",$ip);
                        $ip = preg_replace("/^\./","",$ip);

                        foreach($this->Devices[$Name]['RECORDS'] as $key => $dev){
                            if($dev['type'] == "pTRRecord"){
                                $ptr_updates[$Name] = $Name;
                                $this->Devices[$Name]['RECORDS'][$key]['value'] = $ip;
                                $found = true;
                                break;
                            }
                        }
                        if(!$found){
                            $dev = array('type'=> 'pTRRecord', 'value' => $ip);
                            $this->Devices[$Name]['RECORDS'][] = $dev;
                        }
                    }
                }
            }

            /* record type changed
             */        
            if(preg_match("/^RecordTypeSelection_/",$name)){
                if(isset($this->Devices[$Name]['RECORDS'][$RecordID])){
                    $this->Devices[$Name]['RECORDS'][$RecordID]['type'] = $value;
                }  
            }   
        }

        /* check for renamed entries 
         */ 
        foreach($_POST as $name => $value){

        
            $value = get_post($name);

            /* Extract informations out of post name 
             */
            $tmp    = preg_replace("/^[^_]*_/","\\1",$name);
            $tmp2   = explode("|",postDecode($tmp));      

            if(count($tmp2) != 2) continue;

            $Name     = $tmp2[0];
            $RecordID = $tmp2[1];

            /* Host renamed
             */
            if(preg_match("/RenameHost_/",$name)){
                if((isset($this->Devices[$Name])) && ($Name != $value)){

                    if(isset($this->Devices[$value])){
                        msg_dialog::display(_("Error"), sprintf(_("Cannot rename '%s' to '%s'. Name is already in use!"), $Name, $value), ERROR_DIALOG);
                    }else{
                        $this->Devices[$value] = $this->Devices[$Name];
                        unset($this->Devices[$Name]);
                    }
                }
            }
        }
    }


    /*  check something 
     */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        if(!preg_match("/w/",$this->acl)) return($message);

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);

        $names = array();

        foreach($this->Devices as $DevName => $device){

            /* Don't need to check empty values ... */
            if(!count($device['RECORDS'])) continue;

            /* Checking entry name
             */
            if(!preg_match("/^[a-z0-9_\.-]+$/i", $DevName) || (empty($DevName))){
                $message[] = msgPool::invalid(_("Name"),$DevName,"/[a-z0-9_\.-]/i");
            }      

            /* Renaming check for existing devices 
             */
            if(isset($device['OrigCn'])  && ($DevName != $device['OrigCn'] )){
                $ldap->cd($this->config->current['BASE']);
                $ldap->search("(relativeDomainName=".$DevName.")",array("relativeDomainName"));
                if($ldap->count()){
                    $message[] = sprintf(_("Cannot rename '%s' to '%s'. Entry is already in use."),$device['OrigCn'],$DevName);
                }
            }elseif(!isset($device['OrigCn'])){
                $ldap->cd($this->config->current['BASE']);
                $ldap->search("(relativeDomainName=".$DevName.")",array("relativeDomainName"));
                if($ldap->count()){
                    $message[] = sprintf(_("Cannot create '%s'. Entry is already in use."),$DevName);
                }
            }

            /* Check names 
             */
            if(!isset($names[$DevName])){
                $names[$DevName] = "";
            }else{
                $message[] = sprintf(_("Entry '%s' is used more than once."),$DevName);
            }

            /* Names should be written in lowercase
             */
#      if(strtolower($DevName) != $DevName){
#        $message[] = sprintf(_("The host name '%s' should be written in lowercase."), $DevName);
#      }

            /* Check records
             */                 
            $singleEntries = array("pTRRecord");

            $tmp  = array();
            $tmp2 = array();
            foreach($device['RECORDS'] as $Num => $Rec){

                /* Check values */
                $message += $this->checkRecordType($DevName, $Rec['type'], $Rec['value']);

                /* Check for multiple use of unique record types
                 */
                if(in_array_strict($Rec['type'],$singleEntries)){
                    if(!isset($tmp[$Rec['type']])){
                        $tmp[$Rec['type']] = "";
                    }else{
                        $message[] = sprintf(_("%s records cannot be used more than once."),$Rec['type']);
                    }
                }

                /* Check for empty / duplicate entries in record array 
                 */
                if(empty($Rec['value'])){
                    $message[] = sprintf(_("Please fix the empty %s record for entry '%s'."),$Rec['type'],$DevName);
                }

                /* Check for duplicate record entries 
                 */
                if(!isset($tmp[$Rec['type']][$Rec['value']])){
                    $tmp[$Rec['type']][$Rec['value']] = "";
                }else{
                    $message[] = sprintf(_("Please fix the duplicate %s record for entry '%s'."),$Rec['type'],$DevName); 
                }
            }
        }
        return ($message);
    }


    function save()
    {
        if($this->disableDialog || !preg_match("/w/",$this->acl)) return;

        $todo = array(); 



        /* Create todolist
         */
        foreach($this->Devices as $name => $dev){
            if(isset($dev['OrigCn'])){
                if(count($dev['RECORDS'])){
                    $todo[] = DNS::getDNSHostEntriesDiff($this->config,$dev['OrigCn'],$dev,$name);
                }else{
                    $dev['exists'] = false;
                    $todo[] = DNS::getDNSHostEntriesDiff($this->config,$dev['OrigCn'],$dev,$name);
                }
            }else{
                if(count($dev['RECORDS'])){
                    $todo[] = DNS::getDNSHostEntriesDiff($this->config,"",$dev,$name);
                }else{
                    $dev['exists'] = false;
                    $todo[] = DNS::getDNSHostEntriesDiff($this->config,"",$dev,$name);
                }
            }
        }

        $tmp = array();
        $tmp['del']   = array();
        $tmp['add']   = array();
        $tmp['move']  = array();
        foreach($todo as $to){
            foreach($to as $type => $entries){
                $tmp[$type] = array_merge($tmp[$type],$entries);
            }
        }

        /* Get ldap link
         */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd ($this->config->current['BASE']);

        /* move follwoing entries
         */
        foreach($tmp['move'] as $src => $dst){
            $this->recursive_move($src,$dst);
        }

        /* Delete dns */
        foreach($tmp['del'] as $dn => $del){
            $ldap->cd($dn);
            $ldap->rmdir_recursive($dn);
            if(is_object($this->parent->parent)){
                $this->parent->parent->handle_post_events("remove",array("dn" => $dn));
            }
        }

        /* Add || Update new DNS entries
         */
        foreach($tmp['add'] as $dn => $attrs){
            $ldap->cd($dn);
            $ldap->cat($dn, array('dn'));
            if(count($ldap->fetch())){
                $ldap->cd($dn);
                $ldap->modify ($attrs);
                if(is_object($this->parent->parent)){
                    $this->parent->parent->handle_post_events("modify",array("dn" => $dn));
                }
            }else{
                $ldap->cd($dn);
                $ldap->add($attrs);
                if(is_object($this->parent->parent)){
                    $this->parent->parent->handle_post_events("add",array("dn" => $dn));
                }
            }
        }
    }


    /* Create html table out of given entry 
     */
    function generateRecordConfigurationRow($objKey){

        /* Get some basic informations 
         */
        $obj        = $this->Devices[$objKey];
        $objectName = $objKey;

        $dis = "";
        if(!preg_match("/w/",$this->acl)){
            $dis = " disabled ";
        }

        /* Abort if emtpy
         */
        if(count($obj['RECORDS']) == 0) return "";

        /* Set title 
         */
        $str= "<br>";

        $hostNameOnce = true;

        /* Walk through all defined records 
         */
        $str.= "<table summary=''>";
        foreach($obj['RECORDS'] as $id => $record){

            /* Skip not selectable entries */
            if(!isset($this->RecordTypes [$record['type']])) {
                continue;
            }

            /* Create unique post name
             */
            $name = postEncode($objKey."|".$id);

            /* Only first host entry name should be editable
             */
            if($hostNameOnce){
                $hostNameOnce = false;  
                $field1 ="<input $dis type='text' style='width:250px;' name='RenameHost_".$name."' value='".set_post($objectName)."'>\n";
            }else{
                $field1 = "";
            }
            $field2 = $this->createRecordTypeSelection($record['type'],$name);
            $field3 = "<input type='text' $dis value='".set_post($record['value'])."' name='ValueSelection_".$name."' style='width:250px;'>";
            if(preg_match("/w/",$this->acl)){
                $acl = image('images/lists/element.png[new]',"AddRecord_".$name,_("Add"));
                $acl.= image('images/lists/trash.png',"RemoveRecord_".$name,_("Remove"));
            }

            /* Display settings backwards for CNAMERecords 
             */
            if($record['type'] == "cNAMERecord"){
                $str .= "
                    <tr>
                    <td style='width:250px;text-align:right;'>".$field3."</td>
                    <td style='width:90px;'>".$field2."</td>
                    <td>".$objectName."</td>
                    <td>".$acl."</td>
                    </tr>";
            }else{ 
                $str .= "
                    <tr>
                    <td style='width:75px;text-align:right;'>".$field1."</td>
                    <td style='width:90px;'>".$field2."</td>
                    <td>".$field3."</td>
                    <td>".$acl."</td>
                    </tr>";
            }
        }
        $str .="</table>";
        return($str); 
    }


    /* Create selectbox with all available option types 
     */
    function createRecordTypeSelection($id,$refID){

        if(preg_match("/w/",$this->acl)){
            $str = "\n<select name='RecordTypeSelection_".$refID."' onChange='document.mainform.submit();'>";
            foreach($this->RecordTypes as $type => $atr) {
                if($id == $type){
                    $str .="\n<option value='".$type."' selected >".strtoupper(preg_replace("/record/i","",$type))."</option>";
                }else{
                    $str .="\n<option value='".$type."'>".strtoupper(preg_replace("/record/i","",$type))."</option>";
                }
            }
            $str.= "\n</select>";
        }else{
            $str = "&nbsp;".strtoupper(preg_replace("/record/i","",$id));
        }
        return($str);
    }


    /* Check record types for strange inputs */
    function checkRecordType($name, $type, $value)
    {
        $template = _("The syntax of entry %s (record type %s, value %s) is invalid!")." %s<br><br><i>"._("Example").":</i> %s";

        $message = Array();
        switch($type) {
            case 'aAAARecord': // RFC 3596
                if(!tests::is_dns_name($name)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Entry should be a DNS name."), "example");
                }
                if(!tests::is_ipv6($value)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be an IPv6 address."), "1fff:0000:0a88:85a3:0000:0000:ac1f:8001");
                }
                break;
            case 'aRecord': // RFC 1035
                if(!tests::is_dns_name($name)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Entry should be a DNS name."), "example");
                }
                if(!tests::is_ip($value)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be an IPv4 address."), "192.168.1.10");
                }
                break;
            case 'cNAMERecord': // RFC 1035
                if(!tests::is_dns_name($name)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Entry should be a DNS name."), "example");
                }
                if(!tests::is_dns_name($value)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be a DNS name."), "example");
                }
                break;
            case 'mXRecord': // RFC 1035
                //value: preference target
                if(preg_match('/^(\S+)\s+(\S+)$/', $value, $matches)) {
                    if(!tests::is_id($matches[1])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 1 should be a number."), "10 example");
                    }
                    if(!tests::is_dns_name($matches[2])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 2 should be a DNS name."), "10 example");
                    }
                } else {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be composed of 'preference target'."), "10 example");
                }
                break;
            case 'nSRecord': // RFC 1035
                if(!tests::is_dns_name($value)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be a DNS name."), "example");
                }
                break;
            case 'pTRRecord': // RFC 1035
                if(!tests::is_dns_name($value)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be a DNS name."), "example");
                }
                break;
            case 'sRVRecord': // RFC 2782
                if(!tests::is_dns_name($name)) {
                    $message[] = sprintf($template, $name, $type, $value, _("Entry should be a DNS name."), "example");
                }
                //value: priority weight port target
                if(preg_match('/^([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+(\S+)$/', $value, $matches)) {
                    if(!tests::is_id($matches[1])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 1 (priority) should be a number."), "0 5 5060 example");
                    }
                    if(!tests::is_id($matches[2])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 2 (weight) should be a number."), "0 5 5060 example");
                    }
                    if(!tests::is_id($matches[3])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 3 (port) should be a number."), "0 5 5060 example");
                    }
                    if(!tests::is_dns_name($matches[4])) {
                        $message[] = sprintf($template, $name, $type, $value, _("Value 4 (target) should be a DNS name."), "0 5 5060 example");
                    }
                } else {
                    $message[] = sprintf($template, $name, $type, $value, _("Value should be composed of 'priority weight port target'."), "0 5 5060 example");
                }
                break;
        }
        return $message;
    }


    function remove_from_parent()
    {
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
