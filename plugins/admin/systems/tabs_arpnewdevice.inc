<?php

class ArpNewDeviceTabs extends tabs
{

  function save($ignore_account= FALSE)
  {
    tabs::save($ignore_account);
  }

  function execute()
  {
    tabs::execute(); 
    return($this->by_object[$this->current]->execute());
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
