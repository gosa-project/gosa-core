<?php

class ConfigManagementListing extends management
{

    private $base = "";

    private $releaseSelector = NULL;
    private $itemSelector = NULL;


    function __construct($config, $ui)
    {
        // Set storage points - We do not have any - We just create a fake list which lists all items
        $this->storagePoints = array("");
        $this->rebuildListing();

       
        $this->releaseSelector = new ItemSelector(array('cn=/'=>'/'),"cn=/","cn=/"); 
        $this->itemSelector = new ItemSelector(array('cn=/'=>'/'),"cn=/","cn=/"); 
    }

    function setRelease($base)
    {
        $this->releaseSelector->setBase($base);
    }

    function setReleases ($releases) 
    {
        $this->releaseSelector->setBases($releases);
    }


    function setItem($base)
    {
        $this->itemSelector->setBase($base);
    }

    function setItems ($items) 
    {
        $this->itemSelector->setBases($items);
    }
    function setAllowedItems() {} 
    function renderList()
    {
        session::set('DEVICE_ITEMS', array());

        $filter = $this->getFilter();
        $headpage = $this->getHeadpage();
        $headpage->update();
        $smarty = get_smarty();
        
        $this->releaseSelector->update(TRUE);    
        $this->itemSelector->update(TRUE);    

        $smarty->assign("RELEASE", $this->releaseSelector->render());
        $smarty->assign("ITEM", $this->itemSelector->render());
        $display = $headpage->render();
        return($this->getHeader().$display);

    }

   
    function rebuildListing()
    {
        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("DeviceConfig-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Load service xml file and fill in placeholders
        $contents = file_get_contents(get_template_path("DeviceConfig-list.xml", true));
        $contents = preg_replace("/%ITEMS%/", "", $contents);
        $headpage = new listing($contents,TRUE);
        $headpage->setBase($this->base);
        $headpage->setFilter($filter);

        parent::__construct($this->config, $this->ui, "services", $headpage);

        // Register default actions
        $this->registerAction("new",        "newEntry");
        $this->registerAction("edit",       "openEntry"); // !! We forward 'edit' to 'open' to have a department like navigation.
        $this->registerAction("editEntry",  "editEntry");

        $this->registerAction("saveItemChanges", "saveItemChanges");
        $this->registerAction("cancelItemEdit", "closeDialogs");
        $this->registerAction("cancelItemAdd", "closeDialogs");
        $this->registerAction("saveItemAdd", "saveItemAdd");
    }

    
} 

?>
