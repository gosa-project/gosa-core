<?php

class apptabs extends tabs
{
  var $parent   = FALSE;
  var $FAIstate = "";

  function __construct($config, $data, $dn,$category,$hide_refs = FALSE, $hide_acls = FALSE)
  {
    tabs::__construct($config, $data, $dn,$category,$hide_refs, $hide_acls);

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function set_FAIstate($state)
  {
    $this->FAIstate = $state;
    foreach($this->by_name as $name => $desc){
      $this->by_object[$name]->FAIstate = $state;
    }
  }

  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['application'];
    $cn      = preg_replace('/,/', '\,', $baseobject->cn); 
    $cn      = preg_replace('/"/', '\"', $cn); 

    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    if($baseobject->parent->parent->IsReleaseManagementActivated()){
      $new_dn= "cn=".$cn.",".$baseobject->parent->parent->app_release;
    }else{
      $new_dn= "cn=".$cn.",".get_ou("application", "applicationRDN").$baseobject->base;
    }
    
    // Do we have to move the object?
    if ($this->dn != $new_dn){
      if ($this->dn != "new"){
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['application']= $baseobject;
      }
      $this->dn= $new_dn;
    }

    tabs::save();
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
