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

  var $Devices    = array();
  var $InitDevices = array();
  var $zoneName       = ""; 
  var $reverseName    = ""; 
  var $RecordTypes= array();

  function servDNSeditZoneEntries ($config, $dn= NULL,$zoneName,$reverseName,$RTypes)
  {
    plugin::plugin ($config, $dn);

    $this->zoneName     = $zoneName;
    $this->reverseName  = $reverseName;
    $this->RecordTypes  = $RTypes;
    $this->RecordTypes['cNAMERecord']  = "relativeDomainName";

 
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Get all objects it this zone ... 
     */
    $res = $ldap->search("(&(objectClass=dNSZone)(|(zoneName=".$zoneName.")(zoneName=".$reverseName.".in-addr.arpa))(!(relativeDomainName=@)))");
    while($attrs = $ldap->fetch($res)){
      foreach($this->RecordTypes as $atr => $val){
        if((isset($attrs[$atr])) && (is_array($attrs[$atr]))){
          unset($attrs[$val]['count']);
          $dn = preg_replace("/^.*,cn/","cn",$attrs['dn']);
          $this->Devices[$dn]['RECORDS'][$atr] = $attrs[$val]; 
        }
      }
    }
    
    /* Add additional informations 
       to all catched objects 
     */
    foreach($this->Devices as $dn => $Recs){
      $this->Devices[$dn]['OBJECT']  = $ldap->fetch($ldap->cat($dn,array("objectClass","cn")));
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
      }
    
      /* Don't know how i should implement this ...
       */
      if((preg_match("/^UserRecord_/",$name)) && ($once)){
        $once   = false;
        $tmp    = preg_replace("/^UserRecord_/","",$name);
        $tmp    = preg_replace("/_.*$/","",$tmp);
        $tmp2   = split("\|",$tmp);

        $dn     = base64_decode($tmp2[0]);
        $record = $tmp2[1];
        $numrec = $tmp2[2];

        $this->FreeDevices[$dn]['RECORDS']['User'][$numrec] = ""; 
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

    // set title 
    $str = "<h3>".sprintf(_("Settings for '%s'"),$obj_dn)." : </h3>";

    /* Walk through all defined records 
     */
    foreach($obj['RECORDS'] as $id => $types){

      /* Create table
       */
      $str.= "<table cellspacing='0' cellpadding='0'>";
      foreach($types as $key => $type){

        /* Create unique post name
         */
        $name = base64_encode($obj_dn)."|".$id."|".$key;

        $str .= "<tr>
          <td style='width:80px;'>
            ".$objectName."
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
      $str .="</table>";
    }
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

  function save_object()
  {
    /* Save all form fields 
     */
    
    /* Possible attributes posted 
     */
    $arr = array("RecordTypeSelection_","ValueSelection_");
   
    foreach($_POST as $name=>$value){
      
      foreach($arr as $type)
    
        /* Check if attribute was posted 
         */
        if(preg_match("/".$type."/",$name)){
        
          /* Extract informations out of postname 
           */
          $action = $type;
          $tmp    = preg_replace("/^".$action."/","",$name);
          $tmp    = preg_replace("/_.*$/","",$tmp); 
          $tmp2   = split("\|",$tmp);      

          $dn     = base64_decode($tmp2[0]);
          $record = $tmp2[1];
          $numrec = $tmp2[2]; 

          /* Check which type of post was posted 
           */
        
          // checkbox was posted 
          if($action == "RecordTypeSelection_"){
            if($record != $value){ 
              $str =  $this->Devices[$dn]['RECORDS'][$record][$numrec];
              unset($this->Devices[$dn]['RECORDS'][$record][$numrec]);
              $this->Devices[$dn]['RECORDS'][$value][] = $str;
            }
          } 

          // entry value was psoted 
          if($action == "ValueSelection_"){
            $this->Devices[$dn]['RECORDS'][$record][$numrec] = $value;
          }
        }
    }
  }

  /*  check something 
   */
  function check()
  {
    $message= array();
    // Nothing right now
    return ($message);
  }

  function save()
  {
    /* Set all initial records to array()
       This means they will be removed from the entry
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
      $ldap->rmdir($key);      
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
      }
    }
  }


  function generate_LDAP_entries($objs)
  {
    $entries = array();
    $delete  = array();    

    /* these entries are added to base object ... 
     */
    $normal = array("mDRecord","aRecord","tXTRecord","mXRecord","hInfoRecord","mInfoRecord","aFSDBRecord","SigRecord","KeyRecord","aAAARecord","nSRecord",
        "LocRecord","nXTRecord","sRVRecord","nAPTRRecord","kXRecord","certRecord","a6Record","dSRecord","sSHFPRecord","rRSIGRecord","nSECRecord");

    /* Create t odo
     */
    foreach($objs as $dn => $recs){
 
      /* Get basic info
       */ 
      $obj        = $this->Devices[$dn];
      $objectName = $obj['OBJECT']['cn'][0];

      /* Calculate records ... 
       */
      foreach($recs as $type =>  $rec){
      
        /*  Simply add normal entries 
         */
        if(in_array($type,$normal)){
          $entries['relativeDomainName='.$objectName.','.$dn][$type] = $rec ;
        }

        /*  Special handling for cNAMERecords 
         */
        if($type == "cNAMERecord"){
          if(isset($this->InitDevices[$dn]['RECORDS']['cNAMERecord'])){
            foreach($this->InitDevices[$dn]['RECORDS']['cNAMERecord'] as $warmal){
              $delete['relativeDomainName='.$warmal.','.$dn] = "";
            }
          }
          foreach($rec as $r){
            if(!empty($r)){
            $entries['relativeDomainName='.$r.','.$dn]['cNAMERecord']        = $objectName;
            $entries['relativeDomainName='.$r.','.$dn]['relativeDomainName'] = $r;
            $entries['relativeDomainName='.$r.','.$dn]['objectClass']        = array("top","dNSZone");
            $entries['relativeDomainName='.$r.','.$dn]['zoneName']           = $this->zoneName;
            }
          }
        }

        /* Special handling for ptrrecord
         */ 
        if($type == "pTRRecord"){
          if(isset($this->InitDevices[$dn]['RECORDS']['pTRRecord'])){
            foreach($this->InitDevices[$dn]['RECORDS']['pTRRecord'] as $warmal){
              $delete['relativeDomainName='.$warmal.','.$dn] = "";
            }
          }
          foreach($rec as $r){
            if(!empty($r)){
              $entries['relativeDomainName='.$r.','.$dn]['pTRRecord']          = $objectName;
              $entries['relativeDomainName='.$r.','.$dn]['zoneName']           = $this->reverseName.".in-addr.arpa";
              $entries['relativeDomainName='.$r.','.$dn]['relativeDomainName'] = $r;
              $entries['relativeDomainName='.$r.','.$dn]['objectClass']        = array("top","dNSZone");
            }
          }
        } 
      }
    }

    return(array("del"=> $delete , "addupdate"=> $entries));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
