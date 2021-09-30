<?php
class mimetype extends plugin
{
    var $ignore_account 	= true;	// This is always a valid account
    var $objectclasses		= array("top","gotoMimeType");
    var $attributes			  = array("cn","gotoMimeApplication","gotoMimeLeftClickAction","gotoMimeIcon",
            "description","gotoMimeFilePattern","gotoMimeEmbeddedApplication",
            "gotoMimeGroup");	

    /* Class vars */
    var $ui;
    var $cn									              = "";					
    var $gotoMimeLeftClickAction			    = "I";
    var $gotoMimeLeftClickAction_I		    = true;
    var $gotoMimeLeftClickAction_E		    = false;
    var $gotoMimeLeftClickAction_Q		    = false;
    var $gotoMimeIcon						          = "*removed*";
    var $description						          = "";
    var $gotoMimeFilePattern				      = array();
    var $gotoMimeApplication				      = array();
    var $gotoMimeEmbeddedApplication	    = array();
    var $gotoMimeGroup						        = "";
    var $iconData							            = NULL;	
    var $base								              = "";
    var $ApplicationList                  = array();  


    /* To prevent errors when using the 'apply' button, we use this variables 
       to manage array attributes */
    var $use_gotoMimeIcon					        = NULL;
    var $use_gotoMimeFilePattern			    = array();
    var $use_gotoMimeApplication			    = array();
    var $use_gotoMimeEmbeddedApplication	= array();

    var $FAIstate ="";

    /* These vars will be copied too, if you use copy&paste mode */
    var $CopyPasteVars        = array("use_gotoMimeFilePattern","use_gotoMimeApplication","use_gotoMimeEmbeddedApplication","iconData",
            "gotoMimeLeftClickAction_I","gotoMimeLeftClickAction_E","gotoMimeLeftClickAction_Q","use_gotoMimeIcon");

    var $view_logged  = FALSE;

    /* Select options */
    var $MimeGroups			      = array("application","audio","chemical","image","inode","message","model",
            "multipart","text","video","x-conference","x-world");

    /* Orig_Dn is used to verify if this object is new or only edited */		
    var $orig_dn			= "";
    var $orig_base	  = "";

    var $baseSelector;

    function __construct(&$config,$dn= NULL)
    {
        plugin::__construct ($config, $dn);

        /* Save original dn */
        $this->orig_dn = $dn;	

        /* get gotoMimeLeftClickActions I/E/Q */
        if(isset($this->gotoMimeLeftClickAction)){
            $str = $this->gotoMimeLeftClickAction;
            for($i = 0 ; $i < strlen($str) ; $i ++ ){
                $varna = "gotoMimeLeftClickAction_". $str[$i];
                if(isset($this->$varna)){
                    $this->$varna = true;
                }
            }
        }

        /* Create list of defined applications in GOsa */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaApplication)(cn=*))",array('cn'));
        $this->ApplicationList = array();
        while($attrs = $ldap->fetch()){
            $this->ApplicationList[$attrs['cn'][0]] = $attrs['cn'][0];
        }

        /* If both radio buttons arn't set, set option I */
        if(!$this->gotoMimeLeftClickAction_I && !$this->gotoMimeLeftClickAction_E){
            $this->gotoMimeLeftClickAction_I = true;
        }

        /* Get list of array attributes */
        foreach(array("gotoMimeFilePattern") as $attr){
            $this->$attr = array();
            if(isset($this->attrs[$attr])){
                $tmp = array();
                for($i = 0 ; $i < $this->attrs[$attr]['count'] ; $i ++){
                    $str = $this->attrs[$attr][$i];
                    $tmp[] = $str;
                }
                $use_attr = "use_".$attr;
                $this->$use_attr = $tmp;
            }
        }

        /* Get list of array attributes with priority tag ( Test|32 )*/
        foreach(array("gotoMimeApplication","gotoMimeEmbeddedApplication") as $attr){
            $this->$attr = array();
            if(isset($this->attrs[$attr])){
                $tmp = array();
                for($i = 0 ; $i < $this->attrs[$attr]['count'] ; $i ++){
                    $str = $this->attrs[$attr][$i];
                    $tmp2= explode("|",$str);

                    if(count($tmp2) == 2){
                        $name = $tmp2[0];
                        $prio = $tmp2[1];
                        $tmp[$prio] = $name;
                    }
                }
                ksort($tmp);
                $use_attr = "use_".$attr;
                $this->$use_attr = $tmp;
            }
        }

        /* Set base */	
        if ($this->dn == "new"){
            if(session::is_set('CurrentMainBase')){
                $this->base= session::get('CurrentMainBase');
            }else{
                $ui= get_userinfo();
                $this->base= dn2base($ui->dn);
            }
        } else {
            $this->base= preg_replace ("/^.*,".preg_quote(get_ou("mimetype", "mimetypeRDN"), '/')."/i", "", $this->dn);
        }

        /* Get icon data */
        if(isset($this->attrs['gotoMimeIcon'])){
            $ldap = $this->config->get_ldap_link();
            $this->iconData = $ldap->get_attribute($this->dn,"gotoMimeIcon");
            $this->saved_attributes['gotoMimeIcon'] = $this->iconData;
        }
        if ($this->iconData == ""){
            $this->set_new_picture("");
        }
        session::set('binary',$this->iconData);
        session::set('binarytype',"image/jpeg");

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        $this->orig_base = $this->base;


        // Prepare lists
        $this->patternList = new sortableListing(array(),array(),TRUE);
        $this->patternList->setDeleteable(true);
        $this->patternList->setInstantDelete(true);
        $this->patternList->setEditable(false);
        $this->patternList->setWidth("100%");
        $this->patternList->setHeight("100px");
        $this->patternList->setColspecs(array('200px','*'));
        $this->patternList->setHeader(array(_("Section"),_("Description")));
        $this->patternList->setDefaultSortColumn(1);
        $this->patternList->setAcl('rwcdm'); // All ACLs, we filter on our own here.

        $this->applicationList = new sortableListing(array(),array(),TRUE);
        $this->applicationList->setDeleteable(true);
        $this->applicationList->setEditable(false);
        $this->applicationList->setWidth("100%");
        $this->applicationList->setHeight("100px");
        $this->applicationList->setColspecs(array('200px','*'));
        $this->applicationList->setHeader(array(_("Section"),_("Description")));
        $this->applicationList->setInstantDelete(true);
        $this->applicationList->setDefaultSortColumn(1);
        $this->applicationList->setAcl('rwcdm'); // All ACLs, we filter on our own here.

        $this->embeddedList = new sortableListing(array(),array(),TRUE);
        $this->embeddedList->setDeleteable(true);
        $this->embeddedList->setEditable(false);
        $this->embeddedList->setWidth("100%");
        $this->embeddedList->setInstantDelete(true);
        $this->embeddedList->setHeight("100px");
        $this->embeddedList->setColspecs(array('200px','*'));
        $this->embeddedList->setHeader(array(_("Section"),_("Description")));
        $this->embeddedList->setDefaultSortColumn(1);
        $this->embeddedList->setAcl('rwcdm'); // All ACLs, we filter on our own here.
    }


    function execute()
    {
        plugin::execute();
        $smarty = get_smarty();

        /* Assign base ACL */
        $smarty->assign("base", $this->baseSelector->render());

        if(!$this->view_logged){
            $this->view_logged =TRUE;
            new log("view","mimetypes/".get_class($this),$this->dn);
        }

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name,preg_match("/freeze/i",$this->FAIstate)));
        }
        $smarty->assign("IconReadable", preg_match("/r/",$this->getacl("gotoMimeIcon",preg_match("/freeze/i",$this->FAIstate))));

        /* Walk through posts and try to find some commands for us. */
        if(!preg_match("/freeze/i",$this->FAIstate)){

            /* Set a new icon was requested */
            if(isset($_POST['update_icon']) && (isset($_FILES['picture_file']['name']))){
                $this->set_new_picture(gosa_file_name($_FILES['picture_file']['tmp_name']));		
            }

            /* Add gotoMimeFilePattern */
            if(isset($_POST['AddNewFilePattern']) && isset($_POST['NewFilePattern'])){
                if($this->acl_is_writeable("gotoMimeFilePattern")){
                    $str = trim(get_post('NewFilePattern'));
                    if($str != ""){
                        $this->use_gotoMimeFilePattern[] = $str;
                    }
                }
            }	

            /* Add gotoMimeFilePattern */
            if(isset($_POST['AddNewApplication']) && 
                    (isset($_POST['NewApplication']) || isset($_POST['NewApplicationSelect']))){
                if($this->acl_is_writeable("gotoMimeApplication")){
                    $str = "";
                    if(isset($_POST['NewApplicationSelect']) && !empty($_POST['NewApplicationSelect'])){
                        $str = trim(get_post("NewApplicationSelect"));
                    }
                    if(isset($_POST['NewApplication']) && !empty($_POST['NewApplication'])){
                        $str = trim(get_post("NewApplication"));
                    }
                    if($str != "" && !in_array_strict($str,$this->use_gotoMimeApplication)){
                        $this->use_gotoMimeApplication[] = $str;
                    }
                }
            }	

            /* Add embedded application 
             *  - From input or from select box 
             */
            if(isset($_POST['AddNewEmbeddedApplication']) && 
                    (isset($_POST['NewEmbeddedApplication']) || isset($_POST['NewEmbeddedApplicationSelect']))){
                if($this->acl_is_writeable("gotoMimeEmbeddedApplication")){
                    $str = "";
                    if(isset($_POST['NewEmbeddedApplicationSelect']) && !empty($_POST['NewEmbeddedApplicationSelect'])){
                        $str = trim(get_post('NewEmbeddedApplicationSelect'));
                    }
                    if(isset($_POST['NewEmbeddedApplication']) && !empty($_POST['NewEmbeddedApplication'])){
                        $str = trim(get_post('NewEmbeddedApplication'));
                    }
                    if($str != "" && !in_array_strict($str,$this->use_gotoMimeEmbeddedApplication)){
                        $this->use_gotoMimeEmbeddedApplication[] = $str;
                    }
                }
            }	
        }	

        // Set list acls 
        $this->patternList->setAcl($this->getacl("gotoMimeFilePattern", 
                    preg_match("/freeze/i",$this->FAIstate)));
        $this->applicationList->setAcl($this->getacl("gotoMimeApplication",
                    preg_match("/freeze/i",$this->FAIstate)));
        $this->embeddedList->setAcl($this->getacl("gotoMimeEmbeddedApplication",
                    preg_match("/freeze/i",$this->FAIstate)));


        /* Before adding some entries check acls */
        $data = $lData = array();
        foreach($this->use_gotoMimeFilePattern as $key => $pattern){
            $data[$key] = $pattern;
            $lData[$key] = array('data'=>array($pattern));
        }			
        $this->patternList->setListData($data, $lData);
        $this->patternList->update();

        $data = $lData = array();
        foreach($this->use_gotoMimeApplication as $key => $pattern){
            $data[$key] = $pattern;
            $lData[$key] = array('data'=>array($pattern));
        }			
        $this->applicationList->setListData($data, $lData);
        $this->applicationList->update();

        $data = $lData = array();
        foreach($this->use_gotoMimeEmbeddedApplication as $key => $pattern){
            $data[$key] = $pattern;
            $lData[$key] = array('data'=>array($pattern));
        }
        $this->embeddedList->setListData($data, $lData);
        $this->embeddedList->update();


        $smarty->assign("isReleaseMimeType",			$this->is_release());
        $smarty->assign("gotoMimeFilePatterns",			$this->patternList->render());
        $smarty->assign("gotoMimeApplications",			$this->applicationList->render());
        $smarty->assign("gotoMimeEmbeddedApplications",	$this->embeddedList->render());

        $smarty->assign("ApplicationList",$this->ApplicationList);

        /* Assign class vars to smarty */
        foreach($this->attributes as $attr){
            $smarty->assign($attr,set_post($this->$attr));
        }	

        /* Assign additional vars that are not included in attributes*/
        foreach(array("gotoMimeLeftClickAction_I","gotoMimeLeftClickAction_E","gotoMimeLeftClickAction_Q") as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }	

        /* Assign select box options */
        $smarty->assign("gotoMimeGroups", set_post($this->MimeGroups));
        $smarty->assign("gotoMimeIcon"	,$this->get_picture());
        return($smarty->fetch(get_template_path("generic.tpl",TRUE,dirname(__FILE__))));
    }


    function save_object()
    {
        if(isset($_POST['MimeGeneric']) && !preg_match("/freeze/i",$this->FAIstate)){

            // Act on list actions here
            $this->patternList->save_object();
            $this->applicationList->save_object();
            $this->embeddedList->save_object();

            $action=$this->patternList->getAction();
            if(in_array_strict($action['action'],array('delete', 'reorder'))){
                $this->use_gotoMimeFilePattern = array_values($this->patternList->getMaintainedData());
            }
            $action=$this->applicationList->getAction();
            if(in_array_strict($action['action'],array('delete', 'reorder'))){
                $this->use_gotoMimeApplication = array_values($this->applicationList->getMaintainedData());
            }
            $action=$this->embeddedList->getAction();
            if(in_array_strict($action['action'],array('delete', 'reorder'))){
                $this->use_gotoMimeEmbeddedApplication = array_values($this->embeddedList->getMaintainedData());
            }

            /* Create a base backup and reset the
               base directly after calling plugin::save_object();
               Base will be set seperatly a few lines below */
            $base_tmp = $this->base;
            plugin::save_object();
            $this->base = $base_tmp;

            /* Refresh base */
            if(!$this->is_release()){
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

            /* Save radio buttons */
            if($this->acl_is_writeable("gotoMimeLeftClickAction")){
                if(isset($_POST['gotoMimeLeftClickAction_IE'])){
                    $chr = get_post('gotoMimeLeftClickAction_IE');
                    if($chr == "E"){
                        $this->gotoMimeLeftClickAction_E = true;
                        $this->gotoMimeLeftClickAction_I = false;
                    }else{
                        $this->gotoMimeLeftClickAction_E = false;
                        $this->gotoMimeLeftClickAction_I = true;
                    }
                }
                if(isset($_POST['gotoMimeLeftClickAction_Q'])){
                    $this->gotoMimeLeftClickAction_Q = true;
                }else{
                    $this->gotoMimeLeftClickAction_Q = false;
                }
            }
        }
    }


    /* save current changes */
    function save()
    {
        /* Create gotoMimeLeftClickAction out of checkboxes and radio buttons */	
        $arr = array ("E","I","Q");
        $str = "";
        foreach ($arr as $Chr){
            $var = "gotoMimeLeftClickAction_".$Chr;
            if($this->$var){
                $str .= $Chr;
            }
        }
        $this->gotoMimeLeftClickAction = $str;

        /* Create array entries with priority tag ( Test|3 )*/
        foreach(array("gotoMimeEmbeddedApplication","gotoMimeApplication") as $attr){
            $i 				= 0;
            $use_attr 		= "use_".$attr;
            $tmp 			= array();
            $this->$attr 	= array();
            foreach($this->$use_attr as $entry){
                $tmp[] = $entry."|".$i ++;
            }
            $this->$attr = $tmp;
        }

        /* Create array entries */
        foreach(array("gotoMimeFilePattern") as $attr){
            $i 				= 0;
            $use_attr 		= "use_".$attr;
            $tmp 			= array();
            $this->$attr 	= array();
            foreach($this->$use_attr as $entry){
                $tmp[] = $entry;
            }
            $this->$attr = $tmp;
        }

        /* Remove Icon if requested  */
        if($this->use_gotoMimeIcon != "*removed*"){
            $this->gotoMimeIcon = $this->iconData;
        }else{
            $this->gotoMimeIcon = "";
        }

        plugin::save();

        /* If this is a newly created object, skip storing those 
           attributes that contain an empty array */
        if($this->orig_dn == "new"){
            foreach(array("gotoMimeEmbeddedApplication","gotoMimeApplication","gotoMimeFilePattern") as $attr){
                if(!count($this->$attr)){
                    unset($this->attrs[$attr]);
                }
            }
        }

        $ldap = $this->config->get_ldap_link();
        $ldap-> cd ( $this->config->current['BASE']);
        $ldap->cat($this->dn);
        if($ldap->count()){
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->modify($this->attrs);
            new log("modify","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);
            new log("create","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }
    }


    /* Remove current mime type */
    function remove_from_parent()
    {
        plugin::remove_from_parent();
        $ldap = $this->config->get_ldap_link();
        $ldap->rmDir($this->dn);
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }
        new log("remove","mimetypes/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        /* Optionally execute a command after we're done */
        $this->handle_post_events("remove");

        /* Delete references to object groups */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
        while ($ldap->fetch()){
            $og= new ogroup($this->config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save ();
            if (!$ldap->success()){
                msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $og->dn, 0, get_class()));
            }
        }
    }


    /* Check given values */
    function check()
    {
        $message = plugin::check();
        if($this->cn == "" ){
            $message[] = msgPool::required(_("Name"));
        }
        if(!count($this->use_gotoMimeFilePattern)){
            $message[] = msgPool::required(_("File pattern"));
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        /* Check if there is already a mime type with this cn */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current["BASE"]);

        if($this->is_release()){
            $base = $this->parent->parent->mime_release;
        }else{
            $base = get_ou("mimetype", "mimetypeRDN").$this->base;
        }

        $ldap->ls("(&(objectClass=gotoMimeType)(cn=".$this->cn."))",$base,array("cn"));
        if($ldap->count()){
            $attrs = $ldap->fetch();
            if($this->dn != $attrs['dn']) {
                $message[]= msgPool::duplicated("cn");
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


    /** Helper functions **/

    /* Set a new picture */	
    function set_new_picture($filename)
    {
        if (empty($filename)){
            $filename= "./plugins/goto/images/default_icon.png";
            $this->use_gotoMimeIcon= "*removed*";
        }else{
            $this->use_gotoMimeIcon= $filename;
        }

        if (file_exists($filename)){
            $fd = fopen ($filename, "rb");
            $this->iconData= fread ($fd, filesize ($filename));
            session::set('binary',$this->iconData);
            session::set('binarytype',"image/jpeg");
            fclose ($fd);
        }
    }

    /* Get picture link */
    function get_picture()
    {
        session::set('binary',$this->iconData);
        session::set('binarytype',"image/jpeg");
        return("getbin.php");
    }


    /* Return a dialog with all fields that must be changed, 
       if we want to copy this entry */
    function getCopyDialog()
    {
        $str = "";

        $smarty = get_smarty();
        $smarty->assign("cn",     	   set_post($this->cn));
        $smarty->assign("description", set_post($this->description));
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));

        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    /* Save all */
    function saveCopyDialog()
    {
        $attrs = array("cn","description");
        foreach($attrs as $attr){
            if(isset($_POST[$attr])){
                $this->$attr = get_post($attr);
            }
        }
    }

    /* Return plugin informations for acl handling  */ 
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Mime type generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gotoMimeType' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(), 'mimetypeManagement')
                        ),
                    "plCategory"    => array("mimetypes" => array("description"  => _("Mime types"),
                            "objectClass"  => "gotoMimeType")),

                    "plProperties" =>
                    array(
                        array(
                            "name"          => "mimetypeRDN",
                            "type"          => "rdn",
                            "default"       => "ou=mime,",
                            "description"   => _("RDN for mime type storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_mimetypeRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)),

            "plProvidedAcls"=> array(
                    "cn"                          => _("Name"),
                    "gotoMimeGroup"               => _("Mime group"),
                    "description"                 => _("Description"),
                    "base"                        => _("Base"),
                    "gotoMimeApplication"         => _("Application"),
                    "gotoMimeLeftClickAction"     => _("Left click action"),
                    "gotoMimeIcon"                => _("Icon"),
                    "gotoMimeFilePattern"         => _("File patterns"),
                    "gotoMimeEmbeddedApplication" => _("Embedded applications"))
                ));

    }


    function is_release()
    {
        if(is_object($this->parent->parent)){
            return($this->parent->parent->IsReleaseManagementActivated());
        }else{
            return($this->config->pluginEnabled("faiManagement"));
        }
        return(FALSE);
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $source_o = new mimetype($this->config,$source['dn'],$this->parent);

        foreach(array("gotoMimeLeftClickAction_Q","gotoMimeEmbeddedApplication","gotoMimeApplication","gotoMimeFilePattern","gotoMimeIcon","iconData") as $name){
            $use_attr     = "use_".$name;
            if(isset($this->$use_attr)){
                $this->$use_attr= $source_o->$use_attr;
            }
            $this->$name = $source_o->$name;
        } 
        foreach($this->attributes as $name){
            $this->$name = $source_o->$name;
        }

        if($this->iconData){
            $this->use_gotoMimeIcon ="Not emtpy, causes icon to be written.";
        }
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
