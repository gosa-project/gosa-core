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

class dhcpHost extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $dhcpHWAddress= "";
  var $options= array();
  var $statements= array();
  var $dn= "";

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpHost($attrs)
  {
    /* Load statements / options */
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->new= FALSE;

      /* Load attributes */
      foreach (array("cn", "dhcpHWAddress") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
      }

      /* Load options */
      if (isset($attrs['dhcpOption'])){
        foreach ($attrs['dhcpOption'] as $opt){
          if ($opt == "count"){
            continue;
          }
          $idx= preg_replace('/\s.+$/', '', $opt);
          $value= preg_replace('/^[^\s]+\s/', '', $opt);
          $this->options[$idx]= $value;
        }
      }

      /* Load statements */
      if (isset($attrs['dhcpStatements'])){
        foreach ($attrs['dhcpStatements'] as $opt){
          if ($opt == "count"){
            continue;
          }
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
    $this->advanced->setAutoStatements(array("fixed-address"));

    /* Save CN for later reference */
    $this->orig_cn= $this->cn;
  }

  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("dhcpHWAddress", preg_replace('/^[^ ]+ /', '', $this->dhcpHWAddress));

    /* Create fixed address */
    if (isset($this->statements['fixed-address'])){
      $smarty->assign("fixedaddr", $this->statements['fixed-address']);
    } else {
      $smarty->assign("fixedaddr", "");
    }

    /* Prepare hw type selector */
    $hwtype= preg_replace('/\s.*$/', '', $this->dhcpHWAddress);
    $smarty->assign("hwtype", $hwtype);
    $smarty->assign("hwtypes", array("ethernet" => _("Ethernet"),
          "fddi" => _("FDDI"),
          "token-ring" => _("Token Ring")));
    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcphost.tpl', TRUE)).$this->network->execute();

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
    plugin::save_object();

    /* Save remaining attributes */
    if (isset($_POST['hwtype'])){

      /* Assemble hwAddress */
      $this->dhcpHWAddress= $_POST['hwtype']." ".$_POST['dhcpHWAddress'];
      $this->cn= validate($_POST['cn']);

      /* Save fixed address */
      if ($_POST['fixedaddr'] != ""){
        $this->statements['fixed-address']= $_POST['fixedaddr'];
      } else {
        unset ($this->statements['fixed-address']);
      }
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

    /* cn already used? */
    if ($this->orig_cn != $this->cn || $this->new){
      
      foreach($cache as $dn => $dummy){
        if (preg_match("/^cn=".$this->cn.",/", $dn) && count($dummy)){
          $message[]= _("The name for this host section is already used!");
          break;
        }
      }
    }

    /* Check syntax of MAC address */
    $check= preg_replace('/^[^\s]*\s/', '', $this->dhcpHWAddress);
    if (!preg_match('/^([0-9a-fA-F]{1,2}:){5}[0-9a-fA-F]{1,2}$/', $check)){
      $message[]= _("The hardware address specified by you is not valid!");
    }

    /* Check external plugins */
    $net= $this->network->check();
    $adv= $this->advanced->check();
    $message= array_merge($message, $net, $adv);

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
    if ($this->dhcpHWAddress != ""){
      $this->attrs['dhcpHWAddress']= array($this->dhcpHWAddress);
    } else {
      $this->attrs['dhcpHWAddress']= array();
    }
    $this->attrs['objectClass']= array('top', 'dhcpHost');
    $this->attrs['MODIFIED']= TRUE;

    return ($this->attrs);
  }

}

?>
