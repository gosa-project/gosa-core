<?php

class ldifViewer extends plugin
{
    private $ldif;    

    function __construct($config, $dn)
    {
        $this->config = &$config;
        $this->dn = $dn;
        $ldap = $this->config->get_ldap_link();
        $this->ldif=$ldap->generateLdif(LDAP::fix($this->dn),'(objectClass=*)',array(),'base');
    }

    function execute()
    {
        $smarty = get_smarty();
        $smarty->assign('ldif', $this->ldif);
        return($smarty->fetch(get_template_path('ldifViewer.tpl')));
    }
}

?>
