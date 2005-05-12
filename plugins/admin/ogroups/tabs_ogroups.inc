<?php

class ogrouptabs extends tabs
{
  var $base= "";

  function ogrouptabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
    $this->base= $this->by_object['ogroup']->base;

    /* Insert extra tabs for several object types - if present */
    $objects= preg_replace('/[\[\]]/', '', $this->by_object['ogroup']->gosaGroupObjects);
    for ($n= 0; $n<strlen($objects); $n++){
      switch ($objects[$n]){
        case "T":
          /* Add a terminal tab */
          require_once("class_termgroup.inc");
          $this->by_name['termgroup']= _("Terminals");
          $this->by_object['termgroup']= new termgroup($this->config, $this->dn);
          $this->by_object['termgroup']->parent= &$this;

          break;
      }
    }
  }

  function check()
  {
    return (tabs::check(TRUE));
  }


  function save()
  {
    $baseobject= $this->by_object['ogroup'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $new_dn= 'cn='.$baseobject->cn.','.get_groups_ou().$baseobject->base;

    /* Move group? */
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['ogroup']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    if ($this->dn == "new"){
      $this->dn= 'cn='.$baseobject->cn.','.get_groups_ou().$baseobject->base;
    }
    tabs::save(TRUE);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
