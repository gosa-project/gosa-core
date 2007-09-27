<?php

class servdb extends plugin
{
  /* Object information */
  var $goImapName= "";
  var $goImapName_old= "";
  var $goImapConnect= "";
  var $goImapAdmin= "";
  var $goImapPassword= "";
  var $goImapSieveServer= "";
  var $goImapSievePort= "";
  var $goKrbRealm= "";
  var $goKrbAdmin= "";
  var $goKrbPassword= "";
  var $goFaxAdmin= "";
  var $goFaxPassword= "";
  var $goLogAdmin= "";
  var $goLogPassword= "";
  var $goFonAdmin= "";
  var $goFonPassword= "";
  var $goFonAreaCode= "";
  var $goFonCountryCode= "";
  var $ignore_account= TRUE;
  var $goGlpiAdmin="";
  var $goGlpiDatabase = "";
  var $goGlpiPassword ="";
  

  /* attribute list for save action */
  var $attributes= array("goImapName", "goImapConnect", "goImapAdmin", "goImapPassword",
                         "goImapSieveServer", "goImapSievePort", "goKrbRealm", 
                         "goKrbAdmin", "goKrbPassword", "goFaxAdmin", "goFaxPassword",
                         "goLogAdmin", "goLogPassword", "goFonAdmin", "goFonPassword",
                         "goFonAreaCode", "goFonCountryCode","goGlpiAdmin","goGlpiDatabase","goGlpiPassword");
  var $objectclasses  = array("top", "goServer");
  var $init_objectclasses = array();
  var $additionaloc= array("goImapServer" => array("goImapName", "goImapConnect",
                                                "goImapAdmin", "goImapPassword",
                                                "goImapSieveServer", 
                                                "goImapSievePort"),
                          "goKrbServer" => array("goKrbRealm", "goKrbAdmin",
                                                "goKrbPassword"),
                          "goFaxServer" => array("goFaxAdmin", "goFaxPassword"),
                          "goLogDBServer" => array("goLogAdmin", "goLogPassword"),
                          "goGlpiServer" => array("goGlpiAdmin","goGlpiDatabase","goGlpiPassword"),
                          "goFonServer" => array("goFonAdmin", "goFonPassword",
                                                "goFonAreaCode", "goFonCountryCode"));
  var $orig_dn = "";

  function servdb ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Save original dn */
    $this->orig_dn = $dn;

    /* Make dynamic list of objectClasses */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->attrs['objectClass']) && in_array($oc, $this->attrs['objectClass'])){
        $this->objectclasses[$oc]= $oc;
      }
    }

    /* Remember used databases */
    $this->init_objectclasses = $this->objectclasses;

    /* We need to save the historical state of goImapName for later checking */
    $this->goImapName_old= $this->goImapName;

    /* Always is account... */
    $this->is_account= TRUE;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();

    /* Attributes... */
    foreach ($this->attributes as $attr){
      $smarty->assign("$attr", $this->$attr);
      $smarty->assign("$attr"."ACL", chkacl($this->acl, $attr));
    }

    /* Classes... */
    foreach ($this->additionaloc as $oc => $dummy){
      if (isset($this->objectclasses[$oc])){
        $smarty->assign("$oc", "checked");
        $smarty->assign("$oc"."State", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));
        
      } else {
        $smarty->assign("$oc", "");
        $smarty->assign("$oc"."ACL", chkacl($this->acl, $oc));
        $smarty->assign("$oc"."State", "disabled");
      }
    }

    return($smarty->fetch (get_template_path('servdb.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['dbtab'])){
      plugin::save_object();
      
      /* Save checkbox state */
      foreach ($this->additionaloc as $oc => $dummy){

        if(chkacl($this->acl,$oc) == ""){
          if (isset($_POST[$oc]) && $_POST[$oc] == '1'){
            $this->objectclasses[$oc]= $oc;
          } else {
            unset($this->objectclasses[$oc]);
          }
        }
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(in_array("goGlpiServer",$this->objectclasses)){
      foreach(array("goGlpiAdmin","goGlpiDatabase") as $attr){
        if(empty($this->$attr)){
          $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
        }
      }
    }

    /* All fields are marked as *must* */
    if (in_array("goImapServer", $this->objectclasses)){
      foreach (array("goImapAdmin", "goImapName") as $attr){
        if ($this->$attr == "" || preg_match("/ /", $this->$attr)){
          $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
        }
      }


      /* Check connect string */
      if (!preg_match('/^\{[^:]+:[0-9]+.*\}$/', $this->goImapConnect)){
        $message[]= sprintf(_("The imap connect string needs to be in the form '%s'."),
                    '{server-name:port/options}');
      }
      if (!preg_match('/^[0-9]+$/', $this->goImapSievePort)){
        $message[]= _("The sieve port needs to be numeric.");
      }
    }
    if (in_array("goKrbServer", $this->objectclasses)){
      foreach (array("goKrbAdmin", "goKrbRealm") as $attr){
        if ($this->$attr == "" || preg_match("/ /", $this->$attr)){
          $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
        }
      }
      /* goKrbPassword is a must field, if goKrbServer is used as objectClass */
      if ($this->goKrbPassword == "" ){
        $message[]= sprintf(_("The specified kerberos password is empty."), $attr);
      }
    }
    if (in_array("goFaxServer", $this->objectclasses)){
      if ($this->goFaxAdmin == "" || preg_match("/ /", $this->goFaxAdmin)){
        $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), "goFaxAdmin");
      }
    }
    if (in_array("goLogServer", $this->objectclasses)){
      if ($this->goLogAdmin == "" || preg_match("/ /", $this->goLogAdmin)){
        $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), "goLogAdmin");
      }
    }
    if (in_array("goFonServer", $this->objectclasses)){
      foreach (array("goFonAdmin", "goFonAreaCode", "goFonCountryCode") as $attr){
        if ($this->$attr == "" || preg_match("/ /", $this->$attr)){
          $message[]= sprintf(_("The attribute '%s' is empty or contains invalid characters."), $attr);
        }
      }
    }
    
    /* Check if we are able to remove the asterisk database. If the database is still in use skip */
    if (!in_array("goFonServer", $this->objectclasses) && in_array("goFonServer", $this->init_objectclasses)){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(goFonHomeServer=".$this->dn.")(|(uid=*)(cn=*)))",array("dn","uid","cn","goFonHomeServer"));
      if($ldap->count()){

        /* Number of entries shown in warning */
        $i    = 3;
        $str  = "";   
        while(($attrs = $ldap->fetch()) && $i >= 0){
          $i --; 
          if(isset($attrs['uid'][0])){
            $str .= $attrs['uid'][0]." ";
          }else{
            $str .= $attrs['cn'][0]." ";
          }
        }

        /* Some entries found */
        if($i != 3){
          $message[] = sprintf(_("You can't remove the asterisk database extension, it is still in use by these objects '%s'."),trim($str));
        }
      } 
    }

    if (in_array("goImapServer", $this->objectclasses) && 
        $this->goImapName != $this->goImapName_old) {
      // Attribute has changed
      if(!preg_match("/^[a-z0-9.-]+$/", $this->goImapName)) {
        $message[]= sprintf(_("The imap name string needs to be a hostname or an IP-address."));
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* Normalize lazy objectclass arrays */
    $objectclasses= array();
    foreach($this->objectclasses as $oc){
      $objectclasses[]= $oc;
    }
    
    /* Move cn to sieve server */
    $this->goImapSieveServer= $this->cn;
    
    plugin::save();
  
    $test = $this->objectclasses;
    $tmp = array_flip($this->attrs['objectClass']);

    foreach($this->additionaloc as $key=>$val) {
      unset($tmp[$key]);
      }

    $classes = (array_flip(array_merge(array_flip($test),$tmp)));

    unset($this->attrs['objectClass']);

    foreach($classes as $class){
      $this->attrs['objectClass'][]=$class;
    }


    /* Remove unneeded attributes */
    foreach ($this->additionaloc as $oc => $attrs){
      if (!in_array($oc, $this->attrs['objectClass'])){
        foreach ($attrs as $attr){
          $this->attrs[$attr]= array();
        }
      }
    }
    $this->attrs = array_reverse($this->attrs);

    /* Write to LDAP */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving server db settings failed"));

    /* Update goFonAccounts if server was is renamed.
       Set attribute goFonHomeServer to current dn
     */
    if(in_array("goFonServer",$this->objectclasses)){
      if($this->orig_dn != "new" && $this->orig_dn != $this->dn){
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(goFonHomeServer=*)(|(objectClass=goFonAccount)(objectClass=goFonConference)))",array("goFonHomeServer"));
        $dns = array();
        while($attrs = $ldap->fetch()){
          $dns[] = $attrs['dn'];
        }
        foreach($dns as $dn){
          $ldap->cd($dn);
          $ldap->modify(array("goFonHomeServer" => $this->dn));
          show_ldap_error($ldap->get_error(),sprintf(_("Updating gofon home server failed for '%s'."),$dn));
        }
      }
    
      /* Updated config list of gofon servers */
      if($this->orig_dn == "new" || $this->orig_dn != $this->dn){
        $_SESSION['config']->load_servers();
      }
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }

  function allow_remove()
  {
    /* Check if we are able to remove the asterisk database. If the database is still in use skip */
    if (in_array_ics("goFonServer", $this->init_objectclasses)){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(goFonHomeServer=".$this->dn.")(|(uid=*)(cn=*)))",array("dn","uid","cn","goFonHomeServer"));
      if($ldap->count()){

        /* Number of entries shown in warning */
        $i    = 3;
        $str  = "";
        while(($attrs = $ldap->fetch()) && $i >= 0){
          $i --;
          if(isset($attrs['uid'][0])){
            $str .= $attrs['uid'][0]." ";
          }else{
            $str .= $attrs['cn'][0]." ";
          }
        }

        /* Some entries found */
        if($i != 3){
          return(sprintf(_("You can't remove the asterisk database extension, it is still in use by these objects '%s'."),trim($str)));
        }
      }
    }
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
