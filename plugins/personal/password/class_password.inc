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

    function password(&$config, $dn= NULL, $parent= NULL)
    {
        plugin::plugin($config, $dn, $parent);

        $this->reloadProposal();
    }


    function reloadProposal()
    {
        $this->proposal = passwordMethod::getPasswordProposal($this->config);
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
        if ($this->config->get_cfg_value("core","handleExpiredAccounts") == "true"){
            $expired= ldap_expired_account($this->config, $ui->dn, $ui->username);
            if($expired == 4){
                return($smarty->fetch(get_template_path("nochange.tpl", TRUE)));
            }
        }

        /* Pwd change requested */
        if (isset($_POST['password_finish'])){

            /* Should we check different characters in new password */
            $check_differ = $this->config->get_cfg_value("core","passwordMinDiffer") != "";
            $differ       = $this->config->get_cfg_value("core","passwordMinDiffer");

            /* Enable length check ? */
            $check_length = $this->config->get_cfg_value("core","passwordMinLength") != "";
            $length       = $this->config->get_cfg_value("core","passwordMinLength");

            /* Call external password quality hook ?*/
            $check_hook   = $this->config->get_cfg_value("core","passwordHook") != "";
            $hook         = $this->config->get_cfg_value("core","passwordHook")." ".$ui->username." ".$_POST['current_password']." ".$_POST['new_password'];
            if($check_hook){
                exec($hook,$resarr);
                $check_hook_output = "";
                if(count($resarr) > 0) {
                    $check_hook_output= join('\n', $resarr);
                }
            }

            /* Check given values */    
            if(!isset($_POST['current_password']) || empty($_POST['current_password'])){
                msg_dialog::display(_("Password change"),
                        _("You need to specify your current password in order to proceed."),WARNING_DIALOG);
            }elseif ($_POST['new_password'] != $_POST['repeated_password']){
                msg_dialog::display(_("Password change"),
                        _("The passwords you've entered as 'New password' and 'Repeated new password' do not match."),WARNING_DIALOG);
            } elseif ($_POST['new_password'] == ""){
                msg_dialog::display(_("Password change"),
                        _("The password you've entered as 'New password' is empty."),WARNING_DIALOG);
            }elseif($check_differ && (substr($_POST['current_password'], 0, $differ) == substr($_POST['new_password'], 0, $differ))){
                msg_dialog::display(_("Password change"),
                        _("The password used as new and current are too similar."),WARNING_DIALOG);
            }elseif($check_length && (strlen($_POST['new_password']) < $length)){
                msg_dialog::display(_("Password change"),
                        _("The password used as new is to short."),WARNING_DIALOG);
            }elseif(!passwordMethod::is_harmless($_POST['new_password'])){
                msg_dialog::display(_("Password change"),
                        _("The password contains possibly problematic unicode characters!"),WARNING_DIALOG);
            }elseif($check_hook && $check_hook_output != ""){
                msg_dialog::display(_("Password change"),
                        sprintf(_("External password changer reported a problem: %s."),$check_hook_output),WARNING_DIALOG);
            }else{

                /* Try to connect via current password */
                $tldap = new LDAP(
                        $ui->dn, 
                        $_POST['current_password'],
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
                        $this->change_password($ui->dn, $_POST['new_password']);
                        gosa_log ("User/password has been changed");
                        $ui->password= $_POST['new_password'];
                        session::set('ui',$ui);
#$this->handle_post_events("modify",array("userPassword" => $_POST['new_password']));
                        return($smarty->fetch(get_template_path("changed.tpl", TRUE)));
                    }
                }
            }
        }
        return($smarty->fetch(get_template_path("password.tpl", TRUE)));
    } 

    function change_password($dn, $pwd)
    {
        change_password ($dn, $pwd);
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
