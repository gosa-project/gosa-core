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
    $new_dn= "cn=".$baseobject->cn.",ou=devices,".$baseobject->base;

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

    /* Fix tagging if needed */
    $baseobject->dn= $this->dn;
    $baseobject->handle_object_tagging();
    $this->by_object['deviceGeneric'] = $baseobject;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
