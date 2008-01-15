<?php

class servtabs extends tabs
{

  function servtabs($config, $data, $dn,$category)
  {
    /* Save dn */
    $this->dn= $dn;
    $this->config= $config;

    $baseobject= NULL;

    foreach ($data as $tab){
      $this->by_name[$tab['CLASS']]= $tab['NAME'];

      if ($baseobject === NULL){
        $baseobject= new $tab['CLASS']($this->config, $this->dn,NULL,$this);
        $this->by_object[$tab['CLASS']]= $baseobject;
      } else {
        $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn, $baseobject,$this);
      }

      $this->by_object[$tab['CLASS']]->parent= &$this;
      $this->by_object[$tab['CLASS']]->set_acl_category($category);

      /* Initialize current */
      if ($this->current == ""){
        $this->current= $tab['CLASS'];
      }
    }

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);
  }


  function save($ignore_account= FALSE)
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['servgeneric'];
    $this->dn= "cn=$baseobject->cn,".get_ou('serverou').$baseobject->base;
    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
      $this->by_object[$key]->cn= $baseobject->cn;
    }

    tabs::save(FALSE);

    /* Reload server settings after saving */
    $config = session::get('config');
    $config->load_servers();
    session::set('config',$config);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
