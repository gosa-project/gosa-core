<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_roleManagement.inc 13520 2009-03-09 14:54:13Z hickert $$
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


class roleGeneric extends plugin {

    // The variables this plugin takes care of.
    var $cn = "";
    var $description ="";
    var $telephoneNumber = "";
    var $facsimileTelephoneNumber = "";

    // The objects base 
    var $base = "";

    // Keep track if possible ng aming modifications
    var $orig_dn = "";
    var $orig_cn = "";
    var $orig_base = "";

    // The object classes written by this plugin
    var $objectclasses = array("top","organizationalRole");

    // The list of occupants  ([dn])
    var $roleOccupant = array();

    // The roleOccupant cache, dn=>attrs
    var $roleOccCache = array();

    // A list of attributes managed by this plugin
    var $attributes = array("cn","description",
            "telephoneNumber","facsimileTelephoneNumber","roleOccupant");

    // 
    var $objCacheLoaded = FALSE;
    var $baseSelector;

    /* Initialize the class 
     */
    function __construct($config,$dn){
        plugin::__construct($config,$dn);
        $this->is_account = TRUE;

        // Initialize list of occupants
        $this->roleOccupant = array();
        if(isset($this->attrs['roleOccupant'])){
            for($i=0;$i<$this->attrs['roleOccupant']['count']; $i++){
                $this->roleOccupant[] = $this->attrs['roleOccupant'][$i];
            }
        }

        // Detect the objects base
        if ($this->dn == "new"){
            $ui = get_userinfo();
            $this->base= dn2base(session::global_is_set("CurrentMainBase")?"cn=dummy,".session::global_get("CurrentMainBase"):$ui->dn);
        } else {
            $this->base= preg_replace("/^[^,]+,".preg_quote(get_ou("roleGeneric", "roleRDN"), '/i')."/","",$this->dn);
        }

        // Keep track of naming attribute modifications
        $this->orig_base = $this->base;
        $this->orig_dn = $dn;
        $this->orig_cn = $this->cn;

        /* Instanciate base selector */
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        // Prepare lists
        $this->memberList = new sortableListing();
        $this->memberList->setDeleteable(true);
        $this->memberList->setInstantDelete(true);
        $this->memberList->setEditable(false);
        $this->memberList->setWidth("100%");
        $this->memberList->setHeight("300px");
        $this->memberList->setHeader(array('~',_("Given name"),_("Surname"),_("UID")));
        $this->memberList->setColspecs(array('20px','*','*','*','20px'));
        $this->memberList->setDefaultSortColumn(1);
    }


    /* Keep occupant cache up to date. 
     * Else, we may have entries we can't display.
     */
    function reload()
    {
        // Entries can't be added twice. 
        $attrs = array("description", "objectClass", "uid","cn",'sn','givenName');
        $this->roleOccupant = array_unique($this->roleOccupant);
        $this->roleOccupant = array_values($this->roleOccupant);

        $ldap = $this->config->get_ldap_link();
        foreach($this->roleOccupant as $dn){
            if(!isset($this->roleOccCache[$dn])){
                if($ldap->dn_exists($dn)){
                    $ldap->cat($dn, $attrs);

                    $tmp = $ldap->fetch();
                    if(!isset($tmp['cn'])){

                        // Extract the namingAttribute out of the dn.
                        $cn = preg_replace("/^[^=]*+=([^,]*).*$/","\\1",$tmp['dn']);
                        if(isset($tmp['uid'])){
                            $cn = $tmp['uid'][0];
                        }
                        if(isset($tmp['description'])){
                            $cn.= " [".$tmp['description'][0]."]";
                        }
                        $tmp['cn'][0] = $cn;
                    }

                    $this->roleOccCache[$dn] = $tmp;
                }
            }
        }

    }


    function getOccupants(){
        return($this->roleOccupant);
    }

    /* Generate HTML output of this plugin.
     */
    function execute()
    {
        plugin::execute();

        $theme = getThemeName();

        // Get list of possible ldap bases, will be selectable in the ui.
        $tmp = $this->allowedBasesToMoveTo();

        // Reload the occupant cache. 
        if(!$this->objCacheLoaded){
            $this->reload();
            $this->objCacheLoaded = TRUE;
        }

        /***************
         * Dialog handling
         ***************/

        if(isset($_POST['edit_membership']) && !$this->dialog instanceOf userSelect){
            $this->dialog = new userSelect($this->config,get_userinfo());
        }
        $this->memberList->save_object();
        $action = $this->memberList->getAction();
        if($action['action'] == 'delete'){
            $this->roleOccupant = $this->memberList->getMaintainedData();
        }

        if(isset($_POST['delete_membership']) && !$this->dialog instanceOf userSelect){
            if(isset($_POST['members'])){
                foreach($_POST['members'] as $id){
                    if(isset($this->roleOccupant[$id])){
                        unset($this->roleOccupant[$id]);
                    }
                }
                $this->reload();
            }
        }

        if(isset($_POST['add_users_cancel']) || isset($_POST['cancel-abort']) && $this->dialog instanceOf userSelect){
            $this->dialog = NULL;
        }
        if(isset($_POST['add_users_finish']) || isset($_POST['ok-save']) && $this->dialog instanceOf userSelect){
            $users = $this->dialog->detectPostActions();
            if(isset($users['targets'])){
                $headpage = $this->dialog->getHeadpage();
                foreach($users['targets'] as $dn){
                    $attrs = $headpage->getEntry($dn);
                    $this->roleOccupant[] = $dn;
                    $this->roleOccCache[$dn] = $attrs;
                }
            } 
            $this->dialog= FALSE;
        }

        if($this->dialog instanceOf userSelect){

            // Build up blocklist
            session::set('filterBlacklist', array('dn' => $this->roleOccupant));
            return($this->dialog->execute());
        }


        /***************
         * Template handling
         ***************/

        $this->memberList->setAcl($this->getacl("roleOccupant"));

        $data = $lData = array();
        foreach ($this->roleOccupant as $key => $dn){
            $data[$key] = $dn;
            if(isset($this->roleOccCache[$dn])){
                switch ($theme) {
                    case 'classic':
                        $icon = image('plugins/users/images/select_user.png');
                        break;
                    
                    default:
                        $icon = image("<i class='material-icons'>person</i>");
                        break;
                }
                $entry     = $this->roleOccCache[$dn];
                $sn        = $entry['sn']['0'];
                $givenName = $entry['givenName']['0'];
                $uid       = $entry['uid']['0'];
            }else{
                $sn = $givenName = _("Unknown");
                $uid = LDAP::fix($dn);
                $icon = image('images/false.png');
            }
            $lData[$key] = array('data' => array($icon,$givenName,$sn, $uid));
        }

        $this->memberList->setListData($data,$lData);
        $this->memberList->update();

        // Get smarty instance and assign required variables.
        $smarty = get_smarty();

        $smarty->assign("base", $this->baseSelector->render());
        $smarty->assign("memberList",$this->memberList->render());
        foreach($this->attributes as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }

        // Assign current permissions for each attribute. 
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $attr => $desc){
            $smarty->assign($attr."ACL",$this->getacl($attr));
        }
        return($smarty->fetch(get_template_path('roleGeneric.tpl',TRUE,dirname(__FILE__))));
    }


    /* Check user input and return a list of 'invalid input' messages.
     */
    function check()
    {
        $message = plugin::check();

        // Set the new acl base 
        if($this->dn == "new") {
            $this->set_acl_base($this->base);
        }

        // Check if we are allowed to create/move this user
        if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
            $message[]= msgPool::permCreate();
        }elseif($this->orig_dn != "new" && 
                !$this->acl_is_moveable($this->base) && 
                ($this->orig_base != $this->base || $this->orig_cn != $this->cn )){ 
            $message[]= msgPool::permMove();
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        /* must: cn */
        if ($this->cn == ""){
            $message[]= msgPool::required(_("Name"));
        }

        if (preg_match ('/[#+:=>\\\\\/]/', $this->cn)){
            $message[]= msgPool::invalid(_("Name"), $this->cn, "/[^#+:=>\\\\\/]/");
        }

        // Check if this name is uniq for roles.
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=organizationalRole)(cn=$this->cn))", array("cn"));
        $ldap->fetch();
        if ($ldap->count() != 0 && ( $this->dn == 'new' || $this->cn != $this->orig_cn)){
            $message[]= msgPool::duplicated(_("Name"));
        }

        return($message);
    }


    /* Removes the object from the ldap database
     */ 
    function remove_from_parent()
    {
        plugin::remove_from_parent();

        // Remove this object.
        $ldap= $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }

        // Log action.
        new log("remove","roles/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        // Trigger remove signal
        $this->handle_post_events("remove");
    }


    /* Saves object modifications
     */  
    function save()
    {

        // Ensure that we've added objects only once.
        $this->roleOccupant = array_unique($this->roleOccupant);
        $this->roleOccupant = array_values($this->roleOccupant);

        plugin::save();

        /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
           new entries. So do a check first... */
        $ldap = $this->config->get_ldap_link();
        $ldap->cat ($this->dn, array('dn'));
        if ($ldap->fetch()){
            $mode= "modify";
        } else {
            $mode= "add";
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }
        @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save via $mode");

        // Finally write data with selected 'mode'
        $this->cleanup();
        $ldap->cd ($this->dn);


        $ldap->$mode ($this->attrs);
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), 
                        $this->dn, LDAP_MOD, get_class()));
            return (1);
        }

        // Send modify/add events
        $this->handle_post_events($mode);

        // Update ACL dependencies too 
        if($this->dn != $this->orig_dn && $this->orig_dn != "new"){
            $tmp = new acl($this->config,$this->parent,$this->dn);
            $tmp->update_acl_membership($this->orig_dn,$this->dn);
        }

        // Log action
        if($mode == "modify"){
            new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }

        return 0;
    }


    /* This avoids that users move themselves out of their rights.
     */
    function allowedBasesToMoveTo()
    {
        $bases  = $this->get_allowed_bases();
        return($bases);
    }


    /* Save HTML inputs
     */
    function save_object()
    {
        plugin::save_object();  

        /* Refresh base */
        if ($this->acl_is_moveable($this->base)){
            if (!$this->baseSelector->update()) {
                msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
            }
            if ($this->base != $this->baseSelector->getBase()) {
                $this->base= $this->baseSelector->getBase();
                $this->is_modified= TRUE;
            }
        }
    }  


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        /* Load member objects */
        $this->roleOccupant = array();
        if (isset($source['roleOccupant'])){
            foreach ($source['roleOccupant'] as $key => $value){
                if ("$key" != "count"){
                    $value= @LDAP::convert($value);
                    $this->roleOccupant["$value"]= "$value";
                }
            }
        }
        $this->reload();
    }


    function getCopyDialog()
    {
        $smarty = get_smarty();
        $smarty->assign("cn", set_post($this->cn));
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));
        $ret = array();
        $ret['string'] = $str;
        $ret['status'] = "";
        return($ret);
    }

    function saveCopyDialog()
    {
        if(isset($_POST['cn'])){
            $this->cn = get_post('cn');
        }
    }


    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Generic"),
                    "plDescription" => _("Role generic"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("administration"),
                    "plRequirements"=> array(
                        'ldapSchema' => array('gosaRole' => '>=2.7'),
                        'onFailureDisablePlugin' => array(get_class(), 'roleManagement')
                        ),
                    "plCategory"    => array("roles" => array("description"  => _("Roles"),
                            "objectClass"  => "organizationalRole")),

                    "plProperties" => 
                    array(                        
                        array(
                            "name"          => "roleRDN",
                            "type"          => "rdn",
                            "default"       => "ou=roles,",
                            "description"   => _("RDN for role storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_roleRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)
                        ),

            "plProvidedAcls"=> array(
                    "cn"                => _("Name"),
                    "description" => _("Description"),
                    "base" => _("Base"),
                    "telephoneNumber" => _("Telephone number"),
                    "facsimileTelephoneNumber" => _("FAX number"),
                    "roleOccupant" => _("Occupants"))
                ));
    }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
