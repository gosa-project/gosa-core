<?php

class devicetabs extends tabs
{
    var $Release= "";

    function __construct($config, $data, $dn,$category,$hide_refs = FALSE, $hide_acls = FALSE)
    {
        tabs::__construct($config, $data, $dn,$category,$hide_refs, $hide_acls);

        /* Add references/acls/snapshots */
        $this->addSpecialTabs();
    }

    function save($ignore_account= FALSE)
    {
        $baseobject= $this->by_object['deviceGeneric'];
        $nAV      = preg_replace('/,/', '\,', $baseobject->cn);
        $nAV      = preg_replace('/"/', '\"', $nAV);
        $new_dn= "cn=".$nAV.",".get_ou("deviceGeneric", "deviceRDN").$baseobject->base;

        /* Move group? */
        if ($this->dn != $new_dn){

            /* Write entry on new 'dn' */
            if ($this->dn != "new"){
                $baseobject->move($this->dn, $new_dn);
                $this->by_object['deviceGeneric']= $baseobject;
            }

            /* Happen to use the new one */
            $this->dn= $new_dn;
        }

        tabs::save();
    }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
