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
    var $graphIDs = array();            // IDs for created grpah
    var $skipSeries = array();          // Graph series which should be skipped
    var $seriesListPerGraph = array();  // Series names per graph

    // Legend colors
    var $legendR = 235;
    var $legendG = 235;
    var $legendB = 235;

    // Font used in graphs
    var $font = "./themes/default/fonts/LiberationSans-Regular.ttf";

    // Datepicker initial
    var $graph1DatePicker1 = 0;
    var $graph1DatePicker2 = 0;

    // A collection opf timestamps for unsubmitted statistics data.
    var $unsbmittedFiles = array();

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
        $this->graphs[] = new pieChart1($config); 
        $this->graphs[] = new pieChart2($config); 

        // Init start and stop times for graph 1
        $this->graph1DatePicker1 = date('d.m.Y', time() - 14 * 24 * 60 *60);
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

        // Get list of unsubmitted files.
        $this->unsbmittedFiles = $this->getUnsubmittedStatistics();

        // Collect category translations
        $this->catTranslations = array();
        foreach($this->config->configRegistry->getListOfPlugins() as $plugin => $data){
            if(isset($data['plCategory'])){
                foreach($data['plCategory'] as $id => $name){
                    if(!is_numeric($id)){
                        $this->catTranslations[$id] = $name['description'];
                    }
                }
            }
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
        $available = $this->getLocalStatisticsFiles();
        $alreadyTransmitted = $this->getStatisticsDatesFromServer();

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

        // Assign list of graphs 
        $tmp = array();
        foreach($this->graphs as $id => $gClass){
            $tmp[$id] = $gClass->getTitle();
        }
        $smarty->assign("selectedGraphType", $this->selectedGraphType);
        $smarty->assign("availableGraphs", $tmp);


        // Do not render anything if we are not prepared to send and receive data via rpc.
        $smarty->assign("rpcConfigured", $this->rpcConfigured);
        $smarty->assign("validRpcHandle", TRUE);
        if(!$this->rpcHandle){
            $smarty->assign("validRpcHandle", FALSE);
            return($smarty->fetch(get_template_path('statistics.tpl', TRUE)));
        }

        // Send stats 
        if(isset($_POST['transmitStatistics'])){
            $this->unsbmittedFiles = $this->getUnsubmittedStatistics();
            foreach($this->unsbmittedFiles as $filename => $date){
                $tmp = stats::dumpTables($filename);
                $dump = array();
                foreach($tmp as $entry){
                    $dump[] = array_values($entry);
                }
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
        if(isset($_POST['receiveStatistics']) && $this->rpcConfigured){
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
        
        $this->reloadGraphs();

        $curGraph = $this->graphs[$this->selectedGraphType];
        $smarty->assign('curGraphID', $curGraph->getGraphID());
        $smarty->assign('curSeriesSelector', $curGraph->getSeriesSelector());
        $smarty->assign('skipSeries', $this->skipSeries);
        $smarty->assign('graphIDs', $this->graphIDs);
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
        $this->skipSeries[1] = array();
        $this->skipSeries[2] = array();
        $this->skipSeries[3] = array();
        $this->skipSeries[4] = array();
        $this->skipSeries[5] = array();


        // Clean data from unusable categories like ('terminals workstations, ...')
        foreach($gData as $serieName => $seriesData){
            foreach($seriesData as $key => $data){
                $list = preg_split("/, /", $key);
                if(count(array_intersect(array('server','terminal','workstation', 'opsi', 'component'), $list))){
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

        $curGraph = $this->graphs[$this->selectedGraphType];
        $curGraph->save_object();
    }


    /*! \brief      This method tries to translate category names.
     *  @param      The category name to translate
     *  @return     String  The translated category names.
     */
    function getCategoryTranslation($name)
    {
        $ret ="";

        // We do not have a category for systems directly, so we've to map all system types to 'System'.
        // If we do not map to _(Systems) the graph legend will be half screen width.
        if($name == "systems"){
            return(_("Systems"));
        }

        // Walk through category names and try to find a translation.
        $cat = trim($name);
        if(isset($this->catTranslations[$cat])){
            $cat = _($this->catTranslations[$cat]);
        }elseif(!empty($cat)){
            $cat = _($cat);
        }
        return($cat); 
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
#        $this->generateActionsGraph($gData);

#       // Generate graph which displays the render time
#       $series = array(
#           'max_render' => _('Max'),
#           'avg_render' => _('Avergae'),
#           'min_render' => _('Min'));
#       $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("Render time"),5);
#
#       // Generate graph which displays the plugin duration
#       $series = array(
#           'max_dur' => _('Max'),
#           'avg_dur' => _('Avergae'),
#           'min_dur' => _('Min'));
#       $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("Seconds per action"),6);
    }

   


    /*! \brief  Generate the pie-chart which displays the overall-plugin-usage
     */
    function generatePieGraph($data, $gID, $title)
    {
        $this->graphIDs[$gID] = 0;
        if(!count($data)) return;

        // Sort data by usage count and slice array to get 
        //  the eight most used categories
        arsort($data);
        
        // Detect max value and throw out entries less than 1% 
        //  the will else be unreadable
        $max = max($data);
        $pos = 0;
        foreach($data as $key => $count){
            if($pos >=7 || ($count / $max)*100 < 1) break;
            $pos ++;
        }
 
        $dataRes = array_slice($data,0,$pos);

        // Get the rest of categories and combine them 'others'
        $theRest = array_slice($data,$pos);
        $dataRes['remaining'] = array_sum($theRest);

        // Try to find a translation for the given category names
        $values = array_values($dataRes);
        $keys = array_keys($dataRes);
        foreach($keys as $id => $cat){
            $keys[$id] = $this->getCategoryTranslation($cat);
        }

        // Dataset definition   
        $dataSet = new pData;  
        $dataSet->AddPoint($values,"Serie1");  
        $dataSet->AddAllSeries();  
        $dataSet->AddPoint($keys,"Serie2");  
        $dataSet->SetAbsciseLabelSerie("Serie2");  

        // Read graph from cache?
        $pCache = new pCache('/var/spool/gosa/');
        if($pCache->IsInCache($gID,$dataSet->GetData())){
            $filename =  $pCache->GetHash($gID,$dataSet->GetData());
            $filename = '/var/spool/gosa/'.$filename;    
            if(file_exists($filename) && is_readable($filename)){
                $this->graphIDs[$gID] = preg_replace("/[^0-9]/","",microtime(TRUE)).rand(0,99999);
                session::set('statistics::graphFile'.$this->graphIDs[$gID],$filename);
                return;
            }
        }

        // Set graph area
        $x = 400;
        $y = 200;

        // Initialise the graph  
        $chart = new pChart($x,$y);  
        $chart->setFontProperties($this->font,10);  
        $chart->drawPieGraph($dataSet->GetData(),$dataSet->GetDataDescription(),($x/3),($y/2)-10,($y/2),PIE_PERCENTAGE,TRUE,50,20,6);  
        $chart->drawPieLegend(($x/3*2),15,$dataSet->GetData(),$dataSet->GetDataDescription(),
                $this->legendR,$this->legendG,$this->legendB);

        // Store graph data
        $this->graphIDs[$gID] = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphIDs[$gID];
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphIDs[$gID],$file);
        $pCache->WriteToCache($gID,$dataSet->GetData(),$chart);   
    }
}
?>
