<?php

class newConfigManagement extends plugin
{
    var $initTime;
    var $plHeadline = "Config management";
    var $plDescription = "Config management";

    var $installationMethods = array();

    var $selectedDistribution;

    function __construct($config, $dn)
    {
        $this->config = &$config;
        $this->listing = new ConfigManagementListing($this->config, get_userinfo());
        $this->dataModel = new ConfigManagementDataModel();

        // Load distributions 
        $rpc = $this->config->getRpcHandle();
        $res = $rpc->getDistributions();
        if(!$rpc->success()){
            msg_dialog::display(_("Error"), sprintf(_("Failed to load distributions, error was '%s'!"), $rpc->get_error()),ERROR_DIALOG);
            return(NULL);
        }
        foreach($res as $dist){
            $this->dataModel->addItem('Distribution','/root', $dist);

            // Load the releases for the current distribution
            $releases = $rpc->getReleases($dist);
            if(!$rpc->success()){
                msg_dialog::display(_("Error"), sprintf(_("Failed to load releases, error was '%s'!"), $rpc->get_error()),ERROR_DIALOG);
                return(NULL);
            }
            foreach($releases as $release){
                $this->dataModel->addItem('Release',"/root/{$dist}", $release);

                // Load packages
                $packages = $rpc->getPackages($release);
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"), sprintf(_("Failed to load packages, error was '%s'!"), $rpc->get_error()),ERROR_DIALOG);
                    return(NULL);
                }
                foreach($packages as $package){
                    $this->dataModel->addItem('Package',"/root/{$dist}/{$release}", $package['name'], $package);
                }
            }
        }

    }

    function save_object()
    {
        $this->listing->save_object();
        $this->selectedDistribution = $this->listing->getSelectedDistribution();
        $this->selectedRelease = $this->listing->getSelectedRelease();
    
        $distList = $this->getDistList();
        $releaseList = $this->getReleaseList();

        if(!isset($releaseList[$this->selectedRelease])){
            if(count($releaseList)){
                $this->selectedRelease = key($releaseList);
            }else{
                $this->selectedRelease = "";
            }
        }
        $this->listing->setDistribution($this->selectedDistribution);
        $this->listing->setRelease($this->selectedRelease);
        $this->listing->setDistributions($this->getDistList());
        $this->listing->setReleases($this->getReleaseList());
    }

    
    /*! \brief  Intializes this plugin
     *          All available installation methods will be loaded
     */
    function loadInstallationMethods()
    {
        // Reset erros
        $this->rpcError = $this->initFailed = FALSE;

        // Load configuration via rpc.
        $rpc = $this->config->getRpcHandle();

        // Populate install methods on success.
        $res = $rpc->getSupportedInstallMethods();
        if(!$rpc->success()){
            $this->rpcError = TRUE;
            $this->errorMessage = $rpc->get_error();;
            return;
        }
        $this->installationMethods = $res;
        if(!count($this->installationMethods)){
            $this->errorMessage = _("No selectable install methods returned!");
            msg_dialog::display(_("Setup"), $this->errorMessage , ERROR_DIALOG);
            $this->initFailed = TRUE;
            return;
        }
    }


    function execute()
    {
        $list = $this->dataModel->getItemsByType('Package');
        $rel = $this->selectedRelease;
        $displayEntries = array();
        foreach($list as $entry){
            if($entry['parentPath'] == $rel){
                $displayEntries[] = $entry;
            }
        }
        return($this->listing->renderList());
    }


    function getReleaseList()
    {
        $res = array();
        $dist = $this->selectedDistribution;
        $list = $this->dataModel->getItemsByType('Release');
        foreach($list as $base => $entry){
            if($entry['parentPath'] != $dist) continue;
            $res[$entry['path']] = $entry['name'];
        }
        return($res);
    }
    
    
    function getDistList()
    {
        $list = $this->dataModel->getItemsByType('Distribution');
        $res = array();
        foreach($list as $base => $entry){
            $res[$entry['path']] = $entry['name'];
        }
        return($res);
    }
    

    function remove_lock()
    {
    }

    public static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Config management"),
                    "plDescription" => _("Config management"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array(
                        "newConfigManagement" => array("description"  => _("Config management"),
                            "objectClass"  => "FAKE_OC_newConfigManagement")),
                    "plProvidedAcls"=> array()
                    ));
    }
}
?>
