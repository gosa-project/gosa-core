<?php

class servdnseditZone extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes     = array("zoneName","ReverseZone","dNSTTL","dNSClass",
      "sOAprimary","sOAmail","sOAserial","sOArefresh","sOAretry","sOAexpire","sOAttl"); 
  var $objectclasses  = array("whatever");

  var $RecordTypes = array();

  var $ReverseZone              = "";
  var $ReverseDN                = "";
  var $zoneName                 = "";
  var $dNSTTL                   = "7200";
  var $dNSClass                 = "IN";
  var $status                   = "new";

  var $sOAprimary               = "";
  var $sOAmail                  = "";
  var $sOAserial                = "";
  var $sOArefresh               = "3600";
  var $sOAretry                 = "1800";
  var $sOAexpire                = "720000";
  var $sOAttl                   = "6400";

  var $Records                  = array();

  var $InitiallyZoneName        = "";
  var $InitiallyReverseZone     = "";
  var $isNew                    = true;

  function servdnseditZone ($config, $dn= NULL,$recordtypes,$attrs = array())
  {
    plugin::plugin ($config, $dn);

    /* All types with required attrs */
    $this->RecordTypes = $recordtypes; 

    if(!count($attrs)){
      $this->InitiallyZoneName      = "";
      $this->InitiallyReverseZone   = "";
      $this->isNew                  = true;
      $this->sOAserial = date("Ymd")."1";
    }else{
      $this->InitiallyZoneName      = $attrs['zoneName'];
      $this->InitiallyReverseZone   = $attrs['ReverseZone'];
      $this->isNew                  = false;

      foreach($this->attributes as $value){
        $this->$value = $attrs[$value];
      }
      if(isset($attrs['Records'])){
        $this->Records = $attrs['Records']; 
      }else{
        $this->Records = array();
      }
      $str = date("Ymd");
      if(preg_match("/^".$str."/",$this->sOAserial)){
        $this->sOAserial = $this->sOAserial + 1;
      }else{
        $this->sOAserial = date("Ymd")."01";
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Handle Post events */
    $once = true;
    foreach($_POST as $name => $value){

      /* Delete record if requested */
      if((preg_match("/RemoveRecord_/",$name))&&($once)){
        $once = false;
        $id= preg_replace("/RemoveRecord_/","",$name);
        if($this->Records[$id]['status']!= "new"){
          $this->Records[$id]['status']= "deleted";
        }else{
          unset($this->Records[$id]);
        }
      }
    }

    /* Add new Zonerecord */
    if(isset($_POST['AddNewRecord'])){
      $this->Records[] = array("type"=>"aRecord","inittype"=>"","value"=>"","status"=>"new");
    }

    /* Fill in values */
    foreach($this->attributes as $name){
      $smarty->assign($name,$this->$name);
    }

    /* Assign records list */
    $smarty->assign("records",$this->generateRecordsList());

    /* Display tempalte */
    $display.= $smarty->fetch(get_template_path('servdnseditzone.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
  }

  /* Save data to object */
  function save_object()
  {
    //plugin::save_object();
    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->$attr = $_POST[$attr];
      }
    }

    foreach($this->Records as $id => $value){  
      if(isset($_POST['RecordTypeSelectedFor_'.$id])){
        $this->Records[$id]['type'] = $_POST['RecordTypeSelectedFor_'.$id];
      }
      if(isset($_POST['RecordValue_'.$id])){
        $this->Records[$id]['value'] = $_POST['RecordValue_'.$id];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    /* Check if zoneName is already in use */
    $usedZones = $this->getUsedZoneNames();
    if(($this->isNew == true)||($this->zoneName  != $this->InitiallyZoneName)||($this->ReverseZone != $this->InitiallyReverseZone)){
      if((isset($usedZones[$this->zoneName]))&&($this->zoneName  != $this->InitiallyZoneName)){
        $message[] =_("This zoneName is already in use");
      }
      if((in_array($this->ReverseZone,$usedZones))&&($this->ReverseZone != $this->InitiallyReverseZone)){
        $message[] =_("This reverse zone is already in use");
      }
    }
    return ($message);
  }

  function getUsedZoneNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@)(zoneName=*))",array("zoneName","tXTRecord"));
    while($attr = $ldap->fetch()){
      if(preg_match("/in-addr\.arpa/",$attr['zoneName'][0])){
        $zn = preg_replace("/zoneName\=/","",$attr['tXTRecord'][0]);
        $ret[$zn] = $this->FlipIp(preg_replace("/\.in-addr\.arpa/","",$attr['zoneName'][0]));
      }else{
        $ret[$attr['zoneName'][0]]="";
      }
    }
    return($ret);
  }

 
  function FlipIp($ip)
  {
    $tmp = array_reverse(split("\.",$ip));
    $new = "";
    foreach($tmp as $section){
      $new .= $section.".";
    }
    return(preg_replace("/.$/","",$new));
  }


  /* Save to LDAP */
  function save()
  {
    $ret =array();
    foreach($this->attributes as $name){
      $ret[$name] = $this->$name;
    }
    $ret['Records'] = $this->Records; 
    return($ret);
  }

  function generateRecordsList($changeStateForRecords="")
  {
    $changeStateForRecords = "";

    $str = "<table summary=''>";
    foreach($this->Records as $key => $entry){
      if($entry['status'] == "deleted") continue;

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
      "    <td colspan=2></td><td>".
      "      <input type='submit' value='"._("Add")."' name='AddNewRecord'>".
      "    </td>".
      "  </tr>".
      "</table>";
    return($str);
  }

  function generateRecordListBox($selected,$name)
  {
    $str = "<select name='".$name."' id='".$name."'>";
    foreach($this->RecordTypes as $type => $value){
      $use = "";
      if($type == $selected){
        $use = " selected ";
      }
      $str.="\n <option value='".$type."' ".$use.">".$type."</option>";
    }
    $str.="</select>";
    return($str);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
