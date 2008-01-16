<?php


/* All available record types 
 */
$RecordTypes['aRecord']       = "aRecord";           
$RecordTypes['mDRecord']      = "mDRecord";         
$RecordTypes['mXRecord']      = "mXRecord";         
$RecordTypes['nSRecord']      = "nSRecord";          
$RecordTypes['pTRRecord']     = "relativeDomainName";
$RecordTypes['hInfoRecord']   = "hInfoRecord";      
$RecordTypes['mInfoRecord']   = "mInfoRecord";       
$RecordTypes['cNAMERecord']   = "relativeDomainName";
$RecordTypes['tXTRecord']     = "tXTRecord";         
$RecordTypes['aFSDBRecord']   = "aFSDBRecord";       
$RecordTypes['SigRecord']     = "SigRecord";         
$RecordTypes['KeyRecord']     = "KeyRecord";         
$RecordTypes['aAAARecord']    = "aAAARecord";        
$RecordTypes['LocRecord']     = "LocRecord";         
$RecordTypes['nXTRecord']     = "nXTRecord";        
$RecordTypes['sRVRecord']     = "sRVRecord";         
$RecordTypes['nAPTRRecord']   = "nAPTRRecord";       
$RecordTypes['kXRecord']      = "kXRecord";          
$RecordTypes['certRecord']    = "certRecord";        
$RecordTypes['a6Record']      = "a6Record";          
$RecordTypes['dSRecord']      = "dSRecord";          
$RecordTypes['sSHFPRecord']   = "sSHFPRecord";       
$RecordTypes['rRSIGRecord']   = "rRSIGRecord";      
$RecordTypes['nSECRecord']    = "nSECRecord";       

class DNS
{

  /* Return all record types 
   */
  static function getDnsRecordTypes($ForZones = false)
  {
    global $RecordTypes;
    if($ForZones){
      $tmp = $RecordTypes;
      unset($tmp['cNAMERecord']);
      unset($tmp['pTRRecord']);
      unset($tmp['tXTRecord']);
      return($tmp);
    }else{
      return($RecordTypes);
    }
  }


  /* This fucntion is used to flip the ip address, for example
     12.3.45  ->  45.3.12
     Because some entries (like zones) are store like that 45.3.12.in-addr.arpa
     but we want to display 12.3.45.
   */
  static function FlipIp($ip)
  {
    $tmp = array_reverse(split("\.",$ip));
    $new = "";
    foreach($tmp as $section){
      $new .= $section.".";
    }
    return(preg_replace("/.$/","",$new));
  }


  /* This static function returns the zones specified for given host
   */
  static function getDNSZoneEntries($config,$HostDn,$silent = false)
  {
    global $RecordTypes;

    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']); 

    /* Not all records are allowed within a zone entry
     */  
    $SkipRecords = array("tXTRecord","cNAMERecord","pTRRecord");

    /* Special sOArecords 
     */
    $sOAREcords  = array("0"=>"sOAprimary","1"=>"sOAmail","2"=>"sOAserial","3"=>"sOArefresh","4"=>"sOAretry","5"=>"sOAexpire","6"=>"sOAttl");

    /* Create tempalte for all fetched zone Data 
     */
    $ZoneBase = array();
    $ZoneBase['exists']  = false;
    $ZoneBase['RECORDS'] = array();
    $ZoneBase['zoneName'] = array();
    $ZoneBase['dNSClass'] = array();

    foreach($sOAREcords as $attr){
      $ZoneBase[$attr] = "";
    }

    $Zones    = array();

    /* Get & Parse all zone entries 
     */
    $ldap->ls("(&(objectClass=dNSZone)(zoneName=*)(relativeDomainName=@))",$HostDn,array("*"));
    $tmp_res = array();
    while($attrs = $ldap->fetch()) {
      $tmp_res[] = $attrs;
    }

    /* Parse fetched zones 
     */
    foreach($tmp_res as $attrs){

      $zoneName                   = $attrs['zoneName'][0];
      $Zones[$zoneName]           = $ZoneBase;
      $Zones[$zoneName]['exists'] = true;

      /* Set basic attributes 
       */
      foreach(array("zoneName","dNSClass") as $attr){
        if(isset($attrs[$attr][0])){
          $Zones[$zoneName][$attr] = $attrs[$attr][0];
        }
      }

      /* Set initial zone name, to be able to detect if this entry was renamed 
       */
      $Zones[$zoneName]['InitialzoneName'] = $zoneName;

      /* Generate SOA entry
       */
      if(isset($attrs['sOARecord'][0])){
        $tmp = split("\ ",$attrs['sOARecord'][0]) ;
        $tmp2 = array();

        /* Assign soa vars */
        foreach($sOAREcords as $key => $name){
          if(isset($tmp[$key])){
            $Zones[$zoneName][$name] = $tmp[$key];
          }else{
            $Zones[$zoneName][$name] = "";
          }
        }
      } // ENDE SOA Record 

      /* Get record attributes 
       */
      foreach($RecordTypes as $name => $value){

        /* Skip some attributes 
         */
        if(in_array($name,$SkipRecords)) continue;

        /* If there is a record attribute
         */
        if(isset($attrs[$name])){

          /* get all entries
           */
          for($i = 0 ; $i < $attrs[$value]['count']; $i ++){
            $Zones[$zoneName]['RECORDS'][] =  array("type"=>$name,"value"=>$attrs[$value][$i]);
          }
        }
      }

      /* Get reverse record ..
       */
      $ldap->ls("(&(objectClass=dNSZone)(relativeDomainName=@)(zoneName=*))",$attrs['dn'],array("zoneName"));

      if($ldap->count() == 0){
        if(!$silent){
          msg_dialog::display(_("Error"), sprintf(_("Cannot find reverse zone for DNS zone '%s'. Parsing zone aborted."),$zoneName), ERROR_DIALOG);
        }
        unset($Zones[$zoneName]);
      }elseif($ldap->count()>1){
        if(!$silent){
          msg_dialog::display(_("Error"), sprintf(_("Found more than one reverse zone for '%s'. Parsing zone aborted."),$zoneName), ERROR_DIALOG);
        }
        unset($Zones[$zoneName]);
      }else{
        $tmp = $ldap->fetch();
        $Zones[$zoneName]['ReverseZone']        = FlipIp(str_replace(".in-addr.arpa","",$tmp['zoneName'][0]));
        $Zones[$zoneName]['InitialReverseZone'] = FlipIp(str_replace(".in-addr.arpa","",$tmp['zoneName'][0]));
      }
    }
    return($Zones);
  }


  /* This static function compares two dns zone objects and returns an 
   *  array with following indexes 
   *   - delete, for dns which must be deleted (only if dns zone is removed)
   *   - rename, if a dn must be renamed, for example, the zoneName has changed
   *   - add,    if there is a new dns account created    
   */
  static function getDNSZoneEntriesDiff($config,$newZones,$HostDn)
  {
    $oldZones = getDNSZoneEntries($config,$HostDn,true);

    $sOAattributes =  array("sOAprimary","sOAmail","sOAserial","sOArefresh","sOAretry","sOAexpire","sOAttl");

    $move   = array();
    $add    = array();
    $del    = array();

    /* Generate a template for zones with default values
     */
    $zoneBase                       = array();
    $zoneBase['objectClass']        = array("top","dNSZone");
    $zoneBase['zoneName']           = "";
    $zoneBase['relativeDomainName'] = "@";
    $zoneBase['dNSClass']           = "IN";
    $zoneBase['sOARecord']          = "";

    /* Contains all renamed zoneNames 
     * For zone entry udpdates
     */
    $PrePareZoneEntries = array();

    /* Walk through all zones and detect renamed/added/deleted zones ... 
     */
    foreach($newZones as $name => $zone){

      /* This zone was renamed 
       */
      if((!empty($zone['InitialzoneName'])) && ($zone['InitialzoneName'] != $zone['zoneName'])){

        /* Move old zone to new position 
         */ 
        $oldDn = "zoneName=".$zone['InitialzoneName'].",".$HostDn;
        $newDn = "zoneName=".$zone['zoneName'].",".$HostDn;
        $PrePareZoneEntries[$zone['InitialzoneName']] = $zone['zoneName'];
        $move [$oldDn] = $newDn;      
      }

      /* Get old zone if available
       */
      $oldZone=array();
      if(!empty($oldZones[$zone['InitialzoneName']])){
        $oldZone = $oldZones[$zone['InitialzoneName']];
      }

      /* Create forward zone entry and put it in our add queue
       */
      $newDn  = "zoneName=".$zone['zoneName'].",".$HostDn;
      $obj    =  $zoneBase;
      $obj['zoneName'] = $zone['zoneName'];

      /* Create sOARecord & add it to the obj
       */ 
      $soa = "";
      foreach($sOAattributes as $attr){
        $soa.=" ".$zone[$attr];
      }  
      $obj['sOARecord'] = trim($soa);
      $obj['nSRecord'] = $zone['sOAprimary'];

      /* If reverse zone was renamed, move entry 
       */
      if(!empty($zone['InitialReverseZone'])){
        if($zone['InitialReverseZone'] != $zone['ReverseZone']){
          $base = "zoneName=".$zone['zoneName'].",".$HostDn;
          $oldRDn = "zoneName=". FlipIp($zone['InitialReverseZone']).".in-addr.arpa,".$base; 
          $newRDn = "zoneName=". FlipIp($zone['ReverseZone']).".in-addr.arpa,".$base;
          $PrePareZoneEntries[FlipIp($zone['InitialReverseZone']).".in-addr.arpa"] = FlipIp($zone['ReverseZone']).".in-addr.arpa";
          $move [$oldRDn] = $newRDn;
        }
      }

      /* Append record entries 
       *  Set old value to array, to ensure that 
       *  they will be deleted if necessary
       */
      if(isset($oldZone['RECORDS'])){
        foreach($oldZone['RECORDS'] as $rec){
          $obj[$rec['type']] = array();
        }
      }

      /* Add new Records 
       */
      foreach($zone['RECORDS'] as $rec){
        if(!isset($obj[$rec['type']])||!is_array($obj[$rec['type']])){
          $obj[$rec['type']] = array();
        }
        $obj[$rec['type']][] = $rec['value'];
      }

      /* Append udpated Zone Forward Entry to our add queue
       */    
      $add[$newDn] = $obj;   

      /* Create Reverse Entry 
       * And append it to our add queue
       */
      $zone['ReverseZone'] = FlipIp($zone['ReverseZone']).".in-addr.arpa";
      $base = "zoneName=".$zone['zoneName'].",".$HostDn;
      $newRDn = "zoneName=".$zone['ReverseZone'].",".$base;
      $rObj = $obj;
      $rObj['zoneName']= $zone['ReverseZone'];
      $add[$newRDn] = $rObj;

      /* Remove currently managed zone from oldZones.
       *  this gives us the ability to detect removed zones
       */
      if(isset($oldZones[$zone['InitialzoneName']])){
        unset($oldZones[$zone['InitialzoneName']]); 
      }
    }

    /* The rest of our oldZones must be deleted
     *  because they are no longer available in newZones anymore.
     */
    foreach($oldZones as $zone)  {
      $oldDn = "zoneName=".$zone['InitialzoneName'].",".$HostDn;
      $del[$oldDn] = $zone;
    }

    /* Check for entries which must be updated 
     */
    $zoneUpdates = array();
    $udpate = array();
    if(count($PrePareZoneEntries)){
      $ldap = $config->get_ldap_link();
      foreach($PrePareZoneEntries as $FromZoneName => $ToZoneName){
        $ldap->cd($HostDn);
        $ldap->search("(&(objectClass=dNSZone)(zoneName=".$FromZoneName.")(!(relativeDomainName=@)))",array("zoneName"));
        while($attrs = $ldap->fetch()){
          $zoneUpdates[$attrs['dn']] = array("zoneName"=>$ToZoneName);
        }
      }
    }

    $ret = array("del" => $del , "move" => $move , "add" => $add,"zoneUpdates"=>$zoneUpdates);
    return($ret);
  }


  /* This static function returns the dns-host eintries for given 
   *  name.
   */
  static function getDNSHostEntries($config,$name,$silent = false)
  {
    global $RecordTypes;

    $types = array();
    $ret = array();
    $ret['RECORDS']   = array();
    $ret['dNSClass']  = "IN";
    $ret['zoneName']  = "";
    $ret['dNSTTL']    = "7440";
    $ret['exists']    = false;

    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']);

    /* First check all zones for an entry with the given name.
     * If the name occurs in more than one entry alert the user ...
     */
    $foundIn = array();
    $zones = DNS::getAvailableZones($config);

    $zonesArr = array();
    foreach($zones as $zoneMix){
      $zoneIndex = split("/",$zoneMix);
      if(!array_key_exists($zoneIndex[0],$zonesArr)) {
        $zonesArr[$zoneIndex[0]] = array();
      }
      array_push($zonesArr[$zoneIndex[0]],$zoneIndex[1]);
    }

    foreach($zonesArr as $nameServer => $nameServerArr){
      $foundInTmp = array();
      foreach($nameServerArr as $zoneArr => $zone){
        $zoneMix = $nameServer."/".$zone;
        $zoneDn = DNS::getDNSZoneDN($config,$zoneMix);
        $ldap->ls("(&(objectClass=dNSZone)(zoneName=*)(relativeDomainName=".$name.")(!(relativeDomainName=@)))", $zoneDn,$attrs = array("*"));
        while($attrs = $ldap->fetch()){
          $foundInTmp [$zoneMix] = $attrs['dn'];
          $foundIn [$zoneMix] = $attrs['dn'];
        }
      }
    }

    /* No zone found which contains an entry for us
     */
    if(count($foundIn) == 0){
      return($ret);
    }

    /* Get host informations from zone
     */ 
    $id_tmp = key($foundIn);
    $ldap->cd($foundIn[$id_tmp]);
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(!(relativeDomainName=@)))",array("*"));
    while($attrs = $ldap->fetch()){

      /* If relative domainname == cn
       * Try to read dnsclass / TTl / zone
       */
      if($attrs['relativeDomainName'][0] == $name){
        $ret['exists'] = true;
        $ret['zoneName'] = $id_tmp;
        foreach(array("dNSClass","dNSTTL") as $atr){
          if(isset($attrs[$atr][0])){
            $ret[$atr] = $attrs[$atr][0];
          }
        }
      }

      /* Create list with all used records */
      foreach($RecordTypes as $name => $value){

        /* If there is a record attribute  */
        if(isset($attrs[$name])){

          /* get all entries */
          for($i = 0 ; $i < $attrs[$value]['count']; $i ++){
            $types[] = array( "type"    => $name,
                "value"   => $attrs[$value][$i]);
          }
        }
      }
      $ret['RECORDS'] = $types;
    }
    return($ret);
  }  



  /* This static function compares two dns settings and returns an 
   *  array with following indexes 
   *   - delete, for dns which must be deleted (only if dns account is removed)
   *   - rename, if a dn must be renamed, for example, the relativeDomainName has changed
   *   - add,    if there is a new dns account created    
   */
  static function getDNSHostEntriesDiff($config,$oldName,$newEntry,$newName)
  {
    global $RecordTypes;

    $oldEntry = getDNSHostEntries($config,$oldName);

    $add    = array();
    $del    = array();
    $move   = array();

    /* Don't go further if there is nothing to do
     * Is no account / was no account
     */
    if(($newEntry['exists'] == false )&& ($oldEntry['exists'] == false)){
      return(array("move"=>$move,"add"=>$add,"del"=>$del));
    }

    $zones              = DNS::getAvailableZones($config);
    $specialAttributes  = array("cNAMERecord","pTRRecord");
    $newRecords         = array();  // Used to remember which records are removed 
    $zoneNameMix           = $newEntry['zoneName'];
    $zoneDn             = DNS::getDNSZoneDN($config,$zoneNameMix);
    $tmp                = array_flip($zones);
    $zoneName 					= DNS::getNameFromMix($zoneNameMix);

    /* If reverseZone can't be resolved ... this 
     *  can't be a valid entry, so remove this account
     */ 
    if(isset($tmp[$zoneNameMix])){
      $reverseNameMix  = $tmp[$zoneNameMix];
      $reverseDn    = DNS::getDNSZoneDN($config,$reverseNameMix);
      if(empty($reverseDn)){
        $newEntry['exists'] = false;
      }
    }else{
      $newEntry['exists'] = false;
    }

    /* If account was edited prepare some
     *  attributes & arrays ... if required add some 
     *  dns to $move 
     */
    if($oldEntry['exists']){

      /* Check if the account was removed 
       */
      if($newEntry['exists'] == false){
        $dn = "relativeDomainName=".$oldName.",".DNS::getDNSZoneDN($config,$oldEntry['zoneName']);
        $del[$dn] ="";
        return(array("move"=>$move,"add"=>$add,"del"=>$del));
      }

      /* Check if zoneName has changed 
       */   
      if(count($newEntry['RECORDS'])){
        if($oldEntry['zoneName'] != $newEntry['zoneName']){
          $oldzoneDn = DNS::getDNSZoneDN($config,$oldEntry['zoneName']);
          $dn = "relativeDomainName=".$oldName.",".$oldzoneDn;
          $dn2= "relativeDomainName=".$oldName.",".$zoneDn;
          $move[$dn]=$dn2;
        }

        /* Check if host name has changed 
         */ 
        if($oldName != $newName){
          $dn = "relativeDomainName=".$oldName.",".$zoneDn;
          $dn2= "relativeDomainName=".$newName.",".$zoneDn;
          $move[$dn]=$dn2;
          $dn = "relativeDomainName=".$oldName.",".$dn2;
          $dn2= "relativeDomainName=".$newName.",".$dn2;
          $move[$dn]=$dn2;
        }
      }

      /* Prepare record entries
       *  Fill old records with array();
       *  To ensure that they will be deleted if they stay unused
       */
      foreach($oldEntry['RECORDS'] as $id => $rec){
        $newRecords[$rec['type']] = array();
      }
    }

    /* There must be at least one record in our entry  
     */
    if((!count($newEntry['RECORDS'])) || (!$newEntry['exists'])){
      $dn = "relativeDomainName=".$newName.",".DNS::getDNSZoneDN($config,$oldEntry['zoneName']);
      $del[$dn] ="";
      $ret = array("move"=>$move,"add"=>$add,"del"=>$del);
      return($ret);
    }

    /* Prepare temp obj
     */
    $baseObj =  array();
    $baseObj['objectClass']       = array("top","dNSZone");
    $baseObj['dNSTTL']            = $newEntry['dNSTTL'];
    $baseObj['dNSClass']          = $newEntry['dNSClass'];
    $baseObj['zoneName']          = $zoneName; 
    $baseObj['relativeDomainName']= $newName; 

    /* Add Container Object to zone
     *  (this possibly already exists, check this before writing to ldap)
     */
    $baseDn =  "relativeDomainName=".$newName.",".$zoneDn;
    $add[$baseDn] = $baseObj;

    /* Add base obejct which contains all std records
     */
    $stdDn = "relativeDomainName=".$newName.",".$baseDn;
    $add[$stdDn] = $baseObj;

    /* Set defaults. Normaly only contains old record names.
     *  The old names will be set to array, to ensure that they will be deleted.
     *  Or overwritten and filled with new values.
     */
    foreach($newRecords as $name => $def){
      if(!in_array($name,$specialAttributes)){
        $add[$stdDn][$name] = $def;
      }
    }

    /* Delete all OLD special attributes.
     */
    foreach($oldEntry['RECORDS'] as $id => $rec){
      if(in_array($rec['type'],$specialAttributes)){
        $deldn= "relativeDomainName=".$rec['value'].",".$baseDn;
        $del[$deldn] = "";
      }
    }


    /* Create new record entries 
     */
    foreach($newEntry['RECORDS'] as $id => $rec){
      /* Create object which contains special records 
       *  like pTRRecord or CNAMERecord
       */
      if($rec['type']  == "pTRRecord"){
        $PTRdn= "relativeDomainName=".DNS::FlipIP($rec['value']).",".$baseDn;
        $ptrObj = $baseObj;
        $reverseName = DNS::getNameFromMix($reverseNameMix);
        $ptrObj['zoneName']           = $reverseName;
        if(!preg_match("/\.$/",$newName)){
          $ptrObj['pTRRecord']          = preg_replace("/\.\.$/",".",$newName.".".$zoneName.".");
        }else{
          $ptrObj['pTRRecord']          = preg_replace("/\.\.$/",".",$newName.".");
        }
        $ptrObj['relativeDomainName'] = DNS::FlipIP($rec['value']);

        $add[$PTRdn] = $ptrObj;
      }else  
        if($rec['type']  == "cNAMERecord"){
          $PTRdn= "relativeDomainName=".$rec['value'].",".$baseDn;
          $ptrObj = $baseObj;
          $ptrObj['zoneName']           = $zoneName;
          $ptrObj['cNAMERecord']        = $newName;
          $ptrObj['relativeDomainName'] = $rec['value'];

          $add[$PTRdn] = $ptrObj;
        }else{
          /* Append basic attributes
           */
          $add[$stdDn][$rec['type']][] = $rec['value'];  
        }
    } // foreach record 

    $ret = array("move"=>$move,"add"=>$add,"del"=>$del);
    return($ret);
  } 

  static function getNameFromMix($zoneMix){
    $ret = "";
    if(!strstr($zoneMix, '/')) return($ret);	
    $zoneIndex 	  	= split("/",$zoneMix);
    return($zoneIndex[1]);
  }

  /* returns the dn for a specified zone
   */
  static function getDNSZoneDN($config,$zoneNameMix)
  {
    $ret = "";
    if(!strstr($zoneNameMix, '/')) {
      msg_dialog::display(_("Error"), sprintf(_("Undefined zone name '%s'!"),$zoneNameMix), ERROR_DIALOG);
      return($ret);
    }

    $zoneNameIndex 	  	= split("/",$zoneNameMix); 
    $zoneName           = $zoneNameIndex[1];
    $nameServer		  		= strtolower($zoneNameIndex[0]);
    $ldap               = $config->get_ldap_link();

    /* search for the nameserver */
    $ldap-> cd($config->current['BASE']);
    $ldap->search("(&(objectClass=goServer)(cn=".$nameServer."))",array("cn"));
    if($ldap->count()){
      $attr = $ldap->fetch();
    } else {
      return($ret);
    }

    $ldap-> cd($attr['dn']);
    $ldap->search("(&(objectClass=dNSZone)(sOARecord=*)(zoneName=".$zoneName."))",array("zoneName"));
    if($ldap->count()){
      $attr = $ldap->fetch();
      return($attr['dn']);
    }

    return($ret);
  }


  /* returns all available zones 
   *  array[reverseName] = zoneName;
   */
  static function getAvailableZones($config)
  {
    $ret = array();
    $ldap = $config->get_ldap_link();
    $ldap->cd ($config->current['BASE']);

    /* Search for zones ...
     */
    $ldap->search("(&(objectClass=dNSZone)(sOARecord=*))",array("zoneName"));

    $ForwardZones = array();
    $ReverseZones = array();
    $zones = array();

    while($at = $ldap->fetch()){
      if(preg_match("/\.in\-addr\.arpa/",$at['zoneName'][0])){
        $ReverseZones[$at['dn']] = $at;
      }else{
        $ForwardZones[$at['dn']] = $at;
      }
    }

    foreach($ForwardZones as $dn => $obj){

      /* try to find reverse
       */  
      foreach($ReverseZones as $Rdn => $Robj ){
        if(preg_match("/".$dn."/",$Rdn)){
          $zones[strtoupper($ldap->getCn($dn))."/".$Robj['zoneName'][0]] =  
            strtoupper($ldap->getCn($dn))."/".$obj['zoneName'][0];
        }
      }   
    }
    return($zones);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
