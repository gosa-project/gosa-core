<?php

class ConfigManagementListing extends management
{

    private $base = "";

    private $containerSelector = NULL;
    private $itemSelector = NULL;

    private $objectTypeStr = "";

    function __construct($config, $ui, $parent)
    {
        // Set storage points - We do not have any - We just create a fake list which lists all items
        $this->storagePoints = array("");
        $this->rebuildListing();

        $this->containerSelector = new ItemSelector(array(),'/root','/root');
        $this->itemSelector = new ItemSelector(array(),'','');

        $this->parent = &$parent;
    }

    function save_object()
    {
        $this->containerSelector->update();    
        $this->itemSelector->update();    
    }

    function getSelectedContainer()
    {
        return($this->containerSelector->getBase());
    }

    function setContainer($dist)
    {
        return($this->containerSelector->setBase($dist));
    }

    function setItem($rel)
    {
        return($this->itemSelector->setBase($rel));
    }

    function getSelectedItem()
    {
        return($this->itemSelector->getBase());
    }


    function setContainers($containers) 
    {
        $this->containerSelector->setBases($containers);
        $this->containerSelector->update(TRUE);
    }

    function setItems($items) 
    {
        $this->itemSelector->setBases($items);
        $this->itemSelector->update(TRUE);
    }


    function renderList()
    {
        session::set('DEVICE_ITEMS', array());

        $headpage = $this->getHeadpage();
        $headpage->update();
        $smarty = get_smarty();
        
        $smarty->assign("CONTAINER", $this->containerSelector->render());
        $smarty->assign("ITEM", $this->itemSelector->render());
        $display = $headpage->render();
        return($this->getHeader().$display);

    }


    function setListingTypes($types)
    {
        $str = "";
        foreach($types as $typeData){
            $str.= "\n<objectType>";
            foreach($typeData as $key => $value){
                $str .= "\n\t<{$key}>{$value}</{$key}>";
            }
            $str.= "\n</objectType>\n";

        }
        $this->objectTypeStr = $str;
        $this->rebuildListing();
    }

   
    function rebuildListing()
    {

        // Handle actions (POSTs and GETs)
        $str = $this->handleActions($this->detectPostActions());
        if($str) return($this->getHeader().$str);


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
        $contents = preg_replace("/%TYPES%/", $this->objectTypeStr, $contents);
        $contents = preg_replace("/%ITEMS%/", "", $contents);

        $headpage = new listing($contents,TRUE);
        $headpage->setBase($this->base);
        $headpage->setFilter($filter);

        parent::__construct($this->config, $this->ui, "services", $headpage);

        // Register default actions
        $this->registerAction("new",    "newEntry");
        $this->registerAction("edit",   "editEntry");
        $this->registerAction("apply",  "applyChanges");
        $this->registerAction("save",   "saveChanges");
        $this->registerAction("cancel", "cancelEdit");
        $this->registerAction("cancelDelete", "cancelEdit");
        $this->registerAction("remove", "removeEntryRequested");
        $this->registerAction("removeConfirmed", "removeEntryConfirmed");

        $this->registerAction("config-filter","editFilter");
        $this->registerAction("saveFilter","saveFilter");
        $this->registerAction("cancelFilter","cancelFilter");

    }

    function removeEntryRequested($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $this->parent->removeEntry($target);
    }

    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $this->parent->editEntry($target);
    }
} 

?>
