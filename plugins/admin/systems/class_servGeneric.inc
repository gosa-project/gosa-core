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

  /* attribute list for save action */
  var $attributes= array("cn", "description");
  var $objectclasses= array("top", "goServer");

  var $netConfigDNS;

  function servgeneric ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }
    $this->netConfigDNS = new termDNS($this->config,$this->dn,$this->objectclasses);
    /* Save dn for later references */
    $this->orig_dn= $this->dn;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Do we represent a valid server? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no server features.")."</b>";
      return($display);
    }

    /* Check for action */
    if (isset($_POST['action'])){
      switch($_POST['action']){
        case 'wake':
          $cmd= search_config($this->config->data['TABS'], "servgeneric", "WAKECMD");
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
          $cmd= search_config($this->config->data['TABS'], "servgeneric", "REBOOTCMD");
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
          $cmd= search_config($this->config->data['TABS'], "servgeneric", "HALTCMD");
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

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->acl, $attr));
      $smarty->assign("$attr", $this->$attr);
    }
    $smarty->assign("staticAddress", "");
    $smarty->assign("base_select", $this->base);

    /* Assign status */
    $query= "fping -q -r 1 -t 500 ".$this->cn;
    exec ($query, $dummy, $retval);

    /* Offline */
    if ($retval == 0){
      $smarty->assign("actions", array("halt" => _("Switch off"), "reboot" => _("Reboot")));
    } else {
      $smarty->assign("actions", array("wake" => _("Wake up")));
    }


    /* Show main page */
    $smarty->assign("netconfig", $this->netConfigDNS->execute());

    return($smarty->fetch (get_template_path('server.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error());

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }
    $this->handle_post_events("remove");
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
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());
    $this->dn= "cn=".$this->cn.",ou=servers,ou=systems,".$this->base;

    /* must: cn */
    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= _("The required field 'Server name' is not set.");
    }

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "server", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a server on this 'Base'.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if ($attrs['dn'] != $this->orig_dn){
            if(!preg_match("/,ou=incoming,/",$attrs['dn'])){
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
      $mode= "add";
    } else {
   
      /* cn is not case sensitive for ldap, but for php it is!! */ 
      if($this->config->current['DNMODE'] == "cn"){
        if (strtolower($this->orig_dn) != (strtolower($this->dn))){
          $this->move($this->orig_dn, $this->dn);
          plugin::save();
        }
      }else{
        if ($this->orig_dn != $this->dn){
          $this->move($this->orig_dn, $this->dn);
          plugin::save();
        }
      }
  
      $ldap->cd($this->dn);
      $this->cleanup();
$ldap->modify ($this->attrs); 

      $mode= "modify";
    }
    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save($this->dn);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events($mode);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
