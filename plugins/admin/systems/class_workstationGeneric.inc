<?php

class workgeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage workstation base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $gotoMode= "locked";
  var $gotoSyslogServer= "";
  var $gotoSyslogServers= array();
  var $gotoNtpServer= array();
  var $gotoNtpServers= array();
  var $gotoSndModule= "";
  var $gotoFloppyEnable= "";
  var $gotoCdromEnable= "";
  var $ghCpuType= "-";
  var $ghMemSize= "-";
  var $ghUsbSupport= "-";
  var $ghNetNic= array();
  var $ghIdeDev= array();
  var $ghScsiDev= array();
  var $ghGfxAdapter= "-";
  var $ghSoundAdapter= "-";
  var $gotoLastUser= "-";
  var $FAIscript= "";
  var $didAction= FALSE;
  var $FAIstate= "";

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $l= "";
  var $orig_dn= "";

  /* Plugin side filled */
  var $modes= array();

  var $netConfigDNS;

  var $inheritTimeServer = true;

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("gotoMode", "gotoSyslogServer", "gotoNtpServer",
      "gotoFloppyEnable", "gotoCdromEnable", "cn", "gotoSndModule",
      "ghCpuType", "ghMemSize", "ghUsbSupport",
      "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser", "l","FAIscript");
  var $objectclasses= array("top", "gotoWorkstation", "GOhard","FAIobject");

  function workgeneric ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->netConfigDNS = new termDNS($this->config,$this->dn,$this->objectclasses);

    /* Read arrays */
    foreach (array("ghNetNic", "ghIdeDev", "ghScsiDev") as $val){
      if (!isset($this->attrs[$val])){
        continue;
      }
      for ($i= 0; $i<$this->attrs[$val]['count']; $i++){
        array_push($this->$val, $this->attrs[$val][$i]);
      }
    }

    /* Create used ntp server array */
    $this->gotoNtpServer= array();
    if(isset($this->attrs['gotoNtpServer'])){
      $this->inheritTimeServer = false;
      unset($this->attrs['gotoNtpServer']['count']);
      foreach($this->attrs['gotoNtpServer'] as $server){
        $this->gotoNtpServer[$server] = $server;
      }
    }

    /* Set inherit checkbox state */
    if((in_array("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer) == 0)){
      $this->inheritTimeServer = true;
      $this->gotoNtpServer=array();
    }

    /* Create available ntp options */
    $tmp = $this->config->data['SERVERS']['NTP'];
    $this->gotoNtpServers = array();
    foreach($tmp as $key => $server){
      if($server == "default") continue;
      $this->gotoNtpServers[$server] = $server;
    }

    $this->modes["active"]= _("Activated");
    $this->modes["locked"]= _("Locked");
    $this->modes["memcheck"]= _("Memory test");
    $this->modes["sysinfo"]= _("System analysis");

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }

    /* Create an array of all Syslog servers */
    $tmp = $this->config->data['SERVERS']['SYSLOG'];
    foreach($tmp as $server){
      $visible = $server;
      if($server == "default") {
        $visible = "["._("inherited")."]";
      }
      $this->gotoSyslogServers[$server] = $visible;
    }

    /* Save 'dn' for later referal */
    $this->orig_dn= $this->dn;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    if (isset($_POST['action'])){
      $cmd= search_config($this->config->data['TABS'], "workgeneric", "ACTIONCMD");
      if ($cmd == ""){
        print_red(_("No ACTIONCMD definition found in your gosa.conf"));
      } else {
        exec ($cmd." ".$this->netConfigDNS->macAddress." ".escapeshellarg($_POST['saction']), $dummy, $retval);
        if ($retval != 0){
          print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
        } else {
          $this->didAction= TRUE;
        }
      }
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no workstation features.")."</b>";
      return($display);
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
        $this->base = $this->dialog->isSelected();
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Add new ntp Server to our list */ 
    if((isset($_POST['addNtpServer'])) && (isset($_POST['gotoNtpServers']))){
      $this->gotoNtpServer[$_POST['gotoNtpServers']] = $_POST['gotoNtpServers'];
    }

    /* Delete selected NtpServer for list of used servers  */
    if((isset($_POST['delNtpServer'])) && (isset($_POST['gotoNtpServerSelected']))){
      foreach($_POST['gotoNtpServerSelected'] as $name){
        unset($this->gotoNtpServer[$name]);
      }
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("l", $this->l);
    $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("staticAddress", "");
    $smarty->assign("gotoNtpServers",$this->gotoNtpServers);
        
    /* Check if workstation is online */
    $query= "fping -q -r 1 -t 500 ".$this->cn;
    exec ($query, $dummy, $retval);

    /* Offline */
    if ($retval == 0){
      $smarty->assign("actions", array("halt" => _("Switch off"), "reboot" => _("Reboot"),
                                       "instant_update" => _("Instant update"),
                                       "update" => _("Scheduled update"),
                                       "reinstall" => _("Reinstall"),
                                       "rescan" => _("Rescan hardware")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up"),
                                       "reinstall" => _("Reinstall"),
                                       "update" => _("Scheduled update")));
    }
    /* Arrays */
    $smarty->assign("modes", $this->modes);
    $smarty->assign("nfsservers", $this->config->data['SERVERS']['NFS']);
    $smarty->assign("syslogservers", $this->gotoSyslogServers);
    $smarty->assign("ntpservers", $this->config->data['SERVERS']['NTP']);

    /* Variables */
    foreach(array("base", "gotoMode", "gotoSyslogServer", "gotoNtpServer") as $val){
      $smarty->assign($val."_select", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }
    $smarty->assign("actionACL", chkacl($this->acl, 'action'));

    /* tell smarty the inherit checkbox state */
    $smarty->assign("inheritTimeServer",$this->inheritTimeServer);

    /* Show main page */
    $smarty->assign("netconfig", $this->netConfigDNS->execute());
    return($smarty->fetch (get_template_path('workstation.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing workstation failed"));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove", array("macAddress" => $this->netConfigDNS->macAddress));

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();

    /* Save base, since this is no LDAP attribute */
    if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
      $this->base= $_POST['base'];
    }
    $this->netConfigDNS->save_object();

    /* Set inherit mode */
    if(isset($_POST['workgeneric_posted'])){
      if(isset($_POST["inheritTimeServer"])){
        $this->inheritTimeServer = true;
      }else{
        $this->inheritTimeServer = false;
      }
    }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());

    $ui= get_userinfo();
    $this->dn= "cn=".$this->cn.",ou=workstations,ou=systems,".$this->base;
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "group", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a workstation on this 'Base'.");
    }

    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= _("The required field 'Workstation name' is not set.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(&(cn=".$this->cn.")(objectClass=gotoWorkstation))", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if (preg_match ("/,ou=incoming,/", $ldap->getDN())){
            continue;
          } else {
            if ($attrs['dn'] != $this->orig_dn){
              $message[]= sprintf (_("There is already an entry '%s' in the base choosen by you"), $this->cn);
              break;
            }
          }
        }
      }
    }
    
    /* Check for valid ntpServer selection */
    if((!$this->inheritTimeServer) && (!count($this->gotoNtpServer))){
      $message[]= _("There must be at least one NTP server selected, or the inherit mode activated.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Strip out 'default' values */
    foreach (array("gotoSyslogServer") as $val){

      if ($this->attrs[$val] == "default"){
        $this->attrs[$val]= array();
      }
    }

    /* Add missing arrays */
    foreach (array("ghScsiDev", "ghIdeDev", "ghNetNic") as $val){
      if (isset ($this->$val) && count ($this->$val) != 0){
        $this->attrs["$val"]= $this->$val;
      }
    }

    /* Remove all empty values */
    if ($this->orig_dn == 'new'){
      $attrs= array();
      foreach ($this->attrs as $key => $val){
        if (is_array($val) && count($val) == 0){
          continue;
        }
        $attrs[$key]= $val;
      }
      $this->attrs= $attrs;
    }

    /* Update ntp server settings */
    if($this->inheritTimeServer){
      if($this->new){
        if(isset($this->attrs['gotoNtpServer'])){
          unset($this->attrs['gotoNtpServer']);
        }
      }else{
        $this->attrs['gotoNtpServer'] = array();
      }
    }else{
      /* Set ntpServers */
      $this->attrs['gotoNtpServer'] = array();
      foreach($this->gotoNtpServer as $server){
        $this->attrs['gotoNtpServer'][] = $server;
      }
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      show_ldap_error($ldap->get_error(), _("Saving workstation failed"));
      if(!$this->didAction){
        $this->handle_post_events("add");
      }
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }
      $ldap->cd($this->dn);
      $this->cleanup();
//      $this->attrs['gosaUnitTag']= array();
      $ldap->modify ($this->attrs); 

      if(!$this->didAction){
        $this->handle_post_events("modify");
      }
    }
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save($this->dn);
    show_ldap_error($ldap->get_error(), _("Saving workstation failed"));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
