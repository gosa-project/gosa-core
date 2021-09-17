<?php
/*!
  \brief   mail plugin
  \author  Mike Gabriel <mike.gabriel@das-netzwerkteam.de>
  \version 0.99.7
  \date    2015.08.27

  This class provides basic information on mail accounts. This plugin
  has been derived from gosa-plugin-mail, but it is not used to
  actually control mailservers. It is only used for allowing
  mail information to be stored in LDAP.

  This plugin actually breaks gosa-mail-account and is intended to be a
  drop-in replacement for it in situations where it is only necessary
  to store information in inetOrgPerson''s mail LDAP attribute description.

Functions :

 - mailgroup (&$config, $dn=NULL, $base_object=NULL)
 - execute ()
 - remove_from_parent ()
 - save_object ()
 - save ()
 - check ()
 - adapt_from_template ($dn,$skip=array())
 - make_name ($attrs)
 - getCopyDialog ()
 - saveCopyDialog ()
 - PrepareForCopyPaste ($source)
 - plInfo ()
 - multiple_execute ()
 - init_multiple_support ($attrs,$all)
 - multiple_save_object ()
 - multiple_check ()
 - get_multi_edit_values ()
 - set_multi_edit_values ($attrs)
 - addForwarder ($address)
 - delForwarder ($addresses)
 - addAlternate ($address)
 - delAlternate ($addresses)

 */
class mailgroup extends plugin {
    /* Multiple edit */
    var $gosaMailForwardingAddress_Some=array ();
    // Used in multiple edit
    /* Default values */
    var $mail="";
    // Default mail address
    var $gosaMailAlternateAddress=array ();
    // Set default Alternate Mail Adresses to empty array
    var $gosaMailForwardingAddress=array ();
    // Forwarding also empty
    /* Internal */
    var $view_logged=FALSE;

    /* GOsa mail account attributes filled with dummy values,
     * required due to gosaMailAccount objectclass requirements.
     */
    var $gosaMailServer="NULL";
    var $gosaMailDeliveryMode="[         ]";
    //
    /* attribute list for save action */
    var $attributes=array ("mail","gosaMailServer","gosaMailDeliveryMode","gosaMailAlternateAddress","gosaMailForwardingAddress");

    var $objectclasses=array ("gosaMailAccount");
    var $multiple_support=TRUE;
    // Not tested yet
    var $uid="";
    var $cn="";

    function __construct (&$config,$dn=NULL,$base_object=NULL) {
        plugin::__construct ($config,$dn);

        /* Get attributes from parent object
         */
        foreach (array ("uid","cn") as $attr) {
            if (isset ($this->parent->by_object['group'])&&isset ($this->parent->by_object['group']->$attr)) {
                $this->$attr=$this->parent->by_object['group']->$attr;
            }
            elseif (isset ($this->attrs[$attr])) {
                $this->$attr=$this->attrs[$attr][0];
            }
        }

        /* Remember account status
         */
        $this->initially_was_account=$this->is_account;

        /* Initialize configured values
         */
        if ($this->is_account) {

            /* Load attributes containing arrays */
            foreach (array ("gosaMailAlternateAddress","gosaMailForwardingAddress") as $val) {
                $this->$val=array ();
                if (isset ($this->attrs["$val"]["count"])) {
                    for ($i=0;$i<$this->attrs["$val"]["count"];$i++) {
                        array_push ($this->$val,$this->attrs["$val"][$i]);
                    }
                }
            }
        }
    }


    function execute () {
        /* Call parent execute */
        plugin::execute ();

        /* Log view */
        if ($this->is_account&&!$this->view_logged) {
            $this->view_logged=TRUE;
            new log ("view","groups/".get_class ($this),$this->dn);
        }

        /****************
          Account status
         ****************/

        if (!$this->multiple_support_active) {

            if (isset ($_POST['modify_state'])) {
                if ($this->is_account&&$this->acl_is_removeable ()) {
                    $this->is_account=FALSE;
                }
                elseif (!$this->is_account&&$this->acl_is_createable ()) {
                    $this->is_account=TRUE;
                }
            }
            if ($this->is_account) {
                $display=$this->show_disable_header (msgPool::removeFeaturesButton (_ ("Mail")),msgPool::featuresEnabled (_ ("Mail")));
            }
            else {
                $display=$this->show_disable_header (msgPool::addFeaturesButton (_ ("Mail")),msgPool::featuresDisabled (_ ("Mail")));
                return ($display);
            }
        }

        /****************
          Forward addresses
         ****************/

        if (isset ($_POST['mailAddressSelect_cancel'])) {
            $this->mailAddressSelect=FALSE;
            $this->dialog=FALSE;
        }

        if (isset ($_POST['mailAddressSelect_save'])) {
            if ($this->acl_is_writeable ("gosaMailForwardingAddress")) {
                $list=$this->mailAddressSelect->save ();
                foreach ($list as $entry) {
                    $val=$entry['mail'][0];
                    if (!in_array_strict ($val,$this->gosaMailAlternateAddress)&&$val!=$this->mail) {
                        $this->addForwarder ($val);
                        $this->is_modified=TRUE;
                    }
                }
                $this->mailAddressSelect=FALSE;
                $this->dialog=FALSE;
            }
            else {
                msg_dialog::display (_ ("Error"),_ ("Please select an entry!"),ERROR_DIALOG);
            }
        }

        if ($this->mailAddressSelect instanceOf mailAddressSelect) {
            $used=array ();
            $used['mail']=array_values ($this->gosaMailAlternateAddress);
            $used['mail']=array_merge ($used['mail'],array_values ($this->gosaMailForwardingAddress));
            $used['mail'][]=$this->mail;

            // Build up blocklist
            session::set ('filterBlacklist',$used);
            return ($this->mailAddressSelect->execute ());
        }

        if (isset ($_POST['add_forwarder'])) {
            if ($_POST['forward_address']!="") {
                $address=get_post ('forward_address');
                $valid=FALSE;
                if (!tests::is_email ($address)) {
                    if (!tests::is_email ($address,TRUE)) {
                        if ($this->is_template) {
                            $valid=TRUE;
                        }
                        else {
                            msg_dialog::display (_ ("Error"),msgPool::invalid (_ ("Mail address"),"","","your-address@your-domain.com"),ERROR_DIALOG);
                        }
                    }
                }
                elseif ($address==$this->mail||in_array_strict ($address,$this->gosaMailAlternateAddress)) {
                    msg_dialog::display (_ ("Error"),_ ("Cannot add primary address to the list of forwarders!"),ERROR_DIALOG);
                }
                else {
                    $valid=TRUE;
                }
                if ($valid) {
                    if ($this->acl_is_writeable ("gosaMailForwardingAddress")) {
                        $this->addForwarder ($address);
                        $this->is_modified=TRUE;
                    }
                }
            }
        }
        if (isset ($_POST['delete_forwarder'])) {
            $this->delForwarder (get_post ('forwarder_list'));
        }


        /****************
          Alternate addresses
         ****************/

        if (isset ($_POST['add_alternate'])) {
            $valid=FALSE;
            if (!tests::is_email (get_post ('alternate_address'))) {
                if ($this->is_template) {
                    if (! (tests::is_email (get_post ('alternate_address'),TRUE))) {
                        msg_dialog::display (_ ("Error"),msgPool::invalid (_ ("Mail address"),"","","your-domain@your-domain.com"),ERROR_DIALOG);
                    }
                    else {
                        $valid=TRUE;
                    }
                }
                else {
                    msg_dialog::display (_ ("Error"),msgPool::invalid (_ ("Mail address"),"","","your-domain@your-domain.com"),ERROR_DIALOG);
                }
            }
            else {
                $valid=TRUE;
            }
            if ($valid&& ($user=$this->addAlternate (get_post ('alternate_address')))!="") {
                $ui=get_userinfo ();
                $addon="";
                if ($user[0]=="!") {
                    $addon=sprintf (_ ("Address is already in use by group '%s'."),mb_substr ($user,1));
                }
                else {
                    $addon=sprintf (_ ("Address is already in use by user '%s'."),$user);
                }
                msg_dialog::display (_ ("Error"),msgPool::duplicated (_ ("Mail address"))."<br><br><i>"."$addon</i>",ERROR_DIALOG);
            }
        }
        if (isset ($_POST['delete_alternate'])&&isset ($_POST['alternates_list'])) {
            $this->delAlternate (get_post ('alternates_list'));
        }


        /****************
          SMARTY- Assign smarty variables
         ****************/

        /* Load templating engine */
        $smarty=get_smarty ();
        $smarty->assign ("initially_was_account",$this->initially_was_account);

        /* Assign acls */
        $tmp=$this->plInfo ();
        foreach ($tmp['plProvidedAcls'] as $name=>$translation) {
            $smarty->assign ($name."ACL",$this->getacl ($name));
        }
        foreach ($this->attributes as $name) {
            $smarty->assign ($name,set_post ($this->$name));
        }


        /******
          Multi edit support
         ******/
        foreach ($this->attributes as $attr) {
            if (in_array_strict ($attr,$this->multi_boxes)) {
                $smarty->assign ("use_".$attr,TRUE);
            }
            else {
                $smarty->assign ("use_".$attr,FALSE);
            }
        }

        $smarty->assign ("Forward_all",set_post ($this->gosaMailForwardingAddress));
        $smarty->assign ("Forward_some",set_post ($this->gosaMailForwardingAddress_Some));
        $smarty->assign ("multiple_support",set_post ($this->multiple_support_active));

        $display.=$smarty->fetch (get_template_path ('mailaddress.tpl',TRUE,dirname (__FILE__)));
        return ($display);
    }


    /* remove object from parent */
    function remove_from_parent () {
        if (!$this->initially_was_account) {
            return;
        }

        /* Remove GOsa attributes */
        plugin::remove_from_parent ();

        /* Zero arrays */
        $this->attrs['gosaMailAlternateAddress']=array ();
        $this->attrs['gosaMailForwardingAddress']=array ();

        $this->cleanup ();
        $ldap=$this->config->get_ldap_link ();
        $ldap->cd ($this->dn);
        $ldap->modify ($this->attrs);
        if (!$ldap->success ()) {
            msg_dialog::display (_ ("LDAP error"),msgPool::ldaperror ($ldap->get_error (),$this->dn,LDAP_MOD,get_class ()));
        }

        new log ("remove","groups/".get_class ($this),$this->dn,array_keys ($this->attrs),$ldap->get_error ());

        /* Optionally execute a command after we''re done */
        $this->handle_post_events ("remove");
    }


    /* Save data to object */
    function save_object () {
        if (isset ($_POST['mailedit'])) {

            /* Acquire updated mail address
             */
            if (isset ($_POST['mail'])) {
                $this->mail=get_post ('mail');
            }

            $mail=$this->mail;
            $gosaMailServer=$this->gosaMailServer;
            $gosaMailDeliveryMode=$this->gosaMailDeliveryMode;

            plugin::save_object ();

            if ($this->initially_was_account&&!$this->is_template) {
                $this->mail=$mail;
                $this->gosaMailServer=$gosaMailServer;
                $this->gosaMailDeliveryMode=$gosaMailDeliveryMode;
            }
        }
    }


    /* Save data to LDAP, depending on is_account we save or delete */
    function save () {
        $ldap=$this->config->get_ldap_link ();

        /* Enforce lowercase mail address and trim whitespaces
         */
        $this->mail=trim (strtolower ($this->mail));

        /* Call parents save to prepare $this->attrs */
        plugin::save ();

        /* Save arrays */
        $this->attrs['gosaMailAlternateAddress']=$this->gosaMailAlternateAddress;
        $this->attrs['gosaMailForwardingAddress']=$this->gosaMailForwardingAddress;

        /* Save data to LDAP */
        $ldap->cd ($this->dn);
        $this->cleanup ();
        $ldap->modify ($this->attrs);
        if (!$ldap->success ()) {
            msg_dialog::display (_ ("LDAP error"),msgPool::ldaperror ($ldap->get_error (),$this->dn,LDAP_MOD,get_class ()));
        }

        if ($this->initially_was_account) {
            new log ("modify","groups/".get_class ($this),$this->dn,array_keys ($this->attrs),$ldap->get_error ());
        }
        else {
            new log ("create","groups/".get_class ($this),$this->dn,array_keys ($this->attrs),$ldap->get_error ());
        }

        /* Optionally execute a command after we''re done */
        if ($this->initially_was_account==$this->is_account) {
            if ($this->is_modified) {
                $this->handle_post_events ("modify");
            }
        }
        else {
            $this->handle_post_events ("add");
        }
    }



    /* Check formular input */
    function check () {
        if (!$this->is_account)
            return array ();
        $ldap=$this->config->get_ldap_link ();

        /* Call common method to give check the hook */
        $message=plugin::check ();

        /* Mail address checks */
        $mail=$this->mail;
        if (empty ($mail)) {
            $message[]=msgPool::required (_ ("Mail address"));
        }
        elseif (!tests::is_email ($mail)) {
            if ($this->is_template) {
                $message[]=msgPool::invalid (_ ("Mail address"),"","","{%cn}@your-domain.com");
            }
            else {
                $message[]=msgPool::invalid (_ ("Mail address"),"","","your-address@your-domain.com");
            }
        }

        /* Check if this mail address is already in use */
        $ldap->cd ($this->config->current['BASE']);
        $filter="(&(!(objectClass=gosaUserTemplate))(!(cn=".$this->cn."))"."(objectClass=gosaMailAccount)"."(|(mail=".$mail.")(alias=".$mail.")(gosaMailAlternateAddress=".$mail.")))";
        $ldap->search ($filter,array ("cn"));
        if ($ldap->count ()!=0) {
            $message[]=msgPool::duplicated (_ ("Mail address"));
        }

        return ($message);
    }

    /* Adapt from template, using 'dn' */
    function adapt_from_template ($dn,$skip=array ()) {
        plugin::adapt_from_template ($dn,$skip);

        // Remove non ASCII charcters
        $this->mail=iconv ('UTF-8','US-ASCII//TRANSLIT',$this->mail);
        $this->mail=preg_replace ('/[^(\x20-\x7F)]*/','',$this->mail);

        // No spaces are allowed here
        $this->mail=preg_replace ("/[ ]/","",$this->mail);

        foreach (array ("gosaMailAlternateAddress","gosaMailForwardingAddress") as $val) {

            if (in_array_strict ($val,$skip)) {
                continue;
            }

            $this->$val=array ();
            if (isset ($this->attrs["$val"]["count"])) {
                for ($i=0;$i<$this->attrs["$val"]["count"];$i++) {
                    $value=$this->attrs["$val"][$i];
                    foreach (array ("sn","givenName","uid") as $repl) {
                        if (preg_match ("/%$repl/i",$value)) {
                            $value=preg_replace ("/%$repl/i",$this->parent->$repl,$value);
                        }

                        // Remove non ASCII charcters and spaces
                        $value=iconv ('UTF-8','US-ASCII//TRANSLIT',$this->mail);
                        $value=preg_replace ('/[^(\x20-\x7F)]*/','',$this->mail);
                        $value=preg_replace ("/[ ]/","",$this->mail);
                    }
                    array_push ($this->$val,$value);
                }
            }
        }
    }

    function make_name ($attrs) {
        $name="";
        if (isset ($attrs['sn'][0])) {
            $name=$attrs['sn'][0];
        }
        if (isset ($attrs['givenName'][0])) {
            if ($name!="") {
                $name.=", ".$attrs['givenName'][0];
            }
            else {
                $name.=$attrs['givenName'][0];
            }
        }
        if ($name!="") {
            $name.=" ";
        }

        return ($name);
    }

    function getCopyDialog () {
        if (!$this->is_account)
            return ("");

        $smarty=get_smarty ();
        $smarty->assign ("gosaMailAlternateAddress",set_post ($this->gosaMailAlternateAddress));
        $smarty->assign ("gosaMailForwardingAddress",set_post ($this->gosaMailForwardingAddress));
        $smarty->assign ("mail",set_post ($this->mail));
        $display=$smarty->fetch (get_template_path ('paste_mailaddress.tpl',TRUE,dirname (__FILE__)));
        $ret=array ();
        $ret['string']=$display;
        $ret['status']="";
        return ($ret);
    }

    function saveCopyDialog () {
        if (!$this->is_account)
            return;

        /* Perform ADD / REMOVE ... for mail alternate / mail forwarding addresses
         */
        $this->execute ();
        if (isset ($_POST['mail'])) {
            $this->mail=get_post ('mail');
        }
    }


    function PrepareForCopyPaste ($source) {
        plugin::PrepareForCopyPaste ($source);

        /* Reset alternate mail addresses */
        $this->gosaMailAlternateAddress=array ();
    }


    /* Return plugin informations for acl handling  */
    static function plInfo () {
        return (array ("plShortName"=>_ ("Mail"),"plDescription"=>_ ("Group mail"),"plSelfModify"=>FALSE,"plDepends"=>array (),"plPriority"=>10,"plSection"=>array ("administration"),"plCategory"=>array ("groups"),"plProvidedAcls"=>array ("mail"=>_ ("Mail address"),"gosaMailAlternateAddress"=>_ ("Alternate addresses"),"gosaMailForwardingAddress"=>_ ("Forwarding addresses"))));
    }


    function multiple_execute () {
        return ($this->execute ());
    }


    function init_multiple_support ($attrs,$all) {
        plugin::init_multiple_support ($attrs,$all);

        $this->gosaMailForwardingAddress=array ();
        if (isset ($attrs['gosaMailForwardingAddress'])) {
            for ($i=0;$i<$attrs['gosaMailForwardingAddress']['count'];$i++) {
                $this->gosaMailForwardingAddress[]=$attrs['gosaMailForwardingAddress'][$i];
            }
        }

        $this->gosaMailForwardingAddress_Some=array ();
        if (isset ($all['gosaMailForwardingAddress'])) {
            for ($i=0;$i<$all['gosaMailForwardingAddress']['count'];$i++) {
                if (!in_array_strict ($all['gosaMailForwardingAddress'][$i],$this->gosaMailForwardingAddress)) {
                    $this->gosaMailForwardingAddress_Some[]=$all['gosaMailForwardingAddress'][$i];
                }
            }
        }
    }

    function multiple_save_object () {
        if (isset ($_POST['multiple_mail_group_posted'])) {
            plugin::multiple_save_object ();
        }
    }


    function multiple_check () {
        $message=plugin::multiple_check ();

        /* Mail address checks */
        $mail_pattern=$this->mail;

        if (!empty ($mail_pattern)) {
            if ( (!tests::is_email ($mail_pattern,TRUE))|| (!preg_match ('/^.*{%cn}.*@.*$/i',$mail_pattern))) {
                $message[]=msgPool::invalid (_ ("Mail address pattern"),"","","{%cn}@your-domain.com")."<br><br><br>"._ ("Please make use of the {%cn} placeholder for the group part of the mail address.");
            }
        }

        return ($message);
    }

    /* Return selected values for multiple edit */
    function get_multi_edit_values () {
        $ret=plugin::get_multi_edit_values ();
        $ret['Forward_some']=$this->gosaMailForwardingAddress_Some;
        $ret['Forward_all']=$this->gosaMailForwardingAddress;
        $ret['mail']=$this->mail;
        return ($ret);
    }

    function set_multi_edit_values ($attrs) {
        $forward=array ();
        foreach ($attrs['Forward_some'] as $addr) {
            if (in_array_strict ($addr,$this->gosaMailForwardingAddress)) {
                $forward[]=$addr;
            }
        }
        foreach ($attrs['Forward_all'] as $addr) {
            $forward[]=$addr;
        }

        /* Replace placeholders in mail address patterns */
        $mail_pattern=$attrs['mail'];

        if ($mail_pattern) {
            $value=$this->cn;
            if (preg_match ("/{%cn}/",$mail_pattern)) {
                $attrs['mail']=str_replace ("{%cn}",$value,$mail_pattern);
            }

            /* Add mail account data, if object is not in account state, yet
             */
            if (!$this->is_account&&$this->acl_is_createable ()) {
                $this->is_account=TRUE;
            }
        }
        else {
            $mail=$this->mail;
        }

        plugin::set_multi_edit_values ($attrs);

        if (!$mail_pattern) {
            $this->mail=$mail;
        }

        $this->gosaMailForwardingAddress=$forward;
    }


    /*! \brief  Add given mail address to the list of forwarders.
     */
    function addForwarder ($address) {
        if (empty ($address))
            return;
        $this->gosaMailForwardingAddress[]=$address;
        $this->gosaMailForwardingAddress=array_unique ($this->gosaMailForwardingAddress);

        /* Update multiple edit values too */
        if ($this->multiple_support_active) {
            $this->gosaMailForwardingAddress_Some=array_remove_entries (array ($address),$this->gosaMailForwardingAddress_Some);
        }

        sort ($this->gosaMailForwardingAddress);
        reset ($this->gosaMailForwardingAddress);
        $this->is_modified=TRUE;
    }


    /*! \brief  Removes the given mail address from the forwarders
     */
    function delForwarder ($addresses) {
        if (empty ($addresses))
            return;
        $this->gosaMailForwardingAddress=array_remove_entries ($addresses,$this->gosaMailForwardingAddress);

        /* Update multiple edit values too */
        if ($this->multiple_support_active) {
            $this->gosaMailForwardingAddress_Some=array_remove_entries ($addresses,$this->gosaMailForwardingAddress_Some);
        }
        $this->is_modified=TRUE;
    }


    /*! \brief  Add given mail address to the list of alternate adresses ,
      .          check if this mal address is used, skip adding in this case
     */
    function addAlternate ($address) {
        if (empty ($address)){
          return "";
        }

        $ldap=$this->config->get_ldap_link ();

        $address=strtolower ($address);

        /* Is this address already assigned in LDAP? */
        $ldap->cd ($this->config->current['BASE']);
        $ldap->search ("(&(objectClass=gosaMailAccount)(|(mail=$address)"."(gosaMailAlternateAddress=$address)))",array ("cn","uid"));

        if ($ldap->count ()>0) {
            $attrs=$ldap->fetch ();
            if (!isset ($attrs["uid"])) {
                return ("!".$attrs["cn"][0]);
            }
            return ($attrs["uid"][0]);
        }

        /* Add to list of alternates */
        if (!in_array_strict ($address,$this->gosaMailAlternateAddress)) {
            $this->gosaMailAlternateAddress[]=$address;
        }

        sort ($this->gosaMailAlternateAddress);
        reset ($this->gosaMailAlternateAddress);
        $this->is_modified=TRUE;

        return ("");
    }


    /*! \brief  Removes the given mail address from the alternate addresses
     */
    function delAlternate ($addresses) {
        if (!count ($addresses))
            return;
        $this->gosaMailAlternateAddress=array_remove_entries ($addresses,$this->gosaMailAlternateAddress);
        $this->is_modified=TRUE;
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
