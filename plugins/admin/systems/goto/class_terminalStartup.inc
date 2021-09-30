<?php
class termstartup extends plugin
{
    /* Generic terminal attributes */
    var $goLdapServerList= array();
    var $gotoBootKernel= "default-inherited";
    var $gotoKernelParameters= "";
    var $gotoLdapServer= "default-inherited";
    var $gotoModules= array();
    var $gotoTerminalPath= "";
    var $gotoBootKernels= array();

    /* Ldap server list */
    var $gotoLdapServers    = array();
    var $gotoLdapServerList = array();
    var $gotoLdap_inherit   = FALSE;

    /* Share */
    var $gotoShares         = array();// Currently Share Option
    var $gotoShare          = "";     // currently selected Share Option
    var $gotoShareSelections= array();// Available Shares for this account in Listbox format
    var $gotoAvailableShares= array();// Available Shares for this account


    /* attribute list for save action */
    var $attributes= array("gotoLdapServer", "gotoBootKernel", "gotoKernelParameters","gotoModules");
    var $objectclasses= array("GOhard");
    var $view_logged = FALSE;
    var $member_of_ogroup = FALSE;
    var $o_group_dn       = "";

    /* Helper */
    var $orig_dn= "";
    var $ignore_account= TRUE;

    function __construct(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::__construct ($config, $dn, $parent);

        $this->gotoBootKernels = array("default"=> ("default"));

        /* Get arrays */
        foreach (array("gotoModules") as $val){
            $this->$val = array();
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

        $this->orig_dn= $this->dn;

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

        if(isset($this->attrs['gotoLdapServer'])){
            unset($this->attrs['gotoLdapServer']['count']);
            sort($this->attrs['gotoLdapServer']);
            foreach($this->attrs['gotoLdapServer'] as $value){
                $this->gotoLdapServers[] = preg_replace("/^[0-9]*:/","",$value);
            }
        }
        natcasesort($this->gotoLdapServerList);

        /* Load hardware list */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(|(objectClass=gotoTerminalTemplate)(objectClass=gotoWorkstationTemplate))(member=".LDAP::prepare4filter($this->dn)."))");
        if ($ldap->count() == 1){
            $map= array("gotoLdapServer","gotoBootKernel");
            $attrs= $ldap->fetch();
            $this->member_of_ogroup = TRUE;
            $this->o_group_dn = $attrs['dn'];

            foreach ($map as $name){
                if (!isset($attrs[$name][0])){
                    continue;
                }
                switch ($name){
                    case 'gotoBootKernel':
                        /* Handle inheritance value "default" */
                        if ($this->member_of_ogroup){
                            $this->gotoBootKernels["default-inherited"]= _("inherited")." [".$attrs['gotoBootKernel'][0]."]";
                        }
                        break;
                    case 'gotoLdapServer':
                        $this->goLdapServerList= array_merge(array('default-inherit' => _("inherited").' ['.$attrs[$name][0].']' ), $this->goLdapServerList);
                        break;
                }
            }
        }

        if(!count($this->gotoLdapServers) && $this->member_of_ogroup){
            $this->gotoLdap_inherit = TRUE;
        }

        /* Get list of boot kernels */
        if (isset($this->config->data['TABS'])){
            $command= $this->config->get_cfg_value('termstartup', "systemKernelsHook");

            if (!check_command($command)){
                $message[]= sprintf(_("Command '%s', specified as systemKernelsHook for plug-in '%s' doesn't seem to exist."), $command,
                        get_class($this));
            } else {
                $fh= popen($command, "r");
                while (!feof($fh)) {
                    $buffer= trim(fgets($fh, 256));

                    if(!empty($buffer)){

                        $name=$value = $buffer;

                        if(preg_match("/:/",$buffer)){
                            $name = preg_replace("/:.*$/","",$buffer);
                            $value= preg_replace("/^.*:/","",$buffer);
                            $this->gotoBootKernels[$name]= $name.":".$value;
                        }else{
                            $this->gotoBootKernels[$name]= $value;
                        }
                    }
                }
                pclose($fh);
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

    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","terminal/".get_class($this),$this->dn);
        }

        /* Do we need to flip is_account state? */
        if (isset($_POST['modify_state'])){
            $this->is_account= !$this->is_account;
        }

        /* Do we represent a valid terminal? */
        if (!$this->is_account && $this->parent === NULL){
            $display= "<img alt=\"\" src=\"images/small-error.png\" align=middle>&nbsp;<b>".
                msgPool::noValidExtension(_("terminal"))."</b>";
            return ($display);
        }

        /* Add module */
        if (isset ($_POST['add_module'])){
            if ($_POST['module'] != "" && $this->acl_is_writeable("gotoMode")){
                $this->add_list ($this->gotoModules, $_POST['module']);
            }
        }

        /* Delete module */
        if (isset ($_POST['delete_module'])){
            if (count($_POST['modules_list']) && $this->acl_is_writeable("gotoMode")){
                $this->del_list ($this->gotoModules, $_POST['modules_list']);
            }
        }

        /* Show main page */
        $smarty= get_smarty();

        /* Assign acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        $smarty->assign("member_of_ogroup",$this->member_of_ogroup);

        /* In this section server shares will be defined
         * A user can select one of the given shares and a mount point
         *  and attach this combination to his setup.
         */
        $smarty->assign("gotoShareSelections",    $this->gotoShareSelections);
        $smarty->assign("gotoShareSelectionKeys", array_flip($this->gotoShareSelections));
        $smarty->assign("gotoBootKernels",$this->gotoBootKernels);

        /* if $_POST['gotoShareAdd'] is set, we will try to add a new entry
         * This entry will be, a combination of mountPoint and sharedefinitions
         */
        if(isset($_POST['gotoShareAdd']) && $this->acl_is_writeable("gotoShare")){
            /* We assign a share to this user, if we don't know where to mount the share */
            if((!isset($_POST['gotoShareMountPoint']))||(empty($_POST['gotoShareMountPoint']))||(preg_match("/[\|]/i",$_POST['gotoShareMountPoint']))){
                msg_dialog::display(_("Error"), msgPool::invalid(_("Mount point")), WARNING_DIALOG);
            }elseif(isset($_POST['gotoShareSelection']) && isset($this->gotoAvailableShares[$_POST['gotoShareSelection']])){
                $a_share = $this->gotoAvailableShares[$_POST['gotoShareSelection']];
                $s_mount = $_POST['gotoShareMountPoint'];
                /* Preparing the new assignment */
                $this->gotoShares[$a_share['name']."|".$a_share['server']]=$a_share;
                $this->gotoShares[$a_share['name']."|".$a_share['server']]['mountPoint']=$s_mount;
            }
        }

        /* if the Post  gotoShareDel is set, someone asked GOsa to delete the selected entry (if there is one selected)
         * If there is no defined share selected, we will abort the deletion without any message
         */
        if((isset($_POST['gotoShareDel']))&&(isset($_POST['gotoShare'])) && $this->acl_is_writeable("gotoShare")){
            unset($this->gotoShares[$_POST['gotoShare']]);
        }

        $smarty->assign("gotoShares",$this->printOutAssignedShares());
        $smarty->assign("gotoSharesCount",count($this->printOutAssignedShares()));
        $smarty->assign("gotoShareKeys",array_flip($this->printOutAssignedShares()));


        /* Add new ldap server to the list */
        if(!$this->gotoLdap_inherit && isset($_POST['add_ldap_server']) && isset($_POST['ldap_server_to_add'])){
            if(isset($this->gotoLdapServerList[$_POST['ldap_server_to_add']])){
                $to_add = $this->gotoLdapServerList[$_POST['ldap_server_to_add']];
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

        $this->ldapList->setAcl($this->getacl('gotoLdapServer'));

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
                $list[$key] = $entry;
            }
        }
        $smarty->assign("gotoLdapServers",    $this->ldapList->render());
        $smarty->assign("gotoLdapServerList", $list);
        $smarty->assign("gotoLdap_inherit",   $this->gotoLdap_inherit);
        $smarty->assign("JS",  session::get('js'));

        foreach (array("gotoModules" ) as $val){
            $smarty->assign("$val", $this->$val);
        }

        /* Values */
        foreach(array("gotoBootKernel", "gotoKernelParameters") as $val){
            $smarty->assign($val, $this->$val);
        }

        /* Show main page */
        return($smarty->fetch (get_template_path('terminalStartup.tpl', TRUE,dirname(__FILE__))));
    }

    function remove_from_parent()
    {
        if($this->acl_is_removeable()){
            $this->handle_post_events("remove");
            new log("remove","terminal/".get_class($this),$this->dn,array_keys($this->attrs));
        }
    }


    /* Save data to object */
    function save_object()
    {
        plugin::save_object();

        if(isset($_POST['TerminalStarttabPosted'])){
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
         * In case of TS itself, we use  "array("GOhard");"
         * if we are currently editing from ogroup menu we use (array("goTerminalTemplate"))
         */
        if(isset($this->parent->by_object['ogroup'])){
            $this->objectclasses = array("gotoTerminalTemplate");
        }elseif(isset($this->parent->by_object['termgeneric'])){
            $this->objectclasses = array("GOhard");
        }else{
            msg_dialog::display(_("Fatal error"),
                    "Object Type Configuration is unknown. Please contact the GOsa developers.",
                    FATAL_ERROR_DIALOG);
            exit();
        }


        plugin::save();

        /* Add missing arrays */
        foreach (array("gotoModules") as $val){
            if (isset ($this->$val) && count ($this->$val) != 0){

                $this->attrs["$val"]= array_unique($this->$val);
            }
            if(!isset($this->attrs["$val"])){
                $this->attrs["$val"]=array();
            }
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

        /* Strip out 'default' values */
        if ($this->attrs['gotoBootKernel'] == "default-inherited"){
            $this->attrs['gotoBootKernel']= array();
        }

        /* prepare share settings */
        $tmp = array();
        foreach($this->gotoShares as $name => $settings){
            $tmp2 = explode("|",$name);
            $name = $tmp2[0];
            $tmp[] = $settings['server']."|".$name."|".$settings['mountPoint'];
        }
        $this->attrs['gotoShare']=$tmp;

        /* Write back to ldap */
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);

        $this->cleanup();
        $ldap->modify ($this->attrs); 

        new log("modify","terminal/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }
        $this->handle_post_events("modify");
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

        $source_o = new termstartup ($this->config, $source['dn']);

        foreach(array("gotoModules", "gotoKernelParameters","gotoShare",
                    "gotoKernelParameters","gotoShares",
                    "goLdapServerList","gotoBootKernel","gotoLdapServer",
                    "gotoBootKernels","gotoLdapServers","gotoLdapServers",
                    "gotoLdapServerList","gotoLdap_inherit","gotoShareSelections",
                    "gotoAvailableShares") as $attr){
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
                    "plDescription" => _("Terminal startup"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 5,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("terminal"),

                    "plProperties" => array(

                        array(
                            "name"          => "systemKernelsHook",
                            "type"          => "command",
                            "default"       => "",
                            "description"   => _("Script to check for available kernels"),
                            "check"         => "gosaProperty::isCommand",
                            "migrate"       => "",
                            "group"         => "system",
                            "mandatory"     => FALSE)
                        ),

                    "plProvidedAcls"=> array(
                            "gotoLdapServer"      => _("LDAP server"),
                            "gotoShare"           => _("Shares"),
                            "gotoModules"         => _("Kernel modules"),
                            "gotoBootKernel"      => _("Boot kernel"), 
                            "gotoKernelParameters"=> _("Kernel parameter"))
                        ));
    }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
