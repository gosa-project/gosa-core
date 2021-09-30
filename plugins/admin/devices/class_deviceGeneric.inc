<?php

class deviceGeneric extends plugin
{
    public $dn            = "";
    public $cn            = "";
    public $orig_cn       = "";
    public $orig_base     = "";
    public $orig_dn       = "";
    public $description   = "";
    public $vendor        = "";
    public $devID        = "";
    public $serial        = "";
    public $base          = "";	
    public $types;
    public $type;
    public $baseSelector;

    public $attributes      = array("cn","description","devID","serial","vendor", "type");
    public $objectclasses   = array("top","gotoDevice");

    public $CopyPasteVars   = array("orig_cn","description","vendor","devID","serial","base");

    public function __construct(&$config,$dn = NULL)
    {
        plugin::__construct($config,$dn);

        $this->is_account = TRUE;

        $this->types= array("camera" => _("Digital camera"),
                "disc" => _("USB stick"),
                "cdrom" => _("CD/DVD drive"),);
        asort($this->types);

        /* Set class values */
        if(isset($this->attrs['gotoHotplugDevice'][0])){
            $tmp = preg_split("/\|/",$this->attrs['gotoHotplugDevice'][0]);
            $this->cn         = $this->attrs['cn'][0];
            $this->description= $tmp[0];
            $this->devID     = $tmp[1];
            $this->serial     = $tmp[2];
            $this->vendor     = $tmp[3];
            if (isset($tmp[4])){
                $this->type= $tmp[4];
            }
        }     

        $this->orig_cn = $this->cn;

        /* Set Base */
        if ($this->dn == "new"){
            if(session::is_set('CurrentMainBase')){
                $this->base = session::get('CurrentMainBase');
            }else{
                $ui= get_userinfo();
                $this->base= dn2base($ui->dn);
            }
        } else {
            $this->base =preg_replace ("/^[^,]+,".preg_quote(get_ou("deviceGeneric", "deviceRDN"), '/')."/i","",$this->dn);
        }
        $this->orig_base = $this->base;
        $this->orig_dn   = $this->dn;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
    }


    public function execute()
    {
        plugin::execute();
        $smarty = get_smarty();

        $smarty->assign("base", $this->baseSelector->render());
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $attr => $desc){
            $smarty->assign($attr."ACL", $this->getacl($attr));
        }

        $smarty->assign("type",set_post($this->type));
        $smarty->assign ("types", set_post($this->types));
        return($smarty->fetch(get_template_path("deviceGeneric.tpl",TRUE,dirname(__FILE__))));
    }


    public function check()
    {
        $message = plugin::check();

        if($this->cn == "" ||(preg_match("/[^a-z0-9]/i",$this->cn))){
            $message[]= msgPool::invalid(_("Name"),$this->cn,"/^[a-z0-9]*$/i");
        }
        if(preg_match("/[^a-z0-9!\"?.,;:-_\(\) ]/i",$this->description)){
            $message[]= msgPool::invalid(_("Description"),$this->cn,"/^[a-z0-9!\"?.,;:-_\(\) ]*$/i");
        }

        /* Skip serial check if vendor and product id are given */
        if(preg_match("/^\s+$/i",$this->devID)){
            $message[]= msgPool::invalid(_("iSerial"),"","01234");
        }elseif(empty($this->devID) && $this->devID!="0"){
            $message[]= msgPool::required(_("iSerial"));
        }
        if(empty($this->serial) || !$this->is_2byteHex($this->serial)){
            $message[]= msgPool::invalid(_("Product-ID"),"","","0x1234");
        }
        if(empty($this->vendor) || !$this->is_2byteHex($this->vendor)){
            $message[]= msgPool::invalid(_("Vendor-ID"),"","","0x1234");
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();
        }

        /* Check if entry already exists */ 
        if($this->cn != $this->orig_cn || $this->dn == "new"){
            $ldap = $this->config->get_ldap_link();
            $ldap->search("(&(objectClass=gotoDevice)(cn=".$this->cn."))",array("cn"));
            if($ldap->count()){
                $message[]= msgPool::duplicated(_("Name"));
            }
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[] = msgPool::permCreate();
        }elseif($this->orig_dn != "new" && $this->base != $this->orig_base && !$this->acl_is_moveable($this->base)){
            $message[] = msgPool::permMove();
        }

        return($message);
    }


    public function save_object()
    {
        if(isset($_POST['deviceGeneric_posted'])){
            plugin::save_object();

            /* Refresh base */
            if ($this->acl_is_moveable($this->base)){
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base= $this->baseSelector->getBase();
                    $this->is_modified= TRUE;
                }
            }

        }
    }


    public function remove_from_parent()
    {
        plugin::remove_from_parent();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);

        $ldap->search("(&(objectClass=gotoEnvironment)(gotoHotplugDeviceDN=".$this->dn."))",array("cn","gotoHotplugDeviceDN"));
        $skip = FALSE;
        $obj = array();
        while($attrs = $ldap->fetch()){
            $obj[$ldap->getDN()]= $attrs['cn'][0];
            $skip =TRUE;
        }
        if($skip){
            msg_dialog::display(_("Error"), msgPool::stillInUse(_("Device"), $obj), INFO_DIALOG);
        }else{
            $ldap->rmdir_recursive($this->dn);
        }
    }


    public function save()
    {
        plugin::save();

        /* Unset non ldap attributes 
         */
        foreach(array("devID","serial","vendor", "type", "description") as $name){
            if(isset($this->attrs[$name])){
                unset($this->attrs[$name]);
            }
        }

        $this->attrs['gotoHotplugDevice'] = "";
        foreach(array("description","devID","serial","vendor", "type") as $post){
            $this->attrs['gotoHotplugDevice'] .= $this->$post."|"; 
        }
        $this->attrs['gotoHotplugDevice'] = preg_replace("/\|$/","",$this->attrs['gotoHotplugDevice']);

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($this->dn);
        if($ldap->count()){
            $ldap->cd($this->dn);
            $ldap->modify($this->attrs);
        }else{
            $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
    }


    /* check if given str in like this 0xffff*/
    function is_2byteHex($str)
    {
        return !strlen($str) || preg_match("/^(0x|x|)[a-f0-9]{4}$/i",$str);
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);
        $source_o = new deviceGeneric($this->config,$source['dn']);
        foreach($this->CopyPasteVars as $post){
            $this->$post = $source_o->$post;
        }
    }


    /* Return a dialog with all fields that must be changed,
       if we want to copy this entry */
    function getCopyDialog()
    {
        $str = "";
        $smarty = get_smarty();
        $smarty->assign("cn",  set_post($this->cn));
        $str = $smarty->fetch(get_template_path("paste_deviceGeneric.tpl",TRUE,dirname(__FILE__)));

        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    /* Save all */
    function saveCopyDialog()
    {
        $attrs = array("cn");
        foreach($attrs as $attr){
            if(isset($_POST[$attr])){
                $this->$attr = get_post($attr);
            }
        }
    }



    /* Return plugin informations for acl handling  */
    public static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Device generic")." ("._("Hot-plug").")",
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoDevice' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(), 'deviceManagement')
                        ),
                    "plCategory"    => array("devices" => array("description"  => _("Devices"),
                            "objectClass"  => "gotoDevice")),
                    "plProperties" =>
                    array(
                        array(
                            "name"          => "deviceRDN",
                            "type"          => "rdn",
                            "default"       => "ou=devices,",
                            "description"   => _("RDN for device storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_deviceRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)),

            "plProvidedAcls"=> array(
                    "cn"            => _("Name"),
                    "base"          => _("Base"),
                    "description"   => _("Description"),
                    "type"          => _("Type"),
                    "serial"        => _("Serial"),
                    "vendor"        => _("Vendor"), 
                    "devID"        => _("Device ID"))
                ));

    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
