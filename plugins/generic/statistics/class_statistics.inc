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
                $res = $this->rpcHandle->getInstanceStats();
                if(!$this->rpcHandle->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($this->rpcHandle->get_error()),ERROR_DIALOG);
                }

                $all = array_sum($res['category_count']);
                $data = array();
                $cnt = 5;
                foreach($res['category_count'] as $category => $count){
                    $tmp[$category] = $count;
                }

                asort($tmp);
                $tmp = array_slice($tmp,0,7);

                // Include pChart 
                new pChartInclude();

                // Dataset definition   
                $DataSet = new pData;  
                $this->graphID_1 = preg_replace("/[^0-9]/","",microtime(TRUE));
                $DataSet->AddPoint(array_values($tmp),"Serie1");  
                $DataSet->AddPoint(array_keys($tmp),"Serie2");  
                $DataSet->AddAllSeries();  
                $DataSet->SetAbsciseLabelSerie("Serie2");  

                // Initialise the graph  
                $Test = new pChart(400,200);  
                # $Test->drawFilledRoundedRectangle(7,7,373,193,5,255,200,200);  
                # $Test->drawRoundedRectangle(5,5,375,195,5,200,255,200);  

                // Draw the pie chart  
                $Test->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",8);  
                $Test->drawPieGraph($DataSet->GetData(),$DataSet->GetDataDescription(),150,90,110,PIE_PERCENTAGE,TRUE,50,20,5);  
                $Test->drawPieLegend(310,15,$DataSet->GetData(),$DataSet->GetDataDescription(),200,255,200);  

                $file = '/tmp/graph_'.$this->graphID_1;
                $Test->Render($file);
                session::set('statistics::graphFile'.$this->graphID_1,$file);



                ## Second graph, plugin usage over time
                $this->graphID_2 = preg_replace("/[^0-9]/","",microtime(TRUE));

                $tmp = array();
                $DataSet2 = new pData; 
                $cnt = 2;
                foreach($res['category_count'] as $category => $count){
                    for($i = 0; $i < 10; $i++){
                        $tmp[$category][time() - ($i*60*60*24)] = rand(0,16);
                    }
                    $DataSet2->AddPoint($tmp[$category], $category);
                    $DataSet2->SetSerieName(_($category), $category);
                    $cnt --;
                    if(!$cnt) break;
                }
                $DataSet2->AddAllSeries();  

                // Build up X-Axis
                $steps = 10;
                $it = 24*60*60;
                $start = time() - ($steps * $it);
                $stop = time();
                $DataDescription = array();
                for($i = $start; $i < $stop; $i += $it){
                    $seriesX[] =  date('d .m', $i);
#                    $seriesX[] =  $i;
                }
#                $DataSet2->SetXAxisFormat("date");
                $DataSet2->AddPoint($seriesX, 'date');
                $DataSet2->SetAbsciseLabelSerie('date');  
                $DataSet2->RemoveSerie('date');  

                #  // Initialise the graph  
                $Test2 = new pChart(700,230);  
                $Test2->setFixedScale(0,20);  
                $Test2->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",8);  
                $Test2->setGraphArea(50,30,585,200);  
                $Test2->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
                $Test2->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
                $Test2->drawGraphArea(255,255,255,TRUE);  
                $Test2->drawScale($DataSet2->GetData(),$DataSet2->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2,TRUE);     
                $Test2->drawGrid(4,TRUE,230,230,230,50);  

                // Draw the 0 line  
                $Test2->drawTreshold(0,143,55,72,TRUE,TRUE);  

                // Draw the cubic curve graph  
                $Test2->drawBarGraph($DataSet2->GetData(),$DataSet2->GetDataDescription(),TRUE);  
                $Test2->drawCubicCurve($DataSet2->GetData(),$DataSet2->GetDataDescription());  

                // Finish the graph  
                $DataSet2->RemoveSerie('date');  
                $Test2->drawLegend(600,30,$DataSet2->GetDataDescription(),255,255,255);  
                $Test2->drawTitle(50,22,"Plugin usage over time",50,50,50,585);  

                $file = '/tmp/graph_'.$this->graphID_2;
                $Test2->Render($file);
                session::set('statistics::graphFile'.$this->graphID_2,$file);
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
