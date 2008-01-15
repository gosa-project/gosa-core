<?php

class environment extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $plHeadline           = "Environment";
  var $plDescription        = "This does something";
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

  var $CopyPasteVars      = array("gotoHotplugDevices","gotoAvailableShares","gotoShareSelections","gotoPrinterSel","gotoProfileFlagL","gotoXResolutions","gotoProfileFlagC","gotoProfileServers","useProfile","is_group","in_dialog","OrigCn","gotoKioskProfile_Server","gotoKioskProfile_Profile");

  var $attributes         = array("uid","gotoProfileServer","gotoProfileFlags","gotoHotplugDeviceDN",
      "gotoXResolution","gotoProfileQuota",
      "gotoLogonScripts","gotoLogonScript",
      "gotoPrinter", "gosaDefaultPrinter",
      "gotoShares","gotoShare",
      "gotoKioskProfile");
  var $objectclasses      = array("gotoEnvironment"); // Specifies the objectClass which contains the attributes edited here 
  var $cn;
  var $OrigCn;
  var $add_del_printer_member_was_called = false;

  var $multiple_support =TRUE;

  var $use_gotoPrinter;

  function environment (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

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
        $tmp = split("\|",$device);
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
        $tmp = split("\|",$share);
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

    if(isset($this->config->data['MAIN']['RESOLUTION_HOOK'])){
      $file = $this->config->data['MAIN']['RESOLUTION_HOOK'];

      if(is_readable($file)){
        $str = file_get_contents($file);
        $lines = split("\n",$str);
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

    $this->update_kiosk_profiles();
  
    $this->gotoKioskProfile= preg_replace("/^.*\//i","",$this->gotoKioskProfile);
  }


  function update_kiosk_profiles()
  { 
    $tmp1  = array("none" => array(_("disabled")));
    $tmp2  = array("none" => _("disabled"));
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=goEnvironmentServer)(gotoKioskProfile=*)(cn=*))",array("cn","gotoKioskProfile"));
    $cnt = 0;
    while($attrs = $ldap->fetch()){
      for($i = 0 ; $i < $attrs['gotoKioskProfile']['count'] ; $i ++){
        $name = preg_replace("/^.*kiosk\//","",$attrs['gotoKioskProfile'][$i]);
        $tmp1[$attrs['cn'][0]][] = $name;
      }
      $tmp2[$attrs['cn'][0]]= $attrs['cn'][0];
      $cnt ++;
    }

    if($cnt && $this->config->search("environment","kioskpath",array('menu','tabs'))){
      $this->kiosk_enabled = TRUE;
    }

    $this->gotoKioskProfiles['BY_SERVER'] = $tmp1;
    $this->gotoKioskProfiles['SERVERS']   = $tmp2;
  
    $this->gotoKioskProfile_Server = preg_replace("/^.*:\/\/([^\/]*).*$/","\\1",$this->gotoKioskProfile);
    $this->gotoKioskProfile_Profile= preg_replace("/^.*\//","",$this->gotoKioskProfile);

    $error = false;
    if(!in_array($this->gotoKioskProfile_Server, $this->gotoKioskProfiles['SERVERS'])){
      $this->gotoKioskProfile_Server = key($this->gotoKioskProfiles['SERVERS']);
      $error = true;
    }
    if(!in_array($this->gotoKioskProfile_Profile, $this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
      $this->gotoKioskProfile_Profile = $this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server][0];
      $error = true;
    }
    if($error && !empty($this->gotoKioskProfile)){
      $this->gotoKioskProfile_Server ="none";
      $this->gotoKioskProfile_Profile="";
      msg_dialog::display(_("Warning"), sprintf(_("Kiosk profile '%s' located on server '%s' is not available anymore. Kiosk profile will be disabled!"), $this->gotoKioskProfile_Profile, $this->gotoKioskProfile_Server), WARNING_DIALOG);
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
      if(isset($this->parent->by_object['user']) || (isset($this->attrs['objectClass']) &&in_array("gosaAccount",$this->attrs['objectClass']))){
        new log("view","users/".get_class($this),$this->dn);
      }else{
        new log("view","groups/".get_class($this),$this->dn);
      }
    }

    /* Are we editing from MyAccount and not editing a user */
    $WriteOnly = (!isset($this->parent)|| !$this->parent) && !session::is_set('edit');

    /* Check profile server */
    if($this->acl_is_writeable("gotoProfileServer",$WriteOnly)){
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
      $smarty->assign($s_attr,$this->$s_attr);

      /* Set checkbox state*/
      if(empty($this->$s_attr)){
        $smarty->assign($s_attr."CHK","");
      }else{
        $smarty->assign($s_attr."CHK"," checked ");
      }

      /* Prepare ACL settings*/
      $smarty->assign($s_attr."ACL",$this->getacl($s_attr,$WriteOnly));
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
        $display= $this->show_enable_header(_("Remove environment extension"),
            _("Environment extension enabled. You can disable it by clicking below."));
      } else {

        /* Environment is disabled 
           If theres is no posixAccount enabled, you won't be able to enable 
           environment extensions
         */
        if((isset($this->parent->by_object['group']))||(isset($this->attrs['objectClass']))&&((in_array("posixAccount",$this->attrs['objectClass'])))){
          // 4. There is a PosixAccount
          $display= $this->show_enable_header(_("Add environment extension"),
              _("Environment extension disabled. You can enable it by clicking below."));
          return $display;
        }else{
          // 4. There is no PosixAccount
          $display= $this->show_enable_header(_("Add environment extension"),
              _("Environment extension disabled. You have to setup a posix account before you can enable this feature."));
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
          $display= $this->show_disable_header(_("Remove environment extension"),
              _("Environment extension enabled. You can disable it by clicking below."));
        } else {

          if($this->parent->by_object['posixAccount']->is_account==true){
            // 4. There is a PosixAccount
            $display= $this->show_disable_header(_("Add environment extension"),
                _("Environment extension disabled. You can enable it by clicking below."));
            return $display;
          }else{
            // 4. There is a PosixAccount
            $display= $this->show_disable_header(_("Add environment extension"),
                _("Environment extension disabled. You have to setup a posix account before you can enable this feature."),TRUE,TRUE);
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
      $smarty->assign($s_attr,$this->$s_attr);

      /* Set checkbox state*/
      if(empty($this->$s_attr)){
        $smarty->assign($s_attr."CHK","");
      }else{
        $smarty->assign($s_attr."CHK"," checked ");
      }

      /* Prepare ACL settings*/
      $smarty->assign($s_attr."ACL",$this->getacl($s_attr,$WriteOnly));
    }

    foreach(array("gotoHotplugDevice","gotoProfileFlagC","gotoProfileFlagL") as $s_attr){
      $smarty->assign($s_attr."ACL",$this->getacl($s_attr,$WriteOnly));
    }

    if($WriteOnly) {
      $smarty->assign("gotoPrinterACL","r");
    }else{
      $smarty->assign("gotoPrinterACL","rw");
    }


    $smarty->assign("useProfile",$this->useProfile);
    if(empty($this->useProfile) && !$this->multiple_support_active){
      $smarty->assign("useProfileCHK","");
      $smarty->assign("gotoProfileServerACL" , preg_replace("/w/","",$this->getacl("gotoProfileServer",$WriteOnly)));
      $smarty->assign("gotoProfileQuotaACL" , preg_replace("/w/","",$this->getacl("gotoProfileQuota",$WriteOnly)));
      $smarty->assign("gotoProfileFlagCACL" , preg_replace("/w/","",$this->getacl("gotoProfileFlagC",$WriteOnly)));
    }else{
      $smarty->assign("useProfileCHK"," checked ");
    }
    
    $smarty->assign("gotoProfileServerWriteable", $this->acl_is_writeable("gotoProfileServer",$WriteOnly));
    $smarty->assign("gotoProfileACL", $this->getacl("gotoProfileServer",$WriteOnly).$this->getacl("gotoProfileQuota",$WriteOnly));

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


    $smarty->assign("gotoXResolutions"    , $this->gotoXResolutions);
    $smarty->assign("gotoXResolutionKeys" , array_flip($this->gotoXResolutions));

    $smarty->assign("gotoProfileServers",$this->gotoProfileServers);
    if(!is_array($this->gotoProfileServers)){
      $this->gotoProfileServers =array();
    }
    $smarty->assign("gotoProfileServerKeys",array_flip($this->gotoProfileServers));

    /* Handle kiosk profiles*/
    $smarty->assign("kiosk_servers" , $this->gotoKioskProfiles['SERVERS']);
    $smarty->assign("kiosk_server" ,  $this->gotoKioskProfile_Server);
    $smarty->assign("kiosk_profiles" , $this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server]);
    $smarty->assign("kiosk_profile" ,  $this->gotoKioskProfile_Profile);
  

    /* Logonscript Management
     * Get available LogonScripts (possibly grey out (or mark) these script that are defined for the group) 
     * Perform add Delete edit Posts 
     */

    /* Dialog Save */
    if(isset($_POST['LogonSave'])){

      if(!$this->acl_is_writeable("gotoLogonScript")){
        msg_dialog::display(_("Permission error"), _("You have no permission to modify logon scripts!"), ERROR_DIALOG);
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
        $is_entry = $this->gotoLogonScripts[$_POST['gotoLogonScript']];
        $this->is_dialog = true;
        $this->dialog = new logonManagementDialog($this->config,$this->dn,$is_entry);
      }
    }

    /* Append List to smarty*/
    if($this->multiple_support_active){
      $smarty->assign("gotoLogonScripts",  $this->gotoLogonScripts);
      $smarty->assign("gotoLogonScriptKeysCnt",count($this->gotoLogonScripts));
    }else{
      $ls = $this->printOutLogonScripts();
      $smarty->assign("gotoLogonScripts",  $ls);
      $smarty->assign("gotoLogonScriptKeys",array_flip($ls));
      $smarty->assign("gotoLogonScriptKeysCnt",count($ls));
    }

    /* In this section server shares will be defined 
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */

    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    if(!is_array($this->gotoShareSelections)){
      $this->gotoShareSelections = array();
    }
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry 
     * This entry will be, a combination of mountPoint and sharedefinitions 
     */
    if(isset($_POST['gotoShareAdd']) && $this->acl_is_writeable("gotoShare")){

      /* We assign a share to this user, if we don't know where to mount the share */
      if(!isset($_POST['gotoShareSelection']) || get_post('gotoShareSelection') == ""){
        msg_dialog::display(_("Error"), _("Please select a valid share!"), ERROR_DIALOG);
      }elseif((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        msg_dialog::display(_("Error"), _("Please specify a valid mount point!"), ERROR_DIALOG);
      }elseif(preg_match('/ /', $_POST['gotoShareMountPoint'])){
        msg_dialog::display(_("Error"), _("You cannot use spaces in the mount path!"), ERROR_DIALOG);
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
        msg_dialog::display(_("Error"), _("Please specify a valid mount point!"), ERROR_DIALOG);
      }else{
        $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
        $s_mount = $_POST['gotoShareMountPoint'];
        $s_user  = $_POST['ShareUser'];
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
          $key  = preg_replace("/_+[xy]$/","",$key);
          $key  = base64_decode($key);
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
          $key  = preg_replace("/_+[xy]$/","",$key);
          $key  = base64_decode($key);
          $this->gotoShares[$key]['PwdHash'] = "";
          if(preg_match("/^!/",$this->gotoShares[$key]['server'])){
            unset($this->gotoShares[$key]);
          }
        }
      }
    }
    $divlistShares = new divSelectBox("gotoShares");
    $divlistShares->SetHeight(100);


    $tmp = array();
    if($this->acl_is_readable("gotoShares")){
      $tmp = $this->printOutAssignedShares();
    }

    
    foreach($tmp as $key => $value){
      $img = "";

      /* Skip apssword only entries */
      if( empty($this->gotoShares[$key]['server']) && 
          empty($this->gotoShares[$key]['name']) &&
          empty($this->gotoShares[$key]['mountPoint']) &&
          empty($this->gotoShares[$key]['Username'])){
        continue;
      } 

      $color = "";
      if($this->multiple_support_active){
        if($this->gotoShares[$key]['UsedByAllUsers']){
          $value .= "&nbsp;(<b>"._("Used by all users")."</b>)";
        }else{
          $color = "color: #999999;";
          $value .= "&nbsp;(<b>"._("Used by some users")."</b>)";
        }
      }

      /* Check if entry starts with an ! */
      if(preg_match("/^!/",$this->gotoShares[$key]['server'])){

        /* If we are currently editing groups environment, skip those ! entries */ 
        if($this->is_group) continue;

        /* Create pwd reset images */
        if($this->gotoShares[$key]['PwdHash'] != ""){
          $img.= "<input type='image' name='gotoShareResetPwd_".base64_encode($key)." 'src='images/list_reset_password.png' alt='"._("Reset password hash")."' 
            title='"._("Reset password hash")."'>";
        }
        $field1 = array("string" => "<font style=\"color:#C0C0C0\">".$value."</font>" , "attach" => "style='".$color."'");
        $field2 = array("string" => $img   , "attach" => "style='border-right:0px;'");
      }else{

        /* Create pwd reset img && delete image */
        if($this->gotoShares[$key]['PwdHash'] != ""){
          $img.= "<input type='image' name='gotoShareResetPwd_".base64_encode($key)." 'src='images/list_reset_password.png' alt='"._("Reset password hash")."' 
            title='"._("Reset password hash")."'>";
          $img.= "&nbsp;";
        }
        $img.= "<input type='image' name='gotoShareDel_".base64_encode($key)." 'src='images/edittrash.png' alt='"._("Delete")."' 
          title='"._("Delete share entry")."'>";
        $field1 = array("string" => $value , "attach" => "style='".$color."'");
        $field2 = array("string" => $img   , "attach" => "style='border-right:0px;'");
      }
      $divlistShares->AddEntry(array($field1,$field2));
    }
    $smarty->assign("divlistShares",$divlistShares->DrawList());

    /* Hotplug devices will be handled here 
     * There are 3 possible methods for this feature
     * Create a new Hotplug, A Dialog will open where you can specify some hotplug information
     * Delete will erase an entry, the entry must be selcted in the ListBox first
     * Editing an entry will open a dialog where the informations about the selcted entry can be changed
     */

    /* We have to delete the selected hotplug from the list*/
    if((isset($_POST['gotoHotplugDeviceDel']))&&(isset($_POST['gotoHotplugDevice_post'])) && $this->acl_is_writeable("gotoHotplugDevice")){
      if($this->acl_is_writeable("gotoHotplugDevice")){
        foreach($_POST['gotoHotplugDevice_post'] as $name){
          unset($this->gotoHotplugDevices[$name]);
        }
      }
    }

    /* There are already defined hotplugs from other users we could use */
    if(isset($_POST['gotoHotplugDeviceUse']) && $this->acl_is_writeable("gotoHotplugDevice")){
      $tmp  =array();
      foreach($this->gotoHotplugDevices as $plugs){
        $tmp[] = $plugs['name'];
      }
      $this->dialog = new hotplugDialog($this->config,$tmp);
      $this->is_dialog = true;
    }

    /* Dialog Aborted */
    if(isset($_POST['HotPlugCancel'])){
      unset($this->dialog);
      $this->dialog= FALSE;
      $this->is_dialog = false;
    }

    /* Dialod saved */
    if(isset($_POST['HotPlugSave'])){

      $this->dialog->save_object();
      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
        }
      }else{
        $this->dialog->save_object();
        $a_tmp = $this->dialog->save();

        if(is_array($a_tmp)){
          foreach($a_tmp as $name => $hotplug){
            if($this->multiple_support_active){
              $hotplug['UsedByAllUsers'] = TRUE;
            }
            $this->gotoHotplugDevices[$name]= $hotplug; 
          }
        }
        unset($this->dialog);
        $this->dialog= FALSE;
        $this->is_dialog = false;
      }
    }

    if($this->multiple_support_active){
      $smarty->assign("gotoHotplugDevices",$this->gotoHotplugDevices);
    }else{
      $smarty->assign("gotoHotplugDevices",$this->printOutHotPlugDevices());
      $smarty->assign("gotoHotplugDeviceKeys",array_flip($this->printOutHotPlugDevices()));
    }

    /* Printer Assignment will managed below 
     * A printer can be assigned in two different ways and two different types
     * There are 2 types of users assigned to a printer : user and admin
     * They only differ in the member attribute they will be assigned to. user: gotoUserPrinter admin: gotoadminPrinter
     * The different types of assigning a user are : 1 assigning a user to a printer 2. assigning a group to a printer
     */ 

    /* First handle Add Post. Open a dialog that allows us to select a printer or two */ 
    if(isset($_POST['gotoPrinterAdd'])){

      $this->is_dialog=true;
      $this->dialog = new selectPrinterDialog($this->config,$this->dn,$this->gotoPrinter);
    }

    if(isset($_POST['PrinterCancel'])){
      $this->is_dialog=false;
      unset($this->dialog);
      $this->dialog=FALSE;
    }

    if(isset($_POST['PrinterSave'])){
      if(count($this->dialog->check())!=0){
        $tmp = $this->dialog->check();
        foreach($tmp as $msg){
          msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
        } 
      }else{
        $this->dialog->save_object();
        $tmp = $this->dialog->save();
        $tmp2= $this->dialog->getPrinter(true);

        foreach($tmp as $pname){
          $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$tmp2[$pname]['dn'],"printer");
          $printerObj->set_acl_base($tmp2[$pname]['dn']);

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
            }elseif($printerObj->by_object['printgeneric']->AddMember("AddUser",$this->dn)){
              $type = "AddUser";
            }
          }

          if($type){
            $this->gotoPrinter[$pname]=$tmp2[$pname];
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
    }

    if((isset($_POST['gotoPrinterDel']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
      $printer = $_POST['gotoPrinterSel'];
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
      $printers = $_POST['gotoPrinterSel'];
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

    $smarty->assign("gotoPrinter",$this->printOutPrinterDevices());
    $smarty->assign("gotoPrinterKeys",array_flip($this->printOutPrinterDevices()));

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
      if(in_array($box,$this->multi_boxes)){
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

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/environment account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove",array("uid" => $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    /* Get all Posted vars 
     * Setup checkboxes 
     */
    $WriteOnly = (!isset($this->parent)|| !$this->parent) && !session::is_set('edit');
    if(isset($_POST['iamposted'])){

      $PACL =  $this->getacl("gotoProfileServer",$WriteOnly).$this->getacl("gotoProfileQuota",$WriteOnly);

      if(isset($_POST['kiosk_server'])){
        $tmp = $_POST['kiosk_server'];
        if(isset($this->gotoKioskProfiles['SERVERS'][$tmp])){
          $this->gotoKioskProfile_Server = $_POST['kiosk_server'];
        }
      }
      if(isset($_POST['kiosk_profile'])){
        $tmp = $_POST['kiosk_profile'];
        if(in_array($tmp,$this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
          $this->gotoKioskProfile_Profile = $_POST['kiosk_profile'];
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
          $this->gotoProfileFlagC = $_POST['gotoProfileFlagC'];
        }else{
          $this->gotoProfileFlagC = false;
        }
      }

      if($this->acl_is_writeable("gotoProfileFlagL")){
        if(isset($_POST['gotoProfileFlagL'])){
          $this->gotoProfileFlagL = $_POST['gotoProfileFlagL'];
        }else{
          $this->gotoProfileFlagL = false;
        }
      }

      plugin::save_object();
      foreach($this->attributes as $s_attr){
        if((!isset($_POST[$s_attr])) || 
            in_array($s_attr,array("gosaDefaultPrinter","gotoShares","gotoHotplugDevices","gotoPrinter","gotoLogonScripts","uid"))) continue;
        if(!$this->acl_is_writeable($s_attr)){
          continue;
        }else{ 
          if(isset($_POST[$s_attr])){
            $this->$s_attr = $_POST[$s_attr];
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
      $message[]=_("Please set a valid profile quota size.");
    } 
    if(!isset($this->attrs['objectClass'])){
      $this->attrs['objectClass']=array();
    } 
    if(!$this->is_group){
      if((!((in_array("posixAccount",$this->attrs['objectClass']))||($this->parent->by_object['posixAccount']->is_account==true)))&&(!$this->is_group)){
        $message[]=(_("You need to setup a valid posix extension in order to enable evironment features."));  
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
      if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
        $method="https://".$this->gotoKioskProfile_Server."/kiosk/".$this->gotoKioskProfile_Profile;
      }else{
        $method="http://".$this->gotoKioskProfile_Server."/kiosk/".$this->gotoKioskProfile_Profile;
      }
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
      $ldap->search("(&(objectClass=gotoPrinter)(goto".$s_suffix."Printer=".$this->$useVar."))",array("*"));
      while($attr = $ldap->fetch()){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$attr['dn'],"printer");
        $printerObj->set_acl_base($attr['dn']);
        $printerObj->by_object['printgeneric']->DelMember($types["goto".$s_suffix."Printer"],$this->$useVar);
        $printerObj->by_object['printgeneric']->save();
      }

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
      $this->attrs['gotoHotplugDeviceDN'][]= $device['dn'];
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

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/environment account with dn '%s' failed."),$this->dn));
    $this->handle_post_events($mode,array("uid"=>$this->uid));
  }

  /* Generate ListBox frindly output for the defined shares 
   * Possibly Add or remove an attribute here, 
   */
  function printOutAssignedShares()
  {
    $a_return = array();
    if(is_array($this->gotoShares)){
      foreach($this->gotoShares as $share){
        if(preg_match("/^!/",$share['server'])){
          $a_return[$share['name']."|".$share['server']]= preg_replace("/^!/","",$share['server'])."://".$share['name']." - "._("group share"); 
        }else{
          $a_return[$share['name']."|".$share['server']]= $share['server']."://".$share['name']." on ".$share['mountPoint']." as ".$share['Username'];
        }
      }
      natcasesort($a_return);
    }
    return($a_return);
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
    
    if($this->acl_is_readable("gotoPrinter")){

      if(is_array($this->gotoPrinter)){
        foreach($this->gotoPrinter as $printer){
          if($printer['mode'] == "admin"){
            $a_return[$printer['cn'][0]]= $printer['cn'][0]." - "._("Administrator");
          }else{
            $a_return[$printer['cn'][0]]= $printer['cn'][0]; 
          }
          if ($printer['cn'][0] == $this->gosaDefaultPrinter){
            $a_return[$printer['cn'][0]].=" - "._("Default printer");
          }
        }
      }
    }
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
      $this->cn = $_POST['cn'];
      $this->uid = $_POST['cn'];
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
        $tmp = split("\|",$share);
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
        $tmp = split("\|",$share);
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
        $tmp = split("\|",$device);
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
        $tmp = split("\|",$device);
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
  
    if(in_array("gotoPrinter",$this->multi_boxes)){
      $ret['gotoPrinter'] = $this->gotoPrinter;
      $ret['gotoPrinterSel'] = $this->gotoPrinterSel;
      $ret['gosaDefaultPrinter'] = $this->gosaDefaultPrinter;
  
      /* Force printer reset */
      $ret['add_del_printer_member_was_called'] = TRUE;
    }
  
    if(in_array("gotoProfileFlagL",$this->multi_boxes)){
      $ret['gotoProfileFlagL'] = $this->gotoProfileFlagL;
    }

    if(in_array("useProfile",$this->multi_boxes)){
      $ret['useProfile']=$this->useProfile;
      if(in_array("gotoProfileServer",$this->multi_boxes)){
        $ret['gotoProfileServer']=$this->gotoProfileServer;
      }
      if(in_array("gotoProfileQuota",$this->multi_boxes)){
        $ret['gotoProfileQuota']=$this->gotoProfileQuota;
      }
      if(in_array("gotoProfileFlagC",$this->multi_boxes)){
        $ret['gotoProfileFlagC'] = $this->gotoProfileFlagC;
      }
    }
    if(in_array("gotoXResolution",$this->multi_boxes)){
      $ret['gotoXResolution'] = $this->gotoXResolution;
    }
    if(in_array("kiosk_server",$this->multi_boxes)){
      $ret['gotoKioskProfile_Server'] = $this->gotoKioskProfile_Server;
      $ret['gotoKioskProfile_Profile'] = $this->gotoKioskProfile_Profile;
    }
    return($ret);
  }


  function multiple_check() 
  {
    $message = plugin::multiple_check();
    $this->detect_grouptype();

    if(preg_match("/[^0-9]/",$this->gotoProfileQuota) && in_array("gotoProfileQuota",$this->multi_boxes)) {
      $message[]=_("Please set a valid profile quota size.");
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
        $this->gotoProfileFlagC = $_POST['gotoProfileFlagC'];
      }else{
        $this->gotoProfileFlagC = false;
      }

      if(isset($_POST['gotoProfileFlagL'])){
        $this->gotoProfileFlagL = $_POST['gotoProfileFlagL'];
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
        $tmp = $_POST['kiosk_server'];
        if(isset($this->gotoKioskProfiles['SERVERS'][$tmp])){
          $this->gotoKioskProfile_Server = $_POST['kiosk_server'];
        }
      }
      if(isset($_POST['kiosk_profile'])){
        $tmp = $_POST['kiosk_profile'];
        if(in_array($tmp,$this->gotoKioskProfiles['BY_SERVER'][$this->gotoKioskProfile_Server])){
          $this->gotoKioskProfile_Profile = $_POST['kiosk_profile'];
        }
      }

    }
  }

  /* Return plugin informations for acl handling 
#FIXME these ACLs should work for groups too */ 
  static function plInfo()
  {
    return (array("plShortName"     => _("Environment"),
          "plDescription"   => _("Environment settings"),         // Description
          "plSelfModify"    => TRUE,                              
          "plDepends"       => array("user", "posixAccount"),     // This plugin depends on 
          "plPriority"      => 3,                                 // Position in tabs 
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users",
                                   "groups"),
          "plOptions"       => array("resolution_hook" => array("type" => "string",
              "description" => _("Command to extend the list of possible screen resolutions"))),

          "plProvidedAcls"  => array(

            "gotoProfileFlagL"    => _("Resolution changeable during session") ,
            "gotoProfileFlagC"    => _("Cache profile localy") ,

            "gotoProfileQuota"    => _("Profile quota") ,
            "gotoProfileServer"   => _("Profile server") ,

            "gotoXResolution"     => _("Resolution") ,
            "gotoKioskProfile"    => _("Kiosk profile") ,

            "gosaDefaultPrinter"  => _("Default printer") ,
            "gotoLogonScript"     => _("Logon script") ,
            "gotoHotplugDevice"   => _("Hotplug devices"),
            "gotoShare"           => _("Shares"))
            ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
