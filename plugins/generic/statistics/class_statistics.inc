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

    var $graph1Interval = 1;
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
        $unsubmitted = array_intersect($available,$alreadyTransmitted);

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
        if(isset($_POST['graph1Interval'])) $this->graph1Interval = get_post('graph1Interval');

        if(!$this->graph1Interval) $this->graph1Interval = 1;

        $smarty = get_smarty();
        $smarty->assign('graph1DatePicker1', $this->graph1DatePicker1);
        $smarty->assign('graph1DatePicker2', $this->graph1DatePicker2);
        $smarty->assign('graph1Interval', $this->graph1Interval);
        $smarty->assign('intervalValues', array('1','2','5','7','30'));

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

                if($res){

                    // Include pChart 
                    new pChartInclude();

                    // Get most used categories, but only eight at once.
                    if(count($res['actionsPerCategory'])){

                        // --------
                        // Generate PIE chart of most used categories
                        // --------

                        arsort($res['actionsPerCategory']);
                        $mostUsedCategories = array_slice($res['actionsPerCategory'],0,8);

                        // Dataset definition   
                        $DataSet = new pData;  
                        $this->graphID_1 = preg_replace("/[^0-9]/","",microtime(TRUE));

                        $values = array_values($mostUsedCategories);
                        $keys = array_keys($mostUsedCategories);

                        $DataSet->AddPoint($values,"Serie1");  
                        $DataSet->AddAllSeries();  
                        $DataSet->AddPoint($keys,"Serie2");  
                        $DataSet->SetAbsciseLabelSerie("Serie2");  

                        // Initialise the graph  
                        $Test = new pChart(400,200);  
                        $Test->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  


                        $Test->drawPieGraph($DataSet->GetData(),$DataSet->GetDataDescription(),150,90,110,PIE_PERCENTAGE,TRUE,50,20,5);  
                        $Test->drawPieLegend(310,15,$DataSet->GetData(),$DataSet->GetDataDescription(),200,255,200);  

                        $file = '/tmp/graph_'.$this->graphID_1;
                        $Test->Render($file);
                        session::set('statistics::graphFile'.$this->graphID_1,$file);
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
                    $DataSet2 = new pData;  
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
                        $DataSet2->AddPoint($dataArray[$category], $category);
                        $DataSet2->SetSerieName(_($category), $category);
                        $Test->setLabel($DataSet->GetData(),$DataSet->GetDataDescription(),"Serie1",0,"Daily incomes");  
                        $DataSet2->AddSerie($category);
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
                    $tmp = array();
                    foreach($dates as $stamp){
                        if((count($dates) <= $Xam) || ($cnt % (floor(count($dates) / $Xam )) == 0)){
                            $tmp[$stamp] = date('d.m.Y',$stamp);
                        }else{
                            $tmp[$stamp] = ' ';
                        }
                        $cnt ++;    
                    }

                    $DataSet2->AddPoint($tmp, 'date');
                    $DataSet2->SetAbsciseLabelSerie('date');  

                    $Test2 = new pChart(800,230);  
                    $Test2->setFixedScale(0.0001,($max*1.1));  
                    $Test2->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
                    $Test2->setGraphArea(50,30,585,200);  
                    $Test2->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
                    $Test2->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
                    $Test2->drawGraphArea(255,255,255,TRUE);  
                    $Test2->drawGrid(4,TRUE,200,200,200,50);  
                    $Test2->drawTreshold(0,143,55,72,TRUE,TRUE);  
                    $Test2->drawTitle(50,22,"Plugin usage over time",50,50,50,585);  

                    if(count($dates)){
                        $Test2->drawScale($DataSet2->GetData(),$DataSet2->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2);     
                    }

                    // Draw the cubic curve graph  
                    if(count($dataArray)){
                        $Test2->drawFilledLineGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),50,TRUE);
                    }

                    // Add error series
                    $errors = array();
                    foreach($res['errorsPerInterval'] as $dateStr => $count){
                        $date = strtotime($dateStr);
                        if($count !=0) $count = ($count);
                        $errors[$date] = $count;
                    }

                    $DataSet2->AddPoint($errors, 'Errors');
                    $DataSet2->SetSerieName('Errors', 'Errors');
                    $DataSet2->AddSerie('Errors');
                    $seriesNumber ++;
                   
                    $Test2->setColorPalette($seriesNumber-1,255,0,0);   

                    // Draw legend
                    $Test2->drawLegend(650,30,$DataSet2->GetDataDescription(),255,255,255);  

                    // Remove plugin usage from data series, just keep error series.
                    foreach($dataArray as $categoryName => $list){
                        $DataSet2->RemoveSerie($categoryName);
                    }
    
                    // Draw right scale (Errors per day)
                    $Test2->setFixedScale(0.0001,(max($errors) +1) *1.1);
                    $Test2->drawRightScale($DataSet2->GetData(),$DataSet2->GetDataDescription(),SCALE_NORMAL,120,150,150,TRUE,0,2);
                    $Test2->drawBarGraph($DataSet2->GetData(),$DataSet2->GetDataDescription());

                    $file = '/tmp/graph_'.$this->graphID_2;
                    $Test2->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_2,$file);
                }
            }
        }

        $smarty->assign('graphID_1', $this->graphID_1);
        $smarty->assign('graphID_2', $this->graphID_2);
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
}
?>
