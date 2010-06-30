<?php

class dbPluginStatus extends plugin
{

   var $installedPlugins = array();
   var $disabledPlugins = array();

   // The HTML list containing the plugin data.
   var $pluginList = NULL;

    function __construct($config)
    {
        // Construct the class
        parent::__construct($config, NULL);

        // Collect plugin information already collected by GOsa.
        $this->installedPlugins = $this->config->configRegistry->getListOfPlugins();
        $this->disabledPlugins = $this->config->configRegistry->getDisabledPlugins();
   
        // Construct the plugin list.
        $this->pluginList= new sortableListing($this->gosaLoginRestriction);
        $this->pluginList->setDeleteable(false);
        $this->pluginList->setEditable(false);
        $this->pluginList->setColspecs(array('*'));
        $this->pluginList->setWidth("100%");
        $this->pluginList->setHeight("200px");
        $this->pluginList->setAcl("rwcdm");

    }

    function execute()
    {
        $smarty = get_smarty();

        // Build up list data
        $data = $lData = array();
        foreach($this->installedPlugins as $plugin => $plInfo){

            // Build plugin name
            $name = $plugin;
            if(isset($plInfo['plShortName'])){
                $name = $plInfo['plShortName']; 
            }

            // Build plugin description
            $desc = "-";
            if(isset($plInfo['plDescription'])){
                $desc = $plInfo['plDescription']; 
            }

            // Detect the plugin status
            $status = 'OK';
            if(isset($this->disabledPlugins[$plugin])){
                $status = 'Failure';

                // Check if an invalid schema is the reason
                $reasons = $this->config->configRegistry->getSchemaResults();
                if(isset($plInfo['plRequirements']['ldapSchema'])){
                    foreach($plInfo['plRequirements']['ldapSchema'] as $class => $requirements){
                        if(isset($reasons['versionMismatch'][$class])){
                            $reason = strip_tags($reasons['versionMismatch'][$class]);
                            $status = "<span style='color:red;' title=\"{$reason}\">"._("Version mismatch")."</span>";
                        }
                        if(isset($reasons['missing'][$class])){
                            $reason = strip_tags($reasons['missing'][$class]);
                            $status = "<span style='font-color:red;' title=\"{$reason}\">"._("Schema missing")."</span>";
                        }
                    }
                }
            }

            // Add entry/line to the list
            $data[$plugin] = $plInfo;
            $lData[$plugin] = array('data' => array($name,$desc, $status));
        }
        $this->pluginList->setListData($data,$lData);
        $this->pluginList->update();
        $smarty->assign('pluginList', $this->pluginList->render());
        return($smarty->fetch(get_template_path('dbPluginStatus/contents.tpl', TRUE)));
    }

    function save_object()
    {
        parent::save_object();

        $this->pluginList->save_object();
    }

    function save()
    {
        parent::save();
    }

    function check()
    {
        return(parent::check());
    }

    function remove_from_parent()
    {
        parent::remove_from_parent();
    }
}

?>
