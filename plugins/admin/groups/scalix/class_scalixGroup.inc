<?php

class scalixGroup extends scalixAccount
{
    var $scalixGroup = TRUE;

    var $attributes           = array("scalixMailnode");
    var $fakeBooleanAttributes= array("scalixHideUserEntry");
    var $objectclasses        = array("scalixGroupClass");


    static function plInfo()
    {
        return (array(
                    "plDescription"     => _("SCALIX account"),
                    "plSelfModify"      => TRUE,
                    "plDepends"         => array("group"),
                    "plPriority"        => 2,
                    "plSection"         => array("administration"),
                    "plCategory"        => array("groups"),
                    "plOptions"         => array(),

                    "plProvidedAcls"  => array(

                        "scalixMailnode"            => _("SCALIX Mail node"),
                        "scalixMailboxClass"        => _("Mailbox class"),
                        "scalixServerLanguage"      => _("Server language"),
                        "scalixAdministrator"       => _("Administrator"),
                        "scalixMailboxAdministrator"=> _("Mailbox administrator"),
                        "scalixHideUserEntry"       => _("Hide user entry in SCALIX"),
                        "scalixLimitMailboxSize"    => _("Mailbox size limitations"),
                        "scalixLimitOutboundMail"   => _("Limit outbound"),
                        "scalixLimitInboundMail"    => _("Limit inbound"),
                        "scalixLimitNotifyUser"     => _("Notify user"),
                        "scalixEmailAddress"        => _("SCALIX email addresses"))

                        ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler
?>
