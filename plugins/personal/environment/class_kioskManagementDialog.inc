<?php

class kioskManagementDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array("filesToAttach");
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  
  var $filesToAttach        = array();

  var $baseDir              = "../kioskProfiles/";

  function kioskManagementDialog ($config, $dn= NULL,$attach=false )
  {
    plugin::plugin ($config, $dn);
    if($attach){
      $this->filesToAttach = $attach;
    }
    $this->baseDir = search_config($this->config->data,"environment", "KIOSKPATH");
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new kiosk profile 
     * in profile directory ($this->baseDir); 
     */
    if((isset($_POST['profileAdd']))&&(isset($_FILES['newProfile']))){
      $file = $_FILES['newProfile'];
      if(!file_exists($this->baseDir.$file['name'])){
        $this->filesToAttach[$file['name']]=$file;
        $this->filesToAttach[$file['name']]['contents'] = file_get_contents($file['tmp_name']);
      }
    }
      
    /* Delete profile
     * Delete selected file form $this->baseDir
     */
    if((isset($_POST['profileDel']))&&(isset($_POST['gotoKioskProfile']))){
      $filename = $this->baseDir."/".$_POST['gotoKioskProfile'];
  
      $res = @unlink($filename);
      if(!$res){
        if(!is_writeable($filename)){
          print_red(sprintf(_("Can't delete '%s'. Error was: permission denied."), $filename));
        }
        if(!file_exists($filename)){
          print_red(sprintf(_("Can't delete '%s'. Errow was: file doesn't exist."), $filename));
        }
      
      }
    }

    /*Assign all existing profiles to smarty*/
    $smarty->assign("gotoKioskProfiles",$this->getKioskProfiles());
    $smarty->assign("gotoKioskProfileKeys",array_flip($this->getKioskProfiles()));

    $display.= $smarty->fetch(get_template_path('kioskManagement.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function save(){
    return($this->filesToAttach);
  }

  function getKioskProfiles($attach = false)
  {
    $a_return = array();
    
    /* Empty? */
    if ($this->baseDir == ""){
      print_red(_("There is no KIOSKPATH defined in your gosa.conf. Can't manage kiosk profiles!"));
      return ($a_return);
    }
    
    if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
      $method="https://";
    }else{
      $method="http://";
    }

    $str = $method.$_SERVER['SERVER_NAME']."/kioskProfiles/";

    $dir = @opendir($this->baseDir);
    if(!$dir){
      print_red(sprintf(_("Kiosk path '%s' is not accessible. Please check the permissions."),$this->baseDir));
    }else{
      $a_return = array();
      while($file = readdir($dir)){
        if(!(($file==".")||($file==".."))){
          $name = $file;
          $a_return[$str.$name] = $name;
        }
      }
    }
    
    if(preg_match("/https/i",$_SERVER['HTTP_REFERER'])){
      $method="https://";
    }else{
      $method="http://";
    }

    $str = $method.$_SERVER['SERVER_NAME']."/kioskProfiles/";
    foreach($this->filesToAttach as $file){
      $a_return[$str.$file['name']] = $file['name'];
    }

    if($attach){
      foreach($attach as $file){
        $a_return[$str.$file['name']] = $file['name'];
      }
    }

    return($a_return);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
