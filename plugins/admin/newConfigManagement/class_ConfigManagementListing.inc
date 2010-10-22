<?php

class ConfigManagementListing extends management
{

    private $base = "";

    private $distributionSelector = NULL;
    private $releaseSelector = NULL;


    function __construct($config, $ui)
    {
        // Set storage points - We do not have any - We just create a fake list which lists all items
        $this->storagePoints = array("");
        $this->rebuildListing();

        $this->distributionSelector = new ItemSelector(array(),'/root','/root');
        $this->releaseSelector = new ItemSelector(array(),'/root','/root');
    }

    function save_object()
    {
        $this->distributionSelector->update();    
        $this->releaseSelector->update();    
    }

    function getSelectedDistribution()
    {
        return($this->distributionSelector->getBase());
    }

    function setDistribution($dist)
    {
        return($this->distributionSelector->setBase($dist));
    }

    function setRelease($rel)
    {
        return($this->releaseSelector->setBase($rel));
    }

    function getSelectedRelease()
    {
        return($this->releaseSelector->getBase());
    }


    function setDistributions($distributions) 
    {
        $this->distributionSelector->setBases($distributions);
        $this->distributionSelector->update(TRUE);
    }

    function setReleases($releases) 
    {
        $this->releaseSelector->setBases($releases);
        $this->releaseSelector->update(TRUE);
    }

    function renderList()
    {
        session::set('DEVICE_ITEMS', array());

        $filter = $this->getFilter();
        $headpage = $this->getHeadpage();
        $headpage->update();
        $smarty = get_smarty();
        
        $smarty->assign("DISTRIBUTION", $this->distributionSelector->render());
        $smarty->assign("RELEASE", $this->releaseSelector->render());
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
    }
} 

?>
