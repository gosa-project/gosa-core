<?php

class goKioskService extends goService{

    /* This plugin only writes its objectClass */
    var $objectclasses    = array("goEnvironmentServer");
    var $attributes       = array("gotoKioskProfile");
    var $StatusFlag       = "";

    /* This class can't be assigned twice so it conflicts with itsself */
    var $conflicts        = array("goKioskService");

    var $baseDir          = "../kiosk";
    var $filesToAttach    = array();
    var $cn               = "unknown";  
    var $orig_cn          = "unknown";  
    var $gotoKioskProfiles= array();
    var $gotoKioskProfile = array();
    var $server_path      = "http://%cn/kiosk";

    function __construct(&$config, $dn, $parent)
    {
        goService::__construct($config,$dn);

        /* Get the current object name. 
         */
        if(isset($parent->parent->cn)){
            $this->cn = &$parent->parent->cn;
        }elseif(isset($this->attrs['cn'][0])){
            $this->cn = $this->attrs['cn'][0];
        }
        $this->orig_cn = $this->cn;

        $this->DisplayName = _("Kiosk profile service");
        $this->baseDir = $this->config->get_cfg_value('environment', 'kioskPath');
        $this->server_path = preg_replace("/%cn/", $this->cn, $this->server_path);

        /* Load list of profiles and check if they still exists */
        if ($this->baseDir == ""){
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

        // Prepare lists
        $this->kioskList = new sortableListing();
        $this->kioskList->setDeleteable(true);
        $this->kioskList->setInstantDelete(false);
        $this->kioskList->setEditable(true);
        $this->kioskList->setWidth("100%");
        $this->kioskList->setHeight("200px");
        $this->kioskList->setHeader(array(_("Profile")));
        $this->kioskList->setColspecs(array('*','40px'));
        $this->kioskList->setDefaultSortColumn(0);
    }


    function remove_from_parent()
    {
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gotoEnvironment)(gotoKioskProfile=*/".$this->orig_cn."/*))",array("cn"));
        if ($ldap->count() != 0){
            $obj= array();
            while($attrs = $ldap->fetch()){
                $obj[$ldap->getDN()]= $attrs['cn'][0];
            }    
            msg_dialog::display(_("Error"), msgPool::stillInUse(_("Kiosk profile service"), msgPool::buildList($obj)), ERROR_DIALOG);
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
        $smarty->assign("baseDir", set_post($this->baseDir));
        $smarty->assign("ThisACL", set_post($this->getacl("")));
        $display= "";

        /* Download kiosk profile 
         */
        $this->kioskList->save_object();
        $action = $this->kioskList->getAction();
        if($action['action'] == 'edit'){
            $id = $this->kioskList->getKey($action['targets'][0]);
            foreach($this->gotoKioskProfiles as $profile){
                $name = $this->baseDir."/".$this->gotoKioskProfiles[$id]['name'];
                if(file_exists($name)){
                    $content = file_get_contents($name);    
                    send_binary_content($content,$this->gotoKioskProfiles[$id]['name']);
                }else{
                    msg_dialog::display(_("Warning"), sprintf(_("The profile does not exists '%s'!"),
                                $this->gotoKioskProfiles[$id]['name']), ERROR_DIALOG);
                }
            }
        }


        /* Add new kiosk profile
         * in profile directory ($this->baseDir);
         */
        if((isset($_POST['profileAdd']))&&(isset($_FILES['newProfile'])) && $this->acl_is_writeable("")){
            $file = $_FILES['newProfile'];
            if(!file_exists($this->baseDir.$file['name'])){
                $filename = gosa_file_name($file['tmp_name']);
                $tmp = array(
                        'url'     => "" ,
                        'name'    => $file['name'] ,
                        'initial' => FALSE,
                        'tmp_name'=> $filename,
                        'content' => file_get_contents($filename),
                        'exists'  => TRUE);


                $used = array();
                foreach($this->gotoKioskProfiles as $k){
                    $used[] = $k['name'];
                }  
                if(in_array_strict($file['name'], $used)){
                    msg_dialog::display(_("Warning"), msgPool::duplicated(_("Profile")), WARNING_DIALOG);
                }else{
                    $this->gotoKioskProfiles[] = $tmp;
                }
            }
        }

        if($action['action'] == 'delete' && $this->acl_is_writeable("")){
            $id = $this->kioskList->getKey($action['targets'][0]);
            $name = $this->gotoKioskProfiles[$id]['name'];
            $filename = $this->baseDir."/".$name;

            /* check if profile is still in use */
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(objectClass=gotoEnvironment)(gotoKioskProfile=*".$name.")",array("cn","uid","gotoKioskProfile"));

            if ($ldap->count() != 0){

                $obj= array();
                $found= false;
                while(($attrs = $ldap->fetch()) && ($cnt)){
                    $check = preg_replace("/^.*\//i","",$attrs['gotoKioskProfile'][0]);
                    if($check == $name){
                        $obj[$ldap->getDN()]= $attrs['cn'][0];
                        $found= true;
                    }
                }

                if ($found){
                    msg_dialog::display(_("Error"), msgPool::stillInUse(_("Kiosk profile"), 
                                msgPool::buildList($obj)), ERROR_DIALOG);
                }
            }else{
                if($this->gotoKioskProfiles[$id]['initial']){
                    $res = @unlink($filename);
                    if(!$res){
                        if(!is_writeable($filename)){
                            msg_dialog::display(_("Error"), msgPool::cannotDeleteFile($filename), ERROR_DIALOG);
                        }
                        if(!file_exists($filename)){
                            msg_dialog::display(_("Error"), msgPool::fileDoesNotExist($filename), ERROR_DIALOG);
                        }
                    }
                    unset($this->gotoKioskProfiles[$id]);
                }else{
                    unset($this->gotoKioskProfiles[$id]);
                }
            }
        }

        /* Display list of profiles */
        $this->kioskList->setAcl($this->getacl(""));
        $lData = array();
        foreach($this->gotoKioskProfiles as $key => $val ){
            $lData[$key] = array('data' => array($val['name']));
        }
        $this->kioskList->setListData($this->gotoKioskProfiles, $lData);
        $this->kioskList->update();

        /* Assign all existing profiles to smarty */
        $smarty->assign("kioskList",$this->kioskList->render());
        $smarty = get_smarty(); 
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }
        $smarty->assign("server_path", set_post($this->server_path));
        return($smarty->fetch(get_template_path("goKioskService.tpl",TRUE,dirname(__FILE__))));
    }



    function getListEntry()
    {
        $fields = goService::getListEntry();
        $fields['Message']    = _("Kiosk profile service");
#$fields['AllowEdit']  = TRUE;
#$fields['AllowStart']  = false;
#$fields['AllowStop']  = false;
#$fields['AllowRestart']  = false;
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
                    msg_dialog::display(_("Error"), msgPool::cannotWriteFile($path), ERROR_DIALOG);
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
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }
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
                    "plRequirements"=> array(
                        'ldapSchema' => array('goEnvironmentServer' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class())
                        ),

                    "plProvidedAcls"=> array()
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
