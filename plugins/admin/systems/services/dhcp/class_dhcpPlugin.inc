<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003-2007 Cajus Pollmeier

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

class dhcpPlugin extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $options= array();
  var $statements= array();

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpPlugin($attrs)
  {
    /* Load statements / options */
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->cn= $attrs['cn'][0];
      $this->new= FALSE;

      /* Load options */
      if (isset($attrs['dhcpOption'])){
        foreach ($attrs['dhcpOption'] as $opt){
          $idx= preg_replace('/\s.+$/', '', $opt);
          if ($idx == $opt){
            $value= "";
          } else {
            $value= preg_replace('/^[^\s]+\s/', '', $opt);
          }
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        foreach ($attrs['dhcpStatements'] as $opt){
          $idx= preg_replace('/\s.+$/', '', $opt);
          if ($idx == $opt){
            $value= "";
          } else {
            $value= preg_replace('/^[^\s]+\s/', '', $opt);
          }
          $this->statements[$idx]= $value;
        }
      }

    } else {
      /* We keep the parent dn here if it's new */
      $this->dn= $attrs;
      $this->new= TRUE;
    }

    /* Load network module */
    $this->network= new dhcpNetwork();
    $this->network->options= $this->options;
    $this->network->statements= $this->statements;
    $this->advanced= new dhcpAdvanced();
    $this->advanced->options= $this->options;
    $this->advanced->statements= $this->statements;

    /* Save CN for later reference */
    $this->orig_cn= $this->cn;
  }

  function execute()
  {
    return ("");
  }


  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    /* Strip network objects */
    foreach (array("routers", "domain-name", "subnet-mask", "broadcast-address") as $toberemoved){
      unset($this->options[$toberemoved]);
    }
    foreach (array("filename", "next-server") as $toberemoved){
      unset($this->statements[$toberemoved]);
    }

    /* Save sub-objects */
    $this->network->save_object();
    $this->advanced->save_object();

    /* Merge arrays for advanced view */
    foreach (array("options", "statements") as $type){
      $this->advanced->$type= $this->$type + $this->network->$type;;
    }
  }


  /* Check values */
  function check()
  {
    $message= array();
    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Merge arrays for network and advanced view */
    foreach (array("options", "statements") as $type){
      $this->$type= $this->$type + $this->network->$type + $this->advanced->$type;
    }

    /* Add cn if we're new */
    if ($this->new){
      $this->dn= "cn=".$this->cn.",".$this->dn;
    } else {
      $this->dn= "cn=".$this->cn.preg_replace('/^cn=[^,]+/', '', $this->dn);
    }

    /* Assemble new entry - options */
    $this->attrs['dhcpOption']= array();
    if (isset ($this->options) && count ($this->options)){
      foreach ($this->options as $key => $val){
        $this->attrs['dhcpOption'][]= "$key $val";
      }
    }

    /* Assemble new entry - statements */
    $this->attrs['dhcpStatements']= array();
    if (isset ($this->statements) && count ($this->statements)){
      foreach ($this->statements as $key => $val){
        if ($val != ""){
          $this->attrs['dhcpStatements'][]= "$key $val";
        } else {
          $this->attrs['dhcpStatements'][]= "$key";
        }
      }
    }

    /* Move dn to the result */
    $this->attrs['dn']= $this->dn;
    $this->attrs['cn']= array($this->cn);
    $this->attrs['objectClass']= $this->objectclasses;
    $this->attrs['MODIFIED']= TRUE;

    return ($this->attrs);
  }


  function removeAttrs($name, $type)
  {
    $new= array();
    foreach ($this->attrs[$type] as $value){
      if (!preg_match("/^$name /", $value)){
        $new[]= $value;
      }
    }
    $this->attrs[$type]= $new;
  }


  function removeOption($name)
  {
    $this->removeAttrs($name, 'dhcpOption');
  }


  function removeStatement($name)
  {
    $this->removeAttrs($name, 'dhcpStatement');
  }


  function fix_options()
  {
    foreach (array('domain-name-servers') as $key){
      unset ($this->options[$key]);
    }
  }

}

?>
