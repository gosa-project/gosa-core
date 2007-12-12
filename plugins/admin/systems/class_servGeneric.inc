<?php

class servgeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $ignore_account= TRUE;
  var $interfaces= array();

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $l= "";
  var $description= "";
  var $orig_dn= "";
  var $didAction= FALSE;

  /* attribute list for save action */
  var $attributes= array("cn", "description","gotoMode");
  var $objectclasses= array("top", "GOhard", "goServer");

  var $gotoMode= "locked";

  var $netConfigDNS;
  var $modes = array();
  var $ui   ;
  var $mapActions   = array("reboot"          => "",
                            "instant_update"  => "softupdate",
                            "localboot"       => "localboot",
                            "update"          => "sceduledupdate",
                            "reinstall"       => "install",
                            "rescan"          => "",
                            "memcheck"        => "memcheck",
                            "sysinfo"         => "sysinfo");

  var $fai_activated  =FALSE;
  var $view_logged = FALSE;

  function servgeneric (&$config, $dn= NULL, $parent= NULL)
  {
    /* Check if FAI is activated */
    $tmp = $config->search("faiManagement", "CLASS",array('menu','tabs'));
    
    if(!empty($tmp)){
      $this->fai_activated = TRUE;
    }

    plugin::plugin ($config, $dn, $parent);

    $this->ui = get_userinfo();
    $this->modes["active"]= _("Activated");
    $this->modes["locked"]= _("Locked");

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }
    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
    $this->netConfigDNS->set_acl_category("server");
    $this->netConfigDNS->set_acl_base($this->base);
       
    /* Save dn for later references */
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
      new log("view","server/".get_class($this),$this->dn);
    }

    /* Do we represent a valid server? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no server features.")."</b>";
      return($display);
    }

    /* Check for action */
    if (isset($_POST['action']) && $this->acl_is_writeable("FAIstate")){
      $cmd = $this->config->search("workgeneric", "ACTIONCMD",array('tabs'));
      if ($cmd == ""){
        print_red(_("No ACTIONCMD definition found in your gosa.conf"));
      } else {

        exec ($cmd." ".$this->netConfigDNS->macAddress." ".escapeshellarg($_POST['saction']), $dummy, $retval);
        if ($retval != 0){
          print_red(sprintf(_("Execution of '%s' failed!"), $cmd));
        } else {
          $this->didAction= TRUE;

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
          }
          show_ldap_error($ldap->get_error(), sprintf(_("Saving of system server/generic (FAIstate) with dn '%s' failed."),$this->dn)); 
        }
      }
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once ){
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
  
        /* Only accept allowed bases */
        $tmp = $this->get_allowed_bases();
        if($tmp[$this->dialog->isSelected()]){
          $this->base = $this->dialog->isSelected();
        }
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Fill templating stuff */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Assign base ACL */
    $smarty->assign("bases"   , $this->get_allowed_bases());

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
    }

    $smarty->assign("staticAddress", "");
    $smarty->assign("base_select", $this->base);

    /* Assign status */
    $query= "fping -q -r 1 -t 500 ".$this->cn;
    exec ($query, $dummy, $retval);

    /* Offline */
    if ($retval == 0){
      $smarty->assign("actions", array("halt" => _("Switch off"), "reboot" => _("Reboot"),
                                       "instant_update" => _("Instant update"),
                                       "update" => _("Scheduled update"),
                                       "reinstall" => _("Reinstall"),
                                       "rescan" => _("Rescan hardware"),
                                       "memcheck" => _("Memory test"),
                                       "localboot" => _("Force localboot"),
                                       "sysinfo"  => _("System analysis")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up"),
                                       "reinstall" => _("Reinstall"),
                                       "update" => _("Scheduled update"),
                                       "memcheck" => _("Memory test"),
                                       "localboot" => _("Force localboot"),
                                       "sysinfo"  => _("System analysis")));
    }

    /* Show main page */
    $smarty->assign("fai_activated",$this->fai_activated);

    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    $smarty->assign("modes", $this->modes);

    return($smarty->fetch (get_template_path('server.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if ((!$this->initially_was_account) || (!$this->acl_is_removeable())){
      return;
    }

    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);

    new log("remove","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of system server/generic with dn '%s' failed."),$this->dn)); 

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }
    $this->handle_post_events("remove",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
  }


  /* Save data to object */
  function save_object()
  {
    /* Save current base, to be able to revert to last base, 
        if new base is invalid or not allowed to be selected */
    $base_tmp = $this->base;
    plugin::save_object();
    $this->netConfigDNS->save_object();

    /* Get base selection */
    $tmp = $this->get_allowed_bases();
    if(isset($_POST['base'])){
      if($tmp[$_POST['base']]){
        $this->base = $_POST['base'];
      }else{
        $this->base = $base_tmp;
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());
    $this->dn= "cn=".$this->cn.",ou=servers,ou=systems,".$this->base;

    /* must: cn */
    if ($this->cn == ""){
      $message[]= _("The required field 'Server name' is not set.");
    }
    
    /* Check if given name is a valid host/dns name */
    if(!is_dns_name($this->cn)){
      $message[] = _("Please specify a valid name for this object.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if ($attrs['dn'] != $this->orig_dn){
            if(!preg_match("/cn=dhcp,/",$attrs['dn']) && !preg_match("/,ou=incoming,/",$attrs['dn']) && preg_match("/,ou=servers,ou=systems,/",$attrs['dn'])){
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
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $mode= "add";
    } else {
   
      /* cn is not case sensitive for ldap, but for php it is!! */ 
      if($this->config->current['DNMODE'] == "cn"){
        if (strtolower($this->orig_dn) != (strtolower($this->dn))){
          $this->recursive_move($this->orig_dn, $this->dn);
          plugin::save();
        }
      }else{
        if ($this->orig_dn != $this->dn){
          $this->recursive_move($this->orig_dn, $this->dn);
          plugin::save();
        }
      }
  
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      $mode= "modify";
    }
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system server/generic with dn '%s' failed."),$this->dn)); 
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save();

    /* Optionally execute a command after we're done */
    if(!$this->didAction){
      $this->handle_post_events($mode,array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }
  }


  /* Display generic part for server copy & paste */
  function getCopyDialog()
  {
    $vars = array("cn");

    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $smarty->assign("object","server");
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

  }
  


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName"   => _("Generic"),
          "plDescription" => _("Server generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 1,
          "plSection"     => array("administration"),           
          "plCategory"    => array("server" => array("description"  => _("Server"),
                                                     "objectClass"  => "goServer")),
          "plProvidedAcls"=> array(
            "cn"           => _("Name"),
            "description"  => _("Description"),
            "gotoMode"     => _("Goto mode"),
            "base"         => _("Base"),
            "FAIstate"     => _("Action flag"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
