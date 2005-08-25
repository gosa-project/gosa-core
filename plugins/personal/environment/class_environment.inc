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

  /* Attribute definition
   */

  /* profile management */
  var $useProfile         = false;  // Specifies if we want to use a Server 
  var $gotoProfileServer  = "";     // Specifies the selected profile server
  var $gotoProfileServers = array();// Specifies all available and selectable servers
  var $gotoProfileFlags   = "";     // Flags enabled  ? only used to set ACL and save 
  var $gotoProfileFlag_C  = "";     // Flag is set to C if we have the profile caching fucntion enabled 
  
  var $gotoXResolution    = "";     // The selected resolution eg: 1024x768
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

  function environment ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Get all Printer assignments */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoPrinter)(gotoUserPrinter=".$this->uid."))",array("*"));
    while($printer = $ldap->fetch()){
      $this->gotoPrinters[$printer['cn'][0]]=$printer;
      $this->gotoPrinters[$printer['cn'][0]]['mode']="user";
    }
    $ldap->search("(&(objectClass=gotoPrinter)(gotoAdminPrinter=".$this->uid."))",array("*"));
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
        $tmp2['id']          = $tmp[2]; 
        $this->gotoHotplugDevices[$tmp[0]]=$tmp2;
      }
    }
    
    /* Prepare Shares */
    if((isset($this->attrs['gotoShare']))&&(is_array($this->attrs['gotoShare']))){
      unset($this->attrs['gotoShare']['count']);
      foreach($this->attrs['gotoShare'] as $share){
        $tmp = $tmp2 = array();
        $tmp = split("\|",$share);
        $tmp2['name']      =$tmp[0];
        $tmp2['mountPoint']=$tmp[1];
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
      /* We are currently editing this tab from usermenu, but htis account is not enabled */
      $smarty->assign("is_account",$this->is_account);
      /* Load template */
      $display .= $smarty->fetch(get_template_path('environment.tpl', TRUE));
      /* Avoid the "You are currently editing ...." message when you leave this tab */
      $display .= back_to_main(); 
      /* Display our message to the user */
      return $display;

    // Account is enabled
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
        // 4. Account disabled . Editing from adminmenu
          $display= $this->show_header(_("Add environment extension"),
              _("This server has environment extension disabled. You can enable it by clicking below."));
          return $display;
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
    $this->gotoProfileServers = array("none","none1");

    $smarty->assign("gotoXResolutions",$this->gotoXResolutions);
    $smarty->assign("gotoProfileServers",$this->gotoProfileServers);

    /* Handle kiosk profiles 
     * Read available from filesystem
     * Open management if post is transmitted
     */
    $tmp = new kioskManagementDialog($this->config,$this->dn);
    $list = $tmp->getKioskProfiles();

    $smarty->assign("gotoKioskProfiles",$list);
    $smarty->assign("gotoKioskProfileKeys",array_flip($list));

    /* Open Management Dialog */
    if(isset($_POST['KioskManagementDialog'])){
      $this->dialog = new kioskManagementDialog($this->config,$this->dn); 
      $this->is_dialog = true;
    }

    /* Save */
    if(isset($_POST['KioskClose'])){
      unset($this->dialog);
      $this->dialog=NULL;
      $this->is_dialog = false;
    }

    /* Logonscript Management
     * Get available LogonScripts (possibly grey out (or mark) these script that are defined for the group) 
     * Perform add Delete edit Posts 
     */

    $this->gotoLogonScripts = array("asdf"=>"one","aaaa"=>"two","121"=>"3");
  
    $smarty->assign("gotoLogonScripts",$this->gotoLogonScripts);
    $smarty->assign("gotoLogonScriptKeys",array_flip($this->gotoLogonScripts));
   
    /* Check Edit Del New Posts for a selected LogonScript */ 
    if(isset($_POST['gotoLogonScriptNew'])||isset($_POST['gotoLogonScriptEdit'])||isset($_POST['gotoLogonScriptDel'])){

      /* New Logon Script: Open an edit dialog, we don't need a $_POST['gotoLogonScript'] here.
       * In this case we create a new Logon Script.
       */
      if(isset($_POST['gotoLogonScriptNew'])){
        print "Adding ".$is_entry;
      }

      /* If we receive a Delete request and there is a Script selected in the selectbox, delete this one.
       * We only can delete if there is an entry selected.
       */
      if((isset($_POST['gotoLogonScriptDel']))&&(isset($_POST['gotoLogonScript']))){
        $is_entry = $this->gotoLogonScripts[$_POST['gotoLogonScript']];
        print "Deleting ".$is_entry;
      }
      
      /* In this case we want to edit an existing entry, we open a new Dialog to allow editing.
       * There must be an entry selected to perform edit request.
       */
      if((isset($_POST['gotoLogonScriptEdit']))&&(isset($_POST['gotoLogonScript']))){
        $is_entry = $this->gotoLogonScripts[$_POST['gotoLogonScript']];
        print "Editing ".$is_entry;
      }
    }
    
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
      if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))){
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
        if(in_array($s_attr,array("gotoShares","gotoHotplugDevices","gotoPrinters"))) continue;
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
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
    $ldap= $this->config->get_ldap_link();

    $realyUsedAttrs= array();
    
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
        $attribute  = "gotoUserPrinter";
        $attribute2 = "gotoAdminPrinter";
      }else{
        $attribute  = "gotoAdminPrinter";
        $attribute2 = "gotoUserPrinter";
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
          $attrs[$attribute2]=$tmp;
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
      $ldap->cd($attrs['dn']);
      unset($attrs['dn']);
      $ldap->modify($attrs);
      if($ldap->get_error()!="Success"){
        print_red($ldap->get_error());
      }
    }
    
    /* Prepare HotPlug devices */
    $this->attrs['gotoHotplugDevice'] = array();
    foreach($this->gotoHotplugDevices as $name => $device){
      $this->attrs['gotoHotplugDevice'][] = $device['name']."|".$device['description']."|".$device['id'];
    }

    /* Prepare Shares */
    $this->attrs['gotoShare']=array();
    foreach($this->gotoShares as $name => $share){
      $this->attrs['gotoShare'][] = $share['name']."|".$share['mountPoint'];
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
      $shareAttrs = split("\|",$entry['goExportEntry'][0]);
      if($listboxEntry) { 
        $return[$shareAttrs[0]] = $shareAttrs[0]." - ".$shareAttrs[3];
      }else{
        $return[$shareAttrs[0]]['name']         = $shareAttrs[0]; 
        $return[$shareAttrs[0]]['description']  = $shareAttrs[1]; 
        $return[$shareAttrs[0]]['type']         = $shareAttrs[2]; 
        $return[$shareAttrs[0]]['charset']      = $shareAttrs[3]; 
        $return[$shareAttrs[0]]['path']         = $shareAttrs[4]; 
        $return[$shareAttrs[0]]['option']       = $shareAttrs[5]; 
      }
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





}




// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
