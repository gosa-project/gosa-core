<?php

class servdns extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = FALSE;
  var $attributes       = array(); 
  var $objectclasses    = array("whatever");

  var $RecordTypes      = array();
  var $Zones            = array();
  var $dialog           = NULL;

  var $orig_dn          = "";

  var $DNSinitially_was_account;


  function servdns ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->orig_dn = $dn;

    /* Get record types for zones
     */
    $this->RecordTypes = getDnsRecordTypes(true);

    /* Get all zone Informations
     */
    $this->Zones = getDNSZoneEntries($config,$dn);
    
    /* If there is at least one entry in this -> types, we have DNS enabled 
     */
    if(count($this->Zones) == 0){
      $this->is_account = false;
    }else{
      $this->is_account = true;
    }
    $this->DNSinitially_was_account = $this->is_account;
  }


  function execute()
  {
    /* Call parent execute 
     */
    plugin::execute();

    /* Fill templating stuff 
     */
    $smarty= get_smarty();
    $display= "";

    /* Do we need to flip is_account state? 
     */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    if ($this->is_account){
      $display= $this->show_header(_("Remove DNS service"),
          _("This server has DNS features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add DNS service"),
          _("This server has DNS features disabled. You can enable them by clicking below."));
      return ($display);
    }


    /* Edited or Added zone 
     */
    if(isset($_POST['SaveZoneChanges'])){
      $this->dialog->save_object();

      /* Check for errors  
       */
      if(count($this->dialog->check())){
        foreach($this->dialog->check() as $msgs){
          print_red($msgs); 
        }
      }else{
        /* add new/edited zone 
         */
        $ret = $this->dialog->save();
        if(!$this->dialog->isNew){
          unset($this->Zones[$this->dialog->OldZoneName]);
        }
        $this->Zones[$ret['zoneName']] = $ret;
        $this->dialog = NULL;
      }
    }

    /* Cancel zone edit / new 
     */
    if(isset($_POST['CancelZoneChanges'])){
      $this->dialog = NULL;
    }

    /* Add empty new zone 
     */
    if(isset($_POST['AddZone'])){
      $this->dialog = new servdnseditZone($this->config,$this->dn);
    }

    /* Check for edit zone request 
     */
    $once = false;
    foreach( $_POST as $name => $value){
  
      /* check all post for edit request 
       */
      if(preg_match("/^editZone_/",$name)&&!$once){
        $once =true;
        $tmp = preg_replace("/^editZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        $this->dialog= new servdnseditZone($this->config,$this->dn,$this->Zones[$tmp]);
      }

      /* check posts for delete zone 
       */
      if(preg_match("/^delZone_/",$name)&&!$once){

        $once =true;
        $tmp = preg_replace("/^delZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
     
        /* Initiate deletion
         */ 
        $this->RemoveZone($tmp); 
      }
    }

    /* Show dialog 
     */
    if($this->dialog!= NULL){
      $this->dialog->save_object();
      $this->dialog->parent = $this;
      return($this->dialog->execute());
    }

    /* Create Listbox with existing Zones 
     */
    $ZoneList = new divSelectBox("dNSZones");
    $ZoneList -> SetHeight(254);

    /* Add entries to divlist
     */
    $editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>
      <input type='image' src='images/edittrash.png' name='delZone_%s'>";
    foreach($this->Zones as $zone => $values ){
      $ZoneList->AddEntry(array(
            array("string" => $zone),
            array("string" => _("Reverse zone")." : ".$values['ReverseZone']),
            array("string" => _("TTL")." : ".$values['sOAttl']),
            array("string" => _("Class")." : ".$values['dNSClass']),
            array("string" =>str_replace("%s",base64_encode($zone),$editImg))
            ));
    }    

    /* Display tempalte 
     */
    $smarty->assign("ZoneList",$ZoneList->DrawList());
    $display.= $smarty->fetch(get_template_path('servdns.tpl', TRUE));
    return($display);
  }


  /* Delete specified zone
   */
  function RemoveZone($id)
  {
    $zones =  $this->getUsedZoneNames();

    if(isset($this->Zones[$id]['InitialReverseZone'])){
      $rev = FlipIp($this->Zones[$id]['InitialReverseZone']);
    }else{
      $rev = FlipIp($this->Zones[$id]['ReverseZone']);
    }

    $zonename = "";
    if(isset($this->Zones[$id]['InitialzoneName'])){
      $zonename= $this->Zones[$id]['InitialzoneName'];
    }

    $used = array();

    /* Add Records which use this zoneName
     */
    if(isset($zones[$zonename])){
      $used = array_merge($used,$zones[$zonename]);
    }

    /* Add Records which uses this reverse zone
     */
    if(isset($zones[$rev.".in-addr.arpa"])){
      $used = array_merge($used,$zones[$rev.".in-addr.arpa"]);
    } 

    /* There are still entries using this configuration
     *  Abort deletion
     */ 
    if(count($used)){
      $i = 2;
      $str ="";
      foreach($used as $dn){
        if($i > 0 ){
          $i --;
          $str.=$dn." ";
        }
      }

      /*  Only show 2 dns in the error message 
       */
      if(count($used)> 2) {
        $str .=" ... ";
      }
      print_red(sprintf(_("Can't delete the selected zone, because it is still in use by these entry/entries '%s'"),trim($str)));

    }else{
      unset($this->Zones[$id]);
    }
  } 


  /* This funtion returns all used Zonenames 
   */
  function getUsedZoneNames()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(!(relativeDomainName=@))(zoneName=*))",array("zoneName","relativeDomainName"));
    while($attr = $ldap->fetch()){
      $ret[$attr['zoneName'][0]][] = $attr['dn'];
    }
    return($ret);
  }


  /* Remove dns service 
   */
  function remove_from_parent()
  {
    if(!$this->DNSinitially_was_account){
      return;
    }
    print_red("Can't remove dns yet. returning without remove.");
    return;
    $ldap = $this->config->get_ldap_link();
    $ldap->ls("(&(objectClass=dNSZone)(zoneName=*)(relativeDomainName=@))",$this->orig_dn,array("relativeDomainName","zoneName"));
    while($attr = $ldap->fetch()){
      $ldap->cd($attr['dn']);
      $ldap->rmDir($attr['dn']);
    }
    show_ldap_error($ldap->get_error(), _("Removing DNS service failed"));
  }


  /* Save to LDAP */
  function save()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);  
  
    /* Get differences 
     */
    $tmp = getDNSZoneEntriesDiff($this->config,$this->Zones,$this->orig_dn);

    /* Updated zone entries if reverser or forward name has changed  
     * Must be done before moving entries, else the given dn is invalid
     */
    if(isset($tmp['zoneUpdates'])){
      foreach($tmp['zoneUpdates'] as $dn => $attrs){
        $ldap->cd($dn);
        $ldap->modify($attrs);
        show_ldap_error("Zone:".$ldap->get_error(), _("Updating DNS service failed"));
      }
    }

    /* Delete dns 
     */
    foreach($tmp['del'] as $dn => $del){
      $ldap->cd($dn);
      $ldap->rmdir_recursive($dn);
      show_ldap_error($ldap->get_error(), _("Removing DNS entries failed"));
    }

    /* move follwoing entries
     */
    foreach($tmp['move'] as $src => $dst){
      $this->recursive_move($src,$dst);
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
      show_ldap_error($ldap->get_error(), _("Saving DNS entries failed"));
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
