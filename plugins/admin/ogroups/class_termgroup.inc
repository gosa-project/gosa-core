<?php

class termgroup extends plugin
{
  /* attribute list for save action */
  var $attributes= array("gotoMode","gotoSyslogServer", "gotoNtpServer");
  var $objectclasses= array("gotoWorkstationTemplate");

  var $members= array();

  var $gotoMode           = "locked";
  var $gotoSyslogServer   = "";
  var $gotoNtpServer      = array();
  var $gotoNtpServers     = array();
  var $modes              = array();
  var $inheritTimeServer  = true;
  var $acl                ;
  var $is_account		  = true; 
  var $orig_dn		      = "";
  var $didAction		  = FALSE;


  function termgroup ($config, $dn= NULL)
  {
    /***************
      Some  initialisations
     ***************/

    plugin::plugin($config, $dn);

    $ldap= $config->get_ldap_link();

    $this->is_account = true;
    $ui               = get_userinfo();
    $acl              = get_permissions ($this->dn, $ui->subtreeACL);
    $this->acl        = get_module_permission($acl, "group", $this->dn);

    $this->modes["active"]= _("Activated");
    $this->modes["locked"]= _("Locked");
    $this->modes["memcheck"]= _("Memory test");
    $this->modes["sysinfo"]= _("System analysis");

    $this->orig_dn =	$this->dn;

    /*************** 
      Get mac addresses from member objects  
     ***************/

    /* We're only interested in the terminal members here, evaluate
       these... */
    if(isset($this->attrs['member'])){
      for ($i= 0; $i<$this->attrs['member']['count']; $i++){
        $member= $this->attrs['member'][$i];
        $ldap->cat($member, array('objectClass', 'macAddress', 'cn'));
        if (preg_match("/success/i", $ldap->error)){
          $attrs = $ldap->fetch();
          if (in_array("gotoTerminal", $attrs['objectClass']) ||
              in_array("gotoWorkstation", $attrs['objectClass'])){
            if (isset($attrs['macAddress'])){
              $this->members[$attrs['cn'][0]]= $attrs['macAddress'][0];
            } else {
              $this->members[$attrs['cn'][0]]= "";
            }
          }
        }
      }
    }

    /*************** 
      Perpare NTP settings 
     ***************/

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
    if(in_array("default",$this->gotoNtpServer)){
      $this->inheritTimeServer = true;
      $this->gotoNtpServer=array();
    }

    /* Create available ntp options */
    $this->gotoNtpServers = $this->config->data['SERVERS']['NTP'];
    foreach($this->gotoNtpServers as $key => $server){
      if($server == "default"){
        unset($this->gotoNtpServers[$key]);
      }
    }


  }

  function check()
  {
     /* Call common method to give check the hook */
    $message= plugin::check();

    if (chkacl($this->acl, "create") != ""){
      $message[]= _("You have no permissions to create a workstation on this 'Base'.");
    }

    /* Check for valid ntpServer selection */
    if((!$this->inheritTimeServer) && (!count($this->gotoNtpServer))){
      $message[]= _("There must be at least one NTP server selected, or the inherit mode activated.");
    }
    return($message);
  }

  function remove_from_parent()
  {
    /* Workstation startup is using gotoWorkstationTemplate too,
        if we remove this oc all other not manged attributes will cause errors */
    if(isset($this->attrs['gotoKernelParameters'])){
      $this->objectclasses = array();
    }

    /* Remove acc */
    plugin::remove_from_parent();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->orig_dn);
    $ldap->modify($this->attrs);
    $this->handle_post_events("remove");
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();


    /*************** 
      Handle requested action
     ***************/

    /* Watch for events */
    if (isset($_POST['action'])){
      $macaddresses="";
      $names="";
      foreach ($this->members as $cn => $macAddress){
        $macaddresses.= "$macAddress ";
        $names.= "$cn ";
      }

      if (isset($_POST['action'])){
        $cmd= search_config($this->config->data['TABS'], "workgeneric", "ACTIONCMD");
        if ($cmd == ""){
          print_red(_("No ACTIONCMD definition found in your gosa.conf"));
        } else {
          exec ($cmd." ".$macaddresses." ".escapeshellarg($_POST['saction']), $dummy, $retval);
          if ($retval != 0){
            print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
          } else {
            $this->didAction= TRUE;
          }
        }
      }
    }


    /*************** 
      Add remove NTP server
     ***************/

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


    /*************** 
      Prepare smarty 
     ***************/

    /* Set government mode */
    $smarty= get_smarty();

    foreach($this->attributes as $attr){
      $smarty->assign($attr,      $this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$this->$attr));
    }

    /* Variables */
    foreach(array("gotoMode","gotoNtpServer") as $val){
      $smarty->assign($val."_select", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    /* Check if workstation is online */
    $smarty->assign("actions", array(
          "halt" =>             _("Switch off"), 
          "reboot" =>           _("Reboot"),
          "instant_update" =>   _("Instant update"),
          "update" =>           _("Scheduled update"),
          "reinstall" =>        _("Reinstall"),
          "rescan" =>           _("Rescan hardware")));

    $smarty->assign("inheritTimeServer",$this->inheritTimeServer);
    $smarty->assign("modes", $this->modes);
    $smarty->assign("actions", array("wake" => _("Wake up"), "halt" => _("Switch off"), "reboot" => _("Reboot")));
    $smarty->assign("actionACL", chkacl($this->acl, 'action'));
    $smarty->assign("gotoNtpServers",$this->gotoNtpServers); 
    $smarty->assign("syslogservers", $this->config->data['SERVERS']['SYSLOG']); 
    $smarty->assign("gotoSyslogServer_select", $this->gotoSyslogServer); 

    /* Show main page */
    return ($smarty->fetch (get_template_path('termgroup.tpl', TRUE)));
  }

  function save_object()
  {
    plugin::save_object();  
    /* Set inherit mode */
    if(isset($_POST['workgeneric_posted'])){
      if(isset($_POST["inheritTimeServer"])){
        $this->inheritTimeServer = true;
      }else{
        $this->inheritTimeServer = false;
      }
    }
  }

  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /***************
      Prepare special vars 
     ***************/

    /* Unset some special vars ... */
    foreach (array("gotoSyslogServer") as $val){
      if ($this->attrs[$val] == "default"){
        $this->attrs[$val]= array();
      }
    }

    /* Update ntp server settings */
    if($this->inheritTimeServer){
      $this->attrs['gotoNtpServer'] = "default";
    }else{
      /* Set ntpServers */
      $this->attrs['gotoNtpServer'] = array();
      foreach($this->gotoNtpServer as $server){
        $this->attrs['gotoNtpServer'][] = $server;
      }
    }


    /***************
      Write to ldap 
     ***************/

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      if(!$this->didAction){
        $this->handle_post_events("add");
      }
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs);

      if(!$this->didAction){
        $this->handle_post_events("modify");
      }
    }
    show_ldap_error($ldap->get_error(), _("Saving workstation failed"));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
