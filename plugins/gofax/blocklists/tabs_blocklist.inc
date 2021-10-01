<?php

class faxblocktabs extends tabs
{
  function __construct($config, $data, $dn)
  {
    tabs::__construct($config, $data, $dn, 'gofaxlist');

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['blocklistGeneric'];

    /* Check for new 'dn', in order to propagate the 'dn' to all plugins */
    $cn      = preg_replace('/,/', '\,', $baseobject->cn);
    $cn      = preg_replace('/"/', '\"', $cn);
    $new_dn= "cn=".$cn.",".get_ou("blocklistGeneric", "faxBlocklistRDN").$baseobject->base;

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
