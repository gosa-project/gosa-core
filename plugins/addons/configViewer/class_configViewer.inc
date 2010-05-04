<?php

class configViewer extends plugin
{
    var $plHeadline= "Config viewer";
    var $plDescription= "Allows to modify configuration values";

    var $configRegistry = NULL;    
    var $viewType = 'byClass';
    var $viewTypes = array();

    function __construct (&$config, $dn= NULL)
    {
        $this->config = &$config;
        $this->configRegistry = &$config->configRegistry;
        $this->viewTypes = array(
                'byClass' => _("By class name"), 
                'byName'  => _("By property name"), 
                'byStatus'=> _("By status"), 
                'byGroup' => _('By group'));

        
        $this->propertyList = new sortableListing();
        $this->propertyList->setDeleteable(false);
        $this->propertyList->setEditable(false);
        $this->propertyList->setWidth("100%");
        $this->propertyList->setHeight("600px");
        $this->propertyList->setHeader(array(_("Group"),_("Class"),_("Name"),_("Value"), _("Status")));
        $this->propertyList->setDefaultSortColumn(0);
        $this->propertyList->setAcl('rwcdm');
    }

    function execute()
    {
        $all = $this->configRegistry->getAllProperties();
        $data = $lData = array();
        foreach($all as $id => $p){
            $data[$id] = $p;
            $lData[$id] = array('data' => array(
                        $p->getGroup(),
                        $p->getClass(),
                        $p->getName(),
                        $p->getValue(),
                        $p->getStatus()
                        ));
        }
        $this->propertyList->setListData($data,$lData);
        $this->propertyList->update();
        $smarty = get_smarty();
        $smarty->assign('content', $this->propertyList->render());
        return($smarty->fetch(get_template_path('generic.tpl', TRUE)));
    }
}
?>
