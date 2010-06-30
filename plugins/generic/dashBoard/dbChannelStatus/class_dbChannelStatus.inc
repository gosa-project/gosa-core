<?php

class dbChannelStatus extends plugin
{
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
        $smarty = get_smarty();

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
