<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */




class Step_Migrate extends setup_step
{
    var $header_image   = "images/setup/migrate.png";
    var $checks         = array();

    /* Create Acl attributes */
    var $acl_create_dialog  = FALSE;
    var $acl_create_selected= ""; // Currently selected element, that should receive admin rights 
    var $acl_create_changes = ""; // Contains ldif information about changes 
    var $acl_create_confirmed= FALSE;

    /* Checks initialised ? */
    var $checks_initialised = FALSE;

    /* Root object classes */
    var $rootOC_migrate_dialog = FALSE;
    var $rootOC_details = array();

    function Step_Migrate()
    {
        $this->update_strings(); 
    }

    function update_strings()
    {
        $this->s_title      = _("LDAP inspection");
        $this->s_title_long = _("LDAP inspection");
        $this->s_info       = _("Analyze your current LDAP for GOsa compatibility");
    }

    function initialize_checks()
    {
        $this->checks = array();
        $this->checks['root']['TITLE']     = _("Checking for root object");
        $this->checks['root']['STATUS']    = FALSE;
        $this->checks['root']['STATUS_MSG']= "";
        $this->checks['root']['ERROR_MSG'] = "";
        $this->checkBase();

        $this->checks['rootOC']['TITLE']     = _("Inspecting object classes in root object");
        $this->checks['rootOC']['STATUS']    = FALSE;
        $this->checks['rootOC']['STATUS_MSG']= "";
        $this->checks['rootOC']['ERROR_MSG'] = "";
        $this->checkBaseOC();

        $this->checks['permissions']['TITLE']     = _("Checking permission for LDAP database");
        $this->checks['permissions']['STATUS']    = FALSE;
        $this->checks['permissions']['STATUS_MSG']= "";
        $this->checks['permissions']['ERROR_MSG'] = "";
        $this->check_ldap_permissions();

        $this->migrate_users = array();
        $this->checks['acls']['TITLE']     = _("Checking for super administrator");
        $this->checks['acls']['STATUS']    = FALSE;
        $this->checks['acls']['STATUS_MSG']= "";
        $this->checks['acls']['ERROR_MSG'] = "";
        $this->check_administrativeAccount();
    }


    /* Check ldap accessibility 
     * Create and remove a dummy object, 
     *  to ensure that we have the necessary permissions
     */
    function check_ldap_permissions()
    {
        /* Establish ldap connection */
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);

        /* Create dummy entry 
         */
        $name     = "GOsa_setup_text_entry_".session_id().rand(0,999999);
        $dn       = "ou=".$name.",".$cv['base'];
        $testEntry= array();
        $testEntry['objectClass'][]= "top";
        $testEntry['objectClass'][]= "organizationalUnit";
        $testEntry['objectClass'][]= "gosaDepartment";
        $testEntry['description']= "Created by GOsa setup, this object can be removed.";
        $testEntry['ou']  = $name;

        /* check if simple ldap cat will be successful 
         */
        $res = $ldap->cat($cv['base']);  
        if(!$res){
            $this->checks['permissions']['STATUS']    = FALSE;
            $this->checks['permissions']['STATUS_MSG']= _("LDAP query failed");
            $this->checks['permissions']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
            return(false);
        }

        /* Try to create dummy object 
         */ 
        $ldap->cd ($dn);
        $res = $ldap->add($testEntry);
        $ldap->cat($dn);
        if(!$ldap->count()){
            new log("view","setup/".get_class($this),$dn,array(),$ldap->get_error());

            $this->checks['permissions']['STATUS']    = FALSE;
            $this->checks['permissions']['STATUS_MSG']= _("Failed");
            $this->checks['permissions']['ERROR_MSG'] = 
                sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
            return(false);
        }

        /* Try to remove created entry 
         */
        $res = $ldap->rmDir($dn);
        $ldap->cat($dn);
        if($ldap->count()){
            new log("view","setup/".get_class($this),$dn,array(),$ldap->get_error());
            $this->checks['permissions']['STATUS']    = FALSE;
            $this->checks['permissions']['STATUS_MSG']= _("Failed");
            $this->checks['permissions']['ERROR_MSG'] = 
                sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
            return(false);
        }

        /* Create & remove of dummy object was successful */
        $this->checks['permissions']['STATUS']    = TRUE;
        $this->checks['permissions']['STATUS_MSG']= _("Ok");
        $this->checks['permissions']['ERROR_MSG'] = "";
        return(true);
    } 




    /* Check Acls if there is at least one object with acls defined 
     */
    function check_administrativeAccount()
    {
        /* Reset settings 
         */ 
        $GOsa_26_found = FALSE;
        $this->migrate_users = array();
        $this->acl_migrate_dialog = FALSE;
        $this->migrate_acl_base_entry  = "";

        /* Establish ldap connection */
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);
        $ldap->cd($cv['base']);
        $res = $ldap->cat($cv['base']);

        if(!$res){
            $this->checks['acls']['STATUS']    = FALSE;
            $this->checks['acls']['STATUS_MSG']= _("LDAP query failed");
            $this->checks['acls']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
        }else{
            $GOsa_26_found = false; // GOsa 2.6 Account found
            $GOsa_25_found = false; // GOsa 2.5 Account found, allow migration

            $username = "";
            $attrs = $ldap->fetch();

            /* Collect a list of available GOsa users and groups 
             */
            $users = array();
            $ldap->search("(&(objectClass=gosaAccount)(objectClass=person)".
                    "(objectClass=inetOrgPerson)(objectClass=organizationalPerson))",array("uid","dn"));
            while($user_attrs = $ldap->fetch()){
                $users[$user_attrs['dn']] = $user_attrs['uid'][0];
                $rusers[$user_attrs['uid'][0]] = $user_attrs['dn'];
            }
            $groups = array();
            $ldap->search("objectClass=posixGroup",array("cn","dn"));
            while($group_attrs = $ldap->fetch()){
                $groups[$group_attrs['dn']] = $group_attrs['cn'][0];
            }

            /* Check if a valid GOsa 2.6 admin exists 
               -> gosaAclEntry for an existing and accessible user.
             */
            $valid_users = "";
            $valid_groups = "";
            if(isset($attrs['gosaAclEntry'])){
                $acls = $attrs['gosaAclEntry'];
                for($i = 0 ; $i < $acls['count'] ; $i++){
                    $acl = $acls[$i];
                    $tmp = explode(":",$acl);

                    if($tmp[1] == "psub"){
                        $members = explode(",",$tmp[2]);
                        foreach($members as $member){
                            $member = base64_decode($member);
                            if(isset($users[$member])){
                                if(preg_match("/all;cmdrw/i",$tmp[3])){
                                    $valid_users .= $users[$member].", ";
                                    $GOsa_26_found  = TRUE;
                                }
                            }
                            if(isset($groups[$member])){
                                if(preg_match("/all;cmdrw/i",$tmp[3])){
                                    $ldap->cat($member);
                                    $group_attrs = $ldap->fetch();
                                    $val_users = "";
                                    if(isset($group_attrs['memberUid'])){
                                        for($e = 0 ; $e < $group_attrs['memberUid']['count']; $e ++){
                                            if(isset($rusers[$group_attrs['memberUid'][$e]])){
                                                $val_users .= $group_attrs['memberUid'][$e].", ";
                                            }
                                        }
                                    }
                                    if(!empty($val_users)){
                                        $valid_groups .= $groups[$member]."(<i>".trim($val_users,", ")."</i>), ";
                                        $GOsa_26_found  = TRUE;
                                    }
                                }
                            }
                        }
                    }elseif($tmp[1] == "role"){

                        /* Check if acl owner is a valid GOsa user account */
                        $ldap->cat(base64_decode($tmp[2]),array("gosaAclTemplate"));
                        $ret = $ldap->fetch();

                        if(isset($ret['gosaAclTemplate'])){
                            $cnt = $ret['gosaAclTemplate']['count'];
                            for($e = 0 ; $e < $cnt ; $e++){

                                $a_str = $ret['gosaAclTemplate'][$e];
                                if(preg_match("/^[0-9]*:psub:/",$a_str) && preg_match("/:all;cmdrw$/",$a_str)){

                                    $members = explode(",",$tmp[3]);
                                    foreach($members as $member){
                                        $member = base64_decode($member);

                                        if(isset($users[$member])){
                                            $valid_users .= $users[$member].", ";
                                            $GOsa_26_found  = TRUE;
                                        }
                                        if(isset($groups[$member])){
                                            $ldap->cat($member);
                                            $group_attrs = $ldap->fetch();
                                            $val_users = "";
                                            if(isset($group_attrs['memberUid'])){
                                                for($e = 0 ; $e < $group_attrs['memberUid']['count']; $e ++){
                                                    if(isset($rusers[$group_attrs['memberUid'][$e]])){
                                                        $val_users .= $group_attrs['memberUid'][$e].", ";
                                                    }
                                                }
                                            }
                                            if(!empty($val_users)){
                                                $valid_groups .= $groups[$member]."(<i>".trim($val_users,", ")."</i>), ";
                                                $GOsa_26_found  = TRUE;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            /* Try to find an old GOsa 2.5 administrative account that may be migrated 
             */
            if(!$GOsa_26_found){
                $valid_users = "";
                $valid_groups = "";
                $ldap->cd($cv['base']);
                $ldap->search("(&(objectClass=posixGroup)(gosaSubtreeACL=:all)(memberUid=*))",array("memberUid","cn"));
                while($p_group = $ldap->fetch()){
                    $val_users = "";
                    for($e = 0 ; $e < $p_group['memberUid']['count'] ; $e ++ ){
                        $user = $p_group['memberUid'][$e];
                        if(isset($rusers[$user])){
                            $val_users .= $user.", ";
                        }  
                    }
                    if(!empty($val_users)){
                        $valid_groups .= $groups[$p_group['dn']]."(<i>".trim($val_users,", ")."</i>), ";
                        $GOsa_25_found  = TRUE;
                    }
                }
            }


            /* Print out results 
             */
            if($GOsa_25_found){
                $str = "";
                if(!empty($valid_groups)){
                    $str.= "<i>".sprintf(_("GOsa 2.5 administrative accounts found: %s"),trim($valid_groups,", "))."</i><br>";
                }
                $this->checks['acls']['STATUS']    = FALSE;
                $this->checks['acls']['STATUS_MSG']= _("Failed");
                $this->checks['acls']['ERROR_MSG'] = $str;
                $this->checks['acls']['ERROR_MSG'].= _("There is no valid GOsa 2.6 administrator account inside your LDAP.")."&nbsp;";
                $this->checks['acls']['ERROR_MSG'].= "<input type='submit' name='migrate_acls' value='"._("Migrate")."'>";
                $this->checks['acls']['ERROR_MSG'].= "<input type='submit' name='create_acls' value='"._("Create")."'>";
            }elseif($GOsa_26_found){
                $str = "";
                if(!empty($valid_users)){
                    $str.= "<b>"._("Users")."</b>:&nbsp;".trim($valid_users,", ")."<br>";
                }
                if(!empty($valid_groups)){
                    $str.= "<b>"._("Groups")."</b>:&nbsp;".trim($valid_groups,", ")."<br>";
                }
                $this->checks['acls']['STATUS']    = TRUE;
                $this->checks['acls']['STATUS_MSG']= _("Ok");
                $this->checks['acls']['ERROR_MSG'] = $str;
            }else{
                $this->checks['acls']['STATUS']    = FALSE;
                $this->checks['acls']['STATUS_MSG']= _("Failed");
                $this->checks['acls']['ERROR_MSG']= _("There is no GOsa administrator account inside your LDAP.")."&nbsp;";
                $this->checks['acls']['ERROR_MSG'].= "<input type='submit' name='create_acls' value='"._("Create")."'>";
            }
        }

        // Reload base OC
        $this->checkBaseOC();
        return($GOsa_26_found);
    }



    function create_admin($only_ldif = FALSE)
    {
        /* Reset '' */
        $this->acl_create_changes="";

        /* Object that should receive admin acls */
        $dn = $this->acl_create_selected;

        /* Get collected configuration settings */
        $cv = $this->parent->captured_values;

        /* On first call check for rid/sid base */
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);

        /* Get current base attributes */
        $ldap->cd($cv['base']);
        $ldap->cat($cv['base'],array("dn","objectClass","gosaAclEntry"));
        $attrs = $ldap->fetch();

        /* Add acls for the selcted user to the base */
        $attrs_new = array();
        $attrs_new['objectClass'] = array("gosaACL");

        for($i = 0; $i < $attrs['objectClass']['count']; $i ++){
            if(!in_array_ics($attrs['objectClass'][$i],$attrs_new['objectClass'])){
                $attrs_new['objectClass'][] = $attrs['objectClass'][$i];
            }
        }

        $acl = "0:psub:".base64_encode($dn).":all;cmdrw";    
        $attrs_new['gosaAclEntry'][] = $acl;
        if(isset($attrs['gosaAclEntry'])){
            for($i = 0 ; $i < $attrs['gosaAclEntry']['count']; $i ++){

                $prio = preg_replace("/[:].*$/","",$attrs['gosaAclEntry'][$i]);
                $rest = preg_replace("/^[^:]/","",$attrs['gosaAclEntry'][$i]);

                $data = ($prio+1).$rest;
                $attrs_new['gosaAclEntry'][] = $data;
            }
        }

        if($only_ldif){
            $this->acl_create_changes ="\n".($ldap->fix($cv['base']))."\n";
            $this->acl_create_changes.=$this->array_to_ldif($attrs)."\n";
            $this->acl_create_changes.="\n".($ldap->fix($cv['base']))."\n";
            $this->acl_create_changes.=$this->array_to_ldif($attrs_new);
        }else{

            $ldap->cd($cv['base']);
            if(!$ldap->modify($attrs_new)){
                msg_dialog::display(_("Migration error"), sprintf(_("Cannot add ACL for user '%s':")."<br><br><i>%s</i>", LDAP::fix($dn), $ldap->get_error()), ERROR_DIALOG);
                return(FALSE);
            }else{
                return(TRUE);
            }
        }
    }


    function create_admin_user()
    {
        $pw1 = $pw2 = "";
        $uid = "";

        /* On first call check for rid/sid base */
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);

        if(isset($_POST['new_user_uid'])){
            $uid = $_POST['new_user_uid'];
        }
        if(isset($_POST['new_user_password'])){
            $pw1 = $_POST['new_user_password'];
        }
        if(isset($_POST['new_user_password2'])){
            $pw2 = $_POST['new_user_password2'];
        }


        $ldap->cd($cv['base']);
        $ldap->search("(uid=".$uid.")");
        if($ldap->count()){
            msg_dialog::display(_("Input error"),msgPool::duplicated(_("Uid")), ERROR_DIALOG);
            return false;
        }

        if(empty($pw1) || empty($pw2) | ($pw1 != $pw2)){
            msg_dialog::display(_("Password error"), _("Provided passwords do not match!"), ERROR_DIALOG);
            return false;
        }

        if(!tests::is_uid($uid) || empty($uid)){
            msg_dialog::display(_("Input error"), _("Specify a valid user ID!"), ERROR_DIALOG);
            return false;
        }


        /* Get current base attributes */
        $ldap->cd($cv['base']);

        $people_ou = "ou=people,"; // Thats the property default.
        $dn = "cn=System Administrator-".$uid.",".$people_ou.$cv['base'];

        $hash = $hash = passwordMethod::make_hash($pw2, 'crypt/standard-des');

        $new_user=array();
        $new_user['objectClass']= array("top","person","gosaAccount","organizationalPerson","inetOrgPerson");
        $new_user['givenName']  = "System";
        $new_user['sn']  = "Administrator";
        $new_user['cn']  = "System Administrator-".$uid;
        $new_user['uid'] = $uid;
        $new_user['userPassword'] = $hash;

        $ldap->cd($cv['base']);

        $ldap->cat($dn,array("dn"));
        if($ldap->count()){
            msg_dialog::display(_("Error"), sprintf(_("Adding an administrative user failed: object '%s' already exists!"), LDAP::fix($dn)), ERROR_DIALOG);
            return(FALSE);  
        }

        $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$dn));
        $ldap->cd($dn);  
        $res = $ldap->add($new_user);
        $this->acl_create_selected = $dn;
        $this->create_admin();

        if(!$res){
            msg_dialog::display(_("LDAP error"), $ldap->get_error(), ERROR_DIALOG);
            return(FALSE);
        }

        $this->acl_create_dialog=FALSE;        
        $this->check_administrativeAccount();
        return(TRUE);
    }


    function execute()
    {
        /* Initialise checks if this is the first call */
        if(!$this->checks_initialised || isset($_POST['reload'])){
            $this->initialize_checks();
            $this->checks_initialised = TRUE;
        }


        /*************
         * Root object check  
         *************/

        if(isset($_POST['retry_root_create'])){

            $state = $this->checks['root']['STATUS'];
            $this->checkBase(FALSE);
            if($state != $this->checks['root']['STATUS']){
                $this->initialize_checks();
            }
        }

        /*************
         * Root object class check  
         *************/

        if(isset($_POST['root_add_objectclasses'])){
            $this->rootOC_migrate_dialog = TRUE;
            $this->dialog = TRUE;
        }
        if(isset($_POST['rootOC_dialog_cancel'])){
            $this->rootOC_migrate_dialog = FALSE;
            $this->dialog = FALSE;
        }
        if(isset($_POST['rootOC_migrate_start'])){
            if($this->checkBaseOC(FALSE)){
                $this->checkBaseOC(); // Update overview info
                $this->dialog = FALSE;
                $this->rootOC_migrate_dialog = FALSE;
            }
        }
        if($this->rootOC_migrate_dialog){
            $smarty = get_smarty();
            $smarty->assign("details",$this->rootOC_details);
            $smarty->assign("method","rootOC_migrate_dialog");
            return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
        }

        /*************
         * Administrative Account -- Migrate/Create 
         *************/

        if(isset($_POST['retry_acls'])){
            $this->check_administrativeAccount();
        }

        /* Dialog handling */
        if(isset($_POST['create_acls'])){
            $this->acl_create_dialog = TRUE;
            $this->dialog = TRUE;
        }

        if(isset($_POST['migrate_acls'])){
            $this->acl_migrate_dialog = TRUE;
            $this->dialog = TRUE;
        }

        if(isset($_POST['create_acls_cancel']) || isset($_POST['migrate_acls_cancel'])){
            $this->acl_create_dialog = FALSE;
            $this->acl_migrate_dialog = FALSE;
            $this->dialog = FALSE;
            $this->show_details = FALSE;
        }

        /* Account creation */
        if(isset($_POST['create_acls_create'])){
            $this->create_admin(TRUE);
        }

        if(isset($_POST['create_admin_user'])){
            if($this->create_admin_user()){
                $this->dialog = FALSE;
                $this->show_details = FALSE;
            }
        }

        /* Add admin acls for the selected users to the ldap base.
         */
        if($this->acl_migrate_dialog && isset($_POST['migrate_admin_user'])){

            /* Update ldap and reload check infos 
             */
            $this->migrate_selected_admin_users();
            $this->dialog = FALSE;
            $this->acl_migrate_dialog = FALSE;

        }elseif($this->acl_migrate_dialog){

            /* Display admin migration dialog.
             */
            $this->migrate_users();
            $smarty = get_smarty();

            /* Do we have to display the changes
             */
            $details = isset($_POST['details']) && $_POST['details'];
            if(isset($_POST['migrate_acls_show_changes'])){
                $details = TRUE;
            }elseif(isset($_POST['migrate_acls_hide_changes'])){
                $details = FALSE;
            }

            $smarty->assign("migrate_acl_base_entry", $this->migrate_acl_base_entry);
            $smarty->assign("details", $details);
            $smarty->assign("method","migrate_acls");
            $smarty->assign("migrateable_users",$this->migrate_users);
            return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
        }

        if($this->acl_create_dialog){
            $smarty = get_smarty();
            $uid = "admin";
            if(isset($_POST['new_user_uid'])){
                $uid = $_POST['new_user_uid'];
            }
            $smarty->assign("new_user_uid",$uid);
            $smarty->assign("new_user_password",@$_POST['new_user_password']);
            $smarty->assign("new_user_password2",@$_POST['new_user_password2']);
            $smarty->assign("method","create_acls");
            $smarty->assign("acl_create_selected",$this->acl_create_selected);
            $smarty->assign("what_will_be_done_now",$this->acl_create_changes);
            return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
        }


        $smarty = get_smarty();
        $smarty->assign("checks",$this->checks);
        $smarty->assign("method","default");
        return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }


    function save_object()
    {
        $this->is_completed= TRUE;


        /* Get "create acl" dialog posts */
        if($this->acl_create_dialog){

            if(isset($_POST['create_acls_create_abort'])){
                $this->acl_create_selected = "";
            }
        }

    }


    /* Check if the root object exists.
     * If the parameter just_check is true, then just check if the 
     *  root object is missing and update the info messages.
     * If the Parameter is false, try to create a new root object.
     */
    function checkBase($just_check = TRUE)
    {
        /* Establish ldap connection */
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);

        /* Check if root object exists */
        $ldap->cd($cv['base']);
        $ldap->set_size_limit(1);
        $res = $ldap->search("(objectClass=*)");
        $ldap->set_size_limit(0);
        $err = ldap_errno($ldap->cid); 

        if( !$res || 
                $err == 0x20 ||  # LDAP_NO_SUCH_OBJECT
                $err == 0x40) {  # LDAP_NAMING_VIOLATION

            /* Root object doesn't exists 
             */
            if($just_check){
                $this->checks['root']['STATUS']    = FALSE;
                $this->checks['root']['STATUS_MSG']= _("Failed");
                $this->checks['root']['ERROR_MSG'] =  _("The LDAP root object is missing. It is required to use your LDAP service.").'&nbsp;';
                $this->checks['root']['ERROR_MSG'].=  "<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
                return(FALSE);
            }else{

                /* Add root object */ 
                $ldap->cd($cv['base']);
                $res = $ldap->create_missing_trees($cv['base']);

                /* If adding failed, tell the user */
                if(!$res){
                    $this->checks['root']['STATUS']    = FALSE;
                    $this->checks['root']['STATUS_MSG']= _("Failed");
                    $this->checks['root']['ERROR_MSG'] = _("Root object couldn't be created, you should try it on your own.");
                    $this->checks['root']['ERROR_MSG'].= "&nbsp;<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
                    return($res);;
                }
            }
        }

        /* Create & remove of dummy object was successful */
        $this->checks['root']['STATUS']    = TRUE;
        $this->checks['root']['STATUS_MSG']= _("Ok");
    }


    /* Check if the root object includes the required object classes, e.g. gosaDepartment is required for ACLs.
     * If the parameter just_check is true, then just check for the OCs. 
     * If the Parameter is false, try to add the required object classes.
     */
    function checkBaseOC($just_check = TRUE)
    {
        /* Establish ldap connection */
        $cv = $this->parent->captured_values;
        $ldap_l = new LDAP($cv['admin'],
                $cv['password'],
                $cv['connection'],
                FALSE,
                $cv['tls']);

        $ldap = new ldapMultiplexer($ldap_l);

        /* Check if root object exists */
        $ldap->cd($cv['base']);
        $ldap->cat($cv['base']);
        if(!$ldap->count()){
            $this->checks['rootOC']['STATUS']    = FALSE;
            $this->checks['rootOC']['STATUS_MSG']= _("LDAP query failed");
            $this->checks['rootOC']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
            return;
        }

        $attrs = $ldap->fetch();

        /* Root object doesn't exists 
         */
        if(!in_array("gosaDepartment",$attrs['objectClass'])){
            if($just_check){

                $this->rootOC_details = array();	
                $mods = array();

                /* Get list of possible container objects, to be able to detect naming 
                 *  attributes and missing attribute types.
                 */
                if(!class_available("departmentManagement")){
                    $this->checks['rootOC']['STATUS']    = FALSE;
                    $this->checks['rootOC']['STATUS_MSG']= _("Failed");
                    $this->checks['rootOC']['ERROR_MSG'] = sprintf(_("Missing GOsa object class '%s'!"),"departmentManagement").
                        "&nbsp;"._("Please check your installation.");
                    return;
                }

                /* Try to detect base class type, e.g. is it a dcObject.
                 */
                $dep_types = departmentManagement::get_support_departments();
                $dep_type ="";
                foreach($dep_types as $dep_name => $dep_class){
                    if(in_array($dep_class['CLASS'], $attrs['objectClass'])){
                        $dep_type = $dep_name;
                        break;
                    }
                }

                /* If no known base class was detect, abort with message
                 */	
                if(empty($dep_type)){
                    $this->checks['rootOC']['STATUS']    = FALSE;
                    $this->checks['rootOC']['STATUS_MSG']= _("Failed");
                    $this->checks['rootOC']['ERROR_MSG'] = 
                        sprintf(_("Cannot handle the structural object type of your root object. Please try to add the object class '%s' manually."),"gosaDepartment");
                    return;
                }

                /* Create 'current' and 'target' object properties, to be able to display 
                 *  a set of modifications required to create a valid GOsa department.
                 */	
                $str = "dn: ".$cv['base']."\n";
                for($i = 0 ; $i<$attrs['objectClass']['count'];$i++){
                    $str .= "objectClass: ".$attrs['objectClass'][$i]."\n";
                }
                $this->rootOC_details['current'] = $str;

                /* Create target infos 
                 */
                $str = "dn: ".$cv['base']."\n";
                for($i = 0 ; $i<$attrs['objectClass']['count'];$i++){
                    $str .= "objectClass: ".$attrs['objectClass'][$i]."\n";
                    $mods['objectClass'][] = $attrs['objectClass'][$i];
                }
                $mods['objectClass'][] = "gosaDepartment";
                $str .= "<b>objectClass: gosaDepartment</b>\n";

                /* Append attribute 'ou', it is required by gosaDepartment
                 */
                if(!isset($attrs['ou'])){
                    $val = "GOsa";
                    if(isset($attrs[$dep_types[$dep_type]['ATTR']][0])){
                        $val = $attrs[$dep_types[$dep_type]['ATTR']][0];
                    }
                    $str .= "<b>ou: ".$val."</b>\n";
                    $mods['ou'] =$val;
                }

                /*Append description, it is required by gosaDepartment too.
                 */
                if(!isset($attrs['description'])){
                    $val = "GOsa";
                    if(isset($attrs[$dep_types[$dep_type]['ATTR']][0])){
                        $val = $attrs[$dep_types[$dep_type]['ATTR']][0];
                    }
                    $str .= "<b>description: ".$val."</b>\n";
                    $mods['description'] = $val;
                }
                $this->rootOC_details['target'] = $str;
                $this->rootOC_details['mods'] = $mods;

                /*  Add button that allows to open the migration details
                 */
                $this->checks['rootOC']['STATUS']    = FALSE;
                $this->checks['rootOC']['STATUS_MSG']= _("Failed");
                $this->checks['rootOC']['ERROR_MSG'] = "&nbsp;<input type='submit' 
                    name='root_add_objectclasses' value='"._("Migrate")."'>";

                return(FALSE);
            }else{

                /* Add root object */ 
                $ldap->cd($cv['base']);
                if(isset($this->rootOC_details['mods'])){
                    $res  = $ldap->modify($this->rootOC_details['mods']);	
                    if(!$res){
                        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $cv['base'], LDAP_MOD, get_class()));
                    }
                    $this->checkBaseOC();
                    $this->check_administrativeAccount();
                    return($res);
                }else{
                    trigger_error("No modifications to make... ");
                }
            }
            return(TRUE);
        }

        /* Create & remove of dummy object was successful */
        $this->checks['rootOC']['STATUS']    = TRUE;
        $this->checks['rootOC']['STATUS_MSG']= _("Ok");
        $this->checks['rootOC']['ERROR_MSG'] = "";
    }


    /* Return ldif information for a 
     * given attribute array 
     */
    function array_to_ldif($atts)
    {
        $ret = "";
        unset($atts['count']);
        unset($atts['dn']);
        foreach($atts as $name => $value){
            if(is_numeric($name)) {
                continue;
            }
            if(is_array($value)){
                unset($value['count']);
                foreach($value as $a_val){
                    $ret .= $name.": ". $a_val."\n";
                }
            }else{
                $ret .= $name.": ". $value."\n";
            }
        }
        return(preg_replace("/\n$/","",$ret));
    }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
