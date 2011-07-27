<?php

class infoPage extends plugin
{
    private $ui;
    private $managers;


    /*! \brief
     */
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


    /*! \brief  Returns a HTML string which respresent a list of plugins the user can access.
     *          Only accessible plugins will be rendered.
     *  @return String  HTML content
     */
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


    /*! \brief  Check if we've access to a given class/plugin of GOsa. 
     *  @param  String  The class name to check for.
     *  @return Boolean     True on success else FALSE.
     */
    function checkAccess($class)
    {
        foreach($this->ui->ocMapping as $cat => $aclClasses){
            if(in_array_strict($class, $aclClasses)){
                if(preg_match('/[rw]/',$this->ui->get_permissions($this->ui->dn, "{$cat}/{$class}", ''))){
                    return(TRUE);
                }
            }
        }
        return(FALSE);
    }


    /*! \brief  Prepares an array which contains info about the users
     *           managers. The personal manager and the next
     *           manager defined for a department.
     *  @return Array   An array with resolved manager dns.
     */
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
        $max = 4;
        while(strlen($dn) >= strlen($this->config->current['BASE']) && $max--){
            $dn = preg_replace("/^[^,]+,/","",$dn);
            $ldap->cat($dn, array('manager'));
            $attrs = $ldap->fetch();
            if(isset($attrs['manager'][0])){
                $dns['DEPARTMENT'] = $attrs['manager'][0];
            }
        }

        // Resolve collected manager dns
        $managers = array();
        foreach($dns as $type => $dn){
            $ldap->cat($dn,array('sn','givenName','mail','telephoneNumber'));
            $managers[$dn] = $ldap->fetch();
            $managers[$dn]['type'] = $type;
            $name = $phone = $mail = "";
            $name = "<tr><td colspan='2' style='background-color:#F0F0F0'><b>".set_post($managers[$dn]['givenName'][0])." ".set_post($managers[$dn]['sn'][0])."</b></td></tr>";
            if(isset($managers[$dn]['telephoneNumber'][0])){
                $phone = "<tr><td>&nbsp;"._("Phone number").":</td><td><i>".set_post($managers[$dn]['telephoneNumber'][0])."</i></td></tr>";
            }
            if(isset($managers[$dn]['mail'][0])){
                $mail = "<tr><td>&nbsp;"._("Mail").":</td><td><i><a href='mailto:".set_post($managers[$dn]['mail'][0])."'>".set_post($managers[$dn]['mail'][0])."</i></td></tr>";
            }
            $managers[$dn]['str'] = "<table cellpadding='2' style='border:1px solid #CCC'>{$name}{$phone}{$mail}</table>";
        }
        return($managers);
    }


    /*! \brief  Renders the plugin UI in HTML.
     *  @return String  HTML content of the plugin.
     */
    function execute()
    {
        $smarty = get_smarty();
        $personalInfoAllowed = FALSE;
        foreach(array("uid","sn","givenName","mail","street","l","o","ou","jpegPhoto","personalTitle",
                    "academicTitle","dateOfBirth","homePostalAddress","homePhone","departmentNumber",
                    "employeeNumber","employeeType") as $attr){
            $smarty->assign($attr, "");
            if(preg_match("/r/", $this->ui->get_permissions($this->ui->dn,"users/user", $attr))
                    && isset($this->attrs[$attr][0])){
                $smarty->assign($attr, set_post($this->attrs[$attr][0]));
                $personalInfoAllowed = TRUE;
            }
        }

        // Convert address
        if(preg_match("/r/", $this->ui->get_permissions($this->ui->dn,"users/user", "homePostalAddress")) && 
                isset($this->attrs['homePostalAddress'][0])){
            $smarty->assign("homePostalAddress", preg_replace("/\n/", "<br>", $this->attrs['homePostalAddress'][0]));
        }else{
            $smarty->assign("homePostalAddress", "");
        }

        // Assign JPEG Photo only if it is set and if we are allowed to view it.
        $smarty->assign("jpegPhoto", "");
        if(preg_match("/r/", $this->ui->get_permissions($this->ui->dn,"users/user", "userPicture")) && 
                isset($this->attrs['jpegPhoto'][0])){
            session::set('binary',$this->attrs['jpegPhoto'][0]);
            session::set('binarytype',"image/jpeg");
            $smarty->assign("jpegPhoto", $this->attrs['jpegPhoto']);
        }

        // Set date of birth
        if(preg_match("/r/", $this->ui->get_permissions($this->ui->dn,"users/user", "dateOfBirth")) && 
                isset($this->attrs['dateOfBirth'][0])){
            $smarty->assign("dateOfBirth", date('d.m.Y',strtotime($this->attrs['dateOfBirth'][0])));
        }else{
            $smarty->assign("dateOfBirth","");
        }

        $smarty->assign("rand", rand(0, 99999999));
        $smarty->assign("personalInfoAllowed", $personalInfoAllowed);
        $smarty->assign("managers", $this->managers);
        $smarty->assign("plugins", $this->plugins);
        $smarty->assign("managersCnt", count($this->managers));
        $smarty->assign("revision", get_gosa_version());
        $smarty->assign("year", date("Y"));
        return($smarty->fetch(get_template_path("infoPage.tpl")));
    }
}

?>
