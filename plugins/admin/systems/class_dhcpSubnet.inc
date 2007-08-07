<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003-2007  Cajus Pollmeier

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

class dhcpSubnet extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $dhcpNetMask= 24;
  var $dhcpRange= "";
  var $range_start= "";
  var $range_stop= "";
  var $options= array();
  var $statements= array();
  var $use_range= FALSE;

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpSubnet($attrs)
  {
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];


      /* Load attributes */
      foreach (array("cn", "dhcpNetMask", "dhcpRange") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
      }
      if (isset($attrs['dhcpRange'])){
        $this->use_range= TRUE;
        list($this->range_start, $this->range_stop)= preg_split('/\s+/', $this->dhcpRange);
      }

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
    $this->dhcpNetMask= normalize_netmask($this->dhcpNetMask);
  }


  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("dhcp_netmask", $this->dhcpNetMask);

    /* Prepare range */
    if ($this->use_range){
      $smarty->assign("use_range", "checked");
      $smarty->assign("range_disabled", "");
    } else {
      $smarty->assign("use_range", "");
      $smarty->assign("range_disabled", "disabled");
    }
    $smarty->assign("range_start", $this->range_start);
    $smarty->assign("range_stop", $this->range_stop);

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_subnet.tpl', TRUE)).$this->network->execute();

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

    /* Show main page */
    return $display;
  }


  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['cn'])){
      $this->cn= validate($_POST['cn']);
    } 
    if (isset($_POST['dhcp_netmask'])){
      $this->dhcpNetMask= validate($_POST['dhcp_netmask']);
    } 
    if (isset($_POST['use_range'])){
      $this->use_range= TRUE;
      $this->range_start= validate($_POST['range_start']);
      $this->range_stop= validate($_POST['range_stop']);
    } else {
      $this->use_range= FALSE;
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
      $message[]= _("Required field 'Network address' is not filled.");
    }
    if ($this->dhcpNetMask == ""){
      $message[]= _("Required field 'Netmask' is not filled.");
    }

    /* cn already used? */
    if ($this->orig_cn != $this->cn || $this->new){

      foreach($cache as $dn => $dummy){
        if (preg_match("/^cn=".$this->cn.",/", $dn) && count($dummy)){
          $message[]= _("The name for this section is already used!");
          break;
        }
      }
    }

    /* IP's? */
    foreach(array('dhcpNetMask' => _("Netmask"), 'cn' => _("Network address"), 'range_start' => _("Range"), 'range_stop' => _("Range")) as $attr => $str){
      if ($this->$attr != "" && !is_ip($this->$attr)){
        print_red(sprintf(_("The field '%s' contains an invalid IP address"), $str));
      }
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
    $this->attrs['dhcpNetMask']= array(netmask_to_bits($this->dhcpNetMask));
    if ($this->dhcpRange != ""){
      $this->attrs['dhcpRange']= array($this->range_start." ".$this->range_stop);
    } else {
      $this->attrs['dhcpRange']= array();
    }
    $this->attrs['objectClass']= array('top', 'dhcpSubnet', 'dhcpOptions');
    $this->attrs['MODIFIED']= TRUE;

    return ($this->attrs);
  }
  
}

?>
