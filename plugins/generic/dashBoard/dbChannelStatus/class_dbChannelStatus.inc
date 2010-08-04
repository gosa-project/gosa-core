<?php




class dbChannelStatus extends plugin
{

    var $receivedStats = array();

    function __construct($config)
    {
        parent::__construct($config, NULL);

        // Construct the channel list.
        $this->channelList= new sortableListing();
        $this->channelList->setDeleteable(false);
        $this->channelList->setEditable(false);
        $this->channelList->setColspecs(array('30px','120px','*','100px'));
        $this->channelList->setHeader(array('?',_("Name"),_("Description"),_("Status")));
        $this->channelList->setWidth("100%");
        $this->channelList->setDefaultSortColumn(1);
        $this->channelList->setHeight("200px");
        $this->channelList->setAcl("rwcdm");
    }

    function execute()
    {
        // Transmit daily statistics to GOsa-Server
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
    
        $channel = array();
        $channel[] = array(
                        'icon' =>  image('images/true.png'),
                        'name' =>  'GONICUS support',
                        'desc' =>  'GONICUS helps you all time!',
                        'stat' =>  'Online'
                        );
        $channel[] = array(
                        'icon' =>  image('images/true.png'),
                        'name' =>  'Free',
                        'desc' =>  'Free channel, basic GOsa plugins!',
                        'stat' =>  'Online'
                        );
        $channel[] = array(
                        'icon' =>  image('images/small_error.png'),
                        'name' =>  'Experimental',
                        'desc' =>  'May be down for maintance!',
                        'stat' =>  'Offline'
                        );


        $data = $lData = array();
        foreach($channel as $key => $ch){
            $data[$key] = $ch;
            $lData[$key] = array('data' => array($ch['icon'],$ch['name'], $ch['desc'], $ch['stat']));
        }

        $this->channelList->setListData($data,$lData);
        $this->channelList->update();
        $smarty->assign('channelList', $this->channelList->render());
        return($smarty->fetch(get_template_path('dbChannelStatus/contents.tpl', TRUE)));
    }

    function save_object()
    {
        parent::save_object();
    }

    function save()
    {
        parent::save();
    }

    function check()
    {
        return(parent::check());
    }

    function remove_from_parent()
    {
        parent::remove_from_parent();
    }
}

?>
