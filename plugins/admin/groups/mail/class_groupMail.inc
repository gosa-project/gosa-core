<?php

class mailgroup extends plugin
{
    /* Multiple edit */
    var $gosaMailForwardingAddress_Some  = array();  // Used in multiple edit 

    /* Default values */
    var $mail                       = "";           // Default mail address 
    var $gosaMailAlternateAddress   = array();  // Set default Alternate Mail Adresses to empty array
    var $gosaMailForwardingAddress  = array();  // Forwarding also empty
    var $gosaMailServer             = "";       // Selected mailserver 
    var $gosaMailQuota              = "";       // Defined Quota 
    var $gosaVacationMessage        = "";       // Vocation message 
    var $gosaSpamSortLevel          = "";     
    var $gosaSpamMailbox            = "";
    var $gosaSharedFolderTarget     ;
    var $gosaMailDeliveryMode       = "[L        ]";   // 
    var $gosaMailMaxSize            = "";       // 
    var $FolderType                 = array("CAT" => '', "SUB_CAT" => '');

    var $quotaUsage                 = -1;        // -1 Means undefined

    /* Internal */
    var $AclTypes                   = array();
    var $members                    = array();  // Group members
    var $mailusers                  = array();  // Group member with mail account
    var $folder_acls                = array();
    var $MailMethod = NULL; 
    var $mailAddressSelect             = FALSE;    
    var $remove_folder_from_imap    = true;
    var $view_logged                = FALSE;
    var $mailDomainPart             = "";

    /* attribute list for save action */
    var $attributes= array( "mail",   "gosaMailServer", "gosaMailQuota", "gosaMailMaxSize",
            "gosaMailAlternateAddress", "gosaMailForwardingAddress",
            "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox",
            "acl","gosaSharedFolderTarget", "gosaVacationMessage");

    var $objectclasses= array("gosaMailAccount");
    var $multiple_support = FALSE; // Not tested yet

    var $uid = "";
    var $cn ="";
    var $orig_cn = "";
    var $show_effective_memeber = FALSE;

    var $aclPostToId = array();

    function __construct (&$config, $dn= NULL, $base_object= NULL)
    {
        plugin::__construct($config, $dn);

        /* Get attributes from parent object
         */
        foreach(array("uid","cn") as $attr){
            if(isset($this->parent->by_object['group']) && isset($this->parent->by_object['group']->$attr)){
                $this->$attr = $this->parent->by_object['group']->$attr;
            }elseif(isset($this->attrs[$attr])){
                $this->$attr = $this->attrs[$attr][0];
            }
        }
        $this->orig_cn = $this->uid = $this->cn;

        /* Intialize the used mailMethod
         */
        $tmp = new mailMethod($config,$this,"group");
        $this->mailMethod           = $tmp->get_method();
        $this->mailMethod->fixAttributesOnLoad();
        $this->mailDomainParts      = $this->mailMethod->getMailDomains();
        $this->AvailableFolderTypes = $this->mailMethod->getAvailableFolderTypes();
        $this->MailBoxes = array();

        /* Remember account status
         */
        $this->initially_was_account = $this->is_account;

        /* While we are not not allowed to modify the mail address
         *  and this is a new mail account, preset the user part of the 
         *  mail address with the accounts cn.
         */ 
        if(!$this->mailMethod->isModifyableMail() && !$this->initially_was_account){
            $this->mail = $base_object->cn;
        }

        /* Load folder_acls with defaults.
           anyone -- The default acl, will be written to ldap.
           member -- The ACL used for the members.
         */ 
        $this->folder_acls = $this->mailMethod->getDefaultACLs();

        /* Load acls
           The most used acl will be used as member acl, this
           shortens the listed acls.        
           This may be merged/overwritten by the mail methods.
         */
        $ldap = $this->config->get_ldap_link();
        if(isset($this->attrs['acl'])){
            for($i = 0; $i < $this->attrs['acl']['count'] ; $i++){

                /* Be carefull here, since kolab22 uses spaces in the acls (herbert read anon/post)
                 */
                $str = $this->attrs['acl'][$i];
                list($name, $acl) = preg_split("/[ ]{1}/", $str, 2);

                if($name == "anyone") $name = "__anyone__";
                $this->folder_acls[$name] = $acl;
            }
        }

        /* Initialize configured values
         */
        if($this->is_account){
            if($this->mailMethod->connect() && $this->mailMethod->account_exists()){

                /* Read quota */
                $this->gosaMailQuota = $this->mailMethod->getQuota($this->gosaMailQuota);
                $this->quotaUsage = $this->mailMethod->getQuotaUsage($this->quotaUsage);
                if($this->mailMethod->is_error()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot read quota settings: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }

                /* Read mailboxes */
                $this->MailBoxes = $this->mailMethod->getMailboxList($this->MailBoxes);
                if($this->mailMethod->is_error()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot get list of mailboxes: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }

                /* Receive folder types */
                $this->FolderType = $this->mailMethod->getFolderType($this->FolderType);
                if($this->mailMethod->is_error()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot receive folder types: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }

                /* Receive permissions */  
                $this->folder_acls = $this->mailMethod->getFolderACLs($this->folder_acls);
                if($this->mailMethod->is_error()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot receive folder permissions: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }

            }elseif(!$this->mailMethod->is_connected()){
                msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"),
                            $this->mailMethod->get_error()), ERROR_DIALOG);
            }elseif(!$this->mailMethod->account_exists()){
                msg_dialog::display(_("Mail error"), sprintf(_("Mailbox '%s' doesn't exists on mail server: %s"),
                            $this->mailMethod->get_account_id(),$this->gosaMailServer), ERROR_DIALOG);
            }

            /* If the doamin part is selectable, we have to split the mail address
             */
            if(!(!$this->mailMethod->isModifyableMail() && $this->is_account)){
                if($this->mailMethod->domainSelectionEnabled() || $this->mailMethod->mailEqualsCN()){
                    $this->mailDomainPart = preg_replace("/^[^@]*+@/","",$this->mail);
                    $this->mail = preg_replace("/@.*$/","\\1",$this->mail);
                    if(!in_array_strict($this->mailDomainPart,$this->mailDomainParts)){
                        $this->mailDomainParts[] = $this->mailDomainPart;
                    }
                }
            }

            /* Load attributes containing arrays */
            foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
                $this->$val= array();
                if (isset($this->attrs["$val"]["count"])){
                    for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
                        array_push($this->$val, $this->attrs["$val"][$i]);
                    }
                }
            }
        }

        /* Disconnect mailMethod. Connect on demand later.
         */
        $this->mailMethod->disconnect();
        $this->AclTypes = $this->mailMethod->getAclTypes();

        /* Summarize most used ACLs as member acl 
         */
        if(count($this->folder_acls) > 2){
            $acl_usage = array();
            $most_acl = $this->folder_acls['__member__'];
            $most_cnt = 0;
            $member = $this->get_member();
            foreach($this->folder_acls as $user => $acl){
                if(preg_match("/^__/",$user)) continue;
                if(!in_array_strict($user,$member['mail'])) continue; 
                if(!isset($acl_usage[$acl])) $acl_usage[$acl]=0;
                $acl_usage[$acl] ++;
                if($acl_usage[$acl] > $most_cnt){
                    $most_cnt = $acl_usage[$acl];
                    $most_acl = $acl;
                }
            }
            $this->folder_acls['__member__'] = $most_acl;  
            foreach($this->folder_acls as $name => $acl){
                if(preg_match("/^__/",$name)) continue;
                if($acl == $most_acl && in_array_strict($name,$member['mail'])){
                    unset($this->folder_acls[$name]);
                }
            }
        }

        /* Get global filter config */
        if (!session::is_set("gmailfilter")){
            $ui= get_userinfo();
            $base= get_base_from_people($ui->dn);
            $gmailfilter= array( "depselect"       => $base,
                    "muser"            => "",
                    "regex"           => "*");
            session::set("gmailfilter", $gmailfilter);
        }
    }


    /*! \brief  Returns all group members once with 'dn' and once with 'mail'.
      This function is used to summarize ACLs by member acls.
      @return Array   Containing all members, with mail and dn
     */ 
    function get_member()
    {
        $member = array('all' => array(), 'mail' => array());
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        if(isset($this->parent->by_object['group'])){
            foreach($this->parent->by_object['group']->memberUid as $uid){
                if(!isset($this->parent->by_object['group']->dnMapping[$uid])) continue;
                $dn = $this->parent->by_object['group']->dnMapping[$uid];
                $member['all'][$uid] = $uid;
                if($ldap->object_match_filter($dn,"(&(objectClass=gosaMailAccount)(".$this->mailMethod->getUAttrib()."=*))")){
                    $ldap->cat($dn);
                    $attrs = $ldap->fetch();
                    $member['mail'][$uid] = $attrs[$this->mailMethod->getUAttrib()][0]; 
                }
            }
        }else{
            if(!isset($this->attrs['memberUid'])) return($member);
            $uattrib = $this->mailMethod->getUAttrib();
            $users = get_list("(&(objectClass=person)(objectClass=gosaAccount)(uid=*))",
                    "users",$this->config->current['BASE'],
                    array("uid","objectClass",$uattrib),GL_SUBSEARCH | GL_NO_ACL_CHECK);
            foreach($users as $user){
                $member['all'][$user['uid'][0]] = $user['dn'];
                if(isset($user[$uattrib]) 
                        && in_array_strict("gosaMailAccount",$user['objectClass']) 
                        && (in_array_strict($user['uid'][0], $this->attrs['memberUid']))){
                    $member['mail'][$user['uid'][0]] = $user[$uattrib][0];
                }
            }
        }
        return($member);
    }


    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Log view */
        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","groups/".get_class($this),$this->dn);
        }

        /****************
          Account status
         ****************/

        if(!$this->multiple_support_active){

            if(isset($_POST['modify_state'])){
                if($this->is_account && $this->acl_is_removeable() && $this->mailMethod->accountRemoveAble()){
                    $this->is_account= FALSE;
                }elseif(!$this->is_account && $this->acl_is_createable() && $this->mailMethod->accountCreateable()){
                    $this->is_account= TRUE;
                }
            }

            if ($this->is_account){
                $reason = "";
                if(!$this->mailMethod->accountRemoveable($reason)){
                    $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Mail")),$reason ,TRUE,TRUE);
                }else{
                    $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Mail")),msgPool::
                            featuresEnabled(_("Mail")));
                }
            } else {
                $reason = "";
                if(!$this->mailMethod->accountCreateable($reason)){
                    $display= $this->show_disable_header(msgPool::addFeaturesButton(_("Mail")),$reason ,TRUE,TRUE);
                }else{
                    $display= $this->show_disable_header(msgPool::addFeaturesButton(_("Mail")),msgPool::
                            featuresDisabled(_("Mail")));

                    /* Show checkbox that allows us to remove imap entry too*/
                    if($this->initially_was_account){
                        $c = "";
                        if($this->remove_folder_from_imap){
                            $c= " checked ";
                        }
                        $display .= "<h3>Shared folder delete options</h3>
                            <input class='center' type='checkbox' name='remove_folder_from_imap' value='1' ".$c."
                            title='"._("Remove shared folder from mail server database when entry gets removed in LDAP")."'>";
                        $display .= _("Remove the shared folder and all its contents after saving this account");
                    }
                }
                return ($display);
            }
        }


        /****************
          Preset mail attribute
         ****************/
        if(empty($this->mail) && $this->mailMethod->mailEqualsCN() && !$this->initially_was_account){
            if($this->mailMethod->domainSelectionEnabled()){
                $this->mail = &$this->parent->by_object['group']->cn;
            }
        }


        /****************
          Forward addresses
         ****************/

        if (isset($_POST['add_local_forwarder'])){
            $this->mailAddressSelect= new mailAddressSelect($this->config, get_userinfo());
            $this->dialog= TRUE;
        }

        if (isset($_POST['mailAddressSelect_cancel'])){
            $this->mailAddressSelect= FALSE;
            $this->dialog= FALSE;
        }

        if (isset($_POST['mailAddressSelect_save'])){
            if($this->acl_is_writeable("gosaMailForwardingAddress")){
                $list = $this->mailAddressSelect->save();
                foreach ($list as $entry){
                    $val = $entry['mail'][0];
                    if (!in_array_strict($val, $this->gosaMailAlternateAddress) && $val != $this->mail){
                        $this->addForwarder($val);
                        $this->is_modified= TRUE;
                    }
                }
                $this->mailAddressSelect= FALSE;
                $this->dialog= FALSE;
            } else {
                msg_dialog::display(_("Error"), _("Please select an entry!"), ERROR_DIALOG);
            }
        }

        if($this->mailAddressSelect instanceOf mailAddressSelect){
            $used  = array();
            $used['mail'] = array_values($this->gosaMailAlternateAddress);
            $used['mail'] = array_merge($used['mail'], array_values($this->gosaMailForwardingAddress));
            $used['mail'][] = $this->mail;

            // Build up blocklist
            session::set('filterBlacklist', $used);
            return($this->mailAddressSelect->execute());
        }

        if (isset($_POST['add_forwarder'])){
            if ($_POST['forward_address'] != ""){
                $address= get_post('forward_address');
                $valid= FALSE;
                if (!tests::is_email($address)){
                    if (!tests::is_email($address, TRUE)){
                        if ($this->is_template){
                            $valid= TRUE;
                        } else {
                            msg_dialog::display(_("Error"), msgPool::invalid(_("Mail address"),
                                        "","","your-address@your-domain.com"),ERROR_DIALOG);
                        }
                    }
                } elseif ($address == $this->mail
                        || in_array_strict($address, $this->gosaMailAlternateAddress)) {
                    msg_dialog::display(_("Error"),_("Cannot add primary address to the list of forwarders!") , ERROR_DIALOG);
                } else {
                    $valid= TRUE;
                }
                if ($valid){
                    if($this->acl_is_writeable("gosaMailForwardingAddress")){
                        $this->addForwarder ($address);
                        $this->is_modified= TRUE;
                    }
                }
            }
        }
        if (isset($_POST['delete_forwarder']) && isset($_POST['forwarder_list'])){
            $this->delForwarder ($_POST['forwarder_list']);
        }


        /****************
          Alternate addresses
         ****************/

        if (isset($_POST['add_alternate'])){
            $valid= FALSE;
            if (!tests::is_email(get_post('alternate_address'))){
                if ($this->is_template){
                    if (!(tests::is_email(get_post('alternate_address'), TRUE))){
                        msg_dialog::display(_("Error"),msgPool::invalid(_("Mail address"),
                                    "","","your-domain@your-domain.com"),     ERROR_DIALOG);
                    } else {
                        $valid= TRUE;
                    }
                } else {
                    msg_dialog::display(_("Error"),msgPool::invalid(_("Mail address"),
                                "","","your-domain@your-domain.com"),       ERROR_DIALOG);
                }
            } else {
                $valid= TRUE;
            }
            if ($valid && ($user= $this->addAlternate (get_post('alternate_address'))) != ""){
                $ui= get_userinfo();
                $addon= "";
                if ($user[0] == "!") {
                    $addon= sprintf(_("Address is already in use by group '%s'."), mb_substr($user, 1));
                } else {
                    $addon= sprintf(_("Address is already in use by user '%s'."), $user);
                }
                msg_dialog::display(_("Error"), msgPool::duplicated(_("Mail address"))."<br><br><i>".
                        "$addon</i>", ERROR_DIALOG);
            }
        }
        if (isset($_POST['delete_alternate']) && isset($_POST['alternates_list'])){
            $this->delAlternate ($_POST['alternates_list']);
        }


        /****************
          SMARTY- Assign smarty variables
         ****************/

        /* Load templating engine */
        $smarty= get_smarty();
        $smarty->assign("initially_was_account", $this->initially_was_account);
        $smarty->assign("isModifyableMail", $this->mailMethod->isModifyableMail());
        $smarty->assign("isModifyableServer", $this->mailMethod->isModifyableServer());
        $smarty->assign("mailEqualsCN", $this->mailMethod->mailEqualsCN());
        $smarty->assign("folder_acls" , $this->postable_acls());
        $smarty->assign("AclTypes" ,    set_post($this->AclTypes));
        $smarty->assign("Effective",    $this->get_effective_member_acls());
        $smarty->assign("show_effective_memeber",    $this->show_effective_memeber);

        $smarty->assign("quotaEnabled", $this->mailMethod->quotaEnabled());
        if($this->mailMethod->quotaEnabled()){
            $smarty->assign("gosaMailQuota", set_post($this->gosaMailQuota));
            $smarty->assign("quotaUsage",   mailMethod::quota_to_image($this->quotaUsage,$this->gosaMailQuota)); 
        }

        $smarty->assign("MailDomains",set_post($this->mailDomainParts));
        $smarty->assign("MailDomain" ,set_post($this->mailDomainPart));
        $smarty->assign("MailServers",set_post($this->mailMethod->getMailServers()));
        $smarty->assign("allowSieveManagement", $this->mailMethod->allowSieveManagement());

        $smarty->assign("domainSelectionEnabled", $this->mailMethod->domainSelectionEnabled());
        $smarty->assign("folderTypesEnabled",$this->mailMethod->folderTypesEnabled());
        $smarty->assign("AvailableFolderTypes", set_post( $this->AvailableFolderTypes));
        $smarty->assign("FolderType", set_post($this->FolderType));

        if (is_numeric($this->gosaMailQuota) && $this->gosaMailQuota != 0){
            if($this->acl_is_readable("gosaMailQuota")){
                $smarty->assign("quotausage", progressbar(round(($this->quotaUsage * 100)/ $this->gosaMailQuota),100,15,true));
                $smarty->assign("quotadefined", "true");
            }else{
                $smarty->assign("quotadefined", "true");
                $smarty->assign("quotausage", "-");
            }
        } else {
            $smarty->assign("quotadefined", "false");
        }

        /* Assign acls */
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $name => $translation) {
            $smarty->assign($name."ACL",$this->getacl($name));
        }
        foreach($this->attributes as $name){
            $smarty->assign($name, set_post($this->$name));
        }



        $smarty->assign("mailServers", set_post($this->mailMethod->getMailServers()));
        if (preg_match("/I/", $this->gosaMailDeliveryMode)) {
            $smarty->assign("only_local", "checked");
        }else{
            $smarty->assign("only_local", "");
        }


        /******
          Multi edit support 
         ******/
        foreach($this->attributes as $attr){
            if(in_array_strict($attr,$this->multi_boxes)){
                $smarty->assign("use_".$attr,TRUE);
            }else{
                $smarty->assign("use_".$attr,FALSE);
            }
        }

        /* Multiple support handling */
        foreach(array("kolabFolderType") as $attr){
            if(in_array_strict($attr,$this->multi_boxes)){
                $smarty->assign("use_".$attr,TRUE);
            }else{
                $smarty->assign("use_".$attr,FALSE);
            }
        }

        $smarty->assign("Forward_all",     set_post($this->gosaMailForwardingAddress));
        $smarty->assign("Forward_some",    set_post($this->gosaMailForwardingAddress_Some));
        $smarty->assign("multiple_support",set_post($this->multiple_support_active));

        $display.= $smarty->fetch (get_template_path(check_for_materialize_theme() ? "mail_materialize.tpl" : 'mail.tpl', TRUE, dirname(__FILE__)));
        return ($display);
    }


    /* remove object from parent */
    function remove_from_parent()
    {
        if(!$this->initially_was_account){
            return;
        }

        /* If domain part was selectable, contruct mail address */
        if($this->mailMethod->domainSelectionEnabled() || $this->mailMethod->mailEqualsCN()){
            $this->mail = $this->mail."@".$this->mailDomainPart;
        }

        /* Remove GOsa attributes */
        plugin::remove_from_parent();

        /* Zero arrays */
        $this->attrs['gosaMailAlternateAddress'] = array();
        $this->attrs['gosaMailForwardingAddress']= array();

        $this->mailMethod->fixAttributesOnRemove();
        $this->cleanup();
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->modify ($this->attrs); 
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }

        new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());


        /* Let the mailMethod remove this mailbox, e.g. from imap and
           update shared folder membership, ACL may need to be updated.
         */
        if (!$this->is_template && $this->remove_folder_from_imap){

            if(!$this->mailMethod->connect()){
                msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"),
                            $this->mailMethod->get_error()), ERROR_DIALOG);
            }else{
                if(!$this->mailMethod->deleteMailbox()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot remove mailbox: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }
                if(!$this->mailMethod->updateSharedFolder()){
                    msg_dialog::display(_("Mail error"), sprintf(_("Cannot update shared folder permissions: %s"),
                                $this->mailMethod->get_error()), ERROR_DIALOG);
                }
            }
        }
        $this->mailMethod->disconnect();


        /* Optionally execute a command after we're done */
        $this->handle_post_events("remove");
    }


    /* Save data to object */
    function save_object()
    {
        /* Check if user wants to remove the shared folder from imap too 
         */
        if($this->initially_was_account && !$this->is_account){
            if(isset($_POST['remove_folder_from_imap'])){
                $this->remove_folder_from_imap = true;
            }else{
                $this->remove_folder_from_imap = false;
            }
        }
        if (isset($_POST['mailedit'])){

            if(isset($_POST['show_effective_memeber'])){
                $this->show_effective_memeber = !$this->show_effective_memeber;
            }

            $mail = $this->mail;
            $server = $this->gosaMailServer;
            plugin::save_object();

            if(!$this->mailMethod->isModifyableServer() && $this->initially_was_account){
                $this->gosaMailServer = $server;
            }
            if(!$this->mailMethod->isModifyableMail() && $this->initially_was_account){
                $this->mail = $mail;
            }else{

                if($this->mailMethod->mailEqualsCN()){
                    $this->mail = &$this->parent->by_object['group']->cn;
                    if(isset($_POST['MailDomain'])){
                        $this->mailDomainPart = get_post('MailDomain');
                    }
                }

                /* Get posted mail domain part, if necessary
                 */
                if($this->mailMethod->domainSelectionEnabled() && isset($_POST['MailDomain'])){
                    if(in_array_strict(get_post('MailDomain'), $this->mailDomainParts)){
                        $this->mailDomainPart = get_post('MailDomain');
                    }
                }
            }

            /* Get folder type 
             */
            if($this->mailMethod->folderTypesEnabled()){
                if(isset($_POST['FolderTypeCAT'])){
                    $this->FolderType['CAT']     = get_post('FolderTypeCAT');
                }
                if(isset($_POST['FolderTypeSUB_CAT'])){
                    $this->FolderType['SUB_CAT'] = get_post('FolderTypeSUB_CAT');
                }
            }

            /* Handle posted ACL changes. 
               Add/del member acls.
             */
            if(isset($_POST['mail_acls_posted'])){
                $new_acls = array();
                foreach(array("__anyone__","__member__") as $attr){
                    $id = (isset($this->aclPostToId[$attr])) ? $this->aclPostToId[$attr] : -1;
                    if(isset($_POST['acl_value_'.$id])){ 
                        $new_acls[$attr] = get_post('acl_value_'.$id);
                    }else{
                        $new_acls[$attr] = $this->folder_acls[$attr];
                    }
                }


                foreach($this->folder_acls as $user => $acl){
                    if($user == "__member__" || $user == "__anyone__") continue;

                    $id = (isset($this->aclPostToId[$user])) ? $this->aclPostToId[$user] : -1;

                    if(isset($_POST['remove_acl_user_'.$id])){
                    }elseif(isset($_POST['acl_user_'.$id])){
                        if($user != get_post('acl_user_'.$id)){
                            $new_acls[get_post('acl_user_'.$id)] = get_post('acl_value_'.$id);
                        }else{
                            $new_acls[$user] = get_post('acl_value_'.$id);
                        }
                    }else{
                        $new_acls[$user] = $acl;
                    }
                }
                if(isset($_POST['add_acl_user'])){
                    $new_acls[_('New')] = $this->folder_acls['__anyone__'];
                }
                $this->folder_acls = $new_acls;
            }

            /* Handle GOsa mail delivery flags.
             */

            /* Assemble mail delivery mode
               The mode field in ldap consists of values between braces, this must
               be called when 'mail' is set, because checkboxes may not be set when
               we're in some other dialog.

               Example for gosaMailDeliveryMode [LR        ]
               L - Local delivery
               R - Reject when exceeding mailsize limit
               S - Use spam filter
               V - Use vacation message
               C - Use custom sieve script
               I - Only insider delivery */
            $tmp= preg_replace("/[^a-z]/i","",$this->gosaMailDeliveryMode);

#     if($this->acl_is_writeable("gosaMailDeliveryModeL")){
#       if(!preg_match("/L/",$tmp) && !isset($_POST['drop_own_mails'])){
#         $tmp.="L";
#       }elseif(preg_match("/L/",$tmp) && isset($_POST['drop_own_mails'])){
#         $tmp = preg_replace("/L/","",$tmp);
#       }
#     }

            $opts = array(
                    "I"   => "only_local");

            foreach($opts as $flag => $post){

                if($this->acl_is_writeable("gosaMailDeliveryMode".$flag)){
                    if(!preg_match("/".$flag."/",$tmp) && isset($_POST[$post])){
                        $tmp.= $flag;
                    }elseif(preg_match("/".$flag."/",$tmp) && !isset($_POST[$post])){
                        $tmp = preg_replace("/".$flag."/","",$tmp);
                    }
                }
            }

            $tmp= "[$tmp]";
            if ($this->gosaMailDeliveryMode != $tmp){
                $this->is_modified= TRUE;
            }
            $this->gosaMailDeliveryMode= $tmp;
        }
    }


    /* Save data to LDAP, depending on is_account we save or delete */
    function save()
    {
        $ldap= $this->config->get_ldap_link();

        /* If domain part was selectable, contruct mail address */
        if(!(!$this->mailMethod->isModifyableMail() && $this->initially_was_account)){

            if($this->mailMethod->domainSelectionEnabled() || $this->mailMethod->mailEqualsCN()){
                $this->mail = $this->mail."@".$this->mailDomainPart;
            }
        }

        /* Enforce lowercase mail address and trim whitespaces
         */
        $this->mail = trim(strtolower($this->mail));


        /* Create acls 
         */
        $this->acl = array("anyone ".$this->folder_acls['__anyone__']);
        $member = $this->get_member();
        $new_folder_acls = array("anyone" => $this->folder_acls['__anyone__']);
        foreach($member['mail'] as $uid => $mail){

            /* Do not save overridden acls */
            if(isset($this->folder_acls[$mail])){
                continue;
            }

            $this->acl[] = $mail." ".$this->folder_acls['__member__'];
            $new_folder_acls[$mail]=$this->folder_acls['__member__'];
        }
        foreach($this->folder_acls as $user => $acls){
            if(preg_match("/^__/",$user)) continue;
            $this->acl[] = $user." ".$acls;
            $new_folder_acls[$user]=$acls;
        }
        $this->folder_acls = $new_folder_acls;
        $this->acl = array_unique($this->acl);

        /* Call parents save to prepare $this->attrs */
        plugin::save();

        /* Save arrays */
        $this->attrs['gosaMailAlternateAddress'] = $this->gosaMailAlternateAddress;
        $this->attrs['gosaMailForwardingAddress']= $this->gosaMailForwardingAddress;

        /* Map method attributes */
        $this->mailMethod->fixAttributesOnStore();

        /* Save data to LDAP */
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify ($this->attrs); 
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }

        if($this->initially_was_account){
            new log("modify","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
        }else{
            new log("create","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
        }

        /* Do imap/sieve actions,
         */
        $this->mailMethod->connect();
        if(!$this->mailMethod->is_connected()){
            msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"),
                        $this->mailMethod->get_error()), ERROR_DIALOG);
        }else{
            if(!$this->mailMethod->updateMailbox()){
                msg_dialog::display(_("Mail error"), sprintf(_("Cannot update mailbox: %s"),
                            $this->mailMethod->get_error()), ERROR_DIALOG);
            }
            if(!$this->mailMethod->setQuota($this->gosaMailQuota)){
                msg_dialog::display(_("Mail error"), sprintf(_("Cannot write quota settings: %s"),
                            $this->mailMethod->get_error()), ERROR_DIALOG);
            }
            /* Save Folder Types, if available 
             */
            if($this->mailMethod->folderTypesEnabled()){
                $this->mailMethod->setFolderType($this->FolderType);
            }
            if(!$this->mailMethod->setFolderACLs($this->folder_acls)){
                msg_dialog::display(_("Mail error"), sprintf(_("Cannot update shared folder permissions: %s"),
                            $this->mailMethod->get_error()), ERROR_DIALOG);
            }
        }
        $this->mailMethod->disconnect();

        /* Optionally execute a command after we're done */
        if ($this->initially_was_account == $this->is_account){
            if ($this->is_modified){
                $this->handle_post_events("modify");
            }
        } else {
            $this->handle_post_events("add");
        }
    }



    /* Check formular input */
    function check()
    {
        if(!$this->is_account) return array();
        $ldap= $this->config->get_ldap_link();


        /* Call common method to give check the hook */
        $message= plugin::check();

        /* Ensure that this group isn't renamed if the mailMethod enforces cn mailAttributes 
         */
        if($this->mailMethod->mailEqualsCN() && $this->initially_was_account){
            if($this->cn != $this->orig_cn){
                $message[] = sprintf(_("The group 'cn' has changed. It can't be changed due to the fact that mail method '%s' relies on it!")
                        ,get_class($this->mailMethod));
            }
        }

        if(empty($this->gosaMailServer)){
            $message[]= msgPool::noserver(_("Mail"));
        }

        /* Mail address checks */
        $mail = $this->mail;
        if(!(!$this->mailMethod->isModifyableMail() && $this->initially_was_account)){
            if($this->mailMethod->domainSelectionEnabled() || $this->mailMethod->mailEqualsCN()){
                $mail.= "@".$this->mailDomainPart;
            }
            if (empty($mail)){
                $message[]= msgPool::required(_("Primary address"));
            }elseif (!tests::is_email($mail)){
                $message[]= msgPool::invalid(_("Mail address"),"","","your-address@your-domain.com");
            }
        }

        /* Check quota */
        if ($this->gosaMailQuota != '' && $this->acl_is_writeable("gosaMailQuota")){
            if (!is_numeric($this->gosaMailQuota)) {
                $message[]= msgPool::invalid(_("Quota size"),$this->gosaMailQuota,"/[0-9]/");
            } else {
                $this->gosaMailQuota= (int) $this->gosaMailQuota;
            }
        }

        /* Check if this mail address is already in use */
        $ldap->cd($this->config->current['BASE']);
        $filter = "(&(!(objectClass=gosaUserTemplate))(!(cn=".$this->cn."))".
            "(objectClass=gosaMailAccount)".
            "(|(mail=".$mail.")(alias=".$mail.")(gosaMailAlternateAddress=".$mail.")))";
        $ldap->search($filter,array("cn"));
        if ($ldap->count() != 0){
            $message[]= msgPool::duplicated(_("Mail address"));
        }

        /* Check rejectsize for integer */
        if ($this->gosaMailMaxSize != '' && $this->acl_is_writeable("gosaMailQuota")){
            if (!is_numeric($this->gosaMailMaxSize)){
                $message[]= msgPool::invalid(_("Mail max size"));
            } else {
                $this->gosaMailMaxSize= (int) $this->gosaMailMaxSize;
            }
        }

        /* Need gosaMailMaxSize if use_mailsize_limit is checked */
        if (is_integer(strpos($this->gosaMailDeliveryMode, "reject")) && $this->gosaMailMaxSize == ""){
            $message[]= _("You need to set the maximum mail size in order to reject anything.");
        }

        if(empty($this->gosaMailServer)){
            $message[] = msgPool::required(_("Mail server"));
        }

        return ($message);
    }

    /* Adapt from template, using 'dn' */
    function adapt_from_template($dn, $skip= array())
    {
        plugin::adapt_from_template($dn, $skip);

        foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){

            if (in_array_strict($val, $skip)){
                continue;
            }

            $this->$val= array();
            if (isset($this->attrs["$val"]["count"])){
                for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
                    $value= $this->attrs["$val"][$i];
                    foreach (array("sn", "givenName", "uid") as $repl){
                        if (preg_match("/%$repl/i", $value)){
                            $value= preg_replace ("/%$repl/i", $this->parent->$repl, $value);
                        }
                    }
                    array_push($this->$val, $value);
                }
            }
        }
    }



    function make_name($attrs)
    {
        $name= "";
        if (isset($attrs['sn'][0])){
            $name= $attrs['sn'][0];
        }
        if (isset($attrs['givenName'][0])){
            if ($name != ""){
                $name.= ", ".$attrs['givenName'][0];
            } else {
                $name.= $attrs['givenName'][0];
            }
        }
        if ($name != ""){
            $name.= " ";
        }

        return ($name);
    }

    function getCopyDialog()
    {
        if(!$this->is_account) return("");

        $smarty = get_smarty();
        $smarty->assign("gosaMailAlternateAddress",    set_post($this->gosaMailAlternateAddress));
        $smarty->assign("gosaMailForwardingAddress",   set_post($this->gosaMailForwardingAddress));
        $smarty->assign("mail", set_post($this->mail));
        $display= $smarty->fetch (get_template_path('paste_mail.tpl', TRUE, dirname(__FILE__)));
        $ret = array();
        $ret['string'] = $display;
        $ret['status'] = "";
        return($ret);
    }

    function saveCopyDialog()
    {
        if(!$this->is_account) return;

        /* Perform ADD / REMOVE ... for mail alternate / mail forwarding addresses 
         */
        $this->execute();
        if(isset($_POST['mail'])){
            $this->mail = get_post('mail');
        }
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        /* Reset alternate mail addresses */
        $this->gosaMailAlternateAddress = array();
    }


    /* Return plugin informations for acl handling  */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Mail"),
                    "plDescription" => _("Group mail"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 10,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("groups"), 
                    "plProvidedAcls"=> array(
                        "mail"                      => _("Mail address"),
                        "gosaMailQuota"             => _("Quota size"),
                        "gosaMailServer"            => _("Mail server"),
                        "kolabFolderType"           => _("Folder type")." ("._("Kolab").")",
                        "gosaMailAlternateAddress"  => _("Alternate addresses"),
                        "gosaMailForwardingAddress" => _("Forwarding addresses"),
                        "gosaMailDeliveryModeI"     => _("Only local"),
                        "acl"                       => _("Permissions"))
                    ));
    }


    /* Remove given ACL for given member (uid,mail) ..
     */
    function removeUserAcl($index )
    {
        if(isset($this->imapacl[$index])){
            unset($this->imapacl[$index]);
        }
    }

    function multiple_execute()
    {
        return($this->execute());
    }


    function init_multiple_support($attrs,$all)
    {
        plugin::init_multiple_support($attrs,$all);

        $this->gosaMailForwardingAddress = array();
        if(isset($attrs['gosaMailForwardingAddress'])){
            for($i = 0 ; $i < $attrs['gosaMailForwardingAddress']['count'] ; $i++){
                $this->gosaMailForwardingAddress[] = $attrs['gosaMailForwardingAddress'][$i];
            }
        }

        $this->gosaMailForwardingAddress_Some = array();
        if(isset($all['gosaMailForwardingAddress'])){
            for($i = 0 ; $i < $all['gosaMailForwardingAddress']['count'] ; $i++){
                if(!in_array_strict($all['gosaMailForwardingAddress'][$i],$this->gosaMailForwardingAddress)){
                    $this->gosaMailForwardingAddress_Some[] = $all['gosaMailForwardingAddress'][$i];
                }
            }
        }
    }

    function multiple_save_object()
    {
        if(isset($_POST['multiple_mail_group_posted'])){
            plugin::multiple_save_object();

            foreach(array("kolabFolderType") as $attr){
                if(isset($_POST['use_'.$attr])){
                    $this->multi_boxes[] = $attr;
                }
            }

            /* Add special kolab attributes */
            if(preg_match("/olab/i",$this->config->get_cfg_value("core","mailMethod"))){
                if(isset($_POST['kolabFolderTypeType']) && $this->acl_is_writeable("kolabFolderType")){
                    $this->kolabFolderTypeType = get_post("kolabFolderTypeType");
                    $this->kolabFolderTypeSubType = get_post("kolabFolderTypeSubType");
                }
            }

            /* Collect data and re-assign it to the imapacl array */
            if ($this->acl_is_writeable("acl")){
                $this->imapacl= array();
                $this->imapacl['%members%']= $_POST['member_permissions'];
                $this->imapacl['anyone']= $_POST['default_permissions'];
                foreach ($this->indexed_user as $nr => $user){
                    if (!isset($_POST["user_$nr"])){
                        continue;
                    }
                    if ($_POST["user_$nr"] != $user ||
                            $_POST["perm_$nr"] != $this->indexed_acl[$nr]){
                        $this->is_modified= TRUE;
                    }
                    $this->imapacl[get_post("user_$nr")]= get_post("perm_$nr");
                }
            }
        }
    }


    /* Return selected values for multiple edit */
    function get_multi_edit_values()
    {
        $ret = plugin::get_multi_edit_values();
        $ret['Forward_some'] = $this->gosaMailForwardingAddress_Some;    
        $ret['Forward_all'] = $this->gosaMailForwardingAddress;    
        if(in_array_strict('kolabFolderType',$this->multi_boxes)){
            $ret['kolabFolderTypeType'] = $this->kolabFolderTypeType;
            $ret['kolabFolderTypeSubType'] = $this->kolabFolderTypeSubType;
        }
        if(in_array_strict("acl",$this->multi_boxes)){
            $ret['imapacl'] = $this->imapacl;
        }
        return($ret);
    }

    function set_multi_edit_values($attrs)
    {
        $forward = array();
        foreach($attrs['Forward_some'] as $addr){
            if(in_array_strict($addr,$this->gosaMailForwardingAddress)){
                $forward[] = $addr;
            }
        }
        foreach($attrs['Forward_all'] as $addr){
            $forward[] = $addr;
        }
        plugin::set_multi_edit_values($attrs);
        $this->gosaMailForwardingAddress = $forward;
    }


    /*! \brief  Add given mail address to the list of forwarders.
     */
    function addForwarder($address)
    {
        if(empty($address)) return;
        $this->gosaMailForwardingAddress[]= $address;
        $this->gosaMailForwardingAddress= array_unique($this->gosaMailForwardingAddress);

        /* Update multiple edit values too */
        if($this->multiple_support_active){
            $this->gosaMailForwardingAddress_Some= 
                array_remove_entries (array($address),$this->gosaMailForwardingAddress_Some);
        }

        sort ($this->gosaMailForwardingAddress);
        reset ($this->gosaMailForwardingAddress);
        $this->is_modified= TRUE;
    }


    /*! \brief  Removes the given mail address from the forwarders 
     */
    function delForwarder($addresses)
    {
        if(empty($addresses)) return;
        $this->gosaMailForwardingAddress= array_remove_entries ($addresses,
                $this->gosaMailForwardingAddress);

        /* Update multiple edit values too */
        if($this->multiple_support_active){
            $this->gosaMailForwardingAddress_Some = array_remove_entries ($addresses,
                    $this->gosaMailForwardingAddress_Some);
        }
        $this->is_modified= TRUE;
    }


    /*! \brief  Add given mail address to the list of alternate adresses ,
      .          check if this mal address is used, skip adding in this case
     */
    function addAlternate($address)
    {
        if(empty($address)) {
            return "";
        }

        $ldap= $this->config->get_ldap_link();

        $address= strtolower($address);

        /* Is this address already assigned in LDAP? */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaMailAccount)(|(mail=$address)".
                "(gosaMailAlternateAddress=$address)))", array("cn", "uid"));

        if ($ldap->count() > 0){
            $attrs= $ldap->fetch ();
            if (!isset($attrs["uid"])) {
                return ("!".$attrs["cn"][0]);
            }
            return ($attrs["uid"][0]);
        }

        /* Add to list of alternates */
        if (!in_array_strict($address, $this->gosaMailAlternateAddress)){
            $this->gosaMailAlternateAddress[]= $address;
        }

        sort ($this->gosaMailAlternateAddress);
        reset ($this->gosaMailAlternateAddress);
        $this->is_modified= TRUE;

        return ("");
    }


    /*! \brief  Removes the given mail address from the alternate addresses  
     */
    function delAlternate($addresses)
    {
        if(!count($addresses)) return;
        $this->gosaMailAlternateAddress= array_remove_entries ($addresses,
                $this->gosaMailAlternateAddress);
        $this->is_modified= TRUE;
    }


    function postable_acls()
    {
        $ret = array();
        $this->aclPostToId = array();
        foreach($this->folder_acls as $name => $acl){
            $id = count($this->aclPostToId);
            $this->aclPostToId[$name] = $id;
            $ret[set_post($name)] = array("name" => set_post($name),"acl" => set_post($acl),"post_name" => $id);
        }
        return($ret);
    }


    function get_effective_member_acls()
    {
        $tmp = array();
        $member = $this->get_member();
        foreach($member['mail'] as $uid => $mail){

            /* Do not save overridden acls */
            if(isset($this->folder_acls[$mail])){
                continue;
            }


            $tmp[$mail]  = $this->folder_acls['__member__'];
        }
        return($tmp);
    }


    function allow_remove()
    {
        $resason = "";
        if(!$this->mailMethod->allow_remove($reason)){
            return($reason);
        }
        return("");
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
