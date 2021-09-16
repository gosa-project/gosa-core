<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_serverService.inc 21155 2012-05-08 13:39:31Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

define("START_SERVICE",1);
define("STOP_SERVICE",2);
define("RESTART_SERVICE",3);

define("SERVICE_STOPPED",_("Stop"));
define("SERVICE_STARTED",_("Start"));
define("SERVICE_RESTARTED",_("Restart"));

define("ALL_SERVICES",100);

class ServerService extends management
{
    /* attribute list for save action */
    var $ignore_account   = TRUE;
    var $skipFooter       = TRUE;
    var $skipHeader       = TRUE;
    var $plugins          = array();
    var $pluign_names     = array();
    var $current          = "";
    var $dialog           = FALSE;
    var $read_only        = FALSE;
    var $backup           = NULL;
    var $acl              ;
    var $cn;
    var $parent           ;
    var $pl_notify = FALSE;
    var $acl_category = "";
    var $acl_base = "";

    var $is_account;

    function __construct (&$config, $dn,$parent)
    {
        $this->config = $config;
        $this->parent = $parent;
        $this->ui     = get_userinfo();
        $this->dn= $dn;
        if(isset($this->parent->attrs)){
            $this->attrs = $this->parent->attrs;
        }

        // Set storage points - We do not have any - We just create a fake list which lists all services 
        $this->storagePoints = array("");

        // Initialize list of used and useable services.
        foreach ($config->data['TABS']['SERVERSERVICE'] as $plug){
            if(class_available($plug['CLASS'])){

                $name= $plug['CLASS'];
                $this->plugin_names[]= $name;
                $this->plugins[$name]= new $name($config, $dn, $this);

                /* Initialize kerberos key support */ 
                if(isset($this->plugins[$name]->krb_service_prefix) && !empty($this->plugins[$name]->krb_service_prefix)){
                    $this->plugins[$name]->krb_host_key = &$this->parent->kerberos_key_service;
                }

                /* Capture all service objectClases, necessary for acl handling */ 
                if(isset($this->plugins[$name]->objectclasses)){
                    foreach($this->plugins[$name]->objectclasses as $oc){
                        $this->objectclasses[] = $oc;
                    }
                }
            }
        }
        $this->rebuildListing();
    }


    function rebuildListing()
    {
        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("serverService-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);


        // Load service xml file and fill in placeholders
        $contents =file_get_contents(get_template_path("serverService-list.xml", true));
        $template = "
            <action>
            <name>instantNew_%s</name>
            <type>entry</type>
            <label>%s</label>
            </action>";
        $serviceList = "";

        // Sort entries by alphabet 
        $list = array();
        $services = $this->getAllUnusedServices();
        foreach($services as $name => $desc){
            $list[_($desc).$name] = $name;
        }
        ksort($list);

        // Create menu entries -> create -> services[]
        foreach($list as $name){
            $desc = $services[$name];
            $serviceList.= sprintf($template, $name,_($desc));
        }

        if(!empty($serviceList)){
            $serviceList = "
              <action>
                 <type>sub</type>
                 <image>images/lists/element.png[new]</image>
                 <label>Create</label>
                 {$serviceList} 
                </action>
            ";

         }

        $contents = preg_replace("/%SERVICES%/",$serviceList, $contents);
        $headpage = new listing($contents,TRUE);
        $headpage->registerElementFilter("filterServiceStatus", "ServerService::filterServiceStatus");
        $headpage->setFilter($filter);

        parent::__construct($this->config, $this->ui, "services", $headpage);
        $this->registerAction("new",                "newService");
        $this->registerAction("remove",             "removeService");
        $this->registerAction("saveService",        "saveService");
        $this->registerAction("cancelService",      "cancelEdit");
        $this->registerAction("restart",            "updateServiceStatus");
        $this->registerAction("stop",               "updateServiceStatus");
        $this->registerAction("start",              "updateServiceStatus");
        $this->registerAction("instantNew",         "instantNewService");
    }


    /*! \brief    Filter extra POST and GET variables for this plugin.
     */ 
    function detectPostActions()
    {
        $action = management::detectPostActions();
        if(isset($_POST['SaveService']))      $action['action'] = "saveService";
        if(isset($_POST['CancelService']))    $action['action'] = "cancelService";
        if(preg_match("/^instantNew_/",$action['action'])){
            $action['targets'][] = preg_replace("/^instantNew_/i","",$action['action']);
            $action['action'] = "instantNew";
        }
        return($action);
    }


    /*! \brief  Edit an existing service here.
     *          Somebody clicked on the paper and pencil icon.
     */
    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $this->dialog = TRUE;
        $s_entry = array_pop($target);
        if($this->plugins[$s_entry]->acl_is_readable("")){
            $this->backup   = get_object_vars($this->plugins[$s_entry]);
            $this->dialogObject   = $this->plugins[$s_entry];
            $this->current  = $s_entry;
        }
    }


    /*! \brief  Editing an object was caneled.
     *          Close dialogs/tabs and remove locks.
     */
    protected function cancelEdit()
    {
        if($this->backup == NULL && $this->current){
            $this->plugins[$this->current] = new $this->current($this->config,$this->dn,$this);
            $this->plugins[$this->current]-> set_acl_base($this->acl_base);
            $this->plugins[$this->current]-> set_acl_category(preg_replace("/\/$/","",$this->acl_category));

        }elseif(is_array($this->backup)){
            foreach($this->backup as $name => $value){
                $this->plugins[$this->current]->$name = $value;
            }
        }
        $this->backup   = NULL;
        $this->current  = "";
        $this->rebuildListing();
        $this->closeDialogs();
    }


    /*! \brief  Create the selected service.
     */
    function instantNewService($action, $targets, $all)
    {
        $this->closeDialogs();
        $serv = $targets[0];
        $this->plugins[$serv]->is_account = true;
        $this->dialogObject  = $this->plugins[$serv];
        $this->current = $serv;
        $this->dialog = TRUE;
    }


    /*! \brief  Save the currently edited service.
     */
    function saveService()
    {
        if(!is_object($this->dialogObject)) return;
        $this->dialogObject->save_object();
        $msgs = $this->dialogObject->check();
        if(count($msgs)){
            foreach($msgs as $msg){
                msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
            }
        }else{
            $this->plugins[$this->current] = $this->dialogObject;
            $tmp  = get_object_vars($this->dialogObject);
            foreach($tmp as $name => $value){
                $this->plugins[$this->current]->$name = $value;
            }
            $this->closeDialogs();
            $this->backup = NULL;
            $this->rebuildListing();
        }
    }


    /*!\brief   Close all opened dialogs 
     *          And reset "dialog open" flags to display bottom buttons again.
     */
    function closeDialogs()
    {
        $dnKeeper = $this->dn;
        management::closeDialogs();
        $this->dialog = FALSE;
        $this->dn = $dnKeeper;
    }


    /*! \brief    Remove the selected service(s)
     */
    protected function removeService($action="",$target=array(),$all=array())
    {
        foreach($target as $s_entry){
            $new_obj = new $s_entry($this->config,$this->dn, $this);
            $new_obj->set_acl_base($this->acl_base);
            $new_obj->set_acl_category(preg_replace("/\/$/","",$this->acl_category));
            $tmp     = $new_obj->getListEntry();

            /* Check if we are allowed to remove this service
             */
            if($tmp['AllowRemove']){
                $str = $this->plugins[$s_entry]->allow_remove();
                if(empty($str)){
                    $this->plugins[$s_entry] = $new_obj;
                    $this->plugins[$s_entry]->is_account = false;
                }else{
                    msg_dialog::display(_("Error"), $str, ERROR_DIALOG);
                }
            }
        }
    }


    /*! \brief    Returns the list of of services, active services.
     *            Used in the filter class for services. 
     *             class_filterServerService.inc
     */
    static function getServiceList()
    { 
        return(session::get('ServerService'));
    }


    /*! \brief    Returns an image for the service status.
     *            Used in the service list class.
     */
    static function filterServiceStatus($a,$b,$c,$d)
    {
        $img ="";
        if(isset($d['0'])){
            switch($d[0]){
                case 'Restart' : $img = ('images/status_restarting.png'); break;
                case 'Stop' : $img = ('images/lists/off.png'); break;
                case 'Start' : $img = ('images/lists/on.png'); break;
            }
        }
        if(!empty($img)){
            return('<img src=\''.$img.'\'>');
        }
        return("&nbsp;");  
    }


    /*! \brief  Execute this plugin
     *          Handle actions/events, locking, snapshots, dialogs, tabs,...
     */
    function execute()
    {

        // Ensure that html posts and gets are kept even if we see a 'Entry islocked' dialog.
        $vars = array('/^act$/','/^listing/','/^PID$/','/^FILTER_PID$/');
        session::set('LOCK_VARS_TO_USE',$vars);

        /* Display the copy & paste dialog, if it is currently open */
        $ret = $this->copyPasteHandler("",array());
        if($ret){
            return($this->getHeader().$ret);
        }

        // Update filter
        $filter = $this->getFilter();
        if ($filter) {
            $filter->update();
            session::global_set(get_class($this)."_filter", $filter);
            session::set('autocomplete', $filter);
        }

        // Handle actions (POSTs and GETs)
        $str = $this->handleActions($this->detectPostActions());
        if($str) return($this->getHeader().$str);

        // Open single dialog objects
        if(is_object($this->dialogObject)){
            if(method_exists($this->dialogObject,'save_object')) $this->dialogObject->save_object();
            if(method_exists($this->dialogObject,'execute')){

                /* There is currently a subdialog open, display this dialog */
                $add ="";
                if(isset($this->dialogObject->krb_host_key) && $this->dialogObject->krb_host_key instanceof krbHostKeys){
                    $add = $this->dialogObject->krb_host_key->save_object_by_prefix($this->dialogObject->krb_service_prefix);
                    $add = $this->dialogObject->krb_host_key->execute_by_prefix($this->dialogObject->krb_service_prefix,TRUE);
                }

                $display = $add.$this->dialogObject->execute();
                $display.= $this->_getTabFooter();
                return($this->getHeader().$display);
            }
        }

        // Display tab object.
        if($this->tabObject instanceOf tabs || $this->tabObject instanceOf multi_plug){
#      $this->tabObject->save_object();
            $display = $this->tabObject->execute();
            $display.= $this->_getTabFooter();
            return($this->getHeader().$display);
        }

        // Set current restore base for snapshot handling.
        if(is_object($this->snapHandler)){
            $bases = array();
            foreach($this->storagePoints as $sp){
                $bases[] = $sp.$this->headpage->getBase();
            }

            // No bases specified? Try base
            if(!count($bases)) $bases[] = $this->headpage->getBase();

            $this->snapHandler->setSnapshotBases($bases);
        }

        // Display list
        session::set('ServerService', $this->plugins);
        return($this->renderList());
    }


    /*! \brief    Updates the status for a list of services.
     */
    function updateServiceStatus($action, $target,$all)
    {
        /* Handle state changes for services */
        $map =  array(    
                "start"    =>  START_SERVICE ,
                "stop"     =>  STOP_SERVICE ,
                "restart"  =>  RESTART_SERVICE );
        foreach($target as $s_entry){
            if(isset($map[$action])){
                $type     = $map[$action];
                $service  = $s_entry;
                $this->ServiceStatusUpdate($type,$service);
            }
        }
    }


    /*! \brief   Returns a list of all used services
     *            CLASSNAME => _($this->plugins[*]->DisplayName);   
     */
    function getAllUsedServices()
    {
        $ret = array();
        foreach($this->plugins as $name => $obj){
            if($obj->is_account){
                if(isset($obj->DisplayName)){
                    $ret[$name] = $obj->DisplayName;
                }else{
                    $ret[$name] = $name;
                }
            }
        }
        return($ret);
    }


    /*! \brief    Returns a list of all unused services.
     */
    function getAllUnusedServices()
    {
        $tmp = $this->getAllUsedServices();
        $pool_of_ocs =array();
        foreach($tmp as $name => $value){
            if(isset($this->plugins[$name]->conflicts)){
                $pool_of_ocs[]= get_class($this->plugins[$name]);
            }
        }

        $ret = array();
        foreach($this->plugins as $name => $obj){

            /* Skip all pluigns that will lead into conflicts */
            $skip = false;
            if(isset($obj->conflicts)){
                foreach($obj->conflicts as $oc){
                    if(in_array_ics($oc,$pool_of_ocs)){
                        $skip = true;
                    }
                }
            }

            if(!$obj->acl_is_createable()){
                $skip = true;
            }

            if(!$skip){
                if(isset($obj->DisplayName)){
                    $ret[$name] = $obj->DisplayName;
                }else{
                    $ret[$name] = $name;
                }
            }
        }
        return($ret);
    }


    /*! \brief    Updates the status of a service and 
     *             calls an external hook if specified in gosa.conf
     */
    function ServiceStatusUpdate($method , $service)
    {
        /* Skip if this is a new server */
        if($this->dn == "new"){
            msg_dialog::display(_("Information"), _("Cannot update service status until it has been saved!"), INFO_DIALOG);
            return;
        }

        $action = "";
        if($method == START_SERVICE){
            $action = SERVICE_STARTED;
        }elseif($method== STOP_SERVICE){
            $action = SERVICE_STOPPED;
        }elseif($method == RESTART_SERVICE){
            $action = SERVICE_RESTARTED;
        }else{
            msg_dialog::display(_("Error"), _("Cannot update service status!"), ERROR_DIALOG);
            return;
        }

        $caseVars = array("cn","dn");
        if($service == ALL_SERVICES){
            foreach($this->plugins as $name => $obj){
                foreach($caseVars as $var){
                    if(isset($this->$var)){
                        $this->plugins[$name]->$var = $this->$var;  
                    }
                }

                /* check if services can be restarted */
                $map =array(SERVICE_STARTED=> "AllowStart" ,
                        SERVICE_STOPPED => "AllowStop",
                        SERVICE_RESTARTED => "AllowRestart");

                /* get plugins informations, restart/start/stop actions allowed ?*/
                $tmp = $this->plugins[$name]->getListEntry();

                /* Check if given action is allowed for this service */
                if($tmp[$map[$action]] && !empty($this->plugins[$name]->StatusFlag)){
                    if($this->plugins[$name]->initially_was_account && $this->plugins[$name]->is_account){
                        $this->plugins[$name]->setStatus($action);
                    }
                }
            }
        }else{
            foreach($caseVars as $var){
                if(isset($this->$var)){
                    $this->plugins[$service]->$var = $this->$var;  
                }
            }
            if($this->plugins[$service]->is_account){
                $this->plugins[$service]->setStatus($action);
            }
        }
    }


    /*! \brief    No checks here.
     */
    function check()
    {   
        return(array());
    }


    /*! \brief    Keep posted form values in opened dialogs
     */
    function save_object()
    {
        foreach($this->plugins as $name => $obj){
            if($obj->is_account){
                $this->plugins[$name]->save_object();
            }
        }
    }


    /*! \brief    Remove all active services
     */
    function remove_from_parent()
    {
        $caseVars = array("cn","dn");
        foreach($this->plugins as $name => $obj){
            foreach($caseVars as $var){
                if(isset($this->$var)){
                    $this->plugins[$name]->$var = $this->$var;  
                }
            }
            if($this->plugins[$name]->initially_was_account){
                if(isset($this->plugins[$name]->krb_host_key) && $this->plugins[$name]->krb_host_key instanceof krbHostKeys){
                    $this->plugins[$name]->krb_host_key->remove_from_parent_by_prefix($this->plugins[$name]->krb_service_prefix);
                }
                $this->plugins[$name]->remove_from_parent();
            }
        }
    }


    /*! \brief    Save all active services
     */
    function save()
    {
        $caseVars = array("cn","dn");
        foreach($this->plugins as $name => $obj){

            foreach($caseVars as $var){
                if(isset($this->$var)){
                    $this->plugins[$name]->$var = $this->$var;
                }
            }

            if($this->plugins[$name]->is_account){
                $this->plugins[$name]->save();
            }else{
                if($this->plugins[$name]->initially_was_account){
                    $this->plugins[$name]->remove_from_parent();
                }
            }
        }
    }


    /*! \brief    Prepare active services to be copied.
     */
    function PrepareForCopyPaste($source)
    {
        foreach($this->plugins as $name => $plugin){
            $this->plugins[$name]->PrepareForCopyPaste($source);
        }
    }


    /*! \brief    Check if there is an active service which prevents 
     *             complete service removal.
     */
    function allow_remove()
    {
        foreach($this->plugins as $name => $obj){
            $str = $obj->allow_remove();
            if(!empty($str)){
                return($str);
            }
        }
    }


    /*! \brief    Forward plugin acls 
     */
    function set_acl_base($base)
    {
        $this->acl_base = $base;
        foreach($this->plugins as $name => $obj){
            $this->plugins[$name]->set_acl_base($base);
        }
    }


    /*! \brief    Forward plugin acls 
     */
    function set_acl_category($category)
    {
        $this->acl_category = $category;
        foreach($this->plugins as $name => $obj){
            $this->plugins[$name]->set_acl_category($category);
        }
    }


    function saveCopyDialog(){
    }


    function getCopyDialog(){
        return(array("string"=>"","status"=>""));
    }


}
?>
