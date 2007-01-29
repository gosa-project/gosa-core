<?php

class pureftpdAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "FTP";
  var $plDescription= "This does something";

  /* FTP attributes */
  var $FTPQuotaFiles = 0;
  var $FTPQuotaMBytes = 0;
  var $FTPUploadRatio = 0;
  var $FTPDownloadRatio = 0;
  var $FTPUploadBandwidth = 0;
  var $FTPDownloadBandwidth = 0;
  var $FTPStatus = "enabled";
  var $FTPuid = "";
  var $FTPgid = "";

  var $uid = "";

  /* attribute list for save action */
  var $attributes= array("FTPQuotaFiles","FTPQuotaMBytes","FTPUploadRatio","FTPDownloadRatio",
      "FTPUploadBandwidth","FTPDownloadBandwidth","FTPStatus","FTPuid","FTPgid");
  var $objectclasses= array("PureFTPdUser");

  function pureftpdAccount ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);
    
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

  }

  function execute()
  {
	  /* Call parent execute */
  	plugin::execute();

    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }
    $smarty->assign("fstate", "");
    if ($this->is_account){
      $smarty->assign("pureftpdState", "checked");
      $smarty->assign("fstate", "");
    } else {
      $smarty->assign("pureftpdState", "");
      if($_SESSION['js']==1){
        if($this->acl!="#none#")
        $smarty->assign("fstate", "disabled");
      }else{
        $smarty->assign("fstate", "");
      }
    }
    
    /* Allow account status toogle?  */
    $smarty->assign("pureftpdACL", "disabled");
    if(!$this->is_account && chkacl($this->acl,"create") == ""){
      $smarty->assign("pureftpdACL", "");
    }elseif($this->is_account && $this->acl == "#all#" ){
      $smarty->assign("pureftpdACL", "");
    }

    $smarty->assign("use_FTPStatus", ($this->FTPStatus == "disabled") ? "checked" : "");

    $changeState = "";
    foreach($this->attributes as $attr){
      $smarty->assign($attr."ACL",chkacl($this->acl,$attr));
      if(chkacl($this->acl,$attr)==""){
        $changeState .= "changeState('".$attr."');\n";
      }
    }
    $smarty->assign("changeState",$changeState);
    $display.= $smarty->fetch (get_template_path('pureftpd.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    plugin::remove_from_parent();
    $ldap= $this->config->get_ldap_link();

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing pureftpd account failed"));

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove',array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){

      /* Change state if needed */
      if(!$this->is_account && isset($_POST["pureftpd"]) && chkacl($this->acl,"create") == ""){
        $this->is_account = true;
      }elseif($this->is_account && !isset($_POST["pureftpd"]) && $this->acl == "#all#"){
        $this->is_account = false;
      }
    }

    plugin::save_object();

    #FIXME seams to be unused code !
    $old= $this->FTPStatus;
    if (isset($_POST["FTPStatus"])){
      $this->FTPStatus = "disabled";
    } else {
      $this->FTPStatus = "enabled";
    }
    $this->is_modified= ($old != $this->FTPStatus)?TRUE:$this->is_modified;
  }


  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Check for positive integer values */
    if ($this->is_account){
      if ((!is_id($this->FTPUploadBandwidth))&&(chkacl($this->acl,"FTPUploadBandwidth")=="")){
        $message[]= _("Value specified as 'Upload bandwidth' is not valid.");
      }
      if ((!is_id($this->FTPDownloadBandwidth))&&(chkacl($this->acl,"FTPDownloadBandwidth")=="")){
        $message[]= _("Value specified as 'Download bandwidth' is not valid.");
      }
      if ((!is_id($this->FTPQuotaFiles))&&(chkacl($this->acl,"FTPQuotaFiles")=="")){
        $message[]= _("Value specified as 'Files' is not valid.");
      }
      if ((!is_id($this->FTPQuotaMBytes))&&(chkacl($this->acl,"FTPQuotaMBytes")=="")){
        $message[]= _("Value specified as 'Size' is not valid.");
      }
      if ((!is_id($this->FTPUploadRatio) || !is_id($this->FTPDownloadRatio))&&(chkacl($this->acl,"FTPUploadRatio")=="")&&(chkacl($this->acl,"FTPDownloadRatio")=="")){
        $message[]= _("Value specified as 'Ratio' is not valid.");
      }
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    foreach($this->attributes as $attr){
      if(chkacl($this->acl,$attr)!=""){
        unset($this->attrs[$attr]);
      }
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving pureftpd account failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
