<?php

class scalixGroup extends scalixAccount
{
    var $scalixGroup = TRUE;

    var $attributes           = array("scalixMailnode");
    var $fakeBooleanAttributes= array("scalixHideUserEntry");
    var $objectclasses        = array("scalixGroupClass");
}

?>
