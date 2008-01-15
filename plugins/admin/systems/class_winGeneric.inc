<?php

class wingeneric extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage component base objects";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $interfaces= array();

  /* Needed values and lists */
  var $base= "";
  var $cn= "";
  var $description= "";
  var $orig_dn= "";
  var $shadowLastChange="";
  var $uidNumber="";
  var $gidNumber="";
  var $loginShell="";
  var $gecos="";
  var $shadowMin="";
  var $shadowWarning="";
  var $shadowInactive="";
  var $uid="";
  var $sn="";
  var $givenName="";
  var $homeDirectory="";
  var $sambaSID="";
  var $sambaPrimaryGroupSID="";
  var $displayName="";
  var $sambaPwdMustChange="";
  var $sambaNTPassword="";
  var $sambaPwdLastSet="";
  var $sambaAcctFlags="";
  var $netConfigDNS;
  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn", "description","shadowLastChange",
                            "uidNumber","gidNumber","loginShell","gecos","shadowMin","shadowWarning",
                            "shadowInactive","uid","cn","sn","givenName","homeDirectory","sambaSID",
                            "sambaPrimaryGroupSID","displayName", "sambaPwdMustChange",
                            "sambaNTPassword","sambaPwdLastSet","sambaAcctFlags");
  var $objectclasses= array("posixAccount","person","organizationalPerson","inetOrgPerson","gosaAccount","shadowAccount","sambaSamAccount","top");

  var $view_logged = FALSE;

  function wingeneric (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses);
    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,".normalizePreg(get_winstations_ou())."/", "", $this->dn);
    }

    /* Save dn for later references */
    $this->orig_dn= $this->dn;

    $this->cn= preg_replace("/\\\$\$/","",$this->cn);
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
      new log("view","winworkstation/".get_class($this),$this->dn);
    }


    /* Do we represent a valid phone? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no network features.")."</b>";
      return($display);
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this);
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

    /* Fill templating stuff */
    $smarty= get_smarty();

    // Undefined index in wingeneric.tpl ... 
    $smarty->assign("bases", $this->config->idepartments);

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
    }
    
    $smarty->assign("base_select", $this->base);

    /* Show main page */
    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    return($smarty->fetch (get_template_path('wingeneric.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    new log("remove","winworkstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of system wingeneric/generic with dn '%s' failed."),$this->dn));
    $this->handle_post_events("remove");

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
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

    $this->netConfigDNS->save_object();

    /* Set new base if allowed */
    $tmp = $this->get_allowed_bases();
    if(isset($_POST['base'])){
      if(isset($tmp[$_POST['base']])){
        $this->base= $_POST['base'];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());
    $this->dn= "cn=".$this->cn.",".get_ou('componentou').$this->base;
    
    /* Set new acl base */
    if($this->dn == "new") {
      $this->set_acl_base($this->base);
    }

    if(!$this->acl_is_createable() && $this->dn == "new"){
      $message[]= _("You have no permissions to create a component on this 'Base'.");
    }

    if ($this->orig_dn != $this->dn){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->base);
      $ldap->search ("(cn=".$this->cn.")", array("cn"));
      if ($ldap->count() != 0){
        while ($attrs= $ldap->fetch()){
          if(preg_match("/cn=dhcp,/",$attrs['dn'])){
            continue;
          }
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

    if(($this->gosaUnitTag) && (!in_array_ics("gosaAdministrativeUnitTag",$this->attrs['objectClass']))){
      $this->attrs['objectClass'][] = "gosaAdministrativeUnitTag";
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      new log("create","winworkstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("add");
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }

      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      new log("modify","winworkstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("modify");
    }

    $this->netConfigDNS->cn =  preg_replace("/\\\$\$/","",$this->cn);
    $this->netConfigDNS->save();
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system wingeneric/generic with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

  /* Return plugin informations for acl handling
  #FIXME FAIscript seams to ununsed within this class... */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Win generic"),
          "plDescription" => _("Windows workstation generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("winworkstation" => array("description"  => _("Win workstation"),
                                                          "objectClass"  => "gotoWorkstation")),
          "plProvidedAcls"=> array(
            "cn"                  => _("Workstation name"),
            "description"         => _("Description"))
          ));
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
