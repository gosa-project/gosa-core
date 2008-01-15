<?php

class mimetabs extends tabs
{
  var $Release= "";

  function mimetabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
  }

  function set_release($newRelease)
  {
    $this->Release= preg_replace('/,ou=mime,.*$/', '', $newRelease);
    if ($this->Release != ""){
      $this->Release= ",".$this->Release;
    }
  }

  function save()
  {
    $baseobject= $this->by_object['mimetype'];

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $tmp = search_config($this->config->data,"faiManagement","CLASS");

    if((!empty($tmp)) && (isset($_SESSION['mimefilter']['release']))){
      if(!$baseobject->isReleaseMimeType){
        $new_dn= "cn=".$baseobject->cn.",ou=mime,".$baseobject->base;
      }else{
        $new_dn ="cn=".$baseobject->cn.",".$_SESSION['mimefilter']['release'];
      }
    }else{
      $new_dn= "cn=".$baseobject->cn.",ou=mime,".$baseobject->base;
    }

    /* Move group? */
    if ($this->dn != $new_dn){

      /* Write entry on new 'dn' */
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['mimetype']= $baseobject;
      }

      /* Happen to use the new one */
      $this->dn= $new_dn;
    }

    tabs::save();

    /* Fix tagging if needed */
    $baseobject->dn= $this->dn;
    $baseobject->handle_object_tagging();
    $this->by_object['mimetype'] = $baseobject;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
