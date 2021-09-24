<?php

class pureftpdAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "FTP";
  var $plDescription= "Manage FTP user settings";

  /* FTP attributes */
  var $FTPQuotaFiles = "";
  var $FTPQuotaMBytes = "";
  var $FTPUploadRatio = "";
  var $FTPDownloadRatio = "";
  var $FTPUploadBandwidth = "";
  var $FTPDownloadBandwidth = "";
  var $FTPStatus = "enabled";
  var $FTPuid = "";
  var $FTPgid = "";

  var $uid = "";

  /* attribute list for save action */
  var $attributes= array("FTPQuotaFiles","FTPQuotaMBytes","FTPUploadRatio","FTPDownloadRatio",
      "FTPUploadBandwidth","FTPDownloadBandwidth","FTPStatus","FTPuid","FTPgid");
  var $objectclasses= array("PureFTPdUser");
  var $ReadOnly;
  var $view_logged = FALSE;

  var $multiple_support=TRUE;

  function __construct(&$config, $dn= NULL, $parent= NULL)
  {
    plugin::__construct ($config, $dn, $parent);
    
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }

  function execute()
  {
    plugin::execute();
    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", set_post($this->$val));
    }


    $tmp = $this->plInfo();
    $changeState = "";
    if($this->multiple_support_active){

      /* We do not need the attribute grey out in multiple edit */
      foreach($tmp['plProvidedAcls'] as $key => $desc){
        $smarty->assign($key."ACL", $this->getacl($key,$this->ReadOnly));
      }
      if ($this->is_account){
        $smarty->assign("pureftpdState", "checked");
      }else{
        $smarty->assign("pureftpdState", "");
      }
      $smarty->assign("fstate", "");
      $smarty->assign("changeState","");

    }else{
      foreach($tmp['plProvidedAcls'] as $key => $desc){
        $smarty->assign($key."ACL", $this->getacl($key,$this->ReadOnly));
        $smarty->assign($key."_W", $this->acl_is_writeable($key,$this->ReadOnly));

        if($this->acl_is_writeable($key)){
          $changeState.= " changeState('".$key."'); \n";
        }
      }
      $smarty->assign("changeState",$changeState);


      $smarty->assign("fstate", "");
      if ($this->is_account){
        $smarty->assign("pureftpdState", "checked");
        $smarty->assign("fstate", "");
      } else {
        $smarty->assign("pureftpdState", "");
        if(session::get('js')==1){
          if($this->acl!="#none#")
            $smarty->assign("fstate", "disabled");
        }else{
          $smarty->assign("fstate", "");
        }
      }
    }

    foreach($this->attributes as $attr){
      if(in_array_strict($attr,$this->multi_boxes)){
        $smarty->assign("use_".$attr,TRUE);
      }else{
        $smarty->assign("use_".$attr,FALSE);
      }
    }
    $smarty->assign("use_pureftpd",in_array_strict("pureftpd",$this->multi_boxes)); 
    $smarty->assign("multiple_support",$this->multiple_support_active);
    $smarty->assign("FTPStatus", ($this->FTPStatus == "disabled") ? "checked" : "");
    $smarty->assign('pureftpdACL', $this->getacl("",$this->ReadOnly));
    $display.= $smarty->fetch (get_template_path('pureftpd.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if ((!$this->initially_was_account) || (!$this->acl_is_removeable())){
      return;
    }

    plugin::remove_from_parent();
    $ldap= $this->config->get_ldap_link();

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    /* Log last action */
    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
  
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove',array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['pureftpd'])){
        if (!$this->is_account && $_POST['pureftpd'] == "B"){
          $this->is_account= TRUE;
        }
      } else {
        $this->is_account= FALSE;
      }

      plugin::save_object();

      $old= $this->FTPStatus;
      if (isset($_POST["FTPStatus"])){
        $this->FTPStatus = "disabled";
      } else {
        $this->FTPStatus = "enabled";
      }
      $this->is_modified= ($old != $this->FTPStatus)?TRUE:$this->is_modified;

      /* Ensure that these vars are numeric. Values starting with 0 like '0123' cause ldap errors */
      foreach(array("FTPQuotaFiles","FTPQuotaMBytes","FTPUploadRatio","FTPDownloadRatio",
                    "FTPUploadBandwidth","FTPDownloadBandwidth") as $testVar){
        if(!empty($this->$testVar)){
          $this->$testVar = (int) ($this->$testVar);
        }
      }
    }
  }


  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Check for positive integer values */
    if ($this->is_account){

      if($this->acl_is_writeable("FTPUploadBandwidth") && preg_match("/[^0-9]/",$this->FTPUploadBandwidth)){
        $message[]= msgPool::invalid(_("Upload bandwidth"), $this->uid, "/^[0-9]+$/");
      }
      if($this->acl_is_writeable("FTPDownloadBandwidth") && preg_match("/[^0-9]/",$this->FTPDownloadBandwidth)){
        $message[]= msgPool::invalid(_("Download bandwidth"), $this->uid, "/^[0-9]+$/");
      }

      if($this->acl_is_writeable("FTPQuotaFiles") && preg_match("/[^0-9]/",$this->FTPQuotaFiles)){
        $message[]= msgPool::invalid(_("Quota file"), $this->uid, "/^[0-9]+$/");
      }
      if($this->acl_is_writeable("FTPQuotaMBytes") && preg_match("/[^0-9]/",$this->FTPQuotaMBytes)){
        $message[]= msgPool::invalid(_("Quota size"), $this->uid, "/^[0-9]+$/");
      }
      if($this->acl_is_writeable("FTPUploadRatio") && preg_match("/[^0-9]/",$this->FTPUploadRatio)){
        $message[]= msgPool::invalid(_("Upload ratio"), $this->uid, "/^[0-9]+$/");
      }
      if($this->acl_is_writeable("FTPDownloadRatio") && preg_match("/[^0-9]/",$this->FTPDownloadRatio)){
        $message[]= msgPool::invalid(_("Download ratio"), $this->uid, "/^[0-9]+$/");
      }
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();


    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 
    
    /* Log last action */
    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }

  }


  /* Return plugin informations for acl handling
  #FIME There possibly some attributes that can be combined to one acl. */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("FTP"),
          "plDescription"   => _("Pure-FTPd account")."&nbsp;("._("Connectivity add-on").")",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 22,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plRequirements"=> array(
              'ldapSchema' => array('PureFTPdUser' => ''),
              'onFailureDisablePlugin' => array(get_class())
              ),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "FTPQuotaFiles"         => _("Quota files"),
            "FTPUploadRatio"        => _("Upload ratio"),
            "FTPQuotaMBytes"        => _("Quota megabytes"),
            "FTPDownloadRatio"      => _("Download ratio"),
            "FTPUploadBandwidth"    => _("Upload bandwidth"),
            "FTPDownloadBandwidth"  => _("Download bandwidth"),
            "FTPStatus"             => _("Status"))
          ));
  }

  function multiple_save_object()
  {
    if (isset($_POST['connectivityTab'])){
      plugin::multiple_save_object();
      if(isset($_POST['use_pureftpd'])){
        $this->multi_boxes[] = "pureftpd";
      }
      $this->save_object();
    }
  }

  function get_multi_init_values()
  {
    $ret = plugin::get_multi_init_values();
    $ret['is_account'] = $this->is_account;
    return($ret);
  }

  function init_multiple_support($attrs,$attr)
  {
    plugin::init_multiple_support($attrs,$attr);

    if(isset($attrs['is_account'])){
      $this->is_account = $attrs['is_account'];
    }
  }

  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    if(in_array_strict("pureftpd",$this->multi_boxes)){
      $ret['is_account'] = $this->is_account;
    }
    return($ret);
  }

  function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);
    if(isset($values['is_account'])){
      $this->is_account = $values['is_account'];
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
