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
    var $proposalSelected = FALSE;

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
        $smarty->assign("proposal" , set_post($this->proposal));
        $smarty->assign("proposalEnabled" , $this->proposalEnabled);
        $smarty->assign("proposalSelected" , $this->proposalSelected);

        if(isset($_POST['proposalSelected'])) $this->proposalSelected = get_post('proposalSelected') == 1;

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


            /* Should we check different characters in new password */
            $check_differ = $this->config->get_cfg_value("core","passwordMinDiffer") != "";
            $differ       = $this->config->get_cfg_value("core","passwordMinDiffer");

            /* Enable length check ? */
            $check_length = $this->config->get_cfg_value("core","passwordMinLength") != "";
            $length       = $this->config->get_cfg_value("core","passwordMinLength");

            /* Call external password quality hook ?*/
            $check_hook   = $this->config->get_cfg_value("core","passwordHook") != "";
            $hook         = $this->config->get_cfg_value("core","passwordHook")." ".
                        $ui->username." ".$current_password." ".$new_password;

            if($check_hook){
                exec($hook,$resarr);
                $check_hook_output = "";
                if(count($resarr) > 0) {
                    $check_hook_output= join('\n', $resarr);
                }
            }

            /* Check given values */    
            if(empty($current_password)){
                msg_dialog::display(_("Password change"),
                        _("You need to specify your current password in order to proceed."),WARNING_DIALOG);
            }elseif ($new_password  != $repeated_password){
                msg_dialog::display(_("Password change"),
                        _("The passwords you've entered as 'New password' and 'Repeated new password' do not match."),WARNING_DIALOG);
            } elseif ($new_password == ""){
                msg_dialog::display(_("Password change"),
                        _("The password you've entered as 'New password' is empty."),WARNING_DIALOG);
            }elseif($check_differ && (substr($current_password, 0, $differ) == substr($new_password, 0, $differ))){
                msg_dialog::display(_("Password change"),
                        _("The password used as new and current are too similar."),WARNING_DIALOG);
            }elseif($check_length && (strlen($new_password) < $length)){
                msg_dialog::display(_("Password change"),
                        _("The password used as new is to short."),WARNING_DIALOG);
            }elseif(!passwordMethod::is_harmless($new_password)){
                msg_dialog::display(_("Password change"),
                        _("The password contains possibly problematic unicode characters!"),WARNING_DIALOG);
            }elseif($check_hook && $check_hook_output != ""){
                msg_dialog::display(_("Password change"),
                        sprintf(_("External password changer reported a problem: %s."),$check_hook_output),WARNING_DIALOG);
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
#$this->handle_post_events("modify",array("userPassword" => $new_password));
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
