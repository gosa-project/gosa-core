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

class dhcpSharedNetwork extends dhcpPlugin
{
  /* attribute list for save action */
  var $objectclasses= array('top', 'dhcpSharedNetwork');

  function dhcpSharedNetwork($attrs)
  {
    dhcpPlugin::dhcpPlugin($attrs);

    $this->advanced->setAutoStatements(array("server-identifier", "default-lease-time",
          "max-lease-time", "min-lease-time", "authoritative", "deny-unknown-clients",
          "deny-bootp", "deny-booting"));
  }


  function execute()
  {
    $smarty= get_smarty();

    /* Fill template */
    $smarty->assign ("cn", $this->cn);
    foreach (array("server-identifier", "default-lease-time", "max-lease-time", "min-lease-time") as $attr){
      if (isset($this->statements[$attr])){
        $smarty->assign(preg_replace('/-/', '_', $attr), $this->statements[$attr]);
      } else {
        $smarty->assign(preg_replace('/-/', '_', $attr), "");
      }
    }
    if (isset($this->statements["authoritative"])){
      $smarty->assign("authoritative", "checked");
    } else {
      $smarty->assign("authoritative", "");
    }
    if (!isset($this->statements["deny unknown-clients"])){
      $smarty->assign("allow_unknown_state", "checked");
    } else {
      $smarty->assign("allow_unknown_state", "");
    }
    if (!isset($this->statements["deny bootp"])){
      $smarty->assign("allow_bootp_state", "checked");
    } else {
      $smarty->assign("allow_bootp_state", "");
    }
    if (!isset($this->statements["deny booting"])){
      $smarty->assign("allow_booting_state", "checked");
    } else {
      $smarty->assign("allow_booting_state", "");
    }

    /* Show main page */
    $display= $smarty->fetch(get_template_path('dhcp_sharedNetwork.tpl', TRUE)).$this->network->execute();

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
    if (isset($_POST['cn'])){
      $this->cn= validate(get_post('cn'));
      dhcpPlugin::save_object();

      foreach (array("server-identifier", "default-lease-time",
            "max-lease-time", "min-lease-time") as $attr){
        if (isset($_POST[$attr]) && $_POST[$attr] != ""){
          $this->statements[$attr]= get_post($attr);
        } else {
          unset($this->statements[$attr]);
        }
      }

      if (isset($_POST["authoritative"])){
        $this->statements["authoritative"]= "";
      } else {
        unset ($this->statements["authoritative"]);
      }

      foreach(array("unknown-clients", "bootp", "booting") as $name){
        if (isset($_POST[$name])){
          $this->statements["allow $name"]= "";
          unset($this->statements["deny $name"]);
        } else {
          $this->statements["deny $name"]= "";
          unset($this->statements["allow $name"]);
        }
      }
    }
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

    /* Check lease times */
    foreach (array("default-lease-time" => _("Default lease time"),
          "max-lease-time" => _("Max. lease time"),
          "min-lease-time" => _("Min. lease time")) as $key => $val){
      if (isset($this->statements[$key]) && $this->statements[$key] != "" && 
          !tests::is_id($this->statements[$key])){
        $message[]= sprintf(_("The value specified as '%s' is not numeric!"), $val);
      }
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
