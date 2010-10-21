<?php

class newConfigManagement extends plugin
{
    var $initTime;
    var $plHeadline = "Config management";
    var $plDescription = "Config management";

    var $installationMethods = array();

    function __construct($config, $dn)
    {
        $this->config = &$config;

        $this->listing = new ConfigManagementListing($this->config, get_userinfo());
        $this->dataModel = new ConfigManagementDataModel();


#       $dists = $this->getDistributions();
#       foreach($dists as $name => $dist){
#
#           
#           $this->dataModel->addItem
#   
#           
#       }
#        
#   
#
#
#
#       // Get installation methods from the backend.
#       $this->loadInstallationMethods();
    }

    
    function getDistributions()
    {
        return(
            array(
                'debian' => 
                    array('type' => 'deb', 'install_method' => 'puppet'),
                'suse' => 
                    array('type' => 'rpm', 'install_method' => 'muppet')
            ));
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


        $this->getReleases();
    }

    

   
    function getReleases()
    {
        $rpc = $this->config->getRpcHandle();
        $res = $rpc->getReleases();
        if(!$rpc->success()){
            $this->rpcError = TRUE;
            $this->errorMessage = $rpc->get_error();;
            return(NULL);
        }
        return($res);
    }
 

    function execute()
    {
        return($this->listing->renderList());
    }

    function remove_lock()
    {}

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
