<?php

class ldifViewer extends plugin
{
    private $ldif;
        private $success= FALSE;
        private $error= "";

    function __construct($config, $dn)
    {
        $this->initTime = microtime(TRUE);
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

        $this->config = &$config;
        $this->dn = $dn;
        $ldap = $this->config->get_ldap_link();
        $this->ldif=$ldap->generateLdif(LDAP::fix($this->dn),'(objectClass=*)','base');
        if(!$ldap->success()){
                msg_dialog::display(_("Error"),
                                msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_READ),
                        ERROR_DIALOG);

        }
        $this->success = $ldap->success();
        $this->error = $ldap->get_error();
    }

    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('success', $this->success);
        $smarty->assign('error', $this->error);
        $smarty->assign('ldif', set_post($this->ldif));
        return($smarty->fetch(get_template_path('ldifViewer.tpl')));
    }
}

?>
