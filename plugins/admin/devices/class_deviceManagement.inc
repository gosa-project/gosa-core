<?php

class deviceManagement extends plugin
{

  /* Definitions */
  var $plHeadline     = "Devices";
  var $plDescription  = "Manage devices";

  /* Dialog attributes */
  var $ui                             = NULL;
  var $DivListDevices               = NULL;
  var $enableReleaseManagement        = false;
  var $devicetabs                       = NULL;
  var $snapDialog                     = NULL;
  var $CopyPasteHandler               = NULL;
  var $start_pasting_copied_objects;
  var $dn ="";

  function deviceManagement(&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    $this->ui = get_userinfo();  

    /* Check if copy & paste is activated */
    if($this->config->boolValueIsTrue("MAIN","ENABLECOPYPASTE")){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Creat dialog object */
    $this->DivListDevices = new divListDevices($this->config,$this);
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /****************
      Variable init
     ****************/

    /* These vars will be stored if you try to open a locked device,
        to be able to perform your last requests after showing a warning message */
    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^device_edit_/",
                                          "/^device_del_/","/^item_selected/","/^remove_multiple_devices/"));

    $smarty       = get_smarty();             // Smarty instance
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton

    /* Test Posts */
    foreach($_POST as $key => $val){

      if(preg_match("/device_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/device_".$s_action."_/i","",$key);
      }elseif(preg_match("/device_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/device_".$s_action."_/i","",$key);
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }elseif(preg_match("/^device_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^remove_multiple_devices/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^multiple_copy_devices/",$key)){
        $s_action = "copy_multiple";
     }elseif(preg_match("/^multiple_cut_devices/",$key)){
        $s_action = "cut_multiple";
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
      $s_action = "copy_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
      $s_action = "cut_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^editPaste/",$_POST['menu_action'])){
      $s_action = "editPaste";
    }

    /* Create options */
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "device_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }

    /****************
      Copy & Paste handling
     ****************/

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }

    /****************
      Create a new device type
     ****************/

    /* New device type? */
    $ui = get_userinfo();
    $acl = $ui->get_permissions($this->DivListDevices->selectedBase,"devices/deviceGeneric");
    if (($s_action=="new") && preg_match("/c/",$acl)){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->devicetabs= new devicetabs($this->config, $this->config->data['TABS']['DEVICETABS'], $this->dn,"devices");
      $this->devicetabs->set_acl_base($this->DivListDevices->selectedBase);
    }


    /****************
      Edit entry canceled
     ****************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) && is_object($this->devicetabs)){
      del_lock ($this->devicetabs->dn);
      unset ($this->devicetabs);
      $this->devicetabs= NULL;
      session::un_set('objectinfo');
    }


    /****************
      Edit entry finished
     ****************/

    /* Finish device edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply']) ) && (isset($this->devicetabs->config))){

      /* Check tabs, will feed message array */
      $this->devicetabs->save_object();
      $message= $this->devicetabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
#        $this->devicetabs->set_release($this->DivListDevices->selectedRelease);
        $this->devicetabs->save();

        if (!isset($_POST['edit_apply'])){
          /* device type has been saved successfully, remove lock from LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }
          unset ($this->devicetabs);
          $this->devicetabs= NULL;
          session::un_set('objectinfo');
        }
      } else {
       /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /****************
      Edit entry
     ****************/

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->devicetabs->config))){

      /* Get 'dn' from posted 'devicelist', must be unique */
      $this->dn= $this->devices[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);


      /* Register devicetabs to trigger edit dialog */
      $this->devicetabs= new devicetabs($this->config,$this->config->data['TABS']['DEVICETABS'], $this->dn,"devices");
      $this->devicetabs->set_acl_base($this->dn);
      session::set('objectinfo',$this->dn);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/
    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->devices[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn;
        }

        $dns_names = "<br><pre>";
        foreach($this->dns as $dn){
          add_lock ($dn, $this->ui->dn);
          $dns_names .= $dn."\n";
        }
        $dns_names .="</pre>";

        /* Lock the current entry, so nobody will edit it during deletion */
        if (count($this->dns) == 1){
          $smarty->assign("intro",     sprintf(_("You're about to delete the following entry %s"), @LDAP::fix($dns_names)));
        } else {
          $smarty->assign("intro",     sprintf(_("You're about to delete the following entries %s"), @LDAP::fix($dns_names)));
        }
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_device_confirm'])){

      $ui = get_userinfo();

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        $acl = $ui->get_permissions($dn,"devices/deviceGeneric");
        if(preg_match("/d/",$acl)){

          /* Delete request is permitted, perform LDAP action */
          $this->devicetabs= new devicetabs($this->config, $this->config->data['TABS']['DEVICETABS'], $dn,"devices");
          $this->devicetabs->set_acl_base($dn);
          $this->devicetabs->delete ();
          unset ($this->devicetabs);
          $this->devicetabs= NULL;

        } else {
          /* Normally this shouldn't be reached, send some extra
             logs to notify the administrator */
          print_red (_("You are not allowed to delete this device type!"));
          new log("security","devices/".get_class($this),$dn,array(),"Tried to trick deletion.");
        }
        /* Remove lock file after successfull deletion */
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
    if(isset($_POST['delete_multiple_device_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /****************
      Delete device type
     ****************/

    /* Remove user was requested */
    if ($s_action == "del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->devices[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $ui = get_userinfo();
      $acl = $ui->get_permissions($this->dn,"devices/deviceGeneric");
      if (preg_match("/d/",$acl)){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the device '%s'."), @LDAP::fix($this->dn)));
        $smarty->assign("multiple", false);
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
       print_red (_("You are not allowed to delete this device!"));
      }
    }


    /****************
      Delete device confirmed
     ****************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_device_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      $ui = get_userinfo();
      $acl = $ui->get_permissions($this->dn,"devices/deviceGeneric");
      if(preg_match("/d/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->devicetabs= new devicetabs($this->config, $this->config->data['TABS']['DEVICETABS'], $this->dn,"devices");
        $this->devicetabs->set_acl_base($this->dn);
        $this->devicetabs->delete ();
        unset ($this->devicetabs);
        $this->devicetabs= NULL;

      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this device!"));
        new log("security","devices/".get_class($this),$dn,array(),"Tried to trick deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /****************
      Delete device canceled
     ****************/

    /* Delete device canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      session::un_set('objectinfo');
  }

    /* Show tab dialog if object is present */
    if (($this->devicetabs) && (isset($this->devicetabs->config))){
      $display= $this->devicetabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->devicetabs->by_object[$this->devicetabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
        $display.= "&nbsp;\n";
        if ($this->dn != "new"){
          $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /****************
      Dialog display
     ****************/

        /* Check if there is a snapshot dialog open */
    $base = $this->DivListDevices->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Display dialog with system list */
    $this->DivListDevices->parent = $this;
    $this->DivListDevices->execute();
    $this->DivListDevices->AddDepartments($this->DivListDevices->selectedBase,3,1);
    $this->reload();
    $this->DivListDevices->setEntries($this->devices);
    return($this->DivListDevices->Draw());

  }

  function save_object() {
    $this->DivListDevices->save_object();
  }


  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_ou('deviceou').$this->DivListDevices->selectedBase));
  }

  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn = $this->devices[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"devicetabs","DEVICETABS","devices");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->devices[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","devicetabs","DEVICETABS","devices");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","devicetabs","DEVICETABS","devices");
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }

    /* Return C&P dialog */
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){

      /* Load entry from queue and set base */
      $this->CopyPasteHandler->load_entry_from_queue();
      $this->CopyPasteHandler->SetVar("base",$this->DivListDevices->selectedBase);

      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
  }



  function reload()
  {
    /* Set base for all searches */
    $base       = $this->DivListDevices->selectedBase;
    $Regex      = $this->DivListDevices->Regex;
    $SubSearch  = $this->DivListDevices->SubSearch;
    $Flags      =  GL_NONE | GL_SIZELIMIT;
    $Filter     = "(&(|(cn=".$Regex.")(description=".$Regex."))(objectClass=gotoDevice))";
    $tmp        = array();

    /* In case of subsearch, add the subsearch flag */
    if($SubSearch){
      $Flags    |= GL_SUBSEARCH;
    }else{
      $base = get_ou('deviceou').$base;
    }

    /* Get results and create index */
    $res= get_sub_list($Filter,"devices",get_ou('deviceou'), $base, array("cn","description","dn","objectClass"), $Flags);
    $tmp2 = array();
    foreach ($res as $val){
      $tmp2[strtolower($val['cn'][0]).$val['cn'][0].$val['dn']] = strtolower($val['cn'][0]).$val['cn'][0].$val['dn'];
      $tmp[strtolower($val['cn'][0]).$val['cn'][0].$val['dn']]=$val;
    }

    /* sort entries */
    natcasesort($tmp2);
    $this->devices=array();
    foreach($tmp2 as $val){
      $this->devices[]=$tmp[$val];
    }
    reset ($this->devices);
  }


  function remove_lock()
  {
    if (isset($this->devicetabs->dn)){
      del_lock ($this->devicetabs->dn);
    }
  }

  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  function remove_from_parent()
  {
    /* This cannot be removed... */
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
