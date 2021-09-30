<?php

class printtabs extends tabs
{

  function __construct($config, $data, $dn,$category,$hide_refs = FALSE, $hide_acls = FALSE)
  {

    /* Save dn */
    $this->dn= $dn;
    $this->config= $config;

    $this->hide_acls = $hide_acls;
    $this->hide_refs = $hide_refs;

    $this->acl_category = $category;

    $baseobject= NULL;

    foreach ($data as $tab){
      if(!class_available($tab['CLASS'])) continue;

      $this->by_name[$tab['CLASS']]= $tab['NAME'];

      if ($baseobject === NULL){
        $baseobject= new $tab['CLASS']($this->config, $this->dn,NULL,$this);
        $this->by_object[$tab['CLASS']]= $baseobject;
      } else {
        $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn, $baseobject,$this);
      }

      $this->by_object[$tab['CLASS']]->parent= &$this;
      $this->by_object[$tab['CLASS']]->set_acl_category($category);
      $this->read_only |= $this->by_object[$tab['CLASS']]->read_only;      

      /* Initialize current */
      if ($this->current == ""){
        $this->current= $tab['CLASS'];
      }
    }

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }


  /*! \brief Reinitializes the tab classes with fresh ldap values.
    This maybe usefull if for example the apply button was pressed.
   */
  function re_init()
  {
    $baseobject= NULL;
    foreach($this->by_object as $name => $object){
      $class = get_class($object);
      if(in_array_strict($class,array("reference","acl"))) continue;
      if ($baseobject === NULL){
        $baseobject= new $class($this->config, $this->dn,NULL,$this);
        $baseobject->enable_CSN_check();
        $this->by_object[$name]= $baseobject;
      } else {
        $this->by_object[$name]= new $class($this->config, $this->dn, $baseobject,$this);
      }
      $this->by_object[$name]->parent= &$this;
      $this->by_object[$name]->set_acl_category($this->acl_category);
    }
  }


  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    /* Assign current cn */
    $baseobject= $this->by_object['printgeneric'];
    $baseobject->netConfigDNS->cn= $baseobject->cn;
  }


  function save($ignore_account= FALSE)
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['printgeneric'];
    $cn      = preg_replace('/,/', '\,', $baseobject->cn);
    $cn      = preg_replace('/"/', '\"', $cn);
    $this->dn= "cn={$cn},".get_ou("printgeneric", "printerRDN").$baseobject->base;
    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
    }

    tabs::save(FALSE);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
