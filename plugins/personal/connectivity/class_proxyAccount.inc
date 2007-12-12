<?php
class proxyAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Proxy";
  var $plDescription= "This does something";

  /* Proxy attributes */
  var $gosaProxyAcctFlags= "[N    ]";
  var $gosaProxyID= "";
  var $gosaProxyWorkingStart= 420;
  var $gosaProxyWorkingStop= 1020;
  var $gosaProxyQuota= "5g";
  var $gosaProxyQuotaPeriod= "m";

  /* attribute list for save action */
  var $attributes= array(   "gosaProxyID", "gosaProxyAcctFlags", "gosaProxyWorkingStart",
                            "gosaProxyWorkingStop", "gosaProxyQuota", "gosaProxyQuotaPeriod");

  var $objectclasses= array("gosaProxyAccount");
  var $ReadOnly = false;

  var $uid = "";
  var $view_logged = FALSE;

  function proxyAccount (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    $display= "";

    /* Prepare templating */
    $smarty= get_smarty();

    $smarty->assign('proxyAccountACL', " disabled ");
    
    if((!$this->ReadOnly)&& (($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable()))){
      $smarty->assign('proxyAccountACL', "");
    }

    /* Assign radio boxes */
    foreach (array("F", "T", "B", "N") as $val){

      if (is_integer(strpos($this->gosaProxyAcctFlags, "$val"))) {
        $smarty->assign("filter$val", "checked");

        /* Add state variables for on-the-fly state-changing of checkboxes */
        $smarty->assign($val."state", "");
        
      } else {
        $smarty->assign("filter$val", "");

        /* Same as above */
        if($_SESSION['js']==1){
          $smarty->assign($val."state", "disabled");
        }else{
          $smarty->assign($val."state", "");
        }
      }
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $acl => $desc){
      $smarty->assign($acl."ACL",$this->getacl($acl,$this->ReadOnly));
      $smarty->assign($acl."_W",$this->acl_is_writeable($acl,$this->ReadOnly));
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

    $changeA = "";
  
    $ProxyWorkingStateChange ="\n";
    if($this->acl_is_writeable("gosaProxyFlagT")){
      $changeA .= "changeState('filterT');\n";

      $ProxyWorkingStateChange.=  "changeState('startHour'); \n";
      $ProxyWorkingStateChange.=  "changeState('startMinute'); \n";
      $ProxyWorkingStateChange.=  "changeState('stopHour'); \n";
      $ProxyWorkingStateChange.=  "changeState('stopMinute'); \n";
    }

    if(preg_match("/F/",$this->gosaProxyAcctFlags)){
      $changeA .= $ProxyWorkingStateChange;
    }
  
    $smarty->assign("ProxyWorkingStateChange",$ProxyWorkingStateChange);

    $changeB = "";
    if($this->acl_is_writeable("gosaProxyFlagB")){
      $changeA .= "changeState('filterB');";
      $changeB = 
        "changeSubselectState('filterB', 'quota_size');
         changeSubselectState('filterB', 'quota_unit');
         changeSubselectState('filterB', 'gosaProxyQuotaPeriod');";
    }

    $smarty->assign("changeB",$changeB);
    $smarty->assign("changeA",$changeA);

    /* Show main page */
    $display.= $smarty->fetch(get_template_path('proxy.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }

  function remove_from_parent()
  {
    if($this->acl_is_removeable() && $this->initially_was_account){   

      plugin::remove_from_parent();

      $ldap= $this->config->get_ldap_link();
      @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
          $this->attributes, "Save");
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 

      /* Log last action */
      new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

      show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/proxy account with dn '%s' failed."),$this->dn));

      /* Optionally execute a command after we're done */
      $this->handle_post_events("remove",array("uid" => $this->uid));
    }
  }

  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    
    /* We've got only one value to check for positive integer or emtpy field */
    if ($this->is_account){
      if($this->acl_is_writeable("gosaProxyQuota")){
        if (isset($_POST["quota_size"])){
          if ($_POST["quota_size"] == "gosaProxyQuota"){
            $message[]= _("Numerical value for Quota Setting is empty.");
          }elseif ($_POST["quota_size"] <= 0){
            $message[]= _("Numerical value for Quota Setting is not valid.");
          }
        }
      }
    }

    return $message;
  }

  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['proxy'])){
        if (!$this->is_account && $_POST['proxy'] == "B"){
          if($this->acl_is_createable()){
            $this->is_account= TRUE;
          }
        }
      } else {
        if($this->acl_is_removeable()){
          $this->is_account= FALSE;
        }
      }
    }

    /* Save flag value */
    if ($this->is_account){

      $flags= "";
      $acl= "";
      foreach(array("F", "T", "B") as $key){
        if($this->acl_is_writeable("gosaProxyFlag".$key)){
  
          /* Add acl */
          if (isset($_POST["filter$key"])){
            $flags.= $key;
          }
        }else{
          
          /* Keep all flags that can't be written*/
          if(preg_match("/".$key."/",$this->gosaProxyAcctFlags)){
            $flags .=$key;
          }
        }
      }
      if ("[$flags]" != $this->gosaProxyAcctFlags){
        $this->is_modified= TRUE;
      }
      $this->gosaProxyAcctFlags= "[$flags]";

      /* Save time values */
      if ($this->acl_is_writeable("gosaProxyFlagT")){
        if(isset($_POST['startMinute'])){
          $old= $this->gosaProxyWorkingStart;
          $this->gosaProxyWorkingStart= $_POST["startHour"] * 60 + $_POST["startMinute"];
          $this->is_modified= ($old != $this->gosaProxyWorkingStart)?TRUE:$this->is_modified;
        }

        if (isset($_POST['stopMinute'])){
          $old= $this->gosaProxyWorkingStop;
          $this->gosaProxyWorkingStop = $_POST["stopHour"]  * 60 + $_POST["stopMinute"];
          $this->is_modified= ($old != $this->gosaProxyWorkingStop)?TRUE:$this->is_modified;
        }
      }

      /* Save quota values */
      if ($this->acl_is_writeable("gosaProxyFlagB")){
        if(isset($_POST["quota_size"]) && isset($_POST["quota_unit"])){
          $this->gosaProxyQuota= $_POST["quota_size"].$_POST["quota_unit"];
        }
      }
  
      /*Save quota period */
      if($this->acl_is_writeable("gosaProxyFlagB")){
        if(isset($_POST["gosaProxyQuotaPeriod"])){
          $this->gosaProxyQuotaPeriod = $_POST["gosaProxyQuotaPeriod"];
        }
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
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    /* Log last action */
    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/proxy account with dn '%s' failed."),$this->dn));

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
          "plShortName"     => _("Proxy"),
          "plDescription"   => _("Proxy account")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 21,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "gosaProxyFlagF"       => _("Filter unwanted content"), 
            "gosaProxyFlagT"       => _("Limit proxy access"),
            "gosaProxyFlagB"       => _("Restrict proxy usage by quota"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
