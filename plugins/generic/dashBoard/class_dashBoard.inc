<?php

class dashBoard extends plugin
{
    var $plHeadline = 'Dash board';
    var $plDescription = 'Dash board';
    var $plShortIcon = 'dashBoard.png';
    var $plIcon = 'plugin.png';

    private $dbPluginStatus = NULL;
    private $dbChannelStatus = NULL;
    private $dbNotifications = NULL;
    private $dbInformation = NULL;

    function __construct($config)
    {
        plugin::plugin($config, NULL);

        // Instantiate child classes
        $this->dbPluginStatus = new dbPluginStatus($config);
        $this->dbChannelStatus = new dbChannelStatus($config);
        $this->dbNotifications = new dbNotifications($config);
        $this->dbInformation = new dbInformation($config);

        // Check if we've access to the registration server
        if($this->config->registration->registrationRequired() && $config->registration->isServerAccessible()){
            if(!$config->registration->isInstanceRegistered()){
                $this->dialog = new RegistrationDialog($config);
            }
        }
    }

    function execute()
    {
        // Seems that we haven't registered our GOsa instance yet.
        // Ask the user to do so, now.
        if($this->dialog instanceOf RegistrationDialog){

            // Check if the registration is complete/canceled
            if(!$this->dialog->finished()){
                return($this->dialog->execute());
            }else{
                $this->dialog = NULL;
            }
        }

        $smarty = get_smarty();
        $smarty->assign('dbPluginStatus', $this->dbPluginStatus->execute());
        $smarty->assign('dbChannelStatus', $this->dbChannelStatus->execute());
        $smarty->assign('dbNotifications', $this->dbNotifications->execute());
        $smarty->assign('dbInformation', $this->dbInformation->execute());
        return($smarty->fetch(get_template_path('dashBoard.tpl', TRUE)));
    }

    function check()
    {
        $messages = plugin::check();
        $messages = array_merge($this->dbPluginStatus->check());    
        $messages = array_merge($this->dbChannelStatus->check());    
        $messages = array_merge($this->dbNotifications->check());    
        $messages = array_merge($this->dbInformation->check());    
        return($messages);
    }

    function save_object()
    {
        plugin::save_object();
       
        if($this->dialog instanceOf RegistrationDialog){ 
            $this->dialog->save_object();
        }else{

            $this->dbPluginStatus->save_object();
            $this->dbChannelStatus->save_object();
            $this->dbNotifications->save_object();
            $this->dbInformation->save_object();
        }
    }

    function save()
    {
        plugin::save();
        $this->dbPluginStatus->save();
        $this->dbChannelStatus->save();
        $this->dbNotifications->save();
        $this->dbInformation->save();
    }

    function remove_from_parent()
    {
        plugin::remove_from_parent();
        $this->dbPluginStatus->remove_from_parent();
        $this->dbChannelStatus->remove_from_parent();
        $this->dbNotifications->remove_from_parent();
        $this->dbInformation->remove_from_parent();
    }
}

?>
