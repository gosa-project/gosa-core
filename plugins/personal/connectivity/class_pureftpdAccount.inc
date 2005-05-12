<?php

class pureftpdAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "FTP";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users ftp account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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

  /* attribute list for save action */
  var $attributes= array("FTPQuotaFiles","FTPQuotaMBytes","FTPUploadRatio","FTPDownloadRatio",
      "FTPUploadBandwidth","FTPDownloadBandwidth","FTPStatus","FTPuid","FTPgid");
  var $objectclasses= array("PureFTPdUser");

  function pureftpdAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Show tab dialog headers */
    $display= "";

    /* Show main page */
    $smarty= get_smarty();

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
    }
    if ($this->is_account){
      $smarty->assign("pureftpdState", "checked");
    } else {
      $smarty->assign("fstate", "disabled");
    }
    $smarty->assign("use_FTPStatus", ($this->FTPStatus == "disabled") ? "checked" : "");

    if ($this->parent != NULL){
      $smarty->assign("tabbed", 1);
    }

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
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
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
    }

    plugin::save_object();

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
    $message= array();

    /* Check for positive integer values */
    if ($this->is_account){
      if (!is_id($this->FTPUploadBandwidth)){
        $message[]= _("Value specified as 'Upload bandwidth' is not valid.");
      }
      if (!is_id($this->FTPDownloadBandwidth)){
        $message[]= _("Value specified as 'Download bandwidth' is not valid.");
      }
      if (!is_id($this->FTPQuotaFiles)){
        $message[]= _("Value specified as 'Files' is not valid.");
      }
      if (!is_id($this->FTPQuotaMBytes)){
        $message[]= _("Value specified as 'Size' is not valid.");
      }
      if (!is_id($this->FTPUploadRatio) || !is_id($this->FTPDownloadRatio)){
        $message[]= _("Value specified as 'Ratio' is not valid.");
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
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
