<?php

class trustModeDialog extends plugin 
{
    public $attributes  = array("accessTo","trustModel");
    public $accessTo    = array();
    public $trustModel  = "";
    public $trustSelect = NULL;

    function __construct(&$config, $dn, $parent = NULL)
    {
        plugin::plugin($config, $dn, $parent);

        $this->accessTo = array();
        $this->trustModel= "";
        $this->is_account = FALSE; 
        if(isset($this->attrs['trustModel'][0])){
            $this->is_account = TRUE;
            $this->trustModel= $this->attrs['trustModel'][0];
            if (isset($this->attrs['accessTo'])){
                for ($i= 0; $i<$this->attrs['accessTo']['count']; $i++){
                    $tmp= $this->attrs['accessTo'][$i];
                    $this->accessTo[$tmp]= $tmp;
                }
            }

        }
        $this->initially_was_account = $this->is_account;
    }

    public function PrepareForCopyPaste($source)
    {
        $this->accessTo = array();
        $this->trustModel= "";
        $this->is_account = FALSE;
        if(isset($source['trustModel'][0])){
            $this->is_account = TRUE;
            $this->trustModel= $source['trustModel'][0];
            if (isset($source['accessTo'])){
                for ($i= 0; $i<$source['accessTo']['count']; $i++){
                    $tmp= $source['accessTo'][$i];
                    $this->accessTo[$tmp]= $tmp;
                }
            }
        }
    }

    public function setAcl($acl)
    {
        $this->acl = $acl;
    }

    public function execute()
    {
        // Call parent
        plugin::execute();

        // Allow to select trusted machines from a list
        if (isset($_POST["add_ws"])){
            $this->trustSelect= new trustSelect($this->config,get_userinfo());
            $this->dialog= TRUE;
        }

        // Cancel trust and group dialog
        if (isset($_POST['add_ws_cancel'])){
            $this->groupSelect= NULL;
            $this->trustSelect= NULL;
            $this->dialog= FALSE;
        }

        // Add selected machines to trusted ones.
        if (isset($_POST["add_ws_finish"]) && $this->trustSelect){
            $trusts = $this->trustSelect->detectPostActions();
            if(isset($trusts['targets'])){
                $headpage = $this->trustSelect->getHeadpage();
                foreach($trusts['targets'] as $id){
                    $attrs = $headpage->getEntry($id);
                    $this->accessTo[$attrs['cn'][0]]= $attrs['cn'][0];
                }
                ksort($this->accessTo);
                $this->is_modified= TRUE;
            }
            $this->trustSelect= NULL;
            $this->dialog= FALSE;
        }

        // Remove machine from trusted ones.
        if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list'])){
            foreach($_POST['workstation_list'] as $name){
                unset ($this->accessTo[$name]);
            }
            $this->is_modified= TRUE;
        }


        if ($this->trustSelect){
            session::set('filterBlacklist', array('cn' => array_values($this->accessTo)));
            return($this->trustSelect->execute());
        }

        /* Work on trust modes */
        $smarty = get_smarty();
        $smarty->assign("trusthide", " disabled ");
        $smarty->assign("trustmodeACL",  $this->getacl("accessTo"));
        if ($this->trustModel == "fullaccess"){
            $trustmode= 1;
            // pervent double disable tag in html code, this will disturb our clean w3c html
            $smarty->assign("trustmode",  $this->getacl("accessTo"));

        } elseif ($this->trustModel == "byhost"){
            $trustmode= 2;
            $smarty->assign("trusthide", "");
        } else {
            // pervent double disable tag in html code, this will disturb our clean w3c html
            $smarty->assign("trustmode",  $this->getacl("accessTo"));
            $trustmode= 0;
        }
        $smarty->assign("trustmode", $trustmode);
        $smarty->assign("trustmodes", array( 0 => _("disabled"), 1 => _("full access"),
                    2 => _("allow access to these hosts")));


        if((count($this->accessTo))==0)
            $smarty->assign("emptyArrAccess",true);
        else
            $smarty->assign("emptyArrAccess",false);

        $smarty->assign("accessTo",$this->accessTo);

         $smarty->assign($smarty->assign("use_trustmode",in_array("trustmode", $this->multi_boxes)));
         $smarty->assign("multiple_support" , $this->multiple_support_active);
        return($smarty->fetch(get_template_path("generic.tpl",TRUE, dirname(__FILE__))));
    }

    public function save_object()
    {

        /* Trust mode - special handling */
        if(preg_match("/w/", $this->getacl("accessTo"))){
            if (isset($_POST['trustmode'])){
                $saved= $this->trustModel;
                if ($_POST['trustmode'] == "1"){
                    $this->trustModel= "fullaccess";
                } elseif ($_POST['trustmode'] == "2"){
                    $this->trustModel= "byhost";
                } else {
                    $this->trustModel= "";
                }
                if ($this->trustModel != $saved){
                    $this->is_modified= TRUE;
                }
            }
        }

    }

    public function save()
    {
        plugin::save();
        
        /* Trust accounts */
        $objectclasses= array();
        foreach ($this->attrs['objectClass'] as $key => $class){
            if (preg_match('/trustAccount/i', $class)){
                continue;
            }
            $objectclasses[]= $this->attrs['objectClass'][$key];
        }
        $this->attrs['objectClass']= $objectclasses;
        if ($this->trustModel != ""){
            $this->attrs['objectClass'][]= "trustAccount";
            $this->attrs['trustModel']= $this->trustModel;
            $this->attrs['accessTo']= array();
            if ($this->trustModel == "byhost"){
                foreach ($this->accessTo as $host){
                    $this->attrs['accessTo'][]= $host;
                }
            }
        } else {
            if ($this->initially_was_account){
                $this->attrs['accessTo']= array();
                $this->attrs['trustModel']= array();
            }
        }
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify($this->attrs);

        /* Log last action */
        if($this->initially_was_account){
            new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), 
                        $this->dn, LDAP_MOD,get_class()));
        }
    }

    public function get_multi_edit_values()
    {
        $ret = plugin::get_multi_edit_values();
        if(in_array("trustmode",$this->multi_boxes)){
            $ret['trustModel'] = $this->trustModel;
            $ret['accessTo'] = $this->accessTo;
        }
        return($ret);
    }

    public function multiple_save_object()
    {
        plugin::multiple_save_object();
        if(isset($_POST["use_trustmode"])){
            $this->multi_boxes[] = "trustmode";
        }

        if(preg_match("/w/", $this->getacl("accessTo"))){
            if (isset($_POST['trustmode'])){
                $saved= $this->trustModel;
                if ($_POST['trustmode'] == "1"){
                    $this->trustModel= "fullaccess";
                } elseif ($_POST['trustmode'] == "2"){
                    $this->trustModel= "byhost";
                } else {
                    $this->trustModel= "";
                }
                if ($this->trustModel != $saved){
                    $this->is_modified= TRUE;
                }
            }
        }
    }

    public function init_multiple_support($attrs,$all)
    {
        plugin::init_multiple_support($attrs,$all);

        if (isset($this->multi_attrs['trustModel'])){
            $this->trustModel= $this->multi_attrs['trustModel'][0];
            $this->initially_was_account= TRUE;
            $this->multi_boxes[] = "trustmode";
        } else {
            $this->initially_was_account= FALSE;
            $this->trustModel= "";
        }

        $this->accessTo = array();
        if (isset($this->multi_attrs['accessTo'])){
            for ($i= 0; $i<$this->multi_attrs['accessTo']['count']; $i++){
                $tmp= $this->multi_attrs['accessTo'][$i];
                $this->accessTo[$tmp]= $tmp;
            }
        }

    }


    public function getacl($attribute,$skip_write= FALSE)
    {
        $ui= get_userinfo();
        $skip_write |= $this->read_only;
        return  $ui->get_permissions($this->acl_base,$this->acl, $attribute,$skip_write);
    }


} 



?>
