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

class password extends plugin
{
    /* Definitions */
    var $plHeadline     = "Change password";
    var $plDescription  = "Change user password";

    var $proposal = "";
    var $proposalEnabled = FALSE;
    var $proposalSelected = TRUE;

    var $forcedHash = NULL;

    function password(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::plugin($config, $dn, $parent);

        // Try to generate a password proposal, if this is successfull 
        //  then preselect the proposal usage. 
        $this->refreshProposal();
        if($this->proposal != ""){
            $this->proposalSelected = TRUE;
        }
    }

    
    function forceHash($hash)
    {
        $this->forcedHash = $hash;
    }    


    function refreshProposal()
    {
        $this->proposal = passwordMethod::getPasswordProposal($this->config);
        $this->proposalEnabled = (!empty($this->proposal));
    }


    function execute()
    {
        plugin::execute();
        $smarty = get_smarty();
        $ui = get_userinfo();

        /* Get acls */
        $password_ACLS = $ui->get_permissions($ui->dn,"users/password");
        $smarty->assign("ChangeACL" ,  $password_ACLS);
        $smarty->assign("NotAllowed" , !preg_match("/w/i",$password_ACLS));

        /* Display expiration template */
        $smarty->assign("passwordExpired", FALSE);
        if ($this->config->boolValueIsTrue("core","handleExpiredAccounts")){
            $expired= ldap_expired_account($this->config, $ui->dn, $ui->username);
            $smarty->assign("passwordExpired", $expired & POSIX_FORCE_PASSWORD_CHANGE);
            if($expired == POSIX_DISALLOW_PASSWORD_CHANGE){
                return($smarty->fetch(get_template_path("nochange.tpl", TRUE)));
            }
        }
        
        // Refresh proposal if requested
        if(isset($_POST['refreshProposal'])) $this->refreshProposal();
        if(isset($_POST['proposalSelected'])) $this->proposalSelected = get_post('proposalSelected') == 1;
        $smarty->assign("proposal" , set_post($this->proposal));
        $smarty->assign("proposalEnabled" , $this->proposalEnabled);
        $smarty->assign("proposalSelected" , $this->proposalSelected);

        /* Pwd change requested */
        if (isset($_POST['password_finish'])){
    
            if($this->proposalSelected){
                $current_password = get_post('current_password');
                $new_password = $this->proposal;
                $repeated_password = $this->proposal;
            }else{
                $current_password = get_post('current_password');
                $new_password = get_post('new_password');
                $repeated_password = get_post('repeated_password');
            }


            // Get configuration flags for further input checks.
            $check_differ = $this->config->get_cfg_value("core","passwordMinDiffer") != "";
            $differ       = $this->config->get_cfg_value("core","passwordMinDiffer");
            $check_length = $this->config->get_cfg_value("core","passwordMinLength") != "";
            $length       = $this->config->get_cfg_value("core","passwordMinLength");

            // Once an error has occured it is stored here.
            $message = array();

            // Call the check hook
            $attrs = array();
            $attrs['current_password'] = escapeshellarg($current_password);
            $attrs['new_password'] = escapeshellarg($new_password);

            // Depricated but execute for backward compability
            $check_hook   = $this->config->get_cfg_value("core","passwordHook") != "";
            $cmd = $this->config->get_cfg_value("core","passwordHook");
            if(!empty($cmd)){
                $cmd = preg_replace("/%current_password/",escapeshellarg($current_password), $cmd);
                $cmd = preg_replace("/%new_password/",escapeshellarg($new_password), $cmd);
                $cmd = preg_replace("/%uid/",escapeshellarg($ui->username), $cmd);
                $cmd = preg_replace("/%dn/",escapeshellarg($ui->dn), $cmd);
                if($check_hook){
                    exec($cmd,$resarr);
                    $check_hook_output = "";
                    if(count($resarr) > 0) {
                        $check_hook_output= join('\n', $resarr);
                        $message[] = sprintf(_("Password hook reported a problem: %s. Password change canceled!"),
                                $check_hook_output);
                    }
                }
            }

            // Perform GOsa password policy checks 
            if(empty($current_password)){
                $message[] = _("You need to specify your current password in order to proceed.");
            }elseif($new_password  != $repeated_password){
                $message[] = _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
            }elseif($new_password == ""){
                $message[] = _("The password you've entered as 'New password' is empty.");
            }elseif($check_differ && (substr($current_password, 0, $differ) == substr($new_password, 0, $differ))){
                $message[] = _("The password used as new and current are too similar.");
            }elseif($check_length && (strlen($new_password) < $length)){
                $message[] = _("The password used as new is to short.");
            }elseif(!passwordMethod::is_harmless($new_password)){
                $message[] = _("The password contains possibly problematic Unicode characters!");
            }

            // Call external check hook to validate the password change
            if(!count($message)){
                $checkRes = $this->callCheckHook($attrs);
                if(count($checkRes)){
                    $message[] = sprintf(_("Check-hook reported a problem: %s. Password change canceled!"),implode($checkRes));
                }
            }

            // Call the pre-event command and check its return code
            if(!count($message)){
                plugin::callHook($this, 'PREMODIFY', $attrs, $output,$retCode,$error, $directlyPrintError = FALSE);
                if($retCode === 0 && count($output)){
                    $message[] = sprintf(_("Pre-event hook reported a problem: %s. Password change canceled!"),implode($output));
                }
            }


            // Some errors/warning occured, display them and abort password change.
            if(count($message)){
                msg_dialog::displayChecks($message);
            }else{ 

                /* Try to connect via current password */
                $tldap = new LDAP(
                        $ui->dn, 
                        $current_password,
                        $this->config->current['SERVER'],
                        $this->config->get_cfg_value("core","ldapFollowReferrals") == "true",
                        $this->config->get_cfg_value("core","ldapTLS") == "true");

                /* connection Successfull ? */
                if (!$tldap->success()){
                    msg_dialog::display(_("Password change"),
                            _("The password you've entered as your current password doesn't match the real one."),WARNING_DIALOG);
                }else{

                    /* Check GOsa permissions */
                    if (!preg_match("/w/i",$password_ACLS)){
                        msg_dialog::display(_("Password change"),
                                _("You have no permission to change your password."),WARNING_DIALOG);
                    }else{
                        $this->change_password($ui->dn, $new_password, $this->forcedHash);
                        gosa_log ("User/password has been changed");
                        $ui->password= $new_password;
                        session::set('ui',$ui);
                        return($smarty->fetch(get_template_path("changed.tpl", TRUE)));
                    }
                }
            }
        }
        return($smarty->fetch(get_template_path("password.tpl", TRUE)));
    } 

    function change_password($dn, $pwd, $hash)
    {
        if(!$hash){
            change_password ($dn, $pwd);
        }else{
            change_password ($dn, $pwd,0, $hash);
        }
    }

    function remove_from_parent()
    {
        $this->handle_post_events("remove");
    }

    function save()
    {
    }

    function callCheckHook($attrs = array())
    {
        $command = $this->config->configRegistry->getPropertyValue(get_class($this),"check");
        if (!empty($command)){

            // Build up ldif to send to the check hook
            $ldif= "dn: $this->dn\n";
            foreach ($attrs as $name => $value){
                $ldif.= "{$name}: {$value}\n";
            }
            $ldif.= "\n";

            /* Feed "ldif" into hook and retrieve result*/
            $descriptorspec = array( 0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));
            $fh= proc_open($command, $descriptorspec, $pipes);
            if (is_resource($fh)) {
                fwrite ($pipes[0], $ldif);
                fclose($pipes[0]);
                $arr= stream_get_contents($pipes[1]);
                $err = stream_get_contents($pipes[2]);
                fclose($pipes[1]);
                fclose($pipes[2]);
                $returnCode = proc_close($fh);

                if($returnCode !== 0){
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execution failed code: ".$returnCode);
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: ".$err);
                    if($displayErrors){
                        $message= msgPool::cmdexecfailed($cmd,$command, get_class($plugin));
                        msg_dialog::display(_("Error"), $message, ERROR_DIALOG);
                    }
                }else{
                    @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: ".$arr);
                    return(preg_split("/\n/", $arr,0,PREG_SPLIT_NO_EMPTY));
                }
            }
        }
        return(array());
    }

    static function plInfo()
    {
        return (array(
                    "plDescription"     => _("User password"),
                    "plSelfModify"      => TRUE,
                    "plDepends"         => array("user"),
                    "plPriority"        => 10,
                    "plSection"     => array("personal" => _("My account")),
                    "plCategory"    => array("users"),
                    "plOptions"         => array(),

                    "plProvidedAcls"  => array())
               );
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
