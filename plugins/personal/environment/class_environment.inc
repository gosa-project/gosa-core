<?php

class environment extends plugin
{
  /* attribute list for save action */
  var $ignore_account       = FALSE;
  var $plHeadline           = "Environment";
  var $plDescription        = "This does something";
  var $dialog               = false;    // Indicates that we are currently editing in an seperate dialog
  
  var $in_dialog            = false;
  var $uid                  = "";

  var $is_group             = false;
  
  /* Attribute definition
   */

  /* profile management */
  var $useProfile         = false;  // Specifies if we want to use a Server 
  var $gotoProfileServer  = "";     // Specifies the selected profile server
  var $gotoProfileServers = array();// Specifies all available and selectable servers
  var $gotoProfileFlags   = "";     // Flags enabled  ? only used to set ACL and save 
  var $gotoProfileFlag_C  = "";     // Flag is set to C if we have the profile caching fucntion enabled 
  
  var $gotoXResolution    = "auto";     // The selected resolution eg: 1024x768
  var $gotoXResolutions   = array();// Contains all available resolutions for this account
  var $gotoProfileFlag_L  = "";     // Flag is set to L to enable runtime resolution change 
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
  var $gotoKioskProfile   = "none";     // The selected Kiosk Profile
  var $gotoKioskProfiles  = array();// All available Kiosk profiles
  var $newKioskProfiles   = array();
  var $kioskProfileList   = array(); // Displayed List of Profiles

  /* Hotplug Devices */
  var $gotoHotplugDevice  = array();     // Selected hotplug
  var $gotoHotplugDevices = array();// Already configured hotplug devices 


  /* general settings */
  // Sets the attributes which will kept on page reload, which will be saved, ...

  var $CopyPasteVars      = array("gotoHotplugDevices","newKioskProfiles","gotoAvailableShares","gotoShareSelections","gotoPrinterSel","gotoProfileFlag_L","gotoXResolutions","gotoProfileFlag_C","gotoProfileServers","useProfile","is_group","in_dialog","OrigCn");

  var $attributes         = array("uid","gotoProfileServer","gotoProfileFlags","gotoHotplugDevice",
                                    "gotoXResolution","gotoProfileQuota",
                                    "gotoLogonScripts","gotoLogonScript",
                                    "gotoPrinter", "gosaDefaultPrinter",
                                    "gotoShares","gotoShare",
                                    "gotoKioskProfile","gotoKioskProfiles"
                                    );
  var $objectclasses      = array("gotoEnvironment"); // Specifies the objectClass which contains the attributes edited here 
  var $cn;
  var $OrigCn;

  function environment ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Check : Are we currently editing a group or user dialog */
    if((isset($this->attrs['cn'][0]))&&(!isset($this->attrs['uid'][0]))){
      $suffix="Group";
      $this->uid          = $this->attrs['cn'][0];
      $this->attrs['uid'] = $this->attrs['cn'][0];
      $this->OrigCn = $this->attrs['cn'][0];
    }else{
      $suffix="User";
    }

    $this->gotoKioskProfile= preg_replace("/^.*\//i","",$this->gotoKioskProfile);

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
  
    /* prepare hotplugs */
    if((isset($this->attrs['gotoHotplugDevice']))&&(is_array($this->attrs['gotoHotplugDevice']))){
      unset($this->attrs['gotoHotplugDevice']['count']);
      foreach($this->attrs['gotoHotplugDevice'] as $device){
        $tmp = $tmp2 = array();
        $tmp = split("\|",$device);
        $tmp2['name']        = $tmp[0]; 
        $tmp2['description'] = $tmp[1]; 
        $tmp2['id']          = $tmp[2]; 
        if(isset($tmp[3])){
          $tmp2['produkt']     = $tmp[3];
        }else{
          $tmp2['produkt']     = "";
        }
        if(isset($tmp[4])){
          $tmp2['vendor']      = $tmp[4];
        }else{
          $tmp2['vendor']      = "";
        }
        $this->gotoHotplugDevices[$tmp[0]]=$tmp2;
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
      $name = "gotoProfileFlag_".$chr;
      $this->$name=$chr;
    }

    if((!empty($this->gotoProfileServer))||($this->gotoProfileFlag_C=="C")){
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
        print_red(sprintf(_("You have specified an external resolution hook which can't be read, please check the permission of the file '%s'."),$file));
      }
    }

 
    $this->gotoProfileServers= $config->getShareServerList() ;
    $this->gotoShareSelections= $config->getShareList(true);
    $this->gotoAvailableShares= $config->getShareList(false);  

    if(search_config($this->config->data,"environment", "KIOSKPATH") !=""){
      $tmp = new kioskManagementDialog($this->config,$this->dn);
      $list = $tmp->getKioskProfiles($this->newKioskProfiles);
      $list['none']=_("None");
      $this->kioskProfileList = array_reverse($list);
      $this->kiosk_enabled = TRUE;
    }else{
      $this->kiosk_enabled = FALSE;
      $this->kioskProfileList = array();
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

  /* Fill templating stuff */
  $smarty= get_smarty();
  $smarty->assign("kiosk_enabled",$this->kiosk_enabled);
  $display= "";

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
    if(chkacl($this->acl,$s_attr)=="") {
      $smarty->assign($s_attr."ACL","");
    }else{
      $smarty->assign($s_attr."ACL"," disabled ");
    }

  }

  /* Is accout enabled | are we editing from usermenu or admin menu 
     All these tab management is done here
   */

  $this->detect_grouptype();

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
        if($this->is_account && $this->acl == "#all#"){
          $this->is_account= !$this->is_account;
        }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
          $this->is_account= !$this->is_account;
        }
      }

      /* Group Dialog with enabled environment options */
      if ($this->is_account){
        $display= $this->show_header(_("Remove environment extension"),
            _("Environment extension enabled. You can disable it by clicking below."));
      } else {
  
      /* Environment is disabled 
         If theres is no posixAccount enabled, you won't be able to enable 
         environment extensions
       */
        if((isset($this->parent->by_object['group']))||(isset($this->attrs['objectClass']))&&((in_array("posixAccount",$this->attrs['objectClass'])))){
          // 4. There is a PosixAccount
          $display= $this->show_header(_("Add environment extension"),
              _("Environment extension disabled. You can enable it by clicking below."));
          return $display;
        }else{
          // 4. There is no PosixAccount
          $display= $this->show_header(_("Add environment extension"),
              _("Environment extension disabled. You have to setup a posix account before you can enable this feature."));
          return $display;
        }
      }
    }else{
      /* Editing from Usermenu 
       *  Tell smarty that this accoutn is enabled 
       */
      $smarty->assign("is_account","true");


      /* Do we need to flip is_account state? */
      if (isset($_POST['modify_state'])){

        /* Onyl change account state if allowed */
        if(!$this->parent->by_object['posixAccount']->is_account && !$this->is_account){
        }elseif($this->is_account && $this->acl == "#all#"){
          $this->is_account= !$this->is_account;
        }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
          $this->is_account= !$this->is_account;
        }
      }


      if(isset($this->parent)){

        // 3. Account enabled . Editing from adminmenu
        if ($this->is_account){
          $display= $this->show_header(_("Remove environment extension"),
              _("Environment extension enabled. You can disable it by clicking below."));
        } else {

          if($this->parent->by_object['posixAccount']->is_account==true){
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("Environment extension disabled. You can enable it by clicking below."));
            return $display;
          }else{
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("Environment extension disabled. You have to setup a posix account before you can enable this feature."),TRUE,TRUE);
            return $display;
          }
        }
      }
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
      if(chkacl($this->acl,$s_attr)=="") {
        $smarty->assign($s_attr."ACL","");
      }else{
        $smarty->assign($s_attr."ACL"," disabled ");
      }
 
    }

    foreach(array("gotoHotplugDevice","gotoPrinterSel", "gotoPrinterToggle") as $s_attr){
      if(chkacl($this->acl,$s_attr)=="") {
        $smarty->assign($s_attr."ACL","");
      }else{
        $smarty->assign($s_attr."ACL"," disabled ");
      }
    }

    if(empty($this->useProfile)){
      $smarty->assign("gotoProfileACL","disabled");
      $smarty->assign("useProfileCHK","");
    }else{
      $smarty->assign("gotoProfileACL","");
      $smarty->assign("useProfileCHK"," checked ");
    }


    
    $smarty->assign("useProfileACL", chkacl($this->acl,"gotoProfileServer"));

    $smarty->assign("gotoProfileQuotaACL" , chkacl($this->acl,"gotoProfileQuota"));
    $smarty->assign("gotoProfileFlag_CACL", chkacl($this->acl,"gotoProfileFlags"))  ;
    $smarty->assign("gotoProfileServerACL", chkacl($this->acl,"gotoProfileServer"))  ;


    /* HANDLE Profile Settings here 
     * Assign available Quota and resolution settings
     * Get all available profile server
     * Get cache checkbox
     * Assign this all to Smarty 
     */

    if(empty($this->gotoProfileFlag_L)){
      $smarty->assign("gotoProfileFlag_LCHK"," ");
    }else{
      $smarty->assign("gotoProfileFlag_LCHK"," checked ");
    }

    if(empty($this->gotoProfileFlag_C)){
      $smarty->assign("gotoProfileFlag_CCHK"," ");
    }else{
      $smarty->assign("gotoProfileFlag_CCHK"," checked ");
    }


    $smarty->assign("gotoXResolutions"    , $this->gotoXResolutions);
    $smarty->assign("gotoXResolutionKeys" , array_flip($this->gotoXResolutions));

    $smarty->assign("gotoProfileServers",$this->gotoProfileServers);
    if(!is_array($this->gotoProfileServers)){
      $this->gotoProfileServers =array();
    }
    $smarty->assign("gotoProfileServerKeys",array_flip($this->gotoProfileServers));

    /* Handle kiosk profiles 
     * Read available from filesystem
     * Open management if post is transmitted
     */

    /* Save */
    if(isset($_POST['KioskClose'])){
      $this->newKioskProfiles = array_merge($this->newKioskProfiles,$this->dialog->save());

      /* Update list of kiosk profiles */ 
      $list = $this->dialog->getKioskProfiles($this->newKioskProfiles);
      $list['none']=_("None");
      $this->kioskProfileList = array_reverse($list);

      unset($this->dialog);
      $this->dialog=NULL;
      $this->is_dialog = false;
    }

    /* Reassign help class */
    $_SESSION['current_class_for_help'] = get_class($this);
 
    /* Open Management Dialog */
    if(isset($_POST['KioskManagementDialog']) && chkacl($this->acl,"gotoKioskProfile") == ""){
      $this->dialog = new kioskManagementDialog($this->config,$this->dn,$this->newKioskProfiles); 
      $this->dialog->parent= $this;
      $this->dialog->acl = $this->acl;
      $this->is_dialog = true;
    }
    $smarty->assign("gotoKioskProfiles",$this->kioskProfileList);
    $smarty->assign("gotoKioskProfileKeys",array_flip($this->kioskProfileList));

    /* Logonscript Management
     * Get available LogonScripts (possibly grey out (or mark) these script that are defined for the group) 
     * Perform add Delete edit Posts 
     */

    /* Dialog Save */
    if(isset($_POST['LogonSave'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $tmp = $this->dialog->save();
        unset($this->dialog);
        $this->dialog=NULL;
        $this->is_dialog=false;
        $this->gotoLogonScripts[$tmp['LogonName']]=$tmp; 
      }
    }
    
    /* Dialog Quit without saving */
    if(isset($_POST['LogonCancel'])){
      $this->is_dialog= false;
      unset($this->dialog);
      $this->dialog= NULL;
    }
   
    /* Check Edit Del New Posts for a selected LogonScript */
    if((chkacl($this->acl,"gotoLogonScript") == "") && 
        (isset($_POST['gotoLogonScriptNew'])||isset($_POST['gotoLogonScriptEdit'])||isset($_POST['gotoLogonScriptDel']))){

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
    $smarty->assign("gotoLogonScripts",   $this->printOutLogonScripts());
    $smarty->assign("gotoLogonScriptKeys",array_flip($this->printOutLogonScripts()));

    /* In this section server shares will be defined 
     * A user can select one of the given shares and a mount point
     *  and attach this combination to his setup.
     */
    
    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    if(!is_array($this->gotoShareSelections)){
      print $this->gotoShareSelections;
      $this->gotoShareSelections = array();
    }
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry 
     * This entry will be, a combination of mountPoint and sharedefinitions 
     */
    if(isset($_POST['gotoShareAdd']) && chkacl($this->acl,"gotoShare") ==""){
      /* We assign a share to this user, if we don't know where to mount the share */
      if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        print_red(_("You must specify a valid mount point."));
      }elseif(preg_match('/ /', $_POST['gotoShareMountPoint'])){
        print_red(_("Spaces are not allowed in the mount path!"));
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
        print_red(_("You must specify a valid mount point.")); 
      }else{
        $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
        $s_mount = $_POST['gotoShareMountPoint'];
        $s_user  = $_POST['ShareUser'];
        /* Preparing the new assignment */ 
        $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['Username']=$s_user;
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['PwdHash']="";
        $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
      }
    }  

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message 
     */
    $once = true;
    foreach($_POST as $name => $value){
      if((preg_match("/^gotoShareDel_/",$name)) && ($once) && chkacl($this->acl,"gotoShare") == ""){
        $once = false;  
        $key  = preg_replace("/^gotoShareDel_/","",$name);
        $key  = preg_replace("/_+[xy]$/","",$key);
        $key  = base64_decode($key);
        if(isset($this->gotoShares[$key])) {
          unset($this->gotoShares[$key]);
        }

        /* Remove corresponding password entry, too. This is a workaround
           to get rid of old-style entries. */
        $key= base64_decode($key);
        if(isset($this->gotoShares[$key])) {
          unset($this->gotoShares[$key]);
        }

      }
      if((preg_match("/^gotoShareResetPwd_/",$name)) && ($once) && chkacl($this->acl,"gotoShare") == ""){
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

    $divlistShares = new divSelectBox("gotoShares");
    $divlistShares->SetHeight(100);
    $tmp = $this->printOutAssignedShares();
    
    
    foreach($tmp as $key => $value){
      $img = "";
   
      $hide       = false;    
      $background = "";
      if(chkacl($this->acl,"gotoShare") != ""){
        $background = "background: #D0D0D0; ";
        $hide = true;    
      }

      /* Skip apssword only entries */
      if( empty($this->gotoShares[$key]['server']) && 
          empty($this->gotoShares[$key]['name']) &&
          empty($this->gotoShares[$key]['mountPoint']) &&
          empty($this->gotoShares[$key]['Username'])){
        continue;
      } 

      /* Check if entry starts with an ! */
      if(preg_match("/^!/",$this->gotoShares[$key]['server'])){

        /* If we are currently editing groups environment, skip those ! entries */ 
        if($this->is_group) continue;
   
        /* Create pwd reset images */
        if($this->gotoShares[$key]['PwdHash'] != "" && !$hide){
          $img.= "<input type='image' name='gotoShareResetPwd_".base64_encode($key)." 'src='images/list_reset_password.png' alt='"._("Reset password hash")."' 
            title='"._("Reset password hash")."'>";
        }
        $field1 = array("string" => "<font style=\"color:#C0C0C0\">".$value."</font>" , "attach" => "style='".$background."'");
        $field2 = array("string" => $img   , "attach" => "style='".$background."border-right:0px;'");
      }else{

        /* Create pwd reset img && delete image */
        if($this->gotoShares[$key]['PwdHash'] != ""){
          $img.= "<input type='image' name='gotoShareResetPwd_".base64_encode($key)." 'src='images/list_reset_password.png' alt='"._("Reset password hash")."' 
            title='"._("Reset password hash")."'>";
          $img.= "&nbsp;";
        }
        if(!$hide){
        $img.= "<input type='image' name='gotoShareDel_".base64_encode($key)." 'src='images/edittrash.png' alt='"._("Delete")."' 
          title='"._("Delete share entry")."'>";
        }
        $field1 = array("string" => $value , "attach" => "style='".$background."'");
        $field2 = array("string" => $img   , "attach" => "style='".$background."border-right:0px;'");
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

    /* If there is a new entry wanted, open a new entry by initilising the dialog */
    if(isset($_POST['gotoHotplugDeviceNew']) && chkacl($this->acl,"gotoHotplugDevice") == ""){
      $this->dialog = new hotplugDialog($this->config,$this->dn);
      $this->is_dialog = true;
    }

    /* We have to delete the selected hotplug from the list*/
    if((isset($_POST['gotoHotplugDeviceDel']))&&(isset($_POST['gotoHotplugDevice']))){
      foreach($_POST['gotoHotplugDevice'] as $name){
        unset($this->gotoHotplugDevices[$name]);
      }
    }

    /* There are already defined hotplugs from other users we could use */
    if(isset($_POST['gotoHotplugDeviceUse']) && chkacl($this->acl,"gotoHotplugDevice") == ""){
      $this->dialog = new hotplugDialog($this->config,$this->dn,true);
      $this->is_dialog = true;
    }

    /* Dialog Aborted */
    if(isset($_POST['HotPlugCancel'])){
      unset($this->dialog);
      $this->dialog= NULL;
      $this->is_dialog = false;
    }
 
    /* Dialod saved */
    if(isset($_POST['HotPlugSave'])){
      $this->dialog->save_object();
      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $this->dialog->save_object();
        $a_tmp = $this->dialog->save();
       
        if(is_array($a_tmp)){
          foreach($a_tmp as $name => $hotplug){
            $this->gotoHotplugDevices[$name]= $hotplug; 
          }
        }
        unset($this->dialog);
        $this->dialog= NULL;
        $this->is_dialog = false;
      }
    }
    
    $smarty->assign("gotoHotplugDevices",$this->printOutHotPlugDevices());
    $smarty->assign("gotoHotplugDeviceKeys",array_flip($this->printOutHotPlugDevices()));
  
    /* Printer Assignment will managed below 
     * A printer can be assigned in two different ways and two different types
     * There are 2 types of users assigned to a printer : user and admin
     * They only differ in the member attribute they will be assigned to. user: gotoUserPrinter admin: gotoadminPrinter
     * The different types of assigning a user are : 1 assigning a user to a printer 2. assigning a group to a printer
     */ 
    
    /* First handle Add Post. Open a dialog that allows us to select a printer or two */ 
    if(isset($_POST['gotoPrinterAdd'])  && chkacl($this->acl,"gotoPrinter") == ""){
      $this->is_dialog=true;
      $this->dialog = new selectPrinterDialog($this->config,$this->dn,$this->gotoPrinter);
    }

    if(isset($_POST['PrinterCancel'])){
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=NULL;
    }

    if(isset($_POST['PrinterSave'])){
      if(count($this->dialog->check())!=0){
        $tmp = $this->dialog->check();
        foreach($tmp as $msg){
          print_red($msg);
        } 
      }else{
        $this->dialog->save_object();
        $tmp = $this->dialog->save();
        $tmp2= $this->dialog->getPrinter(true);
  
        foreach($tmp as $pname){
          $this->gotoPrinter[$pname]=$tmp2[$pname];
          $this->gotoPrinter[$pname]['mode']="user";
        }
  
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog   =NULL;
      }
    }
 
    
    /* So some printer operations if allowed */
    if(chkacl($this->acl,"gotoPrinter")==""){ 
      if((isset($_POST['gotoPrinterDel']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
        $printer = $_POST['gotoPrinterSel'];
        foreach($printer as $pname){
          unset($this->gotoPrinter[$pname]);
        }
      }

      if((isset($_POST['gotoPrinterEdit']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
        $printers = $_POST['gotoPrinterSel'];

        foreach($printers as $printer){
          if($this->gotoPrinter[$printer]['mode']=="user"){
            $this->gotoPrinter[$printer]['mode']="admin";
          }else{
            $this->gotoPrinter[$printer]['mode']="user";
          }
        }
      }

      if(!$this->is_group){
        if((isset($_POST['gotoPrinterDefault']))&&(isset($_POST['gotoPrinterSel']))&&(!empty($_POST['gotoPrinterSel']))){
          if ($this->gosaDefaultPrinter == $_POST['gotoPrinterSel'][0]){
            $this->gosaDefaultPrinter= "";
          } else {
            $this->gosaDefaultPrinter= $_POST['gotoPrinterSel'][0];
          }
        }
      }  
    }

    $smarty->assign("gotoPrinter",$this->printOutPrinterDevices());
    $smarty->assign("gotoPrinterKeys",array_flip($this->printOutPrinterDevices()));
 
    /* General behavior */
    if((isset($this->dialog))&&($this->dialog!=NULL)&&(!empty($this->dialog))){
      $this->dialog->save_object();
      $disp =$this->dialog->execute();

      $tmp = new kioskManagementDialog($this->config,$this->dn);
      $list = $tmp->getKioskProfiles($this->newKioskProfiles);
      $list['none']=_("None");
      $list = array_reverse($list);
      if(!isset($list[$this->gotoKioskProfile])){
        print_red(sprintf(_("The selected kiosk profile '%s' is no longer available, setting current profile to 'none'."),$this->gotoKioskProfile));
        $this->gotoKioskProfile = 'none';
      }
      return($disp);
    }

    if(!$this->useProfile){
      $smarty->assign("gotoProfileFlag_CACL"," disabled ");
      $smarty->assign("gotoProfileServerACL"," disabled ");
      $smarty->assign("gotoProfileQuotaACL"," disabled ");
    }

    $smarty->assign("is_group",$this->is_group);

    /* Als smarty vars are set. Get smarty template and generate output */
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
    $skip = array("uid","gotoLogonScripts","gotoPrinter","gotoShares","gotoKioskProfiles","gotoHotplugDevices" );

    /* Skip all these attributes */
    foreach($skip as $del){
      unset($this->attrs[$del]);
    }

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");

    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs);

    show_ldap_error($ldap->get_error(), _("Removing environment information failed"));

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

      if(chkacl($this->acl,"gotoProfileServer") == ""){
        if(isset($_POST['useProfile'])){
          $this->useProfile = true;
        }else{
          $this->useProfile = false;
        }
      }

      if(chkacl($this->acl,"gotoProfileFlags") == ""){
        if(isset($_POST['gotoProfileFlag_C'])){
          $this->gotoProfileFlag_C = $_POST['gotoProfileFlag_C'];
        }else{
          $this->gotoProfileFlag_C = false;
        }
        if(isset($_POST['gotoProfileFlag_L'])){
          $this->gotoProfileFlag_L = $_POST['gotoProfileFlag_L'];
        }else{
          $this->gotoProfileFlag_L = false;
        }
      }

      $tmp= $this->gosaDefaultPrinter;
      plugin::save_object();
      $this->gosaDefaultPrinter= $tmp;
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $this->detect_grouptype();
    $message= plugin::check();
  
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
    if ($this->gotoKioskProfile =="none") $this->gotoKioskProfile ="";
    if((!empty($this->gotoKioskProfile))&&($this->gotoKioskProfile != "none")){
      if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
        $method="https://";
      }else{
        $method="http://";
      }

      $str = $method.str_replace("//","/",$_SERVER['SERVER_NAME']."/kiosk/");
      $this->gotoKioskProfile= $str.$this->gotoKioskProfile;
    }else{
      $this->gotoKioskProfile= array();
    }
    
    plugin::save();
    $ldap= $this->config->get_ldap_link();

    $realyUsedAttrs= array();

    $path = search_config($this->config->data,"environment", "KIOSKPATH"); 
    /* Creating Kiosk Profiles */
    foreach($this->newKioskProfiles as $file){
      $contents = $file['contents'];
      $fp = @fopen($path."/".$file['name'],"w");
      if(!$fp){
        print_red(_("Can't save new kiosk profiles, possibly permission denied for folder")." : ",$path);
      }else{
        fwrite($fp,$contents,strlen($contents));
      }
      @unlink($file['tmp_name']);
    }
 
    /* Save already used objectClasses */
    $ocs        = $this->attrs['objectClass'];
    unset($ocs['count']);
    $this->attrs = array();
    $this->attrs['objectClass']= $ocs;
    foreach($this->objectclasses as $objc){
      if(!in_array($objc,$this->attrs['objectClass'])){
        $this->attrs['objectClass'][]=$objc;
      }
    }


    /* Save usersettings to Printer */
    $skip_printer_changes = false;
    if(chkacl($this->acl,"gotoPrinter")!=""){
      $this->gotoPrinter    = array();
      $skip_printer_changes = true;
    }  
    
    if(!$skip_printer_changes){
      /* 1. Search all printers that have our uid/cn as member
       * 2. Delete this uid/cn from every single entry and save it again.
       * 2.1 There are different types of members: Users / Groups, this will be defined in $suffix
       * 2.2 And each type has two modes, Admin (e.g. 'gotoUserAdminPrinter') and Normal
       */
      $types = array( "gotoUserPrinter"       => "AddUser",
          "gotoGroupPrinter"      => "AddGroup",
          "gotoUserAdminPrinter"  => "AddAdminUser",
          "gotoGroupAdminPrinter" => "AddAdminGroup");

      /* Detect type of edited object, sometimes this wasn't set correctly ... */
      $this->detect_grouptype();

      if($this->is_group){
        $s_suffix = "Group";
        $useVar   = "cn";
      }else{
        $useVar   = "uid";
        $s_suffix = "User";
      }

      /* Remove old entries */
      $ldap->search("(&(objectClass=gotoPrinter)(goto".$s_suffix."Printer=".$this->uid."))",array("*"));
      while($attr = $ldap->fetch()){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$attr['dn']);
        $printerObj->by_object['printgeneric']->DelMember($types["goto".$s_suffix."Printer"],$this->uid);
        $printerObj->by_object['printgeneric']->save();
      }

      $ldap->search("(&(objectClass=gotoPrinter)(goto".$s_suffix."AdminPrinter=".$this->uid."))",array("*"));
      while($attr = $ldap->fetch()){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$attr['dn']);
        $printerObj->by_object['printgeneric']->DelMember($types["goto".$s_suffix."AdminPrinter"],$this->uid);
        $printerObj->by_object['printgeneric']->save();
      }

      foreach($this->gotoPrinter as $printer){
        $printerObj = NULL;
        $printerObj = new printtabs($this->config,$this->config->data['TABS']['PRINTTABS'],$printer['dn']);

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
    $this->attrs['gotoHotplugDevice'] = array();
    foreach($this->gotoHotplugDevices as $name => $device){
      $tmp= $device['name']."|".$device['description']."|".$device['id'];
      if (isset($device['produkt']) && isset($device['vendor'])){
        $tmp.= "|".$device['produkt']."|".$device['vendor'];
      }
      $this->attrs['gotoHotplugDevice'][]= $tmp;
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
    $this->attrs['gotoProfileFlags'] = array($this->gotoProfileFlag_C.$this->gotoProfileFlag_L);
    if(empty($this->attrs['gotoProfileFlags'][0])){
      $this->attrs['gotoProfileFlags']=array();
      }

    if($this->useProfile == false){
      $this->attrs['gotoProfileFlags'] = preg_replace("/C/i","",$this->attrs['gotoProfileFlags']);
      $this->attrs['gotoProfileServer']= array(); 
    }

    foreach($this->attributes as $s_attr){
      if(chkacl($this->acl,$s_attr)!="") {
        if(isset($this->attrs[$s_attr])){
          unset($this->attrs[$s_attr]);
        }
      }
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
    show_ldap_error($ldap->get_error(), _("Adding environment information failed"));
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
    return($a_return);
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
  
  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
      $this->uid = $_POST['cn'];
    }
  }
}




// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
