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

      /* Don't touch base object */
      if ($name != 'user'){
        $obj->parent= &$this;
        $obj->uid= $baseobject->uid;
        $obj->sn= $baseobject->uid;
        $obj->givenName= $baseobject->uid;
        $this->by_object[$name]= $obj;
      }

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
    if ($this->last == "gofaxAccount"){

      /* Move number from fax plugin */
      $this->by_object['user']->facsimileTelephoneNumber=
        $this->by_object['gofaxAccount']->facsimileTelephoneNumber;
    }
    if ($this->last == "phoneAccount" && $this->by_object['phoneAccount']->is_account){
      /* Move number from fax plugin */
      reset($this->by_object['phoneAccount']->phoneNumbers);
      $number= key($this->by_object['phoneAccount']->phoneNumbers);
      $this->by_object['user']->telephoneNumber= $number;
    }
  }

  function save()
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['user'];
    if ($this->dn == 'new'){
      $baseobject->cn= $baseobject->givenName." ".$baseobject->sn;
      $this->by_object['user']= $baseobject;
      if (isset($this->config->current['DNMODE']) && $this->config->current['DNMODE'] == "uid"){
        $this->dn= 'uid='.$baseobject->uid.','.get_people_ou().$baseobject->base;
      } else {
        $this->dn= 'cn='.$baseobject->cn.','.get_people_ou().$baseobject->base;
      }
    } // != 'new' 
    else 
    {
      /* Extract cn and compare with actual cn present in 'dn' */
      if (isset($this->config->current['DNMODE']) && $this->config->current['DNMODE'] == "uid"){
        $new_dn= 'uid='.$baseobject->uid.','.get_people_ou().$baseobject->base;
      } else {
        $new_dn= "cn=".$baseobject->givenName." ".$baseobject->sn.','.
          get_people_ou().$baseobject->base;
      }
      
      /* Hickert: 31.08.2005 
       * fixed error = Trying to overwrite cn=***  which already exists.
       * If you changed one of the "Name" / "Givenname" attributes to lower or upper case, like "test" to "Test", (in DNMODE=cn)
       *  the old_dn and new dn were different. cn=test ... cn=Test ...
       * The following code decided to move the entry from old_dn to new_dn. (Because they differ)
       * But the ldap functions are not case sensitive and so the function plugin::move() quits with 
       *  an error, because source and destination dn are both the same (for ldap, but not for php).
       * This fix allow you to change the givenname or name attributes.
       */
      if(strtolower($this->dn)==strtolower($new_dn)){
        $this->dn=$new_dn;
      }
  
      if ($this->dn != $new_dn){

        /* Write entry on new 'dn' */
        $baseobject->cn= $baseobject->givenName." ".$baseobject->sn;
        $baseobject->move($this->dn, $new_dn);
        $this->by_object['user']= $baseobject;

        /* Did we change ourselves? Update ui object. */
        change_ui_dn($this->dn, $new_dn);

        /* Happen to use the new one */
        $this->dn= $new_dn;
      }
    }
    print_a($this);
    return tabs::save();
  }


  function set_template_mode()
  {
    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->is_template= TRUE;
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
