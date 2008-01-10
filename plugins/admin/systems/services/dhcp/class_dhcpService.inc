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

class dhcpService extends dhcpPlugin
{
  /* Used attributes */
  var $dhcpPrimaryDN= "";
  var $orig_dhcpPrimaryDN= "";
  var $ddns_styles= array('none', 'interim', 'ad-hoc');

  /* attribute list for save action */
  var $objectclasses= array('top', 'dhcpService');


  function dhcpService($attrs)
  {
    dhcpPlugin::dhcpPlugin($attrs);

    /* Load statements / options */
    if (!$this->new){
      /* Load attributes */
      $this->dhcpPrimaryDN= $attrs['dhcpPrimaryDN'][0];
    } else {
      /* We keep the parent dn here if it's new */
      $this->statements['default-lease-time']= 600;
      $this->statements['max-lease-time']= 1700;
      $this->statements['authoritative']= TRUE;
      $this->statements['ddns-update-style']= 'none';
    }

    $this->advanced->setAutoStatements(array("default-lease-time", "max-lease-time", "authoritative", "server-identifier", "ddns-update-style"));
    $this->advanced->setAutoOptions(array("server-name"));

    /* Save for later action */
    $this->orig_dhcpPrimaryDN= $this->dhcpPrimaryDN;
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

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_service.tpl', TRUE)).$this->network->execute();

    /* Merge arrays for advanced view */
    $this->fix_options();
    foreach (array("options", "statements") as $type){
      $this->advanced->$type= $this->$type + $this->network->$type;;
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
        $this->statements[$tval]= validate(get_post($val));
      } else {
        unset ($this->statements[$tval]);
      }
    }
    if (isset($_POST['authoritative'])){
      $this->statements['authoritative']= "";
    } else {
      unset($this->statements['authoritative']);
    }

    dhcpPlugin::save_object();
  }


  /* Check values */
  function check()
  {
    $message= array();

    if (!tests::is_id($this->statements['default-lease-time'])){
      $message[]= _('Default lease time needs to be numeric.');
    }
    if (!tests::is_id($this->statements['max-lease-time'])){
      $message[]= _('Maximum lease time needs to be numeric.');
    }
    if ($this->statements['default-lease-time'] > $this->statements['max-lease-time']){
      $message[]= _('Default lease time needs to smaller than the maximum lease time.');
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
    global $config;
    $this->attrs= array();

    /* Get and set server name */
    $ldap= $config->get_ldap_link();
    $ldap->cat($this->dhcpPrimaryDN, array('cn'));
    $res= $ldap->fetch();
    $server_name= $res['cn'][0];
    
    dhcpPlugin::save();

    $this->attrs['dhcpPrimaryDN']= array($this->dhcpPrimaryDN);
    $this->removeOption('server-name');
#    $this->attrs['dhcpOption'][]= "server-name $server_name";

    return ($this->attrs);
  }
  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
