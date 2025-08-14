<?php
class workstartup extends plugin
{
    /* Ldap server list */
    var $gotoLdapServers    = array();
    var $gotoLdapServerList = array();
    var $gotoLdap_inherit   = FALSE;

    /* Generic terminal attributes */
# This is disabled as long there is no possiblilty to set these parameters
    var $gotoBootKernel       = "default-inherited";
    var $gotoKernelParameters = "";
    var $gotoLdapServer       = "default-inherited";
    var $gotoModules          = array();
    var $gotoAutoFs           = array();
    var $gotoFilesystem       = array();
    var $gotoTerminalPath     = "";
    var $gotoBootKernels      = array();

    /* attribute list for save action */
    var $attributes           = array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters", 
            "FAIclass", "FAIstatus", "gotoShare","FAIdebianMirror", "FAIrelease");
    var $objectclasses        = array("GOhard", "FAIobject");

    /* Share */
    var $gotoShares         = array();// Currently Share Option
    var $gotoShare          = "";     // currently selected Share Option
    var $gotoShareSelections= array();// Available Shares for this account in Listbox format
    var $gotoAvailableShares= array();// Available Shares for this account

    /* Helper */
    var $orig_dn            = "";
    var $ignore_account     = TRUE;

    /* FAI class selection */ 
    var $FAIclass           = array();  // The currently selected classes 
    var $FAIrelease           = "";
    var $FAIdebianMirror      = "auto";
    var $si_active            = FALSE;
    var $si_fai_action_failed = FALSE;

    var $cache              = array(); // Used as cache in fai mehtods

    var $FAIstatus          = "";
    var $FAIclasses         = array();

    var $view_logged        = FALSE;

    /* FAI class selection */
    var $InheritedFAIclass       = array();
    var $InheritedFAIrelease     = "";
    var $InheritedFAIdebianMirror= "auto";

    var $CopyPasteVars    = array("gotoModules","gotoShares");
    var $fai_activated    = FALSE;
    var $o_group_dn       = "";
    var $member_of_ogroup = FALSE;

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        /* Check if FAI is active */
        if($config->pluginEnabled("faiManagement")){
            $this->fai_activated = TRUE;
        }else{
            $this->attributes = array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters", "gotoShare");
            $this->objectclasses  = array("GOhard");
        }

        plugin::__construct ($config, $dn, $parent);

        /* Check for si daemon */
        $this->si_active = $this->config->get_cfg_value("core","gosaSupportURI") != "";

        /* Check object group membership */
        if(!isset($this->parent->by_object['ogroup'])){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd ($this->config->current['BASE']);
            $ldap->search("(&(objectClass=gotoWorkstationTemplate)(member=".LDAP::prepare4filter($this->dn)."))",array("cn","dn"));
            if($ldap->count()){
                $this->member_of_ogroup = TRUE;
                $attrs = $ldap->fetch();
                $this->o_group_dn = $attrs['dn'];
            }

            if ($ldap->count() > 1) { 
                $ogroups = array(); 
                array_push($ogroups, $attrs['dn']); 
                while ($attrs = $ldap->fetch()) { 
                    array_push($ogroups, $attrs['dn']); 
                } 
                $str = ""; 
                foreach($ogroups as $dn) { 
                    $str .= "<li>".$dn."</li>"; 
                } 
                msg_dialog::display(_("Warning"), sprintf(_("This system is member of more than one object group, object group inheritence will not work!")."<br><ul>%s</ul>", $str), WARNING_DIALOG); 
            } 
        }

        /* Creating a list of valid Mirrors 
         * none will not be saved to ldap.
         */
        $ldap   = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ui = get_userinfo();
        foreach($this->config->data['SERVERS']['LDAP'] as $dn => $data){
            if($ui->get_category_permissions($data['dn'],"server",TRUE)){
                for($i = 0; $i < $data['goLdapBase']['count']; $i ++){
                    $name = $data["cn"][0].":".$data["goLdapBase"][$i];
                    $this->gotoLdapServerList[]= $name; 
                }
            }
        }

        /* Get list of assigned ldap servers 
         */ 
        if(isset($this->attrs['gotoLdapServer'])){
            unset($this->attrs['gotoLdapServer']['count']);
            sort($this->attrs['gotoLdapServer']);
            foreach($this->attrs['gotoLdapServer'] as $value){
                $this->gotoLdapServers[] = preg_replace("/^[0-9]*:/","",$value);
            }
        } 
        natcasesort($this->gotoLdapServerList);

        if(!count($this->gotoLdapServers) && $this->member_of_ogroup){ 
            $this->gotoLdap_inherit = TRUE;
        }

        /* FAI Initialization
           Skip this if FAI is not activated 
         */
        if($this->fai_activated) {

            /* Parse used FAIclasses (stored as string).
             * The single classes are seperated by ' '.
             * There is also the release type given, after first
             *  occurrence of ':'.
             */
            $this->FAIclass =array();
            if(isset($this->attrs['FAIclass'][0])){
                $tmp = explode(" ",$this->attrs['FAIclass'][0]);
                $tmp2 =array();  

                foreach($tmp as $class){
                    if( ":" == $class[0] ) {
                        $this->FAIrelease = trim(substr($class, 1));
                    }else{
                        $tmp2[$class] = $class;
                    }
                }
                $this->FAIclass = $tmp2;
            }
        }

        /* Get arrays */
        foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
            if (isset($this->attrs["$val"]["count"])){
                for ($i= 0; $i<$this->attrs["count"]; $i++){
                    if (isset($this->attrs["$val"][$i])){
                        array_push($this->$val, $this->attrs["$val"][$i]);
                    }
                }
            }
            sort ($this->$val);
            $this->$val= array_unique($this->$val);
        }

        /* Prepare Shares */
        if((isset($this->attrs['gotoShare']))&&(is_array($this->attrs['gotoShare']))){
            unset($this->attrs['gotoShare']['count']);
            foreach($this->attrs['gotoShare'] as $share){
                $tmp = $tmp2 = array();
                $tmp = explode("|",$share);
                $tmp2['server']      =$tmp[0];
                $tmp2['name']        =$tmp[1];
                $tmp2['mountPoint']  =$tmp[2];
                $this->gotoShares[$tmp[1]."|".$tmp[0]]=$tmp2;
            }
        }

        $this->gotoShareSelections= $config->getShareList(true);
        $this->gotoAvailableShares= $config->getShareList(false);
        $tmp2 = array();


        $this->orig_dn= $this->dn;

        /* Handle inheritance value "default" */
        if ($this->member_of_ogroup){
            $this->gotoBootKernels= array("default-inherited" => '['._("inherited").']'); 
        }

        /* If we are member in an object group,
         *  we have to handle inherited values.
         * So you can see what is inherited.
         */
        if ($this->member_of_ogroup){

            if(count($this->FAIclass)==0 && $this->FAIrelease == ""){
                $this->FAIdebianMirror = "inherited";
            }

            if($this->fai_activated){
                $map= array("gotoBootKernel","FAIclass","FAIdebianMirror");
            }else{
                $map= array("gotoBootKernel");
            }

            $ldap = $this->config->get_ldap_link();
            $ldap->cat($this->o_group_dn);
            $attrs= $ldap->fetch();

            foreach ($map as $name){
                if (!isset($attrs[$name][0])){
                    continue;
                }

                switch ($name){
                    case 'gotoBootKernel':
                        $this->gotoBootKernels['default-inherited']=  _("inherited").' ['.$attrs[$name][0].']' ;
                        break;

                    case 'FAIclass':
                        $str = explode(":",$attrs[$name][0]);
                        $this->InheritedFAIclass    = explode(" ",trim($str[0]));
                        $this->InheritedFAIrelease  = trim($str[1]);
                        break;

                    case 'FAIdebianMirror':
                        $this->InheritedFAIdebianMirror = $attrs[$name][0];
                        break;
                }
            }
        }

        $this->update_fai_cache();

        if($this->fai_activated && !$this->si_fai_action_failed && $this->si_active){

            /* Check if the current mirror is available 
             */
            if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror])){
                if(count($this->FAIclass)){
                    msg_dialog::display(_("Error"), sprintf(_("FAI mirror '%s' is not available - setting to mirror 'auto'!"), $this->FAIdebianMirror), ERROR_DIALOG);
                }
                $this->FAIdebianMirror = "auto";
                $this->FAIrelease = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
                $this->cache['CLASSES'] = array();
                $this->update_fai_cache();
            }

            /* Check if the current mirror is available 
             */
            if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){
                $new_release = key($this->cache['SERVERS'][$this->FAIdebianMirror]); 
                if(count($this->FAIclass)){
                    msg_dialog::display(_("Error"), sprintf(_("FAI release '%s' is not available on mirror '%s' - setting to release '%s'!"), $this->FAIrelease, $this->FAIdebianMirror,$new_release), ERROR_DIALOG);
                }
                $this->FAIrelease = $new_release;
                $this->cache['CLASSES'] = array();
                $this->update_fai_cache();
            }
        }


        // Prepare lists
        $this->ldapList = new sortableListing(array(),array(), TRUE);
        $this->ldapList->setDeleteable(true);
        $this->ldapList->setEditable(false);
        $this->ldapList->setWidth("100%");
        $this->ldapList->setHeight("100px");
        $this->ldapList->setHeader(array(_("LDAP server")));
        $this->ldapList->setDefaultSortColumn(0);

        $this->faiList = new sortableListing(array(),array(), TRUE);
        $this->faiList->setDeleteable(true);
        $this->faiList->setEditable(false);
        $this->faiList->setWidth("100%");
        $this->faiList->setHeight("160px");
        $this->faiList->setColspecs(array('50%','50%'));
        $this->faiList->setHeader(array(_("Name"),_("Classes")));
        $this->faiList->setDefaultSortColumn(0);

    }


    function check()
    {
        $messages = array();

        /* Call common method to give check the hook */
        $messages= plugin::check();

        /* If there are packages selected, but no mirror show error */   
        if(($this->FAIdebianMirror == "none")&&(count($this->FAIclass)>0)){
            $messages[]=_("Please select a 'FAI server' or remove the 'FAI classes'.");
        }

        return($messages);
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        $theme = getThemeName();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","workstation/".get_class($this),$this->dn);
        }

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            switch ($theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
            }
            $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("workstation"))."</div>";
            return ($display);
        }

        /* Add module */
        if (isset ($_POST['add_module'])){
            if ($_POST['module'] != "" && $this->acl_is_writeable("gotoModules")){
                $this->add_list ($this->gotoModules, get_post('module'));
            }
        }

        /* Delete module */
        if (isset ($_POST['delete_module'])){
            if (count($_POST['modules_list']) && $this->acl_is_writeable("gotoModules")){
                $this->del_list ($this->gotoModules, get_post('modules_list'));
            }
        }

        /* FAI class management */
        if($this->fai_activated){
            if(((isset($_POST['AddClass']))&&(isset($_POST['FAIclassesSel']))) && ($this->acl_is_writeable("FAIclass"))){
                $found = 0 ; 

                /* If this new class/profile will attach a second partition table
                 * to our list of classes, abort and show a message.
                 */
                foreach($this->FAIclass as $name){
                    if(isset($this->FAIclassInfo[$name])){
                        foreach($this->FAIclassInfo[$name] as $atr){
                            if(isset($atr['obj'])){
                                if($atr['obj'] == "FAIpartitionTable"){
                                    $found ++ ; 
                                }
                            }
                        }
                    }
                }

                if((isset($this->FAIclassInfo[get_post('FAIclassesSel')]['FAIpartitionTable']))&&($found>0)){
                    msg_dialog::display(_("Error"), _("There is already a profile containing a partition table in your configuration!") , ERROR_DIALOG);
                }else{
                    $this->FAIclass[get_post('FAIclassesSel')]=get_post('FAIclassesSel');
                }
            }


            $this->faiList->save_object();
            $action = $this->faiList->getAction();
            if($action['action'] == 'delete'){
                $id = $this->faiList->getKey($action['targets'][0]);
                unset($this->FAIclass[$id]);
            }
            if($action['action'] == 'reorder'){
                $this->FAIclass= $this->faiList->getMaintainedData();
            }

            /* Move one used class class one position up or down */
            if($this->acl_is_writeable("FAIclass")){
                foreach($_POST as $name => $val){


                    if(preg_match("/fai_remove/i",$name)){
                        $value = postDecode(preg_replace("/fai_remove_/i","",$name));
                        unset($this->FAIclass[$value]);
                    }
                }
            }

            /* Delete selected class from our list */
            if($this->acl_is_writeable("FAIclass")){
                if((isset($_POST['DelClass']))&&(isset($_POST['FAIclassSel']))){
                    if(isset($this->FAIclass[get_post('FAIclassSel')])){
                        unset($this->FAIclass[get_post('FAIclassSel')]);
                    }
                }
            }
        }// END fai handling

        /* Show main page */
        $smarty= get_smarty();

        /* Assign ACLs to smarty */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        } 

        $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

        /* In this section server shares will be defined
         * A user can select one of the given shares and a mount point
         *  and attach this combination to his setup.
         */
        $smarty->assign("gotoShareSelections",    set_post($this->gotoShareSelections));
        $smarty->assign("gotoShareSelectionKeys", set_post(array_flip($this->gotoShareSelections)));

        /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry
         * This entry will be, a combination of mountPoint and sharedefinitions
         */
        if((isset($_POST['gotoShareAdd'])) && isset($_POST['gotoShareSelection']) && ($this->acl_is_writeable("gotoShare"))) {
            /* We assign a share to this user, if we don't know where to mount the share */
            if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
                msg_dialog::display(_("Error"), msgPool::required(_("Mount point")), ERROR_DIALOG);
            }else{

                if(isset($this->gotoAvailableShares[get_post('gotoShareSelection')])){
                    $a_share = $this->gotoAvailableShares[get_post('gotoShareSelection')];
                    $s_mount = get_post('gotoShareMountPoint');
                    /* Preparing the new assignment */
                    $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
                    $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
                }
            }
        }

        /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
         * If there is no defined share selected, we will abort the deletion without any message
         */
        if(($this->acl_is_writeable("gotoShare"))&& (isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare']))){
            unset($this->gotoShares[get_post('gotoShare')]);
        }

        $smarty->assign("gotoShares",       set_post($this->printOutAssignedShares()));
        $smarty->assign("gotoSharesCount",  count($this->printOutAssignedShares()));
        $smarty->assign("gotoShareKeys",    set_post(array_flip($this->printOutAssignedShares())));
        $smarty->assign("gotoBootKernels",  set_post($this->gotoBootKernels));

        $this->ldapList->setAcl($this->getacl('gotoLdapServer'));

        /* Add new ldap server to the list */
        if($this->acl_is_writeable("gotoLdapServer") && 
                !$this->gotoLdap_inherit && 
                isset($_POST['add_ldap_server']) && 
                isset($_POST['ldap_server_to_add'])){
            if(isset($this->gotoLdapServerList[get_post('ldap_server_to_add')])){
                $to_add = $this->gotoLdapServerList[get_post('ldap_server_to_add')];
                if(!in_array_strict($to_add,$this->gotoLdapServers)){
                    $this->gotoLdapServers[] = $to_add;
                }
            }
        }

        /* Move ldap servers up and down */
        $this->ldapList->save_object();
        $action = $this->ldapList->getAction();
        if($action['action'] == 'reorder'){
            $this->gotoLdapServers = array_values($this->ldapList->getMaintainedData());
        }
        if($action['action'] == 'delete'){
            $id = $this->ldapList->getKey($action['targets'][0]);
            $value = $this->gotoLdapServers[$id];
            $this->gotoLdapServers = array_remove_entries(array($value),$this->gotoLdapServers);
        }

        /* Add Entries */
        $data = $lData = array();
        foreach($this->gotoLdapServers as $key => $server){
            $data[$key]=$server;

            /* Announce missing entries */
            if(!in_array_strict($server,$this->gotoLdapServerList)){
                $server = $server."&nbsp;<font style='color:red'>(missing)</font>";
            }

            /* Convert old style entry */
            if (!preg_match('%:ldaps?://%', $server)){
                $server= "ldap://".preg_replace('/^([^:]+):/', '\1/', $server);
        } else {
            $server= preg_replace("/^[^:]+:/", "", $server);
            $lData[$key] = array('data'=>array($server));
        }    
        }
        $this->ldapList->setListData($data,$lData);
        $this->ldapList->update();


        if($this->gotoLdap_inherit){
            $smarty->assign("gotoLdapServerACL_inherit", preg_replace("/w/","",$this->getacl("gotoLdapServer")));;
        }else{
            $smarty->assign("gotoLdapServerACL_inherit", $this->getacl("gotoLdapServer"));
        }

        $list = array();
        foreach($this->gotoLdapServerList as $key => $entry){
            if(!in_array_strict($entry,$this->gotoLdapServers)){

                /* Convert old style entry */
                if (!preg_match('%:ldap[s]*://%', $entry)){
                    $entry= "ldap://".preg_replace('/^([^:]+):/', '\1/', $entry);

                /* Beautify new style entries */
            } else {
                $entry= preg_replace("/^[^:]+:/", "", $entry);
            }

            $list[$key] = $entry;
            }
        }
        $smarty->assign("gotoLdapServers",    $this->ldapList->render());
        $smarty->assign("gotoLdapServerList", $list);
        $smarty->assign("gotoLdap_inherit",   $this->gotoLdap_inherit);

        foreach (array("gotoModules", "gotoAutoFs", "gotoFilesystem") as $val){
            $smarty->assign("$val", set_post($this->$val));
        }

        /* Values */
        foreach(array("gotoBootKernel","gotoShare","FAIclasses","FAIclass","FAIdebianMirror","FAIrelease") as $val){
            $smarty->assign($val, set_post($this->$val));
        }

        $smarty->assign("fai_activated",$this->fai_activated);

        /* Create FAI output */
        $this->update_fai_cache();
        $smarty->assign("si_fai_action_failed",$this->si_fai_action_failed);
        $smarty->assign("si_active",$this->si_active);

        if(!$this->si_fai_action_failed && $this->si_active && $this->fai_activated){

            $smarty->assign("FAIservers"  ,    set_post($this->cache['SERVERS']));
            $smarty->assign("FAIdebianMirror", set_post($this->FAIdebianMirror));
            $smarty->assign("FAIrelease"  ,    set_post($this->FAIrelease));
            $smarty->assign("FAIclasses"  ,    set_post($this->selectable_classes()));

            /* Get classes for release from cache.
             * Or build cache
             */
            if($this->FAIdebianMirror == "inherited"){
                $release = $this->InheritedFAIrelease;
            }else{
                $release = $this->FAIrelease;
            }

            $smarty->assign("gotoBootKernels", set_post($this->cache['KERNELS'][$release]));
            $smarty->assign("InheritedFAIrelease", set_post($this->InheritedFAIrelease));

            /* Get classes */
            if($this->FAIdebianMirror == "inherited"){
                $tmp = $this->InheritedFAIclass;
            }else{
                $tmp = $this->FAIclass;
            }

            /* Get invalid classes */
            $invalid = $this->get_invalid_classes($tmp);
            $data = $lData = array();

            /* Draw every single entry */
            $i = 1;
            if($this->acl_is_readable("FAIclass")){
                foreach($tmp as $class){

                    /* Mark invalid classes. (Not in selected release)
                     */
                    $marker = "";
                    if(in_array_ics($class,$invalid)){
                        $marker = "&nbsp;<font color='red'>("._("Not available in current setup").")</font>";
                    }

                    $i ++ ; 

                    /* Get Description tag 
                     *  There may be several FAI objects with the same class name, 
                     *   use the description from FAIprofile, if possible.
                     */  
                    $desc = ""; 

                    $objects = array(
                            "Pl"  => array("IMG"=> "plugins/fai/images/fai_partitionTable.png"),
                            "Pt"     => array("IMG"=> "plugins/fai/images/fai_packages.png"),
                            "S"          => array("IMG"=> "plugins/fai/images/fai_script.png"),
                            "V"        => array("IMG"=> "plugins/fai/images/fai_variable.png"),
                            "H"            => array("IMG"=> "plugins/fai/images/fai_hook.png"),
                            "P"         => array("IMG"=> "plugins/fai/images/fai_profile.png"),
                            "T"        => array("IMG"=> "plugins/fai/images/fai_template.png"));

                    if(isset($this->cache['CLASSES'][$this->FAIrelease][$class])){
                        $desc ="";
                        foreach($this->cache['CLASSES'][$this->FAIrelease][$class] as $types ){
                            if(isset($types['Abbr'])){
                                $desc.= image($objects[$types['Abbr']]['IMG']);
                                if($types['Type'] == "FAIprofile"){
                                    break;
                                }
                            }
                        }
                    }
                    if(!empty($desc)){
                        $desc = "&nbsp;".trim($desc)."";
                    }        

                    $data[$class] = $class;
                    $lData[$class] = array('data' => array($class,$desc));
                }  
            }// END FAI output generation 
        }// END FAI output generation 

        $this->faiList->setAcl('rwcdm');
        $this->faiList->setReorderable($this->FAIdebianMirror != "inherited");
        $this->faiList->setListData($data,$lData);
        $this->faiList->update();

        $smarty->assign("FAIScriptlist", $this->faiList->render());

        /* Show main page */
        $smarty->assign("gotoKernelParameters",set_post($this->gotoKernelParameters));
        return($smarty->fetch (get_template_path('workstationStartup.tpl', TRUE,dirname(__FILE__))));
    }


    function remove_from_parent()
    {
        $this->handle_post_events("remove");
        new log("remove","workstation/".get_class($this),$this->dn);
    }


    /* Save data to object */
    function save_object()
    {
        $old_mirror  = $this->FAIdebianMirror;
        plugin::save_object();

        /* Update release */
        if($old_mirror != $this->FAIdebianMirror){
            if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){
                $this->FAIrelease      = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
            }
        }

        if(isset($_POST['WorkstationStarttabPosted'])){
            if(isset($_POST['gotoLdap_inherit'])){
                $this->gotoLdap_inherit = TRUE;
            }else{
                $this->gotoLdap_inherit = FALSE;
            }
        }
    }


    /* Save to LDAP */
    function save()
    {

        /* Depending on the baseobject (Ogroup / WS) we
         *  use another set of objectClasses
         * In case of WS itself, we use  "array("GOhard", "FAIobject");"
         * if we are currently editing from ogroup menu we use (array("gotWorkstationTemplate","GOhard", "FAIobject"))
         */
        if(isset($this->parent->by_object['ogroup'])){
            $this->objectclasses = array("gotoWorkstationTemplate");
        }elseif(isset($this->parent->by_object['workgeneric'])){
            $this->objectclasses = array("GOhard");
        }elseif(isset($this->parent->by_object['servgeneric'])){
            $this->objectclasses = array("GOhard","gotoWorkstationTemplate");
        }else{
            msg_dialog::display(_("Fatal error"),
                    "Object Type Configuration is unknown. Please contact the GOsa developers.",
                    FATAL_ERROR_DIALOG);
            exit();
        }

        /* Append FAI class */
        if($this->fai_activated){
            $this->objectclasses[]  = "FAIobject";
        }

        /* Find proper terminal path for tftp configuration
FIXME: This is suboptimal when the default has changed to
another location! */
        if (($this->gotoTerminalPath == "default")){
            $ldap= $this->config->get_ldap_link();

            /* Strip relevant part from dn, keep trailing ',' */
            $tmp= preg_replace("/^cn=[^,]+,".preg_quote(get_ou("termgeneric", "terminalRDN"), '/')."/i", "", $this->dn);
            $tmp= preg_replace("/".$this->config->current['BASE']."$/i", "", $tmp);

            /* Walk from top to base and try to load default values for
               'gotoTerminalPath'. Abort when an entry is found. */
            while (TRUE){
                $tmp= preg_replace ("/^[^,]+,/", "", $tmp);

                $ldap->cat("cn=default,".get_ou("termgeneric", "terminalRDN").$tmp.
                        $this->config->current['BASE'], array('gotoTerminalPath'));
                $attrs= $ldap->fetch();
                if (isset($attrs['gotoTerminalPath'])){
                    $this->gotoTerminalPath= $attrs['gotoTerminalPath'][0];
                    break;
                }

                /* Nothing left? */
                if ($tmp == ""){
                    break;
                }
            }
        }

        plugin::save();

        unset( $this->attrs['FAIrelease'] );
        $str = "";

        /* Skip FAI attribute handling if not necessary */
        if($this->fai_activated && !$this->si_fai_action_failed){
            if($this->FAIdebianMirror == "inherited"){
                $this->attrs['FAIclass'] = $this->attrs['FAIrelease'] =  $this->attrs['FAIdebianMirror'] = array();
            }else{
                foreach($this->FAIclass as $class){
                    $str .= $class." ";
                }
                $str = trim($str);
                if(empty($this->attrs['FAIclass'])){
                    $this->attrs['FAIclass'] = array();
                }else{
                    $this->attrs['FAIclass']= $str." :".$this->FAIrelease;
                }
            }
        }

        /* Add missing arrays */
        foreach (array("gotoFilesystem", "gotoAutoFs", "gotoModules") as $val){
            if (isset ($this->$val) && count ($this->$val) != 0){

                $this->attrs["$val"]= array_unique($this->$val);
            }
            if(!isset($this->attrs["$val"])) $this->attrs["$val"]=array();
        }

        /* Prepare list of ldap servers */
        $this->attrs['gotoLdapServer'] = array();
        if(!$this->gotoLdap_inherit){
            $i = 0;
            foreach($this->gotoLdapServers as $server){
                $i ++;
                $this->attrs['gotoLdapServer'][] = $i.":".$server;
            }
        }

        if ($this->attrs['gotoBootKernel'] == "default-inherited"){
            $this->attrs['gotoBootKernel']= array();
        }

        /* if mirror == none stop saving this attribute */
        if($this->FAIdebianMirror == "none"){
            $this->FAIdebianMirror = "";
        }

        /* Get FAIstate from object, the generic tab could have changed it during execute */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);


        /* Skip FAI attribute handling if not necessary */
        if($this->fai_activated && !$this->si_fai_action_failed && $this->si_active){
            $ldap->cat($this->dn,array("FAIstate"));
            $checkFAIstate = $ldap->fetch();

            /* Remove FAI objects if no FAI class is selected */ 
            if((count($this->FAIclass)==0) && (!isset($checkFAIstate['FAIstate']))){
                $this->attrs['FAIclass']        = array();
                $this->attrs['FAIdebianMirror'] = array();
            }
        }else{

            /* Don't touch FAI objects if something went wrong with the si daemon.
             */
            if(isset($this->attrs['FAIclass'])) unset($this->attrs['FAIclass']);
            if(isset($this->attrs['FAIdebianMirror'])) unset($this->attrs['FAIdebianMirror']);
        }

        /* prepare share settings */
        $tmp = array();
        foreach($this->gotoShares as $name => $settings){
            $tmp2= explode("|",$name);
            $name = $tmp2[0];
            $tmp[] = $settings['server']."|".$name."|".$settings['mountPoint'];
        }
        $this->attrs['gotoShare']=$tmp;
        $this->cleanup();
        $ldap->modify ($this->attrs); 
        new log("modify","workstation/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, self::class));
        }
        $this->handle_post_events("modify");

        /* Check if LDAP server has changed */
        if ($this->si_active && (isset($this->attrs['gotoLdapServer']) && class_available("DaemonEvent") || $this->gotoLdap_inherit)){
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
            if(isset($events['TRIGGERED']['DaemonEvent_reload_ldap_config'])){
                $evt = $events['TRIGGERED']['DaemonEvent_reload_ldap_config'];
                $macs = array();

                /* Get list of macAddresses 
                 */
                if(isset($this->parent->by_object['ogroup'])){

                    /* If we are an object group, add all member macs 
                     */
                    $p = $this->parent->by_object['ogroup'];
                    foreach($p->memberList as $dn => $obj){
                        if(isset($p->objcache[$dn]['macAddress']) && !empty($p->objcache[$dn]['macAddress'])){
                            $macs[] = $p->objcache[$dn]['macAddress'];
                        }
                    }
                }elseif(isset($this->parent->by_object['workgeneric']->netConfigDNS->macAddress)){

                    /* We are a workstation. Add current mac.
                     */
                    $mac = $this->parent->by_object['workgeneric']->netConfigDNS->macAddress;
                    if(!empty($mac)){
                        $macs[] = $mac;
                    }          
                }elseif(isset($this->parent->by_object['servgeneric']->netConfigDNS->macAddress)){

                    /* We are a server. Add current mac.
                     */
                    $mac = $this->parent->by_object['servgeneric']->netConfigDNS->macAddress;
                    if(!empty($mac)){
                        $macs[] = $mac;
                    }          
                }

                /* Trigger event for all member objects 
                 */
                foreach($macs as $mac){
                    $tmp = new $evt['CLASS_NAME']($this->config);
                    $tmp->set_type(TRIGGERED_EVENT);
                    $tmp->add_targets(array($mac));
                }
            }
        }
    }


    /* Add value to array, check if unique */
    function add_list (&$array, $value)
    {
        if ($value != ""){
            $array[]= $value;
            sort($array);
            array_unique ($array);
        }
    }


    /* Delete value to array, check if unique */
    function del_list (&$array, $list)
    {
        $tmp= array();
        foreach ($array as $mod){
            if (!in_array_strict($mod, $list)){
                $tmp[]= $mod;
            }
        }
        $array= $tmp;
    }

    /* Generate ListBox frindly output for the defined shares
     * Possibly Add or remove an attribute here,
     */
    function printOutAssignedShares()
    {
        $a_return = array();
        if(is_array($this->gotoShares)){
            foreach($this->gotoShares as $share){
                $a_return[$share['name']."|".$share['server']]= $share['name']." [".$share['server']."]";
            }
        }
        return($a_return);
    }



    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);    
        $source_o = new workstartup ($this->config, $source['dn']);
        foreach(array("FAIclass","gotoModules", "gotoAutoFs", "gotoFilesystem",
                    "gotoKernelParameters","gotoShares","gotoLdapServers","gotoLdapServerList",
                    "gotoLdap_inherit") as $attr){
            $this->$attr = $source_o->$attr;
        }
    }


    function array_switch_item($ar,$from,$to)
    {
        if(!is_array($ar)){
            return(false);
        }
        if(!isset($ar[$from])){
            return(false);
        }
        if(!isset($ar[$to])){
            return(false);
        }

        $tmp = $ar[$from];
        $ar[$from] = $ar[$to];    
        $ar[$to] = $tmp;    
        return($ar);
    }


    /* Return plugin informations for acl handling */ 
    static function plInfo()
    {
        return (array( 
                    "plShortName"   => _("Startup"),
                    "plDescription" => _("System startup"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 9,
                    "plSection"     => array("administration"),           
                    "plCategory"    => array("workstation","server","ogroups"),

                    "plProvidedAcls"=> array(
                        "gotoLdapServer"        => _("LDAP server"),
                        "gotoBootKernel"        => _("Boot kernel"),
                        "gotoKernelParameters"  => _("Kernel parameter"),

                        "gotoModules"           => _("Kernel modules"),
                        "gotoShare"             => _("Shares"),

                        "FAIclass"              => _("FAI classes"),
                        "FAIdebianMirror"       => _("Debian mirror"),
                        "FAIrelease"            => _("Debian release"),

                        "FAIstatus"             => _("FAI status flag")) // #FIXME is this acl realy necessary ?
                        ));
    }


    /* Updates release dns 
     *  and reads all classes for the current release, 
     *  if not already done ($this->cache).
     */
    function update_fai_cache($first_call = FALSE)
    {
        $force = FALSE;
        if(!$this->si_active) return; 
        $start = microtime(TRUE);  

        if($this->si_fai_action_failed && !isset($_POST['fai_si_retry'])) return;

        $this->si_fai_action_failed = FALSE;

        /* Get the list of available servers and their releases. 
         */
        if($force || !isset($this->cache['SERVERS'])){
            $this->si_fai_action_failed = TRUE;
            $this->cache = array();
        }

        /* Ensure that our selection is valid, else we get several PHP warnings 
           if there is no FAI configuration at all.
         */
        if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror])){
            $this->cache['SERVERS'][$this->FAIdebianMirror][''] ='';
        }

        /* Build up arrays, without checks */
        if(!$first_call){

            /* Check if the selected mirror is available */
            if(!isset($this->cache['SERVERS'][$this->FAIdebianMirror])){
                $this->FAIdebianMirror = "auto";
                $this->FAIrelease      = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
                trigger_error("There was a problem with the selected FAIdebianMirror. This mirror ('".$this->FAIdebianMirror."') is not available");
            }

            /* Check if the selected release is available */
            if($this->FAIdebianMirror != "inherited" && !isset($this->cache['SERVERS'][$this->FAIdebianMirror][$this->FAIrelease])){

                if($this->FAIrelease != ""){
                    trigger_error("There was a problem with the selected FAIrelease. This release ('".$this->FAIrelease."') is not available");
                }
                $this->FAIrelease = key($this->cache['SERVERS'][$this->FAIdebianMirror]);
            }
        }

        /* Get classes for release from cache. 
         * Or build cache
         */
        if($this->FAIdebianMirror == "inherited"){
            $release = $this->InheritedFAIrelease;
        }else{
            $release = $this->FAIrelease;
        }

        if($force || !isset($this->cache['CLASSES'][$release]) && $release != ""){

            /* Get the list of available servers and their releases.
             */
            $this->cache['CLASSES'][$release] = array();
            $this->si_fai_action_failed = TRUE;
            $this->cache=array();
            return;

            /* Add object caught from external hook
             */
            $lines= $this->GetHookElements();
            foreach ($lines as $hline){
                $entries= explode(";", $hline);
                $server = $entries['0'];
                $url    = $entries['1'];
                if (!empty($url)){

                    /* Split releases */
                    if (isset($entries[2])){
                        $releases= explode(",", $entries[2]);

                        foreach ($releases as $release_data){
                            $release_c  = preg_replace('/:.*$/', '', $release_data);
                            $sections_c = explode(':', preg_replace('/^[^:]+:([^|]+)|.*$/', '\1', $release_data));
                            $classes_c  = explode('|', preg_replace('/^[^|]+\|(.*)$/', '\1', $release_data));

                            if($release_c == $release){
                                $this->cache['SERVERS'][$url][$release_c]=$release_c;
                                $this->cache['SERVERS']['auto'][$release_c]=$release_c; 
                                foreach ($classes_c as $class){
                                    if ($class != ""){
                                        $this->cache['CLASSES'][$release_c][$class]= array();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            uksort($this->cache['SERVERS'], 'strnatcasecmp');

            /* Only add inherit option, if we are part in an object group
             */
            if($this->member_of_ogroup){
                $this->cache['SERVERS'] = array_merge(array('inherited' => array()),$this->cache['SERVERS']);
            }
        }

        /* Get list of available kernel for this release 
         */
        if(!isset($this->cache['KERNELS'])) $this->cache['KERNELS'] = array();

        if($force || !isset($this->cache['KERNELS'][$release])){
            $this->cache['KERNELS'][$release] = array();
            foreach($this->gotoBootKernels as $name => $default){
                $this->cache['KERNELS'][$release][$name] = $default;
            }
            ksort($this->cache['KERNELS'][$release]);
        }
    }


    /* This function return an array containing all 
     *  invalid classes for the selected server/release
     */
    function get_invalid_classes($classes)
    {
        $this->update_fai_cache();
        if($this->FAIdebianMirror == "inherited" && isset($this->cache['CLASSES'][$this->InheritedFAIrelease])){
            $release_classes = $this->cache['CLASSES'][$this->InheritedFAIrelease];
        }elseif(isset($this->cache['CLASSES'][$this->FAIrelease])){
            $release_classes = $this->cache['CLASSES'][$this->FAIrelease];
        }else{
            $release_classes = array();
        }


        /* Detect all classes that are not valid 
         *  for the selected release 
         */
        $NA = array();
        foreach($classes as $class){
            if(!isset($release_classes[$class])){
                $NA[] = $class;
            }
        }
        return($NA);
    }  


    /* Get all selectable classes for the ui select box
     */
    function selectable_classes()
    {
        $this->update_fai_cache();

        if($this->FAIdebianMirror == "inherited" && isset($this->cache['CLASSES'][$this->InheritedFAIrelease])){
            $classes = $this->cache['CLASSES'][$this->InheritedFAIrelease];
        }elseif(isset($this->cache['CLASSES'][$this->FAIrelease])){
            $classes = $this->cache['CLASSES'][$this->FAIrelease];
        }else{
            $classes = array();
        }

        $Abbr ="";
        $ret= array();
        foreach($classes as $class_name => $class_types){
            if(!in_array_strict($class_name,$this->FAIclass)){
                $Abbr = "";
                foreach($class_types as $type){
                    if(!preg_match("/".$type['Abbr']."/",$Abbr)){
                        $Abbr .= $type['Abbr']." ";
                    }
                }
                $ret[$class_name] = trim($Abbr);
            }
        }
        uksort($ret, 'strnatcasecmp');
        return($ret);
    }


    /* Analyse FAI object and return an array with usefull informations like 
     *  FAIobject type.
     */
    function analyse_fai_object($attr)
    {
        $tmp = array();
        switch($attr['TYPE']){

            case 'FAIpackageList':
                $tmp["Type"]= 'FAIpackageList';
                $tmp["Abbr"]= 'Pl';
                break;
            case 'FAItemplate': 
                $tmp["Type"]= 'FAItemplate'; 
                $tmp["Abbr"]= 'T'; 
                break;
            case 'FAIvariable':
                $tmp["Type"]= 'FAIvariable'; 
                $tmp["Abbr"]= 'V'; 
                break;
            case 'FAIscript':
                $tmp["Type"]= 'FAIscript'; 
                $tmp["Abbr"]= 'S'; 
                break;
            case 'FAIhook':
                $tmp["Type"]= 'FAIhook'; 
                $tmp["Abbr"]= 'H'; 
                break;
            case 'FAIpartitionTable':
                $tmp["Type"]= 'FAIpartitionTable'; 
                $tmp["Abbr"]= 'Pt'; 
                break;
            case 'FAIprofile':
                $tmp["Type"]= 'FAIprofile'; 
                $tmp["Abbr"]= 'P'; 
                break;
            default: trigger_error("Unknown FAI object type!");;
        }
        return($tmp);
    }


    /* Return repository hook output, if possible.
     */
    function GetHookElements()
    {
        $ret = array();
        $cmd= $this->config->get_cfg_value("servrepository", "repositoryBranchHook");
        if(!empty($cmd)){
            $res = shell_exec($cmd);
            $res2 = trim($res);
            if((!$res)){
                msg_dialog::display(_("Configuration error"), msgPool::cmdexecfailed("repositoryBranchHook", $cmd), ERROR_DIALOG);
            }elseif(empty($res2)){
                msg_dialog::display(_("Configuration error"), _("'repositoryBranchHook' returned no result!"), ERROR_DIALOG);
            }else{
                $tmp = preg_split("/\n/",$res);
                foreach($tmp as $line){
                    if(empty($line)) continue;
                    $ret[]= $line;
                }
            }
        }
        return($ret);
    }


    /* This function creates the release name out of a dn 
     *  e.g. "ou=1.0rc2,ou=siga,ou=fai,..." => "siga/1.0rc2"
     */
    function dn_to_release_name($dn)
    {
        $relevant = preg_replace("/,".preg_quote(get_ou("faiManagement", "faiBaseRDN"), '/').".*$/i","",$dn);
        $parts    = array_reverse(explode(",",$relevant));
        $str ="";
        foreach($parts as $part){
            $str .= preg_replace("/^ou=/","",$part)."/";
        }
        return(preg_replace("/\/$/","",$str)); 
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
