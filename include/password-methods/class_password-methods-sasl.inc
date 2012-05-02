<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_password-methods-ssha.inc 15380 2010-01-27 14:03:19Z cajus $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class passwordMethodsasl extends passwordMethod
{
  var $dn  = "";
  var $uid = "";
  var $realm = "";
  
  function __construct($config, $dn)
  {
    if($dn == "" || $dn == "new") return;

    $this->realm = trim($config->get_cfg_value('core','SASLRealm'));
    if($this->realm == ""){
        trigger_error(msgPool::cmdnotfound("SASLRealm", _("SASL")));
    }


    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']);
    $ldap->cat($dn,array('uid'));
    if($ldap->count() == 1){
        $attrs = $ldap->fetch();
        $this->uid = $attrs['uid'][0];
    }else{
        trigger_error("Cannot change password, unknown users '".$dn."'");
    }
  }

  function is_available()
  {
      return(true);
  }

  function generate_hash($pwd)
  {
    return("{SASL}".$this->uid."@".$this->realm);
  }


  function get_hash_name()
  {
    return "sasl";
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
