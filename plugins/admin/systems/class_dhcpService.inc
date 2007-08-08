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

class dhcpService extends plugin
{
  /* Used attributes */
  var $cn= "";
  var $orig_cn= "";
  var $dhcpPrimaryDN= "";
  var $orig_dhcpPrimaryDN= "";
  var $options= array();
  var $statements= array();
  var $ddns_styles= array('none', 'interim', 'ad-hoc');

  /* Subobjects */
  var $network;
  var $advanced;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();


  function dhcpService($attrs)
  {
    /* Load statements / options */
    if (is_array($attrs)){
      $this->dn= $attrs['dn'];
      $this->new= FALSE;

      /* Load attributes */
      foreach (array("cn", "dhcpPrimaryDN") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
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
      $this->statements['default-lease-time']= 600;
      $this->statements['max-lease-time']= 1700;
      $this->statements['authoritative']= TRUE;
      $this->statements['get-lease-hostnames']= 'on';
      $this->statements['ddns-update-style']= 'none';
    }

    /* Load network module */
    $this->network= new dhcpNetwork();
    $this->network->options= $this->options;
    $this->network->statements= $this->statements;
    $this->advanced= new dhcpAdvanced();
    $this->advanced->options= $this->options;
    $this->advanced->statements= $this->statements;
    $this->advanced->setAutoStatements(array("default-lease-time", "max-lease-time", "authoritative", "get-lease-hostnames", "server-identifier", "ddns-update-style"));
    $this->advanced->setAutoOptions(array("server-name"));

    /* Save for later action */
    $this->orig_dhcpPrimaryDN= $this->dhcpPrimaryDN;
    $this->orig_cn= $this->cn;
  }


  function execute()
  {
    /* Show main page */
    $smarty= get_smarty();

    $smarty->assign('ddns_styles', $this->ddns_styles);
    foreach (array('max_lease_time', 'default_lease_time', 'ddns_update_style') as $value){
      if (isset($this->statements[preg_replace('/_/', '-', $value)])){
        $smarty->assign("$value", $this->statements[preg_replace('/_/', '-', $value)]);
      } else {
        $smarty->assign("$value", "");
      }
    }

    if (isset($this->statements['authoritative'])){
      $smarty->assign("authoritative", "checked");
    } else {
      $smarty->assign("authoritative", "");
    }

    if (isset($this->statements['get-lease-hostnames']) && preg_match('/on/i', $this->statements['get-lease-hostnames'])){
      $smarty->assign("get_lease_hostnames", "checked");
    } else {
      $smarty->assign("get_lease_hostnames", "");
    }

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_service.tpl', TRUE)).$this->network->execute();

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
    /* No need to save in the first time */
    if (!isset($_POST['ddns_update_style'])){
      return;
    }

    /* Save remaining attributes */
    foreach (array('max_lease_time', 'default_lease_time', 'ddns_update_style') as $val){
      $tval= preg_replace('/_/', '-', $val);
      if ($_POST[$val] != ""){
        $this->statements[$tval]= validate($_POST[$val]);
      } else {
        unset ($this->statements[$tval]);
      }
    }
    if (isset($_POST['authoritative'])){
      $this->statements['authoritative']= "";
    } else {
      unset($this->statements['authoritative']);
    }
    if (isset($_POST['get_lease_hostnames'])){
      $this->statements['get-lease-hostnames']= "on";
    } else {
      $this->statements['get-lease-hostnames']= "off";
    }

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
      $tmp= array_merge($this->$type, $this->network->$type);
      $this->advanced->$type= $tmp;
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
    global $config;
    $this->attrs= array();

    /* Get and set server name */
    $ldap= $config->get_ldap_link();
    $ldap->cat($this->dhcpPrimaryDN, array('cn'));
    $res= $ldap->fetch();
    $server_name= $res['cn'][0];
    
    /* Merge arrays for network and advanced view */
    foreach (array("options", "statements") as $type){
      $tmp= array_merge($this->$type, $this->network->$type, $this->advanced->$type);
      $this->$type= $tmp;
    }

    /* Assemble new entry - options */
    $this->attrs['dhcpOption']= array();
    if (isset ($this->options) && count ($this->options)){
      foreach ($this->options as $key => $val){
        if ($key != "server-name"){
          $this->attrs['dhcpOption'][]= "$key $val";
        }
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

    $this->attrs['dn']= $this->dn;
    $this->attrs['cn']= array($this->cn);
    $this->attrs['dhcpPrimaryDN']= array($this->dhcpPrimaryDN);
    $this->attrs['objectClass']= array('top', 'dhcpService');
    $this->attrs['MODIFIED']= TRUE;
    $this->attrs['dhcpOption'][]= "server-name $server_name";

    return ($this->attrs);
  }
  
}

?>
