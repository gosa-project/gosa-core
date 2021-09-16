<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_systemManagement.inc 21155 2012-05-08 13:39:31Z hickert $$
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


#
# NewDevices 
# -> Edit
#     |- SelectDeviceType()        (Allows to select target SystemType and OGroup)
# -> Save
#     |-> systemTypeChosen()       (Queue entry to be activated) 
#     |-> handleActivationQueue()  (Now acticvate queued objects)
#       |-> Ogroup selected   
#       | |-> Try to adapt values from ogroup and save directly.
#       |
#       |-> NO Ogroup selected
#       |  |-> Open dialogs of the target system type and allow modifcations.
#       |
#       |->activate_new_device()   (Finally activate the device - FAIsate=..)
#
#
# NewArpDevices (NewUnknwonDevices )
# -> Edit
#     |-> editEntry - ArpNewDeviceTabs
# -> Save       
#     |-> NO gotoIntegeration 
#     | |-> Save DHCP and DNS entries, then remove the source entry from ldap.
#     |
#     |-> gotoIntegration selected (Handle object like a NewDevice now)
#     |-> systemTypeChosen()       (Queue entry to be activated)
#     |-> handleActivationQueue()  (Now acticvate queued objects)
#       |-> Ogroup selected
#       | |-> Try to adapt values from ogroup and save directly.
#       |
#       |-> NO Ogroup selected
#       |  |-> Open dialogs of the target system type and allow modifcations.
#       |
#       |->activate_new_device()   (Finally activate the device - FAIsate=..)
#
class systemManagement extends management
{
    var $plHeadline     = "Systems";
    var $plDescription  = "Manage systems, their services and prepare them for use with GOsa";
    var $plIcon  = "plugins/systems/images/plugin.png";

    // Tab definition 
    protected $tabClass = "";
    protected $tabType = "";
    protected $aclCategory = "";
    protected $aclPlugin   = "";
    protected $objectName   = "system";
    protected $objectInfo = array();
    protected $opsi = NULL;
    protected $activationQueue  = array();

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        // Set storage points 
        $tD = $this->getObjectDefinitions(); 
        $sP = array();
        foreach($tD as $entry){
            if(!empty($entry['ou']))
                $sP[] = $entry['ou'];
        }

        $this->storagePoints = array_unique($sP);

        // Build filter
        // #TODO: We got problems with this list when using it from session.
        // Somehow we got the Service list instead of the Systems list...
        // To reproduce, open a server and go to the services tab, no editing such 
        // stuff needed, it just has to be visible once.
        // Afterwards got back to the systems list, and take a look.. all systems
        // are gone
        if (FALSE && session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("system-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("system-list.xml", true));
        $headpage->registerElementFilter("systemRelease", "systemManagement::systemRelease");
        $headpage->registerElementFilter("filterSystemDescription", "systemManagement::filterSystemDescription");
        $headpage->registerElementFilter("filterLink", "systemManagement::filterLink");
        $headpage->setFilter($filter);
        $filter->setConverter('systemManagement::incomingFilterConverter');

        // Register Daemon Events
        if(class_available("DaemonEvent")){
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);
            foreach($events['TRIGGERED'] as $name => $data){
                $this->registerAction("T_".$name,"handleEvent");
                $this->registerAction("S_".$name,"handleEvent");
            }
            $this->registerAction("activateMultiple","activateMultiple");
        }
        $this->registerAction("saveEvent","saveEventDialog");
        $this->registerAction("createISO","createISO");
        $this->registerAction("initiateISOcreation","initiateISOcreation");
        $this->registerAction("performIsoCreation","performIsoCreation");
        $this->registerAction("systemTypeChosen","systemTypeChosen");
        $this->registerAction("handleActivationQueue","handleActivationQueue");

        $this->registerAction("new_device",           "newEntry");
        $this->registerAction("new_goServer",         "newEntry");
        $this->registerAction("new_gotoWorkstation",  "newEntry");
        $this->registerAction("new_gotoTerminal",     "newEntry");
        $this->registerAction("new_gotoPrinter",      "newEntry");
        $this->registerAction("new_goFonHardware",    "newEntry");
        $this->registerAction("new_ieee802Device",    "newEntry");
        $this->registerAction("new_FAKE_OC_OpsiHost", "newEntry");

        $this->registerAction("setPassword", "setPassword");
        $this->registerAction("passwordChangeConfirmed", "passwordChangeConfirmed");

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }

        $this->si_active = FALSE;

        // Check if we are able to communicate with the GOsa supprot daemon
        if(class_available("opsi")){
            $this->opsi = new opsi($this->config);
        }
        parent::__construct($config, $ui, "systems", $headpage);
    }


    /*! \brief  Act on password change requests.
     */
    function setPassword($action,$target)
    {
        if(count($target) == 1){
            $tDefs= $this->getObjectDefinitions();
            $headpage = $this->getHeadpage();
            $dn = array_pop($target);
            $type = $headpage->getType($dn);
            $entry = $headpage->getEntry($dn);
            $ui       = get_userinfo();
            $smarty = get_smarty();
            if(in_array_strict("FAKE_OC_PWD_changeAble", $entry['objectClass'])){
                $acl = $tDefs[$type]['aclCategory'].'/'.$tDefs[$type]['aclClass'];
                $tabacl   = $ui->get_permissions($dn,$acl,"userPassword");
                if(preg_match("/w/",$tabacl)){
                    $this->dn= $dn;
                    set_object_info($this->dn);
                    return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
                }else{
                    msg_dialog::display(_("Permission error"), _("You have no permission to change this password!"), ERROR_DIALOG);
                }
            }
        }
    }


    function getAclCategories()
    {
        $ret = array();
        $tDefs= $this->getObjectDefinitions();
        foreach($tDefs as $type => $data){
            $ret[] = $data['aclCategory'];
        }
        return(array_unique($ret));
    }


    /*! \brief  This method is used to queue and process copy&paste actions.
     *          Allows to copy, cut and paste mutliple entries at once.
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function copyPasteHandler($action="",$target=array(),$all=array(),
            $altTabClass ="", $altTabType = "", $altAclCategory="",$altAclPlugin="")
    {
        // Return without any actions while copy&paste handler is disabled.
        if(!is_object($this->cpHandler))  return("");

        // Save user input
        $this->cpHandler->save_object();

        // Add entries to queue
        if($action == "copy" || $action == "cut"){

            $tDefs= $this->getObjectDefinitions();
            $headpage = $this->getHeadpage();
            $ui       = get_userinfo();
            $this->cpHandler->cleanup_queue();
            foreach($target as $dn){

                $type = $headpage->getType($dn);
                $entry = $headpage->getEntry($dn);

                $aclCategory = $tDefs[$type]['aclCategory'];
                $aclPlugin = $tDefs[$type]['aclClass'];
                $tabClass = $tDefs[$type]['tabClass'];
                $tabType = $tDefs[$type]['tabDesc'];

                if($action == "copy" && $this->ui->is_copyable($dn,$aclCategory,$aclPlugin)){
                    $this->cpHandler->add_to_queue($dn,"copy",$tabClass,$tabType,$aclCategory,$this);
                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$dn,"Entry copied!");
                }
                if($action == "cut" && $this->ui->is_cutable($dn,$aclCategory,$aclPlugin)){
                    $this->cpHandler->add_to_queue($dn,"cut",$tabClass,$tabType,$aclCategory,$this);
                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$dn,"Entry cutted!");
                }
            }
        }

        // Initiate pasting
        if($action == "paste"){
            $this->cpPastingStarted = TRUE;
        }

        // Display any c&p dialogs, eg. object modifications required before pasting.
        if($this->cpPastingStarted && $this->cpHandler->entries_queued()){
            $headpage = $this->getHeadpage();
            $this->cpHandler->SetVar("base",$headpage->getBase());
            $data = $this->cpHandler->execute();
            if(!empty($data)){
                return($data);
            }
        }

        // Automatically disable pasting process since there is no entry left to paste.
        if(!$this->cpHandler->entries_queued()){
            $this->cpPastingStarted = FALSE;
        }
        return("");
    }


    /*! \brief  Password change confirmed, now try to change the systems pwd.
     */
    function passwordChangeConfirmed()
    {
        $tDefs= $this->getObjectDefinitions();
        $headpage = $this->getHeadpage();
        $type = $headpage->getType($this->dn);
        $entry = $headpage->getEntry($this->dn);
        $ui       = get_userinfo();
        $smarty = get_smarty();

        if(!in_array_strict('FAKE_OC_PWD_changeAble', $entry['objectClass'])){
            trigger_error("Tried to change pwd, for invalid object!");
        }elseif (get_post('new_password') != get_post('repeated_password')){
            msg_dialog::display(_("Error"), 
                    _("The passwords you've entered as 'New password' and 'Repeated password' do not   match!"), ERROR_DIALOG);
            return($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }else{
            $acl = $tDefs[$type]['aclCategory'].'/'.$tDefs[$type]['aclClass'];
            $tabacl   = $ui->get_permissions($this->dn,$acl,"userPassword");

            // Check acls
            if(!preg_match("/w/",$tabacl)){
                msg_dialog::display(_("Permission error"), _("You have no permission to change this password!"), ERROR_DIALOG);
            }else{
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($this->dn);
                $ldap->cat($this->dn);
                $old_attrs = $ldap->fetch();

                $attrs= array();
                if ($_POST['new_password'] == ""){

                    /* Remove password attribute
                     */
                    if(in_array_strict("simpleSecurityObject",$old_attrs['objectClass'])){
                        $attrs['objectClass'] = array();
                        for($i = 0 ; $i < $old_attrs['objectClass']['count'] ; $i ++){
                            if(!preg_match("/simpleSecurityObject/i",$old_attrs['objectClass'][$i])){
                                $attrs['objectClass'][] = $old_attrs['objectClass'][$i];
                            }
                        }
                    }
                    $attrs['userPassword']= array();
                } else {

                    /* Add/modify password attribute
                     */
                    if(!in_array_strict("simpleSecurityObject",$old_attrs['objectClass'])){
                        $attrs['objectClass'] = array();
                        for($i = 0 ; $i < $old_attrs['objectClass']['count'] ; $i ++){
                            $attrs['objectClass'][] = $old_attrs['objectClass'][$i];
                        }
                        $attrs['objectClass'][] = "simpleSecurityObject";
                    }

                    if(class_available("passwordMethodCrypt")){
                        $pwd_m = new passwordMethodCrypt($this->config);
                        $pwd_m->set_hash("crypt/md5");
                        $attrs['userPassword'] = $pwd_m->generate_hash(get_post('new_password'));
                    }else{
                        msg_dialog::display(_("Password method"),_("Password method crypt is missing. Cannot set system password."));
                        $attrs = array();
                    }
                }
                $ldap->modify($attrs);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, $type));
                }else{
                    if(class_available($tDefs[$type]['plugClass'])){
                        $plug = $tDefs[$type]['plugClass'];
                        $p = new $plug($this->config,$this->dn);
                        $p->handle_post_events("modify");
                    }
                }
                new log("security","systems/".get_class($this),$this->dn,array_keys($attrs),$ldap->get_error());
            }
            set_object_info();
        }
    }


    /*! \brief  The method gets called when somebody clicked the CD icon 
     *           in the system listing. 
     *          A confirmation will be shown to acknowledge the creation.
     */
    function createISO($action,$target)
    {
        if(count($target) == 1){
            $smarty = get_smarty();
            $this->dn= array_pop($target);
            set_object_info($this->dn);
            return ($smarty->fetch(get_template_path('goto/gencd.tpl', TRUE)));

        }
    }


    /*! \brief  Once the user has confirmed the ISO creation in 'createISO',
     *           this method gets called. 
     *          An iFrame is shown which then used 'performIsoCreation' as contents. 
     */
    function initiateISOcreation()
    {
        $smarty = get_smarty();
        $smarty->assign("src", "?plug=".$_GET['plug']."&amp;PerformIsoCreation");
        return ($smarty->fetch(get_template_path('goto/gencd_frame.tpl', TRUE)));  
    }


    /*! \brief  ISO creation confirmed and iFrame is visible, now create the ISO 
     *           and display the status to fill the iFrame.
     */
    function performIsoCreation()
    {
        $return_button   = "<form method='get' action='main.php' target='_parent'>
            <input type='submit' value='"._("Back")."'>
            <input type='hidden' name='plug' value='".$_GET['plug']."'/>
            </form>";

        $dsc   = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));

        /* Get and check command */
        $command= $this->config->get_cfg_value("workgeneric", "systemIsoHook");
        if (check_command($command)){
            @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");

            /* Print out html introduction */
            echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
                <html>
                <head>
                <title></title>
                <style type="text/css">@import url("themes/default/style.css");</style>
                <script language="javascript" src="include/focus.js" type="text/javascript"></script>
                </head>
                <body style="background: none; margin:4px;" id="body" >
                <pre>';

            /* Open process handle and check if it is a valid process */
            $process= proc_open($command." '".$this->dn."'", $dsc, $pipes);
            if (is_resource($process)) {
                fclose($pipes[0]);

                /* Print out returned lines && write JS to scroll down each line */
                while (!feof($pipes[1])){
                    $cur_dat = fgets($pipes[1], 1024);
                    echo $cur_dat;
                    echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
                    flush();
                }
            }

            /* Get error string && close streams */
            $buffer= stream_get_contents($pipes[2]);

            fclose($pipes[1]);
            fclose($pipes[2]);
            echo "</pre>";

            /* Check return code */
            $ret= proc_close($process);
            if ($ret != 0){
                echo "<h1 style='color:red'>"._("Creating the image failed. Please see the report below.")."</h1>";
                echo "<pre style='color:red'>$buffer</pre>";
            }
            echo $return_button."<br>";
        } else {
            $tmp= "<h1 style='color:red'>".sprintf(_("Command '%s', specified for ISO creation doesn't seem to exist."), $command)."</h1>";
            echo $tmp;
            echo $return_button."<br>";
        }

        /* Scroll down completly */
        echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
        echo '</body></html>';
        flush();
        exit;
    }


    /*! \brief    Handle GOsa-si events
     *            All schedules and triggered events are handled here.
     */
    function handleEvent($action="",$target=array(),$all=array())
    {
        // Detect whether this event is scheduled or triggered.
        $triggered = TRUE;
        if(preg_match("/^S_/",$action)){
            $triggered = FALSE;
        }

        // Detect triggere or scheduled actions 
        $headpage = $this->getHeadpage();
        $event = preg_replace("/^[TS]_/","",$action); 
        if(preg_match("/^[TS]_/", $action)){

            // Send special reinstall action for opsi hosts
            if($event == "DaemonEvent_reinstall" && $this->si_active && $this->opsi){
                foreach($target as $key => $dn){
                    $type = $headpage->getType($dn);

                    // Send Reinstall event for opsi hosts
                    if($type == "FAKE_OC_OpsiHost"){
                        $obj = $headpage->getEntry($dn);
                        $this->opsi->job_opsi_install_client($obj['cn'][0],$obj['macAddress'][0]);
                        unset($target[$key]);
                    }
                }
            }
        } 

        // Now send remaining FAI/GOsa-si events here.
        if(count($target) && $this->si_active){
            $mac= array();

            // Collect target mac addresses
            $ldap = $this->config->get_ldap_link();
            $tD = $this->getObjectDefinitions();
            $events = DaemonEvent::get_event_types(SYSTEM_EVENT);
            foreach($target as $dn){
                $type = $headpage->getType($dn);
                if($tD[$type]['sendEvents']){
                    $obj = $headpage->getEntry($dn);
                    if(isset($obj['macAddress'][0])){
                        $mac[] = $obj['macAddress'][0];
                    }
                }
            }


            // Prepare event to be added
            if(count($mac) && isset($events['BY_CLASS'][$event]) && $this->si_active){
                $event = $events['BY_CLASS'][$event];
                $this->dialogObject = new $event['CLASS_NAME']($this->config);
                $this->dialogObject->add_targets($mac);

                if($triggered){
                    $this->dialogObject->set_type(TRIGGERED_EVENT);
                    $this->closeDialogs();
                }else{
                    $this->dialogObject->set_type(SCHEDULED_EVENT);
                }
            }
        }
    }


    /*! \brief  Close all dialogs and reset the activationQueue.
     */ 
    function cancelEdit()
    {
        management::cancelEdit();
        $this->activationQueue = array();
    }


    /*! \brief  Save event dialogs. 
     *          And append the new GOsa-si event.
     */ 
    function saveEventDialog()
    {

            $this->closeDialogs();
    }


    /*! \brief    Update filter part for INCOMING.
     *            Allows us to search for "systemIncomingRDN".
     */
    static function incomingFilterConverter($filter)
    {
        $rdn = preg_replace("/^[^=]*=/", "", get_ou("ArpNewDevice", "systemIncomingRDN"));
        $rdn = preg_replace("/,.*$/","",$rdn);
        return(preg_replace("/%systemIncomingRDN/", $rdn,$filter));
    }


    /*! \brief    Queue selected objects to be removed. 
     *            Checks ACLs, Locks and ask for confirmation.
     */
    protected function removeEntryRequested($action="",$target=array(),$all=array())
    {
        // Close dialogs and remove locks for currently handled dns
        $this->cancelEdit();

        $disallowed = array();
        $this->dns = array();

        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$target,"Entry removel requested!");

        // Check permissons for each target
        $tInfo = $this->getObjectDefinitions();
        $headpage = $this->getHeadpage();
        foreach($target as $dn){
            $type = $headpage->getType($dn);
            if(!isset($tInfo[$type])){
                trigger_error("Unknown object type received '".$type."' please update systemManagement::getObjectDefinitions()!");
            }else{
                $info = $tInfo[$type];
                $acl = $this->ui->get_permissions($dn, $info['aclCategory']."/".$info['aclClass']);
                if(preg_match("/d/",$acl)){
                    $this->dns[] = $dn;
                }else{
                    $disallowed[] = $dn;
                }
            }
        }
        if(count($disallowed)){
            msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
        }

        // We've at least one entry to delete.
        if(count($this->dns)){

            // check locks
            if ($user= get_multiple_locks($this->dns)){
                return(gen_locked_message($user,$this->dns));
            }

            // Add locks
            $dns_names = array();
            $types = array();
            $h = $this->getHeadpage();

            // Build list of object -labels
            foreach($h->objectTypes as $type){
                $map[$type['objectClass']]= $type['label'];
            }

            foreach($this->dns as $dn){
                $tmp = $h->getType($dn);
                if(isset($map[$tmp])){
                    $dns_names[] = '('._($map[$tmp]).')&nbsp;-&nbsp;'.LDAP::fix($dn);
                }else{
                    $dns_names[] =LDAP::fix($dn);
                }
            }
            add_lock ($this->dns, $this->ui->dn);

            // Display confirmation dialog.
            $smarty = get_smarty();
            $smarty->assign("info", msgPool::deleteInfo($dns_names));
            return($smarty->fetch(get_template_path('removeEntries.tpl')));
        }
    }


    /*! \brief  Object removal was confirmed, now remove the requested entries.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function removeEntryConfirmed($action="",$target=array(),$all=array(),
            $altTabClass="",$altTabType="",$altAclCategory="", $aclPlugin="")
    {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$target,"Entry removel confirmed!");

        // Check permissons for each target
        $tInfo = $this->getObjectDefinitions();
        $headpage = $this->getHeadpage();
        $disallowed = array();
        foreach($this->dns as $key => $dn){
            $type = $headpage->getType($dn);
            if(!isset($tInfo[$type])){
                trigger_error("Unknown object type received '".$type."' please update systemManagement::getObjectDefinitions()!");
            }else{

                $info = $tInfo[$type];
                $acl = $this->ui->get_permissions($dn, $info['aclCategory']."/".$info['aclClass']);
                if(preg_match("/d/",$acl)){

                    // Delete the object
                    $this->dn = $dn;
                    if($info['tabClass'] == "phonetabs"){
                        $this->tabObject= new $info['tabClass']($this->config, $this->config->data['TABS'][$info['tabDesc']], $dn,$type);
                        $this->tabObject->set_acl_base($dn);
                        $this->tabObject->by_object['phoneGeneric']->remove_from_parent ();
                    }else{
                        $this->tabObject= new $info['tabClass']($this->config,$this->config->data['TABS'][$info['tabDesc']], 
                                $this->dn, $info['aclCategory'], true, true);
                        $this->tabObject->set_acl_base($this->dn);
                        $this->tabObject->parent = &$this;
                        $this->tabObject->delete ();
                    }

                    // Remove the lock for the current object.
                    del_lock($this->dn);

                }else{
                    $disallowed[] = $dn;
                    new log("security","system/".get_class($this),$dn,array(),"Tried to trick deletion.");
                }
            }
        }
        if(count($disallowed)){
            msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
        }

        // Cleanup
        $this->remove_lock();
        $this->closeDialogs();
    }


    /*! \brief  Edit the selected system type.
     *          NewDevice and ArpNewDevice are handled here separately 
     */
    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        if(count($target) == 1){
            $tInfo = $this->getObjectDefinitions();
            $headpage = $this->getHeadpage();
            $dn = $target[0];
            $type =$headpage->getType($dn);

            if(!isset($tInfo[$type])){
                msg_dialog::display(_("Information"),sprintf(_("The object-type '%s' cannot be managed with your setup of GOsa, please install the required  plugins!"), $type), ERROR_DIALOG);
                return;
            }

            $tData = $tInfo[$type];

            if($type == "FAKE_OC_ArpNewDevice"){
                if(!class_available("ArpNewDeviceTabs")){
                    msg_dialog::display(_("Error"), msgPool::class_not_found("ArpNewDevice"), ERROR_DIALOG);
                }else{
                    return(management::editEntry($action,$target,$all,"ArpNewDeviceTabs","ARPNEWDEVICETABS","incoming"));
                }
            }elseif($type == "FAKE_OC_NewDevice"){
                if(!class_available("SelectDeviceType")){
                    msg_dialog::display(_("Error"), msgPool::class_not_found("SelectDeviceType"), ERROR_DIALOG);
                }else{
                    $this->activationQueue[$dn] = array();
                    $this->dialogObject = new SelectDeviceType($this->config,$dn);
                    $this->dialogObject->set_acl_category("incoming");
                    $this->skipFooter = TRUE;
                    $this->displayApplyBtn = FALSE;
                    // see condition  -$s_action == "::systemTypeChosen"-  for further handling
                }
            }else{
                return(management::editEntry($action,$target,$all,$tData['tabClass'],$tData['tabDesc'],$tData['aclCategory']));
            }
        }
    }


    /*! \brief  Edit the selected system type.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $tInfo = $this->getObjectDefinitions();
        $info = preg_replace("/^new_/","",$action);
        if(!isset($tInfo[$info])){
            trigger_error("Unknown action type '".$action."' cant create a new system!");
        }else{
            return(management::newEntry($action,$target,$all, 
                        $tInfo[$info]['tabClass'],
                        $tInfo[$info]['tabDesc'],
                        $tInfo[$info]['aclCategory']));
        }
    }


    /*! \brief  Activates all selcted 'NewDevices' at once.
     *          Enqueues the selected Devices in the activation queue.
     */
    function activateMultiple($action,$target)
    {
        $headpage = $this->getHeadpage();
        foreach($target as $dn) {
            if($headpage->getType($dn) == "FAKE_OC_NewDevice"){
                $this->activationQueue[$dn] = array();
            }
        }
        if(count($this->activationQueue)){
            $this->dialogObject = new SelectDeviceType($this->config, array_keys($this->activationQueue));
            $this->skipFooter = TRUE;
        }
    }


    /*! \brief  The system selection dialog was closed. 
     *          We will now queue the given entry to be activated.
     */ 
    function systemTypeChosen()
    {
        // Detect the systems target type 
        $tInfo = $this->getObjectDefinitions();
        $selected_group = "none";
        if(isset($_POST['ObjectGroup'])){
            $selected_group = get_post('ObjectGroup');
        }
        $selected_system = get_post('SystemType');
        $tmp = array();
        foreach($this->activationQueue as $dn => $data){
            $tmp[$dn]['OG'] = $selected_group;
            $tmp[$dn]['SS'] = $selected_system;
        }
        $this->closeDialogs();
        $this->activationQueue = $tmp;
        return($this->handleActivationQueue());
    }


    /*! \brief  Activate queued goto systems.
     */
    function handleActivationQueue()
    {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                'Entries left: '.count($this->activationQueue), "<b>Handling system activation queue!</b>");

        if(!count($this->activationQueue)) return("");

        $ldap     = $this->config->get_ldap_link();
        $pInfo    = $this->getObjectDefinitions(); 
        $ui       = get_userinfo();
        $headpage = $this->getHeadpage();
        $ldap->cd($this->config->current['BASE']);

        // Walk through systems to activate
        while(count($this->activationQueue)){

            // Get next entry 
            reset($this->activationQueue);
            $dn = key($this->activationQueue);
            $data= $this->activationQueue[$dn];

            // Validate the given system type.
            if(!isset($data['SS'])) continue;
            $sysType = $data['SS'];
            if(!isset($pInfo[$sysType])){
                trigger_error('Unknown type \''.$sysType.'\'!');
                continue;
            }
            $type = $pInfo[$sysType];

            @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                    $dn, "<b>Try to activate:</b>");

            // Get target type definition 
            $plugClass    = $type["plugClass"];
            $tabClass     = $type["tabClass"];
            $aclCategory  = $type["aclCategory"];
            $tabDesc      = $type["tabDesc"];

            if(!class_available($tabClass)){
                msg_dialog::display(_("Error"), msgPool::class_not_found($tabclass), ERROR_DIALOG);
                unset($this->activationQueue[$dn]);
                continue;
            }else{

                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                        $sysType, "<b>System type:</b>");

                // Load permissions for selected 'dn' and check if we're allowed to create this 'dn' 
                $this->dn = $dn;
                $acls   = $ui->get_permissions($this->dn,$aclCategory."/".$plugClass);

                // Check permissions
                if(!preg_match("/c/",$acls)){
                    unset($this->activationQueue[$dn]);
                    msg_dialog::display(_("Error"), msgPool::permCreate(), ERROR_DIALOG);

                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                            $acls, "<b>Insufficient permissions!</b>");
                    continue;
                }else{

                    // Open object an preset some values like the objects base 
                    del_lock($dn);
                    management::editEntry('editEntry',array($dn),array(),$tabClass,$tabDesc, $aclCategory);
                    $this->displayApplyBtn = FALSE;
                    $this->tabObject->set_acl_base($headpage->getBase());

                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                            $data['OG'], "<b>Selected ogroup:</b>");

                    if($data['OG'] != "none"){
                        $this->tabObject->base = preg_replace("/^[^,]+,".preg_quote(get_ou("group", "ogroupRDN"), '/')."/i", "", $data['OG']);
                        $this->tabObject->by_object[$plugClass]->baseSelector->setBase($this->tabObject->base);
                    } else {
                        $this->tabObject->by_object[$plugClass]->baseSelector->setBase($headpage->getBase());
                        $this->tabObject->base = $headpage->getBase();
                    }

                    // Assign some default values for opsi hosts
                    if($this->tabObject instanceOf opsi_tabs){
                        $ldap = $this->config->get_ldap_link();
                        $ldap->cat($dn);
                        $source_attrs = $ldap->fetch();
                        foreach(array("macAddress" => "mac" ,"cn" => "hostId","description" => "description") as $src => $attr){
                            if(isset($source_attrs[$src][0])){
                                $this->tabObject->by_object['opsiGeneric']->$attr = $source_attrs[$src][0];
                            }
                        }
                        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                "", "<b>OPSI attributes adapted</b>");
                    }

                    // Queue entry to be activated, when it is saved.
                    if($data['OG'] != "none"){

                        // Set gotoMode to active if there was an ogroup selected.
                        $found = false;
                        foreach(array("workgeneric"=>"active","servgeneric"=>"active","termgeneric"=>"active") as $tab => $value){
                            if(isset($this->tabObject->by_object[$tab]->gotoMode)) {
                                $found = true;
                                $this->tabObject->by_object[$tab]->gotoMode = $value;
                                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                        $tab."->gotoMode = {$value}", "<b>Setting gotoMode to: </b>");
                            }
                        }
                        if(!$found){
                            msg_dialog::display(_("Internal error"), _("Cannot set mode to 'active'!"), ERROR_DIALOG);
                        }

                        // Update object group membership
                        $og = new ogroup($this->config,$data['OG']);
                        if($og){
                            $og->AddDelMembership($this->tabObject->dn);
                            $og->save();
                            @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                    $og->dn, "<b>Adding system to ogroup</b>");
                        }

                        // Set default system specific attributes
                        foreach (array("workgeneric", "termgeneric") as $cls){
                            if (isset($this->tabObject->by_object[$cls])){
                                $this->tabObject->by_object[$cls]->set_everything_to_inherited();
                                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                        $og->dn, "<b>Calling {$cls}->set_everything_to_inherited()</b>");
                            }
                        }

                        // Enable activation
                        foreach (array("servgeneric", "workgeneric", "termgeneric") as $cls){
                            if (isset($this->tabObject->by_object[$cls])){
                                $this->tabObject->by_object[$cls]->auto_activate= TRUE;
                                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                        $cls, "<b>Setting auto_activate=TRUE for</b>");
                            }
                        }

                        // Enable sending of LDAP events
                        if (isset($this->tabObject->by_object["workstartup"])){
                            $this->tabObject->by_object["workstartup"]->gotoLdap_inherit= TRUE;
                            @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                    "", "<b>Setting workstartup->gotoLdap_inherit=TRUE</b>");
                        }
                    }

                    // Try to inherit everythin from the selected object group and then save
                    //  the entry, normally this should work without any problems. 
                    // But if there is any, then display the dialogs.
                    if($data['OG'] != "none"){
                        $str = $this->saveChanges();

                        // There was a problem, skip activation here and allow to fix the problems..
                        if(is_object($this->tabObject)){
                            @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                    "", "<b>Automatic saving failed, let the user fix the issues now.</b>");
                            return;
                        }
                        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                "", "<b>System activated!</b>");
                    }else{
                        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                                "", "<b>Open dialogs now</b>");
                        return;
                    }
                }
            }
        }
    }


    /*! \brief  Save object modifications here and any dialogs too.
     *          After a successfull update of the object data, close
     *           the dialogs.
     */
    protected function saveChanges()
    {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                get_class($this->tabObject).": ".$this->tabObject->dn, "<b>Save</b>");

        // Handle 'New Unknown Devices' here.
        if($this->tabObject instanceOf ArpNewDeviceTabs){
            $this->tabObject->save_object();

            if($this->tabObject->by_object['ArpNewDevice']->gotoIntegration){
                $message = $this->tabObject->check();
                if(count($message)){
                    msg_dialog::displayChecks($message);
                }else{
                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                            get_class($this->tabObject).": ".$this->tabObject->dn, "<b>Queued for goto activation</b>");
                    $this->tabObject->save();
                    $this->activationQueue[$this->tabObject->dn]=array();
                    $this->closeDialogs();
                    $this->systemTypeChosen();
                }
                return;
            }
        }

        // Try to save changes here.
        $str = management::saveChanges();
        if($this->tabObject) return("");

        // Activate system if required..
        if(isset($this->activationQueue[$this->last_dn])){
            $dn = $this->last_tabObject->dn;
            $this->activate_new_device($dn);
            unset($this->activationQueue[$this->last_dn]);
        }

        /* Post handling for activated systems
           target opsi -> Remove source.
           target gosa -> Activate system.
         */
        if($this->last_tabObject instanceOf opsi_tabs){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->rmdir ($this->last_tabObject->dn);
            @DEBUG(DEBUG_LDAP,__LINE__, __FUNCTION__, __FILE__,
                    "Source removed: ".$this->tabObject->dn,"<b>Opsi host activated</b>");

            $hostId =  $this->last_tabObject->by_object['opsiGeneric']->hostId;
            $mac    =  $this->last_tabObject->by_object['opsiGeneric']->mac;
            $this->opsi->job_opsi_activate_client($hostId,$mac);

        }elseif(isset($this->last_tabObject->was_activated) && $this->last_tabObject->was_activated){
            $this->activate_new_device($this->last_tabObject->dn);
        }

        // Avoid using values from an older input dialog
        $_POST = array();
        $this->handleActivationQueue();
    }


    /*! \brief  Save object modifications here and any dialogs too.
     *          Keep dialogs opened.
     */
    protected function applyChanges()
    {
        $str = management::applyChanges();
        if($str) return($str);

        /* Post handling for activated systems
           target opsi -> Remove source.
           target gosa -> Activate system.
         */
        if($this->tabObject instanceOf opsi_tabs){
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->rmdir ($this->tabObject->dn);
            @DEBUG(DEBUG_LDAP,__LINE__, __FUNCTION__, __FILE__,
                    "Source removed: ".$this->tabObject->dn,"<b>Opsi host activated</b>");

            $hostId =  $this->tabObject->by_object['opsiGeneric']->hostId;
            $mac    =  $this->tabObject->by_object['opsiGeneric']->mac;
            $this->opsi->job_opsi_activate_client($hostId,$mac);
            $this->tabObject->set_acl_base($this->dn);

        }elseif(isset($this->tabObject->was_activated) && $this->tabObject->was_activated){
            $this->activate_new_device($this->tabObject->dn);
        }
    }


    /*! \brief  Sets FAIstate to "install" for "New Devices".
      This function is some kind of "Post handler" for activated systems,
      it is called directly after the object (workstabs,servtabs) gets saved.
      @param  String  $dn   The dn of the newly activated object.
      @return Boolean TRUE if activated else FALSE
     */
    function activate_new_device($dn)
    {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                $dn, "<b>Activating system:</b>");
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($dn);
        if($ldap->count()){
            $attrs = $ldap->fetch();
            if(count(array_intersect(array('goServer','gotoWorkstation'), $attrs['objectClass']))){
                $ocs = $attrs['objectClass'];
                unset($ocs['count']);
                $new_attrs = array();
                if(!in_array_strict("FAIobject",$ocs)){
                    $ocs[] = "FAIobject";
                    $new_attrs['objectClass'] = $ocs;
                }
                $new_attrs['FAIstate'] = "install";
                $ldap->cd($dn);
                $ldap->modify($new_attrs);
                if (!$ldap->success()){
                    msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn,
                                LDAP_MOD, "activate_new_device($dn)"));
                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                            $dn, "<b>Failed!</b>");
                }else{
                    @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                            $dn, "<b>Success</b>");
                    return(TRUE);
                }
            }else{
                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,
                        $dn, "<b>FAIstate not set to install, this is only done for gotoWorkstation/goServer!</b>");
            }
        }
        return(FALSE);
    }


    /*! \brief  Opens the snapshot creation dialog for the given target.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function createSnapshotDialog($action="",$target=array(),$all=array())
    {
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$target,"Snaptshot creation initiated!");

        $pInfo    = $this->getObjectDefinitions();
        $headpage = $this->getHeadpage();
        foreach($target as $dn){

            $entry = $headpage->getEntry($dn);
            $type = $headpage->getType($dn);
            if(!isset($pInfo[$type])) {
                trigger_error('Unknown system type \''.$type.'\'!');
                return;
            }

            if(!empty($dn) && $this->ui->allow_snapshot_create($dn,$pInfo[$type]['aclCategory'])){
                $this->dialogObject = new SnapShotDialog($this->config,$dn,$this);
                $this->dialogObject->aclCategories = array($pInfo[$type]['aclCategory']);
                $this->dialogObject->parent = &$this;

            }else{
                msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to create a snapshot for %s."),$dn),
                        ERROR_DIALOG);
            }
        }
    }


    /*! \brief  Displays the "Restore snapshot dialog" for a given target.
     *          If no target is specified, open the restore removed object
     *           dialog.
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function restoreSnapshotDialog($action="",$target=array(),$all=array())
    {
        // Set current restore base for snapshot handling.
        $headpage = $this->getHeadpage();
        $pInfo    = $this->getObjectDefinitions();
        if(is_object($this->snapHandler)){
            $bases = array();
            foreach($this->storagePoints as $sp){
                $bases[] = $sp.$headpage->getBase();
            }
        }

        // No bases specified? Try base
        if(!count($bases)) $bases[] = $this->headpage->getBase();

        // No target, open the restore removed object dialog.
        if(!count($target)){

            $cats = array();
            foreach($pInfo as $data){
                $cats[] = $data['aclCategory'];
            }
            $cats = array_unique($cats);

            $entry = $headpage->getBase();
            if(!empty($entry) && $this->ui->allow_snapshot_restore($entry,$cats)){
                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$entry,"Snaptshot restoring initiated!");
                $this->dialogObject = new SnapShotDialog($this->config,$entry,$this);
                $this->dialogObject->set_snapshot_bases($bases);
                $this->dialogObject->display_all_removed_objects = true;
                $this->dialogObject->display_restore_dialog = true;
                $this->dialogObject->parent = &$this;
            }else{
                msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to restore a snapshot for %s."),$entry),
                        ERROR_DIALOG);
            }
        }else{

            // Display the restore points for a given object.
            $dn = array_pop($target);
            $entry = $headpage->getEntry($dn);
            $type = $headpage->getType($dn);
            if(!isset($pInfo[$type])) {
                trigger_error('Unknown system type \''.$type.'\'!');
                return;
            }

            if(!empty($dn) && $this->ui->allow_snapshot_create($dn,$pInfo[$type]['aclCategory'])){
                @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$dn,"Snaptshot restoring initiated!");
                $this->dialogObject = new SnapShotDialog($this->config,$dn,$this);
                $this->dialogObject->set_snapshot_bases($bases);
                $this->dialogObject->display_restore_dialog = true;
                $this->dialogObject->parent = &$this;
            }else{
                msg_dialog::display(_("Permission"),sprintf(_("You are not allowed to restore a snapshot for %s."),$dn),
                        ERROR_DIALOG);
            }
        }
    }


    /*! \brief  Restores a snapshot object.
     *          The dn of the snapshot entry has to be given as ['target'] parameter.
     *
     *  @param  String  'action'  The name of the action which was the used as trigger.
     *  @param  Array   'target'  A list of object dns, which should be affected by this method.
     *  @param  Array   'all'     A combination of both 'action' and 'target'.
     */
    function restoreSnapshot($action="",$target=array(),$all=array())
    {
        $dn       = array_pop($target);
        $this->snapHandler->restore_snapshot($dn);
        @DEBUG (DEBUG_TRACE, __LINE__, __FUNCTION__, __FILE__,$dn,"Snaptshot restored!");
        $this->closeDialogs();
    }


    /*! \brief  Detects actions/events send by the ui
     *           and the corresponding targets.
     */
    function detectPostActions()
    {
        $action= management::detectPostActions();
        if(isset($_POST['abort_event_dialog']))  $action['action'] = "cancel";
        if(isset($_POST['save_event_dialog']))  $action['action'] = "saveEvent";
        if(isset($_POST['cd_create']))  $action['action'] = "initiateISOcreation";
        if(isset($_GET['PerformIsoCreation']))  $action['action'] = "performIsoCreation";
        if(isset($_POST['SystemTypeAborted']))  $action['action'] = "cancel";
        if(isset($_POST['password_cancel']))  $action['action'] = "cancel";
        if(isset($_POST['password_finish']))  $action['action'] = "passwordChangeConfirmed";

        if(isset($_POST['new_goServer']))  $action['action'] = "new_goServer";
        if(isset($_POST['new_gotoWorkstation']))  $action['action'] = "new_gotoWorkstation";
        if(isset($_POST['new_gotoTerminal']))  $action['action'] = "new_gotoTerminal";
        if(isset($_POST['new_gotoPrinter']))  $action['action'] = "new_gotoPrinter";
        if(isset($_POST['new_goFonHardware']))  $action['action'] = "new_goFonHardware";
        if(isset($_POST['new_ieee802Device']))  $action['action'] = "new_ieee802Device";
        if(isset($_POST['new_FAKE_OC_OpsiHost']))  $action['action'] = "new_FAKE_OC_OpsiHost";

        if(!is_object($this->tabObject) && !is_object($this->dialogObject)){
            if(count($this->activationQueue)) $action['action'] = "handleActivationQueue";
        }

        if(isset($_POST['systemTypeChosen']))  $action['action'] = "systemTypeChosen";

        return($action);
    }


    /*! \brief   Overridden render method of class management.
     *            this allows us to add a release selection box.
     */
    function renderList()
    {
        $headpage = $this->getHeadpage();
        $headpage->update();

        $tD = $this->getObjectDefinitions();
        $smarty = get_smarty();
        foreach($tD as $name => $obj){
            $smarty->assign("USE_".$name, (empty($obj['TABNAME']) || class_available($obj['TABNAME'])));
        }

        $display = $headpage->render();
        return($this->getHeader().$display);
    }


    public function getObjectDefinitions()
    {
        $tabs = array(
                "device" => array(
                    "ou"          => get_ou('Device','GOsaDeviceRDN'),
                    "plugClass"   => "Device",
                    "tabClass"    => "DeviceTab",
                    "tabDesc"     => "NEWDEVICETABS",
                    "aclClass"    => "Device",
                    "sendEvents"  => FALSE,
                    "aclCategory" => "Device"),

                "FAKE_OC_OpsiHost" => array(
                    "ou"          => "",
                    "plugClass"   => "opsiGeneric",
                    "tabClass"    => "opsi_tabs",
                    "tabDesc"     => "OPSITABS",
                    "aclClass"    => "opsiGeneric",
                    "sendEvents"  => TRUE,
                    "aclCategory" => "opsi"),

                "goServer" => array(
                    "ou"          => get_ou("servgeneric", "serverRDN"),
                    "plugClass"   => "servgeneric",
                    "tabClass"    => "servtabs",
                    "tabDesc"     => "SERVTABS",
                    "aclClass"    => "servgeneric",
                    "sendEvents"  => TRUE,
                    "aclCategory" => "server"),

                "gotoWorkstation" => array(
                    "ou"          => get_ou("workgeneric", "workstationRDN"),
                    "plugClass"   => "workgeneric",
                    "tabClass"    => "worktabs",
                    "tabDesc"     => "WORKTABS",
                    "aclClass"    => "workgeneric",
                    "sendEvents"  => TRUE,
                    "aclCategory" => "workstation"),

                "gotoTerminal" => array(
                        "ou"          => get_ou("termgeneric", "terminalRDN"),
                        "plugClass"   => "termgeneric",
                        "tabClass"    => "termtabs",
                        "sendEvents"  => TRUE,
                        "tabDesc"     => "TERMTABS",
                        "aclClass"    => "termgeneric",
                        "aclCategory" => "terminal"),

                "gotoPrinter" => array(
                        "ou"          => get_ou("printgeneric", "printerRDN"),
                        "plugClass"   => "printgeneric",
                        "tabClass"    => "printtabs",
                        "tabDesc"     => "PRINTTABS",
                        "aclClass"    => "printgeneric",
                        "sendEvents"  => FALSE,
                        "aclCategory" => "printer"),

                "FAKE_OC_NewDevice" => array(
                        "ou"          => get_ou("ArpNewDevice", "systemIncomingRDN"),
                        "plugClass"   => "termgeneric",
                        "tabClass"    => "termtabs",
                        "sendEvents"  => TRUE,
                        "tabDesc"     => "TERMTABS",
                        "aclClass"    => "termgeneric",
                        "aclCategory" => "terminal"),

                "goFonHardware" => array(
                        "ou"          => get_ou("phoneGeneric", "phoneRDN"),
                        "plugClass"   => "phoneGeneric",
                        "tabClass"    => "phonetabs",
                        "tabDesc"     => "PHONETABS",
                        "sendEvents"  => FALSE,
                        "aclClass"    => "phoneGeneric",
                        "aclCategory" => "phone"),

                "FAKE_OC_winstation" => array(
                        "ou"          => get_winstations_ou(),
                        "plugClass"   => "wingeneric",
                        "sendEvents"  => TRUE,
                        "tabClass"    => "wintabs",
                        "tabDesc"     => "WINTABS",
                        "aclClass"    => "wingeneric",
                        "aclCategory" => "winworkstation"),

                "ieee802Device" => array(
                        "ou"          => get_ou("componentGeneric", "componentRDN"),
                        "plugClass"   => "componentGeneric",
                        "sendEvents"  => FALSE,
                        "tabClass"    => "componenttabs",
                        "tabDesc"     => "COMPONENTTABS",
                        "aclClass"    => "componentGeneric",
                        "aclCategory" => "component"),
                );

        // Now map some special types
        $tabs['FAKE_OC_NewWorkstation'] = &$tabs['gotoWorkstation'];
        $tabs['FAKE_OC_NewTerminal'] = &$tabs['gotoTerminal'];
        $tabs['FAKE_OC_NewServer'] = &$tabs['gotoWorkstation'];
        $tabs['gotoWorkstation__IS_BUSY'] = &$tabs['gotoWorkstation'];
        $tabs['gotoWorkstation__IS_ERROR'] = &$tabs['gotoWorkstation'];
        $tabs['gotoWorkstation__IS_LOCKED'] = &$tabs['gotoWorkstation'];
        $tabs['gotoTerminal__IS_BUSY'] = &$tabs['gotoTerminal'];
        $tabs['gotoTerminal__IS_ERROR'] = &$tabs['gotoTerminal'];
        $tabs['gotoTerminal__IS_LOCKED'] = &$tabs['gotoTerminal'];
        $tabs['FAKE_OC_TerminalTemplate'] = &$tabs['gotoTerminal'];
        $tabs['FAKE_OC_WorkstationTemplate'] = &$tabs['gotoTerminal'];
        $tabs['goServer__IS_BUSY'] = &$tabs['goServer'];
        $tabs['goServer__IS_ERROR'] = &$tabs['goServer'];
        $tabs['goServer__IS_LOCKED'] = &$tabs['goServer'];

        $tabs['FAKE_OC_ArpNewDevice'] = &$tabs['FAKE_OC_NewDevice'];


        // Remove those types, we cannot handle at the moment due to lack of 
        //  installed GOsa-plugins.
        foreach($tabs as $name => $tab){

            if(!class_available($tab['plugClass'])) {
                unset($tabs[$name]);
            }
        }

        return($tabs);
    }


    static function filterSystemDescription($row,$dn,$pid,$state = '',$description=array())
    {
        $dn= LDAP::fix(func_get_arg(1));
        $desc = isset($description[0])?set_post($description[0]):"";
    
        $rc = "";
        switch($state){
            case 'locked' : $rc = "<rowClass:entry-locked/><rowLabel:locked/>"; break;
            case 'error' : $rc = "<rowClass:entry-error/><rowLabel:error/>"; break;
            case 'busy' : $rc = "<rowClass:entry-busy/><rowLabel:busy/>"; break;
            case 'warning' : $rc = "<rowClass:entry-warning/><rowLabel:warning/>"; break;
        }
        return("<a href='?plug=".$_GET['plug']."&amp;PID={$pid}&amp;act=listing_edit_{$row}' title='{$dn}'>".$desc."</a>{$rc}");
    }


    static function filterLink()
    {
        $result= "&nbsp;";
        $row= func_get_arg(0);
        $pid= func_get_arg(4);
        $dn= LDAP::fix(func_get_arg(1));
        $params= array(func_get_arg(2));

        // Collect sprintf params
        for ($i = 3;$i < func_num_args();$i++) {
            $val= func_get_arg($i);
            if (is_array($val)){
                $params[]= $val[0];
                continue;
            }
            $params[]= $val;
        }

        $result= "&nbsp;";
        $trans= call_user_func_array("sprintf", $params);
        if ($trans != "") {
            return("<a href='?plug=".$_GET['plug']."&amp;PID=$pid&amp;act=listing_edit_$row' title='$dn'>".$trans."</a>");
        }
        return $result;
    }


    static function systemRelease($a,$b,$c,$objectclasses= null,$class= null)
    {
        global $config;

        // No objectclasses set - go ahead
        if(!$objectclasses) return("&nbsp;");

        // Skip non fai objects
        if (!in_array_ics("FAIobject", $objectclasses)) {
            return "&nbsp;";
        }

        // If we've an own fai class, just use this
        if ($class && is_array($class)) {
            foreach (explode(' ', $class[0]) as $element) {
                if ($element[0] == ":") {
                    return "&nbsp;".image('images/empty.png')."&nbsp;".mb_substr($element, 1);
                }
            }
        }

        // Load information if needed
        $ldap = $config->get_ldap_link();
        $ldap->cd($config->current['BASE']);
        $ldap->search("(&(objectClass=gosaGroupOfNames)(FAIclass=*)(member=".$b."))",array('FAIclass','cn'));
        while($attrs = $ldap->fetch()){
            $rel = preg_replace("/^.*:/","",$attrs['FAIclass'][0]);
            $sys = sprintf(_("Inherited from %s"),$attrs['cn'][0]);
            $str = "&nbsp;".image('plugins/ogroups/images/ogroup.png', "", $sys)."&nbsp;".$rel;
            return($str);
        }

        return("&nbsp;");
    }


    /*! \brief  !! Incoming dummy acls, required to defined acls for incoming objects
     */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Incoming objects"),
                    "plDescription" => _("Incoming objects"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 99,
                    "plSection"     => array("administration"),

                    "plProperties" =>
                    array(
                        array(
                            "name"          => "systemRDN",
                            "type"          => "rdn",
                            "default"       => "ou=systems,",
                            "description"   => _("RDN for system storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_systemRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE
                            )
                        ),


            "plCategory"    => array("incoming"   => array( "description"  => _("Incoming"))),
            "plProvidedAcls"=> array()
                ));
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
