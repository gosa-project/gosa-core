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
                $Test->drawFilledRoundedRectangle(7,7,373,193,5,255,200,200);  
                $Test->drawRoundedRectangle(5,5,375,195,5,200,255,200);  

                // Draw the pie chart  
                $Test->setFontProperties("./themes/default/fonts/LiberationSans-Regular.ttf",8);  
                $Test->drawPieGraph($DataSet->GetData(),$DataSet->GetDataDescription(),150,90,110,PIE_PERCENTAGE,TRUE,50,20,5);  
                $Test->drawPieLegend(310,15,$DataSet->GetData(),$DataSet->GetDataDescription(),200,255,200);  

                $file = '/tmp/graph_'.$this->graphID_1;
                $Test->Render($file);

                session::set('statistics::graphFile'.$this->graphID_1,$file);
            }
        }

        $smarty->assign('graphID_1', $this->graphID_1);
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
