<?php

class faxblocktabs extends tabs
{
  function faxblocktabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn, 'gofaxlist');

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['blocklistGeneric'];

    /* Check for new 'dn', in order to propagate the 'dn' to all plugins */
    $new_dn= "cn=".$baseobject->cn.",ou=gofax,ou=systems,".$baseobject->base;

    /* Move group? */
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['blocklistGeneric']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    tabs::save();
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
