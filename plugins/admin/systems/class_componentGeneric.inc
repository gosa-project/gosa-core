<?php

class componentGeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage component base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $interfaces= array();
  var $ignore_account= TRUE;

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $description= "";
  var $orig_dn= "";

  /* attribute list for save action */
  var $attributes= array("cn", "description");
  var $objectclasses= array("top", "device", "ipHost", "ieee802Device");
  var $netConfigDNS;

  function componentgeneric ($config, $dn= NULL)
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
    $this->netConfigDNS = new termDNS($this->config,$this->dn,$this->objectclasses, true);
    /* Save dn for later references */
    $this->orig_dn= $this->dn;
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Do we represent a valid phone? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no network features.")."</b>";
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

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign($attr."ACL", chkacl($this->acl, $attr));
      $smarty->assign("$attr", $this->$attr);
    }
    $smarty->assign("base_select", $this->base);
    $smarty->assign("baseACL", chkacl($this->acl,"base"));

    /* Show main page */
    $smarty->assign("netconfig", $this->netConfigDNS->execute());
    return($smarty->fetch (get_template_path('component.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $this->netConfigDNS->remove_from_parent();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing generic component failed"));
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
    $message= array_merge($message,$this->netConfigDNS->check());

    $this->dn= "cn=".$this->cn.",ou=netdevices,ou=systems,".$this->base;

    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= _("The required field 'Component name' is not set.");
    }

    /* To check for valid ip*/
    if($this->netConfigDNS->ipHostNumber == ""){
       $message[]= _("The required field IP address is empty.");
    } else {
      if (!is_ip($this->netConfigDNS->ipHostNumber)){
        $message[]= _("The field IP address contains an invalid address.");
      }
    }

    $ui= get_userinfo();
    $acl= get_permissions ($this->dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "component", $this->dn);
    if (chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a component on this 'Base'.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if ($attrs['dn'] != $this->orig_dn){
            $message[]= sprintf (_("There is already an entry '%s' in the base choosen by you"), $this->cn);
            break;
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

    /* If this is a new Object IP & Mac aren't set.
         IP & Mac are msut attributes, so we set this values by here. */
    if($this->orig_dn == 'new'){
      $this->attrs['ipHostNumber'] = $this->netConfigDNS->ipHostNumber;
      $this->attrs['macAddress']  = $this->netConfigDNS->macAddress;
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
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      $this->handle_post_events("modify");
    }

    $this->netConfigDNS->cn = $this->cn;
    $this->netConfigDNS->save($this->dn);

    show_ldap_error($ldap->get_error(), _("Saving generic component failed"));

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
