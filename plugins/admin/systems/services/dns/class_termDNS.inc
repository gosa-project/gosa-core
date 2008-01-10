<?php

class termDNS extends plugin
{
  /* attribute list for save action */
  var $ignore_account = true;
  var $autonet        = false;

  /* Basic informations 
   */
  var $attributes     = array("ipHostNumber","macAddress");
  var $objectclasses  = array("whatever");

  var $ipHostNumber   = "";    // IP address 
  var $macAddress     = "";    // Mac address 

  var $orig_ipHostNumber   = "";    // IP address 
  var $orig_macAddress     = "";    // Mac address 

  var $cn             = "";    // CN of currently edited device 
  var $OrigCn         = "";    // Initial cn
  var $IPisMust       = false;
  var $MACisMust      = false;
  var $dialog;

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
  var $DNSattributes            = array("dNSClass","zoneName","dNSTTL");
  var $DNS_is_account           = false;
  var $initially_was_account = false;
  var $dnsEntry                 = array();
  var $DNSenabled               = false;
  var $hide_dns_check_box       = FALSE;

  /*  Terminal dns 
   */
  function termDNS (&$config, $parent,$objectClasses,$IPisMust = false)
  {
    /* We need to know which objectClasses are used, to store the ip/mac
     * Because of different type of devices   
     */ 
    $this->parent         = $parent;
    $this->objectclasses  =  $objectClasses;
    $this->IPisMust       = $IPisMust;

    plugin::plugin ($config, $parent->dn);

    if(isset($this->attrs['cn'][0])){
      $this->OrigCn = preg_replace("/\\\$\$/","",$this->attrs['cn'][0]);
      $this->cn = preg_replace("/\\\$\$/","",$this->attrs['cn'][0]);
    }

 
    /************
     * DHCP 
     ************/

    /* Hide all dhcp specific code, if dhcp plugin is not present in config */
    $dhcpEnabled = FALSE;
    if($this->config->search("servdhcp","class",array("tabs"))){
      $this->dhcpEnabled = TRUE;
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
     * Autonetwork hook 
     ************/
 
    /* Do we have autonet support? */
    if (isset($this->config->data['MAIN']['AUTO_NETWORK_HOOK'])){
      $this->autonet= true;
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
    if(!$this->DNSenabled){
      $this->DNS_is_account = false;
      return;
    }

    if($this->DNSenabled){

      /* Get Zones  
       */
      $this->Zones        = getAvailableZones($config);

      /* Get Entry 
       */
      $this->dnsEntry     = getDNSHostEntries($config,$this->OrigCn);

      /* Remove A record which equals $this->ipHostNumber
       */
      $ptr = $this->get_pTRRecord();
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
      $this->RecordTypes  = getDnsRecordTypes();

      /* If there is at least one entry in this -> types, we have DNS enabled 
       */
      if($this->dnsEntry['exists']){
        $this->DNS_is_account = true;
      }else{
        $this->DNS_is_account = false;
      }
    }

    /* Create a list of used mac and ip addresses */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(|(macAddress=*)(ipHostNumber=*))",array("macAddress","ipHostNumber"));
    while($attrs = $ldap->fetch()){
      if(isset($attrs['ipHostNumber'][0])){
        $this->used_ip_mac["ip:".$attrs['ipHostNumber'][0]] = "ip:".$attrs['ipHostNumber'][0];
      }
      if(isset($attrs['macAddress'][0])){
        $this->used_ip_mac["mac:".$attrs['macAddress'][0]] = "mac:".$attrs['macAddress'][0];
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
    $netmask = FlipIp(str_replace(".in-addr.arpa","",getNameFromMix($idZone)));
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
	  /* Call parent execute */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $display= "";

    $smarty->assign("staticAddress", ""); 
    $smarty->assign("autonet", $this->autonet);
 
    /* Check for autonet button */
    if ($this->autonet && isset($_POST['autonet'])){
      $cmd= $this->config->data['MAIN']['AUTO_NETWORK_HOOK'];
      if(!empty($cmd) && $this->cn != ""){
        $res = shell_exec($cmd." ".$this->cn);
        if(!$res){
          print_red(sprintf(_("Can't execute specified AUTO_NETWORK_HOOK '%s'. Please check your gosa.conf."),$cmd));
        } else {
          $res= split(';', trim($res));
          if (isset($res[0]) && $res[0] != ""){
            $this->ipHostNumber= $res[0];
          }
          if (isset($res[1]) && $res[1] != ""){
            $this->macAddress= $res[1];
          }
        }
      }
    }
    
  
    /**********
     * DHCP Handling
     **********/
 
    if(isset($_POST['dhcpEditOptions'])){

      if(count($this->dhcpHostEntry) == 0){
        $this->dialog = new dhcpHost($this->dhcpParentNode,TRUE);
      }else{
        $this->dialog = new dhcpHost($this->dhcpHostEntry,TRUE);
      }
      $this->dialog->cn = $this->cn; 
      $this->dialog->dhcpHWAddress = "ethernet ".$this->macAddress; 
      if(!empty($this->ipHostNumber)){
        $this->dialog->statements['fixed-address'] = $this->ipHostNumber; 
      }
    }

    if(isset($_POST['cancel_dhcp'])){
      $this->dialog = FALSE; 
    }

    if(isset($_POST['save_dhcp'])){
      $this->dialog->save_object();
      
      $msgs = $this->dialog->check(array());
      if(count($msgs)){
        foreach($msgs as $msg){
          print_red($msg);
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
 
    $smarty->assign("dhcpEnabled",    $this->dhcpEnabled);
    $smarty->assign("dhcp_is_Account",$this->dhcp_is_Account);
    $smarty->assign("dhcpParentNode", $this->dhcpParentNode);
    $smarty->assign("dhcpParentNodes",$this->dhcpParentNodes);
    $smarty->assign("dhcpParentNodeCnt",count($this->dhcpParentNodes));


    /**********
     * DNS Handling
     **********/

    /* There is no dns available
     */
    if($this->DNSenabled == false){

      /* Is IP address must ? */
      $smarty->assign("DNS_is_account",false);
      $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));

      /* Assign smarty all non DNs attributes */
      foreach($this->attributes as $attr){
        $smarty->assign($attr,$this->$attr);
      }
      $smarty->assign("staticAddress","<font class=\"must\">*</font>");

      $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
    }else{
      $smarty->assign("DNS_is_account",true);

      /* Add new empty array to our record list */
      if(isset($_POST['AddNewRecord'])){
        $this->dnsEntry['RECORDS'][]  =array("type"=>"aRecord","value"=>"");
      }

      /* propose_ip */
      if(isset($_POST['propose_ip'])){
        foreach($this->Zones as $key => $name){
          if($name == $this->dnsEntry['zoneName']){
            $net = FlipIp(str_replace(".in-addr.arpa","",getNameFromMix($key)));
            $this->ipHostNumber = $this->generateRandomIp($net);
          }
        }
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
          if(isset($this->dnsEntry['RECORDS'][$id])){
            unset($this->dnsEntry['RECORDS'][$id]);
          }
        }
      }
      /* Assign smarty all non DNs attributes */
      foreach($this->attributes as $attr){
        $smarty->assign($attr,$this->$attr);
      }

      /* Assign smarty all DNS attributes */
      foreach($this->DNSattributes as $attr){
        $smarty->assign($attr,$this->dnsEntry[$attr]);
      }

      /* Assign all needed vars */
      $smarty->assign("DNSAccount",$this->DNS_is_account);
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
      $smarty->assign("staticAddress","<font class=\"must\">*</font>");

      $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
    }
    return($display);
  }


  function remove_from_parent()
  {
    if($this->initially_was_account){

      $ldap = $this->config->get_ldap_link();

      $tmp = array();
      $this->dnsEntry['exists'] = false;
      $tmp = getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);

      /* Delete dns */
      foreach($tmp['del'] as $dn => $del){
        $ldap->cd($dn);
        $ldap->rmdir_recursive($dn);
        new log("remove","unknown/".get_class($this),$dn);
      }
    }
  }


  /* Save data to object */
  function save_object()
  {

    if(isset($_POST['network_tpl_posted'])){

      /* Save all posted vars */
      plugin::save_object();

      /* Handle DHCP Posts*/
      if($this->dhcpEnabled && isset($_POST['network_tpl_posted'])){
        foreach($this->dhcpAttributes as $attr){
          if(isset($_POST[$attr])){
            $this->$attr = $_POST[$attr];
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
          $this->$attr = $_POST[$attr];
        }
      }

      /* Check if DNS should be enabled / disabled */
      if($this->DNS_is_account && $this->acl_is_removeable() && !isset($_POST['DNS_is_account'])){
        $this->DNS_is_account = false;
      }elseif(!$this->DNS_is_account && $this->acl_is_createable() && isset($_POST['DNS_is_account'])){
        $this->DNS_is_account = true;
      }

      /* Get dns attributes */
      if(($this->DNSenabled) && (isset($_POST['network_tpl_posted']))){

        /* Check for posted record changes */
        if(is_array($this->dnsEntry['RECORDS']) && $this->acl_is_writeable("Records")){
          foreach($this->dnsEntry['RECORDS'] as $key => $value){

            /* Check if type has changed */
            if(isset($_POST['RecordTypeSelectedFor_'.$key])){
              $this->dnsEntry['RECORDS'][$key]['type'] = $_POST['RecordTypeSelectedFor_'.$key];
            }
            /* Check if value has changed */
            if(isset($_POST['RecordValue_'.$key])){
              $this->dnsEntry['RECORDS'][$key]['value'] = $_POST['RecordValue_'.$key];
            }
          }
        }
        /* Get all basic DNS attributes (TTL, Clas ..)*/
        foreach($this->DNSattributes as $attr){
          if(isset($_POST[$attr]) && $this->acl_is_writeable($attr)){
            $this->dnsEntry[$attr] = $_POST[$attr];
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

    if($this->dhcpEnabled && $this->dhcp_is_Account && $this->dhcpParentNode != "" && count($this->dhcpHostEntry) == 0){
#      $message[] =_("You have not configured your dhcp settings yet.");
    }
    
    /* Check if mac and ip are already used */
    if(!empty($this->ipHostNumber) && $this->DNS_is_account && 
        $this->ipHostNumber != $this->orig_ipHostNumber && 
        in_array("ip:".$this->ipHostNumber,$this->used_ip_mac)){
      $message[] =_("The specified IP address is already in use.");
    }
    if(!empty($this->macAddress) && $this->dhcp_is_Account && 
        $this->macAddress != $this->orig_macAddress && 
        in_array("mac:".$this->macAddress,$this->used_ip_mac)){
      print_red(sprintf(_("The specified MAC address '%s' for this system '%s' is already in use."),$this->macAddress,$this->cn));
    }

    /* Check if ip must be given
     */  
    if(($this->IPisMust)||($this->DNS_is_account)){
      if (empty($this->ipHostNumber)){
        $message[]= _("The required field 'IP-address' is not set.");
      }

      if (!tests::is_ip($this->ipHostNumber)){
        $message[]= _("Wrong IP format in field IP-address.");
      }
    }

    /* Check if mac is empty 
     */
    if ($this->macAddress == "" ){
      $message[]= _("The required field 'MAC-address' is not set.");
    }
    if(!tests::is_mac($this->macAddress)){
      $message[]=(_("The given macaddress is invalid. There must be 6 2byte segments seperated by ':'."));
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
         $message[] =sprintf(_("The specified IP address '%s' is not matching the selected reverse zone entry '%s'."),$this->ipHostNumber,$tmp2);
       }

      /* Walk through all entries and detect duplicates or mismatches
       */  
      foreach($this->dnsEntry['RECORDS'] as $name => $values){

        /* Count record values, to detect duplicate entries for a specific record
         */
        if(!isset($checkArray[$values['type']][$values['value']])){
          $checkArray[$values['type']][$values['value']] = 0;
        }else{
          $message[] = sprintf(_("Found duplicate value for record type '%s'."),$values['type']);
        }

        /* Check if given entries in $onlyOnce are used more than once
         */
        if(isset($onlyOnce[$values['type']])){
          $onlyOnce[$values['type']] ++;
          if($onlyOnce[$values['type']] > 1){
            $message[] = sprintf(_("Found more than one entry for the uniqe record type '%s'."),$values['type']);
          }
        }

        /* Skip txt record ... 
         */
        if($values['type'] == "tXTRecord") continue;

        /* Check if there is an aRecord defined which uses the same IP as used in IPhostAddress 
         */
        if(($values['type'] == "aRecord")&&($values['value'] == $this->ipHostNumber)){
          $message[]=sprintf(_("The device IP '%s' is added as 'A Record', this will be done automatically, please remove the record."), 
               $this->ipHostNumber);
        }

        /* only lower-case is allowed in record entries ... 
         */
        if($values['value'] != strtolower($values['value'])){
          $message[] = sprintf(_("Only lowercase is allowed, please check your '%ss'."),$values['type']);
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

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    /****************/ 
    /* DHCP HANDLING */
    /****************/ 
  
    /* New entry */
    if($this->dhcpEnabled){

      if(count($this->dhcpHostEntry) == 0){
        $this->dialog = new dhcpHost($this->dhcpParentNode,TRUE);
        $this->dialog->cn = $this->cn;
        $this->dialog->dhcpHWAddress = "ethernet ".$this->macAddress;
        if(!empty($this->ipHostNumber)){
          $this->dialog->statements['fixed-address'] = $this->ipHostNumber;
        }
        $this->dialog->execute();
        $this->dialog->save_object(); 
        $this->dhcpHostEntry = $this->dialog->save();
        if(count($this->dhcpHostEntry['dhcpOption']) == 0){
          $this->dhcpHostEntry['dhcpOption']= array("host-name ".$this->cn);
        }
      }

      if(count($this->dhcpHostEntry) == 0){
        $this->dialog = new dhcpHost($this->dhcpParentNode,TRUE);
        $this->dialog->cn = $this->cn;
        $this->dialog->dhcpHWAddress = "ethernet ".$this->macAddress;
        if(!empty($this->ipHostNumber)){
          $this->dialog->statements['fixed-address'] = $this->ipHostNumber;
        }
        $this->dialog->execute();
        $this->dialog->save_object(); 
        $this->dhcpHostEntry = $this->dialog->save();
        if(count($this->dhcpHostEntry['dhcpOption']) == 0){
          $this->dhcpHostEntry['dhcpOption']= array("host-name ".$this->cn);
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
  

      /* Unset dhcpStatements if this attribute is empty  */
      if(isset($this->dhcpHostEntry['dhcpStatements']) && 
          ($this->dhcpHostEntry['dhcpStatements'] == "" || count($this->dhcpHostEntry['dhcpStatements']) == 0) ){
        unset($this->dhcpHostEntry['dhcpStatements']);
      }
  
      /* DHCP removed */
      if($this->initial_dhcp_is_Account && !$this->dhcp_is_Account){
        $ldap->rmdir_recursive($this->dhcpHostEntry['dn']);
        show_ldap_error($ldap->get_error(),_("Removing dhcp entry for this object failed."));

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

        show_ldap_error($ldap->get_error(),_("Tried to add new dhcp entry failed."));
      }

      /* DHCP still activated */
      if($this->initial_dhcp_is_Account && $this->dhcp_is_Account){

        /* DHCP node changed */
        if(($this->initial_dhcpParentNode != $this->dhcpParentNode) || 
           ($this->cn != $this->OrigCn)){
          $attrs = $this->dhcpHostEntry;
          $attrs['cn'] = $this->cn;
          unset($attrs['dn']);
          unset($attrs['MODIFIED']);
          $ldap->cd("cn=".$this->cn.",".$this->dhcpParentNode);
          $res = $ldap->add($attrs);

          $tmp = new servdhcp($this->config,$this->dhcpParentNode);
          $tmp->handle_post_events("modify");

          show_ldap_error($ldap->get_error(),_("Tried to add new dhcp entry failed."));
          if($res){
            $ldap->rmdir_recursive($this->dhcpHostEntry['dn']);
            show_ldap_error($ldap->get_error(),_("Removing old dhcp entry failed."));
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

          show_ldap_error($ldap->get_error(),_("Modifying dhcp entry failed."));
        }    
      }
    }
      

    /****************/ 
    /* DNS HANDLING */
    /****************/ 

    /* If isn't DNS account but initially was DNS account 
       remove all DNS entries 
     */ 
    if((!$this->DNSenabled) || ((!$this->DNS_is_account)&&(!$this->initially_was_account))){
      return;
    }else{

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
        $tmp = getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);
      }else{
        $this->dnsEntry['exists'] = $this->DNS_is_account;
        $tmp = getDNSHostEntriesDiff($this->config,$this->OrigCn,$this->dnsEntry,$this->cn);
      }   

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
      if($ldap->get_error() != "Success"){
        show_ldap_error($ldap->get_error(), sprintf(_("Saving of terminal/dns account with dn '%s' failed."),$this->dn));
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
    
    if(!$this->DNS_is_account) {
      $str = "<input type='submit' value='"._("Add")."' name='AddNewRecord' id='AddNewRecord' disabled='disabled'>";
      return(array("str" => $str, "changeStateForRecords"=> ""));
    }
 
    $str = "<table summary='' width='100%'>";
    foreach($this->dnsEntry['RECORDS'] as $key => $entry){

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
           "      <input type='submit' value='"._("Add")."' name='AddNewRecord' id='AddNewRecord'>".
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
        "plCategory"    => array("workstation","terminal","phone","server","component","printer","winworkstation"),

        "plProvidedAcls"=> array(
          "ipHostNumber"  => _("IP address"),
          "macAddress"    => _("MAC address"))
        );

    /* Hide all dns specific code, if dns is not available
     */
    $config = session::get('config');
    foreach($config->data['TABS']['SERVERSERVICE'] as $tab){
      if(preg_match("/^servdns$/",$tab['CLASS'])){
        $tmp['plProvidedAcls']["Records"]        = _("DNS records");
        $tmp['plProvidedAcls']["zoneName"]       = _("Zone name");
        $tmp['plProvidedAcls']["dNSTTL"]         = _("TTL");
        break;
      }
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
    $ldap->search ("(&(objectClass=dhcpHost)(cn=".$this->cn."))",array("cn","dn"));

    if($ldap->count()){
      $attr = $ldap->fetch();
      return($attr['dn']);
    }else{
      return("");
    }
  }  


  function get_dhcp_parent_node()
  {
    return(preg_replace("/^cn=".normalizePreg($this->cn).",/","",$this->get_dhcp_host_entry_dn()));
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
      $ldap->cat(getDNSZoneDN($this->config,$this->dnsEntry['zoneName']));
      $attrs = $ldap->fetch();
      $tmp = array_flip($this->Zones);
      $tmp = preg_replace("/^[^\/]*+\//","",$tmp[$this->dnsEntry['zoneName']]);
      $tmp = trim(preg_replace("/\.in-addr.arpa$/","",$tmp));
      $ptr = preg_replace("/^".normalizePreg(FlipIp($tmp))."\./","",$this->ipHostNumber);
      return($ptr);
    }else{
      return(FALSE);
    }
  }

  
  function generateRandomIP($net = "")
  {
    $str = $net;
    $cnt = 4;
    while(substr_count($str,".") < 3 && $cnt > 0){
      $str .= ".".rand(0,255);
      $str = preg_replace("/\.\.*/",".",$str);
      $str = trim($str,". ");
      $cnt --;
    }
    return($str);
  }

  
  function create_tree($arr,$base,$current = "")
  {
    $ret = array();
    foreach($arr as $r => $name){
      $base_part = str_replace($base,"",$r);
      if(preg_match("/^[a-z]*=".normalizePreg($name)."(|,)$/i",$base_part)){
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
      $this->DNS_is_account  = TRUE;
      $this->hide_dns_check_box = TRUE;
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
