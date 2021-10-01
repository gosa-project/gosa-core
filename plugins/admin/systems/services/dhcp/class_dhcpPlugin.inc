<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2007 Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class dhcpPlugin extends plugin
{
    /* Used attributes */
    var $cn= "";
    var $orig_cn= "";
    var $options= null;
    var $statements= array();

    /* Subobjects */
    var $network;
    var $advanced;

    /* attribute list for save action */
    var $attributes= array();
    var $objectclasses= array();

    function __construct(&$parent,$attrs)
    {
        $this->initTime = microtime(TRUE);
        $this->parent = $parent;

        $this->options = new dhcpOption();
        $this->statements = new dhcpOption();

        /* Load statements / options */
        if (is_array($attrs)){
            $this->dn= $attrs['dn'];
            $this->cn= $attrs['cn'][0];
            $this->new= FALSE;

            /* Load options */
            if (isset($attrs['dhcpOption'])){
                foreach($attrs['dhcpOption'] as $opt){
                    $this->options->add(trim($opt));
                }
            }

            /* Load statements */
            if (isset($attrs['dhcpStatements'])){
                foreach($attrs['dhcpStatements'] as $opt){
                    $this->statements->add(trim($opt));
                }
            }
        } else {
            /* We keep the parent dn here if it's new */
            $this->dn= $attrs;
            $this->new= TRUE;
        }

        /* Load network module */
        $this->network= new dhcpNetwork();
        $this->network->options= &$this->options;
        $this->network->statements= &$this->statements;
        $this->advanced= new dhcpAdvanced();
        $this->advanced->options= &$this->options;
        $this->advanced->statements= &$this->statements;

        $this->network->parent = $parent;
        $this->advanced->parent = $parent;

        /* Save CN for later reference */
        $this->orig_cn= $this->cn;

        // Create statistic table entry
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

    }

    function execute()
    {
        plugin::execute();
        return ("");
    }


    function remove_from_parent()
    {
    }


    /* Save data to object */
    function save_object()
    {
# // CHECK
#   foreach (array("filename", "next-server","get-lease-hostnames","use-host-decl-names") as $toberemoved){
#     unset($this->statements[$toberemoved]);
#   }

        /* Save sub-objects */
        $this->network->save_object();
        $this->advanced->save_object();
    }


    /* Check values */
    function check()
    {
        $message= array();
        return $message;
    }


    /* Save to LDAP */
    function save()
    {
        /* Add cn if we're new */
        if ($this->new){
            $this->dn= "cn=".$this->cn.",".$this->dn;
        } else {
            $this->dn= "cn=".$this->cn.preg_replace('/^cn=[^,]+/', '', $this->dn);
        }

        /* Assemble new entry - options */
        $this->attrs['dhcpOption']= $this->options->save();

        /* Assemble new entry - statements */
        $this->attrs['dhcpStatements']= $this->statements->save();

        /* Move dn to the result */
        $this->attrs['dn']= $this->dn;
        $this->attrs['cn']= array($this->cn);
        $this->attrs['objectClass']= $this->objectclasses;
        $this->attrs['MODIFIED']= TRUE;
        return ($this->attrs);
    }


    function removeAttrs($name, $type)
    {
        $new= array();
        foreach ($this->attrs[$type] as $value){
            if (!preg_match("/^$name /", $value)){
                $new[]= $value;
            }
        }
        $this->attrs[$type]= $new;
    }


    function removeOption($name)
    {
        $this->removeAttrs($name, 'dhcpOption');
    }


    function removeStatement($name)
    {
        $this->removeAttrs($name, 'dhcpStatement');
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
