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
        $this->propertyList->setHeight("500px");
        $this->propertyList->setHeader(array(_("Group"),_("Class"),_("Name"),_("Value"), _("Status")));
        $this->propertyList->setDefaultSortColumn(0);
        $this->propertyList->setAcl('rwcdm');
    }

    function execute()
    {
        $all = $this->configRegistry->getAllProperties();
        $data = $lData = array();
        foreach($all as $id => $p){
            $value = htmlentities($p->getValue(), ENT_COMPAT, 'UTF-8');
            $data[$id] = $p;


            $status = $p->getStatus();
            if($status == 'modified'){
                $status = "<button name='save_{$p->getClass()}:{$p->getName()}'>".msgPool::saveButton()."</button>";
                $status.= "<button name='reset_{$p->getClass()}:{$p->getName()}'>".("Reset")."</button>";
            }
            if($status == 'ldap'){
                $status = "<button name='del_{$p->getClass()}:{$p->getName()}'>".msgPool::delButton()."</button>";
            }

            $lData[$id] = array('data' => array(
                        $p->getGroup(),
                        $this->configRegistry->classToName[$p->getClass()],
                        $p->getName(),
                        "<input type='text' value=\"{$value}\" name='{$p->getClass()}:{$p->getName()}'>",
                        $status
                        ));
        }
        $this->propertyList->setListData($data,$lData);
        $this->propertyList->update();
        $smarty = get_smarty();
        $smarty->assign('content', $this->propertyList->render());
        return($smarty->fetch(get_template_path('generic.tpl', TRUE)));
    }

    function save_object()
    {
        $all = $this->configRegistry->getAllProperties();
        foreach($all as $id => $p){
            if(isset($_POST[$p->getClass().":".$p->getName()])){
                $val = get_post($p->getClass().":".$p->getName());
                if($val != $p->getValue()){
                    $p->setValue($val);
                }
            }
        }
    }
}
?>
