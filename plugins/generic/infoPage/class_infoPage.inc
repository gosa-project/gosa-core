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

            if(!$this->checkAccess($plugin)) continue;

            list($index, $title, $desc, $icon) = $plist->getPlugData($plugin);
            $str.= "\n        <div class='icon-menu-item' style='width: 20%;' onclick='openPlugin(\"{$myAccountID}\",\"{$plugin}\")'>";
            $str.= "\n          ".image($icon);
            $str.= "\n          <div class='dsc'>";
            $str.= "\n            <h1>{$title}</h1>";
            $str.= "\n            <p>{$desc}</p>";
            $str.= "\n          </div>";
            $str.= "\n        </div>";

        }
        return($str);
    }

    function checkAccess($class)
    {
        foreach($this->ui->ocMapping as $cat => $aclClasses){
            if(in_array($class, $aclClasses)){
                if(preg_match('/[rw]/',$this->ui->get_permissions($this->ui->dn, "{$cat}/{$class}", ''))){
                    return(TRUE);
                }
            }
        }
        return(FALSE);
    }
    

    function detectManagers()
    {

        // Check if we've an own manager set
        $ldap = $this->config->get_ldap_link(); 
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($this->dn, array('manager'));
        $attrs = $ldap->fetch();
        $dns = array();
        if(isset($attrs['manager'][0])){
            $dns['PERSONAL'] = $attrs['manager'][0];
        }

        // Get next department manager dn
        $dn = $this->dn;
        $max = 10;
        while(strlen($dn) >= strlen($this->config->current['BASE']) && $max){
            $dn = preg_replace("/^[^,]+,/","",$dn);
            $max --;
            $ldap->cat($dn, array('manager'));
            $attrs = $ldap->fetch();
            if(isset($attrs['manager'][0])){
                $dns['DEPARTMENT'] = $attrs['manager'][0];
                break;
            }
        }

        // Resolve collected manager dns
        $managers = array();
        foreach($dns as $type => $dn){
            $ldap->cat($dn,array('sn','givenName','mail','telephoneNumber'));
            $managers[$dn] = $ldap->fetch();
            $managers[$dn]['type'] = $type;
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

        // Set date of birth
        if(isset($this->attrs['dateOfBirth'][0])){
            $smarty->assign("dateOfBirth", date('d.m.Y',strtotime($this->attrs['dateOfBirth'][0])));
        }

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
