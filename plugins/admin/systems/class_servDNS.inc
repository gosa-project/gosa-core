<?php

class servdns extends plugin
{
  /* attribute list for save action */
  var $ignore_account   = FALSE;
  var $attributes       = array(); 
  var $objectclasses    = array("whatever");

  var $RecordTypes      = array();
  var $Zones            = array();
  var $dialog           = NULL;

  var $orig_dn          = "";

  var $DNSinitially_was_account;

  var $dns_server_list   = array("ENTRIES"=> array(),"FOR_LIST"=> array());
  var $take_over_id       = -1;
  var $display_warning  = TRUE;

  function servdns ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

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
      $this->dns_server_list = $this->get_list_of_dns_servers();
    }else{
      $this->is_account = true;
    }
    $this->DNSinitially_was_account = $this->is_account;
  }


  function get_list_of_dns_servers()
  {
    $ret = array("ENTRIES"=> array(),"FOR_LIST"=> array());
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=dNSZone)(zoneName=*))",array("dn","zoneName"));
    $dns = array();
    while($attrs = $ldap->fetch()){
      /* Skip own config */
      if($this->dn != "new" && preg_match("/".normalizePreg($this->dn)."$/",$attrs['dn'])){
        continue;
      }
      $dn = preg_replace("/^zoneName=[^,]+,/","",$attrs['dn']);
      if(preg_match("/^cn=/",$dn) && !in_array($dn,$dns)){
        $dns[] = $dn;
      }
    }
    $i = 0;
    foreach($dns as $dn){
      $ldap->cat($dn,array('*'));
      if($ldap->count()){
        $i ++;
        $attrs = $ldap->fetch();
          $ret['ENTRIES'][$i]   = $attrs;
          $ret['FOR_LIST'][$i] = $attrs['cn'][0];
      }
    }
    return($ret);
  }  


  function execute()
  {
    /* Call parent execute 
     */
    plugin::execute();

    /* Fill templating stuff 
     */
    $smarty= get_smarty();
    $smarty->assign("dns_take_over",FALSE);
    $display= "";


    /*****************/
    /* Handle Take Over Actions
    /*****************/

    /* Give smarty the required informations */
    $smarty->assign("dns_server_list", $this->dns_server_list['FOR_LIST']);
    $smarty->assign("dns_server_list_cnt", count($this->dns_server_list['FOR_LIST']));

    /* Take over requested, save id */
    if(isset($_POST['take_over_src']) && isset($_POST['take_over'])){
      $id = $_POST['take_over_src'];
      if(isset($this->dns_server_list['ENTRIES'][$id])){
        $this->take_over_id = $id;
      }
    }

    /* Abort take over action */
    if(isset($_POST['cancel_take_over'])){
      $this->dialog =false;
      $this->take_over_id = -1;
      $this->dns_server_list = $this->get_list_of_dns_servers();
    }

    /* Display informartion about take over that will be started when saving this server
     *  and hide default dns output
     */
    if($this->take_over_id != -1){
      $this->dialog = FALSE;
      $id = $this->take_over_id;
      $smarty->assign("dns_take_over",TRUE);
      $warning = sprintf(_("You are going to migrate the DNS setup from server '%s'."),$this->dns_server_list['ENTRIES'][$id]['cn'][0]);
#      $warning.= "&nbsp;"._("This includes 'all' DNS zones that are located within this server. Please double check if your really want to do this.");
      $warning.= "&nbsp;"._("The migration will be started when you save this system. To cancel this action, use the cancel button below.");

      if($this->display_warning){
        print_red($warning);
        $this->display_warning = FALSE;
      }
      return($smarty->fetch(get_template_path('servdns.tpl', TRUE)));
    }


    /* Do we need to flip is_account state? 
     */
    if (isset($_POST['modify_state'])){

      /* Only change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
        $this->is_modified = true;
      }
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
    if((isset($_POST['SaveZoneChanges'])) && is_object($this->dialog)){
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
    if(isset($_POST['AddZone']) && chkacl($this->acl,"servdns") == ""){
      $this->dialog = new servdnseditZone($this->config,$this->dn);
    }

    /* Check for edit zone request 
     */
    $once = false;
    foreach( $_POST as $name => $value){
  
      /* check all post for edit request 
       */
      if(preg_match("/^editZone_/",$name)&&!$once && chkacl($this->acl,"servdns") == ""){
        $once =true;
        $tmp = preg_replace("/^editZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
        $this->dialog= new servdnseditZone($this->config,$this->dn,$this->Zones[$tmp]);
      }

      /* check posts for delete zone 
       */
      if(preg_match("/^delZone_/",$name)&&!$once && chkacl($this->acl,"servdns") == ""){

        $once =true;
        $tmp = preg_replace("/^delZone_/","",$name);
        $tmp = base64_decode(preg_replace("/_.*$/","",$tmp));
     
        /* Initiate deletion
         */ 
        $this->RemoveZone($tmp); 
      }
    }

    if(isset($_GET['act']) && $_GET['act'] == "edit" && isset($_GET['id'])){
      $id = base64_decode($_GET['id']);
      if(isset($this->Zones[$id])){
         $this->dialog= new servdnseditZone($this->config,$this->dn,$this->Zones[$id]);
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
    $ZoneList -> SetHeight(300);

    /* Add entries to divlist
     */
    $editImg = "<input type='image' src='images/edit.png' name='editZone_%s'>
      <input type='image' src='images/edittrash.png' name='delZone_%s'>";
    foreach($this->Zones as $zone => $values ){
      $link = "<a href='?plug=".$_GET['plug']."&act=edit&id=%s'>%s</a>";
      $ZoneList->AddEntry(array(
            array("string" => sprintf($link,base64_encode($zone),getNameFromMix($zone))),
            array("string" => sprintf($link,base64_encode($zone),_("Reverse zone")." : ".getNameFromMix($values['ReverseZone']))),
            array("string" => _("TTL")." : ".$values['sOAttl']),
            array("string" => _("Class")." : ".$values['dNSClass']),
            array("string" => str_replace("%s",base64_encode($zone),$editImg))
            ));
    }    
  
    $smarty->assign("servdnsACL",chkacl($this->acl,"servdns"));
  
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
    $rev  ="";
  
    if(isset($this->Zones[$id]['InitialReverseZone'])){
      $rev = FlipIp(getNameFromMix($this->Zones[$id]['InitialReverseZone']));
    }elseif(isset($this->Zones[$id]['ReverseZone'])){
      $rev = FlipIp(getNameFromMix($this->Zones[$id]['ReverseZone']));
    }

    $zonename = "";
    if(isset($this->Zones[$id]['InitialzoneName'])){
      $zonename= getNameFromMix($this->Zones[$id]['InitialzoneName']);
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
      return(true);
    }
    return(false);
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
    if($this->DNSinitially_was_account){
      $this->is_account = FALSE;
      $bool = true;
      foreach($this->Zones as $key => $zone){
        $bool= $bool & $this->RemoveZone($key);
      }

      if($bool){
        $this->save();
      }
      return($bool);
    }
  }



  /* Save to LDAP */
  function save()
  {

    /* Take over handling
     * - Create list of zones managed by source server 
     * - Copy ldap entries to destination server 
     * - Remove old zone entries from source
     */
    if($this->take_over_id != -1){
      $del = array();
      $id = $this->take_over_id;
      $src = $this->dns_server_list['ENTRIES'][$id]['dn'];
      $ldap = $this->config->get_ldap_link(); 
      $ldap->ls("(objectClass=dnsZone)",$src,array('cn'));
      while($attrs = $ldap->fetch()){
        $src_zone = $attrs['dn'];
        $dst_zone = preg_replace("/".normalizePreg($src)."$/",$this->dn,$src_zone);
        $res = plugin::recursive_move($src_zone, $dst_zone);

        if($res){
          $del [] = $src_zone;
        }
      }
      foreach($del as $src_zone){
        $ldap->rmdir_recursive($src_zone);
      }
      return;
    }


    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);  
  
    /* Get differences 
     */

    $old_dn =  $this->orig_dn;
    if($old_dn == "new"){
      $old_dn = $this->dn;
    }
 
    /* Update dns to current object dn */ 
    $tmp = getDNSZoneEntriesDiff($this->config,$this->Zones,$old_dn);
    $tmp2 = array();
    foreach($tmp as $key1 => $data1){
      $tmp2[$key1] = array();
      foreach($data1 as $key2 => $data2){
        $tmp2[$key1][preg_replace("/".normalizePreg($old_dn)."$/",$this->dn,$key2)] = $data2;
      }
    }
    $tmp = $tmp2;

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
      $for = getNameFromMix($del['InitialzoneName']);
      $rev = FlipIp(getNameFromMix($del['InitialReverseZone'])).".in-addr.arpa";
      $ldap->cd($dn);
      $ldap->rmdir_recursive($dn);
      show_ldap_error($ldap->get_error(), _("Removing DNS entries failed"));

      /* Handle Post events */
      if(preg_match("/^zoneName=/",$dn)){
#        $this->handle_post_events("remove",array("dn" => $dn,"zoneName" => $for));
#        $this->handle_post_events("remove",array("dn" => $dn,"zoneName" => $rev));
      }
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
      if($ldap->count()){
        $ldap->cd($dn);
        $ldap->modify ($attrs);
        show_ldap_error($ldap->get_error(), _("Saving DNS entries failed"));

        /* Handle Post events */
        if(preg_match("/^zoneName=/",$dn)){
#          $this->handle_post_events("modify",array("dn" => $dn,"zoneName" => $attrs['zoneName']));
        }
      }else{
        $ldap->cd($dn);
        $ldap->add($attrs);
        show_ldap_error($ldap->get_error(), _("Saving DNS entries failed"));

        /* Handle Post events */
        if(preg_match("/^zoneName=/",$dn)){
#          $this->handle_post_events("add",array("dn" => $dn,"zoneName" => $attrs['zoneName']));
        }
      }
    }
    $this->handle_post_events("modify");
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
