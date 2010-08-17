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
            $tmp = stats::dumpTables();
            $dump = array();
            foreach($tmp as $entry){
                $dump[] = array_values($entry);
            }
            $res = $this->rpcHandle->updateInstanceStatus($dump);
            if(!$this->rpcHandle->success()){
                msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
            }else{
                echo $res." Entries inserted";
            }
        }

        // Transmit daily statistics to GOsa-Server
        if(isset($_POST['receiveStatistics'])){

            // First try to retrieve values via RPC
            if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){

                $interval = $this->graph1Interval;
                $start = strtotime($this->graph1DatePicker1);
                $stop  = strtotime($this->graph1DatePicker2);

                // Request statistics now
                $res = $this->rpcHandle->getInstanceStats($start,$stop,$interval);
                if(!$this->rpcHandle->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
                }

                if($res){

                    // Include pChart 
                    new pChartInclude();

                    // --------
                    // Generate PIE chart of most used categories
                    // --------

                    // Get most used categories, but only eight at once.
                    arsort($res['actionsPerCategory']);
                    $mostUsedCategories = array_slice($res['actionsPerCategory'],0,8);

                    // Dataset definition   
                    $DataSet = new pData;  
                    $this->graphID_1 = preg_replace("/[^0-9]/","",microtime(TRUE));
                    $DataSet->AddPoint(array_values($mostUsedCategories),"Serie1");  
                    $DataSet->AddPoint(array_keys($mostUsedCategories),"Serie2");  
                    $DataSet->AddAllSeries();  
                    $DataSet->SetAbsciseLabelSerie("Serie2");  

                    // Initialise the graph  
                    $Test = new pChart(400,200);  
                    $Test->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
                    $Test->drawPieGraph($DataSet->GetData(),$DataSet->GetDataDescription(),150,90,110,PIE_PERCENTAGE,TRUE,50,20,5);  
                    $Test->drawPieLegend(310,15,$DataSet->GetData(),$DataSet->GetDataDescription(),200,255,200);  

                    $file = '/tmp/graph_'.$this->graphID_1;
                    $Test->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_1,$file);


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
                    $max = 0;
                    foreach($res['actionsPerInterval'] as $category => $entriesPerDate){

                        // Only print the 8 most used categories, else we receive a lot of errors
                        //  from the pChart classes, seems that it can only handle 8 data series at once.
                        if(!isset($mostUsedCategories[$category])) continue;

                        // Collect data per category and store used timestamps
                        foreach($entriesPerDate as $dateStr => $count){
                            $date = strtotime($dateStr);
                            $dataArray[$category][$date] = log($count + 1);
                            $dates[$date]=$date;
                        }
                
                        // Sort results. 
                        ksort($dataArray[$category]);
                        ksort($dates);

                        // Add results to our data set.
                        $DataSet2->AddPoint($dataArray[$category], $category);
                        $DataSet2->SetSerieName(_($category), $category);

                        // Detect maximum value, to adjust the Y-Axis
                        $tmpMax = max($dataArray[$category]);
                        if($tmpMax > $max) $max = $tmpMax;
                    }
                    $DataSet2->AddAllSeries();  

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
                    $DataSet2->RemoveSerie('date');  

                    $Test2 = new pChart(700,230);  
                    $Test2->setFixedScale(0,$max);  
                    $Test2->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",10);  
                    $Test2->setGraphArea(50,30,585,200);  
                    $Test2->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
                    $Test2->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
                    $Test2->drawGraphArea(255,255,255,TRUE);  
                    $Test2->drawScale($DataSet2->GetData(),$DataSet2->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2);     
                    $Test2->drawGrid(4,TRUE,230,230,230,50);  

                    // Draw the 0 line  
                    $Test2->drawTreshold(0,143,55,72,TRUE,TRUE);  

                    // Draw the cubic curve graph  
                    #$Test2->drawBarGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),TRUE);  
                    #$Test2->drawLineGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),TRUE); 
                    #$Test2->drawFilledLineGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),50,TRUE);  
                    $Test2->drawFilledCubicCurve($DataSet2->GetData(),$DataSet2->GetDataDescription(),.1,20);  

                    // Finish the graph  
                    $DataSet2->RemoveSerie('date');  
                    $Test2->drawLegend(600,30,$DataSet2->GetDataDescription(),255,255,255);  
                    $Test2->drawTitle(50,22,"Plugin usage over time",50,50,50,585);  

                    $file = '/tmp/graph_'.$this->graphID_2;
                    $Test2->Render($file);
                    session::set('statistics::graphFile'.$this->graphID_2,$file);
                }
            }
        }

        $smarty->assign('graphID_1', $this->graphID_1);
        $smarty->assign('graphID_2', $this->graphID_2);
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
