<?php


class all extends plugin {
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("All"),
                    "plDescription" => _("All objects in this category"),
                    "plSelfModify"  => TRUE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("all" => array("objectClass" => "none", "description" => '*&nbsp;'._("All"))),
                    "plProvidedAcls"    => array())
               );
    }
}

class core extends plugin {
    static function plInfo()
    {
        return (array(
                    "plShortName" => _("Core"),
                    "plDescription" => _("GOsa core plugin"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 0,
                    "plSection"     => array("administration"),
                    "plCategory"    => array("all"),
                    "plProperties"  => array(

                        array(
                            "name"          => "logging",
                            "type"          => "string",
                            "default"       => "",
                            "description"   => "",
                            "check"         => "",
                            "migrate"       => "",
                            "group"         => "core",
                            "mandatory"     => FALSE),

                        array(
                            "name"          => "listSummary",
                            "type"          => "string",
                            "default"       => "",
                            "description"   => "",
                            "check"         => "",
                            "migrate"       => "",
                            "group"         => "core",
                            "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordMinLength",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordMinDiffer",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordHook",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "displayErrors",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "schemaCheck",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "copyPaste",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "forceGlobals",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "forceSSL",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapStats",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "warnSSL",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ppdGzip",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ppdPath",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapMaxQueryTime",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "primaryGroupFilter",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailQueueScriptPath",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "storeFilterSettings",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sendCompressedOutput",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "modificationDetectionAttribute",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "language",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "theme",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sessionLifetime",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "templateCompileDirectory",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "debugLevel",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaHashHook",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "passwordDefaultHash",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "accountPrimaryAttribute",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "userRDN",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "groupRDN",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gidNumberBase",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "uidNumberBase",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSupportURI",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSupportTimeout",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "loginAttribute",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "timezone",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "honourUnitTags",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "useSaslForKerberos",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "rfc2307bis",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "personalTitleInDN",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "nextIdHook",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaMachineAccountRDN",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "idGenerator",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "strictNamingRules",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "minId",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailAttribute",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "gosaSharedPrefix",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailUserCreation",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailFolderCreation",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "imapTimeout",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "mailMethod",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "cyrusUseSlashes",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "vacationTemplateDirectory",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "ldapTLS",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "honourIvbbAttributes",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaIdMapping",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "handleExpiredAccounts",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaSID",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "sambaRidBase",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "enableSnapshots",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotBase",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotAdminDn",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotAdminPassword",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE),

                        array(
                                "name"          => "snapshotURI",
                                "type"          => "string",
                                "default"       => "",
                                "description"   => "",
                                "check"         => "",
                                "migrate"       => "",
                                "group"         => "core",
                                "mandatory"     => FALSE)
                            ),

                        "plProvidedAcls"    => array(
                                "accessTo"          => _("System trust"),
                                "cn"                => _("Name"),
                                "description"       => _("Description"),
                                "sudoUser"          => _("Users"),
                                "sudoHost"          => _("Host"),
                                "sudoCommand"       => _("Command"),
                                "sudoRunAs"         => _("Run as user"),
                                "trustModel"        => _("Access control list"))
                            )
                            );
    }
}
?>
