<?php

class gotoLogView extends plugin
{

    var $mac;
    var $event;
    var $parent;
    var $config;

    var $o_queue = NULL;  

    var $selected_date;
    var $selected_date_str;
    var $selected_file = 0;

    var $attributes = array("macAddress");
    var $macAddress = "";

    var $sort_by  = "time";
    var $sort_dir = 1; // 1 => up, 0 => down

    var $ignore_account = TRUE;
    var $standalone = FALSE;

    var $logs = array();
    var $logSelector = NULL;

    function __construct(&$config,$dn,$parent)
    {
        $this->config = $config;
        $this->parent = $parent;
    
        $this->initTime = microtime(TRUE);

        /* Load ldap object if given 
           and use this macAddress.
         */
        if(is_object($parent) && $dn != "" && $dn != "new"){
            plugin::__construct($config,$dn,$parent);
        } 

        /* Get correct macAddress.
           Check if an event is given or a ldap object.
         */      
        if(is_array($this->parent) && isset($this->parent['MACADDRESS'])){
            $this->mac = $this->parent['MACADDRESS'];
            $this->standalone = TRUE;
        }elseif(isset($parent->attrs['macAddress'][0])){
            $this->mac = $parent->attrs['macAddress'][0];
            $this->standalone = FALSE;
        }

        // Create the filter list
        $this->logSelector= new sortableListing($this->logs, $this->convertFilterList($this->logs));
        $this->logSelector->setDeleteable(false);
        $this->logSelector->setEditable(true);
        $this->logSelector->setAcl("rwcdm");
        $this->logSelector->setWidth("100%");
        $this->logSelector->setHeight("220px");
        $this->logSelector->setHeader(array(_("Date"),_("Filename")));
        $this->logSelector->setColspecs(array('80px', '100px', '200px', '120px','150px'));
    }


    /*! \brief    Converts the list of filters ($this->filters) into data which is useable
     *             for the sortableList object ($this->filterWidget).
     *  @return   Array   An array containg data useable for sortableLists ($this->filterWidget)
     */
    function convertFilterList($logs)
    {
        $data = array();
        foreach($logs as $key => $log){
            $data[$key] = array('data' => 
                    array(date('d.m.Y H:i:s',$log['DATE']),$log['FILE']));
        }
        return($data);
    }


    function execute()
    {
        plugin::execute();

        // Act on edit requests
        $this->logSelector->save_object();
        $action = $this->logSelector->getAction();

        if(isset($action['action']) && $action['action'] == 'edit'){
            $id = $action['targets'][0];
            $entry = $this->logs[$id];
            $this->selected_file = $entry['FILE'];
            $this->selected_date = $entry['DATE'];
            $this->selected_date_str = $entry['DATE_STR'];
        }


        $smarty = get_smarty();
        $smarty->assign("logs",            $this->logs);   
        $smarty->assign("logs_available",  count($this->logs));
        $smarty->assign("mac",             set_post($this->mac));
        $smarty->assign("selected_file",   set_post($this->selected_file));
        $smarty->assign("selected_date",   set_post($this->selected_date));
        $smarty->assign("log_file",        $this->get_log($this->mac,$this->selected_date_str,$this->selected_file));        
        $smarty->assign("standalone",      $this->standalone);
        if (isset($this->logs[$this->mac])){
            $date = date("d.m.Y H:i:s",$this->logs[$this->mac][$this->selected_date]['REAL_DATE']);
        }
        $file = $this->selected_file;
        $smarty->assign("selected_log",_("none"));
        if(!empty($file)){
            $smarty->assign("selected_log", $file.", ".date('d.m.Y H:i:s', $this->selected_date));
        }
        $smarty->assign("ACL",preg_match("/r/",$this->getacl("")));
        $this->logSelector->setAcl($this->getacl(""));
        $smarty->assign("listing", $this->logSelector->render());
        return($smarty->fetch(get_template_path('log_view.tpl', TRUE,dirname(__FILE__))));
    }


    function get_log($mac,$date,$file)
    {
        $res = nl2br(htmlentities(""));
        return($res);
    }


    function save_object()
    {
        foreach(array("time"=>"selected_date","file"=>"selected_file") as $attr => $dest){
            if(isset($_GET[$attr])){
                $this->$dest = $_GET[$attr];
            }
        }
        if(isset($_GET['sort_by']) && in_array_strict($_GET['sort_by'],array("file","time"))){
            if($_GET['sort_by'] == $this->sort_by){
                $this->sort_dir = !$this->sort_dir;
            }
            $this->sort_by = $_GET['sort_by'];
        }
    }


    /* Return plugin informations for acl handling */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Log view"),
                    "plDescription" => _("GOto log view"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 30,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("workstation","server"),

                    "plProvidedAcls"=> array()
                    ));
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
