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
  var $in_dialog            = false;    // Indicates that we are currently editing in an seperate dialog

  /* Attribute definition
   */

  /* profile managment */
  var $useProfile         = false;  // Specifies if we want to use a Server 
  var $goProfileServer    = "";     // Specifies the selected profile server
  var $goProfileServers   = array();// Specifies all available and selectable servers
  var $gotoProfileFlags   = "";     // Flags enabled  ? only used to set ACL and save 
  var $gotoProfileFlag_C  = "";     // Flag is set to C if we have the profile caching fucntion enabled 
  
  var $gotoXResolution    = "";     // The selected resolution eg: 1024x768
  var $gotoXResolutions   = array();// Contains all available resolutions for this account
  var $gotoProfileFlag_L  = "";     // Flag is set to L to enable runtime resolution change 

  /* Login script section*/
  var $gotoLoginScripts   = array();// Contains all available Login Scripts  
  var $gotoLoginScript    = "";     // The selected Login Script

  /* Printer */
  var $gotoPrinters       = array();// All available Printer
  var $gotoPrinter        = "";     //  The selected Printer

  /* Share */
  var $gotoShares         = array();// Currently Share Option
  var $gotoShare          = "";     // currently selected Share Option
  var $gotoShareSelections= array();// Available Shares for this account
  
  /* Kiosk profile */
  var $gotoKioskProfile   = "";     // The selected Kiosk Profile
  var $gotoKioskProfiles  = array();// All available Kiosk profiles

  /* Hotplug Devices */
  var $gotoHotplugDevice  = "";     // Selected hotplug
  var $gotoHotplugDevices = array();// Already configured hotplug devices 


  /* general settings */
  // Sets the attributes which will kept on page reload, which will be saved, ...
  var $attributes           = array("useProfile","goProfileServer","goProfileServers","gotoProfileFlags","gotoProfileFlag_C",
                                    "gotoXResolution","gotoXResolutions","gotoProfileFlag_L",
                                    "gotoLoginScripts","gotoLoginScript",
                                    "gotoPrinters","gotoPrinter",
                                    "gotoShares","gotoShare","gotoShareSelections",
                                    "gotoKioskProfile","gotoKioskProfiles",
                                    "gotoHotplugDevice","gotoHotplugDevices");
  var $objectclasses        = array("whatever"); // Specifies the objectClass which contains the attributes edited here 






  function environment ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Is accout enabled | are we editing from usermenu or admin menu 
       All these tab managment is done here
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
 
    /* Als smarty vars are set. Get smarty template and generate output */
    $display.= $smarty->fetch(get_template_path('environment.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function remove_from_parent()
  {
    /* This cannot be removed... */
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
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

    /* Optionally execute a command after we're done */
    #$this->handle_post_events($mode);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
