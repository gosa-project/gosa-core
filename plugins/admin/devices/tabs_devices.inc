<?php

class devicetabs extends tabs
{
  var $Release= "";

  function devicetabs($config, $data, $dn,$category)
  {
    tabs::tabs($config, $data, $dn,$category);

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['deviceGeneric'];
    $new_dn= "cn=".$baseobject->cn.",".get_ou('deviceou').$baseobject->base;

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
