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
  var $gotoNtpServer= "";
  var $gotoSndModule= "";
  var $gotoFloppyEnable= "";
  var $gotoCdromEnable= "";
  var $ghCpuType= "-";
  var $ghMemSize= "-";
  var $macAddress= "";
  var $ipHostNumber= "";
  var $ghUsbSupport= "-";
  var $ghNetNic= array();
  var $ghIdeDev= array();
  var $ghScsiDev= array();
  var $ghGfxAdapter= "-";
  var $ghSoundAdapter= "-";
  var $gotoLastUser= "-";

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $orig_dn= "";

  /* Plugin side filled */
  var $modes= array();

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array("gotoMode", "gotoTerminalPath", "macAddress",
      "gotoSwapServer", "gotoSyslogServer", "gotoNtpServer",
      "gotoFloppyEnable", "gotoCdromEnable", "cn", "gotoSndModule",
      "ghCpuType", "ghMemSize", "ipHostNumber", "ghUsbSupport",
      "ghGfxAdapter", "ghSoundAdapter", "gotoLastUser");
  var $objectclasses= array("top", "gotoTerminal", "GOhard");

  function termgeneric ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Read arrays */
    foreach (array("ghNetNic", "ghIdeDev", "ghScsiDev") as $val){
      if (!isset($this->attrs[$val])){
        continue;
      }
      for ($i= 0; $i<$this->attrs[$val]['count']; $i++){
        array_push($this->$val, $this->attrs[$val][$i]);
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
      switch($_POST['saction']){
        case 'wake':
          $cmd= search_config($this->config->data['TABS'], "termgeneric", "WAKECMD");
          if ($cmd == ""){
            print_red(_("No WAKECMD definition found in your gosa.conf"));
          } else {
            exec ($cmd." ".$this->macAddress, $dummy, $retval);
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

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("staticAddress", "");

    $smarty->assign("bases", $this->config->idepartments);

    /* Check if terminal is online */
    $query= "fping -q -r 1 -t 500 ".$this->cn;
    exec ($query, $dummy, $retval);

    /* Offline */
    if ($retval == 0){
      $smarty->assign("actions", array("halt" => _("Switch off"), "reboot" => _("Reboot")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up")));
    }
    /* Arrays */
    $smarty->assign("modes", $this->modes);

    $tmp2 = array(); 
    foreach($this->config->data['SERVERS']['NFS'] as $server){
      if($server != "default"){
        $tmp = split("\|",$server);
        $tmp3= split(":",$tmp[0]);

        $servername = $tmp3[0];
        $nfsname    = $tmp3[1];  

        $path ="";
        if(isset($tmp[4])){
          $path       = $tmp[4];  
        }

        $tmp2[$servername.":".$path]= $servername.":".$path; 
      }else{
        $tmp2[$server]=$server;
      }
    }
  
    $smarty->assign("nfsservers", $tmp2);
    $smarty->assign("syslogservers", $this->config->data['SERVERS']['SYSLOG']);
    $smarty->assign("ntpservers", $this->config->data['SERVERS']['NTP']);

    /* Variables */
    foreach(array("base", "gotoMode", "gotoTerminalPath", "gotoSwapServer",
          "gotoSyslogServer", "gotoNtpServer") as $val){

      $smarty->assign($val."_select", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }
    $smarty->assign("ipHostNumber", $this->ipHostNumber);
    $smarty->assign("macAddress", $this->macAddress);

    /* Show main page */
    $smarty->assign("netconfig", dirname(__FILE__)."/network.tpl");
    $smarty->assign("actionACL", chkacl($this->acl, 'action'));
    return($smarty->fetch (get_template_path('terminal.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn);
    if($ldap->count()){

      $ldap->rmdir($this->dn);
      show_ldap_error($ldap->get_error());

      /* Optionally execute a command after we're done */
      $this->handle_post_events("remove");

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

    /* Save base, since this is no LDAP attribute */
    if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
      $this->base= $_POST['base'];
    }

    /* Save terminal path to parent since it is used by termstartup, too */
    $this->parent->by_object['termstartup']->gotoTerminalPath=
      $this->gotoTerminalPath;
  }


  /* Check supplied data */
  function check()
  {
    $message= array();

    /* Permissions for that base? */
    $this->dn= "cn=".$this->cn."ou=terminals,ou=systems,".$this->base;

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "group", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a terminal on this 'Base'.");
    }

    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= _("The required field 'Terminal name' is not set.");
    }

    if ($this->orig_dn == 'new'){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(&(objectClass=gotoTerminal)(cn=".$this->cn."))", array("cn"));
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

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    /* Strip out 'default' values */
    foreach (array("gotoTerminalPath", "gotoSwapServer", "gotoSyslogServer") as $val){

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

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
      $this->handle_post_events("modify");
    }
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
