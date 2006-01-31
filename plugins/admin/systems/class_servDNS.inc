<?php

class servdns extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("zoneName","relativeDomainName","dNSTTL","dNSClass","sOARecord"); 
  var $objectclasses= array("whatever");

  var $RecordTypes = array();
  var $usedRecords = array();

  var $Zones = array();
  var $editMode = false;

  function servdns ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    
    $RecordTypes['ARecord']     ['name']  ="A Record";
    $RecordTypes['ARecord']     ['type']  ="string";
    $RecordTypes['MDRecord']    ['name']  ="MD Record";
    $RecordTypes['MDRecord']    ['type']  ="string";
    $RecordTypes['MXRecord']    ['name']  ="MX Record";
    $RecordTypes['MXRecord']    ['type']  ="string";
    $RecordTypes['NSRecord']    ['name']  ="NS Record";
    $RecordTypes['NSRecord']    ['type']  ="string";
    $RecordTypes['CNAMERecord'] ['name']  ="CNAME Record";
    $RecordTypes['CNAMERecord'] ['type']  ="string";
    $RecordTypes['PTRRecord']   ['name']  ="PTR Record";
    $RecordTypes['PTRRecord']   ['type']  ="string";
    $RecordTypes['HINFORecord'] ['name']  ="HINFO Record";
    $RecordTypes['HINFORecord'] ['type']  ="string";
    $RecordTypes['MINFORecord'] ['name']  ="MINFO Record";
    $RecordTypes['MINFORecord'] ['type']  ="string";
    $RecordTypes['TXTRecord']   ['name']  ="TXT Record";
    $RecordTypes['TXTRecord']   ['type']  ="string";
    $RecordTypes['AFSDBRecord'] ['name']  ="AFSDB Record";
    $RecordTypes['AFSDBRecord'] ['type']  ="string";
    $RecordTypes['SIGRecord']   ['name']  ="SIG Record";
    $RecordTypes['SIGRecord']   ['type']  ="string";
    $RecordTypes['KEYRecord']   ['name']  ="KEY Record";
    $RecordTypes['KEYRecord']   ['type']  ="string";
    $RecordTypes['AAAARecord']  ['name']  ="AAAA Record";
    $RecordTypes['AAAARecord']  ['type']  ="string";
    $RecordTypes['LOCRecord']   ['name']  ="LOC Record";
    $RecordTypes['LOCRecord']   ['type']  ="string";
    $RecordTypes['NXTRecord']   ['name']  ="NXT Record";
    $RecordTypes['NXTRecord']   ['type']  ="string";
    $RecordTypes['SRVRecord']   ['name']  ="SRV Record";
    $RecordTypes['SRVRecord']   ['type']  ="string";
    $RecordTypes['NAPTRRecord'] ['name']  ="NAPTR Record";
    $RecordTypes['NAPTRRecord'] ['type']  ="string";
    $RecordTypes['KXRecord']    ['name']  ="KX Record";
    $RecordTypes['KXRecord']    ['type']  ="string";
    $RecordTypes['CERTRecord']  ['name']  ="CERT Record";
    $RecordTypes['CERTRecord']  ['type']  ="string";
    $RecordTypes['A6Record']    ['name']  ="A6 Record";
    $RecordTypes['A6Record']    ['type']  ="string";
    $RecordTypes['DNAMERecord'] ['name']  ="DBNAME Record";
    $RecordTypes['DNAMERecord'] ['type']  ="string";
    $RecordTypes['DSRecord']    ['name']  ="DS Record";
    $RecordTypes['DSRecord']    ['type']  ="string";
    $RecordTypes['SSHFPRecord'] ['name']  ="SSHFP Record";
    $RecordTypes['SSHFPRecord'] ['type']  ="string";
    $RecordTypes['RRSIGRecord'] ['name']  ="RRSIG Record";
    $RecordTypes['RRSIGRecord'] ['type']  ="string";
    $RecordTypes['NSECRecord']  ['name']  ="NSEC Record";
    $RecordTypes['NSECRecord']  ['type']  ="string";
    $RecordTypes['mXRecord']  ['name']  ="MX Record";
    $RecordTypes['mXRecord']  ['type']  ="string";
    $RecordTypes['nSRecord']  ['name']  ="nS Record";
    $RecordTypes['nSRecord']  ['type']  ="string";

    $this->RecordTypes = $RecordTypes;

    $attrs  = $this->attributes;
    foreach($RecordTypes as $name => $att){
      $attrs2[] = $name;
    } 
    $attrs3 = array_merge($attrs,$attrs2);

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->search("(&(objectClass=dNSZone)(sOARecord=*))",$attrs3);
    while($zone = $ldap->fetch()){
      $tmp = array();
      foreach($attrs as $att){
        if(isset($zone[$att]['count'])&&(is_array($zone[$att])))
        unset($zone[$att]['count']);
        
        if(isset($zone[$att])){ 
          $tmp[$att] = $zone[$att][0];
        }else{
          $tmp[$att] = "";
        }
      }

      if(isset($tmp['sOARecord'])){
        $tmp2 = split("\ ",$tmp['sOARecord']);
        unset($tmp['sOARecord']);
        $tmp['sOARecord'] = array();
        $tmp['sOARecord']['sOAprimary'] = $tmp2[0];
        $tmp['sOARecord']['sOAmail']    = $tmp2[1];
        $tmp['sOARecord']['sOAserial']  = $tmp2[2];
        $tmp['sOARecord']['sOArefresh'] = $tmp2[3];
        $tmp['sOARecord']['sOAretry']   = $tmp2[4];
        $tmp['sOARecord']['sOAexpire']  = $tmp2[5];
        $tmp['sOARecord']['sOAttl']     = $tmp2[6];
      }

      $tmp['usedRecords'] = array();
      foreach($attrs2 as $name){
        if(isset($zone[$name])){
          for($i = 0 ; $i < $zone[$name]['count'] ; $i++ ){
            $tmp['usedRecords'][] =  array("type"=> $name,"value"=>$zone[$name][$i]);
          }
        }
      }
      $this->Zones[$zone['zoneName'][0]] = $tmp;
    }
    if(count($this->Zones > 0)){
      $this->is_account = true;
    }
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

    $once = false;
    foreach( $_POST as $name => $value){
      if(preg_match("/^editZone_/",$name)&&!$once){
        $once =true;
        $tmp = preg_replace("/^editZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","...",$tmp));
        $this->editMode = $this->Zones[$tmp];
      }
    }


    $smarty->assign("editMode",$this->editMode);
    if($this->editMode){

      foreach($this->attributes as $attrs){
        $smarty->assign($attrs,$this->editMode[$attrs]);
      }
      
      foreach($this->editMode['sOARecord'] as $name => $value){
        $smarty->assign($name,$value);
      }
      $smarty->assign("records","");

    }else{
      $ZoneList = new divSelectBox("dNSZones");
      $ZoneList -> SetHeight(254);

      $editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>";

      foreach($this->Zones as $zone => $values ){
        $ZoneList->AddEntry(array(array("string" => $zone.str_replace("%s",base64_encode($zone),$editImg))));
      }    


      $smarty->assign("ZoneList",$ZoneList->DrawList());
    }

    $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    //plugin::save_object();

    if(!$this->editMode){return;}

    foreach($this->attributes as $attr){
      if(isset($_POST[$attr])){
        $this->editMode[$attr] = $_POST[$attr];
      }
    }

    foreach($this->editMode['sOARecord'] as $name => $value){
      if(isset($_POST[$name])){
        $this->editMode['sOARecord'][$name] = $_POST[$name];
      }
    }

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
    plugin::save();

    /* Optionally execute a command after we're done */
    #$this->handle_post_events($mode);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
