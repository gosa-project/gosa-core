<?php


/* This function returns the vhost entries specified for given host
 */
function getVhostEntries($config,$HostDn,$silent = false)
{

  $ldap = $config->get_ldap_link();
  $ldap->cd($config->current['BASE']); 

  /* Get host entry */
  $ldap->cat($HostDn);
  $host_attr = $ldap->fetch();

  /* Create tempalte for all fetched zone Data 
   */
  $VhostBase = array();
  $VhostBase['exists']  = false;
  //$VhostBase['RECORDS'] = array();
  $VhostBase['apacheServerName'] = array();
  $VhostBase['apacheConfig'] = array();
    
  $Vhosts    = array();

  /* Get & Parse all zone entries 
   */
  $ldap->ls("(&(objectClass=apacheConfig)(apacheServerName=*))",$HostDn,array("*"));
  $tmp_res = array();
  while($attrs = $ldap->fetch()) {
    $tmp_res[] = $attrs;
  }

  /* Parse fetched zones 
   */
  foreach($tmp_res as $attrs){

    $apacheServerName                    = $attrs['apacheServerName'][0];
    $apacheDocumentRoot                  = $attrs['apacheDocumentRoot'][0];
    $apacheServerAdmin                   = $attrs['apacheServerAdmin'][0];
    $Vhosts[$apacheServerName]           = $VhostBase;
    $Vhosts[$apacheServerName]['exists'] = true;

    /* Set basic attributes 
     */
    if(isset($attrs["apacheConfig"][0])){
      $Vhosts[$apacheServerName]["apacheConfig"] = $attrs["apacheConfig"][0];
    }

    /* Set initial zone name, to be able to detect if this entry was renamed 
     */
    $Vhosts[$apacheServerName]['InitialApacheServerName'] = $apacheServerName;
    $Vhosts[$apacheServerName]['apacheServerName'] = $apacheServerName;
    $Vhosts[$apacheServerName]['apacheDocumentRoot'] = $apacheDocumentRoot;
    $Vhosts[$apacheServerName]['apacheServerAdmin'] = $apacheServerAdmin;

  
    if (isset($attrs['apacheServerAlias'])){
    	for($i = 0 ; $i < $attrs['apacheServerAlias']['count']; $i ++){
          	$Vhosts[$apacheServerName]['apacheServerAlias'][] =  $attrs['apacheServerAlias'][$i];
        }
    }
    
  	if (isset($attrs['apacheScriptAlias'])){
    	for($i = 0 ; $i < $attrs['apacheScriptAlias']['count']; $i ++){
          	$Vhosts[$apacheServerName]['apacheScriptAlias'][] =  $attrs['apacheScriptAlias'][$i];
        }
    }

  }	
  return($Vhosts);
}


/* This function compares two dns zone objects and returns an 
 *  array with following indexes 
 *   - delete, for vhost entries which must be deleted (only if vhost entries is removed)
 *   - rename, if a dn must be renamed, for example, the apacheServerName has changed
 *   - add,    if there is a new vhost entries created    
 */
function getVhostEntriesDiff($config,$newVhosts,$HostDn)
{
  $oldVhosts = getVhostEntries($config,$HostDn,true);

  $move   = array();
  $add    = array();
  $del    = array();

  /* Generate a template for zones with default values
   */
  $zoneBase                       = array();
  $zoneBase['objectClass']        = array("top","apacheConfig");
  $zoneBase['apacheServerName']           = "";

  /* Contains all renamed apacheServerNames 
   * For zone entry udpdates
   */
  $PrePareVhostEntries = array();

  /* Walk through all zones and detect renamed/added/deleted zones ... 
   */
  foreach($newVhosts as $name => $zone){
    
    /* This zone was renamed 
     */
    if((!empty($zone['InitialApacheServerName'])) && ($zone['InitialApacheServerName'] != $zone['apacheServerName'])){
      
      /* Move old zone to new position 
       */ 
      $oldDn = "apacheServerName=".$zone['InitialApacheServerName'].",".$HostDn;
      $newDn = "apacheServerName=".$zone['apacheServerName'].",".$HostDn;
      $PrePareVhostEntries[$zone['InitialApacheServerName']] = $zone['apacheServerName'];
      $move [$oldDn] = $newDn;      
    }

    /* Get old zone if available
     */
    $oldVhost=array();
    if(isset($oldVhosts[$zone['InitialApacheServerName']])){
	    if(!empty($oldVhosts[$zone['InitialApacheServerName']])){
	      $oldVhost = $oldVhosts[$zone['InitialApacheServerName']];
	    }
    }

    /* Create forward zone entry and put it in our add queue
     */
    $newDn  = "apacheServerName=".$zone['apacheServerName'].",".$HostDn;
    $obj    =  $zoneBase;
    $obj['apacheServerName'] = $zone['apacheServerName'];
	$obj['apacheDocumentRoot'] = $zone['apacheDocumentRoot'];
	$obj['apacheServerAdmin'] = $zone['apacheServerAdmin'];

 
   	foreach($zone['apacheServerAlias'] as $rec){
      $obj['apacheServerAlias'][] = $rec;
    }
   	foreach($zone['apacheScriptAlias'] as $rec){
      $obj['apacheScriptAlias'][] = $rec;
    }

    /* Append udpated Vhost Forward Entry to our add queue
     */    
    $add[$newDn] = $obj;   
 
    /* Remove currently managed zone from oldVhosts.
     *  this gives us the ability to detect removed zones
     */
    if(isset($oldVhosts[$zone['InitialApacheServerName']])){
      unset($oldVhosts[$zone['InitialApacheServerName']]); 
    }
  }
 
  /* The rest of our oldVhosts must be deleted
   *  because they are no longer available in newVhosts anymore.
   */
  foreach($oldVhosts as $zone)  {
    $oldDn = "apacheServerName=".$zone['InitialApacheServerName'].",".$HostDn;
    $del[$oldDn] = "";
  }

  /* Check for entries which must be updated 
   */
  $zoneUpdates = array();
  $udpate = array();
  if(count($PrePareVhostEntries)){
    $ldap = $config->get_ldap_link();
    foreach($PrePareVhostEntries as $FromVhostName => $ToVhostName){
      $ldap->cd($HostDn);
      $ldap->search("(&(objectClass=apacheConfig)(apacheServerName=".$FromVhostName."))",array("apacheServerName"));
      while($attrs = $ldap->fetch()){
        $zoneUpdates[$attrs['dn']] = array("apacheServerName"=>$ToVhostName);
      }
    }
  }

  $ret = array("del" => $del , "move" => $move , "add" => $add,"vhostUpdates"=>$zoneUpdates);
  	user_error(print_r($ret,true));
  return($ret);
}

/* returns the dn for a specified zone
 */
function getVhostDN($config,$apacheServerNameMix)
{
  $ret = "";
  if(!strstr($apacheServerNameMix, '/')) {
    print_red(sprintf(_("Undefined zone name '%s'. Vhost name must look like this 'server/zone.com'."),$apacheServerNameMix));
    return($ret);
  }

  $apacheServerNameIndex  	= split("/",$apacheServerNameMix); 
  $apacheServerName           = $apacheServerNameIndex[1];
  $nameServer		  	= strtolower($apacheServerNameIndex[0]);
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
  $ldap->search("(&(objectClass=apacheConfig)(apacheServerName=".$apacheServerName."))",array("apacheServerName"));
  if($ldap->count()){
    $attr = $ldap->fetch();
    return($attr['dn']);
  }
  
  return($ret);
}


/* returns all available zones 
 *  array[reverseName] = apacheServerName;
 */
function getAvailableVhosts($config)
{
  $ret = array();
  $ldap = $config->get_ldap_link();
  $ldap->cd ($config->current['BASE']);

  /* Search for zones ...
   */
  $ldap->search("(&(objectClass=apacheConfig)(apacheServerName=*))",array("apacheServerName"));

  $ForwardVhosts = array();
  $zones = array();

  while($at = $ldap->fetch()){
    $ForwardVhosts[$at['dn']] = $at;
  }

  foreach($ForwardVhosts as $dn => $obj){
    
      if(preg_match("/".$dn."/",$Rdn)){
        $zones[$Robj['apacheServerName'][0]] =$obj['apacheServerName'][0];
      }   
  }
  return($zones);
}

?>
