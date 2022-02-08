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

class Step_Policy extends setup_step
{
    var $cv = array();

    var $policy = FALSE;
    var $policy_config = FALSE;
    var $policy_ou = '';
    var $policy_dn = '';

    var $policy_settings = array();

    var $b_displayCheckbutton = TRUE;


    function __construct()
    {
        $this->update_strings();
    }

    function update_strings()
    {
        $this->s_title      = _("Policy setup");
        $this->s_title_long = _("Policy configuration setup");
        $this->s_info       = _("This dialog is used to configure the password policy for GOsa.");
    }

    function execute()
    {
        $smarty = get_smarty();

        $this->cv = $this->parent->captured_values;

        if (isset($_POST['policy'])) {
            $this->policy = TRUE;
            $this->load_policy_module();
            $this->set_policy_container($this->policy);
            $this->set_policy_overlay($this->policy);
        } else {
            $this->policy = FALSE;
            if (!isset($_POST['create_policy'])) {
                $this->set_policy_overlay($this->policy);
                $this->set_policy_container($this->policy);
            }
        }

        if (isset($_POST['config_policy'])) {
            $this->dialog = TRUE;
            $this->policy_config = TRUE;
            $this->policy_settings = $this->init_default_policy_config();

            $smarty->assign('pwdFailureCountInterval', $this->policy_settings['pwdFailureCountInterval']);
            $smarty->assign('pwdInHistory', $this->policy_settings['pwdInHistory']);
            $smarty->assign('pwdMaxFailure', $this->policy_settings['pwdMaxFailure']);
            $smarty->assign('pwdLockoutDuration', $this->policy_settings['pwdLockoutDuration']);
            $smarty->assign('pwdMinLength', $this->policy_settings['pwdMinLength']);
        }

        if (isset($_POST['create_policy_cancel'])) {
            $this->dialog = FALSE;
            $this->policy_config = FALSE;
            $this->policy = TRUE;
        }

        if (isset($_POST['create_policy'])) {
            $this->dialog = FALSE;
            $this->policy_config = FALSE;
            $this->policy = TRUE;
            $this->create_default_policies();
        }

        $smarty->assign('policy', $this->policy);
        $smarty->assign('policy_config', $this->policy_config);
        return ($smarty->fetch(get_template_path("../setup/" . $GLOBALS['theme'] . "/setup_policy.tpl")));
    }

    function save_object()
    {
        $this->is_completed = TRUE;
    }

    function load_policy_module()
    {
        $policy_dn = 'cn=module{0},cn=config';
        $policy_module['olcModuleLoad'] = 'ppolicy';

        $ldap = $this->auth_ldap('cn=config', $this->cv['password'], $this->cv['connection'], $this->cv['tls']);
        $ldap->cd($policy_dn);

        $ldap->mod_add($policy_module, $policy_dn);
    }

    function set_policy_container($create)
    {
        $this->policy_ou = 'ou=policies,' . 'ou=configs,' . 'ou=systems,' . $this->cv['base'];;

        $ldap = $this->auth_ldap($this->cv['admin'], $this->cv['password'], $this->cv['connection'], $this->cv['tls']);
        $ldap->cd($this->cv['base']);

        if ($create) {
            $ldap->create_missing_trees($this->policy_ou);
        } else {
            $pt = $this->policy_ou;

            foreach (explode(',', $pt) as $value) {
                if (!in_array($value, explode(',', $this->cv['base']))) {
                    $ldap->rmdir($pt);
                    $pt = str_replace($value . ',', '', $pt);
                }
            }
        }
    }

    function set_policy_overlay($create)
    {
        $this->policy_dn = 'cn=default,' . $this->policy_ou;

        $conf_dn = 'olcOverlay=ppolicy,olcDatabase={1}mdb,cn=config';
        $policy_overlay = array(
            'objectClass' => 'olcOverlayConfig',
            'objectClass' => 'olcPpolicyConfig',
            'olcOverlay' => 'ppolicy',
            'olcPPolicyDefault' => $this->policy_dn,
            'olcPPolicyHashCleartext' => 'TRUE',
        );

        $ldap = $this->auth_ldap('cn=config', $this->cv['password'], $this->cv['connection'], $this->cv['tls']);
        $ldap->cd($conf_dn);

        if ($create) {
            $ldap->add($policy_overlay);
        } else {
            $ldap->rmdir($conf_dn);
        }
    }

    function create_default_policies()
    {
        $change_replace = array();

        $ldap = $this->auth_ldap($this->cv['admin'], $this->cv['password'], $this->cv['connection'], $this->cv['tls']);
        $ldap->cd($this->policy_dn);

        //Write POST back to policy_settings
        foreach ($_POST as $key => $value) {
            if (array_key_exists($key, $this->policy_settings)) {
                if ($value != $this->policy_settings[$key]) {
                    $change_replace[$key] = $value;
                }
                $this->policy_settings[$key] = $value;
            }
        }

        if ($ldap->dn_exists($this->policy_ou) != "") {
            if ($ldap->dn_exists($this->policy_dn) != "") {
                $ldap->rename($change_replace);
            } else {
                $ldap->add($this->policy_settings, $this->policy_dn);
            }
        } else {
            msg_dialog::display(
                _("Internal error"),
                sprintf('The container for the default policies "%s" is not present,
                try to re-enable the policy. If this does not solve the problem,
                please contact an administrator.', $this->policy_ou),
                ERROR_DIALOG
            );
        }
    }

    function init_default_policy_config()
    {
        $settings = array();

        $ldap = $this->auth_ldap($this->cv['admin'], $this->cv['password'], $this->cv['connection'], $this->cv['tls']);

        if ($ldap->dn_exists($this->policy_dn) != "") {
            $ldap->cd($this->policy_dn);
            $ldap->search('(objectClass=*)', array('*'));

            $result = $ldap->fetch();

            foreach ($result as $key => $value) {
                if (!is_numeric($key)) {
                    if ($key == 'objectClass') {
                        $settings[$key] = $value;
                    } elseif ($key != 'dn' && $key != 'count') {
                        $settings[$key] = $value[0];
                    }
                }
            }

            return $settings;
        } else {
            return array(
                'objectClass' => array(
                    'device',
                    'pwdPolicyChecker',
                    'pwdPolicy'
                ),
                'pwdAllowUserChange' => 'TRUE',     // Specifies whether users are allowed to change their own passwords or not.
                'pwdAttribute' => 'userPassword',   // Name of the attribute to which the password policy is applied - NOTE: the only value accepted is 'userPassword'.
                'pwdCheckQuality' => '1',           /* Indicates if and how password syntax will be checked while a password is being modified or added.
                                                            * - 0 = No syntax checking.
                                                            * - 1 = Syntax checking, if the server is unable to check the syntax, password will be accepted.
                                                            * - 2 = Syntax checking without exceptions, return an error refusing the password.
                                                            */
                'pwdMinLength' => '5',              // Minimum number of characters that will be accepted in a password.
                'pwdExpireWarning' => '0',          // Seconds before a password is due to expire, warning messages will be returned.
                'pwdFailureCountInterval' => '30',  // Seconds after which old consecutive failed bind attempts are purged from the failure counter.
                'pwdInHistory' => '0',              // Maximum number of used passwords that will be stored in 'pwdHistory'.
                'pwdLockout' => 'TRUE',             /* The action when a user has reached max failed Login attempts. 
                                                            * TRUE = not allowed to Login, 
                                                            * False = allowed to Login regardless of the failures
                                                            */
                'pwdLockoutDuration' => '30',       // Seconds during which the password cannot be used after reach max Login attempts.
                'pwdMaxFailure' => '5',             // Consecutive failed bind attempts.
                'pwdMaxAge' => '0',                 // Seconds after which a modified password will expire.
                'pwdMinAge' => '0',                 // Seconds that must elapse between modifications allowed to the password.
                'pwdMustChange' => 'FALSE',         // Users must change their passwords when they bind for the first time.
                'pwdSafeModify' => 'FALSE',         // User's existing password must be sent along with their new password when changing a password.
            );
        }
    }

    /**
     * Establish LDAP connection
     *
     * @param [string] $dn DN to bind to LDAP
     * @param [string] $pwd Password to bind to LDAP
     * @param [string] $conn Connectionstring
     * @param [boolean] $tls Optional
     * @return ldapMultiplexer
     */
    function auth_ldap($dn, $pwd, $conn, $tls)
    {
        $ldap_l = new LDAP(
            $dn,
            $pwd,
            $conn,
            FALSE,
            $tls
        );

        $ldap = new ldapMultiplexer($ldap_l);
        return $ldap;
    }
}
