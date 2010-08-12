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
                $tmp = array();
                $tmpRendered = array();
                foreach($res['category_count'] as $category => $count){
                    $tmp[$category] = floor(($count / $all) * 10000) / 100;
                }


               $data = array_values($tmp);

               // Include jpgraph classes now.
               new jpGraphInclude();

               $graph = new PieGraph(600,500);
               $graph->title->Set("Example 4 of pie plot");
               $graph->SetAntiAliasing();

               $p1 = new PiePlot($data);
               $p1 = new PiePlot3D($data);
               $p1->value->SetColor("darkred");
               $p1->SetSize(0.3);
               $p1->SetCenter(0.4);
               $p1->SetLegends(array_keys($tmp));
               $graph->Add($p1);

               $this->graphID_1 = preg_replace("/[^0-9]/","",microtime(TRUE));
               $graph->Stroke('/tmp/graph_'.$this->graphID_1, TRUE);
               session::set('statistics::graphFile'.$this->graphID_1,'/tmp/graph_'.$this->graphID_1);
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
