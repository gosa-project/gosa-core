<?php

class printtabs extends tabs
{

  function printtabs($config, $data, $dn)
  {
    /* Save dn */
    $this->dn= $dn;
    $this->config= $config;

    foreach ($data as $tab){
      if($tab['CLASS'] == "printgeneric"){
        $this->by_name[$tab['CLASS']]= $tab['NAME'];
        $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn,$this);
        $this->by_object[$tab['CLASS']]->parent= &$this;
      }else{
        $this->by_name[$tab['CLASS']]= $tab['NAME'];
        $this->by_object[$tab['CLASS']]= new $tab['CLASS']($this->config, $this->dn);
        $this->by_object[$tab['CLASS']]->parent= &$this;
      }
      /* Initialize current */
      if ($this->current == ""){
        $this->current= $tab['CLASS'];
      }
    }
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);
  }


  function save()
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['printgeneric'];
    $this->dn= "cn=$baseobject->cn,ou=printers,ou=systems,".$baseobject->base;
    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
    }

    tabs::save(FALSE);

    /* Fix tagging if needed */
    $baseobject->handle_object_tagging();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
