<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003-2005  Cajus Pollmeier

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

class userinfo
{
  var $dn;
  var $ip;
  var $username;
  var $cn;
  var $gidNumber= -1;
  var $language= "";
  var $config;
  var $gosaUnitTag= "";
  var $subtreeACL= array();

  /* get acl's an put them into the userinfo object
     attr subtreeACL (userdn:components, userdn:component1#sub1#sub2,component2,...) */
  function userinfo($config, $userdn){
    $this->config= $config;
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($userdn,array('sn', 'givenName', 'uid', 'gidNumber', 'preferredLanguage', 'gosaUnitTag'));
    $attrs= $ldap->fetch();

    if (isset($attrs['givenName'][0]) && isset($attrs['sn'][0])){
      $this->cn= $attrs['givenName'][0]." ".$attrs['sn'][0];
    } else {
      $this->cn= $attrs['uid'][0];
    }
    if (isset($attrs['gidNumber'][0])){
      $this->gidNumber= $attrs['gidNumber'][0];
    }

    /* Assign user language */
    if (isset($attrs['preferredLanguage'][0])){
      $this->language= $attrs['preferredLanguage'][0];
    }

    if (isset($attrs['gosaUnitTag'][0])){
      $this->gosaUnitTag= $attrs['gosaUnitTag'][0];
    }

    $this->dn= $userdn;
    $this->ip= $_SERVER['REMOTE_ADDR'];
  }


  function loadACL()
  {
    $ldap= $this->config->get_ldap_link();

    /* Check if we should include the unittag */
    $tag= "";
    if ($this->gosaUnitTag != "" && isset($this->config->current['STRICT_UNITS']) &&
        preg_match('/TRUE/i', $this->config->current['STRICT_UNITS'])){
      $tag= "(gosaUnitTag=".$this->gosaUnitTag.")";
    }

    /* Load ACL's from all groups we're in */
    $this->subtreeACL= array();
    $ldap->cd($this->config->current['BASE']);
    if ($this->gidNumber == -1){
      $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaObject)".$tag.
          "(memberUid=$this->username))");
    } else {
      $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaObject)".$tag.
          "(|(memberUid=$this->username)(gidNumber=$this->gidNumber)))");
    }

    while($attrs = $ldap->fetch()){
      $base= preg_replace('/^[^,]*+,'.normalizePreg(get_groups_ou()).'/i', "",$ldap->getDN());
      $base= preg_replace("/\s*,\s*/", ",", $base);

      for ($i= 0; $i<$attrs["gosaSubtreeACL"]["count"]; $i++){
        $this->subtreeACL[$base][]= $attrs["gosaSubtreeACL"][$i];
      }
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
