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

    function __construct($config)
    {
        plugin::plugin($config, NULL);

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

        $this->reloadGraphs();

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
                $this->reloadGraphs();
            }
            $this->rpcHandle_Error = !$this->rpcHandle->success();
        }


        $smarty->assign('seriesListPerGraph', $this->seriesListPerGraph);
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

    
        // Get series to enable or disable
        foreach($this->seriesListPerGraph as $graphId => $series){
            $this->skipSeries[$graphId] = array();
            if(isset($_POST["graphPosted_{$graphId}"])){
                foreach($series as $seriesName => $seriesDesc){
                    if(!isset($_POST["addSeries_{$graphId}_{$seriesName}"])){
                        $this->skipSeries[$graphId][] = $seriesName;
                    }
                }
            }
        }
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
        if(count($gData['actionsPerCategory'])){
            $this->generateCategoryPieGraph($gData['actionsPerCategory']);
        }
        $this->generateActionsGraph($gData);

        // Generate graph which displays the memory usage over time
        $series = array(
            'max_mem' => _('Max'),
            'avg_mem' => _('Avergae'),
            'min_mem' => _('Min'));
        $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("Memory usage"),3);

        // Generate graph which displays the cpu load over time
        $series = array(
            'max_load' => _('Max'),
            'avg_load' => _('Avergae'),
            'min_load' => _('Min'));
        $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("CPU load"),4);

        // Generate graph which displays the render time
        $series = array(
            'max_render' => _('Max'),
            'avg_render' => _('Avergae'),
            'min_render' => _('Min'));
        $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("Render time"),5);

        // Generate graph which displays the plugin duration
        $series = array(
            'max_dur' => _('Max'),
            'avg_dur' => _('Avergae'),
            'min_dur' => _('Min'));
        $this->generateSystemStatsGraph($gData,'usagePerInterval',$series, _("Seconds per action"),6);
    }


    /*! \brief  Generates the line-graph which displays the plugin usage over time.
     */ 
    function generateActionsGraph($gData)
    {
        $lineMax = 100;
        $errorMax = (max($gData['errorsPerInterval']) < 100)? 100:max($gData['errorsPerInterval']);
        $dataSet = new pData;  
        $seriesCnt = 0;
        $gID = 2;
        foreach($gData['actionsPerInterval'] as $category => $entriesPerDate){
            if(empty($category) || in_array($category, $this->skipSeries[$gID])) continue;

            // Add results to our data set.
            $dataSet->AddPoint($entriesPerDate, $category);
            $dataSet->SetSerieName($this->getCategoryTranslation($category), $category);
            $dataSet->AddSerie($category);

            // Detect maximum value, to adjust the Y-Axis
            $tmpMax = max($entriesPerDate);
            if($tmpMax > $lineMax) $lineMax = $tmpMax;
            $seriesCnt ++;
        }

        // Add timeline
        $dataSet->AddPoint($gData['dates'], 'date');
        $dataSet->SetAbsciseLabelSerie('date');  

        $chart = new pChart(800,230);  
        $chart->setFixedScale(0.000,$lineMax);
        $chart->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
        $chart->setGraphArea(50,30,585,200);  
        $chart->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
        $chart->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
        $chart->drawGraphArea(255,255,255,TRUE);  
        $chart->drawGrid(4,TRUE,200,200,200,50);  
        $chart->drawTreshold(0,143,55,72,TRUE,TRUE);  
        $chart->drawTitle(50,22,"Plugin usage over time",50,50,50,585);  
        $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, TRUE);     

        // Only draw this graph if we've at least one series to draw! 
        if($seriesCnt){
            $chart->drawFilledLineGraph($dataSet->GetData(),$dataSet->GetDataDescription(),50,TRUE);
        }

        // Do we've to add the errors series?
        // If we have to, then add the error-data-series.
        //  and set the color for the new error-series to red.
        if(!in_array('errorsPerInterval', $this->skipSeries[$gID])){

            // Set the color for the error Series to 'red'. 
            // This has to be done before drawing the legend.
            $chart->setColorPalette($seriesCnt,255,0,0);   

            $dataSet->AddPoint($gData['errorsPerInterval'], 'Errors');
            $dataSet->SetSerieName(_('Error'), 'Errors');
            $dataSet->AddSerie('Errors');
        }

        
        // Draw legend, but only if there is something to draw!
        if($seriesCnt || !in_array('errorsPerInterval', $this->skipSeries[$gID])){
            $chart->drawLegend(650,30,$dataSet->GetDataDescription(),255,255,255);  
        }

        // Draw the error graph on top of the other graphs now.
        // But remove the category-graph before. 
        if(!in_array('errorsPerInterval', $this->skipSeries[$gID])){

            // Remove all graph series and add the error-series, then draw the new graph.
            // (It is not relevant if it was really added before, so we simply remove all series!)
            foreach($gData['actionsPerInterval'] as $category => $data){
                $dataSet->RemoveSerie($category);
            }
            $chart->setFixedScale(0,$errorMax);
            $chart->drawRightScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,120,150,150,TRUE,0,2, TRUE);
            $chart->drawBarGraph($dataSet->GetData(),$dataSet->GetDataDescription());
        }

        // Generate new and unique graph id
        $this->graphIDs[$gID] = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphIDs[$gID];
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphIDs[$gID],$file);

        // Keep a list of all selecteable data-series, to allow the user to disable
        //  or enable series on demand.
        $this->seriesListPerGraph[$gID] = array();
        foreach($gData['actionsPerInterval'] as $key => $data){
            $this->seriesListPerGraph[$gID][$key] = $this->getCategoryTranslation($key); 
        }
        $this->seriesListPerGraph[$gID]['errorsPerInterval'] = _("Error");
 
        return;
    }
   

    /*! \brief  Generates a graph about system informations.
     */ 
    function generateSystemStatsGraph($gData, $key = "", $series= array(), $title = "", $gID=0 )
    { 
        // Add series data to dataSet        
        $dataSet = new pData;  
        $max = 0;
        foreach($series as $seriesName => $seriesDesc){
            if(isset($gData[$key][$seriesName])){
                $dataSet->AddPoint($gData[$key][$seriesName],$seriesName);
                $dataSet->SetSerieName($seriesDesc,$seriesName);
                if($max < max($gData[$key][$seriesName])) $max = max($gData[$key][$seriesName]);
            }
        }
        $dataSet->AddAllSeries();  
        $dataSet->AddPoint($gData['dates'], 'date');
        $dataSet->SetAbsciseLabelSerie('date');  
        
        $chart = new pChart(800,230);  
        $chart->setFixedScale(0.0001,($max*1.1));  
        $chart->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
        $chart->setGraphArea(50,30,585,200);  
        $chart->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
        $chart->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
        $chart->drawGraphArea(255,255,255,TRUE);  
        $chart->drawGrid(4,TRUE,200,200,200,50);  
        $chart->drawTreshold(0,143,55,72,TRUE,TRUE);  
        $chart->drawTitle(50,22,$title,50,50,50,585);  
        $chart->drawLegend(650,30,$dataSet->GetDataDescription(),255,255,255);  

        $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, FALSE);
        $chart->drawFilledCubicCurve($dataSet->GetData(),$dataSet->GetDataDescription(),.1,50); 

        $this->graphIDs[$gID] = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphIDs[$gID];
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphIDs[$gID],$file);
    }


    /*! \brief  Generate the pie-chart which displays the overall-plugin-usage
     */
    function generateCategoryPieGraph($data)
    {
        // Sort data by usage count and slice array to get 
        //  the eight most used categories
        $gID = 1;
        arsort($data);
        $mostUsedCategories = array_slice($data,0,7);

        // Get the rest of categories and combine them 'others'
        $theRest = array_slice($data,7);
        $mostUsedCategories['remaining'] = array_sum($theRest);

        // Try to find a translation for the given category names
        $values = array_values($mostUsedCategories);
        $keys = array_keys($mostUsedCategories);
        foreach($keys as $id => $cat){
            $keys[$id] = $this->getCategoryTranslation($cat);
        }

        // Dataset definition   
        $dataSet = new pData;  
        $dataSet->AddPoint($values,"Serie1");  
        $dataSet->AddAllSeries();  
        $dataSet->AddPoint($keys,"Serie2");  
        $dataSet->SetAbsciseLabelSerie("Serie2");  

        // Set graph area
        $x = 400;
        $y = 200;

        // Initialise the graph  
        $chart = new pChart($x,$y);  
        $chart->setFontProperties($this->font,10);  
        $chart->drawPieGraph($dataSet->GetData(),$dataSet->GetDataDescription(),($x/3),($y/2)-10,($y/2),PIE_PERCENTAGE,TRUE,50,20,3);  
        $chart->drawPieLegend(($x/3*2),15,$dataSet->GetData(),$dataSet->GetDataDescription(),
                $this->legendR,$this->legendG,$this->legendB);

        // Store graph data
        $this->graphIDs[$gID] = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphIDs[$gID];
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphIDs[$gID],$file);
    }
}
?>
