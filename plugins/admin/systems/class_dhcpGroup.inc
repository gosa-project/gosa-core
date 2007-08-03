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
      $this->new= FALSE;


      /* Load options */
      if (isset($attrs['dhcpOption'])){
        for($i= 0; $i<$attrs['dhcpOption']['count']; $i++){
          $tmp= $attrs['dhcpOption'][$i];
          $idx= preg_replace('/\s.+$/', '', $tmp);
          $value= preg_replace('/^[^\s]+\s/', '', $tmp);
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        for($i= 0; $i<$attrs['dhcpStatements']['count']; $i++){
          $tmp= $attrs['dhcpStatements'][$i];
          $idx= preg_replace('/\s.+$/', '', $tmp);
          $value= preg_replace('/^[^\s]+\s/', '', $tmp);
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
    $this->advanced->setAutoStatements(array("fixed-address"));

    /* Save CN for later reference */
    $this->orig_cn= $this->cn;
  }

  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);

    if ($this->dn != "new"){
      $smarty->assign("mode", "readonly");
    }

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
  function check()
  {
    $message= array();

    /* All required fields are set? */
    if ($this->cn == ""){
      $message[]= _("Required field 'Name' is not filled.");
    }

    /* cn already used? */
    echo "TODO: no ldap check";
    if ($this->dn != "new"){
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=dhcpGroup)(cn=".$this->cn."))");
      if ($ldap->count() >= 1){
        while ($attrs= $ldap->fetch()){
          if ($ldap->getDN() != $this->dn){
            $message[]= _("The name for this group section is already used!");
            break;
          }

        }
      }
      $ldap->fetch();
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
    echo "TODO: save";
    return;

    /* Get ldap mode */
    if ($this->dn == "new"){
      $mode= "add";
    } else {
      $mode= "modify";
    }

    /* Generate new dn */
    if ($this->parent->parent != NULL && $this->dn == "new"){
      $this->dn= "cn=".$this->cn.",".$this->parent->parent;
    }


    /* Assemble new entry - options */
    if (isset ($this->options) && count ($this->options)){
      $this->attrs['dhcpOption']= array();
      foreach ($this->options as $key => $val){
        $this->attrs['dhcpOption'][]= "$key $val";
      }
    } else {
      if ($mode == "modify"){
        $this->attrs['dhcpOption']= array();
      }
    }

    /* Assemble new entry - statements */
    if (isset ($this->statements) && count ($this->statements)){
      $this->attrs['dhcpStatements']= array();
      foreach ($this->statements as $key => $val){
        $this->attrs['dhcpStatements'][]= "$key $val";
      }
    } else {
      if ($mode == "modify"){
        $this->attrs['dhcpStatements']= array();
      }
    }

    /* Do LDAP action */
    $ldap= $this->config->get_ldap_link();
    if ($mode == "add"){
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }
    $ldap->cd($this->dn);
    $ldap->$mode($this->attrs);
    show_ldap_error($ldap->get_error());
  }

}

?>
