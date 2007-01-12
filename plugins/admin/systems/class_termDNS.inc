<?php

class termDNS extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account = true;
  var $autonet        = false;

  /* Basic informations 
   */
  var $attributes     = array("ipHostNumber","macAddress");
  var $objectclasses  = array("whatever");

  var $ipHostNumber   = "";    // IP address 
  var $macAddress     = "";    // Mac address 
  var $cn             = "";    // CN of currently edited device 
  var $OrigCn         = "";    // Initial cn
  var $IPisMust       = false;
  var $MACisMust      = false;

  /* DNS attributes  
   */
  var $DNSattributes            = array("dNSClass","zoneName","dNSTTL");
  var $DNS_is_Account           = false;
  var $DNSinitially_was_account = false;
  var $dnsEntry                 = array();
  var $DNSenabled               = false;

  /*  Terminal dns 
   */
  function termDNS ($config, $dn,$objectClasses,$IPisMust = false)
  {
    /* We need to know which objectClasses are used, to store the ip/mac
     * Because of different type of devices   
     */
    $this->objectclasses  =  $objectClasses;
    $this->IPisMust       = $IPisMust;

    plugin::plugin ($config, $dn);

    if(isset($this->attrs['cn'][0])){
      $this->OrigCn = $this->attrs['cn'][0];
      $this->cn = $this->attrs['cn'][0];
    }

    /* Do we have autonet support? */
    if (isset($this->config->data['MAIN']['AUTO_NETWORK_HOOK'])){
      $this->autonet= true;
    }

    /* Hide all dns specific code, if dns is not available 
     */
    $DNSenabled = false;
    foreach($this->config->data['TABS']['SERVTABS'] as $tab){
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
      foreach($this->dnsEntry['RECORDS'] as $key => $rec){
        if(($rec['type'] == "aRecord") && ($rec['value'] == $this->ipHostNumber)){
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
 
    /* Store initally account settings 
     */
    $this->DNSinitially_was_account = $this->DNS_is_account;
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

    /* There is no dns available 
     */
    if($this->DNSenabled == false){
       
      /* Is IP address must ? */ 
      $smarty->assign("DNS_is_account",false);  
      $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));
    
      /* Assign smarty all non DNs attributes */
      foreach($this->attributes as $attr){
        $smarty->assign($attr,$this->$attr);
        $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
      }
      $smarty->assign("staticAddress","<font class=\"must\">*</font>");

      $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
      return($display);
    }else{
      $smarty->assign("DNS_is_account",true); 
    }
 
    /* Add new empty array to our record list */
    if(isset($_POST['AddNewRecord'])){
      $this->dnsEntry['RECORDS'][]  =array("type"=>"aRecord","value"=>"");
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
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Assign smarty all DNS attributes */
    foreach($this->DNSattributes as $attr){
      $smarty->assign($attr,$this->dnsEntry[$attr]);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }
    
    /* Assign all needed vars */
    $smarty->assign("DNSaccountACL",chkacl($this->acl,"termDNS"));

    $smarty->assign("DNSAccount",$this->DNS_is_account);
    $smarty->assign("Zones",$this->Zones);
    $smarty->assign("ZoneKeys",($this->Zones));
    $smarty->assign("IPisMust",(($this->IPisMust)||($this->DNS_is_account)));
  
    $tmp = $this->generateRecordsList();
    
    $changeStateForRecords = $tmp['changeStateForRecords'];

    $smarty->assign("records",$tmp['str']);
    $smarty->assign("changeStateForRecords",$changeStateForRecords);
    $smarty->assign("staticAddress","<font class=\"must\">*</font>");

    $display.= $smarty->fetch(get_template_path('network.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /*
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->orig_dn);
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(!(relativeDomainName=@)))",array("relativeDomainName","zoneName"));
    while($attr = $ldap->fetch()){  
      $ldap->cd($attr['dn']);
      $ldap->rmDir($attr['dn']);
      show_ldap_error("Record:".$ldap->get_error(), _("Removing terminal from DNS object failed")); 
    }
    */
  }

  /* Save data to object */
  function save_object()
  {
    /* Save all posted vars */
    plugin::save_object();
    
    /* Get dns attributes */
    if(($this->DNSenabled) && (isset($_POST['network_tpl_posted'])) && chkacl($this->acl,"termDNS") == ""){

      /* Check for posted record changes */
      if(is_array($this->dnsEntry['RECORDS'])){
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
        if(isset($_POST[$attr])){
          $this->dnsEntry[$attr] = $_POST[$attr];
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
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Check if ip must be given
     */  
    if(($this->IPisMust)||($this->DNS_is_account)){
  
      /* Check if ip is empty 
       */
      if ($this->ipHostNumber == "" && chkacl ($this->acl, "ipHostNumber") == ""){
        $message[]= _("The required field 'IP-address' is not set.");
      }

    }

    /* check if given ip is valid ip */
    if ($this->ipHostNumber != "" && !is_ip($this->ipHostNumber)){
      $message[]= _("Wrong IP format in field IP-address.");
    }

    /* Check if mac is empty 
     */
    if ($this->macAddress == "" && chkacl ($this->acl, "macAddress") == ""){
      $message[]= _("The required field 'MAC-address' is not set.");
    }

    /* Check if given mac is valid mac 
     */
    if(!is_mac($this->macAddress)){
      $message[]=(_("The given macaddress is invalid. There must be 6 2byte segments seperated by ':'."));
    }

    /* only perfrom this checks if this is a valid DNS account */
    if($this->DNS_is_account){

      $checkArray = array();
      $onlyOnce   = array();

			// There can be many CNAME records
      //$onlyOnce['cNAMERecord'] = 0;

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
    if(!$this->DNSenabled){
      return;
    }else{

      /* Add ipHostNumber to aRecords
       */
      $this->dnsEntry['RECORDS'][] = array("type"=>"aRecord","value"=>$this->ipHostNumber);

      /* Create diff and follow instructions 
       * If Account was disabled, remove account by setting exists to false
       */
      if((!$this->DNS_is_account)&&($this->DNSinitially_was_account)){  
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
      }

      /* Display errors 
       */
      if($ldap->get_error() != "Success"){
        show_ldap_error("Record:".$ldap->get_error(), _("Saving terminal to DNS object failed")); 
      }
    }
  }

  /*  Create html table with all used record types
   */
  function generateRecordsList()
  {
    $changeStateForRecords = "";
    
    if(!$this->DNS_is_account) {
      $str = "<input type='submit' value='"._("Add")."' name='AddNewRecord' id='AddNewRecord' disabled='disabled'>";
      return $str;
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
           "      <input type='submit' value='"._("Add")."' name='AddNewRecord'>".
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
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
