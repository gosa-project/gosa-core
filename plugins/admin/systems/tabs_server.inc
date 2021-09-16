<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: tabs_server.inc 20962 2011-07-27 08:00:12Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class servtabs extends tabs
{
  public $was_activated = FALSE;

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
  
      if(!class_available($tab['CLASS'])){
        continue;
      }

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

    $baseobject = $this->by_object['servgeneric'];
    $baseobject->netConfigDNS->cn= $baseobject->cn;
  }


  function save($ignore_account= FALSE)
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['servgeneric'];
    $cn      = preg_replace('/,/', '\,', $baseobject->cn);
    $cn      = preg_replace('/"/', '\"', $cn);
    $this->dn= "cn={$cn},".get_ou("servgeneric", "serverRDN").$baseobject->base;

    /* cn is not case sensitive for ldap, but for php it is!! */
    if($baseobject->orig_dn != "new"){
      if($this->config->get_cfg_value("core","accountPrimaryAttribute") == "cn"){
        if (strtolower($baseobject->orig_dn) != (strtolower($this->dn))){
          $baseobject->recursive_move($baseobject->orig_dn, $this->dn);
        }
      }else{
        if ($baseobject->orig_dn != $this->dn){
          $baseobject->recursive_move($baseobject->orig_dn, $this->dn);
        }
      }
    }

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
