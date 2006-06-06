<?php

class phonetabs extends tabs
{

  function phonetabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);
  }


  function save()
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['phonegeneric'];
    $this->dn= "cn=$baseobject->cn,ou=phones,ou=systems,".$baseobject->base;
    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
    }

    tabs::save(TRUE);

    /* Fix tagging if needed */
    $baseobject->handle_object_tagging();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
