<?php

class termgeneric extends plugin
{
  /* Generic terminal attributes */
  var $gotoMode= "disabled";
  var $gotoTerminalPath= "";
  var $gotoSwapServer= "";
  var $gotoSyslogServer= "";
  var $gotoSyslogServers = array();
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
  var $netConfigDNS;
  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $orig_dn= "";

  var $inheritTimeServer = true;

  /* Plugin side filled */
  var $modes= array();

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("gotoMode", "gotoTerminalPath", 
      "gotoSwapServer", "gotoSyslogServer", "gotoNtpServer",
      "gotoFloppyEnable", "gotoCdromEnable", "cn", "gotoSndModule",
      "ghCpuType", "ghMemSize","ghUsbSupport",
      "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser");
  var $objectclasses= array("top", "gotoTerminal", "GOhard");

  var $mapActions   = array("reboot"          => "",
                            "instant_update"  => "softupdate",
                            "update"          => "scheduledupdate",
                            "reinstall"       => "install",
                            "rescan"          => "",
                            "memcheck"        => "memcheck",
                            "sysinfo"         => "sysinfo");

  var $fai_activated = FALSE;

  var $member_of_ogroup = FALSE;

  function termgeneric ($config, $dn= NULL, $parent= NULL)
  {
    /* Check if FAI is activated */
    $tmp = search_config($config->data,"faiManagement","CLASS");
    if(!empty($tmp)){
      $this->fai_activated = TRUE;
    }

    plugin::plugin ($config, $dn, $parent);

    if(!isset($this->parent->by_object['ogroup'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".LDAP::prepare4filter($this->dn)."))",array("cn"));
      $this->member_of_ogroup = $ldap->count() >= 1;
    }

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
    if((in_array("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer)==0)){
      $this->inheritTimeServer = true;
      $this->gotoNtpServer=array();
    }

    /* You can't inherit the NTP service, if we are not member in an object group */
    if(!$this->member_of_ogroup){
      $this->inheritTimeServer = FALSE;
    }

    /* Create available ntp options */
    $this->gotoNtpServers = $this->config->data['SERVERS']['NTP'];
    foreach($this->gotoNtpServers as $key => $server){
      if($server == "default"){
        unset($this->gotoNtpServers[$key]);
      }
    }

    $this->modes["disabled"]= _("disabled");
    $this->modes["text"]= _("text");
    $this->modes["graphic"]= _("graphic");

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
      if($server == "default" && $this->member_of_ogroup) {
        $visible = "["._("inherited")."]";
      }
      $this->gotoSyslogServers[$server] = $visible;
    }

    $this->orig_dn= $this->dn;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $this->netConfigDNS->acl = $this->acl;

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    if (isset($_POST['action']) && chkacl($this->acl,"FAIstate") ==""){

      if($this->fai_activated && $this->dn != "new"){
        /* Set FAIstate */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($this->dn,array("objectClass"));
        $res = $ldap->fetch();

        $attrs = array();
        $attrs['FAIstate'] = "";
        if(isset($this->mapActions[$_POST['saction']])){
          $attrs['FAIstate'] = $this->mapActions[$_POST ['saction']];
        }

        for($i = 0; $i < $res['objectClass']['count'] ; $i ++){
          $attrs['objectClass'][] = $res['objectClass'][$i];
        }

        if(($attrs['FAIstate'] != "") && (!in_array("FAIobject",$attrs['objectClass']))){
          $attrs['objectClass'][] = "FAIobject";
        }

        if($attrs['FAIstate'] == ""){
#FIXME we should check if FAIobject is used anymore
          $attrs['FAIstate'] = array();
        }

        $ldap->cd($this->dn);
        $ldap->modify($attrs);
        show_ldap_error($ldap->get_error());
      }

      switch($_POST['saction']){
        case 'wake':
          $cmd= search_config($this->config->data['TABS'], "termgeneric", "WAKECMD");
          if ($cmd == ""){
            print_red(_("No WAKECMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$this->netConfigDNS->macAddress, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;

        case 'reboot':
          $cmd= search_config($this->config->data['TABS'], "termgeneric", "REBOOTCMD");
          if ($cmd == ""){
            print_red(_("No REBOOTCMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$this->cn, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;

        case 'halt':
          $cmd= search_config($this->config->data['TABS'], "termgeneric", "HALTCMD");
          if ($cmd == ""){
            print_red(_("No HALTCMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$this->cn, $dummy, $retval);
            if ($retval != 0){
              print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
            }
          }
          break;
      }
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once && chkacl($this->acl,"base") == ""){
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

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return($display);
    }

    /* Add new ntp Server to our list */
    if((isset($_POST['addNtpServer'])) && (isset($_POST['gotoNtpServers'])) && chkacl($this->acl,"gotoNtpServer")== ""){
      $this->gotoNtpServer[$_POST['gotoNtpServers']] = $_POST['gotoNtpServers'];
    }

    /* Delete selected NtpServer for list of used servers  */
    if((isset($_POST['delNtpServer'])) && (isset($_POST['gotoNtpServerSelected'])) && chkacl($this->acl,"gotoNtpServer")== ""){
      foreach($_POST['gotoNtpServerSelected'] as $name){
        unset($this->gotoNtpServer[$name]);
      } 
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("cnACL", chkacl($this->acl,"cn"));
    $smarty->assign("staticAddress", "");

    $smarty->assign("bases", $this->config->idepartments);

    /* tell smarty the inherit checkbox state */
    $smarty->assign("inheritTimeServer",$this->inheritTimeServer);

    /* Check if terminal is online */
    $query= "fping -q -r 1 -t 500 ".$this->cn;
    exec ($query, $dummy, $retval);

    /* Offline */
    if ($retval == 0){
      $smarty->assign("actions", array( "halt" => _("Switch off"), 
                                        "reboot" => _("Reboot"),
                                        "memcheck" => _("Memory test"),
                                        "sysinfo"  => _("System analysis")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up"),
                                       "memcheck" => _("Memory test"),
                                       "sysinfo"  => _("System analysis")));
    }

    /* Arrays */
    $smarty->assign("modes", $this->modes);

    $tmp2 = array(); 
    foreach($this->config->data['SERVERS']['NFS'] as $server){
      if($server != "default"){
        $tmp2[$server]= $server;
      }else{
        if($this->member_of_ogroup){
          $tmp2[$server]="["._("inherited")."]";
        }
      }
    }
  
    $smarty->assign("nfsservers",     $tmp2);

    

    $smarty->assign("syslogservers",  $this->gotoSyslogServers);
    $smarty->assign("ntpservers",     $this->gotoNtpServers);

    /* Variables */
    foreach(array("base", "gotoMode", "gotoTerminalPath", "gotoSwapServer",
          "gotoSyslogServer", "gotoNtpServer") as $val){

      $smarty->assign($val."_select", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    /* Show main page */
    $this->netConfigDNS->cn= $this->cn;
    $smarty->assign("netconfig", $this->netConfigDNS->execute());
    $smarty->assign("fai_activated",$this->fai_activated);
    $smarty->assign("actionACL", chkacl($this->acl, 'action'));

    /* Display sub dialog from network settings */
    if($this->netConfigDNS->dialog){
      $this->dialog = TRUE;
      return($this->netConfigDNS->execute());
    }else{ 
      $this->dialog = FALSE;
    }

    $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

    return($smarty->fetch (get_template_path('terminal.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    
    $this->netConfigDNS->acl = $this->acl;
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn, array('dn'));
    if($ldap->count()){
      $this->netConfigDNS->remove_from_parent();
      $ldap->rmDir($this->dn);
      show_ldap_error($ldap->get_error(), _("Removing terminal failed"));

      /* Optionally execute a command after we're done */
      $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

      /* Delete references to object groups */
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
      while ($ldap->fetch()){
        $og= new ogroup($this->config, $ldap->getDN());
        unset($og->member[$this->dn]);
        $og->save ();
      }
    }
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
    $this->netConfigDNS->save_object();

    /* Save base, since this is no LDAP attribute */
    if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
      $this->base= $_POST['base'];
    }

    /* Save terminal path to parent since it is used by termstartup, too */
    if(isset($this->parent->by_object['termstartup'])){
    $this->parent->by_object['termstartup']->gotoTerminalPath=
      $this->gotoTerminalPath;
    }
  
    if(isset($_POST['termgeneric_posted']) && chkacl($this->acl,"gotoNtpServer") == ""){
      if(isset($_POST["inheritTimeServer"]) && $this->member_of_ogroup){
        $this->inheritTimeServer = true;
      }else{
        $this->inheritTimeServer = false;
      }
    }  

    if(isset($_POST["inheritAll"])){
      $this->set_everything_to_inherited();
    }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if($this->cn != "default"){
      $message= array_merge($message, $this->netConfigDNS->check());
    }

    /* Permissions for that base? */
    $this->dn= "cn=".$this->cn.",ou=terminals,ou=systems,".$this->base;

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "group", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a terminal on this 'Base'.");
    }

    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= _("The required field 'Terminal name' is not set.");
    }

    /* Check if given name is a valid host/dns name */
    if(!is_dns_name($this->cn) && chkacl ($this->acl, "cn") == ""){
      $message[] = _("Please specify a valid name for this object.");
    }

    if ($this->orig_dn == 'new'){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->config->current['BASE']);
 
      /* It is possible to have a 'default' terminal on every base */
      if($this->cn == "default"){
        $ldap->cat($this->dn);
      }else{
        $ldap->search ("(&(objectClass=gotoTerminal)(cn=".$this->cn."))", array("cn"));
      }
  
      /* Check if there are some other terminals found */
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if (preg_match("/cn=dhcp,/",$attrs['dn']) || preg_match ("/,ou=incoming,/", $ldap->getDN())){
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
    $this->netConfigDNS->acl = $this->acl;

    /* Move object if requested */
    if( $this->orig_dn != 'new' && $this->dn != $this->orig_dn){
      $this->move($this->orig_dn, $this->dn);
    }
 
    plugin::save();

    /* Strip out 'default' values */
    foreach (array("gotoTerminalPath", "gotoSwapServer", "gotoSyslogServer") as $val){

      if (!isset($this->attrs[$val]) || $this->attrs[$val] == "default"){
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

    /* Set ntpServers */
    $this->attrs['gotoNtpServer'] = array();
    if(!$this->inheritTimeServer){
      foreach($this->gotoNtpServer as $server){
        $this->attrs['gotoNtpServer'][] = $server;
      }
    }

    /* Append gosaAdministrativeUnitTag to objectClass if gosaUnitTag isset */
    if(($this->gosaUnitTag) && (!in_array_ics("gosaAdministrativeUnitTag",$this->attrs['objectClass']))){
      $this->attrs['objectClass'][] = "gosaAdministrativeUnitTag";
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      if (!count($this->attrs['gotoNtpServer'])){
        unset($this->attrs['gotoNtpServer']);
      }
      $ldap->add($this->attrs);
      $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    } else {
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }
    
    /* cn=default and macAddress=- indicates that this is a template */ 
    if($this->cn == "default"){
      $this->netConfigDNS->macAddress = "-";
    }
  
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save($this->dn);
    show_ldap_error($ldap->get_error(), _("Saving terminal failed"));
  }


  function set_everything_to_inherited()
  {
    $this->gotoTerminalPath  = "default";
    $this->gotoSwapServer    = "default" ;
    $this->gotoSyslogServer  = "default";
    $this->inheritTimeServer = TRUE;

    /* Set workstation service attributes to inherited */
    if($this->member_of_ogroup && isset($this->parent->by_object['termservice'])){
      foreach(array("gotoXKbLayout","gotoXKbModel","gotoXKbVariant",
            "gotoXResolution","gotoXColordepth","gotoXMouseType","gotoXMouseport") as $name){
        $this->parent->by_object['termservice']->$name = "default";
      }
    }

    /* Set workstation startup attributes to inherited */
    if($this->member_of_ogroup && isset($this->parent->by_object['termstartup'])){
      $this->parent->by_object['termstartup']->gotoBootKernel = "default-inherited";
      $this->parent->by_object['termstartup']->gotoLdapServer = "default-inherited";
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
