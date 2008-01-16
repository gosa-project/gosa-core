<?php

class servdnseditZone extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes     = array("zoneName","ReverseZone","dNSClass",
      "sOAprimary","sOAmail","sOAserial","sOArefresh","sOAretry","sOAexpire","sOAttl"); 
  var $objectclasses  = array("whatever");

  var $RecordTypes              = array();

  var $ReverseZone              = "";
  var $zoneName                 = "";
  var $dNSClass                 = "IN";

  var $sOAprimary               = "";
  var $sOAmail                  = "";
  var $sOAserial                = "";
  var $sOArefresh               = "3600";
  var $sOAretry                 = "1800";
  var $sOAexpire                = "720000";
  var $sOAttl                   = "6400";

  var $Records                  = array();
  var $mXRecords                = array();

  var $OldZoneName              = ""; // To detect changes made with this edit
  var $OldReverseZone           = "";

  var $InitialReverseZone       = "";
  var $InitialzoneName          = "";
  var $NetworkClass                = "A" ; // One out of A,B,C

  var $dialog                   = false;

  var $zoneEditor               = NULL;

  var $isNew                    = true;

  var $ZoneObject               = array();

  function servdnseditZone (&$config, $dn= NULL,$attrs = array())
  {
    plugin::plugin ($config, $dn);

    /* All types with required attrs */
    $this->RecordTypes = DNS::getDnsRecordTypes(true); 

    if(!count($attrs)){
      $this->OldZoneName        = "";
      $this->OldReverseZone     = "";
      $this->isNew              = true;
      $this->sOAserial          = date("Ymd")."1";
      
      $this->InitialzoneName    = "";//$attrs['InitialzoneName'];
      $this->InitialReverseZone = "";//$attrs['InitialReverseZone'];
    }else{
      $this->ZoneObject         = $attrs;

      if(isset($attrs['zoneEditor'])){
        $this->zoneEditor         = $attrs['zoneEditor'];
      }
      $this->OldZoneName        = $attrs['zoneName'];
      $this->OldReverseZone     = $attrs['ReverseZone'];

      $this->InitialzoneName    = $attrs['InitialzoneName'];
      $this->InitialReverseZone = $attrs['InitialReverseZone'];

      $this->isNew                  = false;

      foreach($this->attributes as $value){
        $this->$value = $attrs[$value];
      }

      $this->sOAmail            = preg_replace("/\./","@",$this->sOAmail,1);
      $this->sOAmail            = preg_replace("/\.$/","",$this->sOAmail);
      $this->sOAprimary         = preg_replace("/\.$/","",$this->sOAprimary);
      $this->zoneName           = preg_replace("/\.$/","",$this->zoneName);

      if(isset($attrs['RECORDS'])){
        $this->Records = $attrs['RECORDS']; 

        $tmp2 = array();
        $usedPrio = array();
        foreach($this->Records as $key => $rec){
          if($rec['type'] == "mXRecord"){
            $tmp = split(" ",$rec['value']);
            $rec['value'] = $tmp[1];
            $tmp2[$tmp[0]] = $rec;
            unset($this->Records[$key]);
          }
          if($rec['type'] == "nSRecord"){
            unset($this->Records[$key]);
          }
        }
        if(count($tmp2) != 0){
          reset($tmp2);
          ksort($tmp2);
        }
        $this->mXRecords = $tmp2;
      }else{
        $this->mXRecords  = array();
        $this->Records    = array();
      }

      $str = date("Ymd");
      if(preg_match("/^".$str."/",$this->sOAserial)){
        $this->sOAserial = $this->sOAserial + 1;
      }else{
        $this->sOAserial = date("Ymd")."01";
      }
    }

    /* Detect Network class */
    if(!empty($this->ReverseZone)){

      $dots = count(split("\.",$this->ReverseZone));
      if($dots == 1){
        $this->NetworkClass = "A";  
        $this->ReverseZone .= ".0.0.0"; 
      }elseif($dots == 2){
        $this->NetworkClass = "B";  
        $this->ReverseZone .= ".0.0"; 
      }else{
        $this->NetworkClass = "C";  
        $this->ReverseZone .= ".0"; 
      }
    }
  }

  /* TRansports the geiven Arraykey one position up*/
  function ArrayUp($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $atr ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == 1)||($pos >$cn))){
      $before = array_slice($attrs,0,($pos-2));
      $mitte  = array_reverse(array_slice($attrs,($pos-2),2));
      $unten  = array_slice($attrs,$pos);
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* TRansports the geiven Arraykey one position up*/
  function ArrayDown($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $atr ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == $cn))){
      $before = array_slice($attrs,0,($pos-1));
      $mitte  = array_reverse(array_slice($attrs,($pos-1),2));
      $unten  = array_slice($attrs,($pos+1));
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }

  /* Combine new array */
  function combineArrays($ar0,$ar1,$ar2)
  {
    $ret = array();
    if(is_array($ar0))
    foreach($ar0 as $ar => $a){
        $ret[]=$a;
    }
    if(is_array($ar1))
    foreach($ar1 as $ar => $a){
        $ret[]=$a;
    }
    if(is_array($ar2))
    foreach($ar2 as $ar => $a){
        $ret[]=$a;
    }
    return($ret);
  }
  
  function getpos($atr,$attrs)
  {
    $i = 0;
    foreach($attrs as $attr => $name)    {
      $i++;
      if($attr == $atr){
        return($i);
      }
    }
    return(-1);
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();



    /* Fill templating stuff */
    $smarty= get_smarty();
    $ui = get_userinfo();

    $smarty->assign("ACLs",$this->parent->getacl(""));
    $display= "";

    /* Open Zone Entry Edit Dialog
     */
    if(!count($this->ZoneObject)){
      $smarty->assign("AllowZoneEdit" , false);
    }else{
      $smarty->assign("AllowZoneEdit" , true);
      if(isset($_POST['EditZoneEntries'])){
        if($this->zoneEditor == NULL){
          $this->zoneEditor= new servDNSeditZoneEntries($this->config,$this->dn,$this->ZoneObject);
          $this->zoneEditor->parent = $this;
        }
        $this->dialog = $this->zoneEditor;
      }
    }

    /* Save Zone Entry Edit Dialog
     */
    if(isset($_POST['SaveZoneEntryChanges'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())){
        $msgs = $this->dialog->check();
        foreach($msgs as $msg){
          print_red($msg);
        }
      }else{
        $this->zoneEditor = clone $this->dialog;
        $this->dialog = FALSE;
#       $rev = DNS::FlipIp(DNS::getNameFromMix($this->InitialReverseZone)).".in-addr.arpa";
#       $for = DNS::getNameFromMix($this->InitialzoneName);
#      
#       $this->parent->handle_post_events("modify",array("dn" => $this->dn,"zoneName" => $rev));
#       $this->parent->handle_post_events("modify",array("dn" => $this->dn,"zoneName" => $for));
#       $this->dialog = false;
      }
    }

    /* Cancel Zone Entrie Edit Dialog
    */
    if(isset($_POST['CancelZoneEntryChanges'])){
      $this->dialog = false;
    }

    /* Display any type of open dialogs 
     */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    $once =true;
    foreach($_POST as $name => $value){
      if((preg_match("/^MXup_/",$name)) && ($once)){
        $once = false;

        $id = preg_replace("/^MXup_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
    
        $this->mXRecords = $this->ArrayUp(($id+1),$this->mXRecords);
      }
      if((preg_match("/^MXdown_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^MXdown_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
  
        $this->mXRecords = $this->ArrayDown(($id+1),$this->mXRecords);
      }
      if((preg_match("/^MXdel_/",$name)) && ($once)){
        $once = false;
        
        $id = preg_replace("/^MXdel_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $id = base64_decode($id);
        
        unset($this->mXRecords[$id]);

        $tmp  =array();
        foreach($this->mXRecords as $entry){
          $tmp[] = $entry;
        }
 
        $this->mXRecords = $tmp; 
      }
    }

    if((isset($_POST['AddMXRecord'])) && (!empty($_POST['StrMXRecord']))){
      $this->mXRecords[] = array("type"=>"mXRecord","value"=>trim($_POST['StrMXRecord']));      
    }

    /* Handle Post events */
    $once = true;
    foreach($_POST as $name => $value){

      /* Delete record if requested */
      if((preg_match("/RemoveRecord_/",$name))&&($once)){
        $once = false;
        $id= preg_replace("/RemoveRecord_/","",$name);
        unset($this->Records[$id]);
      }
    }

    /* Add new Zonerecord */
    if(isset($_POST['AddNewRecord'])){
      $this->Records[] = array("type"=>"aRecord","value"=>"");
    }

    /* Fill in values */
    foreach($this->attributes as $name){
      $smarty->assign($name,$this->$name);
    }


    $div = new divSelectBox("MxRecords");
    $div->setHeight(120);
    $recs = $this->mXRecords;

    $oneup    = "<input name='MXup_%s'    type='image' src='images/sort_up.png'    title='"._("Up")."'      class='center'>&nbsp;"; 
    $onedown  = "<input name='MXdown_%s'  type='image' src='images/sort_down.png'  title='"._("Down")."'    class='center'>&nbsp;"; 
    $onedel   = "<img src='images/empty.png' width='20' class='center'>
                 <input name='MXdel_%s'   type='image' src='images/edittrash.png'  title='"._("Delete")."'  class='center'>"; 

    foreach($recs as $key => $rec){
      $div ->AddEntry(array(
            array("string"=>$rec['value']),
/*            array("string"=>$key,
                  "attach"=>"style='width:20px;'"),*/
            array("string"=>str_replace("%s",base64_encode($key),$oneup.$onedown.$onedel),
                  "attach"=>"style='width:70px;border-right:0px;'")
            ));
    }

    /* Assign records list */
    $smarty->assign("NotNew", false);
    $smarty->assign("Mxrecords",  $div->DrawList());
    $smarty->assign("records"  ,  $this->generateRecordsList());
    $smarty->assign("NetworkClass",  $this->NetworkClass);
    $smarty->assign("NetworkClasses",  array("A"=>"255.0.0.0 (Class A)","B"=>"255.255.0.0 (Class B)","C"=>"255.255.255.0 (Class C)"));

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

     if(isset($_POST['NetworkClass'])){
       $this->NetworkClass = $_POST['NetworkClass'];
     }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
        
    /* Check if zoneName is already in use */
    $usedZones = $this->getUsedZoneNames();
    if(($this->isNew == true)||($this->zoneName  != $this->InitialzoneName)||($this->ReverseZone != $this->InitialReverseZone)){
    /*  if((isset($usedZones[$this->zoneName]))&&($this->zoneName  != $this->InitialzoneName)){
        $message[] =_("This zoneName is already in use");
      }
      if((in_array($this->ReverseZone,$usedZones))&&($this->ReverseZone != $this->InitialReverseZone)){
        $message[] =_("This reverse zone is already in use");
      }*/
    }

    if(empty($this->zoneName)){
      $message[] =sprintf(_("Please choose a valid zone name."));
    }

    if(empty($this->ReverseZone)){
      $message[] =sprintf(_("Please choose a valid reverse zone name."));
    }

    if($this->zoneName != strtolower($this->zoneName)){
      $message[] = _("Only lowercase strings are allowed as zone name.");
    }

    if(!is_numeric($this->sOAserial)){
      $message[] = _("Please specify a numeric value for serial number.");
    }

    if(!is_numeric($this->sOArefresh)){
      $message[] = _("Please specify a numeric value for refresh.");
    }

    if(!is_numeric($this->sOAttl)){
      $message[] = _("Please specify a numeric value for ttl.");
    }

    if(!is_numeric($this->sOAexpire)){
      $message[] = _("Please specify a numeric value for expire.");
    }

    if(!is_numeric($this->sOAretry)){
      $message[] = _("Please specify a numeric value for retry.");
    }

    foreach($this->Records as $name => $values){
      /* only lower-case is allowed in record entries ... */
      if($values['value'] != strtolower($values['value'])){
        $message[] = sprintf(_("Only lowercase is allowed, please check your '%ss'."),$values['type']);
      }
    }

    /* Check class for given Zone Address */
    $addr = preg_replace("/^[^\/]*+\//","",$this->ReverseZone);
  
    /* Check for valid&complete IP address */
    if(!tests::is_ip($addr)){
      $message[] = _("The given network address is not a valid, please specify a valid IP address.");
    }
 
    /* Check if given address matches selected network class */
    switch($this->NetworkClass){
      case 'A': { 
                  if(!preg_match("/^[0-9]*\.0\.0\.0$/",$addr)){
                    $message[] = sprintf(_("The specified network address is not matching with the specified zone class, try it this way x.0.0.0"));
                  }
                }
                break;
      case 'B': {
                  if(!preg_match("/^[0-9]*\.[0-9]*\.0\.0$/",$addr)){
                    $message[] = sprintf(_("The specified network address is not matching with the specified zone class, try it this way x.x.0.0"));
                  }
                }
                break;
      case 'C': {
                  if(!preg_match("/^[0-9]*\.[0-9]*\.[0-9]*\.0$/",$addr)){
                    $message[] = sprintf(_("The specified network address is not matching with the specified zone class, try it this way x.x.x.0"));
                  }
                }
                break;
      default : $message[] =sprintf(_("The given network class '%s' is not valid."),$this->NetworkClass);
    }

    return ($message);
  }

  /* This funtion returns all used Zonenames */
  function getUsedZoneNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(relativeDomainName=@)(zoneName=*))",array("zoneName","tXTRecord"));
    while($attr = $ldap->fetch()){
      if(preg_match("/in-addr\.arpa/",$attr['zoneName'][0])){
        if(isset($attr['tXTRecord'][0])){
          $zn = preg_replace("/zoneName\=/","",$attr['tXTRecord'][0]);
          $ret[$zn] =DNS::FlipIp(preg_replace("/\.in-addr\.arpa/","",$attr['zoneName'][0]));
        }
      }else{
        $ret[$attr['zoneName'][0]]="";
      }
    }
    return($ret);
  }

  /* Save to LDAP */
  function save()
  {
    $ret =array();
    foreach($this->attributes as $name){
      $ret[$name] = $this->$name;
    }

    /* Create mx records 
     */
    foreach($this->mXRecords as $key => $rec){
      $rec['value']= $key." ".$rec['value'];
      $this->Records [] = $rec;
    }

  
    $ret['RECORDS'] = $this->Records; 
 
    switch($this->NetworkClass){
      case 'C' : $ret['ReverseZone']= preg_replace("/\.[0-9]*$/","",$this->ReverseZone);break;
      case 'B' : $ret['ReverseZone']= preg_replace("/\.[0-9]*\.[0-9]*$/","",$this->ReverseZone);break;
      case 'A' : $ret['ReverseZone']= preg_replace("/\.[0-9]*\.[0-9]*\.[0-9]*$/","",$this->ReverseZone);break;
      default : trigger_error("Invalid network class given '".$this->NetworkClass."'");
    }

    $ret['InitialReverseZone']=  $this->InitialReverseZone;
    $ret['InitialzoneName']   =  $this->InitialzoneName;

    $ret['sOAmail']            = preg_replace("/\@/",".",$this->sOAmail);

    foreach(array("sOAprimary","zoneName","sOAmail") as $attr){
      if(!preg_match("/\.$/",$ret[$attr])){
        if(!tests::is_ip($ret[$attr])){
          $ret[$attr] = $ret[$attr].".";
        }
      }
    }

    $ret['RECORDS'][] = array("type" => "nSRecord","value" => $ret['sOAprimary']) ;

    $ret['zoneEditor'] = $this->zoneEditor;
    return($ret);
  }

  
  /* This function generate a table row for each used record.
     This table row displays the recordtype in a select box
      and the specified value for the record, and a remove button.
     The last element of the table also got an 'add' button.
   */
  function generateRecordsList($changeStateForRecords="")
  {
    $changeStateForRecords = "";

    $str = "<table summary=''>";
    foreach($this->Records as $key => $entry){

      if($entry['type'] == "mXRecord") continue;
      
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

  /* This function generates a select box out of $this->RecordTypes options.
     The Parameter $selected is used to predefine an attribute.
     $name is used to specify a post name
   */
  function generateRecordListBox($selected,$name)
  {
    $str = "<select name='".$name."' id='".$name."'>";
    foreach($this->RecordTypes as $type => $value){

      if(preg_match("/^mXRecord$/i",$value)) continue;

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
