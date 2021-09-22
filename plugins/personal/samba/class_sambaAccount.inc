<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004-2005 Cajus Pollmeier

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

class sambaAccount extends plugin
{
    /* Definitions */
    var $plHeadline= "Samba";
    var $plDescription= "Manage personal Samba settings";
    var $view_logged = FALSE;

    // Domaind information, gid, sid, rid
    var $uid= "";
    var $uidNumber= 65535;
    var $gidNumber= 65535;
    var $SID= "";
    var $ridBase= 0;
    var $sambaSID= "";
    var $orig_sambaDomainName= "";
    var $sambaDomainName= "";

    // Some of these attributes are just used to 
    //  display the domain information dialog, and 
    //  thus not writte back to the ldap. 
    var $sambaBadPasswordCount= "";
    var $sambaBadPasswordTime= "";
    var $sambaPasswordHistory= "";
    var $sambaLogonHours= "";
    var $sambaPwdLastSet= "0";
    var $sambaLogonTime= "0";
    var $sambaLogoffTime= "2147483647";
    var $sambaKickoffTime= "";
    var $sambaPwdCanChange= "0";
    var $sambaPwdMustChange= "0";

    // Flags (checkboxes) to restrict account settings.
    var $sambaAcctFlags= "[UX        ]";
    var $flag_enforcePasswordChange = FALSE;
    var $flag_passwordNeverExpires  = FALSE;
    var $flag_noPasswordRequired    = FALSE;
    var $flag_temporaryDisabled     = FALSE;
    var $flag_cannotChangePassword  = FALSE;
    var $flag_sambaKickoffTime      = FALSE;

    // String values 
    var $sambaHomePath= "";
    var $sambaHomeDrive= "";
    var $sambaLogonScript= "";
    var $sambaProfilePath= "";
    var $sambaPrimaryGroupSID= "";
    var $sambaUserWorkstations= "";

    // Munged object.
    var $sambaMungedDial= "";
    var $mungedObject;

    /* Helper */
    var $cache = array();
    var $trustSelect= FALSE;

    /* attribute list for save action */
    var $ctxattributes= array();
    var $attributes= array("sambaSID", "sambaPwdLastSet", "sambaLogonTime",
            "sambaLogoffTime", "sambaKickoffTime", "sambaPwdCanChange",
            "sambaPwdMustChange", "sambaAcctFlags", "uid", "sambaMungedDial",
            "sambaHomePath", "sambaHomeDrive", "sambaLogonScript",
            "sambaProfilePath", "sambaPrimaryGroupSID", "sambaDomainName",
            "sambaUserWorkstations", "sambaPasswordHistory",
            "sambaLogonHours", "sambaBadPasswordTime",
            "sambaBadPasswordCount");
    var $objectclasses= array('sambaSamAccount');

    var $CopyPasteVars = array("mungedObject","orig_sambaDomainName");

    var $multiple_support = TRUE;
    var $multiple_sambaUserWorkstations = array();


    function __construct(&$config, $dn= NULL)
    {

        plugin::__construct ($config, $dn);

        // Set current uid if possible.
        if(isset($this->attrs['uid'][0])){
            $this->uid = $this->attrs['uid'][0];
        }

        // Get samba domain and its sid/rid base
        if ($this->sambaSID != ""){
            $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
            $ldap= $this->config->get_ldap_link();
            $ldap->cd($this->config->current['BASE']);
            $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase","sambaDomainName"));
            if ($ldap->count() != 0){
                $attrs= $ldap->fetch();
                if(isset($attrs['sambaAlgorithmicRidBase'])){
                    $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];
                } else {
                    $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                }
                if ($this->sambaDomainName == ""){
                    $this->sambaDomainName= $attrs['sambaDomainName'][0];
                }
            } else {

                // Fall back to a 'DEFAULT' domain, if none was found in LDAP.
                if ($this->sambaDomainName == "")  $this->sambaDomainName= "DEFAULT";

                // Nothing in ldap, use configured sid and rid values.
                $this->ridBase= $this->config->get_cfg_value("core","sambaRidBase");
                $this->SID= $this->config->get_cfg_value("core","sambaSid");
            }
        }

        // Keep original domain name and plugin status, to be able to detect modifications.
        $this->orig_sambaDomainName= $this->sambaDomainName;
        $this->initially_was_account= $this->is_account;

        // Instantiate munged object and load info.
        $this->mungedObject= new sambaMungedDial;
        $this->ctxattributes= $this->mungedObject->ctxattributes;
        if (isset($this->attrs['sambaMungedDial'])){
            $this->mungedObject->load($this->sambaMungedDial);
        }

        // Load flags
        $this->loadFlagsFromSource($this->attrs);

        // Set kickOffTime to date
        if (isset($this->attrs['sambaKickoffTime'][0])){
            if ($this->sambaKickoffTime == 2147483647){
                $this->sambaKickoffTime = "";
            } elseif (!$this->sambaKickoffTime == ""){
                $this->sambaKickoffTime = date("d.m.Y", $this->sambaKickoffTime);
                $this->flag_sambaKickoffTime = TRUE;
            }
        }
    }


    function loadFlagsFromSource($attrs)
    {

        // Samba flag description 
        // ----------------------
        //  The Official Samba 3.2.x HOWTO and Reference Guide
        //  Jelmer R. Vernooij, John H. Terpstra, and Gerald (Jerry) Carter
        //  May 27, 2009
        // ----------------------
        //  D - Account is disabled.
        //  H - A home directory is required.
        //  I - An inter-domain trust account.
        //  L - Account has been auto-locked.
        //  M - An MNS (Microsoft network service) logon account.
        //  N - Password not required.
        //  S - A server trust account.
        //  T - Temporary duplicate account entry.
        //  U - A normal user account.
        //  W - A workstation trust account.
        //  X - Password does not expire.

        // sambaPwdCanChange
        // _______________
        // Specifies the time (UNIX time format) after which the user is allowed to change his password.
        // If this attribute is not set, the user will be free to change his password whenever he wants.

        // sambaPwdLastSet
        // _______________
        // The integer time in seconds since 1970 when the sambaLMPassword and sambaNTPassword attributes were last set.

        // sambaPwdMustChange
        // _______________
        // Specifies the time (UNIX time format) when the user is forced to change his password. If this
        // value is set to 0, the user will have to change his password at first login. If this attribute is not
        // set, then the password will never expire.


        // A password change is enforced by using a timestamp in sambaPwdMustChange.
        //  We simple set it to '0' to enforce a change.
        // --------------------------------
        // Normally it contains a timestamp, which specifies and expiration date. 
        $this->flag_enforcePasswordChange =  (isset($attrs['sambaPwdMustChange']) && $attrs['sambaPwdMustChange'][0] == '0');

        // A user cannot change his password until the given timestamp has reached.
        //  We simply set it to max int to disallow a password change till the timestamp reaches 4294967295, 
        //  this is definitly far in the future and thus disallows a password change at all.
        // --------------------------------
        // The user is not able to change his password while sambaPwdCanChange is 4294967295 (Integer 32 Bit max)
        $this->flag_cannotChangePassword = (isset($attrs['sambaPwdCanChange']) && $attrs['sambaPwdCanChange'][0] == '4294967295');

        // Load samba account flags
        if(isset($attrs['sambaAcctFlags'][0])){
            $this->sambaAcctFlags = $attrs['sambaAcctFlags'][0];

            // A password never expires if 'sambaAcctFlags' contains 'X'. 
            // (See flags above for details)
            $this->flag_passwordNeverExpires  = preg_match("/X/i", $this->sambaAcctFlags);

            // A password is NOT required if 'sambaAcctFlags' contains 'N'. 
            // (See flags above for details)
            $this->flag_noPasswordRequired    = preg_match("/N/i", $this->sambaAcctFlags);

            // A account is locked if if 'sambaAcctFlags' contains 'L' or 'D'.
            // (See flags above for details)
            $this->flag_temporaryDisabled = preg_match("/L/i", $this->sambaAcctFlags) ||
                preg_match("/D/i", $this->sambaAcctFlags);

        }
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Log view */
        if($this->is_account && !$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","users/".get_class($this),$this->dn);
        }

        /* Do we need to flip is_account state? */
        if (isset($_POST['modify_state'])){
            $this->is_account= !$this->is_account;
        }
        /* Do we represent a valid account? */
        if (!$this->is_account && $this->parent === NULL){
            $display= "<img alt=\"\"src=\"images/small-error.png\" align=\"middle\">&nbsp;<b>".
                msgPool::noValidExtension(_("Samba"))."</b>";
            $display.= back_to_main();
            return ($display);
        }

        $display ="";
        if(!$this->multiple_support_active){

            // Show tab heades to activate and deactivate the samba extension.
            $display= "";
            if ($this->parent !== NULL){
                if ($this->is_account){
                    $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Samba")),
                            msgPool::featuresEnabled(_("Samba")));
                } else {

                    // Samba3 dependency on posix accounts are enabled in the moment, because I need to rely on unique
                    // uidNumbers. There'll be a better solution later on. 
                    $obj= $this->parent->by_object['posixAccount'];
                    if ($obj->is_account){
                        $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Samba")),
                                msgPool::featuresDisabled(_("Samba")));
                    } else {
                        $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Samba")),
                                msgPool::featuresDisabled(_("Samba"), _("POSIX")), TRUE);
                    }
                    return ($display);
                }
            }
        }

        // Editing from the MyAccount/Personal section may be disabled until we've pressed the 'edit' button.
        $SkipWrite = (!isset($this->parent) || !$this->parent) && !session::is_set('edit');


        // Handle the samba logon hours dialog here, instantiate it on request.
        if(isset($_POST['SetSambaLogonHours']) && $this->acl_is_readable("sambaLogonHours")){
            $this->dialog = new sambaLogonHours($this->config,$this->dn,$this->sambaLogonHours, $this->getacl('sambaLogonHours'));
        }
        if(isset($_POST['cancel_logonHours'])){
            $this->dialog = FALSE;
        }
        if(isset($_POST['save_logonHours'])){
            $this->dialog->save_object();
            if($this->acl_is_writeable("sambaLogonHours")){
                $this->sambaLogonHours = $this->dialog->save();
            }
            $this->dialog = FALSE;
        }
        if((isset($this->dialog)) && (is_object($this->dialog))){
            $this->dialog->save_object();
            return($this->dialog->execute());
        }


        // Get smarty
        $smarty= get_smarty();
        $smarty->assign("usePrototype", "true");

        // Assign GOsa ACLs 
        $tmp = $this->plInfo();
        foreach($tmp['plProvidedAcls'] as $var => $rest){
            $smarty->assign($var."ACL",$this->getacl($var,$SkipWrite));
        }

        if(!session::is_set('edit') && !isset($this->parent)){
            $smarty->assign("sambaLogonHoursACL","");
        }


        // Handle workstation list - Remove entries.
        if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list'])){
            if($this->acl_is_writeable("sambaUserWorkstations",$SkipWrite)){
                if($this->multiple_support_active){
                    foreach($_POST['workstation_list'] as $name){
                        if(isset($this->multiple_sambaUserWorkstations[trim($name)])){
                            unset($this->multiple_sambaUserWorkstations[trim($name)]);
                        }
                    } 
                }else{
                    $tmp= $this->sambaUserWorkstations;
                    foreach($_POST['workstation_list'] as $name){
                        $tmp= preg_replace("/$name/", '', $tmp);
                        $this->is_modified= TRUE;
                    }
                    $tmp= preg_replace('/,+/', ',', $tmp);
                    $this->sambaUserWorkstations= trim($tmp, ',');
                }
            }
        }

        // Handle trust maschines/accessTo list 
        if (isset($_POST["add_ws"])){
            if($this->acl_is_writeable("sambaUserWorkstations",$SkipWrite)){
                $this->trustSelect= new trustSelect($this->config,get_userinfo());
                $this->dialog= TRUE;
            }
        }

        // Dialog canceled
        if (isset($_POST["add_ws_cancel"]) || isset($_POST["cancel-abort"])){
            $this->trustSelect= FALSE;
            $this->dialog= FALSE;
        }

        // Add selected machines to trusted ones.
        if (isset($_POST["add_ws_finish"]) || isset($_POST["ok-save"]) &&  $this->trustSelect){
            $trusts = $this->trustSelect->detectPostActions();
            if(isset($trusts['targets'])){
                $headpage = $this->trustSelect->getHeadpage();
                if($this->multiple_support_active){
                    foreach($trusts['targets'] as $id){
                        $attrs = $headpage->getEntry($id);
                        $we =$attrs['cn'][0];
                        $this->multiple_sambaUserWorkstations[trim($we)] = array("Name" => trim($ws), "UsedByAllUsers" => TRUE);
                    }
                }else{

                    $tmp= $this->sambaUserWorkstations;
                    foreach($trusts['targets'] as $id){
                        $attrs = $headpage->getEntry($id);
                        $we =$attrs['cn'][0];
                        $tmp.= ",$we";
                    }
                    $tmp= preg_replace('/,+/', ',', $tmp);
                    $this->sambaUserWorkstations= trim($tmp, ',');
                }

                $this->is_modified= TRUE;
            }
            $this->trustSelect= NULL;
            $this->dialog= FALSE;
        }

        // Display trust dialog
        if ($this->trustSelect){
            session::set('filterBlacklist', array('cn' => preg_split('/,/',$this->sambaUserWorkstations)));
            return($this->trustSelect->execute());
        }


        // Fill domain selection. 
        $domains= array_keys($this->config->data['SERVERS']['SAMBA']);
        $smarty->assign("domains", set_post($domains));

        // Fill drive letters.
        $letters= array("");
        for ($i= 68; $i<91; $i++){
            $letters[]= chr($i).":";
        }
        $smarty->assign("drives", $letters);


        // Fill terminal server setttings
        foreach ($this->ctxattributes as $attr){
            if (isset($this->mungedObject->ctx[$attr])){
                $smarty->assign("$attr", set_post($this->mungedObject->ctx[$attr]));

                // Set field  to blank if value is 0
                if(in_array_strict($attr, array("CtxMaxConnectionTime", "CtxMaxDisconnectionTime", "CtxMaxIdleTime"))) {
                    if($this->mungedObject->ctx[$attr] == 0) {
                        $smarty->assign("$attr", "");
                    }
                }
            } else {
                $smarty->assign("$attr", "");
            }
        }

        // Assign enum values for preset items. 
        $shadowModeVals= array( 
                "0" => _("disabled"),
                "1" => _("input on, notify on"),
                "2" => _("input on, notify off"),
                "3" => _("input off, notify on"),
                "4" => _("input off, notify off"));
        $brokenConnModeVals= array( 	
                "0" => _("disconnect"),
                "1" => _("reset"));
        $reConnModeVals= array( "0" => _("from any client"),
                "1" => _("from previous client only"));
        $smarty->assign("shadow",      set_post($shadowModeVals));
        $smarty->assign("brokenconn",  set_post($brokenConnModeVals));
        $smarty->assign("reconn",      set_post($reConnModeVals));

        // Fill preset items with values 
        $smarty->assign("shadowmode",      set_post($this->mungedObject->getShadow()));
        $smarty->assign("brokenconnmode",  set_post($this->mungedObject->getBrokenConn()));
        $smarty->assign("reconnmode",      set_post($this->mungedObject->getReConn()));


        // Set form elements to disabled/enable state 
        $smarty->assign("tsloginstate", $this->mungedObject->getTsLogin()?"":"disabled");
        $smarty->assign("inheritstate", "");
        if($this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite)){
            $smarty->assign("inheritstate", $this->mungedObject->getInheritMode()?"disabled":"");
        }

        // Set checkboxes to checked or unchecked state 
        $smarty->assign("tslogin", $this->mungedObject->getTsLogin()?"checked":"");
        $smarty->assign("inherit", $this->mungedObject->getInheritMode()?"checked":"");
        $smarty->assign("connectclientdrives",
                $this->mungedObject->getConnectClientDrives()?"checked":"");
        $smarty->assign("connectclientprinters",
                $this->mungedObject->getConnectClientPrinters()?"checked":"");
        $smarty->assign("defaultprinter",
                $this->mungedObject->getDefaultPrinter()?"checked":"");
        $smarty->assign("CtxMaxConnectionTimeF",
                $this->mungedObject->getCtxMaxConnectionTimeF()?"checked":"");
        $smarty->assign("CtxMaxDisconnectionTimeF",
                $this->mungedObject->getCtxMaxDisconnectionTimeF()?"checked":"");
        $smarty->assign("CtxMaxIdleTimeF",
                $this->mungedObject->getCtxMaxIdleTimeF()?"checked":"");


        // Fill sambaUserWorkstations 
        $ws= explode(",", $this->sambaUserWorkstations);
        sort($ws);

        // Tidy checks for empty option, and smarty will produce one if array[0]=""
        if(($ws[0]=="")&&(count($ws)==1)) $ws=array();
        if($this->multiple_support_active){
            $smarty->assign("multiple_workstations",$this->multiple_sambaUserWorkstations);
        }  
        $smarty->assign("workstations", set_post($ws));


        // Assign plugin values
        foreach($this->attributes as $val){
            $smarty->assign("$val", set_post($this->$val));
        }

        // Assign munged attributes
        foreach($this->mungedObject->getOnDemandFlags() as $key => $value) {
            $smarty->assign("$key", set_post($value));
        }

        // Assign selected multi edit checkbox values.
        foreach($this->attributes as $attr){
            if(in_array_strict($attr,$this->multi_boxes)){
                $smarty->assign("use_".$attr,TRUE);
            }else{
                $smarty->assign("use_".$attr,FALSE);
            }
        }
        foreach(array("tslogin","CtxWFHomeDir","CtxWFHomeDirDrive","CtxWFProfilePath",
                    "inherit","CtxWorkDirectory","CtxInitialProgram","CtxMaxConnectionTimeF",
                    "CtxMaxConnectionTime","CtxMaxDisconnectionTimeF",
                    "CtxMaxDisconnectionTime","CtxMaxIdleTimeF","CtxMaxIdleTime","connectclientdrives",
                    "onnectclientprinters","defaultprinter","shadow","brokenconn",
                    "reconn","connectclientprinters","SetSambaLogonHours",
                    "workstation_list","sambaKickoffTime",
                    "enforcePasswordChange", "passwordNeverExpires", "noPasswordRequired",
                    "temporaryDisabled","cannotChangePassword") as $attr){

            if(in_array_strict($attr,$this->multi_boxes)){
                $smarty->assign("use_".$attr,TRUE);
            }else{
                $smarty->assign("use_".$attr,FALSE);
            }
        }

        if($this->multiple_support_active){
            $smarty->assign("tsloginstate","");
        }

        // Create additional info for sambaKickOffTime and sambaPwdMustChange. 
        //  e.g. Display effective kickoff time. Domain policy + user settings. 
        $additional_info_PwdMustChange = "";

        // Calculate effective max Password Age 
        //  This can only be calculated if sambaPwdLastSet is set. 
        if(isset($this->attrs['sambaPwdLastSet'][0])){
            $last = $this->attrs['sambaPwdLastSet'][0];
            $sid = $this->get_domain_info();
            if(isset($sid['sambaMaxPwdAge'][0]) && $sid['sambaMaxPwdAge'][0] != -1){
                $d = ($last + $sid['sambaMaxPwdAge'][0]) - time();

                // A negative value means the password is outdated 
                if($d < 0){
                    $additional_info_PwdMustChange = sprintf(_("The password is outdated since %s, by domain policy."),
                            date("d.m.Y H:i:s",$last + $sid['sambaMaxPwdAge'][0]));
                }else{
                    $additional_info_PwdMustChange = sprintf(_("The password is valid till %s."),
                            date("d.m.Y H:i:s",  ($last + $sid['sambaMaxPwdAge'][0])));
                }
            }
        }

        // Assign flags
        foreach(array("flag_enforcePasswordChange", "flag_passwordNeverExpires", "flag_noPasswordRequired", 
                    "flag_temporaryDisabled","flag_cannotChangePassword","flag_sambaKickoffTime") as $attr){
            $smarty->assign($attr, set_post($this->$attr));
        }

        $smarty->assign("additional_info_PwdMustChange",set_post($additional_info_PwdMustChange));
        $smarty->assign("multiple_support",$this->multiple_support_active);
        $display.= $smarty->fetch (get_template_path('samba3.tpl', TRUE, dirname(__FILE__)));
        return ($display);
    }


    /*! \brief  Returns the samba Domain object, selected in the samba tab.   
     */
    function get_domain_info()
    {
        /* Only search once, return last result if available
         */
        if(!isset($this->cache['DOMAIN'][$this->sambaDomainName])){
            $this->cache['DOMAIN'][$this->sambaDomainName] = array();
            if(!empty($this->sambaDomainName) && isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName])){
                $cfg = $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName];
                $ldap = $this->config->get_ldap_link();
                $ldap->cd($this->config->current['BASE']);
                $ldap->search("(&(objectClass=sambaDomain)(sambaSID=".$cfg['SID']."))",array("*"));
                if($ldap->count()){
                    $this->cache['DOMAIN'][$this->sambaDomainName] = $ldap->fetch();
                }
            }
        }
        return($this->cache['DOMAIN'][$this->sambaDomainName]);
    }



    function get_samba_information()
    {
        $zone = timezone::get_default_timezone();

        /* Defaults 
         */
        $sambaMinPwdLength = "unset";
        $sambaPwdHistoryLength = "unset";
        $sambaLogonToChgPwd = "unset";
        $sambaMaxPwdAge = "unset";
        $sambaMinPwdAge = "unset";
        $sambaLockoutDuration = "unset";
        $sambaLockoutThreshold = "unset";
        $sambaForceLogoff = "unset";
        $sambaRefuseMachinePwdChange = "unset";
        $sambaPwdLastSet = "unset";
        $sambaLogonTime = "unset";
        $sambaLogoffTime = "unset";

        $sambaKickoffTime = "unset"; 
        $sambaPwdCanChange = "unset";
        $sambaPwdMustChange = "unset";
        $sambaBadPasswordCount = "unset";
        $sambaBadPasswordTime = "unset";

        /* Domain attributes 
         */
        $domain_attributes = array("sambaMinPwdLength","sambaPwdHistoryLength","sambaMaxPwdAge",
                "sambaMinPwdAge","sambaLockoutDuration","sambaRefuseMachinePwdChange",
                "sambaLogonToChgPwd","sambaLockoutThreshold","sambaForceLogoff");

        /* User attributes 
         */
        $user_attributes = array("sambaBadPasswordTime","sambaPwdLastSet","sambaLogonTime","sambaLogoffTime",
                "sambaKickoffTime","sambaPwdCanChange","sambaPwdMustChange","sambaBadPasswordCount", "sambaSID");

        /* Get samba SID object and parse settings.
         */  
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        if(!empty($this->sambaDomainName) && isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName])){
            $attrs = $this->get_domain_info();
            foreach($domain_attributes as $attr){
                if(isset($attrs[$attr])){
                    $$attr = $attrs[$attr][0];
                }
            }
        }

        /* Get user infos
         */
        foreach($user_attributes as $attr){
            if(isset($this->attrs[$attr])){
                $$attr = $this->attrs[$attr][0];
            }
        }
        if (is_numeric($sambaPwdMustChange)) {
            $sambaPwdMustChange= date('d.m.Y', $sambaPwdMustChange);
        }
        if (is_numeric($sambaKickoffTime)) {
            $sambaKickoffTime= date('d.m.Y', $sambaKickoffTime);
        }
        $sambaPwdCanChange = $this->sambaPwdCanChange;


        /* DOMAIN Attributes 
         */

        /* sambaMinPwdLength: Password length has a default of 5 
         */
        if($sambaMinPwdLength == "unset" || $sambaMinPwdLength == 5){
            $sambaMinPwdLength  = "5 <i>("._("default").")</i>";
        }

        /* sambaPwdHistoryLength: Length of Password History Entries (default: 0 => off)
         */
        if($sambaPwdHistoryLength == "unset" || $sambaPwdHistoryLength == 0){
            $sambaPwdHistoryLength = _("Off")." <i>("._("default").")</i>";
        }

        /* sambaLogonToChgPwd: Force Users to logon for password change (default: 0 => off, 2 => on) 
         */
        if($sambaLogonToChgPwd == "unset" || $sambaLogonToChgPwd == 0){
            $sambaLogonToChgPwd = _("Off")." <i>("._("default").")</i>";
        }else{
            $sambaLogonToChgPwd = _("On");
        }

        /* sambaMaxPwdAge: Maximum password age, in seconds (default: -1 => never expire passwords)'
         */
        if($sambaMaxPwdAge == "unset" || $sambaMaxPwdAge == "-1"){
            $sambaMaxPwdAge = _("disabled")." <i>("._("default").")</i>";
        }else{
            $sambaMaxPwdAge .= " "._("seconds"); 
        }

        /* sambaMinPwdAge: Minimum password age, in seconds (default: 0 => allow immediate password change
         */
        if($sambaMinPwdAge == "unset" || $sambaMinPwdAge == 0){
            $sambaMinPwdAge = _("disabled")." <i>("._("default").")</i>";
        }else{
            $sambaMinPwdAge .= " "._("seconds"); 
        }

        /* sambaLockoutDuration: Lockout duration in minutes (default: 30, -1 => forever)
         */
        if($sambaLockoutDuration == "unset" || $sambaLockoutDuration == 30){
            $sambaLockoutDuration = "30 "._("minutes")." <i>("._("default").")</i>";
        }elseif($sambaLockoutDuration == -1){
            $sambaLockoutDuration = _("forever");
        }else{
            $sambaLockoutDuration .= " "._("minutes");
        }

        /* sambaLockoutThreshold: Lockout users after bad logon attempts (default: 0 => off
         */
        if($sambaLockoutThreshold == "unset" || $sambaLockoutThreshold == 0){
            $sambaLockoutThreshold = _("disabled")." <i>("._("default").")</i>";
        }

        /* sambaForceLogoff: Disconnect Users outside logon hours (default: -1 => off, 0 => on 
         */
        if($sambaForceLogoff == "unset" || $sambaForceLogoff == -1){
            $sambaForceLogoff = _("off")." <i>("._("default").")</i>";
        }else{
            $sambaForceLogoff = _("on");
        }

        /* sambaRefuseMachinePwdChange: Allow Machine Password changes (default: 0 => off
         */
        if($sambaRefuseMachinePwdChange == "none" || $sambaRefuseMachinePwdChange == 0){
            $sambaRefuseMachinePwdChange = _("off")." <i>("._("default").")</i>";
        }else{
            $sambaRefuseMachinePwdChange = _("on");
        }

        /* USER Attributes 
         */
        /* sambaBadPasswordTime: Time of the last bad password attempt
         */
        if($sambaBadPasswordTime == "unset" || empty($sambaBadPasswordTime)){
            $sambaBadPasswordTime = "<i>("._("unset").")</i>";
        }else{
            $sambaBadPasswordTime = date("d.m.Y H:i:s",$sambaBadPasswordTime);
        }

        /* sambaBadPasswordCount: Bad password attempt count 
         */
        if($sambaBadPasswordCount == "unset" || empty($sambaBadPasswordCount)){
            $sambaBadPasswordCount = "<i>("._("unset").")</i>";
        }else{
            $sambaBadPasswordCount = date("d.m.Y H:i:s",$sambaBadPasswordCount);
        }

        /* sambaPwdLastSet: Timestamp of the last password update
         */
        if($sambaPwdLastSet == "unset" || empty($sambaPwdLastSet)){
            $sambaPwdLastSet = "<i>("._("unset").")</i>";
        }else{
            $sambaPwdLastSet = date("d.m.Y H:i:s",$sambaPwdLastSet);
        }

        /* sambaLogonTime: Timestamp of last logon
         */
        if($sambaLogonTime == "unset" || empty($sambaLogonTime)){
            $sambaLogonTime = "<i>("._("unset").")</i>";
        }else{
            $sambaLogonTime = date("d.m.Y H:i:s",$sambaLogonTime);
        }

        /* sambaLogoffTime: Timestamp of last logoff
         */
        if($sambaLogoffTime == "unset" || empty($sambaLogoffTime)){
            $sambaLogoffTime = "<i>("._("unset").")</i>";
        }else{
            $sambaLogoffTime = date("d.m.Y H:i:s",$sambaLogoffTime);
        }

        /* sambaKickoffTime: Timestamp of when the user will be logged off automatically
         */
        if($sambaKickoffTime == "unset" || empty($sambaKickoffTime)){
            $sambaKickoffTime = "<i>("._("unset").")</i>";
        }

        /* sambaPwdMustChange: Timestamp of when the password will expire
         */
        if($sambaPwdMustChange == "unset" || empty($sambaPwdMustChange)){
            $sambaPwdMustChange = "<i>("._("unset").")</i>";
        }

        /* sambaPwdCanChange: Timestamp of when the user is allowed to update the password
         */
        if($sambaPwdCanChange == "unset" || empty($sambaPwdCanChange)){
            $sambaPwdCanChange = "<i>("._("unset").")</i>";
        }elseif($sambaPwdCanChange != "unset" && time() > $sambaPwdCanChange){
            $sambaPwdCanChange = _("immediately") ;
        }else{
            $days     = floor((($sambaPwdCanChange - time()) / 60 / 60 / 24)) ;
            $hours    = floor((($sambaPwdCanChange - time()) / 60 / 60) % 24) ;
            $minutes  = floor((($sambaPwdCanChange - time()) / 60 ) % 60) ;

            $sambaPwdCanChange = " ".$days." "._("days");
            $sambaPwdCanChange.= " ".$hours." "._("hours");
            $sambaPwdCanChange.= " ".$minutes." "._("minutes");
        }

        $str =
            "\n<div style='height:200px; overflow: auto;'>".
            "\n<table style='width:100%;'>".
            "\n<tr><td><b>"._("Domain attributes")."</b></td></tr>". 
            "\n<tr><td>"._("Min password length").":           </td><td>".$sambaMinPwdLength."</td></tr>". 
            "\n<tr><td>"._("Min password length").":           </td><td>".$sambaMinPwdLength."</td></tr>". 
            "\n<tr><td>"._("Password history").":              </td><td>".$sambaPwdHistoryLength."</td></tr>".
            "\n<tr><td>"._("Force password change").":         </td><td>".$sambaLogonToChgPwd."</td></tr>".
            "\n<tr><td>"._("Maximum password age").":          </td><td>".$sambaMaxPwdAge."</td></tr>".
            "\n<tr><td>"._("Minimum password age").":          </td><td>".$sambaMinPwdAge."</td></tr>".
            "\n<tr><td>"._("Lockout duration").":              </td><td>".$sambaLockoutDuration."</td></tr>".
            "\n<tr><td>"._("Bad lockout attempt").":           </td><td>".$sambaLockoutThreshold."</td></tr>".
            "\n<tr><td>"._("Disconnect time").":               </td><td>".$sambaForceLogoff."</td></tr>".
            "\n<tr><td>"._("Refuse machine password change").":</td><td>".$sambaRefuseMachinePwdChange."</td></tr>".
            "\n<tr><td>&nbsp;</td></tr>". 
            "\n<tr><td><b>"._("User attributes")."</b></td></tr>". 
            "\n<tr><td>"._("SID").":                           </td><td>".$sambaSID."</td></tr>".
            "\n<tr><td>"._("Last failed login").":             </td><td>".$sambaBadPasswordTime."</td></tr>".
            "\n<tr><td>"._("Log on attempts").":                </td><td>".$sambaBadPasswordCount."</td></tr>".
            "\n<tr><td>"._("Last password update").":          </td><td>".$sambaPwdLastSet."</td></tr>".
            "\n<tr><td>"._("Last log on").":                    </td><td>".$sambaLogonTime."</td></tr>".
            "\n<tr><td>"._("Last log off").":                   </td><td>".$sambaLogoffTime."</td></tr>".
            "\n<tr><td>"._("Automatic log off").":              </td><td>".$sambaKickoffTime."</td></tr>";

        if($this->flag_passwordNeverExpires){
            $str .= "\n<tr><td>"._("Password expires").":              </td><td>"._("No")."</td></tr>";
            $str .= "\n<tr><td colspan='2'><font color='gray'>".
                sprintf(_("The password would expire on %s, but the password expiry is disabled."),$sambaPwdMustChange).
                "</font></td></tr>";
        }else{
            $str .= "\n<tr><td>"._("Password expires").":              </td><td>".$sambaPwdMustChange."</td></tr>";
        }

        $str .= "\n<tr><td>"._("Password change available").":     </td><td>".$sambaPwdCanChange."</td></tr>".
            "\n</table>";
        "\n</div>";
        return($str);
    }


    function remove_from_parent()
    {
        /* Cancel if there's nothing to do here */
        if (!$this->initially_was_account){
            return;
        }

        /* include global link_info */
        $ldap= $this->config->get_ldap_link();

        plugin::remove_from_parent();

        /* Keep uid attribute for gosaAccount */
        unset($this->attrs['uid']);
        unset($this->attrs['uidNumber']);
        unset($this->attrs['gidNumber']);

        /* Remove objectClass for sambaIdmapEntry */
        $tmp= array();
        for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
            if ($this->attrs['objectClass'][$i] != 'sambaIdmapEntry'){
                $tmp[]= $this->attrs['objectClass'][$i];
            }
        }
        $this->attrs['objectClass']= $tmp;

        @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
                $this->attributes, "Save");
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify ($this->attrs); 

        new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
        }

        /* Optionally execute a command after we're done */
        $this->handle_post_events("remove", array("uid" => $this->uid));
    }


    /* Check for input problems */
    function check()
    {
        /* Call common method to give check the hook */
        $message= plugin::check();

        if ($this->flag_sambaKickoffTime){
            if (!preg_match("/^[0-3][0-9]\.[0-1][0-9]\.([0-9]){4}$/",$this->sambaKickoffTime)){
                $message[] = msgPool::invalid(_("Account expires after"));
            } elseif (!strtotime($this->sambaKickoffTime) || strtotime($this->sambaKickoffTime) > 2147483647){
                $message[] = msgPool::invalid(_("Account expires after"));
            }
        }

        /* sambaHomePath requires sambaHomeDrive and vice versa */
        if(!empty($this->sambaHomePath) && empty($this->sambaHomeDrive)){
            $message[]= msgPool::required(_("Home drive"));
        }
        if(!empty($this->sambaHomeDrive) && empty($this->sambaHomePath)){
            $message[]= msgPool::required(_("Home path"));
        }

        /* Strings */
        foreach (array( "sambaHomePath" => _("Home directory"),
                    "sambaProfilePath" => _("Profile path")) as $key => $val){
            if (!$this->mungedObject->is_samba_path($this->$key)){
                $message[]= msgPool::invalid($val);
            }
        }

        /* Numeric values */
        foreach (array(	"CtxMaxConnectionTime" => _("Connection"),
                    "CtxMaxDisconnectionTime" => _("Disconnection"),
                    "CtxMaxIdleTime" => _("IDLE")) as $key => $val){

            if (isset($this->mungedObject->ctx[$key]) && !tests::is_id($this->mungedObject->ctx[$key]) && $val != 0){
                $message[]= msgPool::invalid($val);
            }
        }

        /* Too many workstations? Windows usrmgr only supports eight */
        if (substr_count($this->sambaUserWorkstations, ",") >= 8){
            $message[]= _("The windows user manager allows eight clients at maximum!");
        }

        return ($message);
    }


    /* Save data to object */
    function save_object()
    {

        $SkipWrite = (!isset($this->parent) || !$this->parent) && !session::is_set('edit');

        /* We only care if we are on the sambaTab... */
        if (isset($_POST['sambaTab'])){
            plugin::save_object();

            // Display domain info dialog
            if(isset($_POST['display_information'])){
                msg_dialog::display(_("Information"), 
                        $this->get_samba_information(),
                        INFO_DIALOG);
            }

            // Get posted flags.
            foreach(array("enforcePasswordChange", "passwordNeverExpires", "noPasswordRequired",
                        "temporaryDisabled","cannotChangePassword","sambaKickoffTime") as $name){
                $flag = "flag_{$name}";
                if($this->acl_is_writeable($name)){
                    $tmp = isset($_POST[$flag]);
                    $this->is_modified |= ($tmp != $this->$flag);
                    $this->$flag = isset($_POST[$flag]);
                }
            }


            // get sambaDomain attribute
            if ($this->acl_is_writeable("sambaDomainName",$SkipWrite) && isset ($_POST['sambaDomainName'],$SkipWrite)){
                $this->sambaDomainName= get_post('sambaDomainName');
            }

            // Save CTX values 
            $TsAcl = $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite);
            foreach($this->ctxattributes as $val){
                if (isset($_POST[$val]) && $TsAcl){
                    $this->mungedObject->ctx[$val]= get_post($val);
                }
            }

            $this->mungedObject->setTsLogin(!isset($_POST['tslogin']) &&  $TsAcl);

            // Need to do some index checking to avoid messages like "index ... not found"
            if(isset($_POST['brokenconn'])) {
                $this->mungedObject->setBrokenConn($_POST['brokenconn'] == '1' && $TsAcl);
            }
            if(isset($_POST['reconn'])) {
                $this->mungedObject->setReConn($_POST['reconn'] == '1' && $TsAcl);
            }
            $this->mungedObject->setInheritMode(isset($_POST['inherit'])  && $TsAcl);
            $this->mungedObject->setCtxMaxConnectionTimeF(!isset($_POST['CtxMaxConnectionTimeF']) && $TsAcl);
            $this->mungedObject->setCtxMaxDisconnectionTimeF(!isset($_POST['CtxMaxDisconnectionTimeF']) && $TsAcl);
            $this->mungedObject->setCtxMaxIdleTimeF(!isset($_POST['CtxMaxIdleTimeF']) && $TsAcl);
            $this->mungedObject->setConnectClientDrives(isset($_POST['connectclientdrives']) && $TsAcl);
            $this->mungedObject->setConnectClientPrinters(isset($_POST['connectclientprinters']) && $TsAcl);
            $this->mungedObject->setDefaultPrinter(isset($_POST['defaultprinter']) && $TsAcl);

            // Save combo boxes. Takes two values 
            if(isset($_POST['reconn'])) {
                $this->mungedObject->setShadow(isset($_POST['shadow']) && $TsAcl,get_post('shadow'));
            }

            // Check for changes
            $this->is_modified |= ($this->sambaMungedDial != $this->mungedObject->getMunged()); 
        }
    }



    function save()
    {
        /* Load uid and gid of this 'dn' */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn, array('uidNumber', 'gidNumber'));
        $tmp= $ldap->fetch();
        $this->uidNumber= $tmp['uidNumber'][0];
        $this->gidNumber= $tmp['gidNumber'][0];

        plugin::save();

        /* Remove objectClass for sambaIdmapEntry */
        $tmp= array();
        for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
            if ($this->attrs['objectClass'][$i] != 'sambaIdmapEntry'){
                $tmp[]= $this->attrs['objectClass'][$i];
            }
        }
        $this->attrs['objectClass']= $tmp;

        // Handle "sambaKickoffTime" flag.
        if($this->flag_sambaKickoffTime){
            $this->attrs['sambaKickoffTime'] = strtotime($this->sambaKickoffTime);
        }else{
            $this->attrs['sambaKickoffTime']= array();
        }

        // Handle "enforce password change" flag.
        if($this->flag_enforcePasswordChange){
            $this->attrs['sambaPwdMustChange'] = 0;
        }else{

            // Keep old values if given.
            if ($this->sambaPwdMustChange != "0"){
                $this->attrs['sambaPwdMustChange']= $this->sambaPwdMustChange;
            } else {
                $this->attrs['sambaPwdMustChange']= array();
            }
        }

        // Handle "Cannot change password" flag.
        if($this->flag_cannotChangePassword){
            $this->attrs['sambaPwdCanChange'] = 4294967295;
        }else{

            // Keep old values if given.
            if ($this->sambaPwdCanChange != 4294967295 && !empty($this->sambaPwdCanChange)){
                $this->attrs['sambaPwdCanChange']= $this->sambaPwdCanChange;
            } else {
                $this->attrs['sambaPwdCanChange']= array();
            }
        }

        // Create sambaAcctFlags
        $tmp = "U";
        if($this->flag_passwordNeverExpires)  $tmp .="X";
        if($this->flag_noPasswordRequired)    $tmp .="N";
        if($this->flag_temporaryDisabled){
            if(preg_match("/L/i", $this->sambaAcctFlags)){
                $tmp .= "L";
            }else{
                $tmp .= "D";
            }
        }

        // Fill flag list with whitespaces
        $fill= "";
        for ($i= strlen($tmp); $i<12; $i++){
            $fill.= " ";
        }
        $this->attrs['sambaAcctFlags'] = "[{$tmp}{$fill}]";


        // Generate rid / primaryGroupId 
        if (!isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])){
            msg_dialog::display(_("Warning"), _("Undefined Samba SID detected. Please fix this problem manually!"), WARNING_DIALOG);
        } else {
            $this->SID= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
            $this->ridBase= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'];
        }

        // Need to generate a new uniqe uid/gid combination? 
        if ($this->sambaSID == "" || $this->orig_sambaDomainName != $this->sambaDomainName){
            $uidNumber= $this->uidNumber;
            while(TRUE){
                $sid= $this->SID."-".($uidNumber*2 + $this->ridBase);
                $ldap->cd($this->config->current['BASE']);
                $ldap->search("(sambaSID=$sid)", array("sambaSID"));
                if ($ldap->count() == 0){
                    break;
                }
                $uidNumber++;
            }
            $this->attrs['sambaSID']= $sid;

            // Check for users primary group 
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(&(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))", array("cn"));
            if ($ldap->count() != 1){
                msg_dialog::display(_("Warning"), 
                        _("Cannot convert primary group to samba group: group cannot be identified!"), 
                        WARNING_DIALOG);
            } else {
                $attrs= $ldap->fetch();
                $g= new group($this->config, $ldap->getDN());
                if ($g->sambaSID == ""){
                    $g->sambaDomainName= $this->sambaDomainName;
                    $g->smbgroup= TRUE;
                    $g->save ();
                }
                $this->attrs['sambaPrimaryGroupSID']= $g->sambaSID;
            }
        }

        // Set or reset homeDrive - Why is this done seperataly?
        if ($this->sambaHomeDrive == ""){
            $this->attrs["sambaHomeDrive"]= array();
        }

        // Generate munged dial value 
        $this->attrs["sambaMungedDial"]= $this->mungedObject->getMunged();

        // User wants me to fake the idMappings? This is useful for
        //  making winbind resolve the user names in a reasonable amount
        //  of time in combination with larger databases. 
        if ($this->config->boolValueIsTrue("core","sambaidmapping")){
            $this->attrs['objectClass'][]= "sambaIdmapEntry";
        }

        // Write back to ldap 
        $ldap->cd($this->dn);
        $this->cleanup();
        $ldap->modify ($this->attrs); 
        if (!$ldap->success()){
            msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
        }else{
            if ($this->initially_was_account == $this->is_account){
                if ($this->is_modified){
                    $this->handle_post_events("modify", array("uid" => $this->uid));
                    new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
                }
            } else {
                $this->handle_post_events("add", array("uid" => $this->uid));
                new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
            }
        }
    }


    /* Force password set, if this account doesn't have any samba passwords  */
    function password_change_needed()
    {
        if(!$this->initially_was_account && $this->is_account){
            $ldap = $this->config->get_ldap_link();
            $ldap->cat($this->dn,array("sambaLMPassword","sambaNTPassword"));
            $attrs = $ldap->fetch();
            if(!isset($attrs['sambaLMPassword']) || !isset($attrs['sambaNTPassword'])){
                return(TRUE);
            }
        }
        return(FALSE);
    }


    function adapt_from_template($dn, $skip= array())
    {
        plugin::adapt_from_template($dn, $skip);


        $this->sambaSID= "";
        $this->sambaPrimaryGroupSID= "";

        /* Fill mungedDial field */
        if (isset($this->attrs['sambaMungedDial']) && !in_array_strict('sambaMungedDial', $skip)){
            $this->mungedObject->load($this->sambaMungedDial);
        }

        /* Adapt munged attributes */
        foreach($this->ctxattributes as $attr){
            if(isset($this->mungedObject->ctx[$attr]))
                $val = $this->mungedObject->ctx[$attr];

            foreach (array("sn", "givenName", "uid") as $repl){
                if (preg_match("/%$repl/i", $val)){
                    $val= preg_replace ("/%$repl/i", $this->parent->$repl, $val);
                }
            }
            $this->mungedObject->ctx[$attr] = $val;
        }

        // Load flags from source.
        $this->loadFlagsFromSource($this->attrs);

        /* Convert sambaKickoffTime for usage */
        if ($this->sambaKickoffTime == 2147483647){
            $this->sambaKickoffTime = "";
        } elseif (!$this->sambaKickoffTime == "") {
            $this->sambaKickoffTime = date("d.m.Y", $this->sambaKickoffTime);
        }

        /* Adjust sambaKickoffTime checkbox */
        if (!$this->sambaKickoffTime == ""){
            $this->flag_sambaKickoffTime = TRUE;
        }
    }


    static function plInfo()
    {
        return (array(
                    "plShortName"     => _("Samba"),
                    "plDescription"   => _("Samba settings"),
                    "plSelfModify"    => TRUE,
                    "plDepends"       => array("user"),
                    "plPriority"      => 5,
                    "plSection"     => array("personal" => _("My account")),
                    "plCategory"    => array("users"),
                    "plOptions"       => array(),
                    "plRequirements"=>array(
                        'ldapSchema' => array('sambaSamAccount' =>''),
                        'onFailureDisablePlugin' => array(get_class())),
                    "plProvidedAcls"  => array(
                        "sambaHomePath"               => _("Generic home directory") ,
                        "sambaHomeDrive"              => _("Generic samba home drive") ,
                        "sambaDomainName"             => _("Domain") ,
                        "sambaLogonScript"            => _("Generic script path") ,
                        "sambaProfilePath"            => _("Generic profile path") ,
                        "AllowLoginOnTerminalServer"  => _("Allow login on terminal server"),
                        "InheritClientConfig"         => _("Inherit client configuration"),
                        "sambaKickoffTime"            => _("Account expires"), 
                        "enforcePasswordChange"       => _("Enforce password change"),
                        "cannotChangePassword"        => _("Disallow password change") ,
                        "noPasswordRequired"          => _("Login from windows client requires no password"),
                        "passwordNeverExpires"        => _("Password never expires"),
                        "temporaryDisabled"           => _("Lock samba account"),

                        "sambaLogonHours"             => _("Log on hours") ,
                        "sambaUserWorkstations"       => _("Allow connection from"))
                        ));
    }    

    function enable_multiple_support()
    {
        plugin::enable_multiple_support();
        $this->multiple_support_active = TRUE;
    } 

    function multiple_save_object()
    {
        if (isset($_POST['sambaTab'])){
            $this->save_object();
            plugin::multiple_save_object();
            foreach(array("allow_pwchange","tslogin","CtxWFHomeDir","CtxWFHomeDirDrive","CtxWFProfilePath",
                        "inherit","CtxWorkDirectory","CtxInitialProgram","CtxMaxConnectionTimeF",
                        "CtxMaxConnectionTime","CtxMaxDisconnectionTimeF",
                        "CtxMaxDisconnectionTime","CtxMaxIdleTimeF","CtxMaxIdleTime","connectclientdrives",
                        "onnectclientprinters","defaultprinter","shadow","brokenconn",
                        "reconn","connectclientprinters","SetSambaLogonHours","workstation_list",
                        "enforcePasswordChange", "passwordNeverExpires", "noPasswordRequired",
                        "temporaryDisabled","cannotChangePassword","sambaKickoffTime"
                        ) as $attr){
                if(isset($_POST["use_".$attr]) || isset($_POST["use_flag_".$attr]) ){
                    $this->multi_boxes[] = $attr;
                }
            }
        }
    }


    function multiple_check()
    {
        $message = plugin::multiple_check();

        /* Strings */
        foreach (array( "sambaHomePath" => _("Home directory"),
                    "sambaProfilePath" => _("Profile path")) as $key => $val){
            if (in_array_strict($key,$this->multi_boxes) && !$this->mungedObject->is_samba_path($this->$key)){
                $message[]= msgPool::invalid($val);
            }
        }

        /* Numeric values */
        foreach (array( "CtxMaxConnectionTime"    => _("Connection"),
                    "CtxMaxDisconnectionTime" => _("Disconnection"),
                    "CtxMaxIdleTime"          => _("IDLE")) as $key => $val){
            if (in_array_strict($key,$this->multi_boxes) && 
                    isset($this->mungedObject->ctx[$key]) && 
                    !tests::is_id($this->mungedObject->ctx[$key]) && $val != 0){
                $message[]=msgPool::invalid($val);
            }
        }

        /* Too many workstations? Windows usrmgr only supports eight */
        if (substr_count($this->sambaUserWorkstations, ",") >= 8){
            $message[]= _("The windows user manager only allows eight clients. You've specified more than eight.");
        }


        if(in_array_strict("sambaKickoffTime", $this->multi_boxes) && $this->flag_sambaKickoffTime){
            if(!preg_match("/^[0-3][0-9]\.[0-1][0-9]\.([0-9]){4}$/",$this->sambaKickoffTime)){
                $message[] = msgPool::invalid(_("Account expires after"));
            } elseif (!strtotime($this->sambaKickoffTime) || strtotime($this->sambaKickoffTime) > 2147483647){
                $message[] = msgPool::invalid(_("Account expires after"));
            }
        }
        return($message);
    }


    function get_multi_init_values()
    {
        $ret = plugin::get_multi_init_values();

        /* Parse given sambaUserWorkstations into array
         *  to allow "init_multiple_support()" to detect multiple used workstations.
         *  Those workstations will be displayed in light grey.
         */
        $tmp2 = array("count" => 0);
        $tmp = explode(",", $this->sambaUserWorkstations);
        foreach($tmp as $station){
            $station = trim($station);
            if(!empty($station)){
                $tmp2[] = $station;
                $tmp2['count'] ++;
            }
        } 
        $ret['sambaUserWorkstations'] = $tmp2;
        return($ret);
    }



    function init_multiple_support($attrs,$all)
    {
        plugin::init_multiple_support($attrs,$all);

        $this->multiple_sambaUserWorkstations = array();
        if(isset($all['sambaUserWorkstations'])){
            for($i = 0 ; $i < $all['sambaUserWorkstations']['count'] ; $i++){
                $station = trim($all['sambaUserWorkstations'][$i]);
                $this->multiple_sambaUserWorkstations[$station] = array("Name" => $station, "UsedByAllUsers" => FALSE);
            }
        }
        if(isset($attrs['sambaUserWorkstations'])){
            for($i = 0 ; $i < $attrs['sambaUserWorkstations']['count'] ; $i++){
                $station = trim($attrs['sambaUserWorkstations'][$i]);
                $this->multiple_sambaUserWorkstations[$station] = array("Name" => $station, "UsedByAllUsers" => TRUE);
            }
        }

        // Set kickOffTime to date
        if(isset($attrs['sambaKickoffTime'][0])){
            $this->sambaKickoffTime = date("d.m.Y", $this->sambaKickoffTime);
            $this->flag_sambaKickoffTime = TRUE;
        }
    }

    function multiple_execute()
    {
        return($this->execute());
    } 

    function get_multi_edit_values()
    {
        $ret = plugin::get_multi_edit_values();

        /* Terminal Server  */
        if(in_array_strict("tslogin",$this->multi_boxes)){
            $ret['tslogin'] = $this->mungedObject->getTsLogin();
        }
        if(in_array_strict("CtxWFHomeDirDrive",$this->multi_boxes)){
            $ret['CtxWFHomeDirDrive'] = $this->mungedObject->ctx['CtxWFHomeDirDrive'];
        }
        if(in_array_strict("CtxWFHomeDir",$this->multi_boxes)){
            $ret['CtxWFHomeDir'] = $this->mungedObject->ctx['CtxWFHomeDir'];
        }
        if(in_array_strict("CtxWFProfilePath",$this->multi_boxes)){
            $ret['CtxWFProfilePath'] = $this->mungedObject->ctx['CtxWFProfilePath'];
        }

        if(in_array_strict("inherit",$this->multi_boxes)){
            $ret['inherit'] = $this->mungedObject->getInheritMode();
        }       
        if(in_array_strict("CtxInitialProgram",$this->multi_boxes)){
            $ret['CtxInitialProgram'] = $this->mungedObject->ctx['CtxInitialProgram'];
        } 
        if(in_array_strict("CtxWorkDirectory",$this->multi_boxes)){
            $ret['CtxWorkDirectory'] = $this->mungedObject->ctx['CtxWorkDirectory'];
        } 

        /* Time Limits. Be careful here, there are some negations  */
        if(in_array_strict("CtxMaxConnectionTimeF",$this->multi_boxes)){
            $ret["CtxMaxConnectionTimeF"]   =  !$this->mungedObject->getCtxMaxConnectionTimeF();
            if(!$ret["CtxMaxConnectionTimeF"]){
                $ret["CtxMaxConnectionTime"]   =  $this->mungedObject->ctx['CtxMaxConnectionTime'];
            }
        }
        if(in_array_strict("CtxMaxDisconnectionTimeF",$this->multi_boxes)){
            $ret["CtxMaxDisconnectionTimeF"]=  !$this->mungedObject->getCtxMaxDisconnectionTimeF();
            if(!$ret["CtxMaxDisconnectionTimeF"]){
                $ret["CtxMaxDisconnectionTime"]=  $this->mungedObject->ctx['CtxMaxDisconnectionTime'];
            }
        }
        if(in_array_strict("CtxMaxIdleTimeF",$this->multi_boxes)){
            $ret["CtxMaxIdleTimeF"]         =  !$this->mungedObject->getCtxMaxIdleTimeF();
            if(!$ret["CtxMaxIdleTimeF"]){
                $ret["CtxMaxIdleTime"]         =  $this->mungedObject->ctx['CtxMaxIdleTime'];
            }
        }

        /* Client Devices */
        if(in_array_strict("connectclientdrives",$this->multi_boxes)){
            $ret["connectclientdrives"]     =  $this->mungedObject->getConnectClientDrives();
        }
        if(in_array_strict("connectclientprinters",$this->multi_boxes)){
            $ret["connectclientprinters"]   =  $this->mungedObject->getConnectClientPrinters();
        }
        if(in_array_strict("defaultprinter",$this->multi_boxes)){
            $ret["defaultprinter"]          =  $this->mungedObject->getDefaultPrinter();
        }

        /* Misc */
        if(in_array_strict("shadow",$this->multi_boxes)){
            $ret["shadow"]    =$this->mungedObject->getShadow();
        }
        if(in_array_strict("brokenconn",$this->multi_boxes)){
            $ret["brokenconn"]=$this->mungedObject->getBrokenConn();
        }
        if(in_array_strict("reconn",$this->multi_boxes)){
            $ret["reconn"]    =$this->mungedObject->getReConn();
        }

        // Handle Flags.
        foreach(array("flag_enforcePasswordChange", "flag_passwordNeverExpires", "flag_noPasswordRequired",
                    "flag_temporaryDisabled","flag_cannotChangePassword","flag_sambaKickoffTime") as $attr){
            $ret[$attr] = $this->$attr;
        }

        if(in_array_strict("SetSambaLogonHours",$this->multi_boxes)){
            $ret['sambaLogonHours'] = $this->sambaLogonHours;
        }

        if(in_array_strict("workstation_list",$this->multi_boxes)){
            $ret['multiple_sambaUserWorkstations'] = $this->multiple_sambaUserWorkstations;
        }

        return($ret);
    }

    function set_multi_edit_values($values)
    {
        plugin::set_multi_edit_values($values);

        /* Prepare current workstation settings to be merged 
         *  with multiple edit settings.
         */
        if(isset($values['multiple_sambaUserWorkstations'])){
            $cur_ws = array();
            $m_ws = $values['multiple_sambaUserWorkstations'];

            /* Prepare current settings to be merged */
            if(isset($this->sambaUserWorkstations)){
                $ttmp = explode(",",$this->sambaUserWorkstations);
                foreach($ttmp as $station){
                    $station = trim($station);
                    if(!empty($station)){
                        $cur_ws[$station] = array("Name" => $station, "UsedByAllUsers" => TRUE);
                    }
                }
            }

            /* Unset removed workstations */
            foreach($cur_ws as $cur_name => $cur_station){
                if(!isset($m_ws[$cur_name])){
                    unset($cur_ws[$cur_name]);
                }
            }

            /* Add all added workstations */
            foreach($m_ws as $name => $station){
                if($station['UsedByAllUsers']){
                    $cur_ws[$name] = $station;
                }
            }

            $this->sambaUserWorkstations = "";
            foreach($cur_ws as $name => $ws){
                $this->sambaUserWorkstations .= $name.",";
            }
            $this->sambaUserWorkstations=preg_replace("/,$/","",$this->sambaUserWorkstations);
        }

        /* Enable disabled terminal login, this is inverted somehow */
        if(isset($values['tslogin']))   $this->mungedObject->setTsLogin(!$values['tslogin']);

        /* Imherit client configuration */
        if(isset($values['inherit']))   $this->mungedObject->setInheritMode($values['inherit']);

        /* Get all ctx values posted */
        $ctx = array("CtxWFHomeDirDrive","CtxWFHomeDir","CtxWFProfilePath","CtxInitialProgram","CtxWorkDirectory",
                "CtxMaxConnectionTime","CtxMaxDisconnectionTime","CtxMaxIdleTime");
        foreach($ctx as $attr){
            if(isset($values[$attr])){
                $this->mungedObject->ctx[$attr] = $values[$attr] ;
            }
        }

        if(isset($values['CtxMaxConnectionTimeF']))   $this->mungedObject->setCtxMaxConnectionTimeF($values['CtxMaxConnectionTimeF']);
        if(isset($values['CtxMaxDisconnectionTimeF']))$this->mungedObject->setCtxMaxDisconnectionTimeF($values['CtxMaxDisconnectionTimeF']);
        if(isset($values['CtxMaxIdleTimeF']))         $this->mungedObject->setCtxMaxIdleTimeF($values['CtxMaxIdleTimeF']);

        if(isset($values['connectclientdrives']))   $this->mungedObject->setConnectClientDrives($values['connectclientdrives']);
        if(isset($values['connectclientprinters'])) $this->mungedObject->setConnectClientPrinters($values['connectclientprinters']);
        if(isset($values['defaultprinter']))        $this->mungedObject->setDefaultPrinter($values['defaultprinter']);

        if(isset($values['shadow']))        $this->mungedObject->setShadow($values['shadow'],$values['shadow']);
        if(isset($values['brokenconn']))    $this->mungedObject->setBrokenConn($values['brokenconn'],$values['brokenconn']);
        if(isset($values['reconn']))        $this->mungedObject->setReConn($values['reconn'],$values['reconn']);
    }


    function PrepareForCopyPaste($source)
    {
        plugin::PrepareForCopyPaste($source);

        /* Set a new SID */
        $this->sambaSID = "";

        /* Fill mungedDial field */
        if (isset($source['sambaMungedDial'])){
            $this->mungedObject->load($source['sambaMungedDial'][0]);
        }

        // Load flags from source.
        $this->loadFlagsFromSource($source);
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
