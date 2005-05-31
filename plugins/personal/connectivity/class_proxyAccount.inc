<?php
class proxyAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Proxy";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Manage users proxy account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Proxy attributes */
  var $gosaProxyAcctFlags= "[N    ]";
  var $gosaProxyID= "";
  var $gosaProxyWorkingStart= 420;
  var $gosaProxyWorkingStop= 1020;
  var $gosaProxyQuota= "5g";
  var $gosaProxyQuotaPeriod= "m";

  /* attribute list for save action */
  var $attributes= array("gosaProxyID", "gosaProxyAcctFlags", "gosaProxyWorkingStart",
      "gosaProxyWorkingStop", "gosaProxyQuota", "gosaProxyQuotaPeriod");
  var $objectclasses= array("gosaProxyAccount");

  function proxyAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    $display= "";

    /* Prepare templating */
    $smarty= get_smarty();
    $smarty->assign("gosaProxyAcctFlagsACL", chkacl($this->acl, "gosaProxyAcctFlags"));
    $smarty->assign("gosaProxyWorkingStartACL", chkacl($this->acl, "gosaProxyWorkingStart"));
    $smarty->assign("gosaProxyWorkingStopACL", chkacl($this->acl, "gosaProxyWorkingStop"));
    $smarty->assign("gosaProxyQuotaACL", chkacl($this->acl, "gosaProxyQuota"));

    /* Show checkbox? */
    if ($this->parent != NULL){
      $smarty->assign("tabbed", "1");
    }

    /* Assign radio boxes */
    foreach (array("F", "T", "B", "N") as $val){
      if (is_integer(strpos($this->gosaProxyAcctFlags, "$val"))) {
        $smarty->assign("filter$val", "checked");
      } else {
        $smarty->assign("filter$val", "");
      }
    }

    /* Assign working time */
    $smarty->assign("starthour", (int)($this->gosaProxyWorkingStart / 60));
    $smarty->assign("startminute", (int)($this->gosaProxyWorkingStart % 60));
    $smarty->assign("stophour", (int)($this->gosaProxyWorkingStop / 60));
    $smarty->assign("stopminute", (int)($this->gosaProxyWorkingStop % 60));
    $hours= array();
    for($i=0; $i<24; $i++){
      $hours[]= sprintf("%02d",$i);
    }
    $smarty->assign("hours", $hours);
    $smarty->assign("minutes", array("00","15","30","45"));

    /* Assign quota values */
    $smarty->assign("quota_unit", array("k" => _("KB"), "m" => _("MB"), "g" => _("GB")));
    $smarty->assign("quota_time", array("h" => _("hour"), "d" => _("day"), "w" => _("week"), "m" => _("month")));
    $smarty->assign("gosaProxyQuotaPeriod", $this->gosaProxyQuotaPeriod);
    $smarty->assign("quota_size", preg_replace("/[a-z]$/i", "", $this->gosaProxyQuota));
    $smarty->assign("quota_u", preg_replace("/^[0-9]+/", "", $this->gosaProxyQuota));
    if ($this->is_account){
      $smarty->assign("proxyState", "checked");
    } else {
      $smarty->assign("proxyState", "");
    }

    /* Prepare correct state */
    if (!$this->is_account){
      $smarty->assign("pstate", "disabled");
    } else {
      $smarty->assign("pstate", "");
    }

    /* Show main page */
    $display.= $smarty->fetch(get_template_path('proxy.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }
    
    plugin::remove_from_parent();

    $ldap= $this->config->get_ldap_link();
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }

  /* Check values */
  function check()
  {
    $message= array();
    
    /* We've got only one value to check for positive integer or emtpy field */
    if ($this->is_account){
      if (isset($_POST["quota_size"])){
        if ($_POST["quota_size"] == ""){
          $message[]= _("Numerical value for Quota Setting is empty.");
        }
        else if ($_POST["quota_size"] <= 0){
          $message[]= _("Numerical value for Quota Setting is not valid.");
        }
      }
    }

    return $message;
  }

  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if ($this->parent != NULL){
      if (isset($_POST['connectivityTab'])){
        if (isset($_POST['proxy'])){
          if (!$this->is_account && $_POST['proxy'] == "B"){
            $this->is_account= TRUE;
          }
        } else {
          $this->is_account= FALSE;
        }
      }
    }

    /* Save flag value */
    if ($this->is_account){
      if (chkacl ($this->acl, "gosaProxyAcctFlags") == ""){
        $flags= "";
        foreach(array("F", "T", "B") as $key){
          if (isset($_POST["filter$key"])){
            $flags.= $key;
          }
        }
        if ("[$flags]" != $this->gosaProxyAcctFlags){
          $this->is_modified= TRUE;
        }
        $this->gosaProxyAcctFlags= "[$flags]";
      }

      /* Save time values */
      if (chkacl ($this->acl, "gosaProxyWorkingTime") == "" && isset($_POST['startMinute'])){
        $old= $this->gosaProxyWorkingStart;
        $this->gosaProxyWorkingStart= $_POST["startHour"] * 60 + $_POST["startMinute"];
        $this->is_modified= ($old != $this->gosaProxyWorkingStart)?TRUE:$this->is_modified;
        $old= $this->gosaProxyWorkingStop;
        $this->gosaProxyWorkingStop = $_POST["stopHour"]  * 60 + $_POST["stopMinute"];
        $this->is_modified= ($old != $this->gosaProxyWorkingStop)?TRUE:$this->is_modified;
      }

      /* Save quota values */
      if (chkacl ($this->acl, "gosaProxyQuota") == ""){
        $old= $this->gosaProxyQuota;
        if(isset($_POST["quota_size"]) && isset($_POST["quota_unit"])){
          $this->gosaProxyQuota= $_POST["quota_size"].$_POST["quota_unit"];
        }
        $this->is_modified= ($old != $this->gosaProxyQuota)?TRUE:$this->is_modified;
        $old= $this->gosaProxyQuotaPeriod;
        if(isset($_POST["gosaProxyQuotaPeriod"])){
          $this->gosaProxyQuotaPeriod = $_POST["gosaProxyQuotaPeriod"];
        }
        $this->is_modified= ($old != $this->gosaProxyQuotaPeriod)?TRUE:$this->is_modified;
      }
    }

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
