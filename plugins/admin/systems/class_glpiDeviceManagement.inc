<?php

class glpiDeviceManagement extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes= array();
  var $objectclasses= array("whatever");

  var $devices = array();
  var $ui;

  var $parent   ;
  var $EditEntry;
  var $editMode =false;

  var $DeviceAttrs = array();
  var $AllowedDevices = array();

  var $Selected = array();

  var $delete = array();

  function glpiDeviceManagement ($config, $dn= NULL,$used=NULL)
  {
    plugin::plugin ($config, $dn);

    if(!isset($_SESSION['glpiDeviceRegex'])){
      $tmp['device_regex'] = "*";
      $_SESSION['glpiDeviceRegex'] = $tmp;
    }
    $this->ui = get_userinfo();  

    if(isset($used)){
    $this->Selected = $used;
    }
 
    /* Specify which vars are allowed for a sepcific type of device */ 
    $this->DeviceAttrs['case']      = array("designation","format","comment","FK_glpi_enterprise");
    $this->DeviceAttrs['moboard']   = array("designation","chipset","comment","FK_glpi_enterprise");
    $this->DeviceAttrs['processor'] = array("designation","frequence","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['iface']     = array("designation","bandwidth","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['ram']       = array("designation","frequence","comment","FK_glpi_enterprise","specif_default","type");
    $this->DeviceAttrs['hdd']       = array("designation","rpm","interface","cache","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['drive']     = array("designation","speed","interface","is_writer","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['control']   = array("designation","interface","raid","comment","FK_glpi_enterprise");
    $this->DeviceAttrs['gfxcard']   = array("designation","ram","interface","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['sndcard']   = array("designation","type","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['power']     = array("designation","power","comment","FK_glpi_enterprise","atx");
    $this->DeviceAttrs['pci']       = array("designation","comment","FK_glpi_enterprise","specif_default");
    $this->DeviceAttrs['monitor']   = array("name","comments","serial","otherserial","size",
                                            "flags_micro","flags_speaker","flags_subd","flags_bnc",
                                            "location","type","FK_glpi_enterprise","is_global","deleted","is_template","tplname");

    $this->AllowedDevices=array("case","moboard","sndcard","processor","iface","ram","hdd","drive","control","gfxcard","power","pci","monitor");
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Get all defined devices */
    $this->reload();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* this var is used to ensure that every post is only performed once */
    $only_once = true;

    /* Check Post for some actions */
    foreach($_POST as $name => $value){
      
      /* If new_ then we should create an new device */
      if((preg_match("/^new_/",$name))&&($only_once)){
    
        /* don't do this twice */
        $only_once = false;

        /*extract device device_type */
        $deviceType = preg_replace("/_.*$/","",preg_replace("/^new_/","",$name));

        /* Check if type is allowed, and create empty entry */
        $tmp = array();
        if((!isset($this->DeviceAttrs[$deviceType]))||((!in_array($deviceType,$this->AllowedDevices)))){
          print_red(sprintf(_("Internal Error can't create device of type '%s'"),$deviceType));
        }else{
          foreach($this->DeviceAttrs[$deviceType] as $attr){
            $tmp[$attr] = "";
          }
          $tmp['device_type'] = $deviceType;
          $this->EditEntry = $tmp;
          $this->editMode =true;
        }
      }
    }

    /* delete was requested ... show dialog   */
    if((isset($_GET['act']))&&($_GET['act']=="del_device")){
      $id = base64_decode($_GET['id']);
      $tmp = $this->devices[$id];

      $this->delete = $tmp;

      if(isset($tmp['name'])){
        $smarty->assign("warning", sprintf(_("You're about to delete the glpi device '%s'."), $tmp['name']));
      }elseif(isset($tmp['designation'])){
        $smarty->assign("warning", sprintf(_("You're about to delete the glpi device '%s'."), $tmp['designation']));
      }else{
        print_red(_("Can't detect object name."));
      }
      
      return($smarty->fetch(get_template_path('remove_glpi.tpl', TRUE)));
    }

    /* Delete entry, but check if this device is in currently in use */
    if(isset($_POST['delete_glpi_confirm'])){
      if(count($this->parent->handle->is_deviceUsed($this->delete))){

        $tmp = $this->parent->handle->is_deviceUsed($this->delete);    
  
        $names = "";
        foreach($tmp as $name){
          $names .= ", ".$name;
        }
        $names = preg_replace("/^, /","",$names);
        $names = trim($names);
        if(count($tmp) == 3){
          $names .= " ...";
        }
        print_red(sprintf(_("You can't delete this device, it is still in use by these system(s) '%s'"),$names));
      }else{
        $this->parent->handle->deleteDevice($this->delete);
        $this->reload();
      }
    }

    /* Open entry for editing if requested */
    if((isset($_GET['act']))&&($_GET['act']=="edit_device")){
      $id = base64_decode($_GET['id']);
      $this->editMode =true;
      $this->EditEntry = $this->devices[$id];
    }

    /* Abort editing this entry */
    if(isset($_POST['AbortDeviceChanges'])){
      $this->EditEntry = array();
      $this->editMode = false;
    }

    /* Save all changes made on currently selected entry */
    if(isset($_POST['SaveDeviceChanges'])){
    
      /* First check if all changes made are allowed */
      if(count($this->check($this->EditEntry))==0){
        $this->save($this->EditEntry);
        $this->editMode = false;
        $this->reload();
      }else{
        foreach($this->check($this->EditEntry) as $msg){
          print_red($msg);
        }
      }
      
    }

    /* Check if we are currently editing something ? */
    if($this->editMode == true){
      return ($this->editDevice($this->EditEntry));
    }

    /*  ENDE :  GET / POST handling 
     *  Below, only output generation for headpage
     */

    $divlist = new divlist("glpi devices");
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("devices"), "attach" => "style=''"),
          array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));

    $edit = "<a href='?plug=".$_GET['plug']."&amp;act=edit_device&amp;id=%s'>%s</a>";
    $editdel = "<a href='?plug=".$_GET['plug']."&amp;act=edit_device&amp;id=%s'><img src='images/edit.png' alt='"._("Edit")."' border=0></a>";
    $editdel.= "<a href='?plug=".$_GET['plug']."&amp;act=del_device&amp;id=%s'><img src='images/edittrash.png' alt='"._("Delete")."' border=0></a>";
    
    $useDevice = "<input type='hidden' name='wasOnPage_%s' value='%s'><input type='checkbox' value='%s' name='useDevice_%s' %USE%>";

    foreach($this->devices as $key=>$user){

      if(isset($user['designation'])){
        $str = "designation";
      }else{
        $str = "name";
      }

      if(isset($this->Selected[$user['device_type']][$user[$str]])){
        $use = " checked ";
      }else{
        $use ="";
      }

      /* Dawn databse struckture ....*/
      if(empty($user['comment']) && isset($user['comments'])) {
        $user['comment'] = $user['comments'];
      }

      $field1 = array("string" => preg_replace("/%s/",base64_encode($key),preg_replace("/%USE%/",$use,$useDevice)), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($edit,base64_encode($key),$user[$str]."&nbsp;[".$user['comment']."]"), "attach" => "style=''");
      $field3 = array("string" => sprintf($editdel,base64_encode($key),base64_encode($key)), 
        "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry(array($field1,$field2,$field3));
      
    }
    
  $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' align='middle' 
      src='images/monitor.png'  title='"._("New monitor")."' alt='"._("M")."' name='new_monitor'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/mainboard.png' title='"._("New mainbord")."' alt='"._("MB")."' name='new_moboard'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/processor.png' title='"._("New processor")."' alt='"._("P")."' name='new_processor'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/server.png' title='"._("New case")."' alt='"._("C")."' name='new_case'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/net_hardware.png' title='"._("New network interface")."' alt='"._("NI")."' name='new_iface'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/memory.png' title='"._("New ram")."' alt='"._("R")."' name='new_ram'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/fai_partitionTable.png' title='"._("New hard disk")."' alt='"._("HDD")."' name='new_hdd'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/drives.png' title='"._("New drive")."' alt='"._("D")."' name='new_drive'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/hardware.png' title='"._("New controller")."' alt='"._("CS")."' name='new_control'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/gfx_hardware.png' title='"._("New graphics card")."' alt='"._("GC")."' name='new_gfxcard'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/snd_hardware.png' title='"._("New sound card")."' alt='"._("SC")."' name='new_sndcard'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/power.png' title='"._("New power supply")."' alt='"._("PS")."' name='new_power'>&nbsp;".
      " <input class='center' type='image' align='middle' 
      src='images/fai_template.png' title='"._("New misc device")."' alt='"._("OC")."' name='new_pci'>&nbsp;".
      "</div>";
    
    $filter= $_SESSION['glpiDeviceRegex'];
    $smarty->assign("devicehead", $listhead);
    $smarty->assign("devices", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info_small.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("device_regex", $filter['device_regex']);

    $display.= $smarty->fetch(get_template_path('glpiDeviceManagement.tpl', TRUE));
    return($display);
  }

  /* Save device to glpi database 
   * If this is a new device, create a new entry, else update this entry 
   */
  function save()
  {
    if($this->parent->handle->deviceExists($this->EditEntry)){
      $this->parent->handle->updateDevices($this->EditEntry);
    }else{
      $this->parent->handle->addDevice($this->EditEntry);
    }

  }

  /* this only gets all already defined devices */
  function reload()
  {
    $this->devices = $this->parent->handle->getDevices();
    ksort($this->devices);
  }

  /* This funtions saves all POST variables.
     The variable must be in the array $this->EditEntry 
  */
  function save_object()
  {
    foreach($_POST as $name => $value){

      if(preg_match("/wasOnPage_/",$name)){
        $dN = base64_decode($value);

        $device = $this->devices[$dN];
        $type   = $device['device_type'];  
            
        if(isset($device['designation'])){
          $str = "designation";
        }else{
          $str = "name";
        }

        if(isset($_POST['useDevice_'.$value])){
          $this->Selected[$type][$device[$str]] = $device;
        }else{
          if(isset($this->Selected[$type][$device[$str]])){
            unset($this->Selected[$type][$device[$str]]);
          }
        }
      }
    }

    if(is_array($this->EditEntry)){
      foreach($this->EditEntry as $name => $value){
        if(isset($_POST[$name])){
          $this->EditEntry[$name] = $_POST[$name];
        }

      }
    }
  }

  /* returns the selected devices */
  function getSelected()
  {
    return($this->Selected); 
  }

  /* This function cehck all created devices if you wan't to create device specific check 
      use >>if($attr['device_type']=="moboard")<< to create a device type depending check
   */
  function check($attr)
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(isset($attr['designation'])){
      $str2 = "designation";
    }else{
      $str2 = "name";
    }
    if(empty($attr[$str2])){
      $message[]=(_("You have to specify a valid name for this device."));
    }

    /* Avoid same name twice */
    $devices = ($this->parent->handle->getDevices());
    foreach($devices as $dev){

      /* Some devices use designation some name for name
       */
      if(isset($dev['designation'])){
        $str = "designation";
      }else{
        $str = "name";
      }

      if($dev[$str]==$attr[$str2]){
        
        /* Entries with ['ID'] already exists, and are only edited, if ID is missing we are currently creating a new entry */
        if(isset($attr['ID'])){
          if(!(($dev['ID'] == $attr['ID'])&&($dev['device_type']==$attr['device_type']))){
            $message[] = _("This device name is already in use.");
          }
        }else{
          $message[] = _("This device name is already in use.");
        }
      }
    }

    return($message);
  }

  /* This functions displays the template for all available devices 
   * This function is also used if we create a new device
   */
  function editDevice($entry)
  {
    $smarty = get_smarty();

    /* Transfer given data to smarty */
    foreach($this->EditEntry as $name => $value){
      $smarty->assign($name,htmlentities(utf8_decode($value)));
    }
  
    /* Set default select boxes, manufacturers ... */
    $smarty->assign("device_type",$entry['device_type']);

    $none = array(0 => _("none"));
    $manufacturer = array_merge($none,$this->parent->handle->getEnterprises());

    $ramtypes = $this->parent->handle->getRAMTypes();

    $smarty->assign("RAMtypes",     $ramtypes);
    $smarty->assign("RAMtypeKeys",  array_flip($ramtypes));

    $deviceControlTypes = array_merge($none,$this->parent->handle->getGlpiDeviceControlTypes());

    $smarty->assign("HDDInterfaceKeys",array_flip($deviceControlTypes));
    $smarty->assign("HDDInterfaces"   , $deviceControlTypes);
    
    $gfxControlTypes = array("0"=>_("None"),"AGP"=>"AGP","PCI"=>"PCI","PCI-X"=>"PCI-X","Other"=>_("Other")); 

    $smarty->assign("GFXInterfaceKeys",array_flip($gfxControlTypes));
    $smarty->assign("GFXInterfaces"   , $gfxControlTypes);
    
    $smarty->assign("FK_glpi_enterpriseKeys",array_flip($manufacturer));
    $smarty->assign("FK_glpi_enterprises", $manufacturer);

    $smarty->assign("formats",array("Large","Medium","Micro"));
    $smarty->assign("formats",array("Large","Medium","Micro"));

    $smarty->assign("formats",array("Large","Medium","Micro"));
    $smarty->assign("formatKeys",array('Grand','Moyen','Micro'));
    return($smarty->fetch(get_template_path('glpi_devices.tpl', TRUE)));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
