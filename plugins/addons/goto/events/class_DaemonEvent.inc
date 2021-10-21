<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_DaemonEvent.inc 20962 2011-07-27 08:00:12Z hickert $$
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

define("SCHEDULED_EVENT",1);
define("TRIGGERED_EVENT",2);

define("HIDDEN_EVENT",1);
define("SYSTEM_EVENT",2);
define("USER_EVENT"  ,4);


/*! \brief    This is the event base class
  \author   Fabian Hickert <hickert@gonicus.de>
  \version  1.00
  \date     26.02.2008

  This is the base class for all new daemon events.
  It implements most of the required functionality.
 */
class DaemonEvent
{
    /* Menu Strings */
    protected $s_Menu_Name  = "s_Menu_Name not set";  // Diplayed in the ActionsMenu->CreateNew
    protected $s_Event_Name = "s_Event_Name not set"; // Will be displayed in the Management list.
    protected $s_Menu_Image = "images/empty.png";     // Image displayed in Actions->New
    protected $s_List_Image = "";                     // Image displayed in event listing

    public $config;          // GOsa configuration object
    protected $data;            // The event data, when edited

    protected $a_targets    = array();  // The list of assigned Targets (When newly created)
    protected $s_Schedule_Action = "";       // The deamon command name when newly created. (e.g. job_trigger_action_halt)
    protected $s_Trigger_Action= "";       // The deamon command name when edited. (e.g. trigger_action_halt)
    protected $s_Queued_Action= "";       // The deamon command name when edited. (e.g. trigger_action_halt)
    protected $timestamp    = 0;        // Event execution time;
    protected $id           = -1;       // The Table ID
    protected $status       = "unknown";// The current event status
    protected $is_new       = TRUE;     // Is TRUE if this is a new event

    protected $mode         = SCHEDULED_EVENT; // Default action is sheduled.

    /* Sub dialog hanlding */
    protected $target_listing       = NULL;     // The listing used by the target add dialog
    protected $target_add_list_used = FALSE;    // Indicates that the target add list was used.
    protected $time_select_used     = FALSE;    // Indicates that we have used the timestamp select boxes.
    protected $target_list_used     = FALSE;    // Target list was diaplayed?
    protected $_target_list         = array();  // Object Cache of those objects displayed in the target add dialog
    protected $workstation_list     = array();  // Used as cache in the target list.
    protected $server_list          = array();  // Used as cache in the target list.

    protected $visible_for          = HIDDEN_EVENT;

    protected $periodType           = "days";
    protected $periodValue          = 7;
    protected $activate_periodical_job = FALSE;

    protected $attributes           = array("timestamp");

    function set_type($type)
    {
        $this->mode = $type;
    }


    function get_type()
    {
        return($this->mode);
    }

    /*! \brief  Class contructor.
      @param  Array   GOsa configuration object.
      @param  Array   Event data, only given when edited.
     */
    public function __construct($config,$data = array())
    {
        $this->data   = $data;
        $this->config = $config;
        timezone::get_default_timezone();
        $this->timestamp = time();

        /* Load values from given data */
        if(count($data)){
            $this->is_new = FALSE;

            $attrs = array("id" => "ID");
            foreach($attrs as $to => $from){
                $this->$to = $data[$from];
            }
            if(isset($data['TIMESTAMP'])){
                $this->timestamp = $this->_event_to_timestamp($data['TIMESTAMP']);
            }
        }

        if(isset($data['PERIODIC']) && !preg_match("/none/i",$data['PERIODIC'])){
            $tmp = explode("_",$data['PERIODIC']);
            if(count($tmp) == 2){
                $this->activate_periodical_job = TRUE;
                $this->periodValue = $tmp[0];
                $this->periodType = $tmp[1];
            }
        }

        // Prepare lists
        $this->targetList = new sortableListing();
        $this->targetList->setDeleteable(true);
        $this->targetList->setInstantDelete(false);
        $this->targetList->setEditable(false);
        $this->targetList->setWidth("100%");
        $this->targetList->setHeight("200px");
        $this->targetList->setColspecs(array('*'));
        $this->targetList->setHeader(array(_("Mac")));
        $this->targetList->setDefaultSortColumn(0);
        $this->targetList->setAcl('rwcdm');
    }


    /*! \brief  Create the HTML output for the plugin.
      @return String  The generated HTML output.
     */
    public function execute()
    {
        $theme = getThemeName();

        $this->time_select_used = FALSE;
        $this->target_list_used = FALSE;

        switch ($theme) {
            case 'classic':
                $str = "<h3>"._("This job has no template!")."</h3>";
                $str.= "<hr>";
                $str.= "<div class='plugin-actions'>
                            <button type='submit' name='abort_event_dialog'>".msgPool::cancelButton()."</button>
                        </div>";
                break;

            default:
                $str = "<h3>"._("This job has no template!")."</h3>
                        <div class='card-action'>
                            <button class='btn-small' type='submit' name='abort_event_dialog'>".msgPool::cancelButton()."</button>
                        </div>";
                break;
        }
        return($str);
    }

    /*! \brief  Returns the plugin header, displayed in the template.
      @return String  HTML header part.
     */
    public function get_header()
    {
        if($this->target_add_list_used){
            return("");
        }
        $str = "<h2>".sprintf(_("Create '%s' job"),$this->s_Event_Name)."</h2>";
        return($str);
    }


    /*! \brief  Returns the plugin footer (save cancel), displayed in the template.
      @return String  HTML footer part.
     */
    public function get_footer()
    {
        $theme = getThemeName();

        if($this->target_add_list_used){
            return("");
        }

        switch ($theme) {
            case 'classic':
                $str = "<hr>";
                $str.= "<div class='plugin-actions'>
                            <button type='submit' name='save_event_dialog'>".msgPool::saveButton()."</button>&nbsp;
                            <button type='submit' name='abort_event_dialog'>".msgPool::cancelButton()."</button>
                        </div>";
                break;

            default:
                $str = "<div class='card-action'>
                            <button class='btn-small primary' type='submit' name='save_event_dialog'>".msgPool::saveButton()."</button>
                            <button class='btn-small primary' type='submit' name='abort_event_dialog'>".msgPool::cancelButton()."</button>
                        </div>";
                break;
        }

        return($str);
    }


    /*! \brief  Returns HTML representation of a timestamp using <select> boxes.
      @return Returns HTML content.
     */
    public function get_time_select()
    {
        $this->time_select_used = TRUE;

        $smarty = get_smarty();

        $year   = date("Y",$this->timestamp);
        $month  = date("m",$this->timestamp);
        $day    = date("d",$this->timestamp);

        $hour   = date("H",$this->timestamp);
        $minute = date("i",$this->timestamp);
        $second = date("s",$this->timestamp);

        $years = array();
        for($i = date("Y",time()); $i <= 2037 ;$i ++){
            $years[$i] = $i;
        }
        $months = array();
        for($i = 1; $i <= 12; $i ++){
            $e = str_pad($i,2,"0",STR_PAD_LEFT);
            $months[$e] = $e;
        }
        $days = array();
        for($i = 1; $i <= cal_days_in_month(CAL_GREGORIAN,$month,$year); $i ++){
            $e = str_pad($i,2,"0",STR_PAD_LEFT);
            $days[$e] = $e;
        }
        $hours = array();
        for($i = 0; $i < 24; $i ++){
            $e = str_pad($i,2,"0",STR_PAD_LEFT);
            $hours[$e] = $e;
        }
        $minutes = array();
        for($i = 0; $i < 60; $i ++){
            $e = str_pad($i,2,"0",STR_PAD_LEFT);
            $minutes[$e] = $e;
        }
        $seconds = array();
        for($i = 0; $i < 60; $i ++){
            $e = str_pad($i,2,"0",STR_PAD_LEFT);
            $seconds[$e] = $e;
        }

        $periodTypes = array(
                "minutes" => _("Minutes"),
                "hours"   => _("Hours"),
                "days"    => _("Days"),
                "weeks"   => _("Weeks"),
                "months"  => _("Months"));


        $smarty->assign("periodTypes", set_post($periodTypes));
        $smarty->assign("periodType", set_post($this->periodType));
        $smarty->assign("periodValue", set_post($this->periodValue));
        $smarty->assign("activate_periodical_job", set_post($this-> activate_periodical_job));

        $smarty->assign("years",       set_post($years));
        $smarty->assign("months",      set_post($months));
        $smarty->assign("days",        set_post($days));
        $smarty->assign("hours",       set_post($hours));
        $smarty->assign("minutes",     set_post($minutes));
        $smarty->assign("seconds",     set_post($seconds));
        $smarty->assign("time_year",   set_post($year));
        $smarty->assign("time_month",  set_post($month));
        $smarty->assign("time_day",    set_post($day));
        $smarty->assign("time_hour",   set_post($hour));
        $smarty->assign("time_minute", set_post($minute));
        $smarty->assign("time_second", set_post($second));
        return($smarty->fetch(get_template_path('timestamp_select.tpl', TRUE, dirname(__FILE__))));
    }


    /*! \brief  HTML representation of all currently assigned targets.
      @return String Returns a listbox with all used targets.
     */
    public function get_target_list()
    {
        $data = $lData = array();

        $theme = getThemeName();

        $this->target_list_used = TRUE;
        foreach($this->a_targets as $key => $target){
            $data[$key] = $target;
            $lData[$key] = array('data' => array($target));
        }
        $this->targetList->setListData($data, $lData);
        $this->targetList->update();
        switch ($theme) {
            case 'classic':
            $list_footer = "<button type='submit' name='open_target_list'>"._("Add")."</button>";
                break;

            default:
                $list_footer = "<button class='btn-small' type='submit' name='open_target_list'>"._("Add")."</button>";
                break;
        }
        return($this->targetList->render().$list_footer);
    }


    /*! \brief  Returns HTML content, displaying a dialog which allows to add new targets.
      @return String HTML content. (EventAddSystemDialog)
     */
    public function get_target_add_list()
    {
        $this->target_add_list_used = TRUE;

        if($this->target_listing == NULL){
            $this->target_listing = new EventAddSystemDialog($this->config,$this);
        }

        $smarty = get_smarty();
        $smarty->assign("listing",$this->target_listing->execute());
        return($smarty->fetch(get_template_path('target_list.tpl', TRUE, dirname(__FILE__))));
    }


    /*! \brief  Handles all posted HTML data, including target add,remove...
     */
    public function save_object()
    {
        if(isset($_POST['open_target_list'])){
            $this->target_add_list_used =TRUE;
        }
        if($this->target_listing != NULL){
            $this->target_listing->save_object();
        }
        if($this->target_add_list_used){
            if(isset($_POST['abort_target_dialog'])){
                $this->target_add_list_used =FALSE;
                $this->target_listing = NULL;
            }
            if(isset($_POST['save_target_dialog'])){
                $this->target_add_list_used =FALSE;
                $this->add_targets($this->target_listing->get_selected_targets());
                $this->target_listing = NULL;
            }
        }

        if($this->time_select_used){
            $time_stamp_values_found = TRUE;
            foreach(array("time_year","time_month","time_day","time_hour","time_minute","time_second") as $attr){
                $time_stamp_values_found &= isset($_POST[$attr]);
            }
            if($time_stamp_values_found){
                $this->timestamp = mktime(
                        get_post('time_hour'),
                        get_post('time_minute'),
                        get_post('time_second'),
                        get_post('time_month'),
                        get_post('time_day'),
                        get_post('time_year'));

                if(isset($_POST['periodValue'])) $this->periodValue = get_post('periodValue');
                if(isset($_POST['periodType'])) $this->periodType = get_post('periodType');

                if(isset($_POST['activate_periodical_job'])){
                    $this->activate_periodical_job = TRUE;
                }else{
                    $this->activate_periodical_job = FALSE;
                }
            }
        }

        $this->targetList->save_object();
        $action = $this->targetList->getAction();
        if($action['action'] == 'delete'){
            $id = $this->targetList->getKey($action['targets'][0]);
            unset($this->a_targets[$id]);
        }
    }


    /*! \brief  Converts a daemon timestamp into an unix timestamp. \
      e.g.  20080101125959 -> 1199188799
      @param  A daemon timestamp  YYYYddmmHHiiss
      @return Integer  A unix timestamp.
     */
    public function _event_to_timestamp($str)
    {
        return(strtotime($str));
    }


    /*! \brief  Converts a unix timestamp in to a gosa-si timestamp. \
      e.g.  1199188799 -> 20080101125959
      @param  A unix timestamp (e.g. 1199188799)
      @return Integer  A daemon timestamp (e.g. 20080101125959).
     */
    public function _timestamp_to_event($stamp)
    {
        return(date("YmdHis",$stamp));
    }


    /*! \brief  Returns event information, like menu strings, images ...
      @return   Array Event informations.
     */
    public function get_event_info()
    {
        $data =array();
        $data['CLASS_NAME']   = get_class($this);
        $data['s_Menu_Name']  = $this->s_Menu_Name;
        $data['s_Event_Name'] = $this->s_Event_Name;
        foreach(array("s_Queued_Action","s_Schedule_Action","s_Trigger_Action") as $attr){
            if(!empty($this->$attr)){
                $data[$attr]  = $this->$attr;
            }
        }
        $data['MenuImage']    = "<img src='".$this->s_Menu_Image."' alt='".$this->s_Menu_Name."' border='0' class='center'>";
        $data['ListImage']    = "<img src='".$this->s_List_Image."' title='".$this->s_Event_Name."'
            alt='".$this->s_Event_Name."' border='0' class='center'>";
        return($data);
    }


    /*! \brief  Check if we have opened the target add dialog.
      @return   Boolean TRUE if we have opened the target add dialog else FALSE.
     */
    protected function is_target_list_open()
    {
        return($this->target_add_list_used);
    }


    /*! \brief  Returns a complete list of all available events.
      @return   Array   Containing $this->get_event_info() for all available events.
     */
    static function get_event_types_by_category($categories)
    {
        $types= array();

        foreach ($categories as $category) {
            if (preg_match('/^users$/', $category)) {
                $types= array_merge($types, DaemonEvent::get_event_types(USER_EVENT));
            }
            if (preg_match('/^systems$/', $category)) {
                $types= array_merge($types, DaemonEvent::get_event_types(SYSTEM_EVENT));
            }
        }

        return $types;
    }


    /*! \brief  Returns a complete list of all available events.
      @return   Array   Containing $this->get_event_info() for all available events.
     */
    static function get_event_types($type)
    {
        global $class_mapping,$config;
        $list = array();
        $list['BY_CLASS']  = array();
        $list['TRIGGERED'] = array();
        $list['SCHEDULED'] = array();
        $list['QUEUED']    = array();

        foreach($class_mapping as $name => $path){
            if(preg_match("/^DaemonEvent_/",$name)){
                $tmp  = new $name($config);
                if($tmp->visible_for & $type){
                    $evt  = $tmp->get_event_info();
                    $list['BY_CLASS'][$name]                      = $evt;
                    if(isset($evt['s_Trigger_Action'])){
                        $list['TRIGGERED'][$name] = $evt;
                        $list['QUEUED'][$evt['s_Trigger_Action']] = $name;
                    }
                    if(isset($evt['s_Schedule_Action'])){
                        $list['SCHEDULED'][$name] = $evt;
                        $list['QUEUED'][$evt['s_Schedule_Action']] = $name;
                    }
                    if(isset($evt['s_Queued_Action'])){
                        $list['QUEUED'][$evt['s_Queued_Action']] = $name;
                    }
                }
            }
        }
        return($list);
    }


    /*! \brief  Returns TRUE if this event is new. (Not edited)
      @return Boolean TRUE if new, else FALSE.
     */
    public function is_new()
    {
        return($this->is_new);
    }


    /*! \brief  Returns the event tag to schedule a new action
      @param    Returns the event e.g. 'job_trigger_action_wake'
     */
    public function get_schedule_action()
    {
        return($this->s_Schedule_Action);
    }


    /*! \brief  Returns the event tag to schedule a new action
      @param    Returns the event e.g. 'trigger_action_wake'
     */
    public function get_trigger_action()
    {
        return($this->s_Trigger_Action);
    }


    /*! brief  Returns an array containig all attributes \
      That should be written.
      @return Array e.g. 'status' => 'bla blub'
     */
    public function save()
    {
        $ret = array();
        foreach($this->attributes as $attr){
            $ret[$attr] = $this->$attr;
        }
        if($this->mode == SCHEDULED_EVENT){
            $ret['timestamp'] = $this->_timestamp_to_event($this->timestamp);
        }elseif(isset($ret['timestamp'])){
            unset($ret['timestamp']);
        }

        if(isset($ret['periodic'])){
            unset($ret['periodic']);
        }
        if($this->activate_periodical_job){
            $ret['periodic']= $this->periodValue."_".$this->periodType;
        }

        return($ret);
    }


    /*! \brief  Returns the event targets
      @return Array  All selected targets.
     */
    public function get_targets()
    {
        return($this->a_targets);
    }


    /*! \brief  Returns the event timestamp in GOsa daemon format.
      @return Returns the event timestamp (20081231120000)
     */
    public function get_timestamp($si_type = TRUE)
    {
        if($si_type){
            return($this->_timestamp_to_event($this->timestamp));
        }else{
            return($this->timestamp);
        }
    }


    /*! \brief  Returns the event ID
      @return Returns the event ID
     */
    public function get_id()
    {
        if($this->is_new){
            return(-1);
        }else{
            return($this->data['ID']);
        }
    }


    /*! \brief Add a target MAC address
      @param Array A List of all target that should be added.
     */
    public function set_timestamp($stamp)
    {
        $this->timestamp = $stamp;
    }


    /*! \brief Add a target MAC address
      @param Array A List of all target that should be added.
     */
    public function add_targets($targets)
    {
        foreach($targets as $target){
            $this->a_targets[] = $target;
        }
    }

    public function check()
    {
        return(array());
    }


    /*! \brief Update a class variable from outside
     */
    public function set_value($name,$value)
    {
        $name = strtolower($name);
        if(isset($this->$name) && in_array_strict($name,$this->attributes)){
            $this->$name = $value;
        }
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
