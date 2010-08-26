<?php

class statistics extends plugin
{
    var $plHeadline = 'Statistics';
    var $plDescription = 'GOsa usage statistics';
    var $plShortIcon = 'statistics.png';
    var $plIcon = 'plugin.png';
    
    var $rpcHandle = NULL;
    var $rpcConfigured = FALSE;

    // Graph data 
    var $statisticData = array();       // Via rpc received stats

    // Font used in graphs
    var $font = "./themes/default/fonts/LiberationSans-Regular.ttf";

    // Datepicker initial
    var $graph1DatePicker1 = 0;
    var $graph1DatePicker2 = 0;

    // A collection of timestamps for unsubmitted statistics data.
    var $unsbmittedFiles = NULL;

    var $graphs = array();
    var $selectedGraphType = 0;

    function __construct($config)
    {
        plugin::plugin($config, NULL);

        $this->graphs[] = new categoryActionsOverTime($config); 
        $this->graphs[] = new memoryUsageChart($config); 
        $this->graphs[] = new cpuLoadChart($config); 
        $this->graphs[] = new renderTimeChart($config); 
        $this->graphs[] = new durationTimeChart($config); 
        $this->graphs[] = new objectCountChart($config); 
        $this->staticChart1 = new pieChart1($config); 
        $this->staticChart2 = new pieChart2($config); 

        // Init start and stop times for graph 1
        $this->graph1DatePicker1 = date('d.m.Y', time() - 28 * 24 * 60 *60);
        $this->graph1DatePicker2 = date('d.m.Y', time());

        // First try to retrieve values via RPC
        $this->rpcConfigured = FALSE;
        if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){
            $this->rpcConfigured = TRUE;
            $this->rpcHandle = $this->config->getRpcHandle(
                    "http://10.3.64.59:4000",
                    "65717fe6-9e3e-11df-b010-5452005f1250",
                    "WyukwauWoid2",
                    TRUE);
        }
    }


    /*! \brief      Returns a list local stored statistic files
        @param      Array   A list of filenames and dates.
     */ 
    function getLocalStatisticsFiles()
    {
        $res = stats::getLocalStatFiles();
        $tmp = array();
        if(count($res)){
            foreach($res as $file){
                $date = strtotime($file);
                if($date){
                    $tmp[$file] = $date;
                }
            }
        }
        return($tmp);
    }

   
    /*! \brief      Returns a list of not transmitted stat files (except files for the current day)
     *  @return     Array   A list of unsubmitted statistic files.
     */ 
    function getUnsubmittedStatistics()
    {
        $alreadyTransmitted = $this->getStatisticsDatesFromServer();
        if($alreadyTransmitted == NULL){
            return(NULL);
        }

        $available = $this->getLocalStatisticsFiles();

        $unsubmitted = array();
        foreach($available as $key => $day){
            if(!isset($alreadyTransmitted[$key])) $unsubmitted [$key] = $day;
        }

        // Exclude statistic collection from today, they are still active and cannot be submitted.
        $curDate =  date('Y-m-d');
        if(isset($unsubmitted)) unset($unsubmitted[$curDate]);
        return($unsubmitted);  
    }


    /*! \brief      Request a list of dates for which the server can return statistics.
        @param      Array   A list of dates    $ret=[iso-str] = timestamp
     */ 
    function getStatisticsDatesFromServer()
    {
        // Do not request anything while rpc isn't configured.
        if(!$this->rpcConfigured){
            return(array());
        }
        
        // Try to gather statistic dates from the backenbd.
        $res = $this->rpcHandle->getInstanceStatDates();
        $dates = array();
        if(!$this->rpcHandle->success()){
            msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
            return(NULL);
        }else{
            foreach($res as $date){
                $dates[$date] = strtotime($date);
            }
        }
        $this->rpcHandle_Error = !$this->rpcHandle->success();
        return($dates);
    }


    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('graph1DatePicker1', $this->graph1DatePicker1);
        $smarty->assign('graph1DatePicker2', $this->graph1DatePicker2);

        // Get list of unsubmitted files.
        if($this->unsbmittedFiles == NULL){
            $this->unsbmittedFiles = $this->getUnsubmittedStatistics();
        }

        // Do not render anything if we are not prepared to send and receive data via rpc.
        $smarty->assign("rpcConfigured", $this->rpcConfigured);
        $smarty->assign("validRpcHandle", TRUE);
        if(!$this->rpcHandle){
            $smarty->assign("validRpcHandle", FALSE);
            return($smarty->fetch(get_template_path('statistics.tpl', TRUE)));
        }

        // Assign list of selectable graphs 
        $tmp = array();
        foreach($this->graphs as $id => $gClass){
            $tmp[$id] = $gClass->getTitle();
        }
        $smarty->assign("selectedGraphType", $this->selectedGraphType);
        $smarty->assign("availableGraphs", $tmp);

        // Send stats 
        if(isset($_POST['transmitStatistics'])){
            $this->unsbmittedFiles = $this->getUnsubmittedStatistics();
            foreach($this->unsbmittedFiles as $filename => $date){
                $tmp = stats::generateStatisticDump($filename);
                $dump = array();
                foreach($tmp as $entry){
                    $dump[] = array_values($entry);
                }
                $objectCount = stats::getLdapObjectCount($this->config, TRUE);
                $dump = array_merge($dump, $objectCount);
                $res = $this->rpcHandle->updateInstanceStatus($dump);
                if(!$this->rpcHandle->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
                }else{
                    stats::removeStatsFile($filename);
                }
                $this->rpcHandle_Error = !$this->rpcHandle->success();
            }
            $this->unsbmittedFiles = $this->getUnsubmittedStatistics();
        }

        // Transmit daily statistics to GOsa-Server
        if((isset($_POST['receiveStatistics']) || !count($this->statisticData)) && $this->rpcConfigured){
            $start = strtotime($this->graph1DatePicker1);
            $stop  = strtotime($this->graph1DatePicker2);
            $res = $this->rpcHandle->getInstanceStats($start,$stop);
            if(!$this->rpcHandle->success()){
                msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
            }elseif($res){
                $this->statisticData = $this->prepareGraphData($res); 
            }
            $this->rpcHandle_Error = !$this->rpcHandle->success();
        }
       
        // Update graphs 
        $this->reloadGraphs();

        $smarty->assign('staticChart1_ID', $this->staticChart1->getGraphID());
        $smarty->assign('staticChart2_ID', $this->staticChart2->getGraphID());

        $curGraph = $this->graphs[$this->selectedGraphType];
        $smarty->assign('curGraphID', $curGraph->getGraphID());
        $smarty->assign('curSeriesSelector', $curGraph->getSeriesSelector());
        $smarty->assign('unsbmittedFiles', count($this->unsbmittedFiles));
        $smarty->assign('unsbmittedFilesMsg', sprintf(
                    _("You have currently %s unsubmitted statistic collection, do you want to transmit them now?"),
                    count($this->unsbmittedFiles)));
    
        $smarty->assign('rpcHandle_Error', $this->rpcHandle_Error);
        return($smarty->fetch(get_template_path('statistics.tpl', TRUE)));
    }


    /*! \brief      Prepares the graph data we've received from the rpc-service.
     *              This method will construct a usable data-array with converted 
     *               date strings.
     */
    function prepareGraphData($res)
    {   
        // Object categories which has to mapped to 'systems'
        $mapSystems = array('server','terminal','workstation', 'opsi', 
                'component','phone', 'winworkstation', 'printer', 'incoming');

        /* Build up array which represents the amount of errors per
         *  interval.
         */
        $gData = array();
        foreach($res['errorsPerInterval'] as $dateStr => $data){
            $date = strtotime($dateStr);
            $gData['errorsPerInterval'][$date] = $data;
        }
        ksort($gData['errorsPerInterval']);


        /* Build up timeline
         */
        $Xam = 5; 
        $cnt = 0;
        $numCnt = $res['errorsPerInterval'];
        foreach($gData['errorsPerInterval'] as $date => $data){
            if((count($numCnt) <= $Xam) || 
                    ($cnt % (floor(count($numCnt) / $Xam )) == 0)){
                $gData['dates'][$date] = date('d.m.Y', $date);
            }else{
                $gData['dates'][$date] = ' ';
            }
            $cnt ++;
        }
        ksort($gData['dates']);
        
        /* Build up 'actions per category' array, this will later
         *   be represented using a pie chart.
         */
        $gData['actionsPerCategory'] = $res['actionsPerCategory'];
        arsort($gData['actionsPerCategory']);


        /* Build up system-info array per interval.
         */
        foreach($res['usagePerInterval'] as $dateStr => $data){
            $date = strtotime($dateStr);
            foreach($data as $type => $count){
                $gData['usagePerInterval'][$type][$date] = $count;
            }
        }
        foreach($gData['usagePerInterval'] as $key => $data)
            ksort($gData['usagePerInterval'][$key]);


        /* Prepare actions-per-interval array.
         */   
        $gData['actionsPerInterval'] = array(); 
        foreach($res['actionsPerInterval'] as $category => $data){
            if(empty($category)) continue;
            foreach($data as $dateStr => $count){
                $date = strtotime($dateStr);
                $gData['actionsPerInterval'][$category][$date]=$count;
            }
            ksort($gData['actionsPerInterval'][$category]);
        }

        /* Prepare object count per interval array.
         */   
        $gData['objectCountPerInterval'] = array(); 
        foreach($res['objectCountPerInterval'] as $category => $data){
            if(empty($category)) continue;
            if(in_array($category,$mapSystems)){
                $category = 'systems';
            }

            // Skip series which are not interesting for us
            if(!in_array($category,array('users','groups','department','systems','ogroups','fai'))){
                $category = 'remaining';
            }

            foreach($data as $dateStr => $count){
                $date = strtotime($dateStr);

                if(!isset($gData['objectCountPerInterval'][$category][$date])){
                    $gData['objectCountPerInterval'][$category][$date]=0;
                }
                $gData['objectCountPerInterval'][$category][$date] += $count;
            }
            ksort($gData['objectCountPerInterval'][$category]);
        }
        // Move remaining to the end of the list
        if(isset($gData['objectCountPerInterval']['remaining'])){
            $data = $gData['objectCountPerInterval']['remaining'];
            unset($gData['objectCountPerInterval']['remaining']);
            $gData['objectCountPerInterval']['remaining'] = $data;
        }

        // Clean data from unusable categories like ('terminals workstations, ...')
        foreach($gData as $serieName => $seriesData){
            foreach($seriesData as $key => $data){
                $list = preg_split("/, /", $key);
                if(count(array_intersect($mapSystems, $list))){
                    unset($gData[$serieName][$key]);
                    $gData[$serieName]['systems'] = $data;
                }
            }
        }

        // Get action usage, to be able to render a pie chart about most done actions.
        $gData['actionsPerPluginAction'] = $res['actionsPerPluginAction'];
        return($gData);
    }


    function check()
    {
        $messages = plugin::check();
        return($messages);
    }


    function save_object()
    {
        plugin::save_object();
        if(isset($_POST['graph1DatePicker1'])) $this->graph1DatePicker1 = get_post('graph1DatePicker1');
        if(isset($_POST['graph1DatePicker2'])) $this->graph1DatePicker2 = get_post('graph1DatePicker2');
   
        if(isset($_POST['selectedGraphType'])) $this->selectedGraphType = get_post('selectedGraphType');

        $this->staticChart1->save_object();
        $this->staticChart2->save_object();

        $curGraph = $this->graphs[$this->selectedGraphType];
        $curGraph->save_object();
    }


    /*! \brief  Reload the graph images.
     */ 
    function reloadGraphs()
    {
        new pChartInclude();

        $gData = $this->statisticData;
        if(!count($gData)){
            return;
        }
        $curGraph = $this->graphs[$this->selectedGraphType];
        $curGraph->setGraphData($gData);
        $curGraph->render();

        $this->staticChart1->setGraphData($gData);
        $this->staticChart1->render();
        $this->staticChart2->setGraphData($gData);
        $this->staticChart2->render();
    }
}
?>
