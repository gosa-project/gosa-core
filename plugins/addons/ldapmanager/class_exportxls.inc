<?php

class xlsexport extends plugin
{
    /* Definitions */
    var $plHeadline= "XLS import";
    var $plDescription= "This does something";
    var $access= "";

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();
    var $ui;
    var $view_logged = FALSE;

    var $base ="";
    var $baseSelector ="";

    function __construct(&$config, $dn= NULL)
    {
        /* Include config object */
        $this->initTime = microtime(TRUE);
        $this->config= &$config;

        $this->ui = get_userinfo();

        $this->base = $this->config->current['BASE'];
        $this->baseSelector= new baseSelector($this->config->idepartments, $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Log view */
        if(!$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","all/".get_class($this),$this->dn);
        }

        $dn = $this->config->current['BASE'];

        /* Set government mode */
        $smarty= get_smarty();
        $smarty->assign("type",FALSE);
        $smarty->assign("depselectivbb","");

        if (!$this->baseSelector->update()) {
            msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
        }
        if ($this->base != $this->baseSelector->getBase()) {
            $this->base= $this->baseSelector->getBase();
            $this->is_modified= TRUE;
        }

        /* Check permissions for export */
        // fill in old vars in the Export form
        if((isset($_POST['single']))&&(isset($_POST['sfrmgetsingle']))){
            $smarty->assign("single",set_post(get_post('single')));
            $smarty->assign("type",set_post("single"));
            $a=get_post('single');
            $b=$this->base;

        }
        if(isset($_POST['sfrmgetfull'])){
            $smarty->assign("type","full");
            $dn = $this->base;
        } else {
            $smarty->assign("depselectfull", "");
        }
        if((isset($_POST['selivbb']))&&(isset($_POST['sfrmgetivbb']))){
            $smarty->assign("depselectivbb",set_post(get_post('selivbb')));
            $smarty->assign("type","ivbb");
            $dn = get_post('selivbb');
        } else {
            $smarty->assign("depselectivbb", "");
        }

        // Set values for optionlist in form
        $bases      = array();
        $acl_bases  = $this->ui->get_module_departments("ldapmanager");
        foreach($this->config->idepartments as $base_dn => $name){
            if(in_array_ics($base_dn,$acl_bases)){
                $bases[$base_dn] = $name;
            }
        }
        $smarty->assign("deplist", $bases);
        $smarty->assign("choicelist",set_post(array( get_people_ou()             =>"users" ,
                        get_groups_ou()             =>"groups" ,
                        get_ou("systemManagement", "systemRDN")         =>"computers",
                        get_ou("servgeneric", "serverRDN")          =>"servers",
                        "dc=addressbook,"           =>"addressbook")));

        // Get the LDAP link, to generate the Export
        $ldap = $this->config->get_ldap_link();

        $smarty->assign ("govmode", $this->config->get_cfg_value("core","honourIvbbAttributes"));
        $smarty->assign("LDIFError",FALSE);

        if(isset($dn))
        {
            if(!($ldap->dn_exists($dn) && $dn != "")){
                $smarty->assign("LDIFError",TRUE);
                msg_dialog::display(_("Error"), _("Cannot export selected entries!"), ERROR_DIALOG);
            }
        }

        /* check alcs for given dn */
        $acls ="";
        if(isset($dn)){
            $acls = $this->ui->get_permissions($dn,"ldapmanager/ldif");
        }

        if(!preg_match("/r/",$acls) && (isset($_POST['sfrmgetsingle']) || isset($_POST['sfrmgetfull']) || isset($_POST['sfrmgetivbb']))){

            /* Show error msg */      
            msg_dialog::display(_("Permission error"), sprintf(_("You have no permission to export %s!"), bold(LDAP::fix($dn))), ERROR_DIALOG);

            /* Show main page */
            $smarty->assign("LDIFError",TRUE);
            $smarty->assign("para","");
        }else{

            // Export a single LDAP entry
            if(isset($_POST['sfrmgetsingle']))
            {
                $smarty->assign("para","?ivbb=2&amp;d=".base64_encode(get_post('single'))."&n=".base64_encode($this->base));	
            }
            elseif(isset($_POST['sfrmgetfull']))
            {
                $smarty->assign("para","?ivbb=3&amp;dn=".base64_encode($this->base));
            }
            elseif(isset($_POST['sfrmgetivbb']))
            {
                $smarty->assign("para","?ivbb=4&amp;dn=".base64_encode(get_post('selivbb')));
            }
        }

        $smarty->assign("base", $this->baseSelector->render());
        return ($smarty->fetch (get_template_path('contentexportxls.tpl', TRUE)));
    }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
