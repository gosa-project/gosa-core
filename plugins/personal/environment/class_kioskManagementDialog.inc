<?php

class kioskManagementDialog extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account       = TRUE;
  var $attributes           = array();
  var $objectclasses        = array("whatever");
  var $use_existing         = false;  

  var $baseDir              = "../kioskProfiles/";
  var $filePrefix           = ".kioskProfile";

  function kioskManagementDialog ($config, $dn= NULL,$use_existing=false )
  {
    $this->use_existing = $use_existing;
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new kiosk profile 
     * Save the new Profile with filePrefix ($this->filePrefix)
     * in profile directory ($this->baseDir); 
     */
    if((isset($_POST['profileAdd']))&&(isset($_FILES['newProfile']))){
      $file = $_FILES['newProfile'];
      if(!file_exists($this->baseDir.$file['name'])){
        $name = preg_replace("/\..*$/","",$file['name']).$this->filePrefix;
        $str  = file_get_contents($file['tmp_name']);
        $fp   = @fopen($this->baseDir.$name,"w+");
        if($fp){
          fwrite($fp,$str,strlen($str));
          fclose($fp);
        }else{
          print_red(_("Can't save kioskProfile. Permission denied.")." ".$this->baseDir);
        }
        unlink($file['tmp_name']);;
      }
    }
      
    /* Delete profile
     * Delete selected file form $this->baseDir
     */
    if((isset($_POST['profileDel']))&&(isset($_POST['gotoKioskProfile']))){
      $filename = $this->baseDir.$_POST['gotoKioskProfile'].$this->filePrefix;
  
      $res = @unlink($filename);
      if(!$res){
        if(!is_writeable($filename)){
          print_red(_("Can't delete '".$filename."' permission denied."));
        }
        if(!file_exists($filename)){
          print_red(_("Can't delete '".$filename."' file don't exists."));
        }
      
      }
    }

    /*Assign all existing profiles to smarty*/
    $smarty->assign("gotoKioskProfiles",$this->getKioskProfiles());
    $smarty->assign("gotoKioskProfileKeys",array_flip($this->getKioskProfiles()));

    $display.= $smarty->fetch(get_template_path('kioskManagement.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }

  function getKioskProfiles()
  {
    $dir = @opendir($this->baseDir);
    if(!$dir){
      print_red(sprintf(_("Can't open %s permission denied."),$this->baseDir));
    }else{
      $a_return = array();
      while($file = readdir($dir)){
        if(preg_match("/".$this->filePrefix."$/i",$file)){
          $name = preg_replace("/".$this->filePrefix."$/i","",$file);;
          $a_return[$name] = $name;
        }
      }
    }
    return($a_return);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
