<?php

class termDNS extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $DNSattributes   = array("dNSClass","zoneName","dNSTTL");
  var $attributes= array("ipHostNumber","macAddress");
  var $objectclasses= array("whatever");

  var $ipHostNumber ="";    // IP address 
  var $macAddress   ="";    // Mac address 
  var $cn           ="";    // CN of currently edited device 

  var $Zones      = array();      // All Available Zones like array("3.2.1"=>"MyServer.de")
  var $RecordTypes= array();      // Possible record types 
    
  var $dNSClass   = "IN";         // dNSClass name 
  var $zoneName   = "";           // Used ZoneName 
  var $dNSTTL     = 7200;         // TTL settings for the created entries 

  /* Used records */
  var $types = array();
  var $DNSinitially_was_account = false;

  var $orig_dn     ="";

  var $IPisMust = false;
  var $MACisMust= false;

  var $found = false;

  function termDNS ($config, $dn,$objectClasses,$IPisMust = false)
  {
    /* We need to know which objectClasses are used, to store the ip/mac*/
    $this->objectclasses= $objectClasses;
    plugin::plugin ($config, $dn);

    $this->orig_dn= $dn;
 
    $this->IPisMust = $IPisMust;

    /* Hide all dns specific code, if dns is not available 
     */
    $found = false;
    foreach($this->config->data['TABS']['SERVTABS'] as $tab){
      if(preg_match("/^servdns$/",$tab['CLASS'])){
        $found = true;
      }
    }
    $this->found = $found;
    if(!$this->found){
      $this->DNS_is_account = false;
      return;
    }

 
    /* All types with required attrs */
    $this->RecordTypes['aRecord']       = "aRecord";           // ok
    $this->RecordTypes['mDRecord']      = "mDRecord";          // ok
    $this->RecordTypes['mXRecord']      = "mXRecord";          // ok
    $this->RecordTypes['nSRecord']      = "nSRecord";          // ok 
    $this->RecordTypes['pTRRecord']     = "relativeDomainName";// ok
    $this->RecordTypes['hInfoRecord']   = "hInfoRecord";       // ok
    $this->RecordTypes['mInfoRecord']   = "mInfoRecord";       // ok
    $this->RecordTypes['cNAMERecord']   = "relativeDomainName";// ok
    $this->RecordTypes['tXTRecord']     = "tXTRecord";         // ok
    $this->RecordTypes['aFSDBRecord']   = "aFSDBRecord";       // ok
    $this->RecordTypes['SigRecord']     = "SigRecord";         // ok
    $this->RecordTypes['KeyRecord']     = "KeyRecord";         // ok 
    $this->RecordTypes['aAAARecord']    = "aAAARecord";        // ok
    $this->RecordTypes['LocRecord']     = "LocRecord";         // ok 
    $this->RecordTypes['nXTRecord']     = "nXTRecord";         // ok
    $this->RecordTypes['sRVRecord']     = "sRVRecord";         // ok
    $this->RecordTypes['nAPTRRecord']   = "nAPTRRecord";       // ok
    $this->RecordTypes['kXRecord']      = "kXRecord";          // ok
    $this->RecordTypes['certRecord']    = "certRecord";        // ok
    $this->RecordTypes['a6Record']      = "a6Record";          // ok
    $this->RecordTypes['dSRecord']      = "dSRecord";          // ok
    $this->RecordTypes['sSHFPRecord']   = "sSHFPRecord";       // ok
    $this->RecordTypes['rRSIGRecord']   = "rRSIGRecord";       // ok
    $this->RecordTypes['nSECRecord']    = "nSECRecord";        // ok 

    /* Get all available zones */
    if(empty($this->cn)&&(isset($this->attrs['cn'][0]))){ 
      $this->cn = $this->attrs['cn'][0];
    }
    $this->Zones = $this->get_Zones();
    $types = array();

    /* Get all records */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn); 
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(!(relativeDomainName=@)))",array("*"));

    while($attrs = $ldap->fetch()){
      /* If relative domainname == cn 
       * Try to read dnsclass / TTl / zone 
       */
      if($attrs['relativeDomainName'][0] == $this->cn){
        /* Get class */
        if(isset($attrs['dNSClass'][0])){
          $this->dNSClass = $attrs['dNSClass'][0];
        }
        /* Get Zone*/
        if(isset($attrs['zoneName'][0])){
          $this->zoneName = $attrs['zoneName'][0];
        }
        /* Get ttl */
        if(isset($attrs['dNSTTL'][0])){
          $this->dNSTTL   = $attrs['dNSTTL'][0];
        }
      }

      /* Create list with all used records */
      foreach($this->RecordTypes as $name => $value){
    
        /* If there is a record attribute  */
        if(isset($attrs[$name])){
  
 
          /* get all entries */
          for($i = 0 ; $i < $attrs[$value]['count']; $i ++){
            if(($value == "aRecord")&&($this->ipHostNumber==$attrs[$value][$i])){
             continue; 
            }
            $types[] =array("type"=>$name,"inittype"=>$name,"value"=>$attrs[$value][$i],"status"=>"edited","dn"=>$attrs['dn']);
          }
        }
      }
    }

    /* If there is at least one entry in this -> types, we have DNS enabled */
    $this->types = $types;
    if(count($this->types) == 0){
      $this->DNS_is_account = false;
    }else{
      $this->DNS_is_account = true;
    }
    
    /* Store initally account settings */
    $this->DNSinitially_was_account = $this->DNS_is_account;
  }

  function execute()
  {
	  /* Call parent execute */
    $smarty= get_smarty();
    $display= "";

    $smarty->assign("staticAddress", ""); 
 
    /* There is no dns available 
     */
    if($this->found == false){
        
      $smarty->assign("DNS_is_account",false);  
      $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));
    
      /* Assign smarty all non DNs attributes */
      foreach($this->attributes as $attr){
        $smarty->assign($attr,$this->$attr);
      }
      $smarty->assign("staticAddress", "");
      $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
      return($display);
    }else{
      $smarty->assign("DNS_is_account",true); 
    }
 
    /* Add new empty array with status new, to our record list */
    if(isset($_POST['AddNewRecord'])){
      $this->types[]  =array("type"=>"aRecord","value"=>"","status"=>"new");
    }
   
    /* Handle all posts */ 
    $only_once =true;
    foreach($_POST as $name => $value){

      /* Check if we have to delete a record entry */
      if((preg_match("/RemoveRecord_/",$name))&&($only_once)) {
      
        /* Avoid performing this once again */
        $only_once = false;

        /* Extract id for specified entry */
        $id = preg_replace("/RemoveRecord_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
    
        /* Delete this record, mark edited entries to be able to delete them */
        if(isset($this->types[$id])){
          if($this->types[$id]['status'] == "edited"){
            $this->types[$id]['status'] = "deleted";
          }else{    
            unset($this->types[$id]);
          }
        }
      }
    }

    /* Assign smarty all non DNs attributes */
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }

    /* Assign smarty all DNS attributes */
    foreach($this->DNSattributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    
    /* Assign all needed vars */
    $smarty->assign("DNSAccount",$this->DNS_is_account);
    $smarty->assign("Zones",$this->Zones);
    $smarty->assign("ZoneKeys",($this->Zones));
    $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));
    $changeStateForRecords ="";
    $smarty->assign("records",$this->generateRecordsList(&$changeStateForRecords));
    $smarty->assign("changeStateForRecords",$changeStateForRecords);
//    $smarty->assign("dNSClasses",array("IN"=>"IN"));
    $smarty->assign("staticAddress","<font class=\"must\">*</font>");
    $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->orig_dn);
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(!(relativeDomainName=@)))",array("relativeDomainName","zoneName"));
    while($attr = $ldap->fetch()){  
      $ldap->cd($attr['dn']);
      $ldap->rmDir($attr['dn']);
    }
  }

  /* Save data to object */
  function save_object()
  {
    /* Save all posted vars */
    plugin::save_object();
    
    /* Ge all non dns attributes (IP/MAC)*/
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }

    /* Get dns attributes */
    if(isset($_POST['network_tpl_posted'])){

      /* Check for posted record changes */
      foreach($this->types as $key => $value){
      
        /* Check if type has changed */
        if(isset($_POST['RecordTypeSelectedFor_'.$key])){
          $this->types[$key]['type'] = $_POST['RecordTypeSelectedFor_'.$key];
        }
        /* Check if value has changed */
        if(isset($_POST['RecordValue_'.$key])){
          $this->types[$key]['value'] = $_POST['RecordValue_'.$key];
        }
      }
      
      /* Get all basic DNS attributes (TTL, Clas ..)*/
      foreach($this->DNSattributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = $_POST[$attr];
        }
      }

      /* Enable diable DNS */
      if(isset($_POST['enableDNS'])){
        $this->DNS_is_account = true;
      }else{
        $this->DNS_is_account = false;
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    
    if(($this->IPisMust)||($this->DNS_is_account)){
      /* Check if ip is empty */
      if ($this->ipHostNumber == "" && chkacl ($this->acl, "ipHostNumber") == ""){
        $message[]= _("The required field 'IP-address' is not set.");
      }

      /* check if given ip is valid ip*/
      $num="(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])";
      if (!preg_match("/^$num\\.$num\\.$num\\.$num$/", $this->ipHostNumber)){
        $message[]= _("Wrong IP format in field IP-address.");
      }
    }

    /* Check if mac is empty */
    if ($this->macAddress == "" && chkacl ($this->acl, "macAddress") == ""){
      $message[]= _("The required field 'MAC-address' is not set.");
    }

    /* Check if given mac is valid mac */
    $tr = count(split(":",$this->macAddress));
    if($tr!=6){
      $message[]=(_("The given macaddress is invalid. There must be 6 1byte segments seperated by ':'."));
    }

    /* only perfrom this checks if this is a valid DNS account */
    if($this->DNS_is_account){
      foreach($this->types as $name => $values){

        /* Check if there is an aRecord defined which uses the same IP as used in IPhostAddress */
        if(($values['type'] == "aRecord")&&($values['value'] == $this->ipHostNumber)&&($values['status']!="deleted")){
          $message[]=sprintf(_("The device IP '%s' is added as 'A Record', this will be done automatically, please remove the record."), 
               $this->ipHostNumber);
        }

        /* only lower-case is allowed in record entries ... */
        if($values['value'] != strtolower($values['value'])){
          $message[] = sprintf(_("Only lowercase is allowed, please check your '%ss'."),$values['type']);
        }
      }
    }
  
    return ($message);
  }


  /* Save to LDAP */
  function save($dn)
  {
    $ldap= $this->config->get_ldap_link();
   
    /*******************/ 
    /* IP-MAC HANDLING */
    /*******************/ 

    /* $dn was posted as parameter */
    $this->dn = $dn;
    
    /* Save DNS setting & ip/Mac*/
    plugin::save();

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
$ldap->modify ($this->attrs); 


    /****************/ 
    /* DNS HANDLING */
    /****************/ 

    /* If isn't DNS account but initially was DNS account 
       remove all DNS entries 
     */ 
    if(!$this->DNS_is_account){
      if($this->DNSinitially_was_account){
        $tmp = array();
        foreach($this->types as $type){
          $dn = $type['dn'];
          if(!isset($tmp[$dn])) {
            $ldap->cd($dn);
            $ldap->rmDir($dn);
          }
          $tmp[$dn]=$dn;
        }
      }
    }else{
    
      /* DNS is enabled, check what we have to do */
      $delete = array();
  
      /* Generate a list of new ldap entries, 
          & $delete contains all dns which should be deleted 
       */
      $entries = $this->generate_LDAP_entries(&$delete);

      /* Delete dns */
      foreach($delete as $dn => $del){
        $ldap->cd($dn);
        $ldap->rmDir($dn);
      }

      /* Add || Update new DNS entries */
      foreach($entries as $dn => $attrs){
        $ldap->cd($dn);
        $ldap->cat($dn);
        
        if(count($ldap->fetch())){
          $ldap->cd($dn);
//          $this->cleanup();
          $ldap->modify ($attrs); 

        }else{
          $ldap->cd($dn);
          $ldap->add($attrs);
        }
      }
    }
    if($ldap->get_error() != "Success"){
      show_ldap_error($ldap->get_error()); 
    }
    
  }

  /*  Create html table with all used record types
   */
  function generateRecordsList($changeStateForRecords)
  {
    $changeStateForRecords = "";
    
    if(!$this->DNS_is_account) {
      $str = "<input type='submit' value='"._("Add")."' name='AddNewRecord' id='AddNewRecord' disabled>";
      return $str;
    }
 
    $str = "<table summary='' width='100%'>";
    foreach($this->types as $key => $entry){
        if($entry['status'] == "deleted") continue;

        $changeStateForRecords.= "changeState('RecordTypeSelectedFor_".$key."');\n";
        $changeStateForRecords.= "changeState('RecordValue_".$key."');\n";
        $changeStateForRecords.= "changeState('RemoveRecord_".$key."');\n";

        $str.=" <tr>".
          "   <td>".$this->generateRecordListBox($entry['type'],"RecordTypeSelectedFor_".$key)."</td>".
          "   <td><input type='text' value='".$entry['value']."' name='RecordValue_".$key."' id='RecordValue_".$key."'></td>".
          "   <td><input type='submit' name='RemoveRecord_".$key."' value='"._("Delete")."' id='RemoveRecord_".$key."'></td>".
          "</tr>";
    }

    $str.= "  <tr>".
           "    <td colspan=2 width='50%'></td><td>".
           "      <input type='submit' value='"._("Add")."' name='AddNewRecord'>".
           "    </td>".
           "  </tr>".
           "</table>";
    return($str);
  }

  /* Create a html select box which allows us to select different types of records */
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

  /* return all Zone names */
  function get_Zones()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap-> cd ($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(sOARecord=*))",array("*"));
    
    while($at = $ldap->fetch()){
      if(preg_match("/\.in\-addr\.arpa/",$at['zoneName'][0])){
        $ret[$at['relativeDomainName'][0]]['addr']= $at['zoneName'][0];
      }else{
        $ret[$at['relativeDomainName'][0]]['name']= $at['zoneName'][0];
      }
    }

    $tmp  =array();
    foreach($ret as $name => $entry){
      if((isset($entry['addr']))&&(isset($entry['name']))){
        $tmp[$entry['addr']]=$entry['name'];
      }
    }
    $ret = $tmp;
    return($ret); 
  } 

  /* this is used to generate ldap friendly output of our 
     dns configuration
  */
  function generate_LDAP_entries($delete)
  {
    $entries = array();    
    $delete  = array();

    /* Generate Main Entry */
    $dn = "relativeDomainName=".$this->cn.",".$this->dn;
    $entries[$dn]['dNSClass']           = $this->dNSClass;      
    $entries[$dn]['zoneName']           = $this->zoneName;      
    $entries[$dn]['dNSTTL']             = $this->dNSTTL;      
    $entries[$dn]['relativeDomainName'] = $this->cn;      

    /* Generate cNAMERecord */
    $aRecords = array();
    foreach($this->types as $type){
      if($type['type'] == "cNAMERecord"){
          
        $Cdn = "relativeDomainName=".$type['value'].",".$this->dn;
        if($type['status']=="deleted"){
          $delete [$type['dn']] = $Cdn;
        }else{
          $entries[$Cdn] = $entries[$dn];
          $entries[$Cdn]['relativeDomainName']  = $type['value'];
          $entries[$Cdn]['cNAMERecord']         = $this->cn.".".$this->zoneName;
        }
      }
    }

    /* Generate tXTRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "tXTRecord")&&($type['status']!="deleted")){
        $entries[$dn]['tXTRecord'][] = $type['value'];
      }
    }

    /* Generate mDRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "mDRecord")&&($type['status']!="deleted")){
        $entries[$dn]['mDRecord'][] = $type['value'];
      }
    }

    /* Generate mXRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "mXRecord")&&($type['status']!="deleted")){
        $entries[$dn]['mXRecord'][] = $type['value'];
      }
    }
 
    /* Generate hInfoRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "hInfoRecord")&&($type['status']!="deleted")){
        $entries[$dn]['hInfoRecord'][] = $type['value'];
      }
    }

    /* Generate mInfoRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "mInfoRecord")&&($type['status']!="deleted")){
        $entries[$dn]['mInfoRecord'][] = $type['value'];
      }
    }
 
    /* Generate aFSDBRecord */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "aFSDBRecord")&&($type['status']!="deleted")){
        $entries[$dn]['aFSDBRecord'][] = $type['value'];
      }
    }
 
    /* Generate some attrs  */
    $arr = array("SigRecord","KeyRecord","aAAARecord","nSRecord",
        "LocRecord","nXTRecord","sRVRecord","nAPTRRecord","kXRecord","certRecord","a6Record","dSRecord","sSHFPRecord","rRSIGRecord","nSECRecord");
    $aRecords = array();
    foreach($arr as $ar){
      foreach($this->types as $type){
        if(($type['type'] == $ar)&&($type['status']!="deleted")){
          $entries[$dn][$ar][] = $type['value'];
        }
      }
    }
 
 
    /* Generate A Records (IP Address relation) */
    $aRecords = array();
    foreach($this->types as $type){
      if(($type['type'] == "aRecord")&&($type['status']!="deleted")){
        $aRecords[] = $type['value'];
      }
    }
    if(count($aRecords)){
      
      /* Add ipHostNumber as default aRecord */
      $aRecords[] = $this->ipHostNumber;

      $dn = "relativeDomainName=".$this->cn.",".$this->dn;
      foreach($aRecords as $rec){
        $entries[$dn]['aRecord'][] = $rec;      
      }
    }

    /* Generate pTRRecord Records */
    foreach($this->types as $type){
      if($type['type'] == "pTRRecord"){
        $PTRdn= "relativeDomainName=".$type['value'].",".$this->dn;
        if($type['status']=="deleted"){        
          $delete [$type['dn']] = $PTRdn;
        }else{
          $zones = array_flip($this->Zones);
          $zone = $zones[$this->zoneName];  
          $entries[$PTRdn]['relativeDomainName'] = $type['value'];
          $entries[$PTRdn]['pTRRecord']         = $this->cn.".".$this->zoneName;
          $entries[$PTRdn]['zoneName']            =  $zone;
        }
      }
    }

    /* add ObjectClasses */
    foreach($entries as $key => $entry ){
      $entries[$key]['objectClass']=array("top","dNSZone");
      $entries[$key] = array_reverse($entries[$key]);
    }

    /* Check if record type has changed, and if we need to delete this record attribute from ldap entry */
    foreach($this->types as $type){
      if(isset($type['inittype'])){
        if(!isset($entries[$dn][$type['inittype']])){
          $entries[$dn][$type['inittype']] = array();
        }
      }
    }

    return($entries); 
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
