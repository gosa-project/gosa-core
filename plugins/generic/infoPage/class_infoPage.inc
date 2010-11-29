<?php

class infoPage extends plugin
{
    private $ui;
    private $managers;

    function __construct($config)
    {
        $this->config = &$config;
        $this->ui = get_userinfo();
        plugin::plugin($config, $this->ui->dn);


        // Detect managers for the current user.
        $this->managers = $this->detectManagers();

        // Get plugin list
        $this->plugins = $this->getPluginList();
    }

    function getPluginList()
    {
        $plist = session::get('plist');
        $myAccountID = array_search('MyAccount',$plist->pluginList);
        $str = "";
        foreach($this->config->data['TABS']['MYACCOUNTTABS'] as $pluginData){
            $plugin = $pluginData['CLASS'];
            $plInfo = call_user_func(array($plugin,'plInfo'));


            $str .= "<div style='width:20%;height:40px; background-color: grey; float: left'
                        onClick='openPlugin(\"{$myAccountID}\", \"{$plugin}\");'>";
            $str .= $plugin;
            $str .= "</div>";
        }
        return($str);
    }
    

    function detectManagers()
    {
        $dn = $this->dn;
        $max = 10;
        $dns = array();
        $dns[] = $dn;
        while(strlen($dn) >= strlen($this->config->current['BASE']) && $max){
            $dn = preg_replace("/^[^,]+,/","",$dn);
            $dns[] = $dn;
            $max --;
        }
        $ldap = $this->config->get_ldap_link(); 
        $ldap->cd($this->config->current['BASE']);
        $managers = array();
        foreach($dns as $dn){
            $ldap->cat($dn, array('manager'));
            $attrs = $ldap->fetch();
            if(isset($attrs['manager'])){
                $ldap->cat($attrs['manager'][0], array('sn','givenName','mail','telephoneNumber'));
                $managers[$dn] = $ldap->fetch();
                $name = $phone = $mail = "";
                $name = "<b>".set_post($managers[$dn]['sn'][0]).", ".set_post($managers[$dn]['givenName'][0])."</b>";
                if(isset($managers[$dn]['mail'][0])){
                    $mail = "<li>"._("Mail address").":&nbsp;".set_post($managers[$dn]['mail'][0])."</li>";
                }
                if(isset($managers[$dn]['telephoneNumber'][0])){
                    $phone = "<li>"._("Phone number").":&nbsp;".set_post($managers[$dn]['telephoneNumber'][0])."</li>";
                }
                $managers[$dn]['str'] = "<p>{$name}<ul>{$phone}{$mail}</ul></p>";
            }
        }
        return($managers);
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
                $smarty->assign($attr,set_post( $this->attrs[$attr][0]));
                $personalInfoAllowed = TRUE;
            }
        }

        session::set('binary',$this->attrs['jpegPhoto'][0]);
        session::set('binarytype',"image/jpeg");
        $smarty->assign("rand", rand(0, 99999999));
        $smarty->assign("personalInfoAllowed", $personalInfoAllowed);
        $smarty->assign("attrs", $this->attrs);
        $smarty->assign("managers", $this->managers);
        $smarty->assign("plugins", $this->plugins);
        $smarty->assign("jpegPhoto", isset($this->attrs['jpegPhoto']));
        $smarty->assign("managersCnt", count($this->managers));
        return($smarty->fetch(get_template_path("infoPage.tpl")));
    }
}

?>
