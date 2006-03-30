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


function getDnsRecordTypes()
{
  global $RecordTypes;
  return($RecordTypes);
}


/* This function returns (if exists) the dns-host eintries for given 
 *  name.
 */
function getDNSHostEntries($config,$name)
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
  $zones = getAvailableZones($config);
  foreach($zones as $zone){
    $zoneDn = getDNSZoneDN($config,$zone);
    $ldap->ls("(&(objectClass=dNSZone)(zoneName=*)(relativeDomainName=".$name.")(!(relativeDomainName=@)))", $zoneDn,$attrs = array("*"));
    while($attrs = $ldap->fetch()){
      $foundIn [] = $attrs['dn'];
    }
  }

  /* Alert if there is more than one zone with an entry named like $name
   */ 
  if(count($foundIn) > 1) {
    print_red(sprintf(_("Found more than one dns zone which contains an entry named '%s'. Aborting getting dns informations for this device."),$name));
  }

  /* No zone found which contains an entry for us
   */
  if(count($foundIn) == 0){
    return($ret);
  }

  /* Get host informations from zone
   */ 
  $ldap->cd($foundIn[0]);
  $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(!(relativeDomainName=@)))",array("*"));
  while($attrs = $ldap->fetch()){

    /* If relative domainname == cn
     * Try to read dnsclass / TTl / zone
     */
    if($attrs['relativeDomainName'][0] == $name){
      $ret['exists'] = true;
      foreach(array("dNSClass","zoneName","dNSTTL") as $atr){
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


/* This function compares two dns settings and returns an 
 *  array with following indexes 
 *   - delete, for dns which must be deleted (only if dns account is removed)
 *   - rename, if a dn must be renamed, for example, the zoneName has changed
 *   - add,    if there is a new dns account created    
 */
function getDNSHostEntriesDiff($config,$oldName,$newEntry,$newName)
{
  global $RecordTypes;

  $oldEntry = getDNSHostEntries($config,$oldName);

  $add    = array();
  $del    = array();
  $move   = array();

  $zones              = getAvailableZones($config);

  $specialAttributes  = array("cNAMERecord","pTRRecord");

  $newRecords         = array();  // Used to remember which records are removed 

  $zoneName     = $newEntry['zoneName'];
  $tmp          = array_flip($zones);
  $reverseName  = $tmp[$zoneName];

  $zoneDn       = getDNSZoneDN($config,$zoneName);
  $reverseDn    = getDNSZoneDN($config,$reverseName);


  /* If account was edited prepare some
   *  attributes & arrays ... if required add some 
   *  dns to $move 
   */
  if(($oldEntry['exists'])){

    /* Check if the account was removed 
     */
    if($newEntry['exists'] == false){
      $dn = "relativeDomainName=".$oldName.",".getDNSZoneDN($config,$oldEntry['zoneName']);
      $del[$dn] ="";
      return(array("move"=>$move,"add"=>$add,"del"=>$del));
    }

    /* Check if zoneName has changed 
     */   
    if($oldEntry['zoneName'] != $newEntry['zoneName']){
      $oldzoneDn = getDNSZoneDN($config,$oldEntry['zoneName']);
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
    
    /* Prepare record entries 
    */
    foreach($oldEntry['RECORDS'] as $id => $rec){
      $newRecords[$rec['type']] = array();
    }
  }

  /* There must be at least one record in our entry  
   */
  if(!count($newEntry['RECORDS'])){
    $dn = "relativeDomainName=".$oldName.",".getDNSZoneDN($config,$oldEntry['zoneName']);
    $del[$dn] ="";
    return(array("move"=>$move,"add"=>$add,"del"=>$del));
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
      $PTRdn= "relativeDomainName=".$rec['value'].",".$baseDn;
      $ptrObj = $baseObj;
      $ptrObj['zoneName']           = $reverseName;
      $ptrObj['pTRRecord']          = $newName.".".$zoneName;
      $ptrObj['relativeDomainName'] = $rec['value'];
      
      $add[$PTRdn] = $ptrObj;
    }else  
    if($rec['type']  == "cNAMERecord"){
      $PTRdn= "relativeDomainName=".$rec['value'].",".$baseDn;
      $ptrObj = $baseObj;
      $ptrObj['zoneName']           = $reverseName;
      $ptrObj['cNAMERecord']        = $newName.".".$zoneName;
      $ptrObj['relativeDomainName'] = $rec['value'];
      
      $add[$PTRdn] = $ptrObj;
    }else{
      /* Append basic attributes
       */
      $add[$stdDn][$rec['type']][] = $rec['value'];  
    }
  } // foreach record 

  return(array("move"=>$move,"add"=>$add,"del"=>$del));
} 


/* returns the dn for a specified zone
 */
function getDNSZoneDN($config,$zoneName){
  $ret = "";
  $ldap = $config->get_ldap_link();
  $ldap-> cd($config->current['BASE']);
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
function getAvailableZones($config)
{
  $ret = array();
  $ldap = $config->get_ldap_link();
  $ldap->cd ($config->current['BASE']);

  /* Search for zones ...
   */
  $ldap->search("(&(objectClass=dNSZone)(sOARecord=*))",array("zoneName","tXTRecord"));

  while($at = $ldap->fetch()){
    if(preg_match("/\.in\-addr\.arpa/",$at['zoneName'][0])){
      $name = preg_replace("/^zoneName=/","",$at['tXTRecord'][0]);
      $ret[$name]['addr']= $at['zoneName'][0];
    }else{
      $name = $at['zoneName'][0];
      $ret[$name]['name']= $at['zoneName'][0];
    }
  }

  $tmp  =array();
  foreach($ret as $name => $entry){
    if((isset($entry['addr']))&&(isset($entry['name']))){
      $tmp[$entry['addr']]=$entry['name'];
    }
  }
  return($tmp);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
