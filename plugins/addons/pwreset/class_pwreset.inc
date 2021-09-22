<?php

/*
  This code is an addon for GOsa (https://gosa.gonicus.de)
  Copyright (C) 2015 Mike Gabriel

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

class pwreset extends plugin
{
    /* Definitions */
    var $plHeadline= "Password Management";
    var $plDescription= "Password Management Add-On";
    var $plIcon = "plugins/pwreset/images/pwreset.png";

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();
    var $pwreset;

    function __construct(&$config)
    {
        $this->initTime = microtime(TRUE);

        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

        /* Include config object */
        $this->pwreset= new pwresettab($config, $config->data['TABS']['PWRESETTAB'], "");
    }

    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Show main page */
        return ($this->pwreset->execute());
    }

    /* Return plugin informations for acl handling
#FIXME You can only read attributes within this report plugin */
    static function plInfo()
    {
        return (array(
                    "plShortName"   => _("Password Management Add-On"),
                    "plDescription" => _("Facilitate mass password resets"),
                    "plSelfModify"  => FALSE,
                    "plDepends"     => array(),
                    "plPriority"    => 1,
                    "plSection"     => array("addon"),
                    "plCategory"    => array("pwreset" => array("description" => _("Password Management Add-On"))),
                    "plProvidedAcls" => array()
                    ));
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
