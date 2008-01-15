<?php

class apptabs extends tabs
{
  var $Release= "";

  function apptabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
  }

  function set_release($newRelease)
  {
    $this->Release= preg_replace('/,ou=apps,.*$/', '', $newRelease);
    if ($this->Release != ""){
      $this->Release= ",".$this->Release;
    }
  }

  function save()
  {
    $baseobject= $this->by_object['application'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if((!empty($tmp)) && (isset($_SESSION['appfilter']['release'])) && ($baseobject->isReleaseApplikation) && $_SESSION['appfilter']['release'] != "main"){
      $new_dn ="cn=".$baseobject->cn.",".$_SESSION['appfilter']['release'];
    }else{
      $new_dn= "cn=".$baseobject->cn.",ou=apps,".$baseobject->base;
    }

    /* Move group? */
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['application']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    tabs::save();
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
