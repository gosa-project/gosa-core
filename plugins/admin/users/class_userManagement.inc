<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class userManagement extends management
{
    var $plHeadline     = "Users";
    var $plDescription  = "Manage aspects of user accounts like generic, POSIX, samba and mail settings";
    var $plIcon  = "plugins/users/images/user.png";

    var $sn = "";
    var $givenName = "";
    var $uid = "";
    var $got_uid = "";
    var $edit_uid = "";

    var $pwd_change_queue = array();
    var $force_hash_type = array();

    // Tab definition 
    protected $tabClass = "usertabs";
    protected $tabType = "USERTABS";
    protected $aclCategory = "users";
    protected $aclPlugin   = "user";
    protected $objectName   = "user";

    protected $proposal = "";
    protected $proposalEnabled = FALSE;
    protected $proposalSelected = FALSE;

    protected $passwordChangeForceable = FALSE;
    protected $enforcePasswordChange = FALSE;

    function __construct($config,$ui)
    {
        $this->config = $config;
        $this->ui = $ui;

        $this->storagePoints = array(get_ou("core", "userRDN"));

        // Build filter
        if (session::global_is_set(get_class($this)."_filter")){
            $filter= session::global_get(get_class($this)."_filter");
        } else {
            $filter = new filter(get_template_path("user-filter.xml", true));
            $filter->setObjectStorage($this->storagePoints);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path("user-list.xml", true));
        $headpage->registerElementFilter("lockLabel", "userManagement::filterLockLabel");
        $headpage->registerElementFilter("lockImage", "userManagement::filterLockImage");
        $headpage->registerElementFilter("filterProperties", "userManagement::filterProperties");
        $headpage->setFilter($filter);

        // Add copy&paste and snapshot handler.
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler = new CopyPasteHandler($this->config);
        }
        if($this->config->get_cfg_value("core","enableSnapshots") == "true"){
            $this->snapHandler = new SnapshotHandler($this->config);
        }

        parent::__construct($config, $ui, "users", $headpage);

        // Register special user actions 
        $this->registerAction("lock",   "lockEntry");
        $this->registerAction("lockUsers",   "lockUsers");
        $this->registerAction("unlockUsers", "lockUsers");
        $this->registerAction("new_template", "newTemplate");
        $this->registerAction("newfromtpl", "newUserFromTemplate");
        $this->registerAction("templateContinue", "templateContinue");
        $this->registerAction("templatize", "templatizeUsers");
        $this->registerAction("templatizeContinue", "templatizeContinue");

        $this->registerAction("password", "changePassword");
        $this->registerAction("passwordQueue", "handlePasswordQueue");
        $this->registerAction("passwordCancel", "closeDialogs");

        $this->registerAction("sendMessage", "sendMessage");
        $this->registerAction("saveEventDialog", "saveEventDialog");
        $this->registerAction("abortEventDialog", "closeDialogs");

        // Register shortcut icon actions 
        $this->registerAction("edit_user","editEntry");
        $this->registerAction("edit_posixAccount","editEntry");
        $this->registerAction("edit_mailAccount","editEntry");
        $this->registerAction("edit_sambaAccount","editEntry");
        $this->registerAction("edit_netatalk","editEntry");
        $this->registerAction("edit_environment","editEntry");
        $this->registerAction("edit_gofaxAccount","editEntry");
        $this->registerAction("edit_phoneAccount","editEntry");
        $this->registerAction("edit_zarafaAccount","editEntry");
    }


    // Inject user actions 
    function detectPostActions()
    {
        $action = management::detectPostActions();
        if(isset($_POST['template_continue'])) $action['action'] = "templateContinue";
        if(isset($_POST['templatize_continue'])) $action['action'] = "templatizeContinue";
        if(isset($_POST['save_event_dialog'])) $action['action'] = "saveEventDialog";
        if(isset($_POST['abort_event_dialog'])) $action['action'] = "abortEventDialog";
        if(isset($_POST['password_cancel'])){
            $action['action'] = "passwordCancel";
        }elseif((count($this->pwd_change_queue) || isset($_POST['password_finish']) || isset($_POST['refreshProposal']))){
            $action['action'] = "passwordQueue";
        }
        return($action);
    }


    function editEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {
        $str = management::editEntry($action,$target);
        if($str) return($str);

        if(preg_match("/^edit_/",$action)){
            $tab = preg_replace("/^edit_/","",$action); 
            if(isset($this->tabObject->by_object[$tab])){
                $this->tabObject->current = $tab;
            }else{
                trigger_error("Unknown tab: ".$tab);
            }
        }

        // Enable template mode if this is a gosaUserTemplate
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        if($ldap->object_match_filter($this->tabObject->dn, "(objectClass=gosaUserTemplate)")){
            $this->tabObject->set_template_mode ();
        }
    }


    function closeDialogs()
    {
        management::closeDialogs();
        $this->pwd_change_queue = array();
    }


    /*! \brief  Sends a message to a set of users using gosa-si events.
     */ 
    function sendMessage($action="",$target=array(),$all=array())
    {
      return;
    }


    /*! \brief  Sends a message to a set of users using gosa-si events.
     */ 
    function saveEventDialog()
    {
        $this->dialogObject->save_object();
        $msgs = $this->dialogObject->check();
        if(count($msgs)){
            msg_dialog::displayChecks($msgs);
        }
    }


    /*! \brief  Intiates template creation. 
     */ 
    function newTemplate($action,$entry)
    {
        $this->newEntry();
        $this->tabObject->set_template_mode ();
    }


    /*! \brief  Queues a set of users for password changes
     */ 
    function changePassword($action="",$target=array(),$all=array())
    {
        $this->dn ="";
        $this->pwd_change_queue = $target;

        // Check permisions
        $disallowed = array();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);

        foreach($this->pwd_change_queue as $key => $dn){

            if(!preg_match("/w/",$this->ui->get_permissions($dn,$this->aclCategory."/password"))){
                unset($this->pwd_change_queue[$key]);
                $disallowed[] = $dn; 
            }
        }
        if(count($disallowed)){
            msg_dialog::display(_("Permission"),msgPool::permModify($disallowed),INFO_DIALOG);
        }

        // Now display change dialog.
        return($this->handlePasswordQueue()); 
    }



    function refreshProposal()
    {
        $this->proposal = passwordMethod::getPasswordProposal($this->config);
        $this->proposalEnabled = (!empty($this->proposal));
    }


    function handlePasswordQueue()
    {
        /* Password checks */
        $pwLength = "";
        $check_special = true;
        $check_lower = true;
        $check_upper = true;
        $check_digits = true;

        // skip if nothing is to do
        if(empty($this->dn) && !count($this->pwd_change_queue)) return;

        // Refresh proposal if requested
        if(isset($_POST['refreshProposal'])) $this->refreshProposal();
        if(isset($_POST['proposalSelected'])) $this->proposalSelected = get_post('proposalSelected') == 1;

        $this->enforcePasswordChange = isset($_POST['new_password']) && isset($_POST['enforcePasswordChange']);

        $smarty = get_smarty();
        $smarty->assign("proposal" , set_post($this->proposal));
        $smarty->assign("proposalEnabled" , $this->proposalEnabled);
        $smarty->assign("proposalSelected" , $this->proposalSelected);

        $smarty->assign("passwordChangeForceable" , $this->passwordChangeForceable);
        $smarty->assign("enforcePasswordChange" , $this->enforcePasswordChange);

        // Get next entry from queue.
        if(empty($this->dn) && count($this->pwd_change_queue)){

            // Generate new proposal
            $this->refreshProposal();
            $this->proposalSelected = ($this->proposal != "");
            $this->dn = array_pop($this->pwd_change_queue);

            // Check if we are able to enforce a password change
            $ldap = $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->cat($this->dn);
            $attrs = $ldap->fetch();
            $this->passwordChangeForceable = 
                    in_array_strict('sambaAccount', $attrs['objectClass']) || 
                    (in_array_strict('posixAccount', $attrs['objectClass']) && isset($attrs['shadowMax']));
            $smarty->assign("passwordChangeForceable" , $this->passwordChangeForceable);
            $smarty->assign("enforcePasswordChange" , $this->enforcePasswordChange);

            // Assign proposal variables
            $smarty->assign("proposal" , set_post($this->proposal));
            $smarty->assign("proposalEnabled" , $this->proposalEnabled);
            $smarty->assign("proposalSelected" , $this->proposalSelected);

            set_object_info($this->dn);
            return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }elseif(!count($this->pwd_change_queue) && empty($this->dn)){
            return;
        }

        // If we've just refreshed the proposal then do not check the password for validity.
        if(isset($_POST['refreshProposal'])){
            return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
        }

        // Check permissions
        if(isset($_POST['password_finish'])){

            $dn  = $this->dn;
            $acl = $this->ui->get_permissions($dn, "users/password");
            $cacl= $this->ui->get_permissions($dn, "users/user");
            print($this->dn);
            echo '<pre>';
            var_dump($this->ui->get_permissions($dn, "users/password"));
            echo '</pre>';
            if (preg_match('/w/', $acl) || preg_match('/c/', $cacl)){

                // Get posted passwords
                if($this->proposalSelected){
                    $new_password = $this->proposal;
                    $repeated_password = $this->proposal;
                }else{
                    $new_password = get_post('new_password');
                    $repeated_password = get_post('repeated_password');
                }

                // Check posted passwords now.            
                $message= array();
                if ($new_password != $repeated_password){
                    $message[]= _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
                } else {
                    if ($new_password == ""){
                        $message[] = msgPool::required(_("New password"));
                    }
                }

                // Call external check hook to validate the password change
                if(!count($message)){
                    $attrs = array();
                    $attrs['current_password'] = '';
                    $attrs['new_password'] = $new_password;
                    $checkRes = password::callCheckHook($this->config,$this->dn,$attrs);
                    if(count($checkRes)){
                        $message[] = sprintf(_("Check-hook reported a problem: %s. Password change canceled!"), 
                                implode($checkRes));
                    }
                }

                // Display errors
                if (count($message) != 0){
                    msg_dialog::displayChecks($message);
                    return($smarty->fetch(get_template_path('password.tpl', TRUE)));
                }



                // Change password 
                if(isset($this->force_hash_type[$this->dn])){
                    if(!change_password ($this->dn, $new_password,0,$this->force_hash_type[$this->dn],'', $message)){
                        msg_dialog::displayChecks(array($message));
                        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
                    }
                }else{
                    if(!change_password ($this->dn, $new_password,0,'','',$message)){
                        msg_dialog::displayChecks(array($message));
                        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
                    }
                }
        
                // The user has to change his password on next login
                // - We are going to update samba and posix attributes here, to enforce
                //   such a password change.
                if($this->passwordChangeForceable && $this->enforcePasswordChange){

                    // Check if we are able to enforce a password change
                    $ldap = $this->config->get_ldap_link();
                    $ldap->cd($this->config->current['BASE']);
                    $ldap->cat($this->dn);
                    $attrs = $ldap->fetch();
                    $samba = in_array_strict('sambaSamAccount', $attrs['objectClass']);
                    $posix = in_array_strict('posixAccount', $attrs['objectClass']);

                    // Update the posix shadow flag...
                    if($posix){

                        $current= floor(date("U") /60 /60 /24);
                        $enforceDate = $current -  $attrs['shadowMax'][0];
                        $new_attrs = array();
                        $new_attrs['shadowLastChange'] = $enforceDate;
                        $ldap->cd($this->dn);
                        $ldap->modify($new_attrs);

                        # $posixAccount = new posixAccount($this->config, $this->dn);
                        # $posixAccount->is_modified=TRUE;
                        # $posixAccount->activate_shadowExpire=1;
                        # $posixAccount->shadowExpire = date('d.m.Y', time() - (1 * 24 * 60 *60));
                        # $posixAccount->save();
                    }
                   
                    // Update the samba kickoff flag...
                    if($samba){
                        $sambaAccount = new sambaAccount($this->config, $this->dn);
                        $sambaAccount->is_modified=TRUE;
                        $sambaAccount->flag_enforcePasswordChange = TRUE;
                        $sambaAccount->flag_cannotChangePassword = FALSE;
                        $sambaAccount->save();
                    }
                }

                new log("modify","users/".get_class($this),$this->dn,array(),"Password has been changed");
                $this->dn ="";

            } // else {
            //     msg_dialog::display(_("Password change"),
            //             _("You have no permission to change this users password!"),
            //             WARNING_DIALOG);
            // }
        }

        // // Cleanup
        // if(!count($this->pwd_change_queue) && $this->dn=""){
        //     $this->remove_lock();
        //     $this->closeDialogs();
        // }else{
        //     return($this->handlePasswordQueue());
        // }
    }


    /*! \brief  Save user modifications. 
     *          Whenever we save a 'new' user, request a password change for him.
     */ 
    function saveChanges()
    {
        $str = management::saveChanges();

        if(!empty($str)) return($str);

        if($this->last_tabObject instanceOf multi_plug){
            foreach($this->last_tabObject->a_handles as $user){
                if($user->password_change_needed()){
                    $this->force_hash_type[$user->dn] = $user->by_object['user']->pw_storage;
                    $this->pwd_change_queue[] = $user->dn;
                }
            }
            return($this->handlePasswordQueue());
        }

        if(isset($this->last_tabObject->by_object['user']) && $this->last_tabObject->by_object['user']->password_change_needed()){
            $this->force_hash_type[$this->last_tabObject->dn] = $this->last_tabObject->by_object['user']->pw_storage;
            $this->pwd_change_queue[] = $this->last_tabObject->dn;
            return($this->handlePasswordQueue());
        }
    }

    function cancelEdit()
    {
        $str = management::cancelEdit();
        if(!empty($str)) return($str);

        if(isset($this->last_tabObject->by_object['user']) && 
                $this->last_tabObject->by_object['user']->dn != "new" &&  
                $this->last_tabObject->by_object['user']->password_change_needed()){
            $this->force_hash_type[$this->last_tabObject->dn] = $this->last_tabObject->by_object['user']->pw_storage;
            $this->pwd_change_queue[] = $this->last_tabObject->dn;
            return($this->handlePasswordQueue());
        }
    }


    /*! \brief  Intiates user creation. 
     *          If we've user templates, then the user will be asked to use to use one. 
     *          -> See 'templateContinue' for further handling.
     */ 
    function newUserFromTemplate($action="",$target=array(),$all=array())
    {
        // Call parent method, it knows whats to do, locking and so on ...
        $str = management::newEntry($action,$target,$all);
        if(!empty($str)) return($str);

        // Reset uid selection.
        $this->got_uid= "";

        // Use template if there are any of them 
        $templates = array();
        $templates['none']= _("none");
        $templates = array_merge($templates,$this->get_templates());

        // We've templates, so preset the current template and display the input dialog.
        if (count($templates)){
            $smarty = get_smarty();
            foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
                $smarty->assign("$attr", "");
            }
            $smarty->assign("template",  array_pop($target));
            $smarty->assign("templates", $templates);
            $smarty->assign("edit_uid", "");
            $smarty->assign("allowUidProposalModification", $this->config->get_cfg_value("core","allowUidProposalModification"));
            return($smarty->fetch(get_template_path('template.tpl', TRUE)));

            // -> See 'templateContinue' for further handling!
        }
    }



    /*! \brief  Intiates user creation. 
     *          If we've user templates, then the user will be asked 
     *           if he wants to use one. 
     *          -> See 'templateContinue' for further handling.
     */ 
    function newEntry($action="",$target=array(),$all=array(), $altTabClass ="", $altTabType = "", $altAclCategory="")
    {

        // Call parent method, it manages everything, locking, object creation...
        $str = management::newEntry($action,$target,$all);
        if(!empty($str)) return($str);

        // If we've at least one template, then ask the user if he wants to use one?
        $templates = array();
        $templates['none']= _("none");
        $templates = array_merge($templates,$this->get_templates());

        // Display template selection
        if (count($templates) > 1){
            $smarty = get_smarty();

            // Set default variables, normally empty.
            foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
                $smarty->assign($attr, "");
            }
            $smarty->assign("template", "none");
            $smarty->assign("templates", $templates);
            $smarty->assign("edit_uid", "");
            $smarty->assign("allowUidProposalModification", $this->config->get_cfg_value("core","allowUidProposalModification"));
            return($smarty->fetch(get_template_path('template.tpl', TRUE)));

            // -> See 'templateContinue' for further handling!
        }
    }


    /* !\brief  This method is called whenever a template selection was displayed.
     *          Here we act on the use selection. 
     *          - Does the user want to create a user from template?
     *          - Create user without template?
     *          - Input correct, every value given and valid? 
     */ 
    function templateContinue()
    {
        // Get the list of available templates.
        $templates = array();
        $templates['none']= _("none");
        $templates = array_merge($templates,$this->get_templates());

        // Input validation, if someone wants to create a user from a template
        //  then validate the given values.
        $message = array();
        if(!isset($_POST['template']) || (empty($_POST['template']))){
            $message[]= msgPool::invalid(_("Template"));
        }
        if(!isset($_POST['sn']) || (empty($_POST['sn']))){
            $message[]= msgPool::required(_("Name"));
        }
        if(!isset($_POST['givenName']) || (empty($_POST['givenName']))){
            $message[]= msgPool::required(_("Given name"));
        }

        /********************
         * 1   We've had input errors - Display errors and show input dialog again. 
         ********************/

        if (count($message) > 0){
            msg_dialog::displayChecks($message);

            // Preset input fields with user input. 
            $smarty = get_smarty();
            foreach(array("sn", "givenName", "uid", "template") as $attr){
                if(isset($_POST[$attr])){
                    $smarty->assign("$attr", set_post(get_post($attr)));
                }else{
                    $smarty->assign("$attr", "");
                }
            }

            $smarty->assign("templates",$templates);
            $smarty->assign("got_uid", $this->got_uid);
            $smarty->assign("edit_uid",false);
            $smarty->assign("allowUidProposalModification", $this->config->get_cfg_value("core","allowUidProposalModification"));
            return($smarty->fetch(get_template_path('template.tpl', TRUE)));
        }

        /********************
         * 2   There was a template selected, now ask for the uid.
         ********************/

        if ($_POST['template'] != 'none' && !isset($_POST['uid'])){

            // Remember user input.
            $smarty = get_smarty();
            $this->sn             = get_post('sn');
            $this->givenName      = get_post('givenName');

            // Avoid duplicate entries, check if such a user already exists.
            $dn= preg_replace("/^[^,]+,/i", "", get_post('template'));
            $ldap= $this->config->get_ldap_link();
            $ldap->cd ($dn);
            $ldap->search ("(&(sn=".normalizeLdap($this->sn).")(givenName=".normalizeLdap($this->givenName)."))", array("givenName"));

            if ($ldap->count () != 0){
                msg_dialog::displayChecks(array(msgPool::duplicated(_("Name"))));
                $smarty->assign("edit_uid", "");
            }else{

                // Preset uid field by using the idGenerator 
                $attributes= array('sn' => $this->sn, 'givenName' => $this->givenName);
                if ($this->config->get_cfg_value("core","idGenerator") != ""){
                    $genStr = $this->config->get_cfg_value("core","idGenerator");
                    $smarty->assign("edit_uid", "");
                    if(!empty($genStr)){
                        $uids= gen_uids($genStr, $attributes);
                        if (count($uids)){
                            $smarty->assign("edit_uid", "false");
                            $smarty->assign("uids", $uids);
                            $this->uid= current($uids);
                        }else{
                            msg_dialog::displayChecks(array(_("Cannot generate a unique id, please specify it manually!")));
                        }
                    }
                } else {
                    $smarty->assign("edit_uid", "");
                    $this->uid= "";
                }
                $this->got_uid= true;
            }

            // Assign user input 
            foreach(array("sn", "givenName", "uid", "got_uid") as $attr){
                $smarty->assign("$attr", set_post($this->$attr));
            }
            if (isset($_POST['template'])){
                $smarty->assign("template", get_post('template'));
            }
            $smarty->assign("templates",$templates); 
            $smarty->assign("allowUidProposalModification", $this->config->get_cfg_value("core","allowUidProposalModification"));
            return($smarty->fetch(get_template_path('template.tpl', TRUE)));
        }


        /********************
         * 3   No template - Ok. Lets fill the data into the user object and skip templating here. 
         ********************/
        if (get_post('template') == 'none'){
            foreach(array("sn", "givenName", "uid") as $attr){
                if (isset($_POST[$attr])){
                    $this->tabObject->by_object['user']->$attr= get_post($attr);
                }
            }

            // The user Tab object is already instantiated, so just go back and let the 
            //  management class do the rest.
            return("");
        }


        /********************
         * 4   Template selected and uid given - Ok, then lets adapt tempalte values. 
         ********************/
        if(isset($_POST['uid'])){

            // Move user supplied data to sub plugins 
            foreach(array("uid","sn","givenName") as $attr){
                $this->$attr = get_post($attr);
                $this->tabObject->$attr       = $this->$attr;
                $this->tabObject->by_object['user']->$attr = $this->$attr;
            }

            // Adapt template values.
            $template_dn              = get_post('template');
            $this->tabObject->adapt_from_template($template_dn, array("uid","cn","givenName","sn"));
            $template_base            = preg_replace("/^[^,]+,".preg_quote(get_people_ou(), '/i')."/", '', $template_dn);
            $this->tabObject->by_object['user']->base= $template_base;

            // The user Tab object is already instantiated, so just go back and let the 
            //  management class do the rest.
            return("");
        }
    }


    /* !\brief  This method applies a template to a set of users.
     */ 
    function templatizeUsers($action="",$target=array(),$all=array())
    {
        $this->dns = array();
        if(count($target)){

            // Get the list of available templates.
            $templates = $this->get_templates();

            // Check entry locking
            foreach($target as $dn){
                if (($user= get_lock($dn)) != ""){
                    $this->dn = $dn;
                    return(gen_locked_message ($user, $dn));
                }
                $this->dns[] = $dn;
            }

            // Display template
            $smarty = get_smarty();
            $smarty->assign("templates", $templates);
            return($smarty->fetch(get_template_path('templatize.tpl', TRUE)));
        }
    }


    /* !\brief  This method is called whenever the templatize dialog was used.
     */ 
    function templatizeContinue()
    {
        // Template readable? 
        $template= get_post('template');
        $acl = $this->ui->get_permissions($template, $this->aclCategory."/".$this->aclPlugin);
        if (preg_match('/r/', $acl)){
            $tab = $this->tabClass;
            foreach ($this->dns as $dn){

                // User writeable
                $acl = $this->ui->get_permissions($dn,  $this->aclCategory."/".$this->aclPlugin);
                if (preg_match('/w/', $acl)){
                    $this->tabObject= new $tab($this->config, $this->config->data['TABS'][$this->tabType], $dn, $this->aclCategory);
                    $this->tabObject->adapt_from_template($template, array("sn", "givenName", "uid"));
                    $this->tabObject->save();
                } else {
                    msg_dialog::display(_("Permission error"), msgPool::permModify($dn), ERROR_DIALOG);
                }
            }
        } else {
            msg_dialog::display(_("Permission error"), msgPool::permView($template), ERROR_DIALOG);
        }

        // Cleanup!
        $this->remove_lock(); 
        $this->closeDialogs();
    }


    /* !\brief  Lock/unlock multiple users.
     */ 
    function lockUsers($action,$target,$all)
    {
        if(!count($target)) return;
        if($action == "lockUsers"){
            $this->lockEntry($action,$target, $all, "lock");
        }else{
            $this->lockEntry($action,$target, $all, "unlock");
        }
    }


    /* !\brief  Locks/unlocks the given user(s).
     */ 
    function lockEntry($action,$entry, $all, $type = "toggle")
    {

        // Filter out entries we are not allowed to modify
        $disallowed = array();
        $dns = array();
        foreach($entry as $dn){
            if (!preg_match("/w/",$this->ui->get_permissions($dn,"users/password"))){
                $disallowed[] = $dn;
            }else{
                $allowed[] = $dn;
            }
        }
        if(count($disallowed)){
            msg_dialog::display(_("Permission"),msgPool::permDelete($disallowed),INFO_DIALOG);
        }

        // Try to lock/unlock the rest of the entries.
        $ldap = $this->config->get_ldap_link();
        foreach($allowed as $dn){
            $ldap->cat($dn, array('userPassword'));
            if($ldap->count() == 1){

                // We can't lock empty passwords.
                $val = $ldap->fetch();
                if(!isset($val['userPassword'])){
                    continue;
                }

                // Detect the password method and try to lock/unlock.
                $pwd = $val['userPassword'][0];
                $method = passwordMethod::get_method($pwd,$val['dn']);
                $success= true;
                if($method instanceOf passwordMethod){
                    if($type == "toggle"){
                        if($method->is_locked($this->config,$val['dn'])){
                            $success= $method->unlock_account($this->config,$val['dn']);
                        }else{
                            $success= $method->lock_account($this->config,$val['dn']);
                        }
                    }elseif($type == "lock" && !$method->is_locked($this->config,$val['dn'])){
                        $success= $method->lock_account($this->config,$val['dn']);
                    }elseif($type == "unlock" && $method->is_locked($this->config,$val['dn'])){
                        $success= $method->unlock_account($this->config,$val['dn']);
                    }

                    // Check if everything went fine.
                    if (!$success){
                        $hn= $method->get_hash_name();
                        if (is_array($hn)){
                            $hn= $hn[0];
                        }
                        msg_dialog::display(_("Account locking"),
                                sprintf(_("Password method '%s' does not support locking. Account (%s) has not been locked!"), 
                                    $hn,$dn),WARNING_DIALOG);
                    }
                }else{
                    // Can't lock unknown methods.
                }
            }
        }
    }


    /* !\brief  This method returns a list of all available templates.
     */ 
    function get_templates()
    {
        $templates= array();
        $ldap= $this->config->get_ldap_link();
        foreach ($this->config->departments as $key => $value){
            $acl = $this->ui->get_permissions($value,$this->aclCategory."/".$this->aclPlugin);
            if (preg_match("/c/",$acl)){

                // Search all templates from the current dn.
                $ldap->cd (get_people_ou().$value);
                $ldap->search ("(objectClass=gosaUserTemplate)", array("uid"));
                if ($ldap->count() != 0){
                    while ($attrs= $ldap->fetch()){
                        $templates[$ldap->getDN()]= $attrs['uid'][0]." - ".LDAP::fix($key);
                    }
                }
            }
        }
        natcasesort ($templates);
        reset ($templates);
        return($templates);
    }


    function copyPasteHandler($action="",$target=array(),$all=array(),
            $altTabClass ="", $altTabType = "", $altAclCategory="",$altAclPlugin="")
    {
        if ($this->config->boolValueIsTrue("core", "copyPaste")){
            $this->cpHandler->lastdn = "";
            $str = management::copyPasteHandler($action,$target,$all);
            if($this->cpHandler->lastdn != "" && isset($_POST['passwordTodo']) && $_POST['passwordTodo'] == "new"){
                $this->pwd_change_queue[] = $this->cpHandler->lastdn;
                return($this->handlePasswordQueue());
            }
            return($str);
        }

        return "";
    }


    static function filterLockImage($userPassword)
    {
        $image= "images/empty.png";
        if(isset($userPassword[0]) && preg_match("/^\{[^\}]/",$userPassword[0])){
            if(preg_match("/^[^\}]*+\}!/",$userPassword[0])){
                $image= "images/lists/locked.png";
            }else{
                $image= "images/lists/unlocked.png";
            }
        }
        return $image;
    }


    static function filterLockLabel($userPassword)
    {
        $label= "";
        if(isset($userPassword[0]) && preg_match("/^\{[^\}]/",$userPassword[0])){
            if(preg_match("/^[^\}]*+\}!/",$userPassword[0])){
                $label= _("Unlock account")."<rowClass:entry-locked/><rowLabel:locked/>";
            }else{
                $label= _("Lock account");
            }
        }
        return $label;
    }


    static function filterProperties($row, $classes)
    {
        $result= "";
        $map= array( "gosaAccount" => array( "image" => "plugins/users/images/select_user.png",
                    "plugin" => "user",
                    "alt" => _("Generic"),
                    "title" => _("Edit generic properties")),
                "posixAccount" => array("image" => "images/penguin.png",
                    "plugin" => "posixAccount",
                    "alt" => _("POSIX"),
                    "title" => _("Edit POSIX properties")),
                "gosaMailAccount" => array("image" => "images/mailto.png",
                    "alt" => _("Mail"),
                    "plugin" => "mailAccount",
                    "title" => _("Edit mail properties")),
                "sambaSamAccount" => array("image" => "plugins/systems/images/select_winstation.png",
                    "plugin" => "sambaAccount",
                    "alt" => _("Samba"),
                    "title" => _("Edit samba properties")),
                "apple-user" => array("image" => "plugins/netatalk/images/select_netatalk.png",
                    "plugin" => "netatalk",
                    "alt" => _("Netatalk"),
                    "title" => _("Edit Netatalk properties")),
                "gotoEnvironment" => array("image" => "plugins/users/images/small_environment.png",
                    "plugin" => "environment",
                    "alt" => _("Environment"),
                    "title" => _("Edit environment properties")),
                "goFaxAccount" => array("image" => "plugins/users/images/fax_small.png",
                        "plugin" => "gofaxAccount",
                        "alt" => _("FAX"),
                        "title" => _("Edit FAX properties")),
                "goFonAccount" => array("image" => "plugins/gofon/images/select_phone.png",
                        "plugin" => "phoneAccount",
                        "alt" => _("Phone"),
                        "title" => _("Edit phone properties")),
              );

        // Walk thru map
        foreach ($map as $oc => $properties) {
            if (in_array_ics($oc, $classes)) {
                $result.= image($properties['image'], "listing_edit_".$properties['plugin']."_$row", $properties['title']);
            } else {
                $result.= image('images/empty.png');
            }
        }

        if(in_array_ics("zarafa-contact", $classes)){
          $result.= image("plugins/zarafa/images/zarafa_user_small.png", "listing_edit_zarafaAccount_$row", _("Edit Zarafa properties"));
        } else if(in_array_ics("zarafa-user", $classes)){
          $result.= image("plugins/zarafa/images/zarafa_user_small.png", "listing_edit_zarafaAccount_$row", _("Edit Zarafa properties"));
        }

        return $result;
    }

} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
