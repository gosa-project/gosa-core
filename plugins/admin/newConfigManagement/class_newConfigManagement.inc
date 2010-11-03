<?php

/*! \brief  This class allows to manage backend config items and packages.
 */
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
        $this->listing = new ConfigManagementListing($this->config, get_userinfo(), $this);

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
        $cont = $this->listing->getSelectedContainer();
        $item = $this->listing->getSelectedItem();
        $this->setCurrentContainer($cont, $item);
    }


    /*! \brief  Load extended sub-objecte like 'config items' or 'packages'
     *           for the given release path.
     *  @param  String  The release path to load sub-objects for.
     *  @return NULL 
     */
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
                    $rpc = $this->config->getRpcHandle();
                    $res = $rpc->getPackages($data['name']);

                    return;
                    foreach($res as $itemPath => $type){

                        // Make names dataModel conform
                        $itemPath = $path.'/'.preg_replace("/^\//","/root", $itemPath);
                        $name = preg_replace("/^.*\//","",$itemPath);   
                        $itemPath = preg_replace("/\/[^\/]*$/","", $itemPath);
                        $this->dataModel->addItem($type, $itemPath, $name); 
                    }
                    $this->dataModel->setItemStatus($path, 'fetched');
                }
            }
        }
    }


    /*! \brief  Sets the currently selected container and item path. 
     *  @param  String  The path of the container to set.
     *  @param  String  The path of the item to set.
     *  @return 
     */
    function setCurrentContainer($cont, $item)
    {
        $this->selectedContainer = $cont;
        $this->selectedItem = $item;

        // Get a list of all available container and items.
        $distList = $this->getContainerList();
        $releaseList = $this->getItemList();

        // Ensure that we've valid values selected.
        if(!isset($releaseList[$this->selectedItem])){
            if(count($releaseList)){
                echo "Fallback";
                $this->selectedItem = key($releaseList);
            }else{
                echo "Fallback 2";
                $this->selectedItem = "";
            }
        }

        // Update list of items within the selected container. 
        $this->updateItemList($this->selectedContainer);

        // Transfer checked values back to the listing class.
        $this->listing->setContainer($this->selectedContainer);
        $this->listing->setItem($this->selectedItem);
        $this->listing->setContainers($this->getContainerList());
        $this->listing->setItems($this->getItemList());
    }


    /*! \brief  Generate the HTML content for this plugin.
     *          Actually renders the listing widget..
     */
    function execute()
    {
        // Get the selected release and store it in a session variable
        //  to allow the configFilter to access it and display the
        //  packages and items.
        $res = $this->listing->execute();
        $this->listing->setListingTypes($this->getListingTypes());

        return($res);
    }


    /*! \brief      Returns a list of items which will then be displayed 
     *               in the management-list. 
     *              (The management class calls this method from its execute())
     *  @return     Array   A list of items/objects for the listing. 
     */
    function getItemsToBeDisplayed()
    {

        $path = $this->selectedContainer.$this->selectedItem;
        $item = $this->dataModel->getItemByPath($path);
        return($item);
    }


    /*! \brief  Returns a simply list of all releases of the CURRENT distribution.
     *          This list will then be used to generate the entries of the 
     *           ItemSelectors in the listing class.
     */
    function getItemList()
    {
        $data = $this->dataModel->getItemByPath($this->selectedContainer);
        $res = array();
        $res[""] = array("name" => "/", "desc" => "");
        $res = array_merge($res,$this->__recurseItem($data, array('Component')));
        return($res);


        if($data['type'] != 'Release'){
        }else{
            $res = array_merge($res,$this->__recurseItem($data, array('Component'),$this->selectedContainer));
        }
        return($res);
    }


    /*! \brief  Returns a simply list of all distributions.
     *          This list will then be used to generate the entries of the 
     *           ItemSelectors in the listing class.
     */
    function getContainerList()
    {
        $data = $this->dataModel->getItemByPath('/root');
        $res = array();
        $res["/root"] = array("name" => "/", "desc" => "");
        $res = array_merge($res,$this->__recurseItem($data, array('Distribution','Release')));

        print_a($res);
        return($res);
    }


    /*! \brief  Recursivly walks through an item and collects all path and name info.
     *          The reult can then be used to fill the ItemSelector.
     *  @param  Array   The Item to recurse. 
     *  @param  Array   The type of of objects to collect. 
     *  @param  String  The parent path prefix which should be removed.
     *  @return Array   An array containing Array[path] = name
     */
    function __recurseItem($item, $types, $parent = "")
    {
        $res = array();
        if(in_array($item['type'], $types)){
            $path = preg_replace("/".preg_quote($parent,'/')."/","",$item['path']);
            $res[$path] = array('name' => $item['name'],'desc'=>$item['type']);
        }
        if(count($item['children'])){
            foreach($item['children'] as $child){
                $res = array_merge($res, $this->__recurseItem($child, $types, $parent));
            }
        }
        return($res);
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


    /*! \brief   Returns a info list about all items we can manage,
     *            this used to fill the listings <objectType> settings.
     *  @return Array   An array with item info.
     */
    function getListingTypes()
    {
        $types= array();
        $types['Distribution']['objectClass'] = 'Distribution';
        $types['Distribution']['label'] = _('Distribution');
        $types['Distribution']['image'] = 'images/lists/edit.png';
        $types['Distribution']['category'] = 'Device';
        $types['Distribution']['class'] = 'Device';

        $types['Release']['objectClass'] = 'Release';
        $types['Release']['label'] = _('Release');
        $types['Release']['image'] = 'images/lists/delete.png';
        $types['Release']['category'] = 'Device';
        $types['Release']['class'] = 'Device';

        $types['Component']['objectClass'] = 'Component';
        $types['Component']['label'] = _('Component');
        $types['Component']['image'] = 'plugins/users/images/select_user.png';
        $types['Component']['category'] = 'Device';
        $types['Component']['class'] = 'Device';
        return($types);
    }


    /*! \brief  Acts on edit requests and opens an edit dialog for the received item-id.
     *          (This action is received from the ConfigManagementListing class.)
     *  @param  Array   The items ids. (May contain multiple ids)
     *  @return  
     */
    function editEntry($ids)
    {
        foreach($ids as $id){
            echo "<br>Edit {$id}";
        }
    }


    /*! \brief  Acts on remove requests.
     *          (This action is received from the ConfigManagementListing class.)
     *  @param  Array   The items ids. (May contain multiple ids)
     *  @return  
     */
    function removeEntry($ids)
    {
        foreach($ids as $id){
            echo "<br>Remove {$id}";
        }
    }


    /*! \brief  Acts on open requests.
     *          (This action is received from the ConfigManagementListing class.)
     *  @param  Array   The items ids. (May contain multiple ids)
     *  @return  
     */
    function openEntry($ids)
    {
        $id = $ids[0];
        $item = $this->dataModel->getItemById($id);
        if($item){
            if(in_array($item['type'], array('Distribution', 'Release'))){
                $this->setCurrentContainer($item['path'], $this->selectedItem);
            }elseif(in_array($item['type'], array('Component'))){
                $next = preg_replace("/".preg_quote($this->selectedContainer,'/')."/","", $item['path']);
                $this->setCurrentContainer($this->selectedContainer, $next);
            }else{
                $this->editEntry(array($id));
            }
        }
    }


    /*! \brief      The plugins ACL and plugin-property definition. 
     *  @return 
     */
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


    /*! \brief   
     *  @param  
     *  @return 
     */
    function remove_lock()
    {
    }


}
?>
