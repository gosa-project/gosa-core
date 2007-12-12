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

class dhcpHost extends dhcpPlugin
{
  /* Used attributes */
  var $dhcpHWAddress= "";
  var $realGosaHost = FALSE;
	
  /* attribute list for save action */
  var $objectclasses= array("top", "dhcpHost");

  function dhcpHost($attrs,$host_exists_in_gosa = FALSE)
  {
    dhcpPlugin::dhcpPlugin($attrs);

    /* Load attributes */
    if (!$this->new){
      $this->dhcpHWAddress= $attrs['dhcpHWAddress'][0];
    }

    $this->advanced->setAutoOptions(array("host-name"));
    $this->advanced->setAutoStatements(array("fixed-address"));

	$this->realGosaHost = $host_exists_in_gosa;
  }

  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);
    $smarty->assign("dhcpHWAddress", preg_replace('/^[^ ]+ /', '', $this->dhcpHWAddress));
    $smarty->assign("realGosaHost",$this->realGosaHost);

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
    $display= $smarty->fetch(get_template_path('dhcp_host.tpl', TRUE,dirname(__FILE__))).$this->network->execute();

    /* Merge arrays for advanced view */
    $this->fix_options();
    foreach (array("options", "statements") as $type){
      $this->advanced->$type= $this->$type + $this->network->$type;
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
    /* Save remaining attributes */
    if (isset($_POST['dhcp_host_posted'])){

      /* Assemble hwAddress */
      if (isset($_POST['dhcpHWAddress'])){
        $this->dhcpHWAddress= get_post('hwtype')." ".get_post('dhcpHWAddress');
      }
		
      if(!$this->realGosaHost){
        $this->cn= validate(get_post('cn'));
      }

      /* Save fixed address */
      if(!$this->realGosaHost){
        if ($_POST['fixedaddr'] != ""){
          $this->statements['fixed-address']= get_post('fixedaddr');
        } else {
          unset ($this->statements['fixed-address']);
        }
      }
      $this->options['host-name']= $this->cn;
    }

    dhcpPlugin::save_object();
  }


  /* Check values */
  function check()
  {
    $message= array();

    $cache = array();
    if(isset($this->parent)){
      $cache = $this->parent->dhcpObjectCache;
    }
  
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
    dhcpPlugin::save();
    if ($this->dhcpHWAddress != ""){
      $this->attrs['dhcpHWAddress']= array($this->dhcpHWAddress);
    } else {
      $this->attrs['dhcpHWAddress']= array();
    }

    return ($this->attrs);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
