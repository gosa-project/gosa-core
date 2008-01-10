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

class dhcpSubnet extends dhcpPlugin
{
  /* Used attributes */
  var $dhcpNetMask= 24;
  var $dhcpRange= "";
  var $range_start= "";
  var $range_stop= "";
  var $use_range= FALSE;

  /* attribute list for save action */
  var $objectclasses= array('top', 'dhcpSubnet', 'dhcpOptions');

  function dhcpSubnet($attrs)
  {
    dhcpPlugin::dhcpPlugin($attrs);

    if (!$this->new){
      /* Load attributes */
      foreach (array("dhcpNetMask", "dhcpRange") as $attr){
        if (isset($attrs[$attr][0])){
          $this->$attr= $attrs[$attr][0];
        }
      }
      if (isset($attrs['dhcpRange']) && count($attrs['dhcpRange'])){
        $this->use_range= TRUE;
        list($this->range_start, $this->range_stop)= preg_split('/\s+/', $this->dhcpRange);
      }
    }

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

    /* Show main page */
    return $display;
  }


  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['dhcp_subnet_posted'])){
      if (isset($_POST['cn'])){
        $this->cn= validate(get_post('cn'));
      } 
      if (isset($_POST['dhcp_netmask'])){
        $this->dhcpNetMask= validate(get_post('dhcp_netmask'));
      } 
      if (isset($_POST['use_range'])){
        $this->use_range= TRUE;
        $this->range_start= validate(get_post('range_start'));
        $this->range_stop= validate(get_post('range_stop'));
      } else {
        $this->use_range= FALSE;
      }

      /* Move range to internal variable */
      $this->dhcpRange= $this->range_start." ".$this->range_stop;
      dhcpPlugin::save_object();
    }
  }


  /* Check values */
  function check()
  {
    $message= array();

    $cache = $this->parent->dhcpObjectCache;

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
      if ($this->$attr != "" && !tests::is_ip($this->$attr)){
        $message[]= sprintf(_("The field '%s' contains an invalid IP address"), $str);
      }
    }

    /* Check ip range */
    if ($this->use_range){
      if(!tests::is_ip_range($this->range_start,$this->range_stop)){
        $message[] = _("Field 'Range' contains invalid IP range.");
      }

      /* Check if range is in the network */
      if (!tests::is_in_network($this->cn, $this->dhcpNetMask, $this->range_start) ||
          !tests::is_in_network($this->cn, $this->dhcpNetMask, $this->range_stop)){
        $message[] = _("'Range' is not inside the configured network.");
      }
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

    /* Move dn to the result */
    $this->attrs['dhcpNetMask']= array(netmask_to_bits($this->dhcpNetMask));
    if ($this->use_range && !empty($this->dhcpRange)){
      $this->attrs['dhcpRange']= array($this->range_start." ".$this->range_stop);
    } else {
      $this->attrs['dhcpRange']= array();
    }

    return ($this->attrs);
  }
  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
