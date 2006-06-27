<?php

class workservice extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage terminal service aspects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  var $gotoScannerEnable;
  /* Generic terminal attributes */
  var $gotoXMonitor= "";
  var $gotoXDriver= "";
  var $gotoXResolution= "";
  var $gotoXColordepth= "";
  var $gotoXHsync= "";
  var $gotoXVsync= "";
  var $AutoSync = false;
  var $gotoXKbModel= "";
  var $gotoXKbLayout= "";
  var $gotoXKbVariant= "";
  var $gotoXMouseType= "";
  var $gotoXMouseport= "";
  var $gotoScannerClients= "";
  var $gotoScannerBackend= "";
  var $goFonHardware= "automatic";

  /* Needed values and lists */
  var $ignore_account= TRUE;
  var $base= "";
  var $cn= "";
  var $orig_dn= "";
  var $XMethods= array();
  var $XDrivers= array("ati", "atimisc", "chips", "cirrus", "cyrix", "fbdev",
      "i128", "i740", "i810", "imstt", "mga", "neomagic", "newport", "nsc",  "nv",
      "r128", "radeon", "rendition", "s3", "s3virge", "savage", "siliconmotion",
      "sis", "tdfx", "tga", "trident", "tseng", "vesa", "vga", "vmware");

  var $XResolutions = array();

  var $MouseTypes= array();
  var $MousePorts= array();
  var $hardware_list= array();
  var $used_hardware= array();


  /* attribute list for save action */
  var $attributes= array("gotoXMonitor",
      "gotoXDriver", "gotoXResolution", "gotoXColordepth",
      "gotoXHsync", "gotoXVsync",
      "gotoScannerEnable", "gotoScannerClients",
      "gotoScannerBackend", "gotoXKbModel", "gotoXKbLayout", "gotoXKbVariant",
      "gotoXMouseType", "gotoXMouseport", "goFonHardware");
  var $objectclasses= array("GOhard");

  var $XColordepths     =array();
  var $XKbModels        =array();
  var $XKbLayouts       =array();
  var $XKbVariants      =array();

  function workservice ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->XResolutions= array( 
        "640x480"   =>  "640x480",
        "800x600"   =>  "800x600",
        "1024x768"  =>  "1024x768",
        "1152x864"  =>  "1152x864", 
        "1280x1024" =>  "1280x1024",
        "1400x1050" =>  "1400x1050", 
        "1600x1200" =>  "1600x1200");

    if(isset($this->config->data['MAIN']['RESOLUTION_HOOK'])){
      $file = $this->config->data['MAIN']['RESOLUTION_HOOK'];

      if(is_readable($file)){
        $str = file_get_contents($file);
        $lines = split("\n",$str);
        foreach($lines as $line){
          $line = trim($line);
          if(!empty($line)){
            $this->XResolutions[$line]=$line;
          }
        }
        //natcasesort($this->gotoXResolutions);
      }else{
        print_red(sprintf(_("You have specified an external resolution hook which can't be read, please check the permission of the file '%s'."),$file));
      }
    }

    array_unshift($this->XDrivers, "["._("unknown")."]");
 
    $this->XColordepths= array( 
        "8"        => "8 " ._("bit"), 
        "15"       => "15 "._("bit"),      
        "16"       => "16 "._("bit"),   
        "24"       => "24 "._("bit"));

    foreach(array ("btc9000", "chicony", "compaq", "dell", "dell101", "everex",
          "flexpro", "geniuscomfy", "hp", "itouch", "jp106", "logicordless",
          "logiinetnav", "logiinternet", "macintosh", "microsoft",
          "microsoftpro", "omnikey101", "pc101", "pc102", "pc104",
          "pc105", "rapidaccess", "rapidaccess2", "winbook") as $type){
      $this->XKbModels[$type] = $type;
    }

    $this->MouseTypes= array("ImPS/2" => "ImPS/2", "PS/2" => "PS/2", "Microsoft" => "Microsoft", 
                              "Logitech" => "Logitech");

    $this->MousePorts= array("/dev/ttyS0"  =>"/dev/ttyS0", 
                             "/dev/ttyS1"       => "/dev/ttyS1",          "/dev/psaux"  =>"/dev/psaux", 
                             "/dev/input/mice"  => "/dev/input/mice");

    /* Additional values will be extracted from /etc/gosa/keyboardLayouts */
    $this->XKbLayouts= array ("de"=> "de","intl" =>"intl","us" =>"us");
    $this->XKbVariants= array ("nodeadkeys"=>"nodeadkeys", "basic"=>"basic");

    /* try to read additional keyboard layouts 
     */
    if(file_exists(CONFIG_DIR."/keyboardLayouts")){
      if(is_readable(CONFIG_DIR."/keyboardLayouts")){
        $str = file_get_contents(CONFIG_DIR."/keyboardLayouts");
        $tmp = split("\n",$str);
        foreach($tmp as $entry){
          if((!empty($entry)) && (!preg_match("/^#/",$entry))){
            $entry = trim($entry);
            $tmp2 = split ("\:",$entry);
            $la =   trim($tmp2[0]);   // What would be saved to ldap
            $da =   trim($tmp2[1]);   // This wis displayed in the listbox
            $this->XKbLayouts [ $la] = $da;  
          } 
        }
      }
    }

    /* Initialize methods */
    $this->XMethods["indirect"]= _("show chooser");
    $this->XMethods["query"]= _("direct");
    #$this->XMethods["squery"]= _("direct via ssh");
    #$this->XMethods["nquery"]= _("direct via nx");
    $this->XMethods["load"]= _("load balanced");
    #$this->XMethods["sload"]= _("load balanced via ssh");
    #$this->XMethods["nload"]= _("load balanced via nx");
    $this->XMethods["rdp"]= _("Windows RDP");
    $this->XMethods["citrix"]= _("ICA client");

    $this->orig_dn= $this->dn;

    /* Load hardware list */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goFonHardware)", array('cn', 'description'));
    while ($attrs= $ldap->fetch()){
      $cn= $attrs['cn'][0];
      if (isset($attrs['description'])){
        $description= " - ".$attrs['description'][0];
      } else {
        $description= "";
      }
      $this->hardware_list[$cn]= "$cn$description";
    }

    /* Eventually colorize phones */
    $ldap->cd($this->config->current['BASE']);
    foreach ($this->hardware_list as $cn => $desc){
      $ldap->search("(goFonHardware=$cn)", array('cn'));
      if ($ldap->count() > 0){
        $ldap->fetch();
        if ($ldap->getDN() != $this->dn){
          $this->used_hardware[$cn]= $ldap->getDN();
        }
      }
    }

    if(preg_match("/\+/",$this->gotoXHsync)){
      $this->AutoSync = true;
      $this->gotoXHsync = preg_replace("/\+/","-",$this->gotoXHsync);
      $this->gotoXVsync = preg_replace("/\+/","-",$this->gotoXVsync);
    }

    $this->hardware_list["automatic"]= _("automatic");
    ksort($this->hardware_list);

    /* Load hardware list */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".$this->dn."))");
    if ($ldap->count() == 1){
      $map= array("gotoXResolution", "gotoXColordepth", "gotoXKbModel", "gotoXKbLayout",
                  "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport");
      $attrs= $ldap->fetch();

      foreach ($map as $name){
        if (!isset($attrs[$name][0])){
          continue;
        }
        
        switch ($name){
          case 'gotoXResolution':
            $this->XResolutions= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']' ), $this->XResolutions);
            break;
          case 'gotoXColordepth':
            $this->XColordepths= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].' '._('Bit').']'), $this->XColordepths);
            break;
          case 'gotoXKbModel':
            $this->XKbModels= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']'), $this->XKbModels);
            break;
          case 'gotoXKbLayout':
            $this->XKbLayouts= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']' ), $this->XKbLayouts);
            break;
          case 'gotoXKbVariant':
            $this->XKbVariants= array_merge(array('default' => _("inherited").' ['.$attrs[$name][0].']' ), $this->XKbVariants);
            break;
          case 'gotoXMouseType':
            $this->MouseTypes= array_merge(array('AUTO' => _("inherited").' ['.$attrs[$name][0].']' ), $this->MouseTypes);
            break;
          case 'gotoXMouseport':
            $this->MousePorts= array_merge(array('AUTO' => _("inherited").' ['.$attrs[$name][0].']' ), $this->MousePorts);
            break;
        }

      }

    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid terminal? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' has no terminal features.")."</b>";
      return ($display);
    }

    /* Show main page */
    $smarty= get_smarty();

    /* Arrays */ 
    foreach(array("XMethods", "XDrivers", "XResolutions", "XColordepths",
          "XKbModels", "XKbVariants",
          "MouseTypes", "MousePorts") as $val){
      $smarty->assign("$val", $this->$val);
    }
    $smarty->assign("XKbLayouts"    ,$this->XKbLayouts);
    $smarty->assign("XKbLayoutKeys" ,array_flip($this->XKbLayouts));

    $smarty->assign("xdmcpservers", $this->config->data['SERVERS']['TERMINAL']);
    $smarty->assign("fontservers", $this->config->data['SERVERS']['FONT']);
    $smarty->assign("nfsservers", $this->config->data['SERVERS']['NFS']);
    $smarty->assign("nfsservers", $this->config->data['SERVERS']['NFS']);

    /* Variables - select */
    foreach(array(
          "gotoXDriver", "gotoXResolution", "gotoXColordepth", 
          "gotoXKbModel", "gotoXKbLayout","gotoScannerEnable",
          "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport") as $val){

      $smarty->assign($val."_select", $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }

    /* Variables */
    foreach(array("gotoXHsync", "gotoXVsync") as $val){
      $smarty->assign($val, $this->$val);
      $smarty->assign($val."ACL", chkacl($this->acl, $val));
    }
    $smarty->assign("staticAddress", "");

    /* Checkboxes */
    foreach(array("gotoScannerEnable") as $val){
      if ($this->$val == TRUE) {
        $smarty->assign("$val", "checked");
      } else {
        $smarty->assign("$val", "");
      }
    }

    /* Phone stuff */
    $smarty->assign ("goFonHardware", $this->goFonHardware);
    $hl= "<select size=\"1\" name=\"goFonHardware\" title=\"".
      _("Choose the phone located at the current terminal")."\" ".
      chkacl($this->acl, "goFonHardware").">\n";
    foreach ($this->hardware_list as $cn => $description){
      if ($cn == $this->goFonHardware){
        $selected= "selected";
      } else {
        $selected= "";
      }
      if (isset($this->used_hardware[$cn])){
        $color= "style=\"color:#A0A0A0\"";
      } else {
        $color= "";
      }
      $hl.= "  <option $color label=\"$cn\" value=\"$cn\" $selected>$description</option>\n";
    }
    $hl.= "</select>\n";
    $smarty->assign ("hardware_list", $hl);
    $smarty->assign ("gotoXMonitor", $this->gotoXMonitor);
    $smarty->assign("AutoSyncACL",chkacl($this->acl,"AutoSync"));

    if($this->AutoSync){
      $smarty->assign("AutoSyncCHK"," checked ");
      $smarty->assign("hiddenState"," disabled ");
    }else{
      $smarty->assign("AutoSyncCHK"," ");
      $smarty->assign("hiddenState","");
    }

    /* Show main page */
    return($smarty->fetch (get_template_path('workstationService.tpl',TRUE,dirname(__FILE__))));
  }

  function remove_from_parent()
  {
    $this->handle_post_events("remove");
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();

    if(isset($_POST['gotoXDriver'])){
      if(isset($_POST['AutoSync'])){
        $this->AutoSync = true;
      }else{
        $this->AutoSync = false;
      }
    }
  }

  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Default entries can use blank hsync/vsync entries */
    if ($this->dn != "" && $this->cn != "default" && $this->cn != "wdefault"){

      /* But only if no auto sync is enabled... */
      if (!$this->AutoSync){

        /* Check vsync for correct usage */
        $val= preg_replace ("/\s/", "", $this->gotoXVsync);
        if (!preg_match ("/^\d+(\.\d+)?([-]\d+(\.\d+)?)?$/", $val)
            && chkacl ($this->acl, "gotoXVsync") == ""){

          $message[]= _("Please specify a valid VSync range.");
        } elseif (chkacl ($this->acl, "gotoXVsync") == ""){
          list($v1,$v2)= preg_split ("/[-+]/", $val);
          if ($v2 != ""){
            if ($v1 > $v2){
              $message[]= _("Please specify a valid VSync range.");
            }
          }
        }

        /* Check hsync for correct usage */
        $val= preg_replace ("/\s/", "", $this->gotoXHsync);
        if (!preg_match ("/^\d+(\.\d+)?([-]\d+(\.\d+)?)?$/", $val)
            && chkacl ($this->acl, "gotoXHsync") == ""){

          $message[]= _("Please specify a valid HSync range.");
        } elseif (chkacl ($this->acl, "gotoXHsync") == ""){
          list($v1,$v2)= preg_split ("/[-+]/", $val);
          if ($v2 != ""){
            if ($v1 > $v2){
              $message[]= _("Please specify a valid HSync range.");
            }
          }
        }
      }
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    /* remove objectclass GOhard if this is an ogroup tab */
    if(isset($this->parent->by_object['ogroup'])){
      $this->objectclasses = array();
    }

    plugin::save();

    /* Strip out 'default' values */
    foreach(array(
          "gotoXDriver", "gotoXResolution", "gotoXColordepth",
          "gotoXKbModel", "gotoXKbLayout",
          "gotoXKbVariant", "gotoXMouseType", "gotoXMouseport") as $val){

      if ($this->attrs[$val] == "default"){
        $this->attrs[$val]= array();
      }
    }

    if($this->AutoSync){
      $this->attrs['gotoXHsync'] = "30+55";
      $this->attrs['gotoXVsync'] = "50+70";
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving workstation services failed"));
    $this->handle_post_events("modify");
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
