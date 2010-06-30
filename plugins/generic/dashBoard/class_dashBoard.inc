<?php

class dashBoard extends plugin
{
    var $plHeadline = 'Dash board';
    var $plDescription = 'Dash board';
    var $plShortIcon = 'dashBoard.png';
    var $plIcon = 'plugin.png';

    private $dbPluginStatus = NULL;
    private $dbChannelStatus = NULL;
    private $dbAdvices = NULL;
    private $dbInformation = NULL;

    function __construct($config)
    {
        plugin::plugin($config, NULL);

        // Instantiate child classes
        $this->dbPluginStatus = new dbPluginStatus($config);
        $this->dbChannelStatus = new dbChannelStatus($config);
        $this->dbAdvices = new dbAdvices($config);
        $this->dbInformation = new dbInformation($config);
    }

    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('dbPluginStatus', $this->dbPluginStatus->execute());
        $smarty->assign('dbChannelStatus', $this->dbChannelStatus->execute());
        $smarty->assign('dbAdvices', $this->dbAdvices->execute());
        $smarty->assign('dbInformation', $this->dbInformation->execute());
        return($smarty->fetch(get_template_path('dashBoard.tpl', TRUE)));
    }

    function check()
    {
        $messages = plugin::check();
        $messages = array_merge($this->dbPluginStatus->check());    
        $messages = array_merge($this->dbChannelStatus->check());    
        $messages = array_merge($this->dbAdvices->check());    
        $messages = array_merge($this->dbInformation->check());    
        return($messages);
    }

    function save_object()
    {
        plugin::save_object();
        $this->dbPluginStatus->save_object();
        $this->dbChannelStatus->save_object();
        $this->dbAdvices->save_object();
        $this->dbInformation->save_object();
    }

    function save()
    {
        plugin::save();
        $this->dbPluginStatus->save();
        $this->dbChannelStatus->save();
        $this->dbAdvices->save();
        $this->dbInformation->save();
    }

    function remove_from_parent()
    {
        plugin::remove_from_parent();
        $this->dbPluginStatus->remove_from_parent();
        $this->dbChannelStatus->remove_from_parent();
        $this->dbAdvices->remove_from_parent();
        $this->dbInformation->remove_from_parent();
    }
}

?>
