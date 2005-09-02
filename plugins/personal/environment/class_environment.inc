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
  
  /* Attribute definition
   */

  /* profile management */
  var $useProfile         = false;  // Specifies if we want to use a Server 
  var $gotoProfileServer  = "";     // Specifies the selected profile server
  var $gotoProfileServers = array();// Specifies all available and selectable servers
  var $gotoProfileFlags   = "";     // Flags enabled  ? only used to set ACL and save 
  var $gotoProfileFlag_C  = "";     // Flag is set to C if we have the profile caching fucntion enabled 
  
  var $gotoXResolution    = "1024x768";     // The selected resolution eg: 1024x768
  var $gotoXResolutions   = array();// Contains all available resolutions for this account
  var $gotoProfileFlag_L  = "";     // Flag is set to L to enable runtime resolution change 
  var $gotoProfileQuota   = "";     // User Quota Settings

  /* Logon script section*/
  var $gotoLogonScripts   = array();// Contains all available Logon Scripts  
  var $gotoLogonScript    = "";     // The selected Logon Script

  /* Printer */
  var $gotoPrinters       = array();// All available Printer
  var $gotoPrinter        = "";     //  The selected Printer

  /* Share */
  var $gotoShares         = array();// Currently Share Option
  var $gotoShare          = "";     // currently selected Share Option
  var $gotoShareSelections= array();// Available Shares for this account in Listbox format
  var $gotoAvailableShares= array();// Available Shares for this account
  
  /* Kiosk profile */
  var $gotoKioskProfile   = "";     // The selected Kiosk Profile
  var $gotoKioskProfiles  = array();// All available Kiosk profiles
  VAR $newKioskProfiles   = array();

  /* Hotplug Devices */
  var $gotoHotplugDevice  = "";     // Selected hotplug
  var $gotoHotplugDevices = array();// Already configured hotplug devices 


  /* general settings */
  // Sets the attributes which will kept on page reload, which will be saved, ...
  var $attributes         = array("uid","useProfile","gotoProfileServer","gotoProfileServers","gotoProfileFlags","gotoProfileFlag_C",
                                    "gotoXResolution","gotoXResolutions","gotoProfileFlag_L","gotoProfileQuota",
                                    "gotoLogonScripts","gotoLogonScript",
                                    "gotoPrinters","gotoPrinter",
                                    "gotoShares","gotoShare","gotoShareSelections",
                                    "gotoKioskProfile","gotoKioskProfiles",
                                    "gotoHotplugDevice","gotoHotplugDevices");
  var $objectclasses      = array("gotoEnvironment"); // Specifies the objectClass which contains the attributes edited here 
  var $cn;

  function environment ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

   

    if((isset($this->attrs['cn'][0]))&&(!isset($this->attrs['uid'][0]))){
      $suffix="Group";
      $this->uid          = $this->attrs['cn'][0];
      $this->attrs['uid'] = $this->attrs['cn'][0];
    }else{
      $suffix="User";
    }

    /* Get all Printer assignments */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoPrinter)(goto".$suffix."Printer=".$this->uid."))",array("*"));
    while($printer = $ldap->fetch()){
      $this->gotoPrinters[$printer['cn'][0]]=$printer;
      $this->gotoPrinters[$printer['cn'][0]]['mode']="user";
    }
    $ldap->search("(&(objectClass=gotoPrinter)(goto".$suffix."AdminPrinter=".$this->uid."))",array("*"));
    while($printer = $ldap->fetch()){
      $this->gotoPrinters[$printer['cn'][0]]=$printer;
      $this->gotoPrinters[$printer['cn'][0]]['mode']="admin";
    }
   
    /* prepare hotplugs */
    if((isset($this->attrs['gotoHotplugDevice']))&&(is_array($this->attrs['gotoHotplugDevice']))){
      unset($this->attrs['gotoHotplugDevice']['count']);
      foreach($this->attrs['gotoHotplugDevice'] as $device){
        $tmp = $tmp2 = array();
        $tmp = split("\|",$device);
        $tmp2['name']        = $tmp[0]; 
        $tmp2['description'] = $tmp[1]; 
        $tmp2['id']          = base64_decode($tmp[2]); 
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
        $tmp2['name']      =$tmp[0];
        $tmp2['mountPoint']=base64_decode($tmp[1]);
        $this->gotoShares[$tmp[0]]=$tmp2;
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

    /* Set to group environment if we editing a group */
    if(!isset($this->parent)){
      $this->is_group = true;
    }


  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";
    /* Is accout enabled | are we editing from usermenu or admin menu 
       All these tab management is done here
    */

    // 1. Account disabled . Editing from usermenu
    if((!isset($this->parent))&&(!$this->is_account)){
      print "asdf";
      /* We are currently editing this tab from usermenu, but this account is not enabled */
      $smarty->assign("is_account",$this->is_account);
      /* Load template */
      $display .= $smarty->fetch(get_template_path('environment.tpl', TRUE));
      /* Avoid the "You are currently editing ...." message when you leave this tab */
      $display .= back_to_main(); 
      /* Display our message to the user */
      return $display;

    // Account is enabled
    }elseif((isset($this->parent))&&(!isset($this->parent->by_object['posixAccount']))){
        $smarty->assign("is_account","true");
        $this->is_Account = true;
        $this->is_group = true;
        $this->uid          = $this->cn;
        $this->attrs['uid'] = $this->cn;

        if (isset($_POST['modify_state'])){
          $this->is_account= !$this->is_account;
        }

        // 3. Account enabled . Editing from adminmenu
        if ($this->is_account){
          $display= $this->show_header(_("Remove environment extension"),
              _("This server has environment extension enabled. You can disable it by clicking below."));
        } else {
          if((in_array("posixAccount",$this->attrs['objectClass']))){
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("This server has environment extension disabled. You can enable it by clicking below."));
            return $display;
          }else{
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("This server has environment extension disabled. You have to setup a posix account before you can enable this feature."));
            return $display;
          }
        }



    }else{
      /* Tell smarty that this accoutn is enabled */
      $smarty->assign("is_account","true");

      /* Do we need to flip is_account state? */
      if (isset($_POST['modify_state'])){
        $this->is_account= !$this->is_account;
      }
      if(!isset($this->parent)){
          // 3. ? Account Enabled . Editing from usermenu        

      }else{
        // 3. Account enabled . Editing from adminmenu
        if ($this->is_account){
          $display= $this->show_header(_("Remove environment extension"),
              _("This server has environment extension enabled. You can disable it by clicking below."));
        } else {
          if((isset($this->attrs['objectClass']))&&((in_array("posixAccount",$this->attrs['objectClass'])))||($this->parent->by_object['posixAccount']->is_account==true)){
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("This server has environment extension disabled. You can enable it by clicking below."));
            return $display;
          }else{
            // 4. There is a PosixAccount
            $display= $this->show_header(_("Add environment extension"),
                _("This server has environment extension disabled. You have to setup a posix account before you can enable this feature."),TRUE,TRUE);
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
 
    if($this->acl != "#none#"){
      $smarty->assign("useProfileACL","");
    }else{
      $smarty->assign("gotoProfileFlag_CACL"," disabled ");
      $smarty->assign("gotoProfileServer"," disabled ");
    }

    /* HANDLE Profile Settings here 
     * Assign available Quota and resolution settings
     * Get all available profile server
     * Get cache checkbox
     * Assign this all to Smarty 
     */

    if(empty($this->useProfile)){
      $smarty->assign("gotoProfileACL","disabled"); 
      $smarty->assign("useProfileCHK",""); 
    }else{
      $smarty->assign("gotoProfileACL",""); 
      $smarty->assign("useProfileCHK"," checked "); 
    }

    $this->gotoXResolutions = array("640x480","800x600","1024x768","1280x768","1280x1024");
    $this->gotoProfileServers =$this->getShareServerList() ;

    $smarty->assign("gotoXResolutions",$this->gotoXResolutions);
    $smarty->assign("gotoProfileServers",$this->gotoProfileServers);
    $smarty->assign("gotoProfileServerKeys",array_flip($this->gotoProfileServers));

    /* Handle kiosk profiles 
     * Read available from filesystem
     * Open management if post is transmitted
     */

    /* Open Management Dialog */
    if(isset($_POST['KioskManagementDialog'])){
      $this->dialog = new kioskManagementDialog($this->config,$this->dn,$this->newKioskProfiles); 
      $this->dialog->parent= $this;
      $this->is_dialog = true;
    }

    /* Save */
    if(isset($_POST['KioskClose'])){
      $this->newKioskProfiles = array_merge($this->newKioskProfiles,$this->dialog->save());
  
      unset($this->dialog);
      $this->dialog=NULL;
      $this->is_dialog = false;
    }
    $tmp = new kioskManagementDialog($this->config,$this->dn);
    $list = $tmp->getKioskProfiles($this->newKioskProfiles);

    $smarty->assign("gotoKioskProfiles",$list);
    $smarty->assign("gotoKioskProfileKeys",array_flip($list));

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
    if(isset($_POST['gotoLogonScriptNew'])||isset($_POST['gotoLogonScriptEdit'])||isset($_POST['gotoLogonScriptDel'])){

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
    
    $this->gotoShareSelections    = $this->getShareList(true);
    $this->gotoAvailableShares    = $this->getShareList(false);  

    $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
    $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));

    /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry 
     * This entry will be, a combination of mountPoint and sharedefinitions 
     */
    if(isset($_POST['gotoShareAdd'])){
      /* We assign a share to this user, if we don't know where to mount the share */
      if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
        print_red("You must specify a valid mount point.");
      }else{
        $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
        $s_mount = $_POST['gotoShareMountPoint'];

        /* Preparing the new assignment */ 
        $this->gotoShares[$a_share['name']]=$a_share;
        $this->gotoShares[$a_share['name']]['mountPoint']=$s_mount;
      }
    }  

    /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
     * If there is no defined share selected, we will abort the deletion without any message 
     */
    if((isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
      unset($this->gotoShares[$_POST['gotoShare']]);
    }

    $smarty->assign("gotoShares",$this->printOutAssignedShares());
    $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));

    /* Hotplug devices will be handled here 
     * There are 3 possible methods for this feature
     * Create a new Hotplug, A Dialog will open where you can specify some hotplug information
     * Delete will erase an entry, the entry must be selcted in the ListBox first
     * Editing an entry will open a dialog where the informations about the selcted entry can be changed
     */

    /* If there is a new entry wanted, open a new entry by initilising the dialog */
    if(isset($_POST['gotoHotplugDeviceNew'])){
      $this->dialog = new hotplugDialog($this->config,$this->dn);
      $this->is_dialog = true;
    }

    /* We have to delete the selected hotplug from the list*/
    if((isset($_POST['gotoHotplugDeviceDel']))&&(isset($_POST['gotoHotplugDevice']))){
      unset($this->gotoHotplugDevices[$_POST['gotoHotplugDevice']]);
    }

    /* There are already defined hotplugs from other users we could use */
    if(isset($_POST['gotoHotplugDeviceUse'])){
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
          $this->gotoHotplugDevices[$a_tmp['name']]= $a_tmp; 
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
    if(isset($_POST['gotoPrinterAdd'])){
      $this->is_dialog=true;
      $this->dialog = new selectPrinterDialog($this->config,$this->dn);
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
        $this->gotoPrinters[$tmp]=$tmp2[$tmp];
        $this->gotoPrinters[$tmp]['mode']="user";
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog   =NULL;
      }
    }
  
    if((isset($_POST['gotoPrinterDel']))&&(isset($_POST['gotoPrinter']))&&(!empty($_POST['gotoPrinter']))){
      $printer = $_POST['gotoPrinter'];
      unset($this->gotoPrinters[$printer]);
    }

    if((isset($_POST['gotoPrinterEdit']))&&(isset($_POST['gotoPrinter']))&&(!empty($_POST['gotoPrinter']))){
      $printer = $_POST['gotoPrinter'];
      if($this->gotoPrinters[$printer]['mode']=="user"){
        $this->gotoPrinters[$printer]['mode']="admin";
      }else{
        $this->gotoPrinters[$printer]['mode']="user";
      }
    }

    $smarty->assign("gotoPrinters",$this->printOutPrinterDevices());
    $smarty->assign("gotoPrinterKeys",array_flip($this->printOutPrinterDevices()));
 
    /* General behavior */
    if((isset($this->dialog))&&($this->dialog!=NULL)&&(!empty($this->dialog))){
      $this->dialog->save_object();
      return ($this->dialog->execute());
    }
    if($this->acl != "#none#"){
      $smarty->assign("useProfileACL","");
    }else{
      $smarty->assign("gotoProfileFlag_CACL"," disabled ");
      $smarty->assign("gotoProfileServerACL"," disabled ");
    }

    if(!$this->useProfile){
      $smarty->assign("gotoProfileFlag_CACL"," disabled ");
      $smarty->assign("gotoProfileServerACL"," disabled ");
    }

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
    $skip = array("useProfile","uid","gotoProfileServers","gotoProfileFlag_C","gotoXResolutions","gotoProfileFlag_L",
                  "gotoLogonScripts","gotoPrinters","gotoShares","gotoShareSelections","gotoKioskProfiles","gotoHotplugDevices",
                  "gotoPrinter");
    /* Skip all these attributes */
    foreach($skip as $del){
      unset($this->attrs[$del]);
    }

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");

    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);

    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    /* Get all Posted vars 
     * Setup checkboxes 
     */
    
    if(isset($_POST['iamposted'])){
      plugin::save_object();
      foreach($this->attributes as $s_attr){
        if(in_array($s_attr,array("gotoShares","gotoHotplugDevices","gotoPrinters","gotoLogonScripts","uid"))) continue;
        if(isset($_POST[$s_attr])){
          $this->$s_attr = $_POST[$s_attr];
        }else{
          $this->$s_attr = false;
        }
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
  
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
    plugin::save();
    $ldap= $this->config->get_ldap_link();

    $realyUsedAttrs= array();


    $path = search_config($this->config->data['MENU'],"environment", "KIOSKPATH"); 
    /* Creating Kiosk Profiles */
    foreach($this->newKioskProfiles as $file){
      $contents = $file['contents'];
      $fp = @fopen($path."/".$file['name'],"w");
      if(!$fp){
        print_red("Can't save new kiosk profiles, possibly permission denied for folder :",$path);
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

    if(chkacl($this->acl,"gotoPrinter")!=""){
      $this->gotoPrinters = array();
    }  
    
    if($this->is_group){
      $s_suffix = "Group";
    }else{
      $s_suffix = "User";
    }
  

    if(count($this->gotoPrinters)==0){
      $ldap->search("(&(objectClass=gotoPrinter)(|(goto".$s_suffix."Printer=".$this->uid.")(goto".$s_suffix."AdminPrinter=".$this->uid.")))",array("*"));
      while($attr = $ldap->fetch()){
      
        foreach($attr['goto'.$s_suffix.'Printer'] as $key => $user){
          if($this->uid==$user){
            unset($attr['goto'.$s_suffix.'Printer'][$key]);
          }
        }    

        foreach($attr['goto'.$s_suffix.'AdminPrinter'] as $key => $user){
          if($this->uid==$user){
            unset($attr['goto'.$s_suffix.'AdminPrinter'][$key]);
          }
        }    

        $attrs_used = array();
        foreach($attr as $key=>$val){
          if((!is_numeric($key))&&($key!="count")){
            if(is_array($val)&&isset($val['count'])){
              unset($val['count']);
            }
            $attrs_used[$key]=$val;
          }
        }
        $attr= $attrs_used;
        $tmp =array();
        foreach($attr['goto'.$s_suffix.'AdminPrinter'] as $print){
          $tmp[]=$print;
        }
        $attr['goto'.$s_suffix.'AdminPrinter'] = $tmp;

        $tmp =array();
        foreach($attr['goto'.$s_suffix.'Printer'] as $print){
          $tmp[]=$print;
        }
        $attr['goto'.$s_suffix.'Printer'] = $tmp;

        $ldap->cd($attr['dn']);
        unset($attr['dn']);
        $ldap->modify($attr);
        if($ldap->get_error()!="Success"){
          print_red("Printer saving ".$ldap->get_error());
        }
      }
    }    

    foreach($this->gotoPrinters as $printer) {
      $ldap->cd($printer['dn']);
      $ldap->cat($printer['dn']);
      $attrs= $ldap->fetch();
      $attrs_used = array(); 
      foreach($attrs as $key=>$val){
        if((!is_numeric($key))&&($key!="count")){
          if(is_array($val)&&isset($val['count'])){
            unset($val['count']);
          }
          $attrs_used[$key]=$val;
        }
      }
      $attrs= $attrs_used;

      /* Filter entries */
 
      if($printer['mode'] == "user"){
        $attribute  = "goto".$s_suffix."Printer";
        $attribute2 = "goto".$s_suffix."AdminPrinter";
      }else{
        $attribute  = "goto".$s_suffix."AdminPrinter";
        $attribute2 = "goto".$s_suffix."UserPrinter";
      }

      /* If this user is already assigned to $attribute2 
       * delete user from $attribute2, to be albe to attach him to $attribute
       * A user can't be admin and normal user for one printer
       */
      if(!isset($printer[$attribute2])){
        $printer[$attribute2]=array();
      }else{
        if(in_array($this->uid,$printer[$attribute2])){ 
          $tmp = array_flip($attrs[$attribute2]);
          unset($tmp[$this->uid]);
          $attrs[$attribute2]=array_flip($tmp);
        }
        /* If Last entry removed, clear attribute*/
        if(empty($attrs[$attribute2])){
          $attrs[$attribute2]=array();
        }
      }
    
      /* Attach user to the $attribute, if he is'nt already attached
       */
      if(!isset($attrs[$attribute])){
        $attrs[$attribute]=array($this->uid);
      }else{
        unset($attrs[$attribute]['count']);
        if(!in_array($this->uid,$attrs[$attribute])){
          $attrs[$attribute][]=$this->uid;
        }
      }

      $tmp =array();
      if(isset($attrs['goto'.$s_suffix.'AdminPrinter'])){
        foreach($attrs['goto'.$s_suffix.'AdminPrinter'] as $print){
          $tmp[]=$print;
        }
      }
      $attrs['goto'.$s_suffix.'AdminPrinter'] = $tmp;

      $tmp =array();
      if(isset($attrs['goto'.$s_suffix.'Printer'])){
        foreach($attrs['goto'.$s_suffix.'Printer'] as $print){
          $tmp[]=$print;
        }
      }
      $attrs['goto'.$s_suffix.'Printer'] = $tmp;

      $ldap->cd($attrs['dn']);
      unset($attrs['dn']);
      $ldap->modify($attrs);
      if($ldap->get_error()!="Success"){
        print_red("Printer saving ".$ldap->get_error());
      }
    }
  
    if(isset($this->attrs['gotoPrinter'])){
      unset($this->attrs['gotoPrinter']);
    }
 
    /* Prepare HotPlug devices */
    $this->attrs['gotoHotplugDevice'] = array();
    foreach($this->gotoHotplugDevices as $name => $device){
      $this->attrs['gotoHotplugDevice'][] = $device['name']."|".$device['description']."|".base64_encode($device['id']);
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
      $this->attrs['gotoShare'][] = $share['name']."|".base64_encode($share['mountPoint']);
    }

    if(!empty($this->gotoKioskProfile)){
      $this->attrs['gotoKioskProfile']= $this->gotoKioskProfile;
    }else{
      $this->attrs['gotoKioskProfile']= array();
    }

    $saveThis = array("gotoKioskProfile","gotoProfileQuota","gotoXResolution","gotoProfileServer");
  
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

    $ldap->cat ($this->dn);
    if ($ldap->fetch()){
      $mode= "modify";
    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    $ldap->cd($this->dn);
    $ldap->$mode($this->attrs);
    if($ldap->get_error()!="Success"){
      print_red($ldap->get_error());
    }
    $this->handle_post_events($mode);
  }

/* This function returns all available Shares defined in this ldap 
 * There are two ways to call this function, if listboxEntry is true
 *  only name and path are attached to the array, in it is false, the whole 
 *  entry will be parsed an atached to the result.
 */
  function getShareList($listboxEntry = false)
  {
    $ldap= $this->config->get_ldap_link();
    $a_res = $ldap->search("(objectClass=goShareServer)",array("goExportEntry"));
    $return= array();
    while($entry = $ldap->fetch($a_res)){
      unset($entry['goExportEntry']['count']);
      foreach($entry['goExportEntry'] as $export){
        $shareAttrs = split("\|",$export);
        if($listboxEntry) { 
          $return[$shareAttrs[0]] = $shareAttrs[0]." - ".$shareAttrs[4];
        }else{
          $return[$shareAttrs[0]]['name']         = $shareAttrs[0]; 
          $return[$shareAttrs[0]]['description']  = $shareAttrs[1]; 
          $return[$shareAttrs[0]]['type']         = $shareAttrs[2]; 
          $return[$shareAttrs[0]]['charset']      = $shareAttrs[3]; 
          $return[$shareAttrs[0]]['path']         = $shareAttrs[4]; 
          $return[$shareAttrs[0]]['option']       = $shareAttrs[5]; 
        }
      }
    }
    return($return);
  }


/* This function returns all available ShareServer  
 */
  function getShareServerList()
  {
    $ldap= $this->config->get_ldap_link();
    $a_res = $ldap->search("(objectClass=goShareServer)",array("goExportEntry","cn"));
    $return= array();
    while($entry = $ldap->fetch($a_res)){
      $return[$entry['cn'][0]] = $entry['cn'][0];
    }
    return($return);
  }

/* Generate ListBox frindly output for the defined shares 
 * Possibly Add or remove an attribute here, 
 */
  function printOutAssignedShares()
  {
    $a_return = array();
    if(is_array($this->gotoShares)){
      foreach($this->gotoShares as $share){
        $a_return[$share['name']]= $share['name']." ".$share['mountPoint'];
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
    if(is_array($this->gotoPrinters)){
      foreach($this->gotoPrinters as $printer){
        if($printer['mode'] == "admin"){
          $a_return[$printer['cn'][0]]= $printer['cn'][0]." - "._("Admin");
        }else{
          $a_return[$printer['cn'][0]]= $printer['cn'][0]; 
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
}




// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
