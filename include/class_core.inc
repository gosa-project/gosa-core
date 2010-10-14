<?php

/*
 *   How to use plugin::plInfo()
 *   ===================
 *   
 *   The function returns a descriptive array of the plugin,
 *    which will then be used by GOsa to populate the plugin, its ACLs, 
 *    its properties, its schema requirements aso.
 *
 *
 *   The following values can be set:
 *   ================================
 *
 *   plShortName    |-> The name of the plugin in short (e.g. Posix)
 *                  |   This short-name will be shown for example in the ACL definitions.
 *                  | 
 *                  | 
 *   plDescription  |-> A descriptive text for the plugin (e.g. User posix account extension)
 *                  |   This will be shown in the ACL definitions.   
 *                  | 
 *                  | 
 *   plSelfModify   |-> If set to true this plugin allows to set 'self' ACLs.
 *                  |   For exampe to allow to change the users own password, but not the others.
 *                  | 
 *                  | 
 *   plDepends      |-> The plugins dependencies to other classes (e.g. sambaAccount requires posixAccount)
 *                  |  
 *                  | 
 *   plPriority     |-> The priority of the plugin, this influences the ACL listings only.
 *                  | 
 *                  | 
 *   plSection      |-> The section of this plugin 'administration', 'personal', 'addons'
 *                  | 
 *                  | 
 *   plCategory     |-> The plugin category this plugins belongs to (e.g. users, groups, departments) 
 *                  | 
 *                  | 
 *   plRequirements |-> Plugin requirements. 
 *                  | |
 *                  | |-> [activePlugin]           The schame checks will only be performed if the given plugin is enabled
 *                  | |                             in the gosa.conf definitions.
 *                  | |                            Defaults to the current class name if empty.
 *                  | |
 *                  | |-> [ldapSchema]             An array of objectClass requirements.
 *                  | |                            Syntax [[objectClass => 'version'], ... ]
 *                  | |                            Version can be emtpy which just checks for the existence of the class.
 *                  | |
 *                  | |-> [onFailureDisablePlugin] A list of plugins that which will be disabled if the 
 *                  |                               requirements couldn't be fillfulled.
 *                  |
 *                  |      ---------------------------------------------
 *                  |      EXAMPLE:
 *                  |      ---------------------------------------------
 *                  |      "plRequirements"=> array(
 *                  |         'activePlugin' => 'applicationManagement', 
 *                  |         'ldapSchema' => array(
 *                  |             'gosaObject' => '',
 *                  |             'gosaAccount' => '>=2.7',
 *                  |             'gosaLockEntry' => '>=2.7',
 *                  |             'gosaDepartment' => '>=2.7',
 *                  |             'gosaCacheEntry' => '>=2.7',
 *                  |             'gosaProperties' => '>=2.7',
 *                  |             'gosaConfig' => '>=2.7'
 *                  |             ),
 *                  |         'onFailureDisablePlugin' => array(get_class(), 'someClassName')
 *                  |         ),
 *                  |      ---------------------------------------------
 *                  |
 *                  |
 *                  |         
 *   plProvidedAcls |-> The ACLs provided by this plugin
 *                  |
 *                  |      ---------------------------------------------
 *                  |      EXAMPLE:
 *                  |      ---------------------------------------------
 *                  |      "plProvidedAcls"=> array(
 *                  |          'cn'             => _('Name'),
 *                  |          'uid'            => _('Uid'),
 *                  |          'phoneNumber'    => _('Phone number')
 *                  |          ),
 *                  |      ---------------------------------------------
 *                  |
 *                  |
 *                  | 
 *   plProperties   |-> Properties used by the plugin.
 *                  |   Properties which are defined here will be modifyable using the property editor.
 *                  |   To read properties you can use $config->get_cfg_value(className, propertyName)
 *                  | 
 *                  |      ---------------------------------------------
 *                  |      EXAMPLE:
 *                  |      ---------------------------------------------
 *                  |      "plProperties"=> array(
 *                  |         array(
 *                  |             "name"          => "htaccessAuthentication",
 *                  |             "type"          => "bool",
 *                  |             "default"       => "false",
 *                  |             "description"   => _("A description..."),
 *                  |             "check"         => "gosaProperty::isBool",
 *                  |             "migrate"       => "",
 *                  |             "group"         => "authentification",
 *                  |             "mandatory"     => TRUE
 *                  |             ),
 *                  |         ),
 *                  |   See class_core.inc for a huge amount of examples.
 */


class all extends plugin {
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("All"),
                    "plDescription" => _("All objects"),
                    "plSelfModify"  => TRUE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("all" => array("description" => '*&nbsp;'._("All"))),
                    "plProvidedAcls"    => array())
               );
    }
}

class core extends plugin {

    static function getPropertyValues($class,$name,$value,$type)
    {
        $list = array();
        switch($name){
            case 'idAllocationMethod':
                $list = array('traditional' => _('Traditional'), 'pool' => _('Use samba pool'));
                break;
            case 'passwordDefaultHash':
                $tmp = passwordMethod::get_available_methods();
                foreach($tmp['name'] as $id => $method){
                    $desc = $tmp[$id]['name'];
                    $list[$method] = $desc;
                }
                break;
            case 'theme':
                $cmd = "cd ../ihtml/themes; find . -name 'img.styles' | sed s/'^[^\/]*\/\([^\/]*\).*'/'\\1'/g";
                $res = `$cmd`  ; 
                $list = array();
                foreach(preg_split("/\n/",$res) as $entry){
                    if(!empty($entry)){
                        $list[$entry] = $entry;
                    }
                }
                break;
            case 'accountPrimaryAttribute':
                $list = array('uid' => 'uid', 'cn' => 'cn');
                break;
            case 'loginAttribute':
                $list = array(
                            'uid' => 'uid',
                            'mail' => 'mail',
                            'both' => 'uid & mail');
                break;
            case 'timezone': 
                $tmp = timezone::_get_tz_zones();
                foreach($tmp['TIMEZONES'] as $name => $offset){
                    if($offset >= 0){
                        $list[$name] = $name." ( + ".sprintf("%0.2f",$offset/(60*60))." "._("hours").")";
                    }else{
                        $offset = $offset * -1;
                        $list[$name] = $name." ( - ".sprintf("%0.2f",($offset/(60*60)))." "._("hours").")";
                    }
                }
                break;
            case 'mailAttribute':
                $list = array('mail' => 'mail','uid' => 'uid');
                break;
            case 'mailMethod': 
                $tmp = mailMethod::get_methods();
                $list =array();
                foreach($tmp as $name => $value){
                    $name = preg_replace('/^mailMethod/','', $name);
                    $list[$name] = $value;
                }
                $list[''] = _("None");
                break;
            case 'language':
                $tmp = get_languages(TRUE);
                $list[""] = _("Automatic");
                foreach($tmp as $key => $desc){
                    $list[$key] = $desc;
                }
                break;
            case 'modificationDetectionAttribute': 
                $list = array('entryCSN' => 'entryCSN (OpenLdap)','textCSN'=>'textCSN (Sun DS)');
                break;
            default: echo $name." ";$list = array();
        }

        if(!isset($list[$value])){
            $list[$value] = $value." ("._("User value").")";
        }

        return($list);
    }

    static function plInfo()
    {
        return (array(
                    "plShortName" => _("Core"),
                    "plDescription" => _("GOsa core plugin"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),

                    "plRequirements"=> array(
                        'ldapSchema' => array(
                            'gosaObject' => '>=2.7',
                            'gosaAccount' => '>=2.7',
                            'gosaLockEntry' => '>=2.7',
                            'gosaDepartment' => '>=2.7',
                            'gosaCacheEntry' => '>=2.7',
                            'gosaProperties' => '>=2.7',
                            'gosaConfig' => '>=2.7'
                            ),
                        'onFailureDisablePlugin' => array(get_class())
                        ),



                    "plCategory"    => array("all"),
                    "plProperties"  => array(

                        array(
                            "name"          => "htaccessAuthentication",
                            "type"          => "bool",
                            "default"       => "false",
                            "description"   => _("The 'htaccessAuthentication' variable tells GOsa to use either htaccess authentication or LDAP authentication. This can be used if you want to use i.e. Kerberos to authenticate the users."),
                            "check"         => "gosaProperty::isBool",
                            "migrate"       => "",
                            "group"         => "authentification",
                            "mandatory"     => TRUE),

                        array(
                            "name"          => "statsDatabaseEnabled",
                            "type"          => "bool",
                            "default"       => "false",
                            "description"   => _("Enables/Disables GOSa usage statistics moduls."),
                            "check"         => "gosaProperty::isBool",
                            "migrate"       => "",
                            "group"         => "core",
                            "mandatory"     => TRUE),

                        array(
                                "name"          => "statsDatabaseDirectory",
                                "type"          => "path",
                                "default"       => "/var/spool/gosa/stats",
                                "description"   => _("The database file for GOSa usage statistics."),
                                "check"         => "gosaProperty::isWriteablePath",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => TRUE),

                        array(
                            "name"          => "logging",
                            "type"          => "bool",
                            "default"       => "true",
                            "description"   => _("The 'logging' statement enables event logging on GOsa side. Setting it to 'true' GOsa will log every action a user performs via syslog. If you use rsyslog and configure it to mysql logging, you can browse all events within GOsa. GOsa will not log anything, if the logging value is empty or set to 'false'."),

                            "check"         => "gosaProperty::isBool",
                            "migrate"       => "",
                            "group"         => "core",
                            "mandatory"     => TRUE),

                        array(
                                "name"          => "listSummary",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'listSummary' statement determines whether a status bar will be shown on the bottom of GOsa generated lists, displaying a short summary of type and number of elements in the list."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "visual",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordMinLength",
                                "type"          => "integer",
                                "default"       => "",
                                "description"   => _("The 'passwordMinLength' statement determines whether a newly entered password has to be of a minimum length."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordMinDiffer",
                                "type"          => "integer",
                                "default"       => "",
                                "description"   => _("The 'passwordMinDiffer' statement determines whether a newly entered password has to be checked to have at least n different characters."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordProposalHook",
                                "type"          => "command",
                                "default"       => "",
                                "description"   => _("The 'passwordProposalHook' can be used to let GOsa generate password proposals for you. Whenever you change a password, you can then decide whether to use the proposal or to manually specify a password.")." "._("Here is an example command:")." /usr/bin/apg -n1",
                                "check"         => "gosaProperty::isCommand",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "displayErrors",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'displayErrors' statement tells GOsa to show PHP errors in the upper part of the screen. This should be disabled in productive deployments, because there might be some important passwords arround.")." ".sprintf(_("The property '%s' may be interesting too."),'developmentMode'),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "developmentMode",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'developmentMode' statement tells GOsa to show development related error messages, for example messages about missing ACL configuration or classes. Due to the fact that enabling this flag will result in a lot of false error messages it should only be enabled while developing or debugging plugins!"),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => TRUE),


                        array(
                                "name"          => "schemaCheck",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'schemaCheck' statement enables or disables schema checking during login. It is recommended to switch this on in order to let GOsa handle object creation more efficient."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "copyPaste",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'copyPaste' statement enables copy and paste for LDAP entries managed with GOsa."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "copyPaste",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "forceGlobals",
                                "type"          => "noLdap",
                                "default"       => "false",
                                "description"   => _("The 'forceGlobals' statement enables PHP security checks to force register_global settings to be switched off."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "forceSSL",
                                "type"          => "noLdap",
                                "default"       => "false",
                                "description"   => _("The 'forceSSL' statement enables PHP security checks to force encrypted access to the web interface. GOsa will try to redirect to the same URL - just with https://"),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "ldapStats",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("Logs information about triggered ldap operations, duration, filter, aso. into syslog."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapFollowReferrals",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'ldapFollowReferrals' statement tells GOsa to follow LDAP referrals."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "ldap",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "ldapFilterNestingLimit",
                                "type"          => "integer",
                                "default"       => 200,
                                "description"   => _("The 'ldapFilterNestingLimit' statement can be used to speed up group handling for groups with several hundreds of members. The default behaviour is, that GOsa will resolv the memberUid values in a group to real names. To achieve this, it writes a single filter to minimize searches. Some LDAP servers (namely Sun DS) simply crash when the filter gets too big. You can set a member limit, where GOsa will stop to do these lookups."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "ldap",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "ldapSizelimit",
                                "type"          => "integer",
                                "default"       => 200,
                                "description"   => _("The ldapSizelimit statement tells GOsa to retrieve the specified maximum number of results. The user will get  a  warning,  that  not  all entries were shown."), 
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "ldapSizeIgnore",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("Disables sizelimit checks, only the configured amount of results will be shown!"),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "warnSSL",
                                "type"          => "noLdap",
                                "default"       => "true",
                                "description"   => _("The 'warnSSL' statement enables PHP security checks to detect non encrypted access to the web interface. GOsa will display a warning in this case."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "ppdGzip",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'ppdGzip' variable enables PPD file compression."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "ppd",
                                "mandatory"     => FALSE),


                        array(
                                "name"          => "ignoreAcl",
                                "type"          => "dn",
                                "default"       => "",
                                "description"   => _("The 'ignoreAcl' value tells GOsa to ignore complete ACL sets for the given DN. Add your DN here and you'll be able to restore accidently dropped ACLs."),
                                "check"         => "gosaProperty::isDN",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => FALSE),


                        array(
                                "name"          => "ppdPath",
                                "type"          => "path",
                                "default"       => "/var/spool/ppd",
                                "description"   => _("The 'ppdPath' variable defines where to store PPD files for the GOto environment plugins."),
                                "check"         => "gosaProperty::isPath",
                                "migrate"       => "",
                                "group"         => "ppd",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapMaxQueryTime",
                                "type"          => "integer",
                                "default"       => "",
                                "description"   => _("The 'ldapMaxQueryTime' statement tells GOsa to stop LDAP actions if there is no answer within the specified number of seconds."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "storeFilterSettings",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'storeFilterSettings' statement determines whether GOsa should store filter and plugin settings inside of a cookie."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sendCompressedOutput",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'sendCompressedOutput' statement determines whether PHP should send compressed HTML pages to browsers or not. This may increase or decrease the performance, depending on your network."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "modificationDetectionAttribute",
                                "type"          => "switch",
                                "default"       => "entryCSN",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'modificationDetectionAttribute' statement enables GOsa to check if a entry currently being edited has been modified from someone else outside GOsa in the meantime. It will display an informative dialog then. It can be set to 'entryCSN' for OpenLDAP based systems or 'contextCSN' for Sun DS based systems."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "language",
                                "type"          => "switch",
                                "default"       => "",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'language' statement defines the default language used by GOsa. Normally GOsa autodetects the language from the browser settings. If this is not working or you want to force the language, just add the language code (i.e. de for german) here."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "theme",
                                "type"          => "switch",
                                "default"       => "default",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'theme' statement defines what theme is used to display GOsa pages. You can install some corporate identity like theme and/or modify certain templates to fit your needs within themes. Take a look at the GOsa FAQ for more information."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "visual",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "sessionLifetime",
                                "type"          => "integer",
                                "default"       => 600,
                                "description"   => _("The 'sessionLifetime' value defines when a session will expire in seconds. For Debian systems, this will not work because the sessions will be removed by a cron job instead. Please modify the value inside of your php.ini instead."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "templateCompileDirectory",
                                "type"          => "path",
                                "default"       => "/var/spool/gosa",
                                "description"   => _("The 'templateCompileDirectory' statements defines the path, where the PHP templating engins 'smarty' should store its compiled GOsa templates for improved speed. This path needs to be writeable by the user your webserver is running with."),
                                "check"         => "gosaProperty::isWriteablePath",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "debugLevel",
                                "type"          => "integer",
                                "default"       => 0,
                                "description"   => sprintf(_("The 'debugLevel' value tells GOsa to display certain information on each page load. Value is an AND combination of the following byte values: %s"),
"
DEBUG_TRACE   = 1
DEBUG_LDAP    = 2
DEBUG_MYSQL   = 4
DEBUG_SHELL   = 8
DEBUG_POST    = 16
DEBUG_SESSION = 32
DEBUG_CONFIG  = 64
DEBUG_ACL     = 128
DEBUG_SI      = 256"),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "debug",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaHashHook",
                                "type"          => "command",
                                "default"       => "perl -MCrypt::SmbHash -e \"print join(q[:], ntlmgen \\\$ARGV[0]), $/;\"",
                                "description"   => sprintf(_("The 'sambaHashHook' statement contains an executable to generate samba hash values. This is required for password synchronization, but not required if you apply gosa-si services. If you don't have mkntpasswd from the samba distribution installed, you can use perl to generate the hash: %s"),"perl -MCrypt::SmbHash -e \"print join(q[:], ntlmgen \\\$ARGV[0]), $/;\""),
                                "check"         => "gosaProperty::isCommand",
                                "migrate"       => "",
                                "group"         => "samba",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordDefaultHash",
                                "type"          => "switch",
                                "default"       => "crypt/md5",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'passwordDefaultHash' statement defines the default password hash to choose for new accounts."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),
                        array(
                                "name"          => "strictPasswordRules",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'strictPasswordRules' tells GOsa to check for UTF-8 characters in the supplied password. These Characters can lead to non working authentications if UTF-8 and none UTF-8 systems locales get mixed. The default is 'true'."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "accountPrimaryAttribute",
                                "type"          => "switch",
                                "default"       => "cn",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'accountPrimaryAttribute' option tells GOsa how to create new accounts. Possible values are 'uid' and 'cn'. In the first case GOsa creates uid style DN entries: 'uid=superuser,ou=staff,dc=example,dc=net'. In the second case, GOsa creates cn style DN entries: 'cn=Foo Bar,ou=staff,dc=example,dc=net'. If you choose \"cn\" to be your 'accountPrimaryAttribute' you can decide whether to include the personal title in your dn by selecting 'personalTitleInDN'."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "userRDN",
                                "type"          => "rdn",
                                "default"       => "ou=people,",
                                "description"   => _("The 'userRDN' statement defines the location where new accounts will be created inside of defined departments. The default is 'ou=people'."),
                                "check"         => "gosaProperty::isRdn",
                                "migrate"       => "migrate_userRDN", 
                                "group"         => "user",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "groupRDN",
                                "type"          => "rdn",
                                "default"       => "ou=groups,",
                                "description"   => _("The 'groupsRDN' statement defines the location where new groups will be created inside of defined departments. The default is 'ou=groups'."),
                                "check"         => "gosaProperty::isRdn",
                                "migrate"       => "migrate_groupRDN",
                                "group"         => "group",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gidNumberBase",
                                "type"          => "integer",
                                "default"       => "1000",
                                "description"   => _("The 'gidNumberBase' statement defines where to start looking for a new free group id. This should be synced with your 'adduser.conf' to avoid overlapping gidNumber values between local and LDAP based lookups. The gidNumberBase can even be dynamic. Take a look at the 'nextIdHook' definition."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => TRUE),

                       array(
                                "name"          => "baseIdHook",
                                "type"          => "command",
                                "default"       => "",
                                "description"   => _("The 'baseIdHook' statement allows to hook into the id generation method (traditional mode), to define the starting range for new generated ids"), 
                                "check"         => "gosaProperty::isCommand",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gidNumberPoolMin",
                                "type"          => "integer",
                                "default"       => 10000,
                                "description"   => _("The 'uidNumberPoolMin/gidNumberPoolMin' statement defines the lowest assignable user/group id for use with the 'idAllocationMethod = pool'."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gidNumberPoolMax",
                                "type"          => "integer",
                                "default"       => 40000,
                                "description"   => _("The 'uidNumberPoolMax/gidNumberPoolMax' statement defines the highest assignable user/group id for use with the 'idAllocationMethod = pool'."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "uidNumberPoolMin",
                                "type"          => "integer",
                                "default"       => 10000,
                                "description"   => _("The 'uidNumberPoolMin/gidNumberPoolMin' statement defines the lowest assignable user/group id for use with the 'idAllocationMethod = pool'."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "uidNumberPoolMax",
                                "type"          => "integer",
                                "default"       => 40000,
                                "description"   => _("The 'uidNumberPoolMax/gidNumberPoolMax' statement defines the highest assignable user/group id for use with the 'idAllocationMethod = pool'."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "uidNumberBase",
                                "type"          => "integer",
                                "default"       => "1000",
                                "description"   => _("The 'uidNumberBase' statement defines where to start looking for a new free user id. This should be synced with your 'adduser.conf' to avoid overlapping uidNumber values between local and LDAP based lookups. The uidNumberBase can even be dynamic. Take a look at the 'baseIdHook' definition."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaRpcServer",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "The server to use for RPC connections! (http://localhost:8080/rpc), Future GOsa-ng service! If this value is set and not empty GOsa will try to establish a connection!",
                                "check"         => "jsonRPC::testConnectionProperties",
                                "migrate"       => "",
                                "group"         => "rpc",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaRpcUser",
                                "type"          => "string",
                                "default"       => "admin",
                                "description"   => "Ther username to use for RPC connections! Future GOsa-ng service!",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "rpc",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaRpcPassword",
                                "type"          => "string",
                                "default"       => "tester",
                                "description"   => "Ther password to use for RPC connections! Future GOsa-ng service!",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "rpc",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSupportURI",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => _("The 'gosaSupportURI' defines the major gosa-si server host and the password for GOsa to connect to it. It can be used if you want to use i.e. kerberos to authenticate the users. The format is: credentials@host:port"),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "gosa-si",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSupportTimeout",
                                "type"          => "integer",
                                "default"       => 15,
                                "description"   => _("The 'gosaSupportTimeout' sets a connection timeout for all gosa-si actions. See 'gosaSupportURI' for details."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "gosa-si",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "loginAttribute",
                                "type"          => "switch",
                                "default"       => "uid",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'loginAttribute' statement tells GOsa which LDAP attribute is used as the login name during login. It can be set to 'uid', 'mail' or 'both'"),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "timezone",
                                "type"          => "switch",
                                "default"       => "",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'timezone' statements defines the timezone used inside of GOsa to handle date related tasks, such as password expiery, vacation messages, etc. The 'timezone' value should be a unix conform timezone value like in /etc/timezone."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "honourUnitTags",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'honourUnitTags' statement enables checking of 'unitTag' attributes when using administrative units. If this is set to 'true' GOsa can only see objects inside the administrative unit a user is logged into."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "useSaslForKerberos",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'useSaslForKerberos' statement defines the way the kerberos realm is stored in the #userPassword' attribute. Set it to 'true' in order to get {sasl}user@REALM.NET, or to 'false' to get {kerberos}user@REALM.NET. The latter is outdated, but may be needed from time to time."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "password",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "rfc2307bis",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'rfc2307bis' statement enables rfc2307bis style groups in GOsa. You can use 'member' attributes instead of memberUid in this case. To make it work on unix systems, you've to adjust your NSS configuration to use rfc2307bis style groups, too"),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "personalTitleInDN",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'personalTitleInDN' option tells GOsa to include the personal title in user DNs when #accountPrimaryAttribute' is set to \"cn\"."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "storage location",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "nextIdHook",
                                "type"          => "command",
                                "default"       => "",
                                "description"   => _("The 'nextIdHook' statement defines a script to be called for finding the next free id for users or groups externaly. It gets called with the current entry \"dn\" and the attribute to be ID'd. It should return an integer value."),
                                "check"         => "gosaProperty::isCommand",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "idGenerator",
                                "type"          => "string",
                                "default"       => "{%sn}-{%givenName[2-4]}",
                                "description"   => _("The 'idGenerator' statement describes an automatic way to generate new user ids. There are two basic functions supported - which can be combined: 

 a) using attributes

    You can specify LDAP attributes (currently only sn and givenName) in
    braces {} and add a percent sign befor it. Optionally you can strip it
    down to a number of characters, specified in []. I.e.

      idGenerator=\"{%sn}-{%givenName[2-4]}\"

    will generate an ID using the full surename, adding a dash, and adding at
    least the first two characters of givenName. If this ID is used, it'll
    use up to four characters. If no automatic generation is possible, a
    input box is shown.

 b) using automatic id's

    I.e. specifying

      idGenerator=\"acct{id:3}\"

    will generate a three digits id with the next free entry appended to \"acct\".

      idGenerator=\"acct{id!1}\"

    will generate a one digit id with the next free entry appended to \"acct\" - if needed.

      idGenerator=\"ext{id#3}\"

    will generate a three digits random number appended to \"ext\".
"),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "strictNamingRules",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'strictNamingRules' statement enables strict checking of uids and group names. If you need   characters like . or - inside of your accounts, set this to 'false'."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "minId",
                                "type"          => "integer",
                                "default"       => 40,
                                "description"   => _("The 'minId' statement defines the minimum assignable user or group id to avoid security leaks with uid 0 accounts. This is used for the 'traditional' method."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailAttribute",
                                "type"          => "switch",
                                "default"       => "mail",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'mailAttribute' statement determines which attribute GOsa will use to create accounts. Valid values are 'mail' and 'uid'."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSharedPrefix",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => _("This attribute allows to override the prefix used to create shared folders."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailUserCreation",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => _("This attribute allows to override the user account creation syntax, see the 'mailFolderCreation' description for more details.

Examples
 mailUserCreation=\"%prefix%%uid%\"           => \"user.foobar\"
 mailUserCreation=\"my-prefix.%uid%%domain%\"  => \"my-prefix.foobar@example.com\"
"),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailFolderCreation",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => _("Every mail method has its own way to create mail accounts like 'share/development' or 'shared.development@example.com' which is used to identify the accounts, set quotas or add acls.

To override the methods default account creation syntax, you can set the 'mailFolderCreation' option.

Examples

 mailFolderCreation=\"%prefix%%cn%\"              => \"shared.development\"
 mailFolderCreation=\"my-prefix.%cn%%domain%\"    => \"my-prefix.development@example.com\"

 %prefix%    The methods default prefix. (Depends on cyrusUseSlashes=FALSE/TRUE)
 %cn%        The groups/users cn.
 %uid%       The users uid.
 %mail%      The objects mail attribute.
 %domain%    The domain part of the objects mail attribute.
 %mailpart%  The user address part of the mail address.
 %uattrib%   Depends on mailAttribute=\"uid/mail\".
"),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "imapTimeout",
                                "type"          => "integer",
                                "default"       => 10,
                                "description"   => _("The 'imapTimeout' statement sets the connection timeout for imap actions."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailMethod",
                                "type"          => "switch",
                                "default"       => "",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'mailMethod' statement tells GOsa which mail method the setup should use to communicate with a possible mail server. Leave this undefined if your mail method does not match the predefined ones."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "cyrusUseSlashes",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'cyrusUseSlashes' statement determines if GOsa should use \"foo/bar\" or \"foo.bar\" namespaces in IMAP. Unix style is with slashes."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "vacationTemplateDirectory",
                                "type"          => "path",
                                "default"       => "/etc/gosa/vacation",
                                "description"   => _("The 'vacationTemplateDirectory' statement sets the path where GOsa will look for vacation message templates. Default is /etc/gosa/vacation. 

Example template /etc/gosa/vacation/business.txt:

   DESC:Away from desk
   Hi, I'm currently away from my desk. You can contact me on
   my cell phone via %mobile.

   Greetings,
   %givenName %sn

"),
                                "check"         => "gosaProperty::isWriteablePath",
                                "migrate"       => "",
                                "group"         => "mail",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapTLS",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'ldapTLS' statement enables or disables TLS operating on LDAP connections."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "security",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "honourIvbbAttributes",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'honourIvbbAttributes' statement enables the IVBB mode inside of GOsa. You need the ivbb.schema file from used by german authorities."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaIdMapping",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'sambaIdMapping' statement tells GOsa to maintain sambaIdmapEntry objects. Depending on your setup this can drastically improve the windows login performance."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "samba",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "handleExpiredAccounts",
                                "type"          => "bool",
                                "default"       => "true",
                                "description"   => _("The 'handleExpiredAccounts' statement enables shadow attribute tests during the login to the GOsa web interface and forces password renewal or account lockout."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaSID",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => _("The 'sambaSID' statement defines a samba SID if not available inside of the LDAP. You can retrieve the current sid by net getlocalsid."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "samba",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaRidBase",
                                "type"          => "integer",
                                "default"       => "",
                                "description"   => _("The 'sambaRidBase' statement defines the base id to add to ordinary sid calculations - if not available inside of the LDAP."),
                                "check"         => "gosaProperty::isInteger",
                                "migrate"       => "",
                                "group"         => "samba",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "enableSnapshots",
                                "type"          => "bool",
                                "default"       => "false",
                                "description"   => _("The 'enableSnapshots' statement enables a snapshot mechaism in GOsa. This enables you to save certain states of entries and restore them later on."),
                                "check"         => "gosaProperty::isBool",
                                "migrate"       => "",
                                "group"         => "snapshot",
                                "mandatory"     => TRUE),

                        array(
                                "name"          => "snapshotBase",
                                "type"          => "dn",
                                "default"       => "ou=snapshots,dc=localhost,dc=de",
                                "description"   => _("The 'snapshotBase' statement defines the base where snapshots should be stored inside of the LDAP."),
                                "check"         => "gosaProperty::isDn",
                                "migrate"       => "",
                                "group"         => "snapshot",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotAdminDn",
                                "type"          => "dn",
                                "default"       => "cn=admin,dc=localhost,dc=de",
                                "description"   => _("The 'snapshotAdminDn' variable defines the user which is used to authenticate when connecting to 'snapshotURI'."),
                                "check"         => "gosaProperty::isDn",
                                "migrate"       => "",
                                "group"         => "snapshot",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotAdminPassword",
                                "type"          => "string",
                                "default"       => "secret",
                                "description"   => _("The 'snapshotAdminPassword' variable defines the credentials which are used in combination with 'snapshotAdminDn' and 'snapshotURI' in order to authenticate."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "snapshot",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "idAllocationMethod",
                                "type"          => "switch",
                                "default"       => "traditional",
                                "defaults"      => "core::getPropertyValues",
                                "description"   => _("The 'idAllocationMethod' statement defines how GOsa generates numeric user and group id values. If it is set to 'traditional' GOsa will do create a lock and perform a search for the next free ID. The lock will be removed after the procedure completes. 'pool' will use the sambaUnixIdPool objectclass settings inside your LDAP. This one is unsafe, because it does not check for concurrent LDAP access and already used IDs in this range. On the other hand it is much faster."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "id",
                                "mandatory"     => TRUE),
                        array(
                                "name"          => "snapshotURI",
                                "type"          => "uri",
                                "default"       => "ldap://localhost:389",
                                "description"   => _("The 'snapshotURI' variable defines the LDAP URI for the server which is used to do object snapshots."),
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "snapshot",
                                "mandatory"     => FALSE)
                            )));
    }
}
?>
