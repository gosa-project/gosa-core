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
    var $plHeadline     = "Password";
    var $plDescription  = "Change user password";

    function password(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::plugin($config, $dn, $parent);
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
        if ($this->config->get_cfg_value("handleExpiredAccounts") == "true"){
            $expired= ldap_expired_account($this->config, $ui->dn, $ui->username);
            if($expired == 4){
                return($smarty->fetch(get_template_path("nochange.tpl", TRUE)));
            }
        }

        /* Pwd change requested */
        if (isset($_POST['password_finish'])){

            /* Should we check different characters in new password */
            $check_differ = $this->config->get_cfg_value("passwordMinDiffer") != "";
            $differ       = $this->config->get_cfg_value("passwordMinDiffer", 0);

            /* Enable length check ? */
            $check_length = $this->config->get_cfg_value("passwordMinLength") != "";
            $length       = $this->config->get_cfg_value("passwordMinLength", 0);

            // Validate input
            $message = array();
            if(!isset($_POST['current_password']) || empty($_POST['current_password'])){
                $message[] = _("You need to specify your current password in order to proceed.");
            }elseif ($_POST['new_password'] != $_POST['repeated_password']){
                $message[] = _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
            } elseif ($_POST['new_password'] == ""){
                $message[] = _("The password you've entered as 'New password' is empty.");
            }elseif($check_differ && (substr($_POST['current_password'], 0, $differ) == substr($_POST['new_password'], 0, $differ))){
                $message[] = _("The password used as new and current are too similar.");
            }elseif($check_length && (strlen($_POST['new_password']) < $length)){
                $message[] = _("The password used as new is to short.");
            }
            
            // No errors yet, so call the external password hook.
            if(!count($message)){
                $check_hook   = $this->config->get_cfg_value("passwordHook") != "";
                $hook         = $this->config->get_cfg_value("passwordHook")." ".
                    $ui->username." ".$_POST['current_password']." ".$_POST['new_password'];
                if($check_hook){
                    exec($hook,$resarr);
                    $check_hook_output = "";
                    if(count($resarr) > 0) {
                        $check_hook_output= join('\n', $resarr);
                    }
                    $check_hook_output= sprintf(_("External password changer reported a problem: %s."),$check_hook_output);
                    if(!empty($check_hook_output))  $message[] = $check_hook_output;
                }
            }

            if(count($message)){
                msg_dialog::displayChecks($message);
            }else{

                /* Try to connect via current password */
                $tldap = new LDAP(
                        $ui->dn, 
                        $_POST['current_password'],
                        $this->config->current['SERVER'],
                        $this->config->get_cfg_value("ldapFollowReferrals") == "true",
                        $this->config->get_cfg_value("ldapTLS") == "true");

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
                        change_password ($ui->dn, $_POST['new_password']);
                        gosa_log ("User/password has been changed");
                        $ui->password= $_POST['new_password'];
                        session::set('ui',$ui);
                        return($smarty->fetch(get_template_path("changed.tpl", TRUE)));
                    }
                }
            }
        }
        return($smarty->fetch(get_template_path("password.tpl", TRUE)));
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
