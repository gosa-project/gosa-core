<?php

class wintabs extends tabs
{

  function wintabs($config, $data, $dn,$category)
  {
    tabs::tabs($config, $data, $dn,$category);

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);
  }


  function save($ignore_account= FALSE)
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */

    /* add $ to end of name if mising */
    $baseobject= $this->by_object['wingeneric'];
    if(!preg_match("/\\\$\$/",$baseobject->cn)){
      $baseobject->cn .= "$";
    }

    $this->dn= "uid=$baseobject->cn,".get_winstations_ou().$baseobject->base;
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
