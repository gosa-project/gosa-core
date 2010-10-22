<?php

class newConfigManagement extends plugin
{
    var $initTime;
    var $plHeadline = "Config management";
    var $plDescription = "Config management";

    var $selectedDistribution;
    var $selectedRelease;

    var $dataModel = NULL;
    var $listing = NULL;


    /*! \brief  Initialize the plugin and finally update the data model.
     */
    function __construct($config, $dn)
    {
        $this->config = &$config;
        $this->listing = new ConfigManagementListing($this->config, get_userinfo());

        // Request an update of the data model
        $this->updateDataModel();
    }


    /*! \brief  Updates all distributions, releases, packages and items in the dataModel
     *          Load information from the backend.
     */
    function updateDataModel()
    {
        // Recreate the data model, to have a clean and fresh instance.
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

                // Load items
                $items = $rpc->listConfigItems($release);
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"), sprintf(_("Failed to load items, error was '%s'!"), $rpc->get_error()),ERROR_DIALOG);
                    return(NULL);
                }
                foreach($items as $package){
                    $this->dataModel->addItem('Item',"/root/{$dist}/{$release}", $package['name'], $package);
                }
            }
        }

    }


    /*! \brief  Keep track of posted values and populate those 
     *           which are interesting for us.
     *          Inspects the _POST and _GET values.
     */
    function save_object()
    {
        // Update the listing class, this is necessary to get post
        //  actions from it.
        $this->listing->save_object();

        // Get the selected distribution and release from the listing widget.
        $this->selectedDistribution = $this->listing->getSelectedDistribution();
        $this->selectedRelease = $this->listing->getSelectedRelease();
   
        // Get a list of all available distributions and releases.
        $distList = $this->getDistList();
        $releaseList = $this->getReleaseList();

        // Ensure that we've valid values selected.
        if(!isset($releaseList[$this->selectedRelease])){
            if(count($releaseList)){
                $this->selectedRelease = key($releaseList);
            }else{
                $this->selectedRelease = "";
            }
        }

        // Transfer checked values back to the listing class.
        $this->listing->setDistribution($this->selectedDistribution);
        $this->listing->setRelease($this->selectedRelease);
        $this->listing->setDistributions($this->getDistList());
        $this->listing->setReleases($this->getReleaseList());
    }


    /*! \brief  Generate the HTML content for this plugin.
     *          Actually renders the listing widget..
     */
    function execute()
    {
        // Get the selected release and store it in a session variable
        //  to allow the configFilter to access it and display the
        //  packages and items.
        $item = $this->dataModel->getItemByPath($this->selectedRelease);
        session::set('CONFIG_ITEM', $item);

        return($this->listing->renderList());
    }


    /*! \brief  Returns a simply list of all releases of the CURRENT distribution.
     *          This list will then be used to generate the entries of the 
     *           ItemSelectors in the listing class.
     */
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
    
    
    /*! \brief  Returns a simply list of all distributions.
     *          This list will then be used to generate the entries of the 
     *           ItemSelectors in the listing class.
     */
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
