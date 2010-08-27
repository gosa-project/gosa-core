<?php

class ldifViewer extends plugin
{
    private $ldif;    

    function __construct($config, $dn)
    {
        $this->initTime = microtime(TRUE);
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

        $this->config = &$config;
        $this->dn = $dn;
        $ldap = $this->config->get_ldap_link();
        $this->ldif=$ldap->generateLdif(LDAP::fix($this->dn),'(objectClass=*)','base');
    }

    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('ldif', set_post($this->ldif));
        return($smarty->fetch(get_template_path('ldifViewer.tpl')));
    }
}

?>
