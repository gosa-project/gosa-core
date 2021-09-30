<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_gotomasses.inc 20962 2011-07-27 08:00:12Z hickert $$
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

class gotomasses extends management
{

    var $plHeadline     = "Deployment status";
    var $plDescription  = "Monitor and schedule system deployment and update processes";
    var $plIcon  = "plugins/goto/images/goto.png";
    var $matIcon = "tune";

    var $current        = FALSE;
    var $dialog         = FALSE;
    var $ids_to_remove  = array();

    var $events         = array();
    var $event_tags     = array();
    var $recently_removed = array();

    var $read_only = FALSE;
    var $acl_base;
    var $acl_category;

    var $o_queue = NULL;

    function __construct(&$config, $ui)
    {
        /* Include config object */
        $this->config= &$config;
        $this->events  = DaemonEvent::get_event_types( SYSTEM_EVENT);
        $this->acl_base = $config->current['BASE'];
        $this->acl_category = "gotomasses/";

        /* Get tags that will be used in queue searches */
        $this->event_tags = array("none");
        foreach($this->events['SCHEDULED'] as $evt){
            $this->event_tags[] = $evt['s_Queued_Action'];
        }

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("deploy-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("deploy-list.xml", true));
        $headpage->registerElementFilter("hostName",  "gotomasses::filterHostName");
        $headpage->registerElementFilter("filterTask","gotomasses::filterTask");
        $headpage->registerElementFilter("filterPeriod","gotomasses::filterPeriod");
        $headpage->registerElementFilter("filterSchedule","gotomasses::filterSchedule");
        $headpage->registerElementFilter("filterStatus","gotomasses::filterStatus");
        $headpage->setFilter($filter);

        parent::__construct($config, $ui, "Events", $headpage);

        $this->registerAction('prioDown',   "prioDown");
        $this->registerAction('prioUp',     "prioUp");
        $this->registerAction('prioPause',  "prioPause");
        $this->registerAction('prioResume', "prioResume");
        $this->registerAction('processNow', "processNow");
        $this->registerAction('viewLogs',   "viewLogs");
        $this->registerAction('abort',      "abortEvent");
        $this->registerAction('saveEventDialog',   "saveEventDialog");
        $this->registerAction('halt', 'newEntry');
        $this->registerAction('reboot', 'newEntry');
        $this->registerAction('wakeup', 'newEntry');
        $this->registerAction('update', 'newEntry');
        $this->registerAction('lock', 'newEntry');
        $this->registerAction('activate', 'newEntry');
        $this->registerAction('reinstall', 'newEntry');
        $this->registerAction('import', 'importEvents');
    }

    function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        if($this->acl_is_writeable("")){
            $type = "DaemonEvent_".$action;
            if(isset($this->events['BY_CLASS'][$type])){
                $e_data = $this->events['BY_CLASS'][$type];
                $this->dialogObject = new $e_data['CLASS_NAME']($this->config);
            }
        }
    }

    function importEvents()
    {
        $this->dialogObject = new goto_import_file($this->config,$this);
    }

    static function filterHostName($mac, $name ="")
    {
        if(isset($name[0]) && $name[0] != "none"){
            return($name[0]);
        }
        return($mac[0]);
    }

    static function filterTask($tag)
    {
        $tag = $tag[0];
        $str = $tag;

        /* Check if this event exists as Daemon class
         * In this case, display a more accurate entry.
         */
        $events  = DaemonEvent::get_event_types( SYSTEM_EVENT);
        if(isset($events['QUEUED'][$tag])){
            $evt_name   = $events['QUEUED'][$tag];
            $event_type = $events['BY_CLASS'][$evt_name];
            $str        = $event_type['s_Menu_Name'];

            if(strlen($str) > 20){
                $str = substr($str,0,18)."...";
            }

            if(isset($event_type['ListImage']) && !empty($event_type['ListImage'])){
                $str = $event_type['ListImage']."&nbsp;".$str;
            }
        }
        return($str);
    }

    static function filterPeriod($periodic=array())
    {
        $period = "&nbsp;-";
        if(isset($periodic[0]) && !preg_match("/none/i",$periodic[0])){
            $tmp = explode("_", $periodic[0]);
            if(count($tmp) == 2){
                $period= $tmp[0]."&nbsp;"._($tmp[1]);
            }
        }
        return($period);
    }

    static function filterSchedule($stamp)
    {
        if ($stamp['0'] == "19700101000000"){
            return(_("immediately"));
        } else {
            return(date("d.m.Y H:i:s",strtotime($stamp[0])));
        }
    }


    static function filterStatus($status, $mac,$headertag, $progress)
    {

        $mac = $mac[0];
        $status = $status[0];
        $progress = $progress[0];
        $headertag = $headertag[0];

        if($status == "waiting"){
            $status = "<img class='center' src='plugins/goto/images/clock.png' alt=''>&nbsp;"._("Waiting");
        }
        if($status == "error"){
            $status = "<img class='center' src='images/false.png' alt=''>&nbsp;"._("Error");
        }
        if($status == "processed"){
            $status = "<img class='center' src='images/true.png' alt=''>&nbsp;"._("Processed");
        }

        /* Special handling for all entries that have
           STATUS == "processing" && PROGRESS == NUMERIC
         */
        if($status == "processing" && $progress){
            $percent = $progress;

            /* Show activation? */
            if ($percent == "goto-activation"){
                $status = "<img class='center' src='images/lists/off.png' alt=''>&nbsp;"._("Locked");

                /* Show hardware detect? */
            } elseif ($percent == "goto-hardware-detection") {
                $status = "<img class='center' src='plugins/goto/images/hardware.png' alt=''>&nbsp;"._("Detection");

                /* Real percent */
            } else {
                if (preg_match('/install/', $headertag)){
                    $status= progressbar($progress, 80, 13, true, false, "progress_".preg_replace("/:/","_",$mac));
                } else {
                    $status = preg_replace('/ /', '&nbsp;', _("in progress"));
                }
            }
        }
        return($status);
    }

    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        if(count($target) == 1){
            $headpage = $this->getHeadpage();
            $entry = $headpage->getEntry($target[0]);
            $event = $entry['EVENT'];
            if($event['STATUS'] == "waiting" && isset($this->events['QUEUED'][$event['HEADERTAG']])){
                $evt_name = $this->events['QUEUED'][$event['HEADERTAG']];
                $type = $this->events['BY_CLASS'][$evt_name];
                $this->dialogObject = new $type['CLASS_NAME']($this->config,$event);
            }
        }
    }

    function removeEntryRequested($action="",$target=array(),$all=array())
    {
        if(!$this->acl_is_removeable()){
            msg_dialog::display(_("Permission"), msgPool::permDelete(), ERROR_DIALOG);
        }else{

            $deleteable_jobs = array();
            $not_deleteable_jobs = array();
            $headpage = $this->getHeadpage();

            foreach($target as $dn){

                $tmp = $headpage->getEntry($dn);
                $task = $tmp['EVENT'];

                /* Create a printable job name/description */
                if(isset($this->events['QUEUED'][$task['HEADERTAG']])){
                    $evt_name = $this->events['QUEUED'][$task['HEADERTAG']];
                    $evt = $this->events['BY_CLASS'][$evt_name];
                    $j_name = $task['ID']." - ".$evt['s_Menu_Name']."&nbsp;".$task['MACADDRESS'];
                }else{
                    $j_name = $task['ID']." - ".$task['HEADERTAG']."&nbsp;".$task['MACADDRESS'];
                }

                /* Only remove WAITING or ERROR entries */
                if(in_array_strict($task['STATUS'],array("waiting","error","processed")) ||
                        ($task['STATUS'] == "processing" && !preg_match("/install/",$task['HEADERTAG'])) ){
                    $this->ids_to_remove[] = $task['ID'];
                    $deleteable_jobs[] = $j_name;
                }else{
                    $not_deleteable_jobs[] = $j_name;
                }
            }
            if(count($not_deleteable_jobs)){
                msg_dialog::display(_("Remove"),
                        sprintf(_("The following jobs couldn't be deleted, they have to be aborted: %s"),
                            "<br>".msgPool::buildList($not_deleteable_jobs)),INFO_DIALOG);
            }

            if(count($this->ids_to_remove)){
                $smarty = get_smarty();
                $smarty->assign("multiple", TRUE);
                $smarty->assign("info",msgPool::deleteInfo($deleteable_jobs));
                return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
            }
        }
    }


    function removeEntryConfirmed($action="",$target=array(),$all=array(),
            $altTabClass="",$altTabType="",  $altAclCategory="",$altAclPlugin="")
    {
        if($this->acl_is_removeable("")){
            timezone::get_default_timezone();
            $this->save();
        }
    }


    /*! \brief  Force queue job to be aborted.
     */
    function abortEvent($action="",$target=array(),$all=array())
    {
        /* Entries are paused by setting the status to
         *  something different from 'waiting'.
         * We simply use 'paused'.
         */
        $data = array("status"    => "paused");

        /* Detect if the ids we got are valid and
         *  check if the status allows pausing.
         */
        $update_ids = array();
        $headpage = $this->getHeadpage();
        foreach($target as $id){
            $tmp = $headpage->getEntry($id);
            $update_ids[] = $tmp['MACADDRESS'][0];
        }

        if(class_available("DaemonEvent_faireboot")){
            $tmp = new DaemonEvent_faireboot($this->config);
            $tmp->add_targets($update_ids);
            $tmp->set_type(TRIGGERED_EVENT);
            $this->recently_removed = $update_ids;
        }else{
            msg_dialog::display(_("Error"),
                    sprintf(_("Required class '%s' cannot be found: job not aborted!"),
                        "DaemonEvent_faireboot") , ERROR_DIALOG);
        }
    }


    function prioDown($action="",$target=array(),$all=array())
    {
        if(count($target) == 1){
            $this->update_priority($target[0], 'down');
        }
    }

    function prioUp($action="",$target=array(),$all=array())
    {
        if(count($target) == 1){
            $this->update_priority($target[0], 'up');
        }
    }

    function prioPause($action="",$target=array(),$all=array())
    {
        $this->pause_queue_entries($target);
    }

    function prioResume($action="",$target=array(),$all=array())
    {
        $this->resume_queue_entries($target);
    }

    function processNow($action="",$target=array(),$all=array())
    {
        $this->execute_queue_entries($target);
    }

    function viewLogs($action="",$target=array(),$all=array())
    {
        if(count($target) == 1){
            $id =  $target[0];
            $type = FALSE;
            $headpage = $this->getHeadpage();
            $tmp = $headpage->getEntry($id);
            $entry = $tmp['EVENT'];
            $this->dialogObject = new gotoLogView($this->config,"",$entry,$this);
        }
    }

    function saveEventDialog()
    {
        if(is_object($this->dialogObject)){
            $this->dialogObject->save_object();
            $this->current = -1;
        }
        $this->closeDialogs();
    }




    /*! \brief  Move an entry up or down in the queue, by updating its execution timestamp                             
      @param  $id     Integer  The ID of the entry which should be updated.                                            
      @param  $type   String   "up" / "down"                                                                           
      @return boolean TRUE in case of success else FALSE                                                               
     */                                                                                                                
    public function update_priority($id,$type = "up")                                                                  
    {                                          
        $headpage = $this->getHeadpage();
        $entries = $headpage->getEntries();
        $entry = $headpage->getEntry($id);

        $map = array();
        $last = 0;
        $next = 0;
        foreach($entries as $pa){
            $map[$pa['TIMESTAMP'][0]] = $pa['TIMESTAMP'][0];
        }
        krsort($map);
        $found = 0;
        $cur = 0;
        foreach($map as $ts){
            if($found){
                $next = $ts;
                break;
            }
            if($ts == $entry['TIMESTAMP'][0]){
                $found = TRUE;  
                $cur = $ts;
            }else{
                $last = $ts;
            }
        }

        if($type == "up" && $next != 0){
            return TRUE;
        }elseif($type == "down" && $last != 0){
            return TRUE;
        }
    }


    function detectPostActions()
    {
        $action = management::detectPostActions();
        if(isset($_POST['save_event_dialog'])) $action['action'] = "saveEventDialog";
        if(isset($_POST['abort_event_dialog'])) $action['action'] = "cancel";
        if(isset($_POST['delete_multiple_confirm'])) $action['action'] = "removeConfirmed";
        if(isset($_POST['delete_cancel'])) $action['action'] = "cancel";
        if(isset($_POST['import_abort'])) $action['action'] = "cancel";
        return($action);
    }


    function closeDialogs()
    {
        $this->current = FALSE;
        management::closeDialogs();
    }

    /*! \brief  Resumes to status 'waiting'.
     *  @return Boolean TRUE in case of success, else FALSE. 
     */
    private function resume_queue_entries($ids)
    {
        /* Entries are resumed by setting the status to 
         *  'waiting'
         */
        $data = array("status"    => "waiting");

        /* Check if given ids are valid and check if the status
         *  allows resuming.
         */
        $update_ids = array();
        $headpage = $this->getHeadpage();
        foreach($ids as $id){
            $tmp = $headpage->getEntry($id);
            $entry = $tmp['EVENT'];
            if(isset($entry['STATUS']) && preg_match("/paused/",$entry['STATUS'])){
                $update_ids[] = $entry['ID'];
            }
        }

        return(TRUE);
    }


    /*! \brief  Force queue job to be done as far as possible.
     *  @return Boolean TRUE in case of success, else FALSE.
     */
    private function execute_queue_entries($ids)
    {
        /* Execution is forced by updating the status to 
         *  waiting and setting the timestamp to current time.
         */
        $data = array(  "timestamp" => date("YmdHis",time()), 
                "status"    => "waiting");

        /* Only allow execution of paused or waiting entries 
         */
        $update_ids = array();
        $headpage = $this->getHeadpage();
        foreach($ids as $id){
            $tmp = $headpage->getEntry($id);
            $entry = $tmp['EVENT'];
            if(in_array_strict($entry['STATUS'],array("paused","waiting"))){
                $update_ids[] = $entry['ID'];
            }
        }

        return(TRUE);
    }


    /*! \brief Pauses the specified queue entry from execution.
     *  @return Boolean TRUE in case of success, else FALSE. 
     */
    private function pause_queue_entries($ids)
    {
        /* Entries are paused by setting the status to 
         *  something different from 'waiting'.
         * We simply use 'paused'.
         */   
        $data = array("status"    => "paused");

        /* Detect if the ids we got are valid and
         *  check if the status allows pausing.
         */ 
        $update_ids = array();
        $headpage = $this->getHeadpage();
        foreach($ids as $id){
            $tmp = $headpage->getEntry($id);
            $entry = $tmp['EVENT']; 
            if(isset($entry['STATUS']) && preg_match("/waiting/",$entry['STATUS'])){
                $update_ids[] = $entry['ID'];
            }
        }


        return(TRUE);
    }

    function save_object(){}

    function save(){}

    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("System deployment"),
                    "plDescription" => _("Provide a mechanism to automatically activate systems"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("addon"),
                    "plCategory"    => array("gotomasses" => array("description" => _("System deployment"))),
                    "plProvidedAcls" => array("Comment"   => _("Description")) 
                    ));
    }


    function set_acl_base($base)
    {
        $this->acl_base= $base;
    }


    function set_acl_category($category)
    {
        $this->acl_category= "$category/";
    }


    function acl_is_writeable($attribute,$skip_write = FALSE)
    {
        if($this->read_only) return(FALSE);
        $ui= get_userinfo();
        return preg_match('/w/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute, $skip_write));
    }


    function acl_is_readable($attribute)
    {
        $ui= get_userinfo();
        return preg_match('/r/', $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute));
    }


    function acl_is_createable($base ="")
    {
        if($this->read_only) return(FALSE);
        $ui= get_userinfo();
        if($base == "") $base = $this->acl_base;
        return preg_match('/c/', $ui->get_permissions($base, $this->acl_category.get_class($this), '0'));
    }


    function acl_is_removeable($base ="")
    {
        if($this->read_only) return(FALSE);
        $ui= get_userinfo();
        if($base == "") $base = $this->acl_base;
        return preg_match('/d/', $ui->get_permissions($base, $this->acl_category.get_class($this), '0'));
    }

    function acl_is_moveable($base = "")
    {
        if($this->read_only) return(FALSE);
        $ui= get_userinfo();
        if($base == "") $base = $this->acl_base;
        return preg_match('/m/', $ui->get_permissions($base, $this->acl_category.get_class($this), '0'));
    }


    function getacl($attribute,$skip_write= FALSE)
    {
        $ui= get_userinfo();
        $skip_write |= $this->read_only;
        return  $ui->get_permissions($this->acl_base, $this->acl_category.get_class($this), $attribute,$skip_write);
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
