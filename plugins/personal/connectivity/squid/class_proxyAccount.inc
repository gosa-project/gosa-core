<?php
class proxyAccount extends plugin
{
  /* Definitions */
  var $plHeadline   = "Proxy";
  var $plDescription= "Manage Proxy user settings";

  /* Proxy attributes */
  var $gosaProxyAcctFlags   = "[N    ]";
  var $gosaProxyID          = "";
  var $gosaProxyWorkingStart= 420;
  var $gosaProxyWorkingStop = 1020;
  var $gosaProxyQuota       = "5g";
  var $gosaProxyQuotaPeriod = "m";

  /* attribute list for save action */
  var $attributes   = array("gosaProxyID", "gosaProxyAcctFlags", "gosaProxyWorkingStart",
                            "gosaProxyWorkingStop", "gosaProxyQuota", "gosaProxyQuotaPeriod");

  var $objectclasses    = array("gosaProxyAccount");
  var $ReadOnly         = false;
  var $uid              = "";
  var $view_logged      = FALSE;
  var $multiple_support = TRUE;

  /*! \brief Initialize proxy acoount 
      @param $config GOsa configuration object 
      @param $dn     Object dn
   */
  public function __construct(&$config, $dn= NULL)
  {
    plugin::__construct ($config, $dn);
    
    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }
  }


  /*! \brief Create html output for this class 
   */
  public function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $theme = getThemeName();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    $display= "";
    $smarty= get_smarty();
    
    /* Assign radio boxes */
    foreach (array("F", "T", "B", "N") as $val){
      if (preg_match("/".$val."/",$this->gosaProxyAcctFlags)){
        $smarty->assign("filter$val", "checked");
        $smarty->assign($val."state", "");
      } else {
        $smarty->assign("filter$val", "");
        if(session::get('js')==1){
          $smarty->assign($val."state", "disabled");
        }else{
          $smarty->assign($val."state", "");
        }
      }
    }

    /* Assign ACLs */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $acl => $desc){
      $smarty->assign($acl."ACL",$this->getacl($acl,$this->ReadOnly));
      $smarty->assign($acl."_W",$this->acl_is_writeable($acl,$this->ReadOnly));
    }

    /* Assign working time */
    $smarty->assign("starthour" ,floor($this->gosaProxyWorkingStart / 60));
    $smarty->assign("startminute", ($this->gosaProxyWorkingStart % 60));
    $smarty->assign("stophour", floor($this->gosaProxyWorkingStop / 60));
    $smarty->assign("stopminute", ($this->gosaProxyWorkingStop % 60));
    $hours= array();
    for($i=0; $i<24; $i++){
      $hours[]= sprintf("%02d",$i);
    }
    $smarty->assign("hours", $hours);
    $smarty->assign("minutes", array("00","15","30","45"));

    /* Assign quota values */
    $smarty->assign("quota_unit", array("k" => _("KB"), "m" => _("MB"), "g" => _("GB")));
    $smarty->assign("quota_time", array("h" => _("hour"), "d" => _("day"), "w" => _("week"), "m" => _("month")));
    $smarty->assign("gosaProxyQuotaPeriod", set_post($this->gosaProxyQuotaPeriod));
    $smarty->assign("quota_size", set_post(preg_replace("/[a-z]$/i", "", $this->gosaProxyQuota)));
    $smarty->assign("quota_u", set_post(preg_replace("/^[0-9]+/", "", $this->gosaProxyQuota)));
    if ($this->is_account){
      $smarty->assign("proxyState", "checked");
    } else {
      $smarty->assign("proxyState", "");
    }


    /* Handle input grey out and javascript enabled/disable of input fields
     */
    if($this->multiple_support_active){

      /* In Multiple edit, everything is enabled */
      $changeB = "";
      $smarty->assign("pstate", "");
      $smarty->assign("ProxyWorkingStateChange","");
    }else{

      /* Depeding on the account status, we disable or 
       *  enable all input fields 
       */
      if (!$this->is_account){
        $smarty->assign("pstate", "disabled");
      } else {
        $smarty->assign("pstate", "");
      }

      /* Create JS activation string for everal input fields  */
      $ProxyWorkingStateChange ="\n";
      switch ($theme) {
        case 'classic':
          if($this->acl_is_writeable("gosaProxyFlagT")){
            $ProxyWorkingStateChange.=  "changeState('startHour'); \n";
            $ProxyWorkingStateChange.=  "changeState('startMinute'); \n";
            $ProxyWorkingStateChange.=  "changeState('stopHour'); \n";
            $ProxyWorkingStateChange.=  "changeState('stopMinute'); \n";
          }
          $smarty->assign("ProxyWorkingStateChange",$ProxyWorkingStateChange);
          $changeB = "";
          if($this->acl_is_writeable("gosaProxyFlagB")){
            $changeB = 
              "changeSubselectState('filterB', 'quota_size');
            changeSubselectState('filterB', 'quota_unit');
            changeSubselectState('filterB', 'gosaProxyQuotaPeriod');";
          }
          break;
        
        default:
          if($this->acl_is_writeable("gosaProxyFlagT")){
            $ProxyWorkingStateChange.=  "changeState('startHour', 'startMinute', 'stopHour', 'stopMinute');";
          }
          $smarty->assign("ProxyWorkingStateChange",$ProxyWorkingStateChange);
          $changeB = "";
          if($this->acl_is_writeable("gosaProxyFlagB")){
            $changeB = 
              "changeState('quota_size', 'quota_unit', 'gosaProxyQuotaPeriod');";
          }
          break;
      }
    }

    /* Assign filter settings */
    $smarty->assign("changeB",$changeB);
    foreach(array("T","B","F") as $attr){
      if(in_array_strict("filter".$attr,$this->multi_boxes)){
        $smarty->assign("use_filter".$attr,TRUE);
      }else{
        $smarty->assign("use_filter".$attr,FALSE);
      }
    }

    /* check if we are allowed to switch the checkbox.
     */
    $smarty->assign('proxyAccountACL', preg_replace("/w/","",$this->getacl("",$this->ReadOnly)));
    if(($this->acl_is_removeable() && $this->is_account) ||
        ($this->acl_is_createable() && !$this->is_account)){
      $smarty->assign('proxyAccountACL', $this->getacl("",$this->ReadOnly));
    }

    $smarty->assign("use_proxy",in_array_strict("proxy",$this->multi_boxes));
    $smarty->assign("multiple_support",$this->multiple_support_active);
    $display.= $smarty->fetch(get_template_path('proxy.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }


  /*! \brief Removes proxy account from current object
   */
  public function remove_from_parent()
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

      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
      }

      /* Optionally execute a command after we're done */
      $this->handle_post_events("remove",array("uid" => $this->uid));
    }
  }


  /*! \brief Check given input
      @return array Returns an array of error messages  
   */
  public function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    
    /* We've got only one value to check for positive integer or emtpy field */
    if ($this->is_account && $this->acl_is_writeable("gosaProxyQuota")){
      if (isset($_POST["quota_size"])){
        if ($_POST["quota_size"] == "gosaProxyQuota"){
          $message[]= msgPool::invalid(_("Quota Setting"));
        }elseif ($_POST["quota_size"] <= 0){
          $message[]= msgPool::invalid(_("Quota Setting"),get_post("quota_size"),"/^[0-9]/");
        }
      }
    }
    return $message;
  }

  /*! \brief Save POST data to object 
   */
  public function save_object()
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
    if ($this->is_account || $this->multiple_support_active){

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
          $this->gosaProxyWorkingStart= get_post('startHour') * 60 + get_post('startMinute');
          $this->is_modified= ($old != $this->gosaProxyWorkingStart)?TRUE:$this->is_modified;
        }

        if (isset($_POST['stopMinute'])){
          $old= $this->gosaProxyWorkingStop;
          $this->gosaProxyWorkingStop = get_post('stopHour')  * 60 + get_post('stopMinute');
          $this->is_modified= ($old != $this->gosaProxyWorkingStop)?TRUE:$this->is_modified;
        }
      }

      /* Save quota values */
      if ($this->acl_is_writeable("gosaProxyFlagB")){
        if(isset($_POST["quota_size"]) && isset($_POST["quota_unit"])){
          $this->gosaProxyQuota= get_post("quota_size").get_post("quota_unit");
        }
      }
  
      /*Save quota period */
      if($this->acl_is_writeable("gosaProxyFlagB")){
        if(isset($_POST["gosaProxyQuotaPeriod"])){
          $this->gosaProxyQuotaPeriod = get_post("gosaProxyQuotaPeriod");
        }
      }
    }
  }


  /*! \brief Save settings to ldap 
   */
  public function save()
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
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
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

    
  /*! \brief  Static  Function returning an ACL information array.
      @return Array   Returns an ACL array 
   */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Proxy"),
          "plDescription"   => _("Proxy account")."&nbsp;("._("Connectivity add-on").")",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 21,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),
          "plRequirements"=> array(
              'ldapSchema' => array('gosaProxyAccount' => '>=2.7'),
              'onFailureDisablePlugin' => array(get_class())
              ),

          "plProvidedAcls"  => array(
            "gosaProxyQuota"       => _("Quota"), 
            "gosaProxyFlagF"       => _("Filter unwanted content"), 
            "gosaProxyFlagT"       => _("Limit proxy access"),
            "gosaProxyFlagB"       => _("Restrict proxy usage by quota"))
          ));
  }


  /*! \brief  Save html POSTs in multiple edit.
   */
  public function multiple_save_object()
  {
    if (isset($_POST['connectivityTab'])){
      plugin::multiple_save_object();
      if(isset($_POST['use_proxy'])){
        $this->multi_boxes[] = "proxy";
      }
      foreach(array("T","B","F") as $attr){
        if(isset($_POST["use_filter".$attr])){
          $this->multi_boxes[] = "filter".$attr;
        }
      }
      $this->save_object();
    }
  }


  /*! \brief  Returns all modified values. \
              All selected an modified values will be returned \
              in an array.
      @return array   Returns an array containing all attribute modifications  
   */
  public function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    if(in_array_strict("proxy",$this->multi_boxes)){
      $ret['is_account'] = $this->is_account;
    }

    if(in_array_strict("filterT",$this->multi_boxes)){
      $ret['gosaProxyWorkingStart']  = $this->gosaProxyWorkingStart;
      $ret['gosaProxyWorkingStop']   = $this->gosaProxyWorkingStop;
    }
    if(in_array_strict("filterB",$this->multi_boxes)){
      $ret['gosaProxyQuota']        = $this->gosaProxyQuota;
      $ret['gosaProxyQuotaPeriod']  = $this->gosaProxyQuotaPeriod;
    }

    foreach(array("B","T","F") as $attr){
      $name = "filter".$attr;
      if(in_array_strict($name,$this->multi_boxes)){
        $ret[$name] = preg_match("/".$attr."/",$this->gosaProxyAcctFlags);
      }
    }

    return($ret);
  }


  /*! \brief  Sets modified attributes in mutliple edit. \
              All collected values from "get_multi_edit_values()" \
              will be applied to this plugin.
      @param  array   An array containing modified attributes returned by get_multi_edit_values();
   */
  public function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);
    if(isset($values['is_account'])){
      $this->is_account = $values['is_account'];
    }
    foreach(array("B","T","F") as $attr){
      $name = "filter".$attr;
      if(isset($values[$name])){
        if($values[$name] && !preg_match("/".$attr."/",$this->gosaProxyAcctFlags)){
          $this->gosaProxyAcctFlags = preg_replace("/\]/",$attr."]",$this->gosaProxyAcctFlags);
        }elseif(!$values[$name] && preg_match("/".$attr."/",$this->gosaProxyAcctFlags)){
          $this->gosaProxyAcctFlags = preg_replace("/".$attr."/","",$this->gosaProxyAcctFlags);
        }
      }
    }
  }


  /*! \brief  Initialize multiple edit ui for this plugin. \
              This function sets plugin defaults in multiple edit.
      @param  array   Attributes used in all object 
      @param  array   All used attributes.
   */
  public function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);
    if(isset($attrs['objectClass']) && in_array_strict("gosaProxyAccount",$attrs['objectClass'])){
      $this->is_account = TRUE;
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
