<?php

class ldifexport extends plugin
{
    /* Definitions */
    var $plHeadline= "CSV import";
    var $plDescription= "This does something";

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();
    var $view_logged = FALSE;

    var $base;

    function __construct(&$config, $dn= NULL)
    {

        $this->initTime = microtime(TRUE);
        /* Include config object */
        $this->config= &$config;
        $this->ui = get_userinfo();

        $this->baseSelector= new baseSelector($this->config->idepartments, $this->config->current['BASE']);
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

        // Update baseSelector
        if (!$this->baseSelector->update()) {
            msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
        }
        if ($this->base != $this->baseSelector->getBase()) {
            $this->base= $this->baseSelector->getBase();
            $this->is_modified= TRUE;
        }


        // Assign smarty values 
        $smarty= get_smarty();

        $smarty->assign("single","");
        $smarty->assign("base", $this->baseSelector->render());

        // fill in old vars in the Export form
        if(isset($_POST['single'])) $smarty->assign("single",set_post(get_post('single')));

        // Get the LDAP link, to generate the Export
        $ldap = $this->config->get_ldap_link();
        $dn = "";
        if(isset($_POST['sfrmgetsingle']))
        {  
            $dn   = get_post('single');
            $scope= "base";
            $acls = $this->ui->get_permissions($dn,"ldapmanager/ldif");
            $name = "entryExport.ldif";
            if(!$ldap->dn_exists($dn)){
                msg_dialog::display(_("LDAP error"),
                        sprintf(_("No such object %s!"),bold(LDAP::fix($dn))),
                        ERROR_DIALOG);
                $dn ="";
            }

        }elseif(isset($_POST['sfrmgetfull'])){
            $dn   = $this->base;
            $scope= "sub";
            $acls = $this->ui->get_permissions($dn,"ldapmanager/ldif");
            $name = "fullExport.ldif";
        }

        if(!empty($dn)){

            // Check permissions before sending the output
            if(!preg_match("/r/",$acls)){
                msg_dialog::display(_("Permission error"), 
                        sprintf(_("You have no permission to export %s!"), bold(LDAP::fix($dn))),
                        ERROR_DIALOG);
            }else{

                // Check permissions before sending the output
                if(!preg_match("/r/",$acls)){
                    msg_dialog::display(_("Permission error"),
                            sprintf(_("You have no permission to export %s!"), bold(LDAP::fix($dn))),
                            ERROR_DIALOG);
                }else{
                    $data = $ldap->generateLdif(LDAP::fix($dn), 'objectClass=*',$scope);
                    if($data === NULL){
                        msg_dialog::display(_("Error"),
                                sprintf(_("Failed to generate LDAP export: %s"), $ldap->get_error()),
                                ERROR_DIALOG);
                    }else{
                        send_binary_content($data,$name);
                    }
                }
            }
        }
    
        // Show the main form
        return ($smarty->fetch (get_template_path('contentexport.tpl', TRUE)));
    }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
