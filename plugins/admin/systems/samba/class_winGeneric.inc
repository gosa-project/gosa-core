<?php

class wingeneric extends plugin
{
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

  var $orig_cn;
  var $orig_base;
  var $baseSelector;

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn", "description","shadowLastChange",
                            "uidNumber","gidNumber","loginShell","gecos","shadowMin","shadowWarning",
                            "shadowInactive","uid","cn","sn","givenName","homeDirectory","sambaSID",
                            "sambaPrimaryGroupSID","displayName", "sambaPwdMustChange",
                            "sambaNTPassword","sambaPwdLastSet","sambaAcctFlags");
  var $objectclasses= array("posixAccount","person","organizationalPerson","inetOrgPerson","shadowAccount","sambaSamAccount","top");

  var $view_logged = FALSE;

  function __construct(&$config, $dn= NULL, $parent= NULL)
  {
    plugin::__construct ($config, $dn, $parent);
    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses,FALSE,"uid");
    $this->netConfigDNS->cn =  preg_replace("/\\\$\$/","",$this->uid);
    $this->netConfigDNS->MACisMust = FALSE;
    $this->netConfigDNS->IPisMust = FALSE;

    // The combination of the used objectClasses may differ, remember the initial ones.
    if(isset($this->attrs['objectClass'])){
        $this->objectclasses = array();
        for($i=0; $i < $this->attrs['objectClass']['count'] ;  $i++ ){
            $this->objectclasses[] = $this->attrs['objectClass'][$i];
        }
    }

    /* Set base */
    if ($this->dn == "new"){
      $ui = get_userinfo();
      $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
      $this->cn= "";
    } elseif(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", $this->dn)){
      $this->base= preg_replace ("/^[^,]+,".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"), '/')."/i", "", $this->dn);
    } else {
      $this->base= preg_replace ("/^[^,]+,".preg_quote(get_winstations_ou(), '/')."/i", "", $this->dn);
    }

    $this->cn= preg_replace("/\\\$\$/","",$this->cn);

    /* Save dn for later references */
    $this->orig_dn= $this->dn;
    $this->orig_base= $this->base;
    $this->orig_cn= $this->cn;

    /* Instanciate base selector */
    $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
    $this->baseSelector->setSubmitButton(false);
    $this->baseSelector->setHeight(300);
    $this->baseSelector->update(true);
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

    $theme = getThemeName();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","winworkstation/".get_class($this),$this->dn);
    }


    /* Do we represent a valid phone? */
    if (!$this->is_account && $this->parent === NULL){
      switch ($theme) {
        case 'classic':
          $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
          break;
        
        default:
          $symbol = "<i class='material-icons'>report</i>";
          break;
    }
    $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("network"))."</div>";
      return($display);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();

    
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    /* Assign attributes */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", set_post($this->$attr));
    }
    
    $smarty->assign("base", $this->baseSelector->render());

    /* Show main page */
    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    return($smarty->fetch (get_template_path('wingeneric.tpl', TRUE,dirname(__FILE__))));
  }

  function remove_from_parent()
  {
    $this->netConfigDNS->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    new log("remove","winworkstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
    }
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

    /* Refresh base */
    if ($this->acl_is_moveable($this->base)){
      if (!$this->baseSelector->update()) {
        msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
      }
      if ($this->base != $this->baseSelector->getBase()) {
        $this->base= $this->baseSelector->getBase();
        $this->is_modified= TRUE;
      }
    }

  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    $message= array_merge($message, $this->netConfigDNS->check());
    $this->dn= "cn=".$this->uid.",".get_ou("wingeneric", "sambaMachineAccountRDN").$this->base;

    if(!$this->acl_is_createable()){
      $message[]= msgPool::permCreate();
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
            $message[]= msgPool::duplicated(_("Name"));
            break;
          }
        }
      }
    }

    // Check if a wrong base was supplied
    if(!$this->baseSelector->checkLastBaseUpdate()){
      $message[]= msgPool::check_base();;
    }

    /* Check if we are allowed to create or move this object
     */
    if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
      $message[] = msgPool::permCreate();
    }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
      $message[] = msgPool::permMove();
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

    $curOcs = $this->attrs['objectClass'];
    $this->cleanup();

    // Prepare list of object classes
    $ocs = array();
    if(!empty($this->netConfigDNS->macAddress)){
        $ocs [] = 'ieee802Device';
    }else{
        $curOcs = array_remove_entries(array('ieee802Device'), $curOcs);
        $this->attrs['macAddress'] = array();
    } 
    if(!empty($this->netConfigDNS->ipHostNumber)){
        $ocs [] = 'ipHost';
    }else{
        $curOcs = array_remove_entries(array('ipHost'), $curOcs);
        $this->attrs['ipHostNumber'] = array();
    } 

    $this->attrs['objectClass']=$curOcs;

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
      $ldap->modify ($this->attrs); 
      new log("modify","winworkstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("modify");
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    // Prepare list of object classes
    $ocs = array();
    if(!empty($this->netConfigDNS->macAddress)){
        $ocs [] = 'ieee802Device';
    } 
    if(!empty($this->netConfigDNS->ipHostNumber)){
        $ocs [] = 'ipHost';
    } 

    $this->netConfigDNS->objectclasses=$ocs;
    $this->netConfigDNS->cn =  preg_replace("/\\\$\$/","",$this->uid);
    $this->netConfigDNS->save();
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

                  "plRequirements"=> array(
                      'ldapSchema' => array(
                          'posixAccount' => '',
                          'organizationalPerson' => '',
                          'person' => '',
                          'inetOrgPerson' => '',
                          'gosaAccount' => '>=2.7',
                          'shadowAccount' => '',
                          'sambaSamAccount' => ''
                          ),
                      'onFailureDisablePlugin' => array(get_class())
                      ),


                  "plProperties" =>
                  array(
                      array(
                          "name"          => "sambaMachineAccountRDN",
                          "type"          => "rdn",
                          "default"       => "ou=winstations,ou=systems,",
                          "description"   => _("RDN for samba machine account storage."),
                          "check"         => "gosaProperty::isRdn",
                          "migrate"       => "migrate_sambaMachineAccountRDN",
                          "group"         => "plugin",
                          "mandatory"     => FALSE
                          )
                      ),

          "plProvidedAcls"=> array(
            "cn"                  => _("Name"),
            "base"                => _("Base"),
            "description"         => _("Description"))
          ));
  }


}


?>
