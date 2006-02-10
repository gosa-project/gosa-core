<?php

class servdns extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = FALSE;
  var $attributes       = array(""); 
  var $objectclasses    = array("whatever");

  var $RecordTypes      = array();

  var $Zones  = array();
  var $dialog = NULL;

  var $usedDNS    = array();

  var $orig_dn = "";

  var $DNSinitially_was_account;


  function servdns ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->orig_dn = $dn;

    /* All types with required attrs */
    $this->RecordTypes['aRecord']       = "aRecord";           // ok
    $this->RecordTypes['mDRecord']      = "mDRecord";          // ok
    $this->RecordTypes['mXRecord']      = "mXRecord";          // ok
    $this->RecordTypes['nSRecord']      = "nSRecord";          // ok
    $this->RecordTypes['pTRRecord']     = "relativeDomainName";// ok
    $this->RecordTypes['hInfoRecord']   = "hInfoRecord";       // ok
    $this->RecordTypes['mInfoRecord']   = "mInfoRecord";       // ok
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

    $types = array();

    /* Get all records */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@))",array("*"));

    while($attrs = $ldap->fetch()){
      /* If relative domainname 
       * Try to read dnsclass / TTl / zone
       */
      $this->usedDNS[$attrs['dn']] = $attrs['dn'];
      if((isset($attrs['tXTRecord'][0]))&&(preg_match("/zoneName\=/",$attrs['tXTRecord'][0]))){
        $zoneName= preg_replace("/zoneName\=/","",$attrs['tXTRecord'][0]);  
        $z = preg_replace("/\.in\-addr\.arpa/","",$attrs['zoneName'][0]);
        
        $z = $this->FlipIp($z);

        $types[$zoneName]['ReverseZone']  = $z;
        $types[$zoneName]['ReverseDN']    = $attrs['dn']; 
      }else{

        /* Generate SOA entry */
        if(isset($attrs['sOARecord'][0])){
          $tmp = split("\ ",$attrs['sOARecord'][0]) ;
          $tmp2 = array();
          $ar = array("0"=>"sOAprimary","1"=>"sOAmail","2"=>"sOAserial","3"=>"sOArefresh","4"=>"sOAretry","5"=>"sOAexpire","6"=>"sOAttl");

          /* Assign soa vars */
          foreach($ar as $key => $name){
            if(isset($tmp[$key])){
              $types[$attrs['zoneName'][0]][$name] = $tmp[$key];
            }else{
              $types[$attrs['zoneName'][0]][$name] = "";
            }
          }
        }

        /* Set TTL value */
        if(isset($attrs['dNSTTL'][0])){
          $types[$attrs['zoneName'][0]]['dNSTTL'] = $attrs['dNSTTL'][0];
        }

        /* Set dns Class*/
        if(isset($attrs['dNSClass'][0])){
          $types[$attrs['zoneName'][0]]['dNSClass'] = $attrs['dNSClass'][0];
        }

        /* Set zone Name */
        if(isset($attrs['zoneName'][0])){
          $types[$attrs['zoneName'][0]]['zoneName'] = $attrs['zoneName'][0];
        }

        /* Create list with all used records */
        foreach($this->RecordTypes as $name => $value){

          /* If there is a record attribute  */
          if(isset($attrs[$name])){

            $types[$attrs['zoneName'][0]]['Records']=array();

            /* get all entries */
            for($i = 0 ; $i < $attrs[$value]['count']; $i ++){
              $types[$attrs['zoneName'][0]]['Records'][] =array("type"      =>$name,
                  "inittype"  =>$name,
                  "value"     =>$attrs[$value][$i],
                  "status"    =>"edited",
                  "dn"        =>$attrs['dn']);
            }
          }
        }
      }
    }

    /* If there is at least one entry in this -> types, we have DNS enabled */
    $this->Zones = $types;
    if(count($this->Zones) == 0){
      $this->is_account = false;
    }else{
      $this->is_account = true;
    }

    /* Store initally account settings */
    $this->DNSinitially_was_account = $this->is_account;
  }


  /* this is used to flip the ip address for example 
      12.3.45  ->  54.3.12     
     Because some entries (like zones) are store like that 54.3.12.in-addr.arpa
      but we want to display 12.3.45.
  */
  function FlipIp($ip)
  {
    $tmp = array_reverse(split("\.",$ip));
    $new = "";
    foreach($tmp as $section){
      $new .= $section.".";
    }
    return(preg_replace("/.$/","",$new));
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove DNS service"),
          _("This server has DNS features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DNS service"),
          _("This server has DNS features disabled. You can enable them by clicking below."));
      return ($display);
    }

    /* Edited or Added zone hould be saved saved */
    if(isset($_POST['SaveZoneChanges'])){
      $this->dialog->save_object();

      /* Check if noting went wrong */
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msgs){
          print_red($msgs); 
        }
      }else{
      
        /* add new/edited zone */
        $ret = $this->dialog->save();
        unset($this->Zones[$this->dialog->InitiallyZoneName]);
        $this->Zones[$ret['zoneName']]                  = $ret;
        $this->dialog = NULL;
      }
    }

    /* Cancel zone edit / new */
    if(isset($_POST['CancelZoneChanges'])){
      $this->dialog = NULL;
    }

    /* Add empty new zone */
    if(isset($_POST['AddZone'])){
      $this->dialog = new servdnseditZone($this->config,$this->dn,$this->RecordTypes);
    }

    /* Check for edit zone request */
    $once = false;
    foreach( $_POST as $name => $value){
  
      /* check all post for edit request */
      if(preg_match("/^editZone_/",$name)&&!$once){
        $once =true;
        $tmp = preg_replace("/^editZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        $this->dialog= new servdnseditZone($this->config,$this->dn,$this->RecordTypes,$this->Zones[$tmp]);
      }

      /* check posts for delete zone */
      if(preg_match("/^delZone_/",$name)&&!$once){


        $once =true;
        $tmp = preg_replace("/^delZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
  
        $zones =  $this->getUsedZoneNames();
        $rev = $this->Zones[$tmp]['ReverseZone'];
        $res = array_merge(($zones[$tmp]),($zones[$rev.".in-addr.arpa"]));
        
        if(count($res)){
          $i = 2;
          $str ="";
          foreach($res as $dn){
            if($i > 0 ){
              $i --;
              $str.=$dn." ";
            }
          }
          if(count($res)> 2) $str .=" ... ";
          print_red(sprintf(_("Can't delete the selected zone, because it is still in use by these entry/entries '%s'"),trim($str)));
        }else{
          unset($this->Zones[$tmp]);
        }
      }
    }

    /* Show dialog */
    if($this->dialog!= NULL){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Zones */
    $ZoneList = new divSelectBox("dNSZones");
    $ZoneList -> SetHeight(254);

    /* Add entries to divlist*/
    $editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>
      <input type='image' src='images/edittrash.png' name='delZone_%s'>";
    foreach($this->Zones as $zone => $values ){
      $ZoneList->AddEntry(array(
            array("string" => $zone),
            array("string" => _("Reverse zone")." : ".$values['ReverseZone']),
            array("string" => _("TTL")." : ".$values['dNSTTL']),
            array("string" => _("Class")." : ".$values['dNSClass']),
            array("string" =>str_replace("%s",base64_encode($zone),$editImg))
            ));
    }    

    /* Display tempalte */
    $smarty->assign("ZoneList",$ZoneList->DrawList());
    $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE));
    return($display);
  }


  /* This funtion returns all used Zonenames */
  function getUsedZoneNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(!(relativeDomainName=@))(zoneName=*))",array("zoneName","relativeDomainName","tXTRecord"));
    while($attr = $ldap->fetch()){
      if(preg_match("/in-addr\.arpa/",$attr['zoneName'][0])){
        $ret[$attr['zoneName'][0]][] = $attr['dn'];
      }else{
        $ret[$attr['zoneName'][0]][] = $attr['dn'];
      }
    }
    return($ret);
  }


  /* Remove dns service */
  function remove_from_parent()
  {
    if(!$this->DNSinitially_was_account){
      return;
    }

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    foreach($this->usedDNS as $dn){
      $ldap->cd($dn);
      $ldap->rmdir_recursive($dn);
    }

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->orig_dn);
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*)(relativeDomainName=@))",array("relativeDomainName","zoneName"));
    while($attr = $ldap->fetch()){
      $ldap->cd($attr['dn']);
      $ldap->rmDir($attr['dn']);
    }


    show_ldap_error($ldap->get_error());
  }


  /* Save data to object */
  function save_object()
  {
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Ldap conenction / var initialization */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $actions =array("update"=>array(),"add"=>array(),"delete"=>array());

    /* Generate entries for all zones, and check if they must be updated deleted added */
    foreach($this->Zones as $zone){
      
      /* Get ldap syntax */
      $tmp = $this->generate_LDAP_entries($zone);

      /* Check if dn is new, or if entry was edited */
      foreach($tmp as $key => $values){
        if(isset($this->usedDNS[$key])){
          $actions['update'][$key]=$values;
          unset($this->usedDNS[$key]);
        }else{
          $actions['add'][$key] = $values;
        }
      }
    }
    
    /* Check which dns are not used anymore ...*/
    foreach($this->usedDNS as $key => $values){
      $actions['delete'][$key] = $values;
    }

    /* Remove deleted zones */
    foreach($actions['delete'] as $dn => $attrs){
      $ldap->cd($dn);
      $ldap->rmdir_recursive($dn);
    }

    /* Add new zones */
    foreach($actions['add'] as $dn => $attrs){
      $ldap->cd($this->config->current['BASE']);
      //      $ldap->create_missing_trees($dn);
      $ldap->cd($dn);
      $ldap->add($attrs);
    }

    /* Update existing entries */
    foreach($actions['update'] as $dn => $attrs){
      $ldap->cd($dn);
      $this->cleanup();
$ldap->modify ($attrs); 

    }
    show_ldap_error($ldap->get_error());
  }


  /* This function generates ldap friendly output 
     of all changes for a single zone (reverse and forward)
   */
  function generate_LDAP_entries($zone)
  {
    $tmp = array();
    $tmp['objectClass']           = array("top","dNSZone");
    $tmp['dNSTTL']                = $zone['dNSTTL']; 
    $tmp['dNSClass']              = $zone['dNSClass']; 
    $tmp['relativeDomainName']    = "@";//$zone['relativeDomainName']; 

    $str = "";
    foreach(array("sOAprimary","sOAmail","sOAserial","sOArefresh","sOAretry","sOAexpire","sOAttl") as $name){
      $str .= $zone[$name]." "; 
    }
    $tmp['sOARecord'] = $str;


    /* Generate Record entries  */
    $arr = array("SigRecord","KeyRecord","aAAARecord","nSRecord","iaFSDBRecord","mInfoRecord","hInfoRecord","mXRecord","mDRecord","tXTRecord",
        "LocRecord","nXTRecord","sRVRecord","nAPTRRecord","kXRecord","certRecord","a6Record","dSRecord","sSHFPRecord","rRSIGRecord","nSECRecord");
    $aRecords = array();
    foreach($arr as $ar){
      if((isset($zone['Records']))&&(is_array($zone['Records']))){
        foreach($zone['Records'] as $type){
          if(($type['type'] == $ar)&&($type['status']!="deleted")){
            $tmp[$ar][] = $type['value'];
          }
        }
      }
    }
   
    /* Check if there are records removed,
        if there are some removed records, the append an array        
        to ensure that these record types are deleted 
     */
    if((isset($zone['Records']))&&(is_array($zone['Records']))){
      foreach($zone['Records'] as $type){
        if((isset($type['inittype']))&&($type['inittype']!="")){
          if($type['type'] != $type['inittype']){
            $tmp[$type['inittype']] = array();
          }
        }
      }
    }
    
    /* generate forward entry */
    $dn = "zoneName=".$zone['zoneName'].",".$this->dn; 
    $tmp2[$dn] = $tmp;
    $tmp2[$dn]['zoneName'] = $zone['zoneName'];

    /* generate reverse entry */
    $dn = "zoneName=".$this->FlipIp($zone['ReverseZone']).".in-addr.arpa,".$this->dn;
    $tmp2[$dn] = $tmp;
    $tmp2[$dn]['tXTRecord'] ="zoneName=".$zone['zoneName'];
    $tmp2[$dn]['zoneName'] = $this->FlipIp($zone['ReverseZone']).".in-addr.arpa";

    return($tmp2);
  }



}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
