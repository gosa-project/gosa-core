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

  /* Create template for all fetched vhosts Data 
   */
  $VhostBase = array();
  $VhostBase['exists']  = false;
  //$VhostBase['RECORDS'] = array();
  $VhostBase['apacheServerName'] = array();
  $VhostBase['apacheConfig'] = array();
    
  $Vhosts    = array();

  /* Get & Parse all vhosts entries 
   */
  $ldap->ls("(&(objectClass=apacheConfig)(apacheServerName=*))",$HostDn,array("*"));
  $tmp_res = array();
  while($attrs = $ldap->fetch()) {
    $tmp_res[] = $attrs;
  }

  /* Parse fetched vhosts 
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

    /* Set initial vhosts name, to be able to detect if this entry was renamed 
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


/* This function compares two apache vhosts objects and returns an 
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

  /* Generate a template for vhosts with default values
   */
  $zoneBase                       = array();
  $zoneBase['objectClass']        = array("top","apacheConfig");
  $zoneBase['apacheServerName']           = "";

  /* Contains all renamed apacheServerNames 
   * For vhosts entry udpdates
   */
  $PrePareVhostEntries = array();

  /* Walk through all vhosts and detect renamed/added/deleted vhosts ... 
   */
  foreach($newVhosts as $name => $zone){
    
    /* This vhosts was renamed 
     */
    if((!empty($zone['InitialApacheServerName'])) && ($zone['InitialApacheServerName'] != $zone['apacheServerName'])){
      
      /* Move old vhosts to new position 
       */ 
      $oldDn = "apacheServerName=".$zone['InitialApacheServerName'].",".$HostDn;
      $newDn = "apacheServerName=".$zone['apacheServerName'].",".$HostDn;
      $PrePareVhostEntries[$zone['InitialApacheServerName']] = $zone['apacheServerName'];
      $move [$oldDn] = $newDn;      
    }

    /* Get old vhosts if available
     */
    $oldVhost=array();
    if(isset($oldVhosts[$zone['InitialApacheServerName']])){
	    if(!empty($oldVhosts[$zone['InitialApacheServerName']])){
	      $oldVhost = $oldVhosts[$zone['InitialApacheServerName']];
	    }
    }

    /* Create vhosts entry and put it in our add queue
     */
    $newDn  = "apacheServerName=".$zone['apacheServerName'].",".$HostDn;
    $obj    =  $zoneBase;
    $obj['apacheServerName'] = $zone['apacheServerName'];
		$obj['apacheDocumentRoot'] = $zone['apacheDocumentRoot'];
		$obj['apacheServerAdmin'] = $zone['apacheServerAdmin'];
 
		if(!empty($zone['apacheServerAlias'])) {
 		
			foreach($zone['apacheServerAlias'] as $rec){
      	$obj['apacheServerAlias'][] = $rec;
    	}
    }
    
		if(!empty($zone['apacheScriptAlias'])) {
			foreach($zone['apacheScriptAlias'] as $rec){
      	$obj['apacheScriptAlias'][] = $rec;
    	}
		}
		
    /* Append udpated Vhost Entry to our add queue
     */    
    $add[$newDn] = $obj;   
 
    /* Remove currently managed vhosts from oldVhosts.
     *  this gives us the ability to detect removed vhosts
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
//  	user_error(print_r($ret,true));
  return($ret);
}

/* returns the dn for a specified zone
 */
function getVhostDN($config,$apacheServerNameMix)
{
  $ret = "";
  if(!strstr($apacheServerNameMix, '/')) {
    print_red(sprintf(_("Undefined vhost name '%s'. Vhost name must look like this 'www.example.com or example.com'."),$apacheServerNameMix));
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


/* returns all available vhosts
 *  array[reverseName] = apacheServerName;
 */
function getAvailableVhosts($config)
{
  $ret = array();
  $ldap = $config->get_ldap_link();
  $ldap->cd ($config->current['BASE']);

  /* Search for vhosts ...
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
  
/* Simple is domain check, it checks if the given string looks like "string.string.string"
   or string.string */
function GetVhostsDomain($str)
{
  return(preg_match("/[^\.\/]+\.[^\.\/]+$/",$str));
}

?>
