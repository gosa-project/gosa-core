<?php

class govirusserver extends plugin{
	
  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goVirusServer");
  var $attributes       = array("avMaxThreads","avMaxDirectoryRecursions","avUser","avFlags","avArchiveMaxFileSize","avArchiveMaxRecursion",
                                "avArchiveMaxCompressionRatio","avDatabaseMirror","avChecksPerDay","avHttpProxyURL");
  var $StatusFlag       = "avStatus";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goVirusServer");

  var $DisplayName      = "";
  var $dn               = NULL;
  var $cn               = "";
  var $avStatus         = "";

  var $dialog           = NULL;
  var $ui               = NULL;
  var $acl              = NULL;

  var $Flags            = array("D","S","A","E");
  
  var $avFlags          = "DS";
  var $avFlagsD         = TRUE;
  var $avFlagsS         = TRUE;
  var $avFlagsA         = FALSE;
  var $avFlagsE         = FALSE;

  var $avMaxThreads                 = 5;
  var $avMaxDirectoryRecursions     = 4;    
  var $avArchiveMaxFileSize         = 4000;
  var $avArchiveMaxRecursion        = 5;
  var $avArchiveMaxCompressionRatio = 95;
  var $avChecksPerDay               = 12;

  var $avUser                       = "";
  var $avHttpProxyURL               = "";
  var $avDatabaseMirror             = "";

  function govirusserver($config,$dn)
  {
    /* Init class */
    plugin::plugin($config,$dn);
    $this->DisplayName = _("Anti virus");

    /* Get userinfo & acls */
    $this->ui = get_userinfo();

    /* Set up the users ACL's for this 'dn' */
    $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goVirusServer", $this->ui->dn);

    /* Get Flags */
    foreach($this->Flags as $flag){
      $var = "avFlags".$flag;
      if(preg_match("/".$flag."/",$this->avFlags)){
        $this->$var = TRUE;
      }
    }
  }


  function execute()
  {
    $smarty = get_smarty(); 
    if(get_class($this->parent) == "servtabs"){

      $smarty->assign("servtabs",TRUE);

      /* Do we need to flip is_account state? */
      if (isset($_POST['modify_state'])) {
        $this->is_account = !$this->is_account;
      }

      /* Show tab dialog headers */
      if ($this->is_account) {
        /* call Add Acoount to add account */
        $display = $this->show_header(_("Remove anti virus extension"), 
            _("This server has anti virus features enabled. You can disable them by clicking below."));
      } else {
        /* call remove Account */
        $display = $this->show_header(_("Add anti virus service"), 
            _("This server has anti virus features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }else{
      $display = "";
      $smarty->assign("servtabs",FALSE);
      $this->is_account = true;
    }

    /* Assign smarty vars */
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
    }

    /* Assign checkbox states */
    foreach($this->Flags as $Flag){
      $var = "avFlags".$Flag;
      $smarty->assign("avFlags".$Flag."ACL",chkacl($this->acl,$Flag));
      if($this->$var){
        $smarty->assign("avFlags".$Flag."CHK"," checked " );
      }else{
        $smarty->assign("avFlags".$Flag."CHK","");
      }
    }

    /* Assign value for max thread select box */
    $tmp = array();
    for($i = 1 ; $i <= 20 ; $i ++){
      $tmp[$i] = $i;
    }
    $smarty->assign("ThreadValues",$tmp);

    if($this->avFlagsA){
      $smarty->assign("avFlagsAState" , "" );
    }else{
      $smarty->assign("avFlagsAState" , " disabled " );
    }

    return($display.$smarty->fetch(get_template_path("goVirusServer.tpl",TRUE,dirname(__FILE__))));
  }


  /* remove this extension */
  function remove_from_parent()
  {
    if(!$this->is_account && $this->initially_was_account){

      plugin::remove_from_parent();

      /* Remove status flag, it is not a memeber of
         this->attributes, so ensure that it is deleted too */
      if(!empty($this->StatusFlag)){
        $this->attrs[$this->StatusFlag] = array();
      }

      /* Check if this is a new entry ... add/modify */
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($this->dn,array("objectClass"));
      if($ldap->count()){
        $ldap->cd($this->dn);
        $ldap->modify($this->attrs);
      }else{
        $ldap->cd($this->dn);
        $ldap->add($this->attrs);
      }
      show_ldap_error($ldap->get_error(), sprintf(_("Removing of server services/anti virus with dn '%s' failed."),$this->dn));
      $this->handle_post_events("remove");
    }
  }


  function save()
  {
    if(!$this->is_account) return;

    /* Create Flags */     
    $this->avFlags = "";
    foreach($this->Flags as $flag){
      $var = "avFlags".$flag;
      if($this->$var){
        $this->avFlags .=$flag;
      }
    }

    plugin::save();

    if(!$this->avFlagsA){
      $arr = array("avArchiveMaxFileSize","avArchiveMaxRecursion","avArchiveMaxCompressionRatio");
      foreach($arr as $attr){
        $this->attrs[$attr] =  array();
      }
      $this->attrs['avFlags'] = preg_replace("/E/","",$this->attrs['avFlags']);
    }

    /* Check if this is a new entry ... add/modify */
    $ldap = $this->config->get_ldap_link();
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    if($this->initially_was_account){
      $this->handle_post_events("modify");
    }else{
      $this->handle_post_events("add");
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of server services/anti virus with dn '%s' failed."),$this->dn));
  }

  function check()
  { 
    $message = plugin::check();

    $mustBeNumeric = array(
          "avMaxDirectoryRecursions"     =>_("Maximum directory recursions"),
          "avMaxThreads"                 =>_("Maximum threads"),
          "avArchiveMaxFileSize"         =>_("Maximum file size"),
          "avArchiveMaxRecursion"        =>_("Maximum recursions"),
          "avArchiveMaxCompressionRatio" =>_("Maximum compression ratio"),
          "avChecksPerDay"               =>_("Checks per day"));

    foreach($mustBeNumeric as $key => $trans){
      if(!is_numeric($this->$key)){
        $message[] = sprintf(_("The specified value for '%s' must be a numeric value."),$trans);
      }
    }

    return($message);
  }
  

  function save_object()
  {
    if(isset($_POST['goVirusServer'])){
      plugin::save_object();
      foreach($this->Flags as $flag){
        $var = "avFlags".$flag;
        if(isset($_POST[$var])){
          $this->$var = TRUE;
        }else{
          $this->$var = FALSE;
        }
      }
    }    
  }  

  

  /* For newer service management dialogs */
  function getListEntry()
  {
    $this->updateStatusState();
    $flag                   = $this->StatusFlag;
    $fields['Status']       = $this->$flag;
    $fields['Message']      = _("Anti virus");
    $fields['AllowStart']   = true;
    $fields['AllowStop']    = true;
    $fields['AllowRestart'] = true;
    $fields['AllowRemove']  = true;
    $fields['AllowEdit']    = true;
    return($fields);
  }

  function updateStatusState()
  {
    if(empty($this->StatusFlag)) return;

    $attrs = array();
    $flag = $this->StatusFlag;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->cn);
    $ldap->cat($this->dn,array($flag));
    if($ldap->count()){
      $attrs = $ldap->fetch();
    }
    if(isset($attrs[$flag][0])){
      $this->$flag = $attrs[$flag][0];
    }
  }

  function action_hook($add_attrs= array())
  {
    /* Find postcreate entries for this class */
    $command= search_config($this->config->data['MENU'], get_class($this), "ACTION_HOOK");
    if ($command == "" && isset($this->config->data['TABS'])){
      $command= search_config($this->config->data['TABS'], get_class($this), "ACTION_HOOK");
    }
    if ($command != ""){
      /* Walk through attribute list */
      foreach ($this->attributes as $attr){
        if (!is_array($this->$attr)){
          $command= preg_replace("/%$attr/", $this->$attr, $command);
        }
      }
      $command= preg_replace("/%dn/", $this->dn, $command);
      /* Additional attributes */
      foreach ($add_attrs as $name => $value){
        $command= preg_replace("/%$name/", $value, $command);
      }

      /* If there are still some %.. in our command, try to fill these with some other class vars */
      if(preg_match("/%/",$command)){
        $attrs = get_object_vars($this);
        foreach($attrs as $name => $value){
          if(!is_string($value)) continue;
          $command= preg_replace("/%$name/", $value, $command);
        }
      }

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__,
            $command, "Execute");

        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as ACTION_HOOK for plugin '%s' doesn't seem to exist."), $command, get_class($this));
        print_red ($message);
      }
    }
  }

  /* Directly save new status flag */
  function setStatus($value)
  {
    if($value == "none") return;
    if(!$this->initially_was_account) return;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()){

      $tmp = $ldap->fetch();
      for($i = 0; $i < $tmp['objectClass']['count']; $i ++){
        $attrs['objectClass'][] = $tmp['objectClass'][$i];
      }
      $flag = $this->StatusFlag;
      $attrs[$flag] = $value;
      $this->$flag = $value;
      $ldap->modify($attrs);
      show_ldap_error($ldap->get_error(), sprintf(_("Set status flag for server services/anti virus with dn '%s' failed."),$this->dn));
      $this->action_hook();
    }
  }

  /* Return plugin informations for acl handling 
  function plInfo()
  {
    return (array(
          "plShortName"   => _("Anti virus"),
          "plDescription" => _("Anti virus service"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),
          "plProvidedAcls"=> array(


          "avFlagsD"         =>_("Enable debugging"),
          "avFlagsS"         =>_("Enable mail scanning"),
          "avFlagsA"         =>_("Enable scanning of archives"),
          "avFlagsE"         =>_("Block encrypted archives"),

          "avMaxThreads"                 =>_("Maximum threads"),
          "avMaxDirectoryRecursions"     =>_("Maximum directory recursions"),
          "avUser"                       =>_("Anti virus user"),
          "avArchiveMaxFileSize"         =>_("Maximum file size"),
          "avArchiveMaxRecursion"        =>_("Maximum recursions"),
          "avArchiveMaxCompressionRatio" =>_("Maximum compression ratio"),
          "avDatabaseMirror"             =>_("Database mirror"),
          "avChecksPerDay"               =>_("Checks per day"),
          "avHttpProxyURL"               =>_("Http proxy URL"))
          ));
  }
  */
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
