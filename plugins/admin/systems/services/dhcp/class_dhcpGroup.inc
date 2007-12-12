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

class dhcpGroup extends dhcpPlugin
{
  /* attribute list for save action */
  var $objectclasses= array('top', 'dhcpGroup', 'dhcpOptions');

  function dhcpGroup(&$attrs)
  {
    /* Load statements / options */
    dhcpPlugin::dhcpPlugin($attrs);
  }

  function execute()
  {
    $smarty= get_smarty();
    $smarty->assign("cn", $this->cn);

    /* Show main page */
    $display= $smarty->fetch (get_template_path('dhcp_group.tpl', TRUE)).$this->network->execute();

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
    /* Save cn */
    if (isset($_POST['cn'])){
      $this->cn= validate(get_post('cn'));
    }

    /* Handle global saving */
    dhcpPlugin::save_object();
  }


  /* Check values */
  function check()
  {
    $message= array();

    $cache = $this->parent->dhcpObjectCache;

    /* All required fields are set? */
    if ($this->cn == ""){
      $message[]= _("Required field 'Name' is not filled.");
    }
    if (!preg_match('/^[a-z0-9_-]*$/i', $this->cn)){
      $message[]= _("Field 'Name' contains illegal characters.");
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
    return ($this->attrs);
  }


}

?>
