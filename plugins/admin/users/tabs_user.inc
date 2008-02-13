<?php

class usertabs extends tabs
{
  var $base;

  function usertabs($config, $data, $dn)
  {
    tabs::tabs($config, $data, $dn);
    $this->base= $this->by_object['user']->base;
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    /* Update reference, transfer variables */
    $baseobject= $this->by_object['user'];
    foreach ($this->by_object as $name => $obj){

      /* Adding uid to sub plugins of connectivity */
      if($name == "connectivity"){
        foreach ($obj->plugin_name as $plg_name){
          if(isset($obj->plugin[$plg_name]->uid)){
            $obj->plugin[$plg_name]->uid = $baseobject->uid;
          }
        }
      }

      /* Copy mail if needed */
      if ($name == "gofaxAccount"){
        if (isset($this->by_object['mailAccount']) && $this->by_object['mailAccount']->is_account){
          $obj->mail= $this->by_object['mailAccount']->mail;
        }
      }

      /* Don't touch base object */
      if ($name != 'user'){
        $obj->parent= &$this;
        $obj->uid= $baseobject->uid;
        $obj->sn= $baseobject->uid;
        $obj->givenName= $baseobject->uid;
      }

      /* Store changes */
      $this->by_object[$name]= $obj;

      /* Update parent in base object */
      $this->by_object['user']->parent= &$this;
    }

    /* Move facsimile / phone number if nessecary */
    if ($this->last == "user" && isset($this->by_object['gofaxAccount'])){

      /* Move number to fax plugin */
      $this->by_object['gofaxAccount']->facsimileTelephoneNumber=
        $this->by_object['user']->facsimileTelephoneNumber;

      /* Move phone number if plugin exists */
      if (isset($this->by_object['phoneAccount']) && 
          !$this->by_object['phoneAccount']->is_account){
        $this->by_object['phoneAccount']->phoneNumbers= array();
        if ($this->by_object['user']->telephoneNumber != ""){
          $this->by_object['phoneAccount']->phoneNumbers[$this->by_object['user']->telephoneNumber]= $this->by_object['user']->telephoneNumber;
        }
      }
    }

    /* Move number from fax plugin */
    if ($this->last == "gofaxAccount"){

      $this->by_object['user']->facsimileTelephoneNumber=
        $this->by_object['gofaxAccount']->facsimileTelephoneNumber;
    }

    /* Move number from fax plugin */
    if ($this->last == "phoneAccount" && $this->by_object['phoneAccount']->is_account){
      reset($this->by_object['phoneAccount']->phoneNumbers);
      $number= key($this->by_object['phoneAccount']->phoneNumbers);

      /* Only the first phoneAccount number, if it is not empty */
      if(!empty($number)){
        $this->by_object['user']->telephoneNumber= $number;
      }
    }
  }

  function save()
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['user'];

    if ($this->dn != 'new'){
      $new_dn= $baseobject->new_dn;
      if ($this->dn != $new_dn){

        $baseobject->move($this->dn, $new_dn);
        $this->by_object['user']= $baseobject;

        /* Did we change ourselves? Update ui object. */
        change_ui_dn($this->dn, $new_dn);
      }
    }

    $this->dn= $baseobject->new_dn;

    return tabs::save();
  }


  function set_template_mode()
  {
    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->is_template= TRUE;
    }
  }
  
  function saveCopyDialog()
  {
    tabs::saveCopyDialog();
    $baseobject= $this->by_object['user'];
    $uid = $baseobject->uid;
    foreach($this->by_object as  $name => $obj){
      $this->by_object[$name]->uid = $uid;
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
