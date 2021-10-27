<?php
/**
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_group.inc 20952 2011-07-27 06:38:29Z hickert $$
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

class group extends plugin
{
    // Group attributes
    var $cn= "";
    var $gidNumber= "";
    var $members_used_by_some= array();

    // Helpers
    var $base= "";
    var $force_gid= FALSE;
    var $fon_group= FALSE;
    var $smbgroup= FALSE;
    var $groupType= FALSE;
    var $sambaSID= "";
    var $sambaDomainName= "DEFAULT";
    var $SID= "";
    var $ridBase= 0;
    var $members= array();
    var $users= array();
    var $saved_gidNumber= "";
    var $oldgroupType= "";
    var $orig_dn= "";
    var $orig_cn= "";
    var $orig_base= "";
    var $has_mailAccount= FALSE;
    var $userSelect= FALSE;
    var $nagios_group =FALSE;
    var $sambaGroupType;
    var $dialog;
    var $rfc2307bis= FALSE;
    var $OnlyShowFirstEntries =200;
    var $view_logged = FALSE;
    var $allowGroupsWithSameNameInOtherSubtrees = true;
    var $baseSelector;
    var $dns= array();

    // attribute list for save action
    var $attributes= array("cn", "description", "gidNumber","sambaGroupType","sambaSID","member");
    var $objectclasses= array("top");
    var $CopyPasteVars  = array("force_gid","fon_group","smbgroup","groupType","sambaSID","sambaDomainName","SID","nagios_group","sambaGroupType");
    var $multiple_support = TRUE;

    // Lists 
    private $memberList;    // Single group editing
    private $commonList;    // (Multiple support active)
    private $partialList;   // (Multiple support active)

    function __construct(&$config, $dn= NULL)
    {
        // Set rfc2307bis flag
        if ($config->get_cfg_value("core","rfc2307bis") == "true"){
            $this->rfc2307bis= TRUE;
            $this->attributes[]= "member";
            $this->objectclasses[]= "groupOfNames";
        }

        plugin::__construct ($config, $dn);

        $this->trustModeDialog = new trustModeDialog($this->config, $this->dn,NULL);
        $this->trustModeDialog->setAcl('groups/group');

        // Load attributes depending on the samba version
        $this->orig_dn= $dn;
        $this->orig_cn= $this->cn;

        // get Member list
        $this->members =$this->reload($this->attrs);

        // Save gidNumber for later use
        if (isset($this->attrs['gidNumber'])){
            $this->saved_gidNumber= $this->attrs['gidNumber'][0];
        }

        // Is a samba group?
        if (isset($this->attrs['objectClass'])){
            if (array_search ('sambaGroupMapping', $this->attrs['objectClass']) == FALSE ){
                $this->smbgroup= FALSE;
            } else {
                $this->smbgroup= TRUE;
                if (isset($this->attrs['sambaSID'])){
                    $this->sambaSID= $this->attrs['sambaSID'][0];
                }
            }
            if (array_search ('goFonPickupGroup', $this->attrs['objectClass']) == FALSE ){
                $this->fon_group= FALSE;
            } else {
                $this->fon_group= TRUE;
            }
            if (array_search ('nagiosContactGroup', $this->attrs['objectClass']) == FALSE ){
                $this->nagios_group= FALSE;
            } else {
                $this->nagios_group= TRUE;
            }
        }

        // Set mail flag
        if (isset($this->attrs['objectClass']) && in_array_strict('gosaMailAccount', $this->attrs['objectClass'])){
            $this->has_mailAccount= TRUE;
        }

        // Get samba Domain in case of samba 3
        if ($this->sambaSID != ""){
            $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase"));
            if ($ldap->count() != 0){
                $attrs= $ldap->fetch();
                if(isset($attrs['sambaAlgorithmicRidBase'])){  
                    $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];  
                } else {  
                    $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                } 

                // Get domain name for SID
                $this->sambaDomainName= "DEFAULT";
                foreach ($this->config->data['SERVERS']['SAMBA'] as $key => $val){
                    if ($val['SID'] == $this->SID){
                        $this->sambaDomainName= $key;
                        break;
                    }
                }
            } else {
                if ($this->config->get_cfg_value("core","sambaRidBase") != ""){
                    $this->sambaDomainName= "DEFAULT";
                    $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                    $this->SID= $this->config->get_cfg_value("core","sambaSID");
                } else {
                    msg_dialog::display(_("Configuration error"), _("Cannot find group SID in your configuration!"), ERROR_DIALOG);
                }
            }

            // Get group type
            $this->groupType= (int)substr(strrchr($this->sambaSID, "-"), 1);
            if ($this->groupType < 500 || $this->groupType > 553){
                $this->groupType= 0;
            }
            $this->oldgroupType= $this->groupType;
        }

        if ($this->dn == "new"){
            if(session::is_set('CurrentMainBase')){
                $this->base = session::get('CurrentMainBase');
            }else{
                $ui= get_userinfo();
                $this->base= dn2base($ui->dn);
            }
        } else {

            // Get object base 
            $this->base =preg_replace ("/^[^,]+,".preg_quote(get_groups_ou(), '/')."/i","",$this->dn);
        }
        $this->orig_base = $this->base;

        // This is always an account 
        $this->is_account= TRUE;

        // Instanciate base selector 
        $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
        $this->baseSelector->setSubmitButton(false);
        $this->baseSelector->setHeight(300);
        $this->baseSelector->update(true);

        // Prepare lists
        $this->memberList = new sortableListing();
        $this->memberList->setDeleteable(true);
        $this->memberList->setInstantDelete(false);
        $this->memberList->setEditable(false);
        $this->memberList->setWidth("100%");
        $this->memberList->setHeight("300px");
        $this->memberList->setHeader(array('~',_("Common Name"),_("UID")));
        $this->memberList->setColspecs(array('20px','*','*','20px'));
        $this->memberList->setDefaultSortColumn(1);

        // Set the dns of selected Group(s)
        if (!$this->dns[0]) {
            $this->dns[] = $this->dn;
        }
    }

    function execute()
    {
        // Call parent execute 
        plugin::execute();

        $theme = getThemeName();

        // Log view 
        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","groups/".get_class($this),$this->dn);
        }

        // Do we represent a valid group? 
        if (!$this->is_account && $this->parent === NULL){
            switch ($theme) {
                case 'classic':
                  $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
                  break;
                
                default:
                  $symbol = "<i class='material-icons'>report</i>";
                  break;
              }
              $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_())."</div>";
            return ($display);
        }

        // Act on list events 
        foreach(array('memberList','commonList','partialList') as $list){

            // Check if list is available, depends on multi- or sinlge- group editing. 
            if($this->$list){
                // ensure we can delete group members if we have
                // write permission to the member attribute
                $mu_acl = $this->getacl("member");
                if (preg_match("/w/",$mu_acl) && !preg_match("/d/",$mu_acl)) {
                    $this->$list->setAcl($mu_acl.'d');
                }
                $this->$list->save_object();
                $action = $this->$list->getAction();
                if($action['action'] == 'delete' && preg_match("/w/",$this->getacl("member"))){
                    foreach ($action['targets'] as $id){
                        $value = $this->$list->getData($id);
                        $this->removeMember($value['dn']);
                    }
                }
            }
        }

        // Add objects? 
        if (isset($_POST["edit_membership"]) && preg_match("/w/",$this->getacl("member"))){
            $this->userSelect= new userSelect($this->config, get_userinfo(), $this->dns);
        }

        // Add objects finished? 
        if (isset($_POST["add_users_cancel"]) || isset($_POST['cancel-abort'])){
            $this->userSelect= NULL;
        }

        // Add user to group 
        if (isset($_POST['add_users_finish']) || isset($_POST['ok-save']) && $this->userSelect){

            $users = $this->userSelect->detectPostActions();
            if(isset($users['targets'])){
                $headpage = $this->userSelect->getHeadpage();
                foreach($users['targets'] as $dn){
                    $attrs = $headpage->getEntry($dn);
                    $this->addMember($attrs['dn']);                        
                }
            }
            $this->userSelect= NULL;
        }

        $smarty= get_smarty();

        // Handle trust mode dialog
        $this->dialog = FALSE;
        $trustModeDialog = $this->trustModeDialog->execute();
        if($this->trustModeDialog->trustSelect){
            $this->dialog = TRUE;
            return($trustModeDialog);
        }
        $smarty->assign("trustModeDialog" , $trustModeDialog);
        $smarty->assign("nagios", $this->config->pluginEnabled("nagiosAccount") && class_available("nagiosAccount"));
        $smarty->assign("pickupGroup", $this->config->pluginEnabled("phoneAccount") && class_available("phoneAccount"));

        // Manage object add dialog 
        if ($this->userSelect){
            $this->dialog = TRUE;
            return($this->userSelect->execute());
        }

        // Create base acls 
        $smarty->assign("base", $this->baseSelector->render());

        $domains= array();
        foreach($this->config->data['SERVERS']['SAMBA'] as $name => $content){
            $domains[$name]= $name;
        }
        $smarty->assign("sambaDomains",     set_post($domains));
        $smarty->assign("sambaDomainName",  set_post($this->sambaDomainName));
        $groupTypes= array(0 => _("Samba group"), 512 => _("Domain administrators"), 513 => _("Domain users"),
                514 => _("Domain guests"));

        // Don't loose special groups! If not key'ed above, add it to the combo box... 	
        if ($this->groupType >= 500 && $this->groupType <= 553 && !isset($groupTypes[$this->groupType])){
            $groupTypes[$this->groupType]= sprintf(_("Special group (%d)"), $this->groupType);
        }

        $smarty->assign("groupTypes", set_post($groupTypes));
        $smarty->assign("groupType",  set_post($this->groupType));

        // Members and users 
        if(!$this->multiple_support_active){
            $this->memberList->setAcl($this->getacl("member"));
            $data = $lData = array();
            foreach($this->members as $member => $values){
                if (!in_array($values['dn'], $this->dns)) {                
                    $data[$member] = $values;
                    $cn = _("Unknown");
                    if (isset($values['cn']) && isset($values['cn'][0])) $cn = $values['cn'][0];
                    $image = image('images/false.png');
                    if(isset($values['sn'])){
                        switch ($theme) {
                            case 'classic':
                                $image = image("plugins/users/images/select_user.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>person</i>");
                                break;
                        }
                    }elseif(isset($values['gidNumber'])){
                        switch ($variable) {
                            case 'classic':
                                $image = image("plugins/groups/images/select_group.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>groups</i>");
                                break;
                        }
                    }
                    $lData[$member] = array('data' => array($image,$cn, $data[$member]['uid'][0] ?? ""));
                }
            }
            $this->memberList->setListData($data,$lData);
            $bool = $this->isRestrictedByDynGroup();
            $this->memberList->setDeleteable(!$bool);
            $this->memberList->update();
            $smarty->assign("memberList", $this->memberList->render());
        }else{

            $this->commonList->setAcl($this->getacl("member"));
            $this->partialList->setAcl($this->getacl("member"));

            $data = $lData = array();
            foreach($this->members as $member => $values){
                if (!in_array($values['dn'], $this->dns)) {
                    $data[$member] = $values;
                    $sn = _("Unknown");
                    if(isset($values['cn'][0])) $cn = $values['cn'][0];
                    $image = image('images/false.png');
                    if(isset($values['sn'])){
                        switch ($theme) {
                            case 'classic':
                                $image = image("plugins/users/images/select_user.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>person</i>");
                                break;
                        }
                    }elseif(isset($values['gidNumber'])){
                        switch ($theme) {
                            case 'classic':
                                $image = image("plugins/groups/images/select_group.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>groups</i>");
                                break;
                        }
                    }
                    $lData[$member] = array('data' => array($image,$cn,$data[$member]['uid'][0] ?? ""));
                }
            }
            $this->commonList->setListData($data,$lData);
            $this->commonList->update();
            $smarty->assign("commonList", $this->commonList->render());

            $data = $lData = array();
            foreach($this->members_used_by_some as $member => $values){
                if (!in_array($values['dn'], $this->dns) && $member !== 'count') {
                    $data[$member] = $values;
                    $sn = _("Unknown");
                    if(isset($values['cn']) && isset($values['cn'][0])) $cn = $values['cn'][0];
                    $image = image('images/false.png');
                    if(isset($values['sn'])){
                        switch ($theme) {
                            case 'classic':
                                $image = image("plugins/users/images/select_user.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>person</i>");
                                break;
                        }
                        
                    }elseif(isset($values['gidNumber'])){
                        switch ($theme) {
                            case 'classic':
                                $image = image("plugins/groups/images/select_group.png");
                                break;
                            
                            default:
                                $image = image("<i class='material-icons'>groups</i>");
                                break;
                        }
                    }
                    $lData[$member] = array('data' => array($image,$cn,$data[$member]['uid'][0] ?? ""));
                }
            }
            $this->partialList->setListData($data,$lData);
            $this->partialList->update();
            $smarty->assign("partialList", $this->partialList->render());
        }

        // Checkboxes 
        foreach (array("force_gid", "smbgroup") as $val){
            if ($this->$val == "1"){
                $smarty->assign("$val", "checked");
            } else {
                $smarty->assign("$val", "");
            }
        }
        if ($this->force_gid != "1"){
            $smarty->assign("forceMode", "disabled");
        }else{
            $smarty->assign("forceMode", "");
        }
        if ($this->fon_group){
            $smarty->assign("fon_group", "checked");
        } else {
            $smarty->assign("fon_group", "");
        }

        if ($this->nagios_group){
            $smarty->assign("nagios_group", "checked");
        } else {
            $smarty->assign("nagios_group", "");
        }

        // Fields 
        foreach (array("cn", "description", "gidNumber") as $val){
            $smarty->assign("$val", set_post($this->$val));
        }

        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation){
            $smarty->assign($name."ACL",$this->getacl($name));
        }

        if($this->acl_is_writeable("base")){
            $smarty->assign("baseSelect",true);
        }else{
            $smarty->assign("baseSelect",false);
        }

        // Multiple edit handling 
        $smarty->assign("multiple_support",$this->multiple_support_active);

        foreach($this->attributes as $val){
            if(in_array_strict($val,$this->multi_boxes)){
                $smarty->assign("use_".$val,TRUE);
            }else{
                $smarty->assign("use_".$val,FALSE);
            }
        }
        foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $val){
            if(in_array_strict($val,$this->multi_boxes)){
                $smarty->assign("use_".$val,TRUE);
            }else{
                $smarty->assign("use_".$val,FALSE);
            }
        }

        $bool = $this->isRestrictedByDynGroup();
        $smarty->assign("restrictedByDynGroup", $bool);
        if($bool){
            $smarty->assign("memberACL", preg_replace("/[^r]/","",$this->getacl('member')));
             
        }
        return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
    }

    function isRestrictedByDynGroup()
    {
        $bool = FALSE;
        if(isset($this->parent->by_object['DynamicLdapGroup'])){
            $bool = $this->parent->by_object['DynamicLdapGroup']->isAttributeDynamic('member') ||
                $this->parent->by_object['DynamicLdapGroup']->isAttributeDynamic('member');
        }
        return($bool);
    }

    // add new Member to a Group(s)
    function addMember($dn)
    {
        /** 
         * In mutliple edit we have to handle two arrays.
         * members               : Containing users used in all groups
         * members_used_by_some  : Those which are not used in all groups
         * So we have to remove the given $dn from the ..used_by_some array first.
        */
        if($this->multiple_support_active){
            if(isset($this->members_used_by_some[$dn])){
                unset($this->members_used_by_some[$dn]);
            }
        }  

        // Ensure that the requested object is known to the group class 
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($dn,array("dn","cn","uid","sn","givenName","gidNumber"));
        if($ldap->count() == 0 ){
            msg_dialog::display(_("Error"), 
                sprintf(_("Adding DN '%s' to group '%s' failed: cannot find user object!"), 
                    $dn,$this->cn), 
                ERROR_DIALOG);
            return;
        }elseif($ldap->count() >= 2){
            msg_dialog::display(_("Error"), 
                sprintf(_("Add DN '%s' to group '%s' failed: DN is used more than once!"),
                    $dn,$this->cn), 
                ERROR_DIALOG);
            return;
        }else{
            while($attrs = $ldap->fetch()){
                $this->members[$attrs['cn'][0]] = $attrs;
            }
        }
    }

    // remove the member using the array id
    function removeMember($dn)
    {
        foreach ($this->members as $key => $member) {
            if ($dn == $member['dn']) {
                unset($this->members[$key]);
            }
        }

        /** 
         * We have two array contianing group members in multiple edit.
         * members             : Groups used by all currently edited groups 
         * members_used_by_some: Used by some 
         * So we have to remove the specified member from both arrays.
        */ 
        if($this->multiple_support_active){
            foreach ($this->members_used_by_some as $key => $member) {
                if ($dn == $member) {
                    unset($this->members_used_by_some[$key]);
                }
            }
        }
    }

    // reload and receive data from the LDAP
    function reload(array $attrs)
    {
        unset($attrs['member']['count']);
        $arr= array();
        if (isset($attrs['member'][0])){
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            foreach ($attrs['member'] as $member => $dn) {
                $ldap->cat($dn,array("dn","cn","uid","sn","givenName","gidNumber"));
                while ($attr = $ldap->fetch()) {
                    $arr[$attr['cn'][0]]= $attr;
                }
            }
            ksort ($arr);
        }

        // check if all members are resolved 
        foreach ($arr as $value){
            if(!isset($arr[$value])){
                $arr[$value] = "";
            }
        }
        return $arr;
    }


    // Create display name, this was used so often that it is excluded into a seperate function 
    function createResultName($attrs)
    {
        if (isset($attrs["givenName"][0]) && isset($attrs["sn"][0])){
            $ret =  $attrs["sn"][0].", ".$attrs["givenName"][0]." [".$attrs["uid"][0]."]";
        } else {
            $ret= $attrs['uid'][0];
        }
        return($ret);
    }

    function remove_from_parent()
    {
        plugin::remove_from_parent();

        $ldap= $this->config->get_ldap_link();
        $ldap->rmdir($this->dn);
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
        }

        new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        // Delete references to object groups 
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
        while ($ldap->fetch()){
            $og= new ogroup($this->config, $ldap->getDN());
            unset($og->member[$this->dn]);
            $og->save ();
        }

        // Remove ACL dependencies too,
         
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=gosaAcl)(gosaAclEntry=*".base64_encode($this->dn)."*))",array("gosaAclEntry","dn"));
        while($attrs = $ldap->fetch()){
            $acl = new acl($this->config,$this->parent,$attrs['dn']);
            foreach($acl->gosaAclEntry as $id => $entry){
                foreach($entry['members'] as $m_id => $member){
                    if($m_id == "G:".$this->dn || $m_id == "U:".$this->dn){
                        unset($acl->gosaAclEntry[$id]['members'][$m_id]);
                        gosa_log("modify","groups/acl",$attrs['dn'],array(),sprintf("Removed acl for %s on object %s.",$this->dn,$attrs['dn']));
                    }
                }
            }
            $acl->save();
        }

        // Remove ACL dependencies, too 
        acl::remove_acl_for($this->dn);

        // Send signal to the world that we've done 
        $this->handle_post_events("remove");
    }


    // Save data to object 
    function save_object()
    {
        // Save additional values for possible next step 
        if (isset($_POST['groupedit'])){

            // Create a base backup and reset the base directly after calling plugin::save_object();  
            // Base will be set seperatly a few lines below 
            $base_tmp = $this->base;
            plugin::save_object();
            $this->trustModeDialog->save_object();
            $this->base = $base_tmp;

            // Refresh base 
            if ($this->acl_is_moveable($this->base)){
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base= $this->baseSelector->getBase();
                    $this->is_modified= TRUE;
                }
            }

            $this->force_gid= 0;

            // Only reset sambagroup flag if we are able to write this flag 
            if($this->acl_is_writeable("sambaGroupType")){
                $this->smbgroup = 0;
            }

            foreach (array(
                        "force_gid"  => "gidNumber", 
                        "smbgroup"   => "sambaGroupType") as $val => $aclname) {
                if ($this->acl_is_writeable($aclname)  && isset($_POST["$val"])){
                    $this->$val= get_post($val);
                }
            }

            // Save sambaDomain attribute 
            if ($this->acl_is_writeable("sambaDomainName") && isset ($_POST['sambaDomainName'])){
                $this->sambaDomainName= get_post('sambaDomainName');
                $this->groupType= get_post('groupType');
            }

            // Save fon attribute 
            if ($this->acl_is_writeable("fonGroup")){
                if (isset ($_POST['fon_group'])){
                    $this->fon_group= TRUE;
                } else {
                    $this->fon_group= FALSE;
                }
            }
            if ($this->acl_is_writeable("nagiosGroup")){
                if (isset ($_POST['nagios_group'])){
                    $this->nagios_group= TRUE;
                } else {
                    $this->nagios_group= FALSE;
                }
            }
        }
    }


    // Save to LDAP 
    function save()
    {
        // ID handling 
        if ($this->force_gid == 0){
            if ($this->saved_gidNumber != ""){
                $this->gidNumber= $this->saved_gidNumber;
            } else {
                // Calculate new, lock gids 
                $wait= 10;
                while (get_lock("gidnumber") != ""){
                    sleep (1);

                    // timed out? 
                    if ($wait-- == 0){
                        break;
                    }
                }
                add_lock ("gidnumber", "gosa");
                $this->gidNumber= get_next_id("gidNumber", $this->dn);
            }
        }

        plugin::save(); 

        // Remove objectClass for samba/phone support 
        $tmp= array();
        for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
            if ($this->attrs['objectClass'][$i] != 'sambaGroupMapping' &&
                    $this->attrs['objectClass'][$i] != 'sambaIdmapEntry' &&
                    $this->attrs['objectClass'][$i] != 'goFonPickupGroup' &&
                    $this->attrs['objectClass'][$i] != 'nagiosContactGroup'){
                $tmp[]= $this->attrs['objectClass'][$i];
            }
        }
        $this->attrs['objectClass']= $tmp;
        $ldap= $this->config->get_ldap_link();

        // Add samba group functionality 
        if ($this->smbgroup){
            // Fixed undefined index ...    
            $this->SID = $this->ridBase = "";
            if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])){
                $this->SID    = $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
            }else{
                msg_dialog::display(_("Error"), sprintf(_("Cannot find any SID for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
            }
            if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'])){
                $this->ridBase= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE']; 
            }else{
                msg_dialog::display(_("Error"), sprintf(_("Cannot find any RIDBASE for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
            }

            $this->attrs['objectClass'][]= 'sambaGroupMapping';
            $this->attrs['sambaGroupType']= "2";

            // Check if we need to create a special entry 
            if ($this->groupType == 0){
                if ($this->sambaSID == "" || $this->oldgroupType != $this->groupType){
                    $sid = $this->getSambaSID();
                    $this->attrs['sambaSID']= $sid;
                    $this->sambaSID= $sid;
                }
            } else {
                $this->attrs['sambaSID']=$this->SID."-".$this->groupType;
            }

            /** 
             * User wants me to fake the idMappings? This is useful for
             * making winbind resolve the group names in a reasonable amount
             * of time in combination with larger databases. 
            */
            if ($this->config->boolValueIsTrue("core","sambaIdMapping")){
                $this->attrs['objectClass'][]= "sambaIdmapEntry";
            }

        }

        // Add phone functionality 
        if ($this->fon_group){
            $this->attrs['objectClass'][]= "goFonPickupGroup";
        }

        // Add nagios functionality 
        if ($this->nagios_group){
            $this->attrs['objectClass'][]= "nagiosContactGroup";
        }

        // New accounts need proper 'dn', propagate it to remaining objects 
        if ($this->dn == 'new'){
            $this->dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
        }

        // Add member dn's for RFC2307bis Support 
        $temp = array();
        if (count($this->members)) {
            foreach($this->members as $member) {
                if ($member['dn'] !== $this->dn) {
                    $temp[] = $member['dn'];
                }   
            }
            $this->attrs['member']= $temp;
        }else {
            $this->attrs['member'] = [$this->dn];
        }

        // Save data. Using 'modify' implies that the entry is already present, 
        // use 'add' for new entries. So do a check first... 
        $ldap->cat ($this->dn, array('dn'));
        if ($ldap->fetch()){
            // Modify needs array() to remove values :-( 
            if (!count($this->members)){
                $this->attrs['member']= [$this->dn];
            }
            if (!$this->smbgroup){
                $this->attrs['sambaGroupType']= array();
                $this->attrs['sambaSID']= array();
            }
            $mode= "modify";
        } else {
            $mode= "add";
            $ldap->cd($this->config->current['BASE']);
            $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
        }

        // check generated gidNumber, it may be used by another group. 
        if($this->gidNumber != ""){
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(!(cn=".$this->orig_cn."))(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))",array("cn"));
            if($ldap->count()){
                $cns = "";
                while($attrs = $ldap->fetch()){
                    $cns .= $attrs['cn'][0].", ";
                }
                $cns = rtrim($cns,", ");
                msg_dialog::display(_("Warning"),sprintf(_("The gidNumber '%s' is already in use by %s!"),$this->gidNumber,$cns) , WARNING_DIALOG );
            }
        }

        // check if members are added to the group and delete redundancies
        $this->attrs['member'] = array_unique($this->attrs['member']);

        // if the member array is empty nothing is saved or updated
        if (count($this->attrs['member'])) {
            // Write back to ldap 
            $ldap->cd($this->dn);
            $this->cleanup();
            $ldap->$mode($this->attrs);
        }

        // Remove ACL dependencies too,
        if($this->dn != $this->orig_dn && $this->orig_dn != "new"){
            $tmp = new acl($this->config,$this->parent,$this->dn);
            $tmp->update_acl_membership($this->orig_dn,$this->dn);
        }

        if($this->initially_was_account){
            new log("modify","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            new log("create","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }

        $ret= 0;
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
            $ret= 1;
        }

        $this->trustModeDialog->dn = $this->dn;
        $this->trustModeDialog->save();

        // Remove gid lock 
        del_lock ("gidnumber");

        // Post that we've done
        $this->handle_post_events($mode);

        return ($ret);
    }

    function check()
    {
        // Call common method to give check the hook 
        $message= plugin::check();

        // Permissions for that base? 
        if ($this->base != ""){
            $new_dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
        } else {
            $new_dn= $this->dn;
        }

        // must: cn 
        if ($this->cn == "" && $this->acl_is_writeable("cn")){
            $message[]= msgPool::required(_("Name"));
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        // Check for valid input 
        if (!tests::is_uid($this->cn)){
            if (strict_uid_mode()){
                $message[]= msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/");
            } else {
                $message[]= msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9 _.-]/i");
            }
        }

        // Check if a wrong base was supplied
        if(!$this->baseSelector->checkLastBaseUpdate()){
            $message[]= msgPool::check_base();;
        }

        if($this->allowGroupsWithSameNameInOtherSubtrees == true){
            // Check for used 'cn' 
            $ldap= $this->config->get_ldap_link();
            if(($this->cn  != $this->orig_cn) || ($this->orig_dn == "new")){
                $ldap->cd(get_groups_ou().$this->base);
                $ldap->ls("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))",get_groups_ou().$this->base,array("cn"));
                if ($ldap->count() != 0){
                    $message[]= msgPool::duplicated(_("Name"));
                }
            }
        }else{
            // Check for used 'cn' 
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))",array("cn"));
            if ($ldap->count() != 0){
                // New entry? 
                if ($this->dn == 'new'){
                    $message[]= msgPool::duplicated(_("Name"));
                }
                // Moved? 
                elseif ($new_dn != $this->orig_dn){
                    $ldap->fetch();
                    if ($ldap->getDN() != $this->orig_dn){
                        $message[]= msgPool::duplicated(_("Name"));
                    }
                }
            }
        }

        // Check ID 
        if ($this->force_gid == "1"){
            if (!tests::is_id($this->gidNumber)){
                $message[]= msgPool::invalid(_("GID"), $this->gidNumber, "/[0-9]/");
            } else {
                if ($this->gidNumber < $this->config->get_cfg_value("core","minId")){
                    $message[]= msgPool::toosmall(_("GID"), $this->config->get_cfg_value("core","minId"));
                }

            }
        }

        // Check if we are allowed to create or move this object 
        if(!$this->orig_dn == "new" || 
                $this->orig_base != $this->base || 
                $this->cn != $this->orig_cn){
            if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
                $message[] = msgPool::permCreate();
            }elseif($this->orig_dn != "new" && !$this->acl_is_moveable($this->base)){
                $message[] = msgPool::permMove();
            }
        }
        return ($message);
    }


    function getCopyDialog()
    {
        $vars = array("cn");

        if($this ->force_gid){
            $used = " checked ";
            $dis  = "";
        }else{
            $used = "";
            $dis  = " disabled ";
        }

        $smarty = get_smarty();
        $smarty->assign("used",     set_post($used));
        $smarty->assign("dis" ,     set_post($dis));
        $smarty->assign("cn" ,      set_post($this->cn));
        $smarty->assign("gidNumber",set_post($this->gidNumber));
        $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
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
        if(isset($_POST['force_gid'])){
            $this->force_gid  = 1;
            $this->gidNumber= get_post('gidNumber');
        }else{
            $this->force_gid  = 0;
            $this->gidNumber  = false;
        }
    }


    // Return plugin informations for acl handling   
    static function plInfo()
    {
        return (array(  
                    "plShortName" => _("Generic"),
                    "plDescription" => _("Generic group settings"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("groups" => array("objectClass" => "posixGroup", "description" => _("Groups"))),

                    "plProperties" =>
                    array(
                        array(
                            "name"          => "ogroupRDN",
                            "type"          => "rdn",
                            "default"       => "ou=groups,",
                            "description"   => _("RDN for object group storage."),
                            "check"         => "gosaProperty::isRdn",
                            "migrate"       => "migrate_ogroupRDN",
                            "group"         => "plugin",
                            "mandatory"     => FALSE)),

            "plProvidedAcls"    => array(
                    "cn"                => _("Name"),
                    "description"       => _("Description"),
                    "base"              => _("Base"),

                    "gidNumber"         => _("GID"),

                    "sambaGroupType"    => _("Samba group type"),
                    "sambaDomainName"   => _("Samba domain name"),
                    "accessTo"          => _("System trust"),
                    "fonGroup"          => _("Phone pickup group"),
                    "nagiosGroup"       => _("Nagios group"),

                    "member"            => _("Group member"))
                ));
    }


    function multiple_save_object()
    {
        if(isset($_POST['group_mulitple_edit'])){
            /** 
             * Create a base backup and reset the base directly
             * after calling plugin::save_object(); 
             * Base will be set seperatly a few lines below 
            */
            $base_tmp = $this->base;
            plugin::multiple_save_object();
            plugin::save_object();
            $this->trustModeDialog->multiple_save_object();
            $this->base = $base_tmp;

            foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $attr){
                if(isset($_POST['use_'.$attr])){
                    $this->multi_boxes[] = $attr;
                }
            }

            // Refresh base 
            if ($this->acl_is_moveable($this->base)){
                if (!$this->baseSelector->update()) {
                    msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
                }
                if ($this->base != $this->baseSelector->getBase()) {
                    $this->base= $this->baseSelector->getBase();
                    $this->is_modified= TRUE;
                }
            }

            foreach (array( "smbgroup"   => "sambaGroupType" ,"nagios_group" => "nagiosGroup") as $val => $aclname) {
                if ($this->acl_is_writeable($aclname)){
                    if(isset($_POST["$val"])){
                        $this->$val=  TRUE;
                    }else{
                        $this->$val=  FALSE;
                    }
                }
            }

            // Save sambaDomain attribute 
            if ($this->acl_is_writeable("sambaDomainName") && isset ($_POST['sambaDomainName'])){
                $this->sambaDomainName= get_post('sambaDomainName');
                $this->groupType= get_post('groupType');
            }

            // Save fon attribute 
            if ($this->acl_is_writeable("fonGroup")){
                if (isset ($_POST['fon_group'])){
                    $this->fon_group= TRUE;
                } else {
                    $this->fon_group= FALSE;
                }
            }
        }
    }

    function get_multi_edit_values()
    {
        $ret = plugin::get_multi_edit_values();
        $ret = array_merge($ret,$this->trustModeDialog->get_multi_edit_values());

        foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $attr){    
            if(in_array_strict($attr,$this->multi_boxes)){
                $ret[$attr] = $this->$attr;
            }
        }

        $ret['member'] = $this->members;
        $ret['members_used_by_some'] = $this->members_used_by_some;
        return($ret);
    }

    function multiple_execute()
    {
        return($this->execute());
    }


    // Initialize plugin with given atribute arrays 
    function init_multiple_support($attrs,$all)
    {
        plugin::init_multiple_support($attrs,$all);
        $this->trustModeDialog->init_multiple_support($attrs,$all);

        $this->dns = $all['dn'];
        $diff = array();
        $this->members = array();
        $this->members_used_by_some = array();

        // get all members who are in all groups for multiple edit
        if (isset($attrs['member'])){
            $this->members = $this->reload($attrs);
            ksort($this->members);
        }

        // sort out all members that are not in all groups
        foreach ($all['member'] as $member => $value) {
            if (!in_array($value, $attrs['member'])) {
                $diff['member'][] = $value;
            }
        }

        // get all members who are not in all groups for multiple edit
        if (isset($diff['member'])){
            $this->members_used_by_some = $this->reload($diff);
            ksort($this->members_used_by_some);
        }

        // We've two lists in mutliple support  
        //  - one which represents those users which are part of ALL groups.
        //  - ond one which represents those users which are only part of SOME groups.
        $this->commonList = new sortableListing();
        $this->commonList->setDeleteable(true);
        $this->commonList->setInstantDelete(false);
        $this->commonList->setEditable(false);
        $this->commonList->setWidth("100%");
        $this->commonList->setHeight("120px");
        $this->commonList->setHeader(array('~',_("Common Name"),_("UID")));
        $this->commonList->setColspecs(array('20px','*','*','20px'));
        $this->commonList->setDefaultSortColumn(1);

        $this->partialList = new sortableListing();
        $this->partialList->setDeleteable(true);
        $this->partialList->setInstantDelete(false);
        $this->partialList->setEditable(false);
        $this->partialList->setWidth("100%");
        $this->partialList->setHeight("120px");
        $this->partialList->setHeader(array('~',_("Common Name"),_("UID")));
        $this->partialList->setColspecs(array('20px','*','*','20px'));
        $this->partialList->setDefaultSortColumn(1);
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        $this->trustModeDialog->PrepareForCopyPaste($source);

        // Get samba Domain in case of samba 3 
        if ($this->sambaSID != ""){
            $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase"));
            if ($ldap->count() != 0){
                $attrs= $ldap->fetch();
                if(isset($attrs['sambaAlgorithmicRidBase'])){
                    $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];
                } else {
                    $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                }

                // Get domain name for SID 
                $this->sambaDomainName= "DEFAULT";
                foreach ($this->config->data['SERVERS']['SAMBA'] as $key => $val){
                    if ($val['SID'] == $this->SID){
                        $this->sambaDomainName= $key;
                        break;
                    }
                }
            } else {
                if ($this->config->get_cfg_value("core","sambaRidBase") != ""){
                    $this->sambaDomainName= "DEFAULT";
                    $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                    $this->SID= $this->config->get_cfg_value("core","sambaSID");
                } else {
                    msg_dialog::display(_("Configuration error"), _("Cannot find group SID in your configuration!"), ERROR_DIALOG);
                }
            }

            // Get group type 
            $this->groupType= (int)substr(strrchr($this->sambaSID, "-"), 1);
            if ($this->groupType < 500 || $this->groupType > 553){
                $this->groupType= 0;
            }
            $this->oldgroupType= $this->groupType;
        }

        // Detect samba groups and adapt its values.
        $this->smbgroup = in_array_strict('sambaGroupMapping', $source['objectClass']);
        if ($this->smbgroup) { 
            $this->sambaSID = $this->getSambaSID(); 
        } 

        $this->members = array();
        if(isset($source['member'])){
            for($i = 0 ; $i < $source['member']['count']; $i ++){
                $this->members[] = $source['member'][$i];
            }
        }
    }

    function set_acl_base($base)
    {
        plugin::set_acl_base($base);
        $this->trustModeDialog->set_acl_base($base);
    }

    //! \brief  Enables multiple support for this plugin
    function enable_multiple_support()
    {
        plugin::enable_multiple_support();
        $this->trustModeDialog->enable_multiple_support();
    }

    // Set all members of edited groups in $members 
    function set_multi_edit_values($attrs)
    {
        $users = array();

        // Update groupMembership, keep optional group 
        foreach($attrs['members_used_by_some'] as $member){
            if(in_array_strict($member,$this->members)){
                $users[] = $member;
            }
        }

        // Update groupMembership, add forced groups 
        foreach($attrs['member'] as $member){
                $users[] = $member;
        }
        
        plugin::set_multi_edit_values($attrs);
        $this->trustModeDialog->set_multi_edit_values($attrs);
        $this->members = $users;
    }


    //! \brief Get a new SambaSID for a group  
    function getSambaSID() 
    { 
        $ldap = $this->config->get_ldap_link(); 
        $gidNumber= $this->gidNumber; 
        while(TRUE){ 
            $sid= $this->SID."-".($gidNumber*2 + $this->ridBase+1); 
            $ldap->cd($this->config->current['BASE']); 
            $ldap->search("(sambaSID=$sid)",array("sambaSID")); 
            if ($ldap->count() == 0){ 
                break; 
            } 
            $gidNumber++; 
        }
        return $sid; 
    } 
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
