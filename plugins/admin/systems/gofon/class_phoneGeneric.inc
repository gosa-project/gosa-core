<?php

class phoneGeneric extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage phone base objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Generic terminal attributes */
  var $interfaces= array();
  var $ignore_account= TRUE;

  /* Needed values and lists */
  var $base             = "";
  var $cn               = "";
  var $description      = "";
  var $orig_dn          = "";
  var $goFonType        = "";
  var $goFonDmtfMode    = "";
  var $goFonHost        = "";
  var $goFonDefaultIP   = "dynamic";
  var $goFonQualify     = "";
  var $goFonAuth        = "";
  var $goFonSecret      = "";
  var $goFonInkeys      = "";
  var $goFonPermit      = array();
  var $goFonDeny        = array();
  var $goFonOutkey      = "";
  var $goFonTrunk       = "";
  var $goFonAccountCode = "";
  var $goFonMSN         = "";
  var $selected_categorie    = 0;
  var $netConfigDNS;
  var $view_logged      = FALSE;

  /* attribute list for save action */
  var $attributes= array("cn", "description",
                          "goFonType","goFonDmtfMode","goFonHost","goFonDefaultIP",
                          "goFonQualify","goFonAuth","goFonSecret","goFonInkeys","goFonOutkey",
                          "goFonTrunk","goFonAccountCode","goFonMSN","selected_categorie","goFonPermit","goFonDeny" 
                         );

  /* this array defines which attributes are schown / saved for the different type of phones */ 
  var $usedattrs =    array( "0"=>array("cn", "description", 
                                  "goFonType","goFonDmtfMode","goFonHost","goFonDefaultIP",
                                  "goFonQualify"),
                             "1"=>array("cn", "description", 
                                  "goFonType","goFonHost","goFonDefaultIP",
                                  "goFonQualify","goFonAuth","goFonSecret","goFonInkeys","goFonOutkey",
                                  "goFonTrunk","goFonAccountCode","selected_categorie","goFonPermit","goFonDeny"),
                             "2"=>array("cn", "description", "goFonMSN"));
  

  var $objectclasses= array("top", "goFonHardware");

  function phoneGeneric (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    $this->netConfigDNS = new termDNS($this->config,$this,$this->objectclasses, true);

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      $this->base= dn2base($ui->dn);
      $this->cn= "";
    } else {
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }
  
    if($this->goFonMSN != ""){
      $this->selected_categorie = 2;
    }elseif($this->goFonAccountCode != ""){
      $this->selected_categorie = 1 ;
    
      if(isset($this->attrs['goFonPermit']['count'])){
        unset ($this->attrs['goFonPermit']['count']);
        $this->goFonPermit=$this->attrs['goFonPermit'];
      }  
     
      if(isset($this->attrs['goFonDeny']['count'])){
        unset ($this->attrs['goFonDeny']['count'])   ;
        $this->goFonDeny=$this->attrs['goFonDeny'];
      } 

    } else {
      $this->selected_categorie = 0;
    }

    if($this->goFonDefaultIP!="dynamic"){
      $this->goFonDefaultIP = "network";
    }

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
      new log("view","phone/".get_class($this),$this->dn);
    }

    /* Do we represent a valid phone? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no phone features.")."</b>";
      return($display);
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
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

    /* handle Permit Add*/
    if(isset($_POST['goFonPermitAdd']) && $this->acl_is_writeable("goFonPermit")){
      if(isset($_POST['goFonPermitNew'])){
        if(is_string($this->goFonPermit)){
          $this->goFonPermit=array();
        }
        $new = $_POST['goFonPermitNew'];
        if(strlen($new)> 1) {
          $this->goFonPermit[]= $new;
        }
      }
    }
      
    /* handle Deny Add*/
    if(isset($_POST['goFonDenyAdd']) && $this->acl_is_writeable("goFonDeny")){
      if(isset($_POST['goFonDenyNew'])){
        if(is_string($this->goFonDeny)){
          $this->goFonDeny=array();
        }
        $new = $_POST['goFonDenyNew'];
        if(strlen($new)> 1) {
          $this->goFonDeny[]= $new;
        }
      }
    }

    /* Handle Permit Deletion*/
    if(isset($_POST['goFonPermitDel']) && $this->acl_is_writeable("goFonPermit")){
      if(isset($_POST['goFonPermitS'])){
        if(is_string($this->goFonPermit)){
          $this->goFonPermit=array();
        }
        $new = $_POST['goFonPermitS'];
        $tmp = array_flip($this->goFonPermit);
        unset($tmp[$new]);
        $this->goFonPermit=array();
        foreach(array_flip($tmp) as $tm){
          $this->goFonPermit[]=$tm;
        }
      }
    }


    /* Handle Permit Deletion*/
    if(isset($_POST['goFonDenyDel']) && $this->acl_is_writeable("goFonDeny")){
      if(isset($_POST['goFonDenyS'])){
        if(is_string($this->goFonDeny)){
          $this->goFonDeny=array();
        }
        $new = $_POST['goFonDenyS'];
        $tmp = array_flip($this->goFonDeny);
        unset($tmp[$new]);
        $this->goFonDeny=array();
        foreach(array_flip($tmp) as $tm){
          $this->goFonDeny[]=$tm;
        }
      }
    }
  
    /* Fill templating stuff */
    $smarty= get_smarty();

    $smarty->assign("bases", $this->get_allowed_bases());

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $smarty->assign("staticAddress","<font class=\"must\">*</font>");// $this->config->idepartments);

    /* Create Arrays for samrty select boxes */
    $smarty->assign("categories",     array("SIP","IAX","CAPI"));
    $smarty->assign("goFonTypes",     array("peer"      =>"peer"      ,"user"   =>"user"    ,"friend" =>"friend"));
    $smarty->assign("goFonDmtfModes", array("inband"    =>"inband"    ,"rfc2833"=>"rfc2833" ,"info"   =>"info"));
    $smarty->assign("goFonAuths",     array("plaintext" =>"plaintext" ,"md5"    =>"md5"     /*,"rsa"    =>"rsa"*/));
    $smarty->assign("goFonTrunks",    array("yes" =>_("yes") ,"no" => _("no")));

    /* deativate all fields that are not used by the specified type */
    foreach($this->attributes as $att){
      if((!in_array($att,$this->usedattrs[$this->selected_categorie]))){
        $smarty->assign($att."USED", "disabled" );
        $smarty->assign($att, "");
      }else{
        $smarty->assign($att."USED", "" );
        $smarty->assign($att, $this->$att);
      }
    }
    
    $smarty->assign("selected_categorie",$this->selected_categorie);

    /* Assign attributes */
    $smarty->assign("base_select", $this->base);
    $smarty->assign("goFonDefaultIPs",array("dynamic"=>_("dynamic"),"network"=>_("Networksettings")));
 
    /* Show main page */
    $str = $this->netConfigDNS->execute();
    if(is_object($this->netConfigDNS->dialog)){
      return($str);
    }
    $smarty->assign("netconfig", $str);
    $smarty->assign("phonesettings", dirname(__FILE__)."/phonesettings.tpl");
    return($smarty->fetch (get_template_path('phone.tpl', TRUE)));
  }

  function remove_from_parent()
  {
    if($this->acl_is_removeable()){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);

      $ldap->search ("(&(objectClass=goFonAccount)(goFonHardware=".$this->cn."))", array("uid","cn"));
      while ($attr =  $ldap->fetch()){
        print_red(sprintf(_("Can't delete because there are users which are depending on this phone. One of them is user '%s'."),
              ($attr['uid'][0]." - ".$attr['cn'][0])));
        return;
      }

      $this->netConfigDNS->remove_from_parent();
      $ldap->rmdir($this->dn);
  
      new log("remove","phone/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      show_ldap_error($ldap->get_error(), sprintf(_("Removing of system phone/generic with dn '%s' failed."),$this->dn));
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

    $this->dn= "cn=".$this->cn.",".get_ou('phoneou').$this->base;

    /* To check for valid ip*/
    if($this->netConfigDNS->ipHostNumber == ""){
       $message[]= _("The required field IP address is empty.");
    } else {
      if (!tests::is_ip($this->netConfigDNS->ipHostNumber)){
        $message[]= _("The field IP address contains an invalid address.");
      }
    }

    /* Check if given name is a valid host/dns name */
    if(!tests::is_dns_name($this->cn) ){
      $message[] = _("Please specify a valid name for this object.");
    }

    if ($this->cn == ""){
      $message[]= _("The required field 'Phone name' is not set.");
    }
    if ($this->cn == "0"){ 
      $message[]= _("The 'Phone name' '0' is reserved and cannot be used.");
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
   
   
    /* only to define which attrs to save*/ 
    $mode = $this->selected_categorie;

    /* Remove all unwanted attrs */
    foreach($this->attributes as $att){
      
      /* Check all attributes, if they are needed for this type of phone */
      if(!in_array($att,$this->usedattrs[$mode])){
        $this->attrs[$att] = array();
      }
    }

    /* unset the categorie*/
    unset($this->attrs['selected_categorie']);
    
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

    if($this->goFonDefaultIP!="dynamic"){
      $this->attrs['goFonDefaultIP'] = $this->netConfigDNS->ipHostNumber;
    }    

    $this->attrs = $this->netConfigDNS->getVarsForSaving($this->attrs);

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    if ($this->orig_dn == 'new'){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      new log("create","phone/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
      $this->handle_post_events("add",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    } else {
      if ($this->orig_dn != $this->dn){
        $this->move($this->orig_dn, $this->dn);
      }

      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      new log("modify","phone/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      // $user_phone_reload   
      $ldap->cd ($this->config->current['BASE']); 
      $user_phone_assignment = $ldap->fetch($ldap->search("(&(objectClass=goFonAccount)(goFonHardware=".$this->cn."))",array("uid")));
      if($user_phone_assignment){
        $usertab= new usertabs($this->config,$this->config->data['TABS']['USERTABS'], $user_phone_assignment['dn']);
        $usertab->by_object['phoneAccount']->is_modified = true;
        $usertab->save();
        unset($usertab);
      }
      $this->handle_post_events("modify",array("macAddress" => $this->netConfigDNS->macAddress,"ipHostNumber" => $this->netConfigDNS->ipHostNumber));
    }
    $this->netConfigDNS->cn = $this->cn;    
    $this->netConfigDNS->save();
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of system phone/generic with dn '%s' failed."),$this->dn));
  }


  /* Display generic part for server copy & paste */
  function getCopyDialog()
  {
    $vars = array("cn");
    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $smarty->assign("object","phone");
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

    $source_o = new phoneGeneric($this->config, $source['dn']);
    $this->selected_categorie = $source_o->selected_categorie;
  }


  /* Return plugin informations for acl handling
      #FIXME FAIscript seams to ununsed within this class... */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Generic"),
          "plDescription" => _("Phone generic"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("phone" => array("description"  => _("Phone hardware"),
                                                    "objectClass"  => "goFonHardware")),
          "plProvidedAcls"=> array(
            "cn"                  => _("Name"),
            "base"                => _("Base"),
            "description"         => _("Description"),
            "goFonType"           => _("SIP Mode"),
            "goFonDmtfMode"       => _("SIP DTMF mode"),
            "goFonDefaultIP"      => _("SIP Default ip"),
            "goFonQualify"        => _("SIP Qualify"),
            "goFonAuth"           => _("IAX authentication type"),
            "goFonSecret"         => _("IAX secret"),
            "goFonAccountCode"    => _("IAX account code"),
            "goFonTrunk"          => _("IAX trunk lines"),
            "goFonPermit"         => _("IAX permit settings"),
            "goFonDeny"           => _("IAX deny settings"),
            "goFonMSN"            => _("CAPI MSN"),
            "categorie"  => _("Hardware type"))
          ));
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
