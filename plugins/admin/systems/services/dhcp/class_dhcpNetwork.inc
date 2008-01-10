<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2004-2007  Cajus Pollmeier

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

class dhcpNetwork extends plugin
{
  /* Used attributes */
  var $options= array();
  var $statements= array();

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function dhcpNetwork()
  {
    /* This is always an account */
    $this->is_account= TRUE;
  }

  function execute()
  {
    /* Check for iteraction */
    if (isset($_POST['add_dns']) && $_POST['addserver'] != ""){
      if (!preg_match('/^[0-9a-z.-]+$/', get_post('addserver'))){
        print_red(_("The name of the DNS server your're going to add is not valid!"));
      } else {
        $servers= array();
        if (isset($this->options['domain-name-servers'])){
          foreach(split(",", $this->options['domain-name-servers']) as $val){
            $servers[$val]= $val;
          }
        }
        $servers[get_post('addserver')]= get_post('addserver');

        $tmp= "";
        foreach($servers as $val){
          $tmp.= $val.",";
        }
        $this->options['domain-name-servers']= preg_replace('/,$/', '', $tmp);
      }
    }
    if (isset($_POST['delete_dns']) && isset($_POST['dnsserver'])){
      $tmp= preg_replace("/(\s*,\s*)?".get_post('dnsserver')."/i", '',
          $this->options['domain-name-servers']);
      $tmp= preg_replace("/(\s*)?,(\s*)?$/", '', $tmp);
      if ($tmp != ""){
        $this->options['domain-name-servers']= $tmp;
      } else {
        unset($this->options['domain-name-servers']);
      }
    }

    /* Show main page */
    $smarty= get_smarty();

    /*
     * Assemble options
     */

    /* Router */
    if (isset($this->options['routers'])){
      $smarty->assign("routers", $this->options['routers']);
    } else {
      $smarty->assign("routers", "");
    }

    /* DNS */
    if (isset($this->options['domain-name'])){
      $smarty->assign("domain", trim($this->options['domain-name'], '"'));
    } else {
      $smarty->assign("domain", "");
    }
    if (isset($this->options['domain-name-servers'])){
      $servers= array();
      foreach(split(",", $this->options['domain-name-servers']) as $val){
        $servers[$val]= $val;
      }
      $smarty->assign("dnsservers", $servers);
    } else {
      $smarty->assign("dnsservers", "");
    }

    /* Netmask / Broadcast */
    if (isset($this->options['subnet-mask'])){
      $this->options['subnet-mask']= normalize_netmask($this->options['subnet-mask']);
      $smarty->assign("subnet_mask", $this->options['subnet-mask']);
    } else {
      $smarty->assign("subnet_mask", "");
    }
    if (isset($this->options['broadcast-address'])){
      $smarty->assign("broadcast_address", $this->options['broadcast-address']);
    } else {
      $smarty->assign("broadcast_address", "");
    }

    /* Boot stuff */
    if (isset($this->statements['filename'])){
      $smarty->assign("filename", trim($this->statements['filename'], '"'));
    } else {
      $smarty->assign("filename", "");
    }
    if (isset($this->statements['next-server'])){
      $smarty->assign("nextserver", $this->statements['next-server']);
    } else {
      $smarty->assign("nextserver", "");
    }

    /* Set flags */
    $smarty->assign("autohost", "");
    if (isset($this->statements['get-lease-hostnames'])){
      if (preg_match('/^(true|on|yes)$/', $this->statements['get-lease-hostnames'])){
        $smarty->assign("autohost", "checked");
      }
    }
    $smarty->assign("autohostdecl", "");
    if (isset($this->statements['use-host-decl-names'])){
      if (preg_match('/^(true|on|yes)$/', $this->statements['use-host-decl-names'])){
        $smarty->assign("autohostdecl", "checked");
      }
    }

    return $smarty->fetch(get_template_path('dhcp_network.tpl', TRUE,dirname(__FILE__)));
  }

  function remove_from_parent()
  {
  }


  /* Save data to object */
  function save_object()
  {
    /* Only save, if we are "active" */
    if (isset($_POST['routers'])){

      /*
       * Assemble options
       */

      /* Options */
      foreach (array("routers" => "routers", "domain-name" => "domain", "subnet-mask" => "subnet_mask",
                     "broadcast-address" => "broadcast_address") as $key => $val){

        if ($_POST["$val"] == ''){
          unset($this->options["$key"]);
        } else {
          $this->options["$key"]= get_post("$val");
        }
      }

      /* Statements */
      foreach (array("filename" => "filename", "next-server" => "nextserver") as $key => $val){
        if ($_POST["$val"] == ''){
          unset($this->statements["$key"]);
        } else {
    
          /* Only quote filename values */
          if(in_array($key,array("filename"))){
            $this->statements["$key"]= '"'.get_post("$val").'"';
          }else{
            $this->statements["$key"]= get_post("$val");
          }
        }
      }

      /* Flags */
      if (isset ($_POST['autohost'])){
        $this->statements['get-lease-hostnames']= "true";
      } else {
        unset($this->statements['get-lease-hostnames']);
      }
      if (isset ($_POST['autohostdecl'])){
        $this->statements['use-host-decl-names']= "on";
      } else {
        unset($this->statements['use-host-decl-names']);
      }
    }
  }


  /* Check values */
  function check()
  {
    $message= array();

    /* Check netmask and broadcast */
    foreach(array("subnet-mask" => _("Netmask"), "broadcast-address" => _("Broadcast")) as $key => $typ){
      if (!isset($this->options["$key"])){
        continue;
      }
      $tmp= preg_replace('/^[^\s]+\s/', '', $this->options["$key"]);

      if (!tests::is_ip($tmp)){
        $message[]= sprintf(_("Error in definition of '%s'!"), $typ);
      }
    }

    return $message;
  }


  /* Save to LDAP */
  function save()
  {
  }
  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
