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

    function __construct($config)
    {
        plugin::plugin($config, NULL);

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
        $smarty = get_smarty();

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


                $days = 40;
                $interval = 1;
                $start = time() - ($days*86400);
                $stop  = time();

                $res = $this->rpcHandle->getInstanceStats($start,$stop,'day',$interval);
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

                    // Build up DataSet. 
                    $tmp = array();
                    $DataSet2 = new pData;  
                    $dates = array();
                    $max = 0;

                    foreach($res['actionsPerInterval'] as $id => $data){

                        $category = $data['category'];
                        $date =  $data['date'];

                        if(!isset($mostUsedCategories[$category])) continue;

                        if($data['count'] > $max) $max = $data['count'];
                        $tmp[$category][$date] = $data['count'];
                        $dates[$date] = $date;
                    }
                    $dates = array_values($dates);
                    sort($dates);

                    foreach($tmp as $category => $data){
                        ksort($tmp[$category]);
                        $DataSet2->AddPoint($tmp[$category], $category);
                        $DataSet2->SetSerieName(_($category), $category);
                    }
                    $DataSet2->AddAllSeries();  

                    $tmp = array();
                    foreach($dates as $date) $tmp[] = date('d.m', $date);

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
                    $Test2->drawFilledLineGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),50,TRUE);  

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
