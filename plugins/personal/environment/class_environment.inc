<?php

class environment extends plugin
{
  var $plIcon = "plugins/goto/images/plugin.png";

  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $plHeadline           = "Desktop";
  var $plDescription        = "Manage desktop settings";
  var $dialog               = false;    // Indicates that we are currently editing in an seperate dialog
  
  var $in_dialog            = false;
  var $uid                  = "";

  var $is_group             = false;
  var $view_logged = FALSE;

  /* Attribute definition
   */

  /* profile management */
  var $useProfile         = false;  // Specifies if we want to use a Server 
  var $gotoProfileServer  = "";     // Specifies the selected profile server
  var $gotoProfileServers = array();// Specifies all available and selectable servers
  var $gotoProfileFlags   = "";     // Flags enabled  ? only used to set ACL and save 
  var $gotoProfileFlagC  = "";     // Flag is set to C if we have the profile caching fucntion enabled 
  
  var $gotoXResolution    = "auto";     // The selected resolution eg: 1024x768
  var $gotoXResolutions   = array();// Contains all available resolutions for this account
  var $gotoProfileFlagL  = "";     // Flag is set to L to enable runtime resolution change 
  var $gotoProfileQuota   = "";     // User Quota Settings

  /* Logon script section*/
  var $gotoLogonScripts   = array();// Contains all available Logon Scripts  
  var $gotoLogonScript    = "";     // The selected Logon Script

  /* Printer */
  var $gotoPrinter        = array();// All available Printer, with their configurations
  var $gotoPrinterSel     = "";     //  The selected Printer
  var $gosaDefaultPrinter = "";     // Default printer

  /* Share */
  var $gotoShares         = array();// Current Share Options
  var $gotoShare          = "";     // currently selected Share Option
  var $gotoShareSelections= array();// Available Shares for this account in Listbox format
  var $gotoAvailableShares= array();// Available Shares for this account

  /* Kiosk profile */
  var $kiosk_enabled      = FALSE;
  var $gotoKioskProfile   = "";     // The selected Kiosk Profile
  var $gotoKioskProfile_Server    = "";     // The selected Kiosk Profile
  var $gotoKioskProfile_Profile   = "";     // The selected Kiosk Profile
  var $gotoKioskProfiles  = array();// All available Kiosk profiles

  /* Hotplug Devices */
  var $gotoHotplugDevice  = array();     // Selected hotplug
  var $gotoHotplugDevices = array();// Already configured hotplug devices 
  var $gotoHotplugDeviceDN= array();

  var $NewAddedPrinters   = array();
  var $NewDeletedPrinters = array();

  /* general settings */
  // Sets the attributes which will kept on page reload, which will be saved, ...

  var $CopyPasteVars      = array("gotoHotplugDevices","gotoAvailableShares","gotoShareSelections","gotoPrinterSel","gotoProfileFlagL","gotoXResolutions","gotoProfileFlagC","gotoProfileServers","useProfile","is_group","in_dialog","OrigCn","gotoKioskProfile_Server","gotoKioskProfile_Profile", "gotoLogonScripts","gotoLogonScript","gotoShare","gotoShares");

  var $attributes         = array("uid","gotoProfileServer","gotoProfileFlags","gotoHotplugDeviceDN",
      "gotoXResolution","gotoProfileQuota",
      "gotoLogonScript","gotoLogonScripts","gotoHotplugDevices",
      "gotoPrinter", "gosaDefaultPrinter",
      "gotoShare",
      "gotoKioskProfile");
  var $objectclasses      = array("gotoEnvironment"); // Specifies the objectClass which contains the attributes edited here 
  var $cn;
  var $OrigCn;
  var $add_del_printer_member_was_called = false;

  var $multiple_support =TRUE;

  var $use_gotoPrinter;
  var $shareList = NULL;


  function __construct(&$config, $dn= NULL)
  {
    plugin::__construct ($config, $dn);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Check : Are we currently editing a group or user dialog */
    if((isset($this->attrs['cn'][0]))&&(!isset($this->attrs['uid'][0]))){
      $suffix="Group";
      $this->uid          = $this->attrs['cn'][0];
      $this->attrs['uid'] = $this->attrs['cn'][0];
      $this->OrigCn = $this->attrs['cn'][0];
    }else{
      $suffix="User";
    }

    /* Get all Printer assignments */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoPrinter)(goto".$suffix."Printer=".$this->uid."))",array("*"));
    while($printer = $ldap->fetch()){
      $this->gotoPrinter[$printer['cn'][0]]=$printer;
      $this->gotoPrinter[$printer['cn'][0]]['mode']="user";
    }
    $ldap->search("(&(objectClass=gotoPrinter)(goto".$suffix."AdminPrinter=".$this->uid."))",array("*"));
    while($printer = $ldap->fetch()){
      $this->gotoPrinter[$printer['cn'][0]]=$printer;
      $this->gotoPrinter[$printer['cn'][0]]['mode']="admin";
    }


    /* Prepare hotplugs */
    if(isset($this->attrs['gotoHotplugDeviceDN']) && is_array($this->attrs['gotoHotplugDeviceDN'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      for($i = 0 ; $i < $this->attrs['gotoHotplugDeviceDN']['count'] ; $i ++){
        $ldap->cat($this->attrs['gotoHotplugDeviceDN'][$i]);
        if($ldap->count()){
          $attrs = $ldap->fetch(); 

          if(isset($attrs['gotoHotplugDevice'][0])){
            $tmp      = preg_split("/\|/",$attrs['gotoHotplugDevice'][0]);
            $tmp2     = array();
            $tmp2['name']         = $attrs['cn'][0];
            $tmp2['description']  = $tmp[0];
            $tmp2['id']           = $tmp[1];
            $tmp2['produkt']      = $tmp[2];
            $tmp2['vendor']       = $tmp[3];
            $tmp2['dn']           = $attrs['dn'];
            $this->gotoHotplugDevices[] = $tmp2; 
          }
        }else{
          msg_dialog::display(_("Warning"), sprintf(_("Device '%s' is not available anymore. It will be removed!"), $this->attrs['gotoHotplugDeviceDN'][$i]), WARNING_DIALOG);
        }
      }
    }

 
    /* prepare LogonScripts */
    if((isset($this->attrs['gotoLogonScript']))&&(is_array($this->attrs['gotoLogonScript']))){
      unset($this->attrs['gotoLogonScript']['count']);
      foreach($this->attrs['gotoLogonScript'] as $device){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$device);
        $tmp2['LogonName']        = $tmp[0]; 
        $tmp2['LogonPriority']    = $tmp[2]; 
        if(preg_match("/O/i",$tmp[1])){
          $tmp2['LogonOverload'] = "O";
        }else{
          $tmp2['LogonOverload'] = "";
        }
        if(preg_match("/L/i",$tmp[1])){
          $tmp2['LogonLast'] = "L";
        }else{
          $tmp2['LogonLast'] = "";
        }
        $tmp2['LogonData']        = base64_decode($tmp[3]); 
        $tmp2['LogonDescription'] = $tmp[4];
        $this->gotoLogonScripts[$tmp[0]]=$tmp2;
      }
    }

    /* Prepare Shares */
    if((isset($this->attrs['gotoShare']))&&(is_array($this->attrs['gotoShare']))){
      unset($this->attrs['gotoShare']['count']);
      foreach($this->attrs['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$share);
        $tmp2['server']      =$tmp[0];
        $tmp2['name']        =$tmp[1];

        /* Decode base64 if needed */
        if (!preg_match('%/%', $tmp[2])){
          $tmp2['mountPoint']  =base64_decode($tmp[2]);
        } else {
          $tmp2['mountPoint']  =$tmp[2];
        }

        if(isset($tmp[3])){
          $tmp2['PwdHash']  =$tmp[3];
        }else{
          $tmp2['PwdHash']  ="";
        }
        if(isset($tmp[4])){
          $tmp2['Username']  =$tmp[4];
        }else{
          $tmp2['Username']  ="";
        }
        $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
      }
    }

    for($i = 0 ; $i < strlen($this->gotoProfileFlags) ; $i ++){
      $chr = $this->gotoProfileFlags[$i];
      $name = "gotoProfileFlag".$chr;
      $this->$name=$chr;
    }

    if((!empty($this->gotoProfileServer))||($this->gotoProfileFlagC=="C")){
      $this->useProfile = true;
    }else{
      $this->useProfile = false;
    }

    /* Set resolutions */
    $this->gotoXResolutions = array("auto"=>_("auto"),
                                    "640x480"   =>  "640x480",
                                    "800x600"   =>  "800x600",
                                    "1024x768"  =>  "1024x768",
                                    "1152x864"  =>  "1152x864",
                                    "1280x768"  =>  "1280x768",
                                    "1280x1024" =>  "1280x1024");

    if($this->config->get_cfg_value("environment","resolutions") != ""){
      $file = $this->config->get_cfg_value("environment","resolutions");

      if(is_readable($file)){
        $str = file_get_contents($file);
        $lines = preg_split("/\n/",$str);
        foreach($lines as $line){
          $line = trim($line);
          if(!empty($line)){
            $this->gotoXResolutions[$line]=$line;
          }
        }
        //natcasesort($this->gotoXResolutions);
      }else{
        msg_dialog::display(_("Configuration error"), sprintf(_("Cannot open file '%s'!"), $file), WARNING_DIALOG);
      }
    }

    $this->gotoProfileServers= $config->getShareServerList() ;
    $this->gotoShareSelections= $config->getShareList(true);
    $this->gotoAvailableShares= $config->getShareList(false);  
  
    /* Ensure that a currently selected server will stay selected, even if the list of profile servers
        is empty due to insufficient permissions.
     */
    if(count($this->gotoProfileServers) && !empty($this->gotoProfileServer)){

      /* The currently selected profile server is outdated/no longer available */
      if(!isset($this->gotoProfileServers[$this->gotoProfileServer])){

      }else{
        $c = $this->gotoProfileServers[$this->gotoProfileServer];
        if(!preg_match("/r/",$c['ACL'])){

          /* We are not allowed to read to currently selected server.
             But to ensure that the selection will be kept after saving the account, we 
              override the current ACL tag.
           */          
          $this->gotoProfileServers[$this->gotoProfileServer]['ACL'] .= "r";
        }
      }
    }
    $this->update_kiosk_profiles();

    // Prepare lists
    $this->shareList = new sortableListing();
    $this->shareList->setDeleteable(false);
    $this->shareList->setEditable(false);
    $this->shareList->setWidth("100%");
    $this->shareList->setHeight("120px");
    $this->shareList->setHeader(array(_("Server"),_("Share name"),_("Mount point"), 
            _("User"), _("Type"), _("Action")));
    $this->shareList->setDefaultSortColumn(1);
  }


  function update_kiosk_profiles()
  { 
    $this->gotoKioskProfile_Server = preg_replace("/^.*:\/\/([^\/]*).*$/","\\1",$this->gotoKioskProfile);
    $this->gotoKioskProfile_Profile= preg_replace("/^.*\//","",$this->gotoKioskProfile);

    $tmp1  = array("none" => array(_("disabled")));
    $tmp2  = array("none" => _("disabled"));
    $tmp3  = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=goEnvironmentServer)(gotoKioskProfile=*)(cn=*))",array("cn","gotoKioskProfile"));
    $cnt = 0;
    $ui = get_userinfo();
    while($attrs = $ldap->fetch()){
      $acl = $ui->get_permissions($attrs['dn'],"server/goKioskService","");
      for($i = 0 ; $i < $attrs['gotoKioskProfile']['count'] ; $i ++){
        if(preg_match("/r/",$acl) || $this->gotoKioskProfile_Server == $attrs['cn'][0]){
          $name = preg_replace("/^.*\//","",$attrs['gotoKioskProfile'][$i]);
          $tmp1[$attrs['cn'][0]][] = $name;
          $tmp3[$attrs['cn'][0]][$name] = $attrs['gotoKioskProfile'][$i];
          $tmp2[$attrs['cn'][0]]= $attrs['cn'][0];
          $cnt ++;
        }
      }
    }

    $tmp = $this->config->get_cfg_value("environment","kioskPath");
    if($cnt && !empty($tmp)){
      $this->kiosk_enabled = TRUE;
    }

    $this->gotoKioskProfiles['BY_SERVER'] = $tmp1;
    $this->gotoKioskProfiles['SERVERS']   = $tmp2;
    $this->gotoKioskProfiles['MAP']       = $tmp3;
  
    $error = false;
    if(!isset($this->gotoKioskProfiles['SERVERS'][$this->gotoKioskProfile_Server])){
      $error = true;
    }elseif(!in_array_strict($this->gotoKioskProfile_Profile, $this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
      $error = true;
    }
    if($error && !empty($this->gotoKioskProfile)){
      msg_dialog::display(_("Warning"), sprintf(_("Kiosk profile '%s' located on server '%s' is not available anymore. Kiosk profile will be disabled!"), $this->gotoKioskProfile_Profile, $this->gotoKioskProfile_Server), WARNING_DIALOG);
      $this->gotoKioskProfile_Server ="none";
      $this->gotoKioskProfile_Profile="";
    }elseif(empty($this->gotoKioskProfile)){
      $this->gotoKioskProfile_Server ="none";
      $this->gotoKioskProfile_Profile="";
    }
  }


  /* Detect type of edited object (user|group)*/
  function detect_grouptype()
  {
    if((!isset($this->parent))&&(!$this->is_account)){
      $this->is_group     = false;
    }elseif((isset($this->parent))&&(!isset($this->parent->by_object['posixAccount']))){
      $this->is_group     = true;
    }else{
      $this->is_group     = false;
    }
  }


  function execute()
  {

    /* Call parent execute */
    plugin::execute();
    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      if(isset($this->parent->by_object['user']) || (isset($this->attrs['objectClass']) &&in_array_strict("gosaAccount",$this->attrs['objectClass']))){
        new log("view","users/".get_class($this),$this->dn);
      }else{
        new log("view","groups/".get_class($this),$this->dn);
      }
    }

    /* Check profile server */
    if($this->acl_is_writeable("gotoProfileServer")){

      if(!empty($this->gotoProfileServer) && !isset($this->gotoProfileServers[$this->gotoProfileServer])){


        if(count($this->gotoProfileServers)){

          /* Get First Profile */
          $new = key($this->gotoProfileServers);

          /* Another profile server found */
          msg_dialog::display(_("Warning"), sprintf(_("Profile server '%s' is not available anymore. Switched to server '%s'."), $this->gotoProfileServer, $new), WARNING_DIALOG);
        }else{

          /* No other profile servers found */
          msg_dialog::display(_("Warning"), sprintf(_("Profile server '%s' is not available anymore. Kiosk profile will be disabled."), $this->gotoProfileServer), WARNING_DIALOG);
          $this->gotoProfileServer = "none";
        }
      }
    }    

    $this->detect_grouptype();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("kiosk_enabled",$this->kiosk_enabled);
    $display= "";

    $smarty->assign("is_group",$this->is_group);

    /* Prepare all variables for smarty */
    foreach($this->attributes as $s_attr){
      /* Set value*/
      $smarty->assign($s_attr,set_post($this->$s_attr));

      /* Set checkbox state*/
      if(empty($this->$s_attr)){
        $smarty->assign($s_attr."CHK","");
      }else{
        $smarty->assign($s_attr."CHK"," checked ");
      }
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $val => $desc){
      $smarty->assign("$val"."ACL", $this->getacl($val));
    }

    /* Is accout enabled | are we editing from usermenu or admin menu 
       All these tab management is done here
     */


    /* Working from Usermenu an the Account is currently disbled
     * this->parent :  is only set if we are working in a list of tabs
     * is_account   :  is only true if the needed objectClass is given
     */
    if((!isset($this->parent))&&(!$this->is_account)){
      /* We are currently editing this tab from usermenu, but this account is not enabled */
      $smarty->assign("is_account",$this->is_account);
      /* Load template */
      $display .= $smarty->fetch(get_template_path('environment.tpl', TRUE));
      /* Avoid the "You are currently editing ...." message when you leave this tab */
      $display .= back_to_main(); 
      /* Display our message to the user */
      return $display;


      /* We are currently editing from group tabs, because 
       * $this->parent is set
       * posixAccount is not set, so we are not in usertabs.
       */
    }elseif((isset($this->parent))&&(!isset($this->parent->by_object['posixAccount']))){
      $smarty->assign("is_account","true");
      $this->uid          = $this->cn;
      $this->attrs['uid'] = $this->cn;

      /* Change state if needed */
      if (isset($_POST['modify_state'])){
        if(($this->acl_is_createable() && !$this->is_account) || 
            ($this->acl_is_removeable() &&  $this->is_account)){
          $this->is_account= !$this->is_account;
        }
      }
      /* Group Dialog with enabled environment options */
      if ($this->is_account){
        $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Desktop")),
            msgPool::featuresEnabled(_("Desktop")));
      } else {

        /* Environment is disabled 
           If theres is no posixAccount enabled, you won't be able to enable 
           environment extensions
         */
        if((isset($this->parent->by_object['group']))||(isset($this->attrs['objectClass']))&&((in_array_strict("posixAccount",$this->attrs['objectClass'])))){
          $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Desktop")),
              msgPool::featuresDisabled(_("Desktop")));
          return $display;
        }elseif((isset($this->parent->by_object['ogroup']))){
          $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Desktop")),
              msgPool::featuresDisabled(_("Desktop")));
          return $display;
        }else{
          $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Desktop")),
              msgPool::featuresDisabled(_("Desktop"), _("POSIX")), TRUE);
          return $display;
        }
      }
    }else{
      /* Editing from Usermenu 
       *  Tell smarty that this accoutn is enabled 
       */
      $smarty->assign("is_account","true");

      /* Change state if needed */
      if (isset($_POST['modify_state'])){
        if(($this->acl_is_createable() && !$this->is_account) || 
            ($this->acl_is_removeable() &&  $this->is_account)){
          $this->is_account= !$this->is_account;
        }
      }

      if(isset($this->parent)){

        // 3. Account enabled . Editing from adminmenu
        if ($this->is_account){
          $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Desktop")),
              msgPool::featuresEnabled(_("Desktop")));
        } else {

          if($this->parent->by_object['posixAccount']->is_account==true){
            $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Desktop")),
                msgPool::featuresDisabled(_("Desktop")));
            return $display;
          }else{
            $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Desktop")),
                msgPool::featuresDisabled(_("Desktop"), _("POSIX")), TRUE);
            return $display;
          }
        }
      }
    }
  
    /* Reset header toggle */
    if($this->multiple_support_active){
      $display = "";
    }

    /* Account is Account : is_accounbt=true.
     * Else we won't reach this. 
     */

    /* Prepare all variables for smarty */
    foreach($this->attributes as $s_attr){
      /* Set value*/
      $smarty->assign($s_attr,set_post($this->$s_attr));

      /* Set checkbox state*/
      if(empty($this->$s_attr)){
        $smarty->assign($s_attr."CHK","");
      }else{
        $smarty->assign($s_attr."CHK"," checked ");
      }
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $val => $desc){
      $smarty->assign("$val"."ACL", $this->getacl($val));
    }

    foreach(array("gotoProfileFlagC","gotoProfileFlagL") as $s_attr){
      $smarty->assign($s_attr."ACL",$this->getacl($s_attr));
    }

    if($this->read_only) {
      $smarty->assign("gotoPrinterACL","r");
    }else{
      $smarty->assign("gotoPrinterACL","rw");
    }


    $smarty->assign("useProfile",$this->useProfile);
    if(empty($this->useProfile) && !$this->multiple_support_active){
      $smarty->assign("useProfileCHK","");
      $smarty->assign("gotoProfileServerACL" , preg_replace("/w/","",$this->getacl("gotoProfileServer")));
      $smarty->assign("gotoProfileQuotaACL" , preg_replace("/w/","",$this->getacl("gotoProfileQuota")));
      $smarty->assign("gotoProfileFlagCACL" , preg_replace("/w/","",$this->getacl("gotoProfileFlagC")));
    }else{
      $smarty->assign("useProfileCHK"," checked ");
    }
    
    $smarty->assign("gotoProfileServerWriteable", $this->acl_is_writeable("gotoProfileServer"));
    $smarty->assign("gotoProfileACL", $this->getacl("gotoProfileServer").$this->getacl("gotoProfileQuota"));

    /* HANDLE Profile Settings here 
     * Assign available Quota and resolution settings
     * Get all available profile server
     * Get cache checkbox
     * Assign this all to Smarty 
     */

    if(empty($this->gotoProfileFlagL)){
      $smarty->assign("gotoProfileFlagLCHK"," ");
    }else{
      $smarty->assign("gotoProfileFlagLCHK"," checked ");
    }

    if(empty($this->gotoProfileFlagC)){
      $smarty->assign("gotoProfileFlagCCHK"," ");
    }else{
      $smarty->assign("gotoProfileFlagCCHK"," checked ");
    }


    $smarty->assign("gotoXResolutions"    , set_post($this->gotoXResolutions));
    $smarty->assign("gotoXResolutionKeys" , set_post(array_flip($this->gotoXResolutions)));

    $smarty->assign("gotoProfileServers", set_post($this->gotoProfileServers));
    if(!is_array($this->gotoProfileServers)){
      $this->gotoProfileServers =array();
    }

    /* Handle kiosk profiles*/
    $smarty->assign("kiosk_servers" , set_post($this->gotoKioskProfiles['SERVERS']));
    $smarty->assign("kiosk_server" ,  set_post($this->gotoKioskProfile_Server));
    $smarty->assign("kiosk_profiles", set_post($this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server]));
    $smarty->assign("kiosk_profile" , set_post($this->gotoKioskProfile_Profile));
  

    /* Logonscript Management
     * Get available LogonScripts (possibly grey out (or mark) these script that are defined for the group) 
     * Perform add Delete edit Posts 
     */

    /* Dialog Save */
    if(isset($_POST['LogonSave'])){

      if(!$this->acl_is_writeable("gotoLogonScript")){
        msg_dialog::display(_("Permission error"), msgPool::permModify(_("Log on scripts")), ERROR_DIALOG);
        unset($this->dialog);
        $this->dialog=FALSE;
        $this->is_dialog=false;
      }else{
        $this->dialog->save_object();
        if(count($this->dialog->check())!=0){
          foreach($this->dialog->check() as $msg){
            msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
          }
        }else{
          $tmp = $this->dialog->save();
          unset($this->dialog);
          $this->dialog=FALSE;
          $this->is_dialog=false;

          if($this->multiple_support_active){
            $tmp['UsedByAllUsers'] = TRUE;
          }
          $this->gotoLogonScripts[$tmp['LogonName']]=$tmp; 
        }
      }
    }
    

    /* Dialog Quit without saving */
    if(isset($_POST['LogonCancel'])){
      $this->is_dialog= false;
      unset($this->dialog);
      $this->dialog= FALSE;
    }

    /* Check Edit Del New Posts for a selected LogonScript */ 
    if($this->acl_is_writeable("gotoLogonScript") && 
        (isset($_POST['gotoLogonScriptNew'])) || isset($_POST['gotoLogonScriptEdit']) ||isset($_POST['gotoLogonScriptDel'])){

      /* New Logon Script: Open an edit dialog, we don't need a $_POST['gotoLogonScript'] here.
       * In this case we create a new Logon Script.
       */
      if(isset($_POST['gotoLogonScriptNew'])){
        $this->is_dialog = true;
        $this->dialog = new logonManagementDialog($this->config,$this->dn);
      }

      /* If we receive a Delete request and there is a Script selected in the selectbox, delete this one.
       * We only can delete if there is an entry selected.
       */
      if((isset($_POST['gotoLogonScriptDel']))&&(isset($_POST['gotoLogonScript']))){
        unset($this->gotoLogonScripts[$_POST['gotoLogonScript']]);
      }

      /* In this case we want to edit an existing entry, we open a new Dialog to allow editing.
       * There must be an entry selected to perform edit request.
       */
      if((isset($_POST['gotoLogonScriptEdit']))&&(isset($_POST['gotoLogonScript']))){
        $is_entry = $this->gotoLogonScripts[get_post('gotoLogonScript')];
        $this->is_dialog = true;
        $this->dialog = new logonManagementDialog($this->config,$this->dn,$is_entry);
      }
    }

    /* Append List to smarty*/
    if($this->multiple_support_active){
      $smarty->assign("gotoLogonScripts",       set_post($this->gotoLogonScripts));
      $smarty->assign("gotoLogonScriptKeysCnt", count($this->gotoLogonScripts));
    }else{
      $ls = $this->printOutLogonScripts();
      $smarty->assign("gotoLogonScripts",  set_post($ls));
      $smarty->assign("gotoLogonScriptKeys",set_post(array_flip($ls)));
      $smarty->assign("gotoLogonScriptKeysCnt",count($ls));
    }

    /* In this section server shares will be defined 
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */

    $smarty->assign("gotoShareSelections",    set_post($this->gotoShareSelections));
    if(!is_array($this->gotoShareSelections)){
      $this->gotoShareSelections = array();
    }
    $smarty->assign("gotoShareSelectionKeys", set_post(array_flip($this->gotoShareSelections)));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry 
     * This entry will be, a combination of mountPoint and sharedefinitions 
     */
    if(isset($_POST['gotoShareAdd']) && $this->acl_is_writeable("gotoShare")){

      /* We assign a share to this user, if we don't know where to mount the share */
      if(!isset($_POST['gotoShareSelection']) || get_post('gotoShareSelection') == ""){
        msg_dialog::display(_("Error"), msgPool::invalid(_("Share")), ERROR_DIALOG);
      }elseif((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        msg_dialog::display(_("Error"), msgPool::invalid(_("Mount point")), ERROR_DIALOG);
      }elseif(preg_match('/ /', $_POST['gotoShareMountPoint'])){
        msg_dialog::display(_("Error"), msgPool::invalid(_("Mount point"), "/[^\s]/"), ERROR_DIALOG);
      }elseif(!(
            preg_match("/^\//",$_POST['gotoShareMountPoint'])  ||
            preg_match("/^~/",$_POST['gotoShareMountPoint']) ||
            preg_match("/^\$HOME/",$_POST['gotoShareMountPoint']) ||
            preg_match("/^.HOME/",$_POST['gotoShareMountPoint']) ||
            preg_match("/^\$USER/",$_POST['gotoShareMountPoint']) ||
            preg_match("/^.USER/",$_POST['gotoShareMountPoint']) ||
            preg_match("/^%/",$_POST['gotoShareMountPoint'])
            )
          ){
        msg_dialog::display(_("Error"), msgPool::invalid(_("Mount point")), ERROR_DIALOG);
      }else{
        $a_share = $this->gotoAvailableShares[get_post('gotoShareSelection')];
        $s_mount = get_post('gotoShareMountPoint');
        $s_user  = get_post('ShareUser');
        /* Preparing the new assignment */ 
        $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['Username']=$s_user;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['PwdHash']="";
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;

        if($this->multiple_support_active){
          $this->gotoShares[$a_share['name']."|".$a_share['server']]['UsedByAllUsers']= TRUE;
        }
      }
    }  

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message 
     */
    $once = true;
    if($this->acl_is_writeable("gotoShare")){
      foreach($_POST as $name => $value){
        if((preg_match("/^gotoShareDel_/",$name)) && ($once)){
          $once = false;  
          $key  = preg_replace("/^gotoShareDel_/","",$name);
          $key  = postDecode($key);
          if(isset($this->gotoShares[$key])) {
            unset($this->gotoShares[$key]);
          }

          /* Remove corresponding password entry, too. This is a workaround
             to get rid of old-style entries. */
          $key= preg_replace("/\|/", "|!", $key);
          if(isset($this->gotoShares[$key])) {
            unset($this->gotoShares[$key]);
          }
        }
        if((preg_match("/^gotoShareResetPwd_/",$name)) && ($once)){
          $once = false;
          $key  = preg_replace("/^gotoShareResetPwd_/","",$name);
          $key  = postDecode($key);
          $this->gotoShares[$key]['PwdHash'] = "";
          if(preg_match("/^!/",$this->gotoShares[$key]['server'])){
            unset($this->gotoShares[$key]);
          }
        }
      }
    }

    // Build up share list
    $data = $lData = array(); 
    foreach($this->gotoShares as $key => $entry){
      $img = "";

      // Skip password only entries
      if( empty($entry['server']) && empty($entry['name']) &&
          empty($entry['mountPoint']) && empty($entry['Username'])){
        continue;
      } 

      // While editing mutlipe users at once we've to seperate 
      //  entries used by all users and those used by only some.
      $color = "";
      if($this->multiple_support_active){
        if($entry['UsedByAllUsers']){
          $value .= "&nbsp;(<b>"._("Used by all users")."</b>)";
        }else{
          $color = "color: #999999;";
          $value .= "&nbsp;(<b>"._("Used by some users")."</b>)";
        }
      }

      // Create password reset image button 
      if($entry['PwdHash'] != ""){
          $img.= 
              image('plugins/goto/images/list_reset_password.png',
                      'gotoShareResetPwd_'.postEncode($key),
                      _("Reset password hash"));
      }

      // Build up list entries - Handle entries starting with '!' here.
      $data[$key]=$key;
      if(preg_match("/^!/",$this->gotoShares[$key]['server'])){

        // If we are currently editing groups environment, skip those ! entries */ 
        if($this->is_group) continue;

        $lData[$key] = array('data' => array(
                    $entry['server']."://",
                    $entry['name'], 
                    "",
                    "",
                    image("plugins/groups/images/select_group.png","",_("Group share")),
                    $img));
      }else{

        $img.= image('images/lists/trash.png',"gotoShareDel_".postEncode($key),msgPool::delButton());
        $lData[$key] = array('data' => array(
                    $entry['server']."://",
                    $entry['name'], 
                    $entry['mountPoint'], 
                    $entry['Username'], 
                    image("plugins/users/images/select_user.png","",_("User share")),
                    $img));
      }
    }
    $this->shareList->setListData($data,$lData);
    $this->shareList->update();
    $this->shareList->setAcl($this->getacl("gotoShare"));
    $smarty->assign("shareList",$this->shareList->render());

    /* Hotplug devices will be handled here 
     * There are 3 possible methods for this feature
     * Create a new Hotplug, A Dialog will open where you can specify some hotplug information
     * Delete will erase an entry, the entry must be selcted in the ListBox first
     * Editing an entry will open a dialog where the informations about the selcted entry can be changed
     */

    /* We have to delete the selected hotplug from the list*/
    if((isset($_POST['gotoHotplugDeviceDel']))&&(isset($_POST['gotoHotplugDevice_post'])) && $this->acl_is_writeable("gotoHotplugDeviceDN")){
      if($this->acl_is_writeable("gotoHotplugDeviceDN")){
        foreach($_POST['gotoHotplugDevice_post'] as $name){
          unset($this->gotoHotplugDevices[$name]);
        }
      }
    }

    /* There are already defined hotplugs from other users we could use */
    if(isset($_POST['gotoHotplugDeviceUse']) && $this->acl_is_writeable("gotoHotplugDeviceDN")){
      $tmp  =array();
      foreach($this->gotoHotplugDevices as $plugs){
        $tmp[] = $plugs['name'];
      }
      $this->dialog = new hotplugSelect($this->config, get_userinfo());
      $this->is_dialog = true;
    }

    /* Dialog Aborted */
    if(isset($_POST['hotplugSelect_cancel']) || isset($_POST['cancel-abort'])){
      $this->dialog= FALSE;
      $this->is_dialog = false;
    }

    /* Dialod saved */
    if(isset($_POST['hotplugSelect_save']) || isset($_POST['ok-save'])){

      $res = $this->dialog->save();
      foreach($res as $hotplug){
        $name = $hotplug['cn'][0];
        $entry['dn'] = $hotplug['dn'];

        /* Set class values */
        $tmp = preg_split("/\|/",$hotplug['gotoHotplugDevice'][0]);
        $entry['name']          = $hotplug['cn'][0];
        $entry['description'] = $tmp[0];
        $entry['id']       = $tmp[1];
        $entry['produkt']      = $tmp[2];
        $entry['vendor']     = $tmp[3];
        if($this->multiple_support_active){
          $entry['UsedByAllUsers'] = TRUE;
        }
        $this->gotoHotplugDevices[$name]= $entry; 
      }
      $this->dialog= FALSE;
      $this->is_dialog = false;
    }

    if($this->dialog instanceOf hotplugSelect){
    
      // Build up blocklist
      session::set('filterBlacklist', array('cn' => array_keys($this->gotoHotplugDevices)));
      return($this->dialog->execute());
    }

    if($this->multiple_support_active){
      $smarty->assign("gotoHotplugDevices", set_post($this->gotoHotplugDevices));
    }else{
      $smarty->assign("gotoHotplugDevices", set_post($this->printOutHotPlugDevices()));
      $smarty->assign("gotoHotplugDeviceKeys",set_post(array_flip($this->printOutHotPlugDevices())));
    }

    /* Printer Assignment will managed below 
     * A printer can be assigned in two different ways and two different types
     * There are 2 types of users assigned to a printer : user and admin
     * They only differ in the member attribute they will be assigned to. user: gotoUserPrinter admin: gotoadminPrinter
     * The different types of assigning a user are : 1 assigning a user to a printer 2. assigning a group to a printer
     */ 

    /* First handle Add Post. Open a dialog that allows us to select a printer or two */ 
    if(isset($_POST['gotoPrinterAdd'])){
      $this->dialog = new printerSelect($this->config,get_userinfo());
      $this->is_dialog=true;
    }

    if(isset($_POST['printerSelect_cancel']) || isset($_POST['cancel-abort']) && $this->dialog instanceOf printerSelect){
      $this->is_dialog=false;
      $this->dialog=FALSE;
    }

    if(isset($_POST['printerSelect_save']) || isset($_POST['ok-save']) && $this->dialog instanceOf printerSelect){

      $res = $this->dialog->save();
        foreach($res as $printer){

          $pname = $printer['cn'][0];
          $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'], $printer['dn'],"printer");
          $printerObj->set_acl_base($printer['dn']);

          $type = false;

          
          if($this->is_group){
            if($this->dn == "new"){  
              $type = "AddGroup";
            }elseif(isset($this->NewDeletedPrinters[$pname])){
              $type = "AddGroup";
            }elseif($printerObj->by_object['printgeneric']->AddMember("AddGroup",$this->dn)){
              $type = "AddGroup";
            }
          }else{
            if($this->multiple_support_active){
              $type = "AddUser";
            }elseif(isset($this->NewDeletedPrinters[$pname])){
              $type = "AddUser";
            }elseif($this->dn == "new"){  
              $type = "AddUser";
            }elseif($printerObj->by_object['printgeneric']->AddMember("AddUser",$this->dn)){
              $type = "AddUser";
            }
          }

          if($type){
            $this->gotoPrinter[$pname]=$printer;
            $this->gotoPrinter[$pname]['mode']="user";
            $this->add_del_printer_member_was_called = true;

            $this->NewAddedPrinters[$pname] = $pname;
            if(isset($this->NewDeletedPrinters[$pname])){
              unset($this->NewDeletedPrinters[$pname]);
            }
          }
        }

        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog   =FALSE;
    }

    // Display printer selection dialog 
    if($this->dialog instanceOf printerSelect){

      // Build up blocklist
      session::set('filterBlacklist',array('cn' => array_keys($this->gotoPrinter)));
      return($this->dialog->execute());
    }


    if((isset($_POST['gotoPrinterDel']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
      $printer = get_post('gotoPrinterSel');
      foreach($printer as $pname){

        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$this->gotoPrinter[$pname]['dn'],"printer");
        $printerObj->set_acl_base($this->gotoPrinter[$pname]['dn']);

        $type = false;
        if($this->is_group){
          if(isset($this->NewAddedPrinters[$pname])){
            $type = "Group";
          }elseif($printerObj->by_object['printgeneric']->DelMember("AddGroup",$this->cn)){
            $type = "Group";
          }
        }else{
          if(isset($this->NewAddedPrinters[$pname])){
            $type = "User";
          }elseif($printerObj->by_object['printgeneric']->DelMember("AddUser",$this->uid)){
            $type = "User";
          }
        }
        if($type){
          $this->add_del_printer_member_was_called = true;
          unset($this->gotoPrinter[$pname]);

          $this->NewDeletedPrinters[$pname] = $pname;
          if(isset($this->NewAddedPrinters[$pname])){
            UNSET($this->NewAddedPrinters[$pname]);
          }
        }
      }
    }

    if((isset($_POST['gotoPrinterEdit']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
      $printers = get_post('gotoPrinterSel');
      $this->add_del_printer_member_was_called = true;
      foreach($printers as $printer){
        if($this->gotoPrinter[$printer]['mode']=="user"){
          $this->gotoPrinter[$printer]['mode']="admin";
        }else{
          $this->gotoPrinter[$printer]['mode']="user";
        }
      }
    }

    if((isset($_POST['gotoPrinterDefault']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
      if($this->is_group){
        msg_dialog::display(_("Error"), _("Cannot set default printer flag for groups!"), ERROR_DIALOG);
      }else{
        if ($this->gosaDefaultPrinter == $_POST['gotoPrinterSel'][0]){
          $this->gosaDefaultPrinter= "";
        } else {
          $this->gosaDefaultPrinter= $_POST['gotoPrinterSel'][0];
        }
      }
    }

    $smarty->assign("gotoPrinter", set_post($this->printOutPrinterDevices()));

    /* General behavior */
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      $disp =$this->dialog->execute();
      return($disp);
    }

    /* Assign used attributes for multiple edit */
    foreach(array("gotoPrinter","kiosk_server","gotoProfileFlagL","gotoXResolution",
                  "useProfile","gotoProfileServer","gotoProfileQuota","gotoProfileFlagC") as $box){
      $ubox ="use_".$box;
      if(in_array_strict($box,$this->multi_boxes)){
        $smarty->assign($ubox,TRUE);
      }else{
        $smarty->assign($ubox,FALSE);
      }
    }

    /* Als smarty vars are set. Get smarty template and generate output */
    $smarty->assign("multiple_support",$this->multiple_support_active);
    $display.= $smarty->fetch(get_template_path('environment.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function remove_from_parent()
  {
    /* only if it was an account*/
    if (!$this->initially_was_account){
      return;
    }

    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();

    /* Don't save our template variables */
    $skip = array("uid","gotoLogonScripts","gotoPrinter","gotoShares","gotoHotplugDevices" );

    /* Skip all these attributes */
    foreach($skip as $del){
      unset($this->attrs[$del]);
    }

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");

    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->is_group){
      new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove",array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    /* Get all Posted vars 
     * Setup checkboxes 
     */
    if(isset($_POST['iamposted'])){

      $PACL =  $this->getacl("gotoProfileServer").$this->getacl("gotoProfileQuota");

      if(isset($_POST['kiosk_server'])){
        $tmp = get_post('kiosk_server');
        if(isset($this->gotoKioskProfiles['SERVERS'][$tmp])){
          $this->gotoKioskProfile_Server = get_post('kiosk_server');
        }
      }
      if(isset($_POST['kiosk_profile'])){
        $tmp = get_post('kiosk_profile');
        if(in_array_strict($tmp,$this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
          $this->gotoKioskProfile_Profile = get_post('kiosk_profile');
        }
      }

      if(preg_match("/w/",$PACL)){
        if(isset($_POST['useProfile'])){
          $this->useProfile = true;
        }else{
          $this->useProfile = false;
        }
      }

      if($this->acl_is_writeable("gotoProfileFlagC")){
        if(isset($_POST['gotoProfileFlagC'])){
          $this->gotoProfileFlagC = get_post('gotoProfileFlagC');
        }else{
          $this->gotoProfileFlagC = false;
        }
      }

      if($this->acl_is_writeable("gotoProfileFlagL")){
        if(isset($_POST['gotoProfileFlagL'])){
          $this->gotoProfileFlagL = get_post('gotoProfileFlagL');
        }else{
          $this->gotoProfileFlagL = false;
        }
      }

      plugin::save_object();
      foreach($this->attributes as $s_attr){
        if((!isset($_POST[$s_attr])) || 
            in_array_strict($s_attr,array("gosaDefaultPrinter","gotoShare","gotoHotplugDevices","gotoPrinter","gotoLogonScripts","uid"))) continue;
        if(!$this->acl_is_writeable($s_attr)){
          continue;
        }else{ 
          if(isset($_POST[$s_attr])){
            $this->$s_attr = get_post($s_attr);
          }else{
            $this->$s_attr = false;
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

    $this->detect_grouptype();

    if(preg_match("/[^0-9]/",$this->gotoProfileQuota)) {
      $message[] = msgPool::invalid(_("Profile quota"),$this->gotoProfileQuota,"/[0-9]/") ;
    } 
    if(!isset($this->attrs['objectClass'])){
      $this->attrs['objectClass']=array();
    } 
    if(!$this->is_group){
      if((!((in_array_strict("posixAccount",$this->attrs['objectClass']))||($this->parent->by_object['posixAccount']->is_account==true)))&&(!$this->is_group)){
        $message[]= msgPool::featuresDisabled(_("environment"),_("POSIX"));  
      }
    }
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* If group was renamed, all printer settings get lost
     */ 
    /* only save changed variables ....*/
    if ($this->gotoKioskProfile_Server != "none"){
      $method = $this->gotoKioskProfiles['MAP'][$this->gotoKioskProfile_Server][$this->gotoKioskProfile_Profile];
      $this->gotoKioskProfile= $method;
    }else{
      $this->gotoKioskProfile= array();
    }

    plugin::save();
    $ldap= $this->config->get_ldap_link();

    $realyUsedAttrs= array();

    /* Save already used objectClasses */
    $ocs        = $this->attrs['objectClass'];
    unset($ocs['count']);
    $this->attrs = array();
    $this->attrs['objectClass']= gosa_array_merge($ocs,$this->objectclasses);

    /* 1. Search all printers that have our uid/cn as member 
     * 2. Delete this uid/cn from every single entry and save it again.
     * 2.1 There are different types of members: Users / Groups, this will be defined in $suffix
     * 2.2 And each type has two modes, Admin (e.g. 'gotoUserAdminPrinter') and Normal 
     */
    
    $this->detect_grouptype();

    if($this->add_del_printer_member_was_called){

      $types = array( "gotoUserPrinter"       => "AddUser",
          "gotoGroupPrinter"      => "AddGroup",
          "gotoUserAdminPrinter"  => "AddAdminUser",
          "gotoGroupAdminPrinter" => "AddAdminGroup");

      if($this->is_group){
        $s_suffix = "Group";
        $useVar   = "cn";
      }else{
        $useVar   = "uid";
        $s_suffix = "User";
      }

      /* Remove old entries */
      $ldap->cd($this->config->current['BASE']);;
      $ldap->search("(&(objectClass=gotoPrinter)(goto".$s_suffix."Printer=".$this->$useVar."))",array("*"));
      while($attr = $ldap->fetch()){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$attr['dn'],"printer");
        $printerObj->set_acl_base($attr['dn']);
        $printerObj->by_object['printgeneric']->DelMember($types["goto".$s_suffix."Printer"],$this->$useVar);
        $printerObj->by_object['printgeneric']->save();
      }

      $ldap->cd($this->config->current['BASE']);;
      $ldap->search("(&(objectClass=gotoPrinter)(goto".$s_suffix."AdminPrinter=".$this->$useVar."))",array("*"));
      while($attr = $ldap->fetch()){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$attr['dn'],"printer");
        $printerObj->set_acl_base($attr['dn']);
        $printerObj->by_object['printgeneric']->DelMember($types["goto".$s_suffix."AdminPrinter"],$this->$useVar);
        $printerObj->by_object['printgeneric']->save();
      }

      foreach($this->gotoPrinter as $printer){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$printer['dn'],"printer");
        $printerObj->set_acl_base($printer['dn']);


        if($printer['mode'] == "admin") {
          $attribute = "goto".$s_suffix."AdminPrinter";
        }else{
          $attribute = "goto".$s_suffix."Printer";
        }

        $printerObj->by_object['printgeneric']->AddMember($types[$attribute],$this->dn);
        $printerObj->by_object['printgeneric']->save();
      }
    }    

    /* Prepare HotPlug devices */
    $this->attrs['gotoHotplugDeviceDN'] = array();
    foreach($this->gotoHotplugDevices as $name => $device){
      $this->attrs['gotoHotplugDeviceDN'][]= LDAP::fix($device['dn']);
    }

    /* Prepare LogonScripts */
    $this->attrs['gotoLogonScript'] = array();
    foreach($this->gotoLogonScripts as $name => $script){
      $this->attrs['gotoLogonScript'][] =   $script['LogonName']."|".
        $script['LogonOverload'].$script['LogonLast']."|".
        $script['LogonPriority']."|".
        base64_encode($script['LogonData'])."|".
        $script['LogonDescription'];
    }

    /* Prepare Shares */
    $this->attrs['gotoShare']=array();
    foreach($this->gotoShares as $name => $share){
      $mntp= $share['mountPoint'];
      if (!preg_match('=^[a-z0-9+\._/%-]+$=i', $mntp)){
        $mntp= base64_encode($mntp);
      }
      $this->attrs['gotoShare'][] =$share['server']."|".$share['name']."|".$mntp."|".$share['PwdHash']."|".$share['Username'];
    }


    if($this->gotoXResolution == "auto") $this->gotoXResolution ="";
    $saveThis = array("gotoProfileQuota","gotoXResolution","gotoProfileServer","gotoKioskProfile","gosaDefaultPrinter");

    foreach($saveThis as $tosave){
      if(!empty($this->$tosave)){
        $this->attrs[$tosave]=$this->$tosave;
      }else{
        $this->attrs[$tosave]=array();
      }
    }

    /* Prepare Flags */
    $this->attrs['gotoProfileFlags'] = array($this->gotoProfileFlagC.$this->gotoProfileFlagL);
    if(empty($this->attrs['gotoProfileFlags'][0])){
      $this->attrs['gotoProfileFlags']=array();
    }

    if($this->useProfile == false){
      $this->attrs['gotoProfileFlags'] = preg_replace("/C/i","",$this->attrs['gotoProfileFlags']);
      $this->attrs['gotoProfileServer']= array(); 
    }

    $ldap->cat ($this->dn, array('dn'));
    if ($ldap->fetch()){
      $mode= "modify";
    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->$mode($this->attrs);
  
    $cat = "users";
    if($this->is_group){
      $cat = "groups";
    }

    /* Log last action */ 
    if($this->initially_was_account){
      new log("modify",$cat."/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create",$cat."/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }
    $this->handle_post_events($mode,array("uid"=>$this->uid));
  }


  /* Generate ListBox frindly output for the definedhotplugs 
   * Possibly Add or remove an attribute here,
   */
  function printOutHotPlugDevices()
  {
    $a_return= array();
    if(is_array($this->gotoHotplugDevices)){
      foreach($this->gotoHotplugDevices as $key=>$device){
        $a_return[$key] = $device['name']." - ".$device['id'];
      }
    }
    return($a_return);
  }

  /* Generates ListBox frienly output of used printer devices 
   * Append ' - admin' if printer is used in admin mode
   */
  function printOutPrinterDevices()
  {
    $a_return = array();

    if(is_array($this->gotoPrinter)){
      foreach($this->gotoPrinter as $printer){
        if($printer['mode'] == 'admin'){
          $a_return[$printer['cn'][0]]= $printer['cn'][0]." - "._("Administrator");
        }else{
          $a_return[$printer['cn'][0]]= $printer['cn'][0]; 
        }
        if ($printer['cn'][0] == $this->gosaDefaultPrinter){
          $a_return[$printer['cn'][0]].=" - "._("Default printer");
        }
      }
    }
		asort($a_return);
    return($a_return);
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $class= get_class($this);
    $o_source = new $class($this->config,$source['dn']);
    foreach($this->CopyPasteVars as $attr){
      $this->$attr = $o_source->$attr;  
    }
  }


  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = get_post('cn');
      $this->uid = get_post('cn');
    }
  }


  /* Generates ListBox frienly output of used logonscripts 
   */
  function printOutLogonScripts()
  {
    $a_return = array();
    if(is_array($this->gotoLogonScripts)){
      foreach($this->gotoLogonScripts as $script){
        $a_return[$script['LogonName']]= $script['LogonPriority']." - ".$script['LogonName']; 
      }
    }
    return($a_return);
  }


  function multiple_execute()
  {
    /* Reset header toggle */
    $this->is_account = TRUE;
    return($this->execute());
  }


    /* Initialize plugin with given atribute arrays
   */
  function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);

    /* Prepare Shares */
    if((isset($this->multi_attrs_all['gotoShare']))&&(is_array($this->multi_attrs_all['gotoShare']))){
      unset($this->multi_attrs_all['gotoShare']['count']);
      foreach($this->multi_attrs_all['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$share);
        $tmp2['server']      =$tmp[0];
        $tmp2['name']        =$tmp[1];

        /* Decode base64 if needed */
        if (!preg_match('%/%', $tmp[2])){
          $tmp2['mountPoint']  =base64_decode($tmp[2]);
        } else {
          $tmp2['mountPoint']  =$tmp[2];
        }

        if(isset($tmp[3])){
          $tmp2['PwdHash']  =$tmp[3];
        }else{
          $tmp2['PwdHash']  ="";
        }
        if(isset($tmp[4])){
          $tmp2['Username']  =$tmp[4];
        }else{
          $tmp2['Username']  ="";
        }
        $tmp2['UsedByAllUsers'] = FALSE;
        $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
      }
    }

    /* Prepare Shares */
    if((isset($this->multi_attrs['gotoShare']))&&(is_array($this->multi_attrs['gotoShare']))){
      unset($this->multi_attrs['gotoShare']['count']);
      foreach($this->multi_attrs['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$share);
        $tmp2['server']      =$tmp[0];
        $tmp2['name']        =$tmp[1];

        /* Decode base64 if needed */
        if (!preg_match('%/%', $tmp[2])){
          $tmp2['mountPoint']  =base64_decode($tmp[2]);
        } else {
          $tmp2['mountPoint']  =$tmp[2];
        }

        if(isset($tmp[3])){
          $tmp2['PwdHash']  =$tmp[3];
        }else{
          $tmp2['PwdHash']  ="";
        }
        if(isset($tmp[4])){
          $tmp2['Username']  =$tmp[4];
        }else{
          $tmp2['Username']  ="";
        }
        $tmp2['UsedByAllUsers'] = TRUE;
        $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
      }
    }


    /* prepare LogonScripts */
    if((isset($this->multi_attrs_all['gotoLogonScript']))&&(is_array($this->multi_attrs_all['gotoLogonScript']))){
      unset($this->multi_attrs_all['gotoLogonScript']['count']);
      foreach($this->multi_attrs_all['gotoLogonScript'] as $device){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$device);
        $tmp2['LogonName']        = $tmp[0];
        $tmp2['LogonPriority']    = $tmp[2];
        if(preg_match("/O/i",$tmp[1])){
          $tmp2['LogonOverload'] = "O";
        }else{
          $tmp2['LogonOverload'] = "";
        }
        if(preg_match("/L/i",$tmp[1])){
          $tmp2['LogonLast'] = "L";
        }else{
          $tmp2['LogonLast'] = "";
        }
        $tmp2['LogonData']        = base64_decode($tmp[3]);
        $tmp2['LogonDescription'] = $tmp[4];
        $tmp2['UsedByAllUsers'] = FALSE;
        $this->gotoLogonScripts[$tmp[0]]=$tmp2;
      }
    }

    /* prepare LogonScripts */
    if((isset($this->multi_attrs['gotoLogonScript']))&&(is_array($this->multi_attrs['gotoLogonScript']))){
      unset($this->multi_attrs['gotoLogonScript']['count']);
      foreach($this->multi_attrs['gotoLogonScript'] as $device){
        $tmp = $tmp2 = array();
        $tmp = explode("|",$device);
        $tmp2['LogonName']        = $tmp[0];
        $tmp2['LogonPriority']    = $tmp[2];
        if(preg_match("/O/i",$tmp[1])){
          $tmp2['LogonOverload'] = "O";
        }else{
          $tmp2['LogonOverload'] = "";
        }
        if(preg_match("/L/i",$tmp[1])){
          $tmp2['LogonLast'] = "L";
        }else{
          $tmp2['LogonLast'] = "";
        }
        $tmp2['LogonData']        = base64_decode($tmp[3]);
        $tmp2['LogonDescription'] = $tmp[4];
        $tmp2['UsedByAllUsers'] = TRUE;
        $this->gotoLogonScripts[$tmp[0]]=$tmp2;
      }
    }

    /* Prepare hotplugs */
    if(isset($this->multi_attrs_all['gotoHotplugDeviceDN']) && is_array($this->multi_attrs_all['gotoHotplugDeviceDN'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      for($i = 0 ; $i < $this->multi_attrs_all['gotoHotplugDeviceDN']['count'] ; $i ++){
        $ldap->cat($this->multi_attrs_all['gotoHotplugDeviceDN'][$i]);
        if($ldap->count()){
          $multi_attrs_all = $ldap->fetch();

          if(isset($multi_attrs_all['gotoHotplugDevice'][0])){
            $tmp      = preg_split("/\|/",$multi_attrs_all['gotoHotplugDevice'][0]);
            $tmp2     = array();
            $tmp2['name']         = $multi_attrs_all['cn'][0];
            $tmp2['description']  = $tmp[0];
            $tmp2['id']           = $tmp[1];
            $tmp2['produkt']      = $tmp[2];
            $tmp2['vendor']       = $tmp[3];
            $tmp2['dn']           = $multi_attrs_all['dn'];
            $tmp2['UsedByAllUsers'] = FALSE;
            $this->gotoHotplugDevices[$tmp2['dn']] = $tmp2;
          }
        }
      }
    }

    /* Prepare hotplugs */
    if(isset($this->multi_attrs['gotoHotplugDeviceDN']) && is_array($this->multi_attrs['gotoHotplugDeviceDN'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      for($i = 0 ; $i < $this->multi_attrs['gotoHotplugDeviceDN']['count'] ; $i ++){
        $ldap->cat($this->multi_attrs['gotoHotplugDeviceDN'][$i]);
        if($ldap->count()){
          $multi_attrs = $ldap->fetch();

          if(isset($multi_attrs['gotoHotplugDevice'][0])){
            $tmp      = preg_split("/\|/",$multi_attrs['gotoHotplugDevice'][0]);
            $tmp2     = array();
            $tmp2['name']         = $multi_attrs['cn'][0];
            $tmp2['description']  = $tmp[0];
            $tmp2['id']           = $tmp[1];
            $tmp2['produkt']      = $tmp[2];
            $tmp2['vendor']       = $tmp[3];
            $tmp2['dn']           = $multi_attrs['dn'];
            $tmp2['UsedByAllUsers'] = TRUE;
            $this->gotoHotplugDevices[$tmp2['dn']] = $tmp2;
          }
        }
      }
    }
    $this->gotoHotplugDevices = array_values($this->gotoHotplugDevices);
    for($i = 0 ; $i < strlen($this->gotoProfileFlags) ; $i ++){
      $chr = $this->gotoProfileFlags[$i];
      $name = "gotoProfileFlag".$chr;
      $this->$name=$chr;
    }
    $this->update_kiosk_profiles();
    $this->gotoKioskProfile= preg_replace("/^.*\//i","",$this->gotoKioskProfile);

    if((!empty($this->gotoProfileServer))||($this->gotoProfileFlagC=="C")){
      $this->useProfile = true;
    }else{
      $this->useProfile = false;
    }

  }


  function set_multi_edit_values($attrs)
  {
    $shares = $this->gotoShares;
    $scripts= $this->gotoLogonScripts;
    $plugs= $this->gotoHotplugDevices;
    plugin::set_multi_edit_values($attrs);

    $this->gotoShares = $shares;
    $this->gotoLogonScripts = $scripts;
    $this->gotoHotplugDevices = $plugs;

    foreach($attrs['gotoShares'] as $name => $share){
      if($share['UsedByAllUsers'] == TRUE){
        $this->gotoShares[$name] = $share;
      }
    }
    foreach($this->gotoShares as $name => $share){
      if(!isset($attrs['gotoShares'][$name])){
        unset($this->gotoShares[$name]);
      }
    }
    foreach($attrs['gotoLogonScripts'] as $name => $script){
      if($script['UsedByAllUsers'] == TRUE){
        $this->gotoLogonScripts[$name] = $script;
      }
    }
    foreach($this->gotoLogonScripts as $name => $script){
      if(!isset($attrs['gotoLogonScripts'][$name])){
        unset($this->gotoLogonScripts[$name]);
      }
    }
    $tmp = array();
    foreach($this->gotoHotplugDevices as $entry){
      $tmp[$entry['dn']] = $entry;
    }
  
    foreach($attrs['gotoHotplugDevices'] as $name => $plug){
      if($plug['UsedByAllUsers'] == TRUE){
        $tmp[$plug['dn']] = $plug;
      }
    }
    foreach($tmp as $name => $plug){
      $found = false;
      foreach($attrs['gotoHotplugDevices'] as $test){
        if($test['dn'] == $name){
          $found = true;
        }
      }
      if(!$found){
        unset($tmp[$name]);
      }
    }
  }


    /* Return selected values for multiple edit */
  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    $ret['gotoShares'] = $this->gotoShares;
    $ret['gotoLogonScripts'] = $this->gotoLogonScripts;
    $ret['gotoHotplugDevices'] = $this->gotoHotplugDevices;
  
    if(in_array_strict("gotoPrinter",$this->multi_boxes)){
      $ret['gotoPrinter'] = $this->gotoPrinter;
      $ret['gotoPrinterSel'] = $this->gotoPrinterSel;
      $ret['gosaDefaultPrinter'] = $this->gosaDefaultPrinter;
  
      /* Force printer reset */
      $ret['add_del_printer_member_was_called'] = TRUE;
    }
  
    if(in_array_strict("gotoProfileFlagL",$this->multi_boxes)){
      $ret['gotoProfileFlagL'] = $this->gotoProfileFlagL;
    }

    if(in_array_strict("useProfile",$this->multi_boxes)){
      $ret['useProfile']=$this->useProfile;
      if(in_array_strict("gotoProfileServer",$this->multi_boxes)){
        $ret['gotoProfileServer']=$this->gotoProfileServer;
      }
      if(in_array_strict("gotoProfileQuota",$this->multi_boxes)){
        $ret['gotoProfileQuota']=$this->gotoProfileQuota;
      }
      if(in_array_strict("gotoProfileFlagC",$this->multi_boxes)){
        $ret['gotoProfileFlagC'] = $this->gotoProfileFlagC;
      }
    }
    if(in_array_strict("gotoXResolution",$this->multi_boxes)){
      $ret['gotoXResolution'] = $this->gotoXResolution;
    }
    if(in_array_strict("kiosk_server",$this->multi_boxes)){
      $ret['gotoKioskProfile_Server'] = $this->gotoKioskProfile_Server;
      $ret['gotoKioskProfile_Profile'] = $this->gotoKioskProfile_Profile;
    }
    return($ret);
  }


  function multiple_check() 
  {
    $message = plugin::multiple_check();
    $this->detect_grouptype();

    if(preg_match("/[^0-9]/",$this->gotoProfileQuota) && in_array_strict("gotoProfileQuota",$this->multi_boxes)) {
      $message[] = msgPool::invalid(_("Profile quota"),$this->gotoProfileQuota,"/[0-9]/") ;
    }
    return($message);
  }


  function multiple_save_object()
  {
    if(isset($_POST['environment_multiple_posted'])){
      plugin::multiple_save_object();
      
      if(isset($_POST['useProfile'])){
        $this->useProfile = true;
      }else{
        $this->useProfile = false;
      }
      if(isset($_POST['gotoProfileFlagC'])){
        $this->gotoProfileFlagC = get_post('gotoProfileFlagC');
      }else{
        $this->gotoProfileFlagC = false;
      }

      if(isset($_POST['gotoProfileFlagL'])){
        $this->gotoProfileFlagL = get_post('gotoProfileFlagL');
      }else{
        $this->gotoProfileFlagL = false;
      }

      foreach(array("gotoProfileFlagL","gotoPrinter","kiosk_server","gotoXResolution",
                    "useProfile","gotoProfileServer","gotoProfileQuota","gotoProfileFlagC") as $attr){
        $uattr = "use_".$attr;
        if(isset($_POST[$uattr])){
          $this->multi_boxes[] = $attr;
        }
      }
      if(isset($_POST['kiosk_server'])){
        $tmp = get_post('kiosk_server');
        if(isset($this->gotoKioskProfiles['SERVERS'][$tmp])){
          $this->gotoKioskProfile_Server = get_post('kiosk_server');
        }
      }
      if(isset($_POST['kiosk_profile'])){
        $tmp = get_post('kiosk_profile');
        if(in_array_strict($tmp,$this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
          $this->gotoKioskProfile_Profile = get_post('kiosk_profile');
        }
      }

    }
  }

  /* Return plugin informations for acl handling 
#FIXME these ACLs should work for groups too */ 
  static function plInfo()
  {
      return (array("plShortName"     => _("Desktop"),
                  "plDescription"   => _("Desktop settings"),         // Description
                  "plSelfModify"    => TRUE,                              
                  "plDepends"       => array("user", "posixAccount"),     // This plugin depends on 
                  "plPriority"      => 3,                                 // Position in tabs 
                  "plSection"     => array("personal" => _("My account")),
                  "plCategory"    => array("users",
                      "groups","ogroups"),
                  "plOptions"       => array("resolution_hook" => array("type" => "string",
                          "description" => _("Command to extend the list of possible screen resolutions"))),
                  "plRequirements"=> array(
                      'ldapSchema' => array('gotoEnvironment' => '>=2.7'),
                      'onFailureDisablePlugin' => array(get_class())
                      ),
                  "plProperties" => array(

                      array(
                          "name"          => "resolutions",
                          "type"          => "file",
                          "default"       => "",
                          "description"   => _("File containing additional resolutions for workstations and thin clients."),
                          "check"         => "gosaProperty::isReadableFile",
                          "migrate"       => "",
                          "group"         => "environment",
                          "mandatory"     => FALSE
                          ),
                      array(
                          "name"          => "kioskPath",
                          "type"          => "path",
                          "default"       => "/var/spool/kiosk",
                          "description"   => _("Directory to store KDE kiosk profiles."),
                          "check"         => "gosaProperty::isWriteablePath",
                          "migrate"       => "",
                          "group"         => "environment",
                          "mandatory"     => FALSE
                          )
                      ),


                  "plProvidedAcls"  => array(

                          "gotoPrinter"         => _("Printer") ,
                          "gotoProfileServer"   => _("Profile server") ,
                          "gosaDefaultPrinter"  => _("Default printer"),
                          "gotoProfileQuota"    => _("Profile quota") ,
                          "gotoProfileFlagC"    => _("Cache profile locally") ,
                          "gotoShare"          => _("Shares"),
                          "gotoHotplugDeviceDN" => _("Hot-plug devices"),
                          "gotoKioskProfile"    => _("Kiosk profile") ,
                          "gotoProfileFlagL"    => _("Resolution changeable during session") ,
                          "gotoXResolution"     => _("Resolution") ,
                          "gotoLogonScript"     => _("Log on script"))
                      ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
