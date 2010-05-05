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
        if (session::global_is_set(get_class($this)."_filter")){
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


    function execute()
    {
        // Walk trough all properties and check if there posts for us.
        $all = $this->config->configRegistry->getAllProperties();
        foreach($all as $prop){
            $post = "{$prop->getClass()}:{$prop->getName()}";
            if(isset($_POST[$post]) && !in_array($prop->getStatus(),array('removed'))){
                $prop->setValue(get_post($post));
            }
        }
        return(management::execute());
    }

    function renderList()
    {
        // Walk trough all properties and check if there posts for us.
        $all = $this->config->configRegistry->getAllProperties();

        $htmlTooltips = "";
        $jsTooltips = "";
        foreach($all as $prop){
            $modified = in_array($prop->getStatus(),array('modified','removed'));
            if($modified) break;
        }
       
        $smarty = get_smarty();
        $smarty->assign('is_modified', $modified);
        $smarty->assign('htmlTooltips', $htmlTooltips);
        $smarty->assign('jsTooltips', $jsTooltips);
        return(management::renderList());
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
        return($group[0]);
    }
    static function propertyClass($class, $description = array())
    {
        global $config;
        if(isset($config->configRegistry->classToName[$class[0]])){
            $class = $config->configRegistry->classToName[$class[0]];
        }else{
            $class = $class[0];
        }
        return($class);
    }
    static function propertyName($class,$cn, $description = array())
    {
        $id = "{$class[0]}_{$cn[0]}";

        $title = _("No description");
        if(isset($description[0])) $title = htmlentities($description[0],ENT_COMPAT, 'UTF-8');
        $title = preg_replace("/\n/", "<br>", $title);
        $tooltip = "<div id='tooltip_{$id}' class='tooltip'>".$title."</div>";
        return($tooltip."<span title='tooltip_{$id}'>{$cn[0]}</span>");
    }
    static function propertyValue($class,$cn,$value,$type,$default,$defaults)
    {
        $name  = "{$class[0]}:{$cn[0]}";
        $value = htmlentities($value[0], ENT_COMPAT , 'UTF-8');
        switch($type[0]){
            case 'switch':
                
                if(!empty($defaults[0])){
                    $data = call_user_func(preg_split("/::/", $defaults[0]));
                    if(is_array($data)){
                        $res = "<select size=1 name=\"{$name}\" style='width:100%'>";
                        foreach($data as $oValue => $oDesc){
                            if($oValue == $value){
                                $res.="<option selected value=\"{$oValue}\">{$oDesc}</option>\n";
                            }else{
                                $res.="<option value=\"{$oValue}\">{$oDesc}</option>\n";
                            }
                        }
                        $res.= "</select>";
                    }
                }
                break;
            case 'string':
            case 'integer':
                $res = "<input style='width:97%' type='text' value=\"{$value}\" name=\"{$name}\">";
                break;
            default: $res = ""; 
        }
        
        return($res);
    }
}
?>
