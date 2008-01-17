<?php

class termgeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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
                            "update"          => "sceduledupdate",
                            "reinstall"       => "install",
                            "rescan"          => "",
                            "memcheck"        => "memcheck",
                            "sysinfo"         => "sysinfo");

  var $fai_activated = FALSE;
  var $view_logged = FALSE;

  var $member_of_ogroup = FALSE;

  function termgeneric (&$config, $dn= NULL, $parent= NULL)
  {
    /* Check if FAI is activated */
    $tmp= $config->search("faiManagement", "CLASS",array('menu','tabs'));
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

    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
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

  function set_acl_base($base)
  {
    plugin::set_acl_base($base);
    $this->netConfigDNS->set_acl_base($base);
  }

  function set_acl_category($cat)
  {
    plugin::set_acl_category($cat);
    $this->netConfigDNS->set_acl_category($cat);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","terminal/".get_class($this),$this->dn);
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    if (isset($_POST['action']) && $this->acl_is_writeable("FAIstate")){

      /* Set FAIstate */
      if($this->fai_activated && $this->dn != "new"){
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
        show_ldap_error($ldap->get_error(), sprintf(_("Saving of system terminal/generic (FAIstate) with dn '%s' failed."),$this->dn));
      }

      switch($_POST['saction']){
        case 'wake':
          $cmd= $this->config->search("termgeneric", "WAKECMD",array('tabs'));

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
          $cmd= $this->config->search("termgeneric", "REBOOTCMD",array('tabs'));
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
          $cmd= $this->config->search("termgeneric", "HALTCMD",array('tabs'));
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
      if(preg_match("/^chooseBase/",$name) && $once && $this->acl_is_moveable()){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
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

        /* A new base was selected, check if it is a valid one */
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$this->dialog->isSelected()])){
          $this->base = $this->dialog->isSelected();
        }
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return($display);
    }

    /* Add new ntp Server to our list */
    if((isset($_POST['addNtpServer'])) && (isset($_POST['gotoNtpServers'])) && $this->acl_is_writeable("gotoNtpServer")){
      $this->gotoNtpServer[$_POST['gotoNtpServers']] = $_POST['gotoNtpServers'];
    }

    /* Delete selected NtpServer for list of used servers  */
    if((isset($_POST['delNtpServer'])) && (isset($_POST['gotoNtpServerSelected'])) && $this->acl_is_writeable("gotoNtpServer")){
      foreach($_POST['gotoNtpServerSelected'] as $name){
        unset($this->gotoNtpServer[$name]);
      } 
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $smarty->assign("cn", $this->cn);
    $smarty->assign("staticAddress", "");

    $smarty->assign("bases", $this->get_allowed_bases());

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

    $tmp = array();
    foreach($this->gotoNtpServers as $server){
      if(!in_array($server,$this->gotoNtpServer)){
        $tmp[$server] = $server;
      }
    }
    
    $smarty->assign("ntpservers",     $tmp);
    $smarty->assign("fai_activated",$this->fai_activated);

    /* Variables */
    foreach(array("base", "gotoMode", "gotoTerminalPath", "gotoSwapServer","gotoSyslogServer", "gotoNtpServer") as $val){
      $smarty->assign($val."_select", $this->$val);
    }

    $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

    /* Show main page */
    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    return($smarty->fetch (get_template_path('terminal.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    if($this->acl_is_removeable()){   
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->dn);
      $ldap->cat($this->dn, array('dn'));
      if($ldap->count()){
        $this->netConfigDNS->remove_from_parent();
        $ldap->rmDir($this->dn);
  
        new log("remove","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
  
        show_ldap_error($ldap->get_error(), sprintf(_("Removing of object system terminal/generic with dn '%s' failed."),$this->dn));

        /* Optionally execute a command after we're done */
        $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));

        /* Delete references to object groups */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
        while ($ldap->fetch()){
          $og= new ogroup($this->config, $ldap->getDN());
          unset($og->member[$this->dn]);
          $og->save ();
        }
      }
    }
  }


  /* Save data to object */
  function save_object()
  {
    /* Create a base backup and reset the
       base directly after calling plugin::save_object();
       Base will be set seperatly a few lines below */
    $base_tmp = $this->base;
    plugin::save_object();
    $this->base = $base_tmp;

    /* Set new base if allowed */
    $tmp = $this->get_allowed_bases();
    if(isset($_POST['base'])){
      if(isset($tmp[$_POST['base']])){
        $this->base= $_POST['base'];
      }
    }
    
    $this->netConfigDNS->save_object();

    /* Save terminal path to parent since it is used by termstartup, too */
    if(isset($this->parent->by_object['termstartup'])){
      $this->parent->by_object['termstartup']->gotoTerminalPath= $this->gotoTerminalPath;
    }
    
    if(isset($_POST['termgeneric_posted'])){
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

    /* Skip IP & Mac checks if this is a template */
    if($this->cn != "default"){
      $message= array_merge($message, $this->netConfigDNS->check());
    }

    /* Permissions for that base? */
    $this->dn= "cn=".$this->cn.",".get_ou('terminalou').$this->base;

    if ($this->cn == ""){
      $message[]= _("The required field 'Terminal name' is not set.");
    }

    /* Check if given name is a valid host/dns name */
    if(!tests::is_dns_name($this->cn) ){
      $message[] = _("Please specify a valid name for this object.");
    }

    if ($this->orig_dn == 'new'){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);

      /* It is possible to have a 'default' terminal on every base */
      if($this->cn == "default"){
        $ldap->cat($this->dn);
      }else{
        $ldap->search ("(&(objectClass=gotoTerminal)(cn=".$this->cn."))", array("cn"));
      }
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if (preg_match("/cn=dhcp,/",$attrs['dn']) || preg_match ("/,".normalizePreg(get_ou('incomingou'))."/", $ldap->getDN())){
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

    /* Move object if requested */
    if( $this->orig_dn != 'new' && $this->dn != $this->orig_dn){
      $this->move($this->orig_dn, $this->dn);
    }

    plugin::save();

    /* Strip out 'default' values */
    foreach (array("gotoTerminalPath", "gotoSwapServer", "gotoSyslogServer") as $val){
      if(isset($this->attrs[$val])){
        if ($this->attrs[$val] == "default"){
          $this->attrs[$val]= array();
        }
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
      new log("create","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    } else {
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      new log("modify","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }
    
    /* cn=default and macAddress=- indicates that this is a template */
    if($this->cn == "default"){
      $this->netConfigDNS->macAddress = "-";
    }

    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save();
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of object system terminal/generic with dn '%s' failed."),$this->dn));
  }





  /* Display generic part for server copy & paste */
  function getCopyDialog()
  {
    $vars = array("cn");
    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $smarty->assign("object","terminal");
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);
    if(isset($source['macAddress'][0])){
      $this->netConfigDNS->macAddress = $source['macAddress'][0];
    }
    if(isset($source['ipHostNumber'][0])){
      $this->netConfigDNS->ipHostNumber = $source['ipHostNumber'][0];
    }

    /* Create used ntp server array */
    $this->gotoNtpServer= array();
    if(isset($source['gotoNtpServer'])){
      $this->inheritTimeServer = false;
      unset($source['gotoNtpServer']['count']);
      foreach($source['gotoNtpServer'] as $server){
        $this->gotoNtpServer[$server] = $server;
      }
    }

    /* Set inherit checkbox state */
    if((in_array("default",$this->gotoNtpServer)) || (count($this->gotoNtpServer)==0)){
      $this->inheritTimeServer = true;
      $this->gotoNtpServer=array();
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Terminal"),
          "plDescription" => _("Terminal generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 1,
          "plSection"     => array("administration"),
          "plCategory"    => array("terminal" => array( "description"  => _("Terminal"),
                                                        "objectClass"  => "gotoTerminal")),
          "plProvidedAcls"=> array(
            "gotoMode"            => _("Mode"),
            "gotoTerminalPath"    => _("Root server"),
            "gotoSwapServer"      => _("Swap server"),
            "gotoSyslogServer"    => _("Syslog server enabled"),
            "gotoNtpServer"       => _("Ntp server settings"),
            "base"                => _("Base"),
            "cn"                  => _("Name"),
            "gotoRootPasswd"      => _("Root password"),
            "FAIstate"            => _("Action flag"))
          ));
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
