<?php

class dashBoard extends plugin
{
    var $plHeadline = 'Dash board';
    var $plDescription = 'Dash board';
    var $plShortIcon = 'dashBoard.png';
    var $plIcon = 'plugin.png';

    function __construct($config)
    {
        plugin::plugin($config, NULL);

        // Instantiate child classes
        $this->dbPluginStatus = new dbPluginStatus($config);
    }

    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('dbPluginStatus', $this->dbPluginStatus->execute());
        return($smarty->fetch(get_template_path('dashBoard.tpl', TRUE)));

    }

    function check()
    {
        $messages = plugin::check();
        $messages = array_merge($this->dbPluginStatus->check());    
        return($messages);
    }

    function save_object()
    {
        plugin::save_object();
        $this->dbPluginStatus->save_object();
    }

    function save()
    {
        plugin::save();
        $this->dbPluginStatus->save();
    }

    function remove_from_parent()
    {
        plugin::remove_from_parent();
        $this->dbPluginStatus->remove_from_parent();
    }
}

?>
