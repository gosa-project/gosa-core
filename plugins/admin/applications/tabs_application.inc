<?php

class apptabs extends tabs
{
  var $Release= "";

  function apptabs($config, $data, $dn,$category)
  {
    tabs::tabs($config, $data, $dn,$category);

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function set_release($newRelease)
  {
    $this->Release= preg_replace('/,'.get_ou('applicationou').'.*$/', '', $newRelease);
    if ($this->Release != ""){
      $this->Release= ",".$this->Release;
    }
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['application'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $tmp = $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    $appfilter = session::get('appfilter');
    if((!empty($tmp)) && (isset($appfilter['release']))){
      if(!$baseobject->isReleaseApplikation){
        $new_dn= "cn=".$baseobject->cn.",".get_ou('applicationou').$baseobject->base;
      }else{
        $new_dn ="cn=".$baseobject->cn.",".$appfilter['release'];
      }
    }else{
      $new_dn= "cn=".$baseobject->cn.",".get_ou('applicationou').$baseobject->base;
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
