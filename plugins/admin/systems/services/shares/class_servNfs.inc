<?php

class servnfs extends plugin
{
    /* attribute list for save action */
    var $ignore_account   = TRUE;
    var $attributes       = array("description","type","charset","path","option", "volume");
    var $objectclasses    = array("whatever");
    var $is_account       = true;

    var $name          ="";  // Name of  
    var $description   ="";  // description
    var $type          ="";  // Type FS/Samba/NCP
    var $charset       ="";  // charset
    var $host          ="";  // hostname
    var $types         =array();  // Array Types NFS/Samba/NCP/netatalk
    var $charsets      =array();  // Array with charsets
    var $path          ="";  // Path
    var $volume        ="";  // Volume
    var $option        ="";  // Options
    var $is_edit           =false;
    var $create_mount_init = false; //mount entry set for this entry (while editing)?
    var $create_mount = false; //save mount entry

    var $parent       = NULL;
    var $view_logged  = FALSE;

    function __construct(&$config, $parent,$entry= false,$mount =false)
    {
        $this->parent = $parent;
        $dn = $parent->dn;
        plugin::__construct ($config, $dn);

        $this->types   = array("NBD" => "NBD", "CIFS" => "CIFS", "NFS"=>"NFS","samba"=>"samba","netatalk"=>"netatalk","NCP"=>"NCP");
        if($dn){
            $this->host = substr($dn, 3, strpos($dn, ',')-3);
        }
        asort($this->types);

        $this->charsets = array();

        if(!file_exists(CONFIG_DIR."/encodings")){
            msg_dialog::display(_("Error"), msgPool::fileDoesNotExist(CONFIG_DIR."/encodings"), ERROR_DIALOG);
        }else{
            if(!is_readable(CONFIG_DIR."/encodings")){
                msg_dialog::display(_("Error"), msgPool::cannotReadFile(CONFIG_DIR."/encodings"), ERROR_DIALOG);
            }else{
                $fp = fopen(CONFIG_DIR."/encodings","r");
                $i = 100;
                while(!feof($fp)&&$i){
                    $i -- ;
                    $str = trim(fgets($fp,256));

                    /* Skip comments */
                    if(!preg_match("/^#/",$str)){
                        $arr = explode("=",$str);
                        if(count($arr)==2){
                            $this->charsets[$arr[0]]=$arr[1];
                        }
                    }
                }
            }
        }

        if($entry){
            list($this->name, $this->description, $this->type, $this->charset,
                    $this->path, $this->option, $this->volume)= explode("|",$entry."|");
            $this->is_edit          = true;

        }else{
            $this->attributes[] = "name";
        }


        $this->create_mount_init = $mount;
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","server/".get_class($this),$this->dn);
        }

        /* Fill templating stuff */
        $smarty= get_smarty();

        $smarty->assign("charsets" ,set_post($this->charsets));
        $smarty->assign("types"    ,set_post($this->types));

        /* attrs to smarty*/
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }

        $tmp = $this->parent->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->parent->getacl($name));
        }

        $smarty->assign("name",set_post($this->name));
        if($this->is_edit){
            $smarty->assign("nameACL", preg_replace("/w/","",$this->parent->getacl("name")));
        } 
        $smarty->assign("allow_mounts", $this->parent->allow_mounts);
        $smarty->assign("mount_checked", "");

        $smarty->assign("appleMountsACL",    $this->parent->getacl("appleMounts"));

        if (($this->type == "netatalk") || ($this->type == "NFS")) {
            if ($this->create_mount_init) {
                $smarty->assign("mount_checked", "checked");
            } else {
                $tmp = explode(",", $this->dn);
                $clip = $tmp[0] . ",".get_ou("servgeneric", "serverRDN");
                $mountsdn = "cn=mounts," . substr($this->dn, strlen($clip));
                switch ($this->type) {
                    case "netatalk" : {
                        $mountdn = "cn=".$this->host.":/".$this->name.",".$mountsdn;
                        break;
                    }
                    case "NFS" : {
                        $mountdn = "cn=".$this->host.":".$this->path.",".$mountsdn;
                        break;
                    }
                    default : {
                                  continue;
                              }
                }
                $ldap = $this->config->get_ldap_link();
                $ldap->cat($mountdn, array('dn'));
                $attrs = $ldap->fetch();
                if (count($attrs) > 0) {
                    $smarty->assign("mount_checked", "checked");
                }
            }        
        }

        $display= $smarty->fetch(get_template_path('servnfs.tpl', TRUE, dirname(__FILE__)));
        return($display);
    }

    function remove_from_parent()
    {
        /* This cannot be removed... */
    }


    /* Save data to object */
    function save_object()
    {
        if(isset($_POST['servnfs_posted'])){

            foreach($this->attributes as $name){
                if($this->parent->acl_is_writeable($name) && isset($_POST[$name])){
                    $this->$name = get_post($name);
                }
            }

            if ((isset($_POST['netatalk_mount'])) && (($this->type == "netatalk") || ($this->type == "NFS"))) {
                $this->create_mount = true;
            } else {
                $this->create_mount = false;
            }
        }
    }


    /* Check supplied data */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        if(empty($this->path)){
            $message[]= msgPool::required(_("Path"));
        }

        if($this->type == "NBD") {
            if (!preg_match('/^[a-z0-9-]+:[0-9]+$/', $this->path)){
                $message[]= _("'Path / Volume' must be of format 'server:port'!");
            }
        } else {
            if (!tests::is_path($this->path)){
                $message[]= _("'Path / Volume' must contain at least one '/'!");
            }
        }

        if(!in_array_strict($this->type,$this->types)){
            $message[]= msgPool::required(_("Type"));
        }

        // only 0-9a-z
        if(!$this->is_edit){
            if(empty($this->name)){
                $message[]= msgPool::required(_("Name"));
            }
            if(!preg_match("/^[a-z0-9\._äüö-]*$/ui",$this->name)){
                $message[]= msgPool::invalid(_("Name"),$this->name,"/[a-z0-9\._äüö-]/ui");
            }
        }

        if(!empty($this->description) && preg_match("/^[^a-z0-9\._äüö\+ -]*$/ui",$this->description)){
            $message[]= msgPool::invalid(_("Description"),$this->description,"/^[a-z0-9\._äüö\+ -]*$/ui");
        }

        if(!empty($this->volume) && preg_match("/^[^a-z0-9\._äüö\+ -]*$/ui",$this->volume)){
            $message[]= msgPool::invalid(_("Volume"),$this->volume,"/^[a-z0-9\._äüö\+ -]*$/ui");
        }

        if(!empty($this->option) && preg_match("/^[^a-z0-9\._äüö,=\+ -]*$/ui",$this->option)){
            $message[]= msgPool::invalid(_("Option"),$this->option,"/^[a-z0-9\._äüö\+ -]*$/ui");
        }

        /* remove a / at the end of the path, we neither need it there nor
         * do we want to check for it later.
         */
        if(substr($this->path, -1, 1) == '/') {
            $this->path=substr($this->path, 0, -1);
        }

        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(objectClass=goShareServer)", array("goExportEntry"));
        while($test = $ldap->fetch()){
            if($test['dn']==$this->dn)
                continue;
            if(isset($test['goExportEntry'])){
                foreach($test['goExportEntry'] as $entry){
                    $tmp = explode("|",$entry);
                    if($tmp[0] == $this->name){
                        $message[]= msgPool::duplicated(_("Name"));
                    }
                }
            }
        }
        return ($message);
    }


    /* Save to LDAP */
    function save()
    {
        /* Everything seems perfect, lets 
           generate an new export Entry 
         */

        $s_return = "";

        $s_return.= $this->name."|";     
        $s_return.= $this->description."|";     
        $s_return.= $this->type."|";     
        $s_return.= $this->charset."|";     
        $s_return.= $this->path."|";     
        $s_return.= $this->option."|";     
        $s_return.= $this->volume;     

        return(array($this->name=>$s_return));
    }

    function should_create_mount() {
        return $this->create_mount;
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
