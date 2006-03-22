<?php

class servDNSeditZoneEntries extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("zoneName");
  var $objectclasses= array("whatever");

  var $Devices      = array();
  var $InitDevices  = array();

  var $FreeHosts    = array();
  var $InitFreeHosts= array();

  var $zoneName       = "";   // ZoneName of currently edited Zone
  var $reverseName    = "";   // ReverseZone of the currently edited Zone
 
  var $RecordTypes= array();  // Possible record type. They will be set in contrucktor
                              //  and some types will be added later  (cName/Ptr)

  function servDNSeditZoneEntries ($config, $dn= NULL,$zoneName,$reverseName,$RTypes)
  {
    plugin::plugin ($config, $dn);

    /* Initialise class
     */
    $this->zoneName     = $zoneName;
    $this->reverseName  = $reverseName;
    $this->RecordTypes  = $RTypes;
  
    /* Add records types which are not allowed in zones 
     */
    $this->RecordTypes['cNAMERecord']  = "relativeDomainName";
    $this->RecordTypes['pTRRecord']    = "relativeDomainName";
    $this->RecordTypes['tXTRecord']    = "tXTRecord";
 
    /* Get ldap connection 
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Get all objects using this zone  
     * Workstations / Terminals
     */
    $res = $ldap->search("(&(objectClass=dNSZone)(|(zoneName=".$zoneName.")(zoneName=".$reverseName.".in-addr.arpa))(!(relativeDomainName=@)))");
    while($attrs = $ldap->fetch($res)){
      if(preg_match("/zoneName=/",$attrs['dn'])) continue;
      foreach($this->RecordTypes as $atr => $val){
        if((isset($attrs[$atr])) && (is_array($attrs[$atr]))){
          unset($attrs[$val]['count']);
          $dn = preg_replace("/^.*,cn/","cn",$attrs['dn']);
          $this->Devices[$dn]['RECORDS'][$atr] = $attrs[$val]; 
          $this->Devices[$dn]['Type']   = "Exists";
        }
      }
    }
   
    /* Add additional informations 
       to all catched objects ( ObjectInformations)
     */
    foreach($this->Devices as $dn => $Recs){
      $attrs = $ldap->fetch($ldap->cat($dn,array("objectClass","cn")));
      $this->Devices[$dn]['OBJECT'] = $attrs;
      $this->Devices[$dn]['OrigCn'] = $attrs['cn'][0];
    }

    /* Add Free Hand  entries
     * Free hand entries are entries which are not directly assigned
     *  to an existing network object like a WS or a Terminal.
     * They will be found in a subtree within this zone.
     * For example : 
     *  The object name is FreeHand1, 
     *                                                             zoneName=test.de,cn=server ...
     *                                // ObjectContainer
     *                                relativeDomainName=FreeHand1,zoneName=....
     * // Dns entry
     * relativeDomainName=FreeHand1,  relativeDomainName=FreeHand1,zoneName=....
     * // cName entry .... 
     * relativeDomainName=terminal12, relativeDomainName=FreeHand1,zone...
     */
    $ldap->cd ("zoneName=".$this->zoneName.",".$this->dn);
    $ldap->ls ("(&(objectClass=dNSZone)(!(relativeDomainName=@)))","zoneName=".$this->zoneName.",".$this->dn,array("*"));
   
    $GetSubInformations = array();
    while($attrs = $ldap->fetch()){

      unset($attrs[$val]['count']);

      $dn = $attrs['dn'];

      /* Split dn into relativeDomainNames 
       */
      $tmp =  split("relativeDomainName=",trim(preg_replace("/,zoneName=.*$/","\\1",$dn)));
      $tmp2 = array();
      foreach($tmp as $t){
        if(empty($t)) continue;
        $tmp2[] = preg_replace("/,$/","",$t);
      }

      /* Set object Name
       */
      $cn = $tmp2[0];
      $dn = "relativeDomainName=".$cn.",zoneName=".$this->zoneName.",".$this->dn;  

      /* Create entry 
       */
      $this->Devices[$dn]['OBJECT']           = $attrs;
      $this->Devices[$dn]['OBJECT']['cn'][0]  = $cn;//attrs['relativeDomainName'];
      $this->Devices[$dn]['OBJECT']['OrigCn'] = $cn;//attrs['relativeDomainName'][0];
      $this->Devices[$dn]['Type']             = "Free";

      $GetSubInformations [] = $dn;
    }
    
    /* Get sub informations 
       CName or pTR records are stored in a sub entry.
    
        // The base entry with aRecords/tXTrecords/....
        relativeDomainName=Keks, relativeDomainName=Keks, zoneName=....

        // And the sub entries like this one
        relativeDomainName=10.2.64.in-addr.arpa, relativeDomainName=Keks, zoneName =...
        (  They can't be stored in a single entry, so i decided to store them this way  ).
     */ 
    foreach($GetSubInformations as $dn){
      $ldap->ls("(&(objectClass=dNSZone)(!(relativeDomainName=@)))",$dn,array("*"));
      while($attrs= $ldap->fetch()){
        foreach($this->RecordTypes as $atr => $val){
          if((isset($attrs[$atr])) && (is_array($attrs[$atr]))){
            unset($attrs[$val]['count']);
            $this->Devices[$dn]['RECORDS'][$atr]    = $attrs[$val];
          }
        }
      } 
    }
    
    $this->Devices;
    $this->InitDevices = $this->Devices;
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
      if((preg_match("/^AddRecord_/",$name)) && ($once)){
        $once   = false;
        $tmp    = preg_replace("/^AddRecord_/","",$name);
        $tmp    = preg_replace("/_.*$/","",$tmp);
        $tmp2   = split("\|",$tmp);
      
        $dn     = base64_decode($tmp2[0]);
        $record = $tmp2[1];
        $numrec = $tmp2[2];        
     
        $this->Devices[$dn]['RECORDS'][$record][] = ""; 
      }
      
      /* Remove record from given dn
       */
      if((preg_match("/^RemoveRecord_/",$name)) && ($once)){
        $once   = false;
        $tmp    = preg_replace("/^RemoveRecord_/","",$name);
        $tmp    = preg_replace("/_.*$/","",$tmp);
        $tmp2   = split("\|",$tmp);
      
        $dn     = base64_decode($tmp2[0]);
        $record = $tmp2[1];
        $numrec = $tmp2[2];        
     
         unset($this->Devices[$dn]['RECORDS'][$record][$numrec]); 
  
        if(count($this->Devices[$dn]['RECORDS'][$record]) == 0){
          unset($this->Devices[$dn]['RECORDS'][$record]);
          if(count($this->Devices[$dn]['RECORDS']) ==0){
            unset($this->Devices[$dn]);
          }
        }

      }
    
      /* Don't know how i should implement this ...
       */
      if((preg_match("/^UserRecord_/",$name)) && ($once)){
        $once = false;
        $tmp['RECORDS']['aRecord'][0] =  'empty';
        $tmp['Type']                  =  'Free';
        $tmp['OBJECT']['cn'][0]       =  'Free';
        $this->Devices[] = $tmp;
      }
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $table = "";
    foreach($this->Devices as $key => $dev){
      $table .= $this->generateRecordConfigurationRow($key);
    }

    $smarty->assign("table",$table);;
    $display.= $smarty->fetch(get_template_path('servDNSeditZoneEntries.tpl', TRUE));
    return($display);
  }
  

  /* Create html table out of given entry 
   */
  function generateRecordConfigurationRow($obj_dn){

    /* get some basic inforamtions 
     */
    $obj        = $this->Devices[$obj_dn];
    $objectName = $obj['OBJECT']['cn'][0];

    /* Set title 
     */
    if($obj['Type'] == "Free"){   
      $str = "<h3>".sprintf(_("Settings for free host entry # %s"),($obj_dn +1))." : </h3>";
    }else{
      $str = "<h3>".sprintf(_("Settings for '%s'"),$obj_dn)." : </h3>";
    }
    
    $hostNameOnce = true;
    /* Walk through all defined records 
     */
    $str.= "<table cellspacing='0' cellpadding='0'>";
    foreach($obj['RECORDS'] as $id => $types){

      /* Create table
       */
      foreach($types as $key => $type){

        /* Create unique post name
         */
        $name = base64_encode($obj_dn)."|".$id."|".$key;

        $str .= "<tr>
          <td style='width:80px;'>";
          
        if($hostNameOnce){
          $hostNameOnce = false;  
          $str .="<input type='text' name='RenameHost_".$name."' value='".$objectName."'>";
        }else{
          $str .=$objectName;
        }  
      
        $str .="
          </td>
          <td style='width:80px;'>
            ".$this->createRecordTypeSelection($id,$name)."
          </td>
          <td>
            <input type='text'  value='".$type."' name='ValueSelection_".$name."' style='width:250px;'>
          </td>
          <td style='width:30px;text-align:right;'>
            <input type='image' name='AddRecord_".$name."'   src='images/crossref.png' alt='"._("Add")."' title='"._("Add")."'>
          </td>
          <td style='width:60px;text-align:right;'>
            <input type='image' name='UserRecord_".$name."'   src='images/select_default.png' alt='"._("New")."' title='"._("New")."'>
            <input type='image' name='RemoveRecord_".$name."' src='images/edittrash.png'      alt='"._("Remove")."' title='"._("Remove")."'>
          </td>
         </tr>";
      }
    }
      $str .="</table>";
    return($str); 
  }

  /* Create selectbox with all available option types 
   */
  function createRecordTypeSelection($id,$refID,$isFreeRecord =false){
    if(!$isFreeRecord){
      $str = "\n<select name='RecordTypeSelection_".$refID."'>";
    }else{
      $str = "\n<select name='FreeRecordTypeSelection_".$refID."'>";
    }
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

  function save_object()
  {
    /* Save all form fields 
     */

    /* Possible attributes posted 
     */
    $actions = array("RenameHost_","ValueSelection_");
    foreach($_POST as $name=>$value){

      foreach($actions as $act){
        /* Check if attribute was posted 
         */
        if(preg_match("/".$act."/",$name)){

          /* Extract informations out of postname 
           */
          $action = $act;
          $tmp    = preg_replace("/^".$action."/","",$name);
          $tmp    = preg_replace("/_.*$/","",$tmp); 
          $tmp2   = split("\|",$tmp);      

          $dn     = base64_decode($tmp2[0]);
          $record = $tmp2[1];
          $numrec = $tmp2[2]; 

          // entry value was psoted
          if($act == "ValueSelection_"){ 
            $this->Devices[$dn]['RECORDS'][$record][$numrec] = $value;
          }elseif($act == "RenameHost_"){
            $tmp = array();
            foreach($this->Devices as $dnn => $dev){
              if($dn == $dnn){
                continue;
              }
              if(isset($dev['OrigCn'])){
                $n1 = strtolower($dev['OrigCn']);
                $tmp[$n1] = $n1;
              } 
              $n2 = strtolower($dev['OBJECT']['cn'][0]);
              $tmp[$n2] = $n2;
            }
    
            if(!isset($tmp[strtolower($value)])){
              $this->Devices[$dn]['OBJECT']['cn'][0] = $value;
            }
          }
        }
      }
    }
    foreach($_POST as $name => $value){

      /* Check if attribute was posted
       */
      if(preg_match("/^RecordTypeSelection_/",$name)){

        /* Extract informations out of postname
         */
        $action = "RecordTypeSelection_";
        $tmp    = preg_replace("/^".$action."/","",$name);
        $tmp    = preg_replace("/_.*$/","",$tmp);
        $tmp2   = split("\|",$tmp);

        $dn     = base64_decode($tmp2[0]);
        $record = $tmp2[1];
        $numrec = $tmp2[2];

        if($record != $value){ 
          $str =  $this->Devices[$dn]['RECORDS'][$record][$numrec];
          unset($this->Devices[$dn]['RECORDS'][$record][$numrec]);
          $this->Devices[$dn]['RECORDS'][$value][] = $str;
        } 
      }
    }
  }

  /*  check something 
   */
  function check()
  {
    $message= array();
 
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
 
    foreach($this->Devices as $devDN => $device){

      /* check: if given name for Free Hand entry is valid.
       * cn exists ? invalid characters ? ...
       */
      if($device['Type'] == "Free"){
        $cn = $device['OBJECT']['cn'][0];

        $ldap->search("(cn=".$cn.")",array("cn"));
        if($ldap->count()){
          $message[] = sprintf(_("The name '%s' can't be used as free hand entry, the name is already used in this ldap tree."),$cn);
        }
      }

      /* Checking entry name
       */
      if(!is_uid($device['OBJECT']['cn'][0])){
        $message[] = sprintf(_("Entry name '%s' contains invalid characters."), $device['OBJECT']['cn'][0]);

      }      

      /* Check if we have renamed the server, hich we are currently editing.
       * reset the value and display a message.
       */
      if(strcmp($devDN,$this->dn) ==0 ){  
        $new = $device['OBJECT']['cn'][0];
        $old = $this->InitDevices[$devDN]['OBJECT']['cn'][0];
     
        if($new != $old){
          $message[] = sprintf(_("Can't rename the server '%s' to '%s', the server contains this zone."),$old,$new); 
        }
      }


      /* Renaming check for existing devices 
       */
      if(($device['Type'] == "Exists") && ($device['OrigCn'] != $device['OBJECT']['cn'][0])){
        $cn = $device['OBJECT']['cn'][0];

        $ldap->search("(cn=".$cn.")",array("cn"));
        if($ldap->count()){
          $message[] = sprintf(_("Can't rename '%s' to '%s',the destination name already exists."),$device['OrigCn'],$cn);
        }
    
        if(!is_uid($cn)){
          $message[] = sprintf(_("Can't rename '%s' to '%s',the destination name contains invalid characters."),$device['OrigCn'],$cn);
        }      
      }

      /* Check records
       */                 
      $singleEntries = array("cNAMERecord","pTRRecord");

      $tmp2 = array();
      foreach($device['RECORDS'] as $type => $entries){
   
        /* Check for multiple use of unique record types
         */
        if((in_array($type,$singleEntries)) && (count($entries) > 1)){
          $message[] = sprintf(_("The record type '%s' is a unique type and can't be defined twice."),$type);
        }

        /* Check for empty / duplicate entries in record array 
        */
        $tmp = array();
        foreach($entries as $entry){

          /* Check for empty records 
           */
          if(empty($entry)){
            $message[] = sprintf(_("There is an empty '%s' for host '%s'."),$type,$device['OBJECT']['cn'][0]);
          }

          /* Check for duplicate record entries 
           */
          if(!isset($tmp[$entry])){
            $tmp[$entry]= 'test';
          }else{
            $message[] = sprintf(_("There is a duplicate entry in '%s' for '%s'."),$type,$device['OBJECT']['cn'][0]); 
          }
        }
      }
    }
    return ($message);
  }

  function save()
  {
    /* Set all initial records to array()
       This means they will be removed from the entry
        if they won't be overwritten
     */
    foreach($this->InitDevices as $dn => $obj){
      $attrs[$dn]= array();
      foreach($obj['RECORDS'] as $rectype => $records){
        $attrs[$dn][$rectype] = array();
      }
    }

    /* Add new entries
     */
    foreach($this->Devices as $dn => $obj){
      foreach($obj['RECORDS'] as $rectype => $records){
        $attrs[$dn][$rectype] = array();
        foreach($records as $rec){
          $attrs[$dn][$rectype][] = $rec;
        } 
      }
    }

    /* Get ldap link
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);

    /* Generate 'T odo' :-)
     */
    $tmp = ($this->generate_LDAP_entries($attrs));

    /* Delete all objects which should be delted 
     */
    foreach($tmp['del'] as $key => $ntr){
      $ldap->rmdir_recursive($key);   
    }

    /* Add append new / updated entries 
     */
    foreach($tmp['addupdate'] as $key => $attrs){
      $ldap->cat($key);
      if($ldap->count() > 0){
        $ldap->cd($key);
        $ldap->modify($attrs);
      }else{
        $ldap->cd($this->config->current['BASE']);
        $ldap->cd($key);
        $ldap->add($attrs);        
      }

      /* Display errors ...
       */
      if($ldap->error != "Success"){
        echo "<br>".$ldap->error."<br>";
        print_a($attrs);
        echo $key;
      }
    }
   
    /* Rename entries 
        !! Terminals / WS / etc will be renamed too
     */
    foreach($tmp['rename'] as $old => $new){
      $ldap->cat($old);
      $this->recursive_move($old,$new);
    }
  }


  /* This function creates an array with following indexs
   * 'delete'       for all dns which should be deleted recursivly
   * 'addupdated'   Contains all entries which should be written
   * 'rename'       Contains all dns which should be renamed 
   */
  function generate_LDAP_entries($objs)
  {

    $entries = array();
    $delete  = array();    
    $rename  = array();

    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);

    /* these entries are added to base object ... 
     */
    $normal = array("mDRecord","aRecord","tXTRecord","mXRecord","hInfoRecord","mInfoRecord","aFSDBRecord","SigRecord","KeyRecord","aAAARecord","nSRecord",
        "LocRecord","nXTRecord","sRVRecord","nAPTRRecord","kXRecord","certRecord","a6Record","dSRecord","sSHFPRecord","rRSIGRecord","nSECRecord");

    /* Create t odo
     */
    foreach($objs as $dn => $recs){

      /* Check if object must be renamed 
       */
      if(isset($this->Devices[$dn]['OrigCn'])){
        if($this->Devices[$dn]['OBJECT']['cn'][0] != $this->Devices[$dn]['OrigCn']){
          $newdn = str_replace("cn=".$this->Devices[$dn]['OrigCn'], "cn=".$this->Devices[$dn]['OBJECT']['cn'][0],$this->Devices[$dn]['OBJECT']['dn']);
          $rename[$this->Devices[$dn]['OBJECT']['dn']]  =  $newdn;
          $rename["relativeDomainName=".$this->Devices[$dn]['OrigCn'].",".$newdn]  = "relativeDomainName=".$this->Devices[$dn]['OBJECT']['cn'][0].",".$newdn;
        }
      }

      /* Get basic info
       */ 
      if(isset($this->Devices[$dn])){
        $obj = $this->Devices[$dn];
      }else{
        $obj = NULL;
      }

      /* If is valid entry 
       */
      if($obj != NULL){  

        /*  Both type must be handled different 
         *  Free / Exists 
         */
        if($obj["Type"] == "Free"){
          $objectName = $obj['OBJECT']['cn'][0];
        }else{
          $objectName = $obj['OrigCn'];
        }

      
        /* If type is Free, check 
         */
        if($obj['Type'] == "Free"){

          /* Remove entry, if it was renamed 
           */
          if(isset($obj['OBJECT']['OrigCn'])){
            if($obj['OBJECT']['cn'][0] != $obj['OBJECT']['OrigCn']){
              $delete["relativeDomainName=".$obj['OBJECT']['OrigCn'].",zoneName=".$this->zoneName.",".$this->dn]="";
            }
          }

          /* Add new entry 
           */
          $dn  = "relativeDomainName=".$objectName.",zoneName=".$this->zoneName.",".$this->dn;
          $dn2 = $dn;

          $ldap->cat($dn);
          if($ldap->count() == 0){
            $entries[$dn]['objectClass']        = array("top","dNSZone");
            $entries[$dn]['zoneName']           = $this->zoneName;
            $entries[$dn]['relativeDomainName'] = $objectName;
          }
        }
      }

      /* Calculate records ... 
       */
      foreach($recs as $type =>  $rec){

        /*  Simply add normal entries 
         */
        if($obj != NULL){
          if(in_array($type,$normal)){
            $ldap->cat('relativeDomainName='.$objectName.','.$dn);

            /* Append objectFrame if object doesn't exists in ldap
             */
            if($ldap->count()  == 0) {
              $entries['relativeDomainName='.$objectName.','.$dn]['objectClass']        = array("top","dNSZone");
              $entries['relativeDomainName='.$objectName.','.$dn]['zoneName']           = $this->zoneName;
              $entries['relativeDomainName='.$objectName.','.$dn]['relativeDomainName'] = $objectName;
            }
            $entries['relativeDomainName='.$objectName.','.$dn][$type] = $rec ;
          }

          /*  Special handling for cNAMERecords 
           */
          if($type == "cNAMERecord"){

            $useDn = $dn;
            if($obj['Type'] == "Free"){
              $useDn = $dn2;
            }
             
            if(isset($this->InitDevices[$useDn]['RECORDS']['cNAMERecord'])){
              foreach($this->InitDevices[$useDn]['RECORDS']['cNAMERecord'] as $warmal){
                $delete['relativeDomainName='.$warmal.','.$useDn] = "";
              }
            }
            foreach($rec as $r){
              if(!empty($r)){
                $entries['relativeDomainName='.$r.','.$useDn]['objectClass']        = array("top","dNSZone");
                $entries['relativeDomainName='.$r.','.$useDn]['zoneName']           = $this->zoneName;
                $entries['relativeDomainName='.$r.','.$useDn]['cNAMERecord']        = $objectName;
                $entries['relativeDomainName='.$r.','.$useDn]['relativeDomainName'] = $r;
              }
            }
          }

          /* Special handling for ptrrecord
           */ 
          if($type == "pTRRecord"){
          
            $useDn = $dn;
            if($obj['Type'] == "Free"){
              $useDn = $dn2;
            }

            if(isset($this->InitDevices[$dn]['RECORDS']['pTRRecord'])){
              foreach($this->InitDevices[$dn]['RECORDS']['pTRRecord'] as $warmal){
                $delete['relativeDomainName='.$warmal.','.$useDn] = "";
              }
            }
            foreach($rec as $r){
              if(!empty($r)){
                $entries['relativeDomainName='.$r.','.$useDn]['pTRRecord']          = $objectName;
                $entries['relativeDomainName='.$r.','.$useDn]['zoneName']           = $this->reverseName.".in-addr.arpa";
                $entries['relativeDomainName='.$r.','.$useDn]['relativeDomainName'] = $r;
                $entries['relativeDomainName='.$r.','.$useDn]['objectClass']        = array("top","dNSZone");
              }
            }
          }
        }
        if($obj == NULL){
          $obj = $this->InitDevices[$dn];
          if($obj['Type'] == "Free"){
            $delete['relativeDomainName='.$objectName.','.$dn] = "";
          }else{
            $ldap->cd($dn);
            $ldap->ls("(&(objectClass=dNSZone)(!(relativeDomainName=@)))",$dn,array("relativeDomainName"));
            while($attrs = $ldap->fetch()){
              $delete[$attrs['dn']] = "";
            }
          }
        }    

      }
    }
    $ret = array("del"=> $delete , "addupdate"=> $entries, "rename"=> $rename);
    return($ret);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
