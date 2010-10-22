<?php

class newConfigManagement extends plugin
{
    var $initTime;
    var $plHeadline = "Config management";
    var $plDescription = "Config management";

    var $selectedContainer;
    var $selectedItem;

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
            $this->dataModel->addItem('Distribution','/root', $dist['name'], $dist);
            foreach($dist['releases'] as $release){
                $distPath = "/root/{$dist['name']}";
                $this->dataModel->addItem('Release',$distPath, $release['name'], $release);

                foreach($dist['components'] as $component){
                    $comPath = "{$distPath}/{$release['name']}";
                    $this->dataModel->addItem('Component',$comPath, $component['name'], $component);
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
        $this->selectedContainer = $this->listing->getSelectedContainer();
        $this->selectedItem = $this->listing->getSelectedItem();
  
        // Get a list of all available distributions and releases.
        $distList = $this->getDistList();
        $releaseList = $this->getItemList();

        // Ensure that we've valid values selected.
        if(!isset($releaseList[$this->selectedItem])){
            if(count($releaseList)){
                $this->selectedItem = key($releaseList);
            }else{
                $this->selectedItem = "";
            }
        }

        // Update list of items within the selected container. 
        $this->updateItemList($this->selectedContainer);

        // Transfer checked values back to the listing class.
        $this->listing->setContainer($this->selectedContainer);
        $this->listing->setItem($this->selectedItem);
        $this->listing->setContainers($this->getDistList());
        $this->listing->setItems($this->getItemList());
    }


    function updateItemList($path)
    {
        // Fist get Item and check if it is an release 
        if($this->dataModel->itemExistsByPath($path)){
            $data = $this->dataModel->getItemByPath($path);

            // Only releases can contain config-items.
            if($data['type'] == 'Release' && $data['status'] != "fetched"){

                $rpc = $this->config->getRpcHandle();
                $res = $rpc->listConfigItems($data['name']);
                if(!$rpc->success()){
                    msg_dialog::display(_("Error"), 
                            sprintf(_("Failed to load distributions, error was '%s'!"), 
                                $rpc->get_error()),ERROR_DIALOG);
                }else{
                    $this->dataModel->setItemStatus($path, 'käse');
                    foreach($res as $itemPath => $type){
                
                        // Make names dataModel conform
                        $itemPath = $path.preg_replace("/^\//","/root", $itemPath);
                        $name = preg_replace("/^.*\//","",$itemPath);   

                        $itemPath = preg_replace("/\/[^\/]*$/","", $itemPath);

                        print_a(array($type, $itemPath, $name));
 
                        $this->dataModel->addItem($type, $itemPath, $name); 
                    }
                }
            }
        }
    }


    /*! \brief  Generate the HTML content for this plugin.
     *          Actually renders the listing widget..
     */
    function execute()
    {
        // Get the selected release and store it in a session variable
        //  to allow the configFilter to access it and display the
        //  packages and items.
        $item = $this->dataModel->getItemByPath($this->selectedItem);
        session::set('CONFIG_ITEM', $item);

        return($this->listing->renderList());
    }


    /*! \brief  Returns a simply list of all releases of the CURRENT distribution.
     *          This list will then be used to generate the entries of the 
     *           ItemSelectors in the listing class.
     */
    function getItemList()
    {
        $data = $this->dataModel->getItemByPath('/root');
        return($this->__recurseItem($data));
        $res = array();
        $dist = $this->selectedContainer;
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
        $data = $this->dataModel->getItemByPath('/root');
        return($this->__recurseItem($data));
       

        $list = $this->dataModel->getItemsByType('Distribution');
        $res = array();
        foreach($list as $base => $entry){
            $res[$entry['path']] = $entry['name'];
        }
        return($res);
    }
    

    function __recurseItem($item)
    {
        $res = array();
        $res[$item['path']] = $item['type'];
        if(count($item['children'])){
            foreach($item['children'] as $child){
                $res = array_merge($res, $this->__recurseItem($child));
            }
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
