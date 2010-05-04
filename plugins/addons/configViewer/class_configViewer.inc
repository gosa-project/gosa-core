<?php


class configViewer extends management
{
    var $plHeadline= "Preferences";
    var $plDescription= "Configure global and special GOsa settings like hooks and plugin parameters";
    var $plIcon  = "plugins/configViewer/images/plugin.png";

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        // Build filter
        if (0&&session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("property-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("property-list.xml", true));
        $headpage->registerElementFilter("propertyName", "configViewer::propertyName");
        $headpage->registerElementFilter("propertyGroup", "configViewer::propertyGroup");
        $headpage->registerElementFilter("propertyClass", "configViewer::propertyClass");
        $headpage->registerElementFilter("propertyValue", "configViewer::propertyValue");
        $headpage->setFilter($filter);
        parent::__construct($config, $ui, "property", $headpage);

        $this->registerAction("saveProperties","saveProperties");
        $this->registerAction("cancelProperties","cancelProperties");
    }

    function cancelProperties()
    {
        $this->config->configRegistry->reload($force=TRUE);
    }

    function saveProperties()
    {
        $this->config->configRegistry->saveChanges();
    }

    function detectPostActions()
    {
        $action = management::detectPostActions();
        if(isset($_POST['saveProperties']))  $action['action'] = 'saveProperties';
        if(isset($_POST['cancelProperties']))  $action['action'] = 'cancelProperties';
        return($action);
    }

    protected function removeEntryRequested($action="",$target=array(),$all=array())
    {
        foreach($target as $dn){
            list($class,$name) = preg_split("/:/", $dn);
            if($this->config->configRegistry->propertyExists($class,$name)){
                $prop = $this->config->configRegistry->getProperty($class,$name);
                $prop->restoreDefault();
            }
        }
    } 

    static function propertyGroup($group, $description = array())
    {
        $title = _("No description");
        if(!empty($description[0])) $title=$description[0];
        return("<span title=\"{$title}\">{$group[0]}</span>");
    }
    static function propertyClass($class, $description = array())
    {
        global $config;
        if(isset($config->configRegistry->classToName[$class[0]])){
            $class = $config->configRegistry->classToName[$class[0]];
        }else{
            $class = $class[0];
        }
        $title = _("No description");
        if(!empty($description[0])) $title=$description[0];
        return("<span title=\"{$title}\">{$class}</span>");
    }
    static function propertyName($cn, $description = array())
    {
        $title = _("No description");
        if(!empty($description[0])) $title=$description[0];
        return("<span title=\"{$title}\">{$cn[0]}</span>");
    }
    static function propertyValue($class,$cn, $value,$type,$default,$description)
    {
        $name  = "{$class[0]}:{$cn[0]}";
        $value = htmlentities($value[0], ENT_COMPAT , 'UTF-8');
        $title = _("No description");
        if(!empty($description[0])) $title=$description[0];
        switch($type[0]){
            case 'string':
            case 'integer':
                $res = "<input style='width:97%' title=\"{$title}\" type='text' value=\"{$value}\" name={$name}>";
                break;
            default: $res = ""; 
        }
        
        return($res);
    }
}
?>
