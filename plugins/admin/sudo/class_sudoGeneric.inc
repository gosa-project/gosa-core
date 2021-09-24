<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_sudo.inc 9975 2008-03-25 14:09:30Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*! \brief Sudo generic class. Allow setting User/Host/Command/Runas
  for a sudo role object.
 */
class sudo extends plugin
{

    protected $cn= "";
    protected $description= "";

    protected $sudoUser   = array("ALL");
    protected $sudoCommand= array();
    protected $sudoHost   = array("ALL");
    protected $sudoRunAs  = array("ALL");

    private $is_default = FALSE;

    public $objectclasses = array("top","sudoRole");
    public $attributes    = array("cn","description","sudoUser","sudoCommand","sudoHost","sudoRunAs");

    public $ignore_account = TRUE;

	
    public $orig_dn;
    private $systemEnabled = FALSE;

    /*! \brief  Returns to the base department for sudo roles.
      This department is then used to store new roles.
      @param  Object  GOsa configuration object.
      @return String sudo store department
     */
    public static function get_sudoers_ou($config)
    {
        return(get_ou("sudo", "sudoRDN").$config->current['BASE']);
    }

    /*! \brief  Initializes this sudo class, with all required attributes.
      @param  Object $config  GOsa configuration object.
      @param  String $db      "new" or the sudo role dn.
      @return .
     */
    function __construct(&$config, $dn= NULL)
    {
        plugin::__construct ($config, $dn);

	$this->systemEnabled = class_available("systemSelect");

        $this->trustModeDialog = new trustModeDialog($this->config, $this->dn,NULL);
        $this->trustModeDialog->setAcl('sudo/sudo');

        if($this->initially_was_account){
            foreach(array("sudoUser","sudoCommand","sudoHost","sudoRunAs") as $attr){
                $this->$attr = array();
                if(isset($this->attrs[$attr])){
                    $tmp = array();
                    for($i = 0 ; $i < $this->attrs[$attr]['count']; $i++){
                        $tmp[] = $this->attrs[$attr][$i];
                    }
                    $this->$attr = $tmp;
                }
            }
        }

        if(preg_match("/^defaults$/i",$this->cn)){
            $this->is_default = TRUE;
        }

        /* Get global filter config */
        if (!session::is_set("sysfilter")){
            $ui= get_userinfo();
            $base= get_base_from_people($ui->dn);
            $sysfilter= array( "depselect"       => $base,
                    "regex"           => "*");
            session::set("sysfilter", $sysfilter);
        }

        $this->orig_dn = $this->dn;

        // Build sortable lists 
        foreach(array('sudoUser','sudoCommand','sudoHost','sudoRunAs') as $l){
            $ll = $l."List";
            $this->$ll = new sortableListing($this->$l);
            $this->$ll->setDeleteable(false);
            $this->$ll->setEditable(false);
            $this->$ll->setWidth("100%");
            $this->$ll->setHeight("100px");
            $this->$ll->setAcl($this->getacl($l));
            $this->$ll->setDefaultSortColumn(1);
        }
        $this->sudoUserList->setHeader(array("!",_("Type"),_("Member"),_("Option")));
        $this->sudoUserList->setColspecs(array('24px','24px','*','46px'));
        $this->sudoUserList->setDefaultSortColumn(2);
        $this->sudoHostList->setHeader(array("!",_("System"),_("Option")));
        $this->sudoHostList->setColspecs(array('24px','*','46px'));
        $this->sudoCommandList->setHeader(array("!",_("Command"),_("Option")));
        $this->sudoCommandList->setColspecs(array('24px','*','46px'));
        $this->sudoRunAsList->setHeader(array("!",_("User"),_("Option")));
        $this->sudoRunAsList->setColspecs(array('24px','*','46px'));
    }


    /*! \brief  Creates the sudo generic ui. 
      @return String  The generated HTML content for this plugin. 
     */
    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        $theme = getThemeName();

        // Set list ACLs
        foreach(array('sudoUser','sudoCommand','sudoHost','sudoRunAs') as $l){
            $ll = $l."List";
            $this->$ll->setAcl($this->getacl($l));
        }

        // Handle trust mode dialog
        $trustModeDialog = $this->trustModeDialog->execute();
        if($this->trustModeDialog->trustSelect){
            $this->dialog = TRUE;
            return($trustModeDialog);

        }

        if(!is_object($this->dialog)){
            $this->dialog = FALSE;
        }

        /*********************
          Add users 
         *********************/ 

        if(isset($_POST['list_sudoUser']) && !is_object($this->dialog) && $this->acl_is_writeable("sudoUser")){
            $this->dialog =new userGroupSelect($this->config,get_userinfo());
        }

        /* Add selected hosts  to the sudoUser list */ 
        if(isset($_POST['userGroupSelect_save']) || isset($_POST['ok-save']) && $this->dialog instanceof userGroupSelect){
            if($this->acl_is_writeable("sudoUser")){
                foreach($this->dialog->save() as $entry){
                    if(in_array_strict("posixGroup",$entry['objectClass'])){
                        $name = trim("%".$entry['cn'][0]);
                    }elseif(isset($entry['uid'][0])){
                        $name = trim($entry['uid'][0]);
                    }
                    if(!in_array_strict($name,$this->sudoUser) && !in_array_strict("!".$name,$this->sudoUser)){
                        $this->sudoUser[] = $name;
                    }
                }   
            }
            unset($this->dialog);
            $this->dialog = NULL;
        }    

        if(isset($_POST['userGroupSelect_cancel']) || isset($_POST['cancel-abort']) && $this->dialog instanceOf userGroupSelect){
            unset($this->dialog);
            $this->dialog = NULL;
        }    

        if($this->dialog instanceOf userGroupSelect){
            $used = array();
            foreach($this->sudoUser as $name){
                $str = preg_replace("/^!/","",$name);
                if(preg_match("/^%/", $str)){
                    $used['cn'][] = preg_replace("/^%/","",$str);
                }else{
                    $used['uid'][] = $str;
                }
            }

            // Build up blocklist
            session::set('filterBlacklist', $used);
            return($this->dialog->execute());
        }



        /*********************
          Add systems 
         *********************/ 

        if(isset($_POST['list_sudoHost']) && !is_object($this->dialog) && $this->acl_is_writeable("sudoHost")){
            $this->dialog =new systemSelect($this->config,get_userinfo());
        }

        /* Add selected hosts  to the sudoHost list */ 
        if(isset($_POST['systemSelect_save']) || isset($_POST['ok-save']) && $this->dialog instanceof systemSelect){
            if($this->acl_is_writeable("sudoHost")){
                foreach($this->dialog->save() as $entry){
                    $cn = trim($entry['cn'][0]);
                    if(!in_array_strict($cn,$this->sudoHost) && !in_array_strict("!".$cn,$this->sudoHost)){
                        $this->sudoHost[] = $cn;
                    }
                }   
            }   
            unset($this->dialog);
            $this->dialog = NULL;
        }    

        if(isset($_POST['systemSelect_cancel']) || isset($_POST['cancel-abort']) && $this->dialog instanceOf systemSelect){
            unset($this->dialog);
            $this->dialog = NULL;
        }    

        if($this->dialog instanceOf systemSelect){
            $used = array();
            foreach($this->sudoHost as $name){
                $used['cn'][] = preg_replace("/^!/","",$name);
            }

            // Build up blocklist
            session::set('filterBlacklist', $used);
            return($this->dialog->execute());
        }

        /*********************
          Dialog handling / display / close  
         *********************/ 

        if(is_object($this->dialog)){
            return($this->dialog->execute());
        }


        /*********************
          NEGATE values 
         *********************/ 
        foreach($_POST as $name => $value){
            $value =get_post($name);
            if(preg_match("/^neg_/",$name)){
                $attr = preg_replace("/^neg_([^_]*)_.*$/","\\1",$name);
                $value= preg_replace("/^neg_[^_]*_([0-9]*)$/","\\1",$name);

                if($this->acl_is_writeable($attr)){
                    $attrs = $this->$attr;
                    if(isset( $attrs[$value])){
                        $v =  $attrs[$value];
                        if(preg_match("/^!/",$v)){
                            $attrs[$value] = preg_replace("/^!/","",$v);
                        }else{
                            $attrs[$value] = "!".$v;
                        }
                        $this->$attr = $attrs;  
                    }
                }
                break; // Do it once, image inputs will be posted twice
            }
        }

        /*********************
          Delete values 
         *********************/ 
        foreach($_POST as $name => $value){
            $value =get_post($name);
            if(preg_match("/^delS_/",$name)){
                $attr = preg_replace("/^delS_([^_]*).*$/","\\1",$name);
                $value= preg_replace("/^delS_[^_]*_([0-9]*)$/","\\1",$name);
                if($this->acl_is_writeable($attr)){
                    $attrs = $this->$attr;
                    if(isset( $attrs[$value])){
                        unset($attrs[$value]);
                        $this->$attr = $attrs;  
                    }
                }
                break; // Do it once, image inputs will be posted twice
            }
        }


        /*********************
          ADD values 
         *********************/

        /* Acceptable characters for various fields */
        $ipv4_regex = "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$";
        $fqdn_regex = "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$";
        $attr_regex = array(
            "sudoUser" => "/^[a-z0-9{$c}]*$/i",
            "sudoHost" => "/$ipv4_regex|$fqdn_regex/i",
            "sudoRunAs" => "/^[a-z0-9{$c}]*$/i"
        );

        /* User / Host / Runas */ 
        foreach(array("sudoUser","sudoHost","sudoRunAs") as $attr){
            if($this->acl_is_writeable($attr) && 
                    isset($_POST["add_".$attr]) && 
                    isset($_POST['new_'.$attr]) && 
                    !empty($_POST['new_'.$attr])){

                $c = preg_quote(' *+-?_|!\'"()','/');
                if(preg_match($attr_regex[$attr],get_post('new_'.$attr))){
                    $attrs = $this->$attr;
                    $attrs[] =  trim(get_post('new_'.$attr)); 
                    $this->$attr = $attrs;
                }else{
                    msg_dialog::display(_("Error"),msgPool::invalid($attr,get_post('new_'.$attr),$attr_regex[$attr]));
                }
            }
        }

        /* Command */
        foreach(array("sudoCommand") as $attr){
            if($this->acl_is_writeable($attr) && isset($_POST["add_".$attr]) && isset($_POST['new_'.$attr])){
                $attrs = $this->$attr;
                $attrs[] =  trim(get_post('new_'.$attr)); 
                $this->$attr = $attrs;
            }
        }


        /*********************
          SMARTY assignments 
         *********************/

        $smarty = get_smarty();
        $smarty->assign("systemEnabled", $this->systemEnabled);
        $smarty->assign("trustModeDialog" , $trustModeDialog);
        $smarty->assign("is_default",$this->is_default);
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
            $smarty->assign($attr."ACL",$this->getacl($attr));
        }

        /* Fill listings
         */

        switch ($theme) {
            case 'classic':
                $neg_img= image('plugins/sudo/images/negate.png','','!');
                $option = image('plugins/sudo/images/negate.png','neg_%ATTR%_%KEY%');
                $option.= image('images/lists/trash.png', 'delS_%ATTR%_%KEY%');
                break;
            
            default:
                $neg_img= image("<i class='material-icons'>not_interested</i>",'','!');
                $option = image("<i class='material-icons input-icons'>not_interested</i>",'neg_%ATTR%_%KEY%');
                $option.= image("<i class='material-icons input-icons'>delete</i>", 'delS_%ATTR%_%KEY%');
                break;
        }
        
        foreach(array('sudoUser','sudoCommand','sudoHost','sudoRunAs') as $l){
            $l.="Data";
            $$l = array();
        }

        foreach(array("sudoCommand","sudoHost","sudoRunAs") as $attr){
            $tmp =array();
            $list = $attr."List";
            $data = $attr."Data";
            foreach($this->$attr as $id => $entry){
                $neg = "";
                if(preg_match("/^!/",$entry)){
                    $neg = $neg_img;
                }
                $entry = preg_replace("/^!/","",$entry);

                $action =preg_replace(array("/%KEY%/","/%ATTR%/"),array($id,$attr),$option);
                $tmp[$id] = array('data'=>array($neg,$entry,$action)) ;
            }
            $this->$list->setListData($this->$attr, $tmp);
            $this->$list->update();
            $smarty->assign("listing_{$attr}", $this->$list->render());
        }

        switch ($theme) {
            case 'classic':
                $img1 = image('plugins/users/images/select_user.png','',_("User"));
                $img2 = image('plugins/groups/images/select_group.png','',_("Group"));
                break;
            
            default:
                $img1 = image("<i class='material-icons'>person</i>",'',_("User"));
                $img2 = image("<i class='material-icons'>group</i>",'',_("Group"));
                break;
        }
        
        $sudoUserData  = array();
        foreach($this->sudoUser as $id => $entry){
            $neg = "";
            if(preg_match("/^!/",$entry)){
                $neg = $neg_img;
            }
            $entry = preg_replace("/^!/","",$entry);

            $img = $img1;
            if(preg_match("/^%/",$entry)){
                $img = $img2;
            }
            $entry = preg_replace("/^%/","",$entry);
            $action =preg_replace(array("/%KEY%/","/%ATTR%/"),array($id,'sudoUser'),$option);
            $sudoUserData[$id] = array('data'=>array($neg,$img,$entry,$action)) ;
        }
        $this->sudoUserList->setListData($this->sudoUser,$sudoUserData);
        $this->sudoUserList->update();
        $smarty->assign("listing_sudoUser", $this->sudoUserList->render());
        return($smarty->fetch(get_template_path('generic.tpl', TRUE)));
    }


    /*! \brief  Remove this sudo role from the ldap server 
     */
    function remove_from_parent()
    {
        plugin::remove_from_parent();

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->rmdir($this->dn);

        /* Send signal to the world that we've done */
        $this->handle_post_events("remove");
    }


    /*! \brief  Save all relevant HTML posts. 
     */
    function save_object()
    {
        plugin::save_object();
        $this->trustModeDialog->save_object();

        if($this->is_default){
            $this->cn = "defaults";
        }  
    }


    function set_acl_base($base)
    {
        plugin::set_acl_base($base); 
        $this->trustModeDialog->set_acl_base($base);
    }


    /*! \brief  Save changes into the ldap database.
     */
    function save()
    {
        plugin::save();

        /* Ensure a correct array index 
         */ 
        $this->attrs['sudoHost']    = array_values($this->attrs['sudoHost']);
        $this->attrs['sudoRunAs']   = array_values($this->attrs['sudoRunAs']);
        $this->attrs['sudoUser']    = array_values($this->attrs['sudoUser']);
        $this->attrs['sudoCommand'] = array_values($this->attrs['sudoCommand']);

        $this->cleanup();

        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);

        if($this->is_new){
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
            $ldap->cd($this->dn);
            $ldap->add($this->attrs);

            /* Send signal to the world that we've done */
            $this->handle_post_events("add");
        }else{
            $ldap->cd($this->dn);
            $ldap->modify($this->attrs);;

            /* Send signal to the world that we've done */
            $this->handle_post_events("modify");
        }
        $this->trustModeDialog->dn = $this->dn;
        $this->trustModeDialog->save();

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }
    }


    /*! \brief  Check the given input.
      @return Array   All error messages in an array();
     */
    function check()
    {
        $message = plugin::check();

        /* Is a name given? */
        if($this->cn == ""){
            $message[] = msgPool::required(_("Name"));
        }

        /* Check if name is reserved */
        if(!$this->is_default && preg_match("/^defaults$/i",$this->cn)){
            $message[] = msgPool::reserved(_("Name"));
        }

        /* Check name */
        if(!preg_match("/^[0-9a-z\@]*$/i",$this->cn)){
            $message[] = msgPool::invalid(_("Name"),$this->cn,"/[0-9a-z\@]/i");
        }

        /* Check if this entry will cause duplicated ldap entries */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->get_sudoers_ou($this->config));
        $ldap->search("(&(objectClass=sudoRole)(cn=".$this->cn."))");
        while($attrs = $ldap->fetch()){
            if($attrs['dn'] != $this->dn){
                $message[] = msgPool::duplicated(_("Name"));
            }
        }

        /* Check if we are allowed to create or move this object
         */
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->get_sudoers_ou($this->config))){
            $message[] = msgPool::permCreate();
        }

        return ($message);
    }


    /*! \brief Force this entry to be handled and saved as 'default'
      @param  BOOL  TRUE -force defaults   FALSE -normal
     */
    public function set_default($state)
    {
        $this->is_default = TRUE;
        $this->cn = "defaults";
    }


    /*! \brief  Add ACL object
      @return Returns the ACL object.
     */
    static function plInfo()
    {
        return (array(  
                    "plShortName" => _("Sudo"),
                    "plDescription" => _("Sudo role"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("sudo" => array("objectClass" => "sudoRole", "description" => _("Sudo role"))),

                    "plProperties" => 
                    array(
                        array(
                            "name"          => "sudoRDN",
                            "type"          => "rdn",
                            "default"       => "ou=sudoers,",
                            "description"   => _("RDN for sudo rule storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_sudoRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)),


            "plProvidedAcls"    => array(
                    "accessTo"          => _("System trust"),
                    "cn"                => _("Name"),
                    "description"       => _("Description"),
                    "sudoUser"          => _("Users"),
                    "sudoHost"          => _("Host"),
                    "sudoCommand"       => _("Command"),
                    "sudoRunAs"         => _("Run as user"),
                    "trustModel"        => _("Access control list"))
                ));
    }


    /*! \brief  This function will be called if an object gets copied.
      This function adapts attributes from the source object.
      @param  Array The source object.
     */
    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $this->trustModeDialog->PrepareForCopyPaste($source);

        foreach(array("sudoUser","sudoCommand","sudoHost","sudoRunAs") as $attr){
            $this->$attr = array();
            if(isset($source[$attr])){
                $tmp = array();
                for($i = 0 ; $i < $source[$attr]['count']; $i++){
                    $tmp[] = $source[$attr][$i];
                }
                $this->$attr = $tmp;
            }
        }
    }


    /*! \brief  Used for copy & paste.
      Returns a HTML input mask, which allows to change the cn of this entry.
      @param  Array   Array containing current status && a HTML template.              
     */
    function getCopyDialog()
    {
        $vars = array("cn");
        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }


    public function get_cn()
    {
        return($this->cn);
    }


    /*! \brief  Used for copy & paste.
      Some entries must be renamed to avaoid duplicate entries.
     */
    function saveCopyDialog()
    {
        if(isset($_POST['cn'])){
            $this->cn = get_post('cn');
        }
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
