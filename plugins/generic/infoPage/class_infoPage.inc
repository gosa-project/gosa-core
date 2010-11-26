<?php

class infoPage extends plugin
{
    private $ui;

    function __construct($config)
    {
        $this->config = &$config;
        $this->ui = get_userinfo();

        plugin::plugin($config, $this->ui->dn);
    }

    function execute()
    {
        $smarty = get_smarty();

        $personalInfoAllowed = FALSE;
        foreach(array("uid","sn","givenName","street","l","o","ou","jpegPhoto","personalTitle",
            "academicTitle","dateOfBirth","homePostalAddress","homePhone","departmentNumber",
            "employeeNumber","employeeType") as $attr){
            $smarty->assign($attr, "");
            if(preg_match("/r/", $this->ui->get_permissions($this->ui->dn,"users/user", $attr))
                && isset($this->attrs[$attr][0])){
                $smarty->assign($attr, $this->attrs[$attr][0]);
                $personalInfoAllowed = TRUE;
            }
        }

        session::set('binary',$this->attrs['jpegPhoto'][0]);
        session::set('binarytype',"image/jpeg");
        $smarty->assign("rand", rand(0, 99999999));
        $smarty->assign("personalInfoAllowed", $personalInfoAllowed);
        $smarty->assign("attrs", $this->attrs);
        return($smarty->fetch(get_template_path("infoPage.tpl")));
    }
}

?>
