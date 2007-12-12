<?php

class goKioskService extends goService{
	
  /* This plugin only writes its objectClass */
  var $objectclasses    = array("goEnvironmentServer");
  var $attributes       = array("gotoKioskProfile","cn");
  var $StatusFlag       = "";
 
  /* This class can't be assigned twice so it conflicts with itsself */
  var $conflicts        = array("goKioskService");

  var $baseDir          = "../kiosk";
  var $filesToAttach    = array();
  var $cn               = "unknown";  
  var $gotoKioskProfiles= array();
  var $gotoKioskProfile = array();

  var $server_path      = "http://%cn/kiosk";
 
  function goKioskService(&$config,$dn)
  {
    goService::goService($config,$dn);

    $this->DisplayName = _("Kiosk profile service");
    $this->baseDir = $this->config->search('environment', 'kioskpath',array('menu','tabs'));
    $this->server_path = preg_replace("/%cn/", $this->cn, $this->server_path);

    /* Load list of profiles and check if they still exists */
    if ($this->baseDir == ""){
      print_red(_("There is no KIOSKPATH defined in your gosa.conf. Can't manage kiosk profiles!"));
    }else{
      $this->gotoKioskProfiles = array();
      if(isset($this->attrs['gotoKioskProfile']) && is_array($this->attrs['gotoKioskProfile'])){
        for($i = 0 ; $i < $this->attrs['gotoKioskProfile']['count']; $i ++){
          $url = $this->attrs['gotoKioskProfile'][$i];
          $this->server_path = preg_replace("/\/[^\/]*$/","",$url);
          $name= preg_replace("/^.*\//","",$url);
          $this->gotoKioskProfiles[] = array('url'     => $url , 
                                            'name'    => $name , 
                                            'initial' =>TRUE,
                                            'exists'  => file_exists($this->baseDir."/".$name));
        }
      }
    }
  }


  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gotoEnvironment)(gotoKioskProfile=*/".$this->cn."/*))",array("cn"));
    $str ="";
    $cnt = 3;
    while($cnt && $attrs = $ldap->fetch()){
      $str .= $attrs['cn'][0].", ";
      $cnt --;
    }    
    $str = preg_replace("/, $/","",$str);
    if($cnt != 3){
      print_red(sprintf(_("Can not remove kiosk profile service, it is currently in use by following user(s) : %s."),$str));
    }else{
      goService::remove_from_parent();
    }
  }
  
  
  function execute()
  { 
    /* log actions */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Add new kiosk profile
     * in profile directory ($this->baseDir);
     */
    if((isset($_POST['profileAdd']))&&(isset($_FILES['newProfile']))){
      $file = $_FILES['newProfile'];
      if(!file_exists($this->baseDir.$file['name'])){
        $tmp = array(
            'url'     => "" ,
            'name'    => $file['name'] ,
            'initial' => FALSE,
            'tmp_name'=> $file['tmp_name'],
            'content' => file_get_contents($file['tmp_name']),
            'exists'  => TRUE);
        $this->gotoKioskProfiles[] = $tmp;
      }
      if(!is_writeable($this->baseDir)){
        print_red(sprintf(_("Can't save new kiosk profiles, possibly permission denied for folder: '%s'."),$this->baseDir));
      }
    }

    $only_once = true;
    foreach($_POST as $name => $value){

      if((preg_match("/^delkiosk_/",$name))&&($only_once)){
  
        /* Get id, name and path */
        $only_once = false;
        $id = preg_replace("/^delkiosk_/","",$name);
        $id = preg_replace("/_.*$/","",$id);
        $name = $this->gotoKioskProfiles[$id]['name'];
        $filename = $this->baseDir."/".$name;

        /* check if profile is still in use */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gotoEnvironment)(gotoKioskProfile=*".$name.")",array("cn","uid","gotoKioskProfile"));
        $used_by = "";
        $cnt = 3;
        while(($attrs = $ldap->fetch()) && ($cnt)){
          $cnt --;
          $check = preg_replace("/^.*\//i","",$attrs['gotoKioskProfile'][0]);
          if($check == $name){
            $used_by .= $attrs['cn'][0].", ";
          }
        }
        $used_by = preg_replace("/, $/","",$used_by);
        if(!empty($used_by)){
          print_red(sprintf(_("Can't remove kioks profile, it is still in use by the following objects '%s'."),$used_by));
        }else{
          if($this->gotoKioskProfiles[$id]['initial']){
            $res = @unlink($filename);
            if(!$res){
              if(!is_writeable($filename)){
                print_red(sprintf(_("Can't delete '%s'. Error was: permission denied."), $filename));
              }
              if(!file_exists($filename)){
                print_red(sprintf(_("Can't delete '%s'. Error was: file doesn't exist."), $filename));
              }
            }
            unset($this->gotoKioskProfiles[$id]);
          }
        }
      }
    }

    /* Display list of profiles */
    $divlist = new divSelectBox("KioskProfiles");
    $divlist -> SetHeight (300);
    foreach($this->gotoKioskProfiles as $key => $val ){
      $divlist->AddEntry(array(
            array("string"=>"<a target='_blank' href='getkiosk.php?id=".$val['name']."'>".$val['name']."</a>"),
            array("string"=>"<input type='image' src='images/edittrash.png'
              class='center' alt='delete' name='delkiosk_".$key."'>" ,
              "attach"=>" style='border-right: 0px;width:24px; text-align:center;' ")
            ));
    }

    /* Assign all existing profiles to smarty */
    $smarty->assign("divlist",$divlist->DrawList());
    $smarty = get_smarty(); 
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    $smarty->assign("server_path",$this->server_path);
    return($smarty->fetch(get_template_path("goKioskService.tpl",TRUE,dirname(__FILE__))));
  }

  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']    = _("Kiosk profile service");
    $fields['AllowEdit']  = TRUE;
    $fields['AllowStart']  = false;
    $fields['AllowStop']  = false;
    $fields['AllowRestart']  = false;
    return($fields);
  }

  function check()
  { 
    $message = plugin::check();
    return($message);
  }

  function save()
  {
    goService::save();

    $method = $this->server_path."/";
    $method = preg_replace("/\/\/*$/","/",$method);
    $this->attrs['gotoKioskProfile'] = array();
    foreach($this->gotoKioskProfiles as $profile){
      if(!$profile['initial']){

        $contents = $profile['content'];
        $path = $this->baseDir."/".$profile['name'];
        $fp = @fopen($path,"w");
        if(!$fp){
          print_red(_("Can't save new kiosk profiles, possibly permission denied for folder")." : ",$path);
        }else{
          fwrite($fp,$contents,strlen($contents));
          $this->attrs['gotoKioskProfile'][] = $method.$profile['name'];
        }
        @unlink($profile['tmp_name']);
      }else{
        $this->attrs['gotoKioskProfile'][] = $method.$profile['name'];
      }
    }
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error(),_("Wohl kaum"));
  }

  
  function save_object()
  {
    if(isset($_POST['goKioskPosted'])){
      goService::save_object();
      if(isset($_POST['server_path'])){
        $this->server_path = get_post("server_path");
      }
    }
  }  


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Kiosk"),
          "plDescription" => _("Kiosk profile management")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 100,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array()
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
