<?php

class statistics extends plugin
{
    var $plHeadline = 'Statistics';
    var $plDescription = 'GOsa usage statistics';
    var $plShortIcon = 'statistics.png';
    var $plIcon = 'plugin.png';
    
    var $receivedStats = array();


    function __construct($config)
    {
        plugin::plugin($config, NULL);

    }

    function execute()
    {


        if(isset($_POST['transmitStatistics'])){

            // First try to retrieve values via RPC
            if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){
                $tmp = stats::dumpTables();
                $dump = array();
                foreach($tmp as $entry){
                    $dump[] = array_values($entry);
                }

                $rpc = $this->config->getRpcHandle(
                        "http://10.3.64.59:4000",
                        "65717fe6-9e3e-11df-b010-5452005f1250",
                        "WyukwauWoid2",
                        TRUE);
                $rpc->updateInstanceStatus($dump);
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($rpc->get_error()),ERROR_DIALOG);
                }
            }
        }

        // Transmit daily statistics to GOsa-Server
        if(isset($_POST['receiveStatistics'])){

            // First try to retrieve values via RPC
            if ($this->config->get_cfg_value("core","gosaRpcServer") != ""){
                $rpc = $this->config->getRpcHandle(
                        "http://10.3.64.59:4000",
                        "65717fe6-9e3e-11df-b010-5452005f1250",
                        "WyukwauWoid2",
                        TRUE);
                $res = $rpc->getInstanceStats();
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"),msgPool::rpcError($rpc->get_error()),ERROR_DIALOG);
                }

                $all = array_sum($res['category_count']);
                $tmp = array();
                $tmpRendered = array();
                foreach($res['category_count'] as $category => $count){
                    $tmp[$category] = floor(($count / $all) * 10000) / 100;
                    $tmpRendered[$category] = progressbar($tmp[$category]);
                }
                $this->receivedStats = $tmpRendered;
            }
        }




        $smarty = get_smarty();
                $smarty->assign("receivedStats",$this->receivedStats);


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
