<?php

class wintabs extends tabs
{

  function wintabs($config, $data, $dn)
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

    /* add $ to end of name if mising */
    $baseobject= $this->by_object['wingeneric'];
    if(!preg_match("/\\\$\$/",$baseobject->cn)){
      $baseobject->cn .= "$";
    }

    $this->dn= "uid=$baseobject->uid,".get_winstations_ou().$baseobject->base;
    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
    }

    tabs::save(TRUE);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
