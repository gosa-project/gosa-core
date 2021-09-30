<?php

class goto_import_file extends plugin
{

    var $events     = array();
    var $csv_fields = array();
    var $import_successful = FALSE; // Indicates that we have successfully imported everything.

    public function __construct($config,&$parent)
    {
        plugin::__construct($config,NULL);
        $this->parent = $parent;
        $this->daemon_events  = DaemonEvent::get_event_types( SYSTEM_EVENT | HIDDEN_EVENT);

        $this->csv_fields = array(
                "0"=>"TIMESTAMP","1" => "MAC",  "2" => "HEADER", "3" => "OGROUP",
                "4" => "BASE", "5" => "FQDN",   "6" => "IP",     "7" => "DHCP");
    }


    private function parse_csv($str)
    {

        /* Some file checks 
         */
        $lines = preg_split("/\n/",$str);
        if(empty($str) || !count($lines)){
            msg_dialog::display(_("Import"), msgPool::incorrectUpload(_("file is empty")),ERROR_DIALOG);
            return;
        }

        /* Reset current events 
         */
        $this->events = array(); 

        /* Parse each line of the given file
         */
        foreach($lines as $line){

            // Skip empty lines.
            if(empty($line)) continue;

            /* Load values from file 
             */
            $fields = explode(";",$line);
            $event = array();
            foreach($this->csv_fields as $key => $val) {
                $event[$val] = "";
                if(isset($fields[$key])){
                    $event[$val] = $fields[$key];
                }
            }
            $this->events[] = $event;
        }
        $this->check_fields();
    }


    public function execute()
    {
        plugin::execute();

        /* Import file 
         */
        if(isset($_POST['import']) && isset($_FILES['file'])) {
            if(isset($_FILES['file']['tmp_name'])){
                $filename = $_FILES['file']['tmp_name'];
                $str = file_get_contents(gosa_file_name($filename));
                $this->parse_csv($str);
            }
        }

        /* Import started 
         */
        $confirmed = FALSE;
        foreach($_POST as $name => $value){ 
            if(preg_match("/^MSG_OK/",$name)){
                $confirmed = TRUE;
            }
        }
        if(isset($_POST['start_import']) || $confirmed){
            $error = FALSE;
            if(!$confirmed){
                foreach($this->events as $event){
                    if(!empty($event['ERROR'])){
                        $error = TRUE;
                        break;
                    } 
                }
                if($error){
                    msg_dialog::display(_("Import"),
                            _("Selected entries will be skipped because of errors. Do you want to proceed?"),CONFIRM_DIALOG);
                }
            }
            if(!$error){

                $success = 0;
                $fail = 0;

                foreach($this->events as $key => $event){
                    if(!empty($event['ERROR'])){  
                        $fail ++;
                        continue;
                    }

                    /* Create event 
                     */
                    $class= $this->daemon_events['QUEUED'][$event['HEADER']];
                    $o_data = $this->daemon_events['BY_CLASS'][$class];
                    $object = new $class($this->config);
                    $object->add_targets(array($event['MAC']));
                    if($o_data['s_Schedule_Action'] == $event['HEADER']){
                        $object->set_type(SCHEDULED_EVENT);
                    }else{
                        $object->set_type(TRIGGERED_EVENT);
                    }

                    /* Update values like fqdn a.s.o 
                     */
                    foreach($this->csv_fields as $name){
                        if($name == "TIMESTAMP" && empty($event[$name])) continue;
                        $object->set_value($name,$event[$name]);
                    }

                    if(!$this->parent->o_queue->append($object)){
                        msg_dialog::display(_("Service infrastructure"),msgPool::siError($this->parent->o_queue->get_error()),ERROR_DIALOG);
                        $fail ++;
                    }else{
                        unset($this->events[$key]);
                        $success ++;
                    }
                }
                msg_dialog::display(_("Import"),sprintf(_("Import complete: %s events successfully send, %s failed"),$success,$fail),INFO_DIALOG);
                $this->import_successful = count($this->events) == 0;
            }
        }


        /* Prepare output 
         */
        $evts = $this->events;
        foreach($this->events as $id => $evt){
            foreach($evt as $key => $val){
                if(in_array_strict($key,$this->csv_fields)){
                    $evts[$id][$key] = "<span style=\"white-space: nowrap;\">".strip_tags($val)."</span>"; 
                }
            }
        }

        $smarty = get_smarty();
        $smarty->assign("info",$evts);
        $smarty->assign("count",count($evts));
        return($smarty->fetch(get_template_path('goto_import_file.tpl', TRUE)));
    }


    public function check()
    {
        $message = plugin::check();
        $this->check_fields();
        return($message);
    }


    private function check_fields()
    {
        foreach($this->events as $key => $event){
            $this->events[$key]['ERROR'] = "";
            if(empty($event['MAC']) || !tests::is_mac($event['MAC'])){
                $this->events[$key]['ERROR'] .=  msgPool::invalid(_("MAC")).", ";
            }
            if(empty($event['HEADER']) || !isset($this->daemon_events['QUEUED'][$event['HEADER']])){
                $this->events[$key]['ERROR'] .=  msgPool::invalid(_("Event")).", ";
            }
            $this->events[$key]['ERROR'] = trim($this->events[$key]['ERROR'],", ");
        }
    } 
} 

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
