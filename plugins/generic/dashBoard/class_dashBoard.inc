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

        // Check if we've access to the registration server
        if($this->config->registration->registrationRequired() && $config->registration->isServerAccessible()){
            if(!$config->registration->isInstanceRegistered()){
                $this->dialog = new RegistrationDialog($config);
            }
        }

        $this->initialized = FALSE;
    }

    function init()
    {
        // Instantiate child classes
        if( $this->config->registration->isInstanceRegistered() && 
                $this->config->registration->isServerAccessible()){

            $this->dbPluginStatus = new dbPluginStatus($this->config);
            $this->dbChannelStatus = new dbChannelStatus($this->config);
            $this->dbNotifications = new dbNotifications($this->config);
            $this->dbInformation = new dbInformation($this->config);
            $this->initialized = TRUE;
        }
    }

    function execute()
    {
        // The wants to registrate his instance of GOsa now!
        if(isset($_POST['registerNow']) && $this->config->registration->isServerAccessible()){
            $this->dialog = new RegistrationDialog($this->config);
        }

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

        $this->init();

        $smarty = get_smarty();
        $smarty->assign('instanceRegistered', $this->config->registration->isInstanceRegistered());
        $smarty->assign('isServerAccessible', $this->config->registration->isServerAccessible());
        $smarty->assign('registrationServerAvailable', $this->config->registration->isServerAccessible());

        if($this->initialized){
            $smarty->assign('dbPluginStatus', $this->dbPluginStatus->execute());
            $smarty->assign('dbChannelStatus', $this->dbChannelStatus->execute());
            $smarty->assign('dbNotifications', $this->dbNotifications->execute());
            $smarty->assign('dbInformation', $this->dbInformation->execute());
        }
        return($smarty->fetch(get_template_path('dashBoard.tpl', TRUE)));
    }

    function check()
    {
        $messages = plugin::check();

        if($this->initialized){
            $messages = array_merge($this->dbPluginStatus->check());    
            $messages = array_merge($this->dbChannelStatus->check());    
            $messages = array_merge($this->dbNotifications->check());    
            $messages = array_merge($this->dbInformation->check());    
        }
        return($messages);
    }

    function save_object()
    {
        plugin::save_object();
       
        if($this->dialog instanceOf RegistrationDialog){ 
            $this->dialog->save_object();
        }
        if($this->initialized){
            $this->dbPluginStatus->save_object();
            $this->dbChannelStatus->save_object();
            $this->dbNotifications->save_object();
            $this->dbInformation->save_object();
        }
    }

    function save()
    {
        plugin::save();
        if($this->initialized){
            $this->dbPluginStatus->save();
            $this->dbChannelStatus->save();
            $this->dbNotifications->save();
            $this->dbInformation->save();
        }
    }

    function remove_from_parent()
    {
        plugin::remove_from_parent();
        if($this->initialized){
            $this->dbPluginStatus->remove_from_parent();
            $this->dbChannelStatus->remove_from_parent();
            $this->dbNotifications->remove_from_parent();
            $this->dbInformation->remove_from_parent();
        }
    }
}

?>
