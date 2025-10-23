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
    var $plHeadline     = 'Change password';
    var $plDescription  = 'Change user password';

    var $proposal = '';
    var $proposalEnabled = false;
    var $proposalSelected = false;
    var $proposalInitialized = false;

    var $forcedHash = '';
    var $userPassword = '';
    var $attributes = array('userPassword');


    public function __construct($config, $dn = null, $parent = null)
    {
        parent::__construct($config, $dn, $parent);
    }


    public function forceHash($hash)
    {
        $this->forcedHash = $hash;
    }


    function refreshProposal()
    {
        global $config;
        $this->proposal = passwordMethod::getPasswordProposal($config);
        $this->proposalEnabled = !empty($this->proposal);
    }


    function execute()
    {
        global $config;
        parent::execute();
        $smarty = get_smarty();
        $ui = get_userinfo();
        $theme = getThemeName();

        /* Password checks */
        $check_special = true;
        $check_lower = true;
        $check_upper = true;
        $check_digits = true;
        $pwLength = $config->get_cfg_value('core', 'passwordMinLength') != '' ?: 8;

        switch ($theme) {
            case 'classic':
                break;

            default:
                if ($pwLength) {
                    $smarty->assign('length', sprintf(_('The password must have at least %d characters.'), $pwLength));
                }
                if ($check_special) {
                    $smarty->assign('special', _('The password must contain at least 1 special character, e.g.'));
                }
                if ($check_lower) {
                    $smarty->assign('lower', _('The password must contain lower case letters.'));
                }
                if ($check_upper) {
                    $smarty->assign('upper', _('The password must contain upper case letters.'));
                }
                if ($check_digits) {
                    $smarty->assign('digits', _('The password must contain digits.'));
                }
                break;
        }

        // Try to generate a password proposal, if this is successfull 
        //  then preselect the proposal usage. 
        if (!$this->proposalInitialized) {
            $this->refreshProposal();
            if ($this->proposal != '') {
                $this->proposalSelected = true;
            }
            $this->proposalInitialized = true;
        }

        /* Get acls */
        $password_ACLS = $ui->get_permissions($ui->dn, 'users/password');
        $smarty->assign('ChangeACL',  $password_ACLS);
        $smarty->assign('SASL', preg_match("/^{SASL}/i", $this->userPassword));
        $smarty->assign('NotAllowed', !preg_match("/w/i", $password_ACLS));

        /* Display expiration template */
        $smarty->assign("passwordExpired", false);
        if ($config->boolValueIsTrue('core', "handleExpiredAccounts")) {
            $expired = ldap_expired_account($config, $ui->dn, $ui->username);
            $smarty->assign("passwordExpired", $expired & POSIX_FORCE_PASSWORD_CHANGE);
            if ($expired == POSIX_DISALLOW_PASSWORD_CHANGE) {
                return ($smarty->fetch(get_template_path("nochange.tpl", true)));
            }
        }

        // Refresh proposal if requested
        if (isset($_POST['refreshProposal'])) $this->refreshProposal();
        if (isset($_POST['proposalSelected'])) $this->proposalSelected = get_post('proposalSelected') == 1;
        $smarty->assign("proposal", set_post($this->proposal));
        $smarty->assign("proposalEnabled", $this->proposalEnabled);
        $smarty->assign("proposalSelected", $this->proposalSelected);

        /* Pwd change requested */
        if (isset($_POST['password_finish'])) {

            if ($this->proposalSelected) {
                $current_password = get_post('current_password');
                $new_password = $this->proposal;
                $repeated_password = $this->proposal;
            } else {
                $current_password = get_post('current_password');
                $new_password = get_post('new_password');
                $repeated_password = get_post('repeated_password');
            }


            // Get configuration flags for further input checks.
            $check_differ = $config->get_cfg_value('core', "passwordMinDiffer") != '';
            $differ       = $config->get_cfg_value('core', "passwordMinDiffer");
            $length       = $config->get_cfg_value('core', "passwordMinLength") != '' ?: 8;

            switch ($theme) {
                case 'classic':
                    break;

                default:
                    $special        = "/[!,@,#,$,%,^,&,*,?,_,~]/i";
                    $lower          = "/[a-z]/";
                    $upper          = "/[A-Z]/";
                    $digits         = "/[0-9]/i";
                    break;
            }

            // Once an error has occured it is stored here.
            $message = [];
            // Perform GOsa password policy checks 
            if (empty($current_password)) {
                $message[] = _("You need to specify your current password in order to proceed.");
            } elseif ($new_password  != $repeated_password) {
                $message[] = _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
            } elseif ($new_password == '') {
                $message[] = _("The password you've entered as 'New password' is empty.");
            } elseif ($check_differ && (substr($current_password, 0, $differ) == substr($new_password, 0, $differ))) {
                $message[] = _("The password used as new and current are too similar.");
            } elseif ((strlen($new_password) < $length)) {
                $message[] = sprintf(_("The password must have at least %s characters."), $length);
            } elseif ($theme == 'default') {
                if ($check_lower && !preg_match($lower, $new_password)) {
                    $message[] = _("The password must contain lowercase letters.");
                } elseif ($check_upper && !preg_match($upper, $new_password)) {
                    $message[] = _("The password must contain uppercase letters.");
                } elseif ($check_digits && !preg_match($digits, $new_password)) {
                    $message[] = _("The password must contain digits.");
                } elseif ($check_special && !preg_match($special, $new_password)) {
                    $message[] = _("The password must contain at least 1 special character, e.g. !, @, #, $, %, ^, &, *, ?, _, ~.");
                }
            } elseif (!passwordMethod::is_harmless($new_password)) {
                $message[] = _("The password contains possibly problematic Unicode characters!");
            }

            // Call external check hook to validate the password change
            if (!count($message)) {
                $attrs = [];
                $attrs['current_password'] = '';
                $attrs['new_password'] = $new_password;
                $checkRes = password::callCheckHook($config, $this->dn, $attrs);
                if (count($checkRes)) {
                    $message[] = sprintf(
                        _("Check-hook reported a problem: %s. Password change canceled!"),
                        implode($checkRes)
                    );
                }
            }

            // Some errors/warning occured, display them and abort password change.
            if (count($message)) {
                msg_dialog::displayChecks($message);
            } else {

                /* Try to connect via current password */
                $tldap = new LDAP(
                    $ui->dn,
                    $current_password,
                    $config->current['SERVER'],
                    $config->get_cfg_value('core', "ldapFollowReferrals") == "true",
                    $config->get_cfg_value('core', "ldapTLS") == "true"
                );

                /* connection Successfull ? */
                if (!$tldap->success()) {
                    msg_dialog::display(
                        _("Password change"),
                        _("The password you've entered as your current password doesn't match the real one."),
                        WARNING_DIALOG
                    );
                } elseif (!preg_match("/w/i", $password_ACLS)) {
                    msg_dialog::display(
                        _("Password change"),
                        _("You have no permission to change your password."),
                        WARNING_DIALOG
                    );
                } elseif (!change_password($ui->dn, $new_password, true, $this->forcedHash, $current_password, $message)) {
                    msg_dialog::display(
                        _("Password change"),
                        $message,
                        WARNING_DIALOG
                    );
                } else {
                    $ui->password = $new_password;
                    session::set('ui', $ui);
                    return ($smarty->fetch(get_template_path("changed.tpl", true, dirname(__FILE__))));
                }
            }
            $smarty->assign("pwLength", $length);
        }
        return $smarty->fetch(get_template_path("password.tpl", true));
    }

    function remove_from_parent()
    {
        $this->handle_post_events("remove");
    }

    function save() {}

    static function callCheckHook($config, $dn, $attrs = [])
    {
        $command = $config->configRegistry->getPropertyValue("password", "check");
        if (!empty($command)) {

            // Build up ldif to send to the check hook
            $ldif = "dn: $dn\n";
            foreach ($attrs as $name => $value) {
                $ldif .= "{$name}: {$value}\n";
            }
            $ldif .= "\n";

            /* Feed "ldif" into hook and retrieve result*/
            $descriptorspec = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));
            $fh = proc_open($command, $descriptorspec, $pipes);
            if (is_resource($fh)) {
                fwrite($pipes[0], $ldif);
                fclose($pipes[0]);
                $arr = stream_get_contents($pipes[1]);
                $err = stream_get_contents($pipes[2]);
                fclose($pipes[1]);
                fclose($pipes[2]);
                $returnCode = proc_close($fh);

                if ($returnCode !== 0) {
                    DEBUG(DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execution failed code: " . $returnCode);
                    DEBUG(DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: " . $err);
                    $message = msgPool::cmdexecfailed($err, $command, "password");
                    msg_dialog::display(_("Error"), $message, ERROR_DIALOG);
                } else {
                    DEBUG(DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Result: " . $arr);
                    return (preg_split("/\n/", $arr, 0, PREG_SPLIT_NO_EMPTY));
                }
            }
        }
        return [];
    }

    static function plInfo()
    {
        return (array(
            "plDescription" => _("User password"),
            "plSelfModify"  => true,
            "plDepends"     => array("user"),
            "plPriority"    => 10,
            "plSection"     => array("personal" => _("My account")),
            "plCategory"    => array("users"),
            "plOptions"     => [],

            "plProperties" => array(
                array(
                    "name"          => "PRELOCK",
                    "type"          => "command",
                    "default"       => '',
                    "description"   => _("Script to be called before a password gets locked."),
                    "check"         => "gosaProperty::isCommand",
                    "migrate"       => '',
                    "group"         => "password",
                    "mandatory"     => false
                ),
                array(
                    "name"          => "POSTLOCK",
                    "type"          => "command",
                    "default"       => '',
                    "description"   => _("Script to be called after a password gets locked."),
                    "check"         => "gosaProperty::isCommand",
                    "migrate"       => '',
                    "group"         => "password",
                    "mandatory"     => false
                ),
                array(
                    "name"          => "PREUNLOCK",
                    "type"          => "command",
                    "default"       => '',
                    "description"   => _("Script to be called before a password gets unlocked."),
                    "check"         => "gosaProperty::isCommand",
                    "migrate"       => '',
                    "group"         => "password",
                    "mandatory"     => false
                ),
                array(
                    "name"          => "POSTUNLOCK",
                    "type"          => "command",
                    "default"       => '',
                    "description"   => _("Script to be called after a password gets unlocked."),
                    "check"         => "gosaProperty::isCommand",
                    "migrate"       => '',
                    "group"         => "password",
                    "mandatory"     => false
                )
            ),
            "plProvidedAcls"  => []
        )
        );
    }
}
