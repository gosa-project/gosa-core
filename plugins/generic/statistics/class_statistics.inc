<?php

class statistics extends plugin
{
    var $plHeadline = 'Statistics';
    var $plDescription = 'GOsa usage statistics';
    var $plShortIcon = 'statistics.png';
    var $plIcon = 'plugin.png';
    
    var $rpcHandle = NULL;
    var $rpcConfigured = FALSE;

    var $graphID_1 = 0;
    var $graphID_2 = 0;
    var $graphID_3 = 0;
    var $graphID_4 = 0;
    var $graphID_5 = 0;

    var $legendR = 235;
    var $legendG = 235;
    var $legendB = 235;

    var $graph1DatePicker1 = 0;
    var $graph1DatePicker2 = 0;

    var $unsbmittedFiles = array();

    function __construct($config)
    {
        plugin::plugin($config, NULL);

        // Init start and stop times for graph 1
        $this->graph1DatePicker1 = date('d.m.Y', time() - 7 * 24 * 60 *60);
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
        $res = $this->rpcHandle->getInstanceStatDates();
        $dates = array();
        if(!$this->rpcHandle->success()){
            msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
        }else{
            foreach($res as $date){
                $dates[$date] = strtotime($date);
            }
        }
        return($dates);
    }


    function execute()
    {
        if(isset($_POST['graph1DatePicker1'])) $this->graph1DatePicker1 = get_post('graph1DatePicker1');
        if(isset($_POST['graph1DatePicker2'])) $this->graph1DatePicker2 = get_post('graph1DatePicker2');

        $smarty = get_smarty();
        $smarty->assign('graph1DatePicker1', $this->graph1DatePicker1);
        $smarty->assign('graph1DatePicker2', $this->graph1DatePicker2);

        // Do not render anything if we are not prepared to send and receive data via rpc.
        $smarty->assign("rpcConfigured", $this->rpcConfigured);
        $smarty->assign("validRpcHandle", TRUE);
        if(!$this->rpcConfigured || !$this->rpcHandle){
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
                    echo "Inserted ".$res." entries for date ".date('d.m.Y', $date)."<br>";
                }
            }
            $this->unsbmittedFiles = $this->getUnsubmittedStatistics();
        }

        // Transmit daily statistics to GOsa-Server
        if(isset($_POST['receiveStatistics'])){

            // First try to retrieve values via RPC
            if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){

                $start = strtotime($this->graph1DatePicker1);
                $stop  = strtotime($this->graph1DatePicker2);

                // Request statistics now
                $res = $this->rpcHandle->getInstanceStats($start,$stop);
                if(!$this->rpcHandle->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
                }

                if($res && $this->rpcHandle->success()){

                    // Include pChart 
                    new pChartInclude();

                    // Get most used categories, but only eight at once.
                    if(count($res['actionsPerCategory'])){
                        $this->generateCategoryPieGraph($res['actionsPerCategory']);
                    }


                    // --------
                    // Generate combined line and car chart of plugin usage, ldap execution time and errors 
                    // --------

                    // Generate new and unique graph id
                    $this->graphID_2 = preg_replace("/[^0-9]/","",microtime(TRUE));

                    // Prepare transmitted data, sort it by date and collect 
                    //  transmitted timestamps to be able to print the x-Axis labels.
                    $dataArray = array();
                    $dates = array();
                    $dataSet = new pData;  
                    $max = 1;
                    $seriesNumber = 0;
                    foreach($res['actionsPerInterval'] as $category => $entriesPerDate){

                        // Collect data per category and store used timestamps
                        foreach($entriesPerDate as $dateStr => $count){
                            $date = strtotime($dateStr);
                            $dates[$date]=$date;

                            // Do not append empty data                            
                            if(empty($category)) continue;
                            if($count) $count = ($count);
                            $dataArray[$category][$date] = $count;
                        }

                        // Do not append empty data 
                        if(empty($category)) continue;

                        // Sort results. 
                        ksort($dataArray[$category]);

                        // Add results to our data set.
                        $dataSet->AddPoint($dataArray[$category], $category);
                        $dataSet->SetSerieName($this->getCategoryTranslation($category), $category);
                        $dataSet->AddSerie($category);
                        $seriesNumber++;

                        // Detect maximum value, to adjust the Y-Axis
                        $tmpMax = max($dataArray[$category]);
                        if($tmpMax > $max) $max = $tmpMax;
                    }
                    ksort($dates);

                    // Prepare date strings for X-Axis, only print a given number of 
                    //  of labels to keep the axis readable.
                    $Xam = 5; // Number of labels
                    $cnt = 0;
                    $dateSeries = array();
                    foreach($dates as $stamp){
                        if((count($dates) <= $Xam) || ($cnt % (floor(count($dates) / $Xam )) == 0)){
                            $dateSeries[$stamp] = date('d.m.Y',$stamp);
                        }else{
                            $dateSeries[$stamp] = ' ';
                        }
                        $cnt ++;    
                    }

                    $dataSet->AddPoint($dateSeries, 'date');
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
                    $chart->drawTitle(50,22,"Plugin usage over time",50,50,50,585);  

                    if(count($dates)){
                        $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2);     
                    }

                    // Draw the cubic curve graph  
                    if(count($dataArray)){
                        $chart->drawFilledLineGraph($dataSet->GetData(),$dataSet->GetDataDescription(),50,TRUE);
                    }

                    // Add error series
                    $errors = array();
                    foreach($res['errorsPerInterval'] as $dateStr => $count){
                        $date = strtotime($dateStr);
                        if($count !=0) $count = ($count);
                        $errors[$date] = $count;
                    }

                    ksort($errors);

                    $dataSet->AddPoint($errors, 'Errors');
                    $dataSet->SetSerieName(_('Error'), 'Errors');
                    $dataSet->AddSerie('Errors');
                    $seriesNumber ++;

                    $chart->setColorPalette($seriesNumber-1,255,0,0);   

                    // Draw legend
                    $chart->drawLegend(650,30,$dataSet->GetDataDescription(),255,255,255);  

                    // Remove plugin usage from data series, just keep error series.
                    foreach($dataArray as $categoryName => $list){
                        $dataSet->RemoveSerie($categoryName);
                    }

                    // Draw right scale (Errors per day)
                    $chart->setFixedScale(0.0001,(max($errors) +1) *1.1);
                    $chart->drawRightScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,120,150,150,TRUE,0,2);
                    $chart->drawBarGraph($dataSet->GetData(),$dataSet->GetDataDescription());

                    $file = '/tmp/graph_'.$this->graphID_2;
                    $chart->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_2,$file);



                    // Prepare Data 
                    $graphData = array();
                    foreach($res['usagePerInterval'] as $dateStr => $data){
                        $date = strtotime($dateStr);
                        foreach($data as $name => $val){
                            $graphData[$name][$date] = $val;
                        }
                    }

                    // Sort Data 
                    foreach($graphData as $key => $data)
                        ksort($graphData[$key]);

                    // Generate new and unique graph id
                    $this->graphID_3 = preg_replace("/[^0-9]/","",microtime(TRUE));

                    // Prepare transmitted data, sort it by date and collect 
                    //  transmitted timestamps to be able to print the x-Axis labels.
                    $dataSet = new pData;  

                    $max = max($graphData['max_mem']);

                    $dataSet->AddPoint(array_values($graphData['max_mem']), 'max_mem');
                    $dataSet->AddPoint(array_values($graphData['avg_mem']), 'avg_mem');
                    $dataSet->AddPoint(array_values($graphData['min_mem']), 'min_mem');

                    $dataSet->SetSerieName('Min Memory', 'min_mem');
                    $dataSet->SetSerieName('Max Memory', 'max_mem');
                    $dataSet->SetSerieName('Average Memory', 'avg_mem');

                    $dataSet->AddAllSeries();  
                    $dataSet->AddPoint($dateSeries, 'date');
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
                    $chart->drawTitle(50,22,"Memory usage",50,50,50,585);  

                    $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, FALSE);
                    $chart->drawFilledCubicCurve($dataSet->GetData(),$dataSet->GetDataDescription(),.1,50); 

                    $file = '/tmp/graph_'.$this->graphID_3;
                    $chart->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_3,$file);







                    // Generate new and unique graph id
                    $this->graphID_4 = preg_replace("/[^0-9]/","",microtime(TRUE));

                    // Prepare transmitted data, sort it by date and collect 
                    //  transmitted timestamps to be able to print the x-Axis labels.
                    $dataSet = new pData;  

                    $max = max($graphData['max_dur']);

                    $dataSet->AddPoint(array_values($graphData['max_dur']), 'max_dur');
                    $dataSet->AddPoint(array_values($graphData['avg_dur']), 'avg_dur');
                    $dataSet->AddPoint(array_values($graphData['min_dur']), 'min_dur');

                    $dataSet->SetSerieName('Min dur', 'min_dur');
                    $dataSet->SetSerieName('Max dur', 'max_dur');
                    $dataSet->SetSerieName('Average dur', 'avg_dur');

                    $dataSet->AddAllSeries();  
                    $dataSet->AddPoint($dateSeries, 'date');
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
                    $chart->drawTitle(50,22,"Render time",50,50,50,585);  

                    $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, FALSE);
                    $chart->drawFilledCubicCurve($dataSet->GetData(),$dataSet->GetDataDescription(),.1,50); 

                    $file= '/tmp/graph_'.$this->graphID_4;
                    $chart->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_4,$file);








                    // Generate new and unique graph id
                    $this->graphID_5 = preg_replace("/[^0-9]/","",microtime(TRUE));

                    // Prepare transmitted data, sort it by date and collect 
                    //  transmitted timestamps to be able to print the x-Axis labels.
                    $dataSet = new pData;  

                    $max = max($graphData['max_load']);

                    $dataSet->AddPoint(array_values($graphData['max_load']), 'max_load');
                    $dataSet->AddPoint(array_values($graphData['avg_load']), 'avg_load');
                    $dataSet->AddPoint(array_values($graphData['min_load']), 'min_load');

                    $dataSet->SetSerieName('Min Load', 'min_load');
                    $dataSet->SetSerieName('Max Load', 'max_load');
                    $dataSet->SetSerieName('Average Load', 'avg_load');

                    $dataSet->AddAllSeries();  
                    $dataSet->AddPoint($dateSeries, 'date');
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
                    $chart->drawTitle(50,22,"CPU load",50,50,50,585);  

                    $chart->drawScale($dataSet->GetData(),$dataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2, FALSE);
                    $chart->drawFilledCubicCurve($dataSet->GetData(),$dataSet->GetDataDescription(),.1,50); 

                    $file = '/tmp/graph_'.$this->graphID_5;
                    $chart->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_5,$file);

                }
            }
        }

        $smarty->assign('graphID_1', $this->graphID_1);
        $smarty->assign('graphID_2', $this->graphID_2);
        $smarty->assign('graphID_3', $this->graphID_3);
        $smarty->assign('graphID_4', $this->graphID_4);
        $smarty->assign('graphID_5', $this->graphID_5);
        $smarty->assign('unsbmittedFiles', count($this->unsbmittedFiles));
        $smarty->assign('unsbmittedFilesMsg', 
                sprintf(
                    _("You have currently %s unsubmitted statistic collection, do you want to transmit them now?"),
                    count($this->unsbmittedFiles)));
        return($smarty->fetch(get_template_path('statistics.tpl', TRUE)));
    }


    function check()
    {
        $messages = plugin::check();
        return($messages);
    }


    function save_object()
    {
        plugin::save_object();
    }


    /*! \brief      This method tries to translate category names.
     *  @param      The category name to translate
     *  @return     String  The translated category names.
     */
    function getCategoryTranslation($name)
    {
        $ret ="";

        // Extract category names from the given string.
        $list = preg_split("/, /", $name);

        // We do not have a category for systems directly, so we've to map all system types to 'System'.
        // If we do not map to _(Systems) the graph legend will be half screen width.
        if(count(array_intersect(array('server','terminal','workstation', 'opsi', 'component'), $list))){
            return(_("Systems"));
        }

        // Walk through category names and try to find a translation.
        foreach($list as $cat){
            $cat = trim($cat);
            if(isset($this->catTranslations[$cat])){
                $cat = _($this->catTranslations[$cat]);
            }elseif(!empty($cat)){
                $cat = _($cat);
            }
            $ret .= $cat.", ";
        }
        return(rtrim($ret, ', '));
    }


    function generateCategoryPieGraph($data)
    {
        // Sort data by usage count and slice array to get 
        //  the eight most used categories
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

        // Initialise the graph  
        $chart = new pChart(600,250);  
        $chart->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
        $chart->drawPieGraph($dataSet->GetData(),$dataSet->GetDataDescription(),200,120,150,PIE_PERCENTAGE,TRUE,50,20,3);  
        $chart->drawPieLegend(420,15,$dataSet->GetData(),$dataSet->GetDataDescription(),
                $this->legendR,$this->legendG,$this->legendB);

        // Store graph data
        $this->graphID_1 = preg_replace("/[^0-9]/","",microtime(TRUE));
        $file = '/tmp/graph_'.$this->graphID_1;
        $chart->Render($file);
        session::set('statistics::graphFile'.$this->graphID_1,$file);
    }
}
?>
