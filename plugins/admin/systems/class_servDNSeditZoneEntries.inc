<?php

class servDNSeditZoneEntries extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account     = TRUE;
  var $attributes         = array();
  var $objectclasses      = array("whatever");

  var $Devices            = array();

  var $zoneName           = "";   // ZoneName of currently edited Zone
  var $reverseName        = "";   // ReverseZone of the currently edited Zone

  var $RecordTypes        = array();  // Possible record type.

  var $disableDialog      = false; // Dialog will be disabled, if this zone is new 

  function servDNSeditZoneEntries ($config,$dn, $zoneObject)
  {
    plugin::plugin ($config, $dn);

    /* Initialise class
     */
    $this->RecordTypes  = getDnsRecordTypes();
    $this->dn           = "zoneName=".$zoneObject['InitialzoneName'].",".$dn; 
    $this->zoneName     = $zoneObject['InitialzoneName'];
    $this->reverseName  = $zoneObject['InitialReverseZone'];

    /* Get ldap connection 
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Get zone content
     */
    $ldap->ls("(&(objectClass=dNSZone)(!(relativeDomainName=@)))",$this->dn,array("relativeDomainName"));
    
    while($attrs = $ldap->fetch()){
      $this->Devices[$attrs['relativeDomainName'][0]] = getDNSHostEntries($config,$attrs['relativeDomainName'][0],true);
      $this->Devices[$attrs['relativeDomainName'][0]]['OrigCn'] = $attrs['relativeDomainName'][0];
    }

    if(!count($this->Devices)){
      $this->disableDialog = true;
    }
  }

  function execute()
  {
    plugin::execute();


    /* Check posts for operations ...  
     */
    $once = true;
    foreach($_POST as $name => $value){

      /* Add a new Record in given object  
       */
      
      $tmp    = preg_replace("/^.*_(.*)_.*$/","\\1",$name);
      $tmp2   = split("\|",$tmp);

      /* Add new host entry
       */
      if((preg_match("/^UserRecord_/",$name)) && ($once)){
        $once = false;
        $entry = getDNSHostEntries($this->config,"",true);     
        $entry['exists']    = true;
        $entry['zoneName']  = $this->zoneName; 
        $entry['RECORDS'][] = array("type" => "aRecord" , "value"=>"");
        $this->Devices['New Entry'] = $entry;
      }

      if(count($tmp2) != 2) continue;

      $Name     = base64_decode($tmp2[0]);
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
      }

    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $table = "";
    foreach($this->Devices as $key => $dev){
      $table .= $this->generateRecordConfigurationRow($key);
    }

    $smarty->assign("disableDialog",$this->disableDialog);
    $smarty->assign("table",$table);;
    $display.= $smarty->fetch(get_template_path('servDNSeditZoneEntries.tpl', TRUE));
    return($display);
  }


  function save_object()
  {
    /* Possible attributes posted 
     */
    foreach($_POST as $name => $value){

      /* Extract informations out of post name 
       */
      $tmp    = preg_replace("/^.*_/","\\1",$name);
      $tmp2   = split("\|",$tmp);      
 
      if(count($tmp2) != 2) continue;

      $Name     = base64_decode($tmp2[0]);
      $RecordID = $tmp2[1];

      /* Check for value change 
       */          
      if(preg_match("/ValueSelection_/",$name)){
        if(isset($this->Devices[$Name]['RECORDS'][$RecordID])){
          $this->Devices[$Name]['RECORDS'][$RecordID]['value'] = $value;
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

      /* Extract informations out of post name 
       */
      $tmp    = preg_replace("/^.*_/","\\1",$name);
      $tmp2   = split("\|",$tmp);      
 
      if(count($tmp2) != 2) continue;

      $Name     = base64_decode($tmp2[0]);
      $RecordID = $tmp2[1];

      /* Host renamed
       */
      if(preg_match("/RenameHost_/",$name)){
        if((isset($this->Devices[$Name])) && ($Name != $value)){
    
          if(isset($this->Devices[$value])){
            print_red(sprintf(_("Can't rename '%s' to '%s' there is already an entry with the same name in our zone editing dialog."),$Name,$value));
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
    
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    $names = array();
    foreach($this->Devices as $DevName => $device){

      /* Checking entry name
       */
      if((!is_uid($DevName)) | (empty($DevName))){
        $message[] = sprintf(_("Entry name '%s' contains invalid characters."), $DevName);
      }      

      /* Renaming check for existing devices 
       */
      if(isset($device['OrigCn'])  && ($DevName != $device['OrigCn'] )){
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(relativeDomainName=".$DevName.")",array("relativeDomainName"));
        if($ldap->count()){
          $message[] = sprintf(_("Can not rename '%s' to '%s',the destination name already exists."),$device['OrigCn'],$DevName);
        }
      }elseif(!isset($device['OrigCn'])){
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(relativeDomainName=".$DevName.")",array("relativeDomainName"));
        if($ldap->count()){
          $message[] = sprintf(_("Can not create '%s',the destination name already exists."),$DevName);
        }
      }

      /* Check names 
       */
      if(!isset($names[$DevName])){
        $names[$DevName] = "";
      }else{
        $message[] = sprintf(_("The name '%s' is used more than once."),$DevName);
      }

      /* Names should be written in lowercase
       */
      if(strtolower($DevName) != $DevName){
        $message[] = sprintf(_("The host name '%s' should be written in lowercase."), $DevName);
      }

      /* Check records
       */                 
      $singleEntries = array("cNAMERecord","pTRRecord");

      $tmp  = array();
      $tmp2 = array();
      foreach($device['RECORDS'] as $Num => $Rec){

        /* Check for multiple use of unique record types
         */
        if(in_array($Rec['type'],$singleEntries)){
          if(!isset($tmp[$Rec['type']])){
            $tmp[$Rec['type']] = "";
          }else{
            $message[] = sprintf(_("The record type '%s' is a unique type and can't be defined twice."),$type);
          }
        }

        /* Check for empty / duplicate entries in record array 
         */
        if(empty($Rec['value'])){
          $message[] = sprintf(_("There is an empty '%s' for host '%s'."),$Rec['type'],$DevName);
        }

        /* Check for duplicate record entries 
         */
        if(!isset($tmp[$Rec['type']][$Rec['value']])){
          $tmp[$Rec['type']][$Rec['value']] = "";
        }else{
          $message[] = sprintf(_("There is a duplicate entry in '%s' for '%s'."),$Rec['type'],$DevName); 
        }
      }
    }
    return ($message);
  }

  function save()
  {
    if($this->disableDialog) return;

    $todo = array(); 

    /* Create todolist
     */
    foreach($this->Devices as $name => $dev){
      if(isset($dev['OrigCn'])){
        if(count($dev['RECORDS'])){
          $todo[] = getDNSHostEntriesDiff($this->config,$dev['OrigCn'],$dev,$name);
        }else{
          $dev['exists'] = false;
          $todo[] = getDNSHostEntriesDiff($this->config,$dev['OrigCn'],$dev,$name);
        }
      }else{
        if(count($dev['RECORDS'])){
          $todo[] = getDNSHostEntriesDiff($this->config,"",$dev,$name);
        }else{
          $dev['exists'] = false;
          $todo[] = getDNSHostEntriesDiff($this->config,"",$dev,$name);
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
    }

    /* Add || Update new DNS entries
     */
    foreach($tmp['add'] as $dn => $attrs){
      $ldap->cd($dn);
      $ldap->cat($dn);
      if(count($ldap->fetch())){
        $ldap->cd($dn);
        $ldap->modify ($attrs);
      }else{
        $ldap->cd($dn);
        $ldap->add($attrs);
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

    /* Abort if emtpy
     */
    if(count($obj['RECORDS']) == 0) return "";

    /* Set title 
     */
    $str = "<h3>".sprintf(_("Settings for '%s'"),$objKey)." : </h3>";

    $hostNameOnce = true;

    /* Walk through all defined records 
     */
    $str.= "<table cellspacing='0' cellpadding='0'>";
    foreach($obj['RECORDS'] as $id => $record){

      /* Create unique post name
       */
      $name = base64_encode($objKey)."|".$id;

      $str .= "<tr><td style='width:80px;'>\n";

      /* Only first host entry name should be editable
       */
      if($hostNameOnce){
        $hostNameOnce = false;  
        $str .="<input type='text' name='RenameHost_".$name."' value='".$objectName."'>\n";
      }else{
        $str .=$objectName;
      }  

      /* Create rest. Selectbox, icons ...
       */
      $str .="
        </td>
        <td style='width:80px;'>
          ".$this->createRecordTypeSelection($record['type'],$name)."
        </td>
        <td>
          <input type='text'  value='".$record['value']."' name='ValueSelection_".$name."' style='width:250px;'>
        </td>
        <td style='width:50px;text-align:right;'>
          <input type='image' name='AddRecord_".$name."'   src='images/crossref.png' alt='"._("Add")."' title='"._("Add")."'>
          <input type='image' name='RemoveRecord_".$name."' src='images/edittrash.png'      alt='"._("Remove")."' title='"._("Remove")."'>
        </td>
      </tr>";
    }
    $str .="</table>";
    return($str); 
  }


  /* Create selectbox with all available option types 
   */
  function createRecordTypeSelection($id,$refID){

    $str = "\n<select name='RecordTypeSelection_".$refID."'>";
    foreach($this->RecordTypes as $type => $atr) {
      if($id == $type){
        $str .="\n<option value='".$type."' selected >".strtoupper(preg_replace("/record/i","",$type))."</option>";
      }else{
        $str .="\n<option value='".$type."'>".strtoupper(preg_replace("/record/i","",$type))."</option>";
      }
    }
    $str.= "\n</select>";
    return($str);
  }


  function remove_from_parent()
  {
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
