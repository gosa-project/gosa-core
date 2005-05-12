<?php

class worktabs extends tabs
{

  function worktabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    /* Update reference, transfer variables */
    $this->by_object['termstartup']->gotoTerminalPath=
      $this->by_object['workgeneric']->gotoTerminalPath;
  }


  function save()
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['workgeneric'];
    $this->dn= "cn=".$baseobject->cn.",ou=workstations,ou=systems,".$baseobject->base;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
      $this->by_object[$key]->cn= $baseobject->cn;
    }

    tabs::save(TRUE);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
