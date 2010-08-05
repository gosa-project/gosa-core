<?php

class statistics extends plugin
{
    var $plHeadline = 'Statistics';
    var $plDescription = 'GOsa usage statistics';
    var $plShortIcon = 'statistics.png';
    var $plIcon = 'plugin.png';
    
    var $receivedStats = array();


    var $rpcHandle = NULL;


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
        if(!$this->rpcConfigured || !$this->rpcHandle){
            $smarty->assign("validRpcHandle", FALSE);
            return($smarty->fetch(get_template_path('statistics.tpl', TRUE)));
        }


        $smarty->assign("validRpcHandle", TRUE);
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
                    $tmpRendered[$category] = progressbar($tmp[$category]);
                }
                $this->receivedStats = $tmpRendered;
            }
        }


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
