<?php

class servdns extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes       = array(""); 
  var $objectclasses    = array("whatever");

  var $RecordTypes      = array();

  var $Zones  = array();
  var $dialog = NULL;

  function servdns ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

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

    $this->cn = $this->attrs['cn'][0];

    $types = array();

    /* Get all records */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@))",array("*"));

    while($attrs = $ldap->fetch()){
      /* If relative domainname == cn
       * Try to read dnsclass / TTl / zone
       */
      if((isset($attrs['tXTRecord'][0]))&&(preg_match("/zoneName\=/",$attrs['tXTRecord'][0]))){
        $zoneName= preg_replace("/zoneName\=/","",$attrs['tXTRecord'][0]);  
        $types[$zoneName]['ReverseZone']  = $attrs['zoneName'][0];
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

        /* Set status */
        $types[$attrs['zoneName'][0]]['status']   ="edited"; 
        $types[$attrs['zoneName'][0]]['ReverseDN']= $attrs['dn']; 

        /* Create list with all used records */
        foreach($this->RecordTypes as $name => $value){

          /* If there is a record attribute  */
          if(isset($attrs[$name])){

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

    if(isset($_POST['SaveZoneChanges'])){
      $this->dialog->save_object();
    
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msgs){
          print_red($msgs); 
        }
      }else{
        $ret = $this->dialog->save();
        if($this->dialog->isNew == true){
          $this->Zones[$ret['zoneName']] = $ret;
          $this->Zones[$ret['zoneName']] = "new";
        }else{

          if(!$this->dialog->isNew){

            /* Edited but not renamed */
            if($ret['zoneName'] == $this->dialog->InitiallyZoneName){
              $this->Zones[$ret['zoneName']] = $ret;

            }else{
              /* Edited and renamed, we have to check if we must delete the old entry from ldap */

              if($this->Zones[$this->dialog->InitiallyZoneName]['status'] == "new"){
                /* this was a new entry (currently not saved to ldap) so we can simply unset the zone */
                unset($this->Zones[$this->dialog->InitiallyZoneName]);
              
              }elseif($this->Zones[$this->dialog->InitiallyZoneName]['status'] == "edited") {
                $this->Zones[$this->dialog->InitiallyZoneName]['status'] = "deleted";
              } 
              $this->Zones[$ret['zoneName']] = $ret;
              $this->Zones[$ret['zoneName']]['status'] = "new";
            }
          }else{
            $this->Zones[$ret['zoneName']] = $ret;
            $this->Zones[$ret['zoneName']]['status'] = "new";
          }
        }
        $this->dialog = NULL;
      }
    }
    if(isset($_POST['CancelZoneChanges'])){
      $this->dialog = NULL;
    }
    /* Add empty new zone */
    if(isset($_POST['AddZone'])){
      $this->dialog = new servdnseditZone($this->config,$this->dn,$this->RecordTypes);
    }

    $once = false;
    foreach( $_POST as $name => $value){
      if(preg_match("/^editZone_/",$name)&&!$once){
        $once =true;
        $tmp = preg_replace("/^editZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        $this->dialog= new servdnseditZone($this->config,$this->dn,$this->RecordTypes,$this->Zones[$tmp]);
      }
    }

    if($this->dialog!= NULL){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Zones */
    $ZoneList = new divSelectBox("dNSZones");
    $ZoneList -> SetHeight(254);

    /* Add entries to divlist*/
    $editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>";
    foreach($this->Zones as $zone => $values ){
      if($values['status'] == "deleted") continue;
      $ZoneList->AddEntry(array(array("string" => $zone.str_replace("%s",base64_encode($zone),$editImg))));
    }    

    /* Display tempalte */
    $smarty->assign("ZoneList",$ZoneList->DrawList());
    $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
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
  return(true);
//    plugin::save();
    foreach($this->Zones as $zone){
      $tmp = array();
      if($zone['status'] == "deleted"){
        echo "deleted";
      }elseif($zone['status'] == "new" ){
        echo "new";
      }elseif($zone['status'] == "edited"){  
        echo "edit";
      }else{
        echo "undefined type";
      }

      //$tmp = $this->generate_LDAP_entries($tmp,$zone,$delete);
    }
  }

  function generate_LDAP_entries($tmp,$zone,$delete)
  {
    $delete  = array();

    $tmp['objectClass']           = array("top","dNSZone");
    $tmp['dNSTTL']                = $zone['dNSTTL']; 
    $tmp['dNSClass']              = $zone['dNSClass']; 
    $tmp['relativeDomainName']    = $zone['relativeDomainName']; 

    $str = "";
    foreach($zone['sOARecord'] as $name => $value){
      $str .= $value." "; 
    }
    $tmp['sOARecord'] = $str;

    $dn = "zoneName=".$zone['zoneName'].",".$this->dn;

    /* Generate some attrs  */
    $arr = array("SigRecord","KeyRecord","aAAARecord","nSRecord","iaFSDBRecord","mInfoRecord","hInfoRecord","mXRecord","mDRecord","tXTRecord",
        "LocRecord","nXTRecord","sRVRecord","nAPTRRecord","kXRecord","certRecord","a6Record","dSRecord","sSHFPRecord","rRSIGRecord","nSECRecord");
    $aRecords = array();
    foreach($arr as $ar){
      foreach($zone['types'] as $type){
        if(($type['type'] == $ar)&&($type['status']!="deleted")){
          $tmp[$ar][] = $type['value'];
        }
      }
    }
    foreach($zone['types'] as $type){
      if(isset($type['inittype'])){
        if($type['type'] != $type['inittype']){
          $tmp[$type['inittype']] = array();
        }
      }
    }
    return($tmp);
  }



}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
