<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003  Cajus Pollmeier

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

class dhcpGroup extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $options= array();
  var $statements= array();
  var $orig_cn= "";
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpGroup($attrs)
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
          $value= preg_replace('/^[^\s]+\s/', '', $opt);
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        foreach ($attrs['dhcpStatements'] as $opt){
          $idx= preg_replace('/\s.+$/', '', $opt);
          $value= preg_replace('/^[^\s]+\s/', '', $opt);
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
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);

    /* Show main page */
    $display= $smarty->fetch (get_template_path('dhcp_group.tpl', TRUE)).$this->network->execute();

    /* Merge arrays for advanced view */
    foreach (array("options", "statements") as $type){
      $tmp= array_merge($this->$type, $this->network->$type);
      $this->advanced->$type= $tmp;
    }

    $display.= $this->advanced->execute();

    /* Merge back for removals */
    foreach (array("options", "statements") as $type){
      $this->$type= $this->advanced->$type;
      $this->network->$type= $this->advanced->$type;
    }

    /* Add footer */
    $display.= "<div style='width:100%;text-align:right;margin-top:5px;'><input type=submit name='save_dhcp' value='"._("Save")."'>".
               "&nbsp;<input type=submit name='cancel_dhcp' value='"._("Cancel")."'></div>";


    return ($display);
  }


  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    /* Save cn */
    if (isset($_POST['cn'])){
      $this->cn= validate($_POST['cn']);
    }

    /* Save sub-objects */
    $this->network->save_object();
    $this->advanced->save_object();

    /* Merge arrays for advanced view */
    foreach (array("options", "statements") as $type){
      $tmp= array_merge($this->$type, $this->network->$type);
      $this->advanced->$type= $tmp;
    }
  }


  /* Check values */
  function check($cache)
  {
    $message= array();

    /* All required fields are set? */
    if ($this->cn == ""){
      $message[]= _("Required field 'Name' is not filled.");
    }
    if (!preg_match('/^[a-z0-9_-]*$/i', $this->cn)){
      $message[]= _("Field 'Name' contains illegal characters.");
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    /* Merge arrays for network and advanced view */
    foreach (array("options", "statements") as $type){
      $tmp= array_merge($this->$type, $this->network->$type, $this->advanced->$type);
      $this->$type= $tmp;
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
        $this->attrs['dhcpStatements'][]= "$key $val";
      }
    }

    /* Move dn to the result */
    $this->attrs['dn']= $this->dn;
    $this->attrs['cn']= array($this->cn);
    $this->attrs['objectClass']= array('top', 'dhcpGroup', 'dhcpOptions');
    $this->attrs['MODIFIED']= TRUE;

    return ($this->attrs);
  }


}

?>
