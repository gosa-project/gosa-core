<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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


class SnapshotHandler {

  var $config;
  var $isEnabled= false;
  var $snapshotBases= array();


  /* Create handler  */
  function SnapshotHandler(&$config)
  {
    $this->config = &$config;	
    $config = $this->config;

    if($config->get_cfg_value("enableSnapshots") == "true"){

      /* Check if the snapshot_base is defined */
      if ($config->get_cfg_value("snapshotBase") == ""){

        /* Send message if not done already */
        if(!session::is_set("snapshotFailMessageSend")){
          session::set("snapshotFailMessageSend",TRUE);
          msg_dialog::display(_("Configuration error"),
              sprintf(_("The snapshot functionality is enabled, but the required variable '%s' is not set."),
                "snapshotBase"), ERROR_DIALOG);
        }
        return;
      }

      /* Check if the snapshot_base is defined */
      if (!is_callable("gzcompress")){

        /* Send message if not done already */
        if(!session::is_set("snapshotFailMessageSend")){
          session::set("snapshotFailMessageSend",TRUE);
          msg_dialog::display(_("Configuration error"),
              sprintf(_("The snapshot functionality is enabled, but the required compression module is missing. Please install '%s'."),"php5-zip / php5-gzip"), ERROR_DIALOG);
        }
        return;
      }

      /* check if there are special server configurations for snapshots */
      if ($config->get_cfg_value("snapshotURI") != ""){

        /* check if all required vars are available to create a new ldap connection */
        $missing = "";
        foreach(array("snapshotURI","snapshotAdminDn","snapshotAdminPassword","snapshotBase") as $var){
          if($config->get_cfg_value($var) == ""){
            $missing .= $var." ";

            /* Send message if not done already */
            if(!session::is_set("snapshotFailMessageSend")){
              session::set("snapshotFailMessageSend",TRUE);
              msg_dialog::display(_("Configuration error"),
                  sprintf(_("The snapshot functionality is enabled, but the required variable '%s' is not set."),
                    $missing), ERROR_DIALOG);
            }
            return;
          }
        }
      }
      $this->isEnabled= true;
      return;
    }
  }


  function enabled()
  {
    return $this->isEnabled;
  }   


  function setSnapshotBases($bases)
  {
    $this->snapshotBases= $bases;
  }


  function getSnapshotBases()
  {
    return $this->snapshotBases;
  }


  function get_snapshot_link()
  {
    $snapshotLdap= null;

    /* check if there are special server configurations for snapshots */
    if($this->config->get_cfg_value("snapshotURI") != ""){
      $server= $this->config->get_cfg_value("snapshotURI");
      $user= $this->config->get_cfg_value("snapshotAdminDn");
      $password= $this->config->get_credentials($this->config->get_cfg_value("snapshotAdminPassword"));
      $snapshotLdap= new ldapMultiplexer(new LDAP($user,$password, $server));
    }

    /* Prepare bases */
    $this->snapshotLdapBase= $this->config->get_cfg_value("snapshotBase");
    $snapshotLdap->cd($this->snapshotLdapBase);
    if (!$snapshotLdap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($snapshotLdap->get_error(), $this->snapshotLdapBase, "", get_class()));
    }

    return $snapshotLdap;
  }


  function getDeletedSnapshots($objectBase, $raw= false)
  {
    // Skip if not enabled
    if(!$this->enabled()){
      return(array());
    }

    // Load user info
    $ui= get_userinfo();

    /* Create an additional ldap object which
       points to our ldap snapshot server */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $snapshotLdap= $this->get_snapshot_link();
    if (!$snapshotLdap) {
      $snapshotLdap= $ldap;
    }

    // Initialize base
    $base= preg_replace("/".preg_quote($this->config->current['BASE'], '/')."$/",
                        "", $objectBase).$this->snapshotLdapBase;

    /* Fetch all objects and check if they do not exist anymore */
    $objects= array();
    $snapshotLdap->cd($base);
    $snapshotLdap->ls("(objectClass=gosaSnapshotObject)", $base,
                      array("gosaSnapshotType", "gosaSnapshotTimestamp", "gosaSnapshotDN", "description"));
    while($entry = $snapshotLdap->fetch()){

      $chk =  str_replace($base,"",$entry['dn']);
      if(preg_match("/,ou=/",$chk)) continue;

      if(!isset($entry['description'][0])){
        $entry['description'][0]  = "";
      }
      $objects[] = $entry;
    }

    /* Check if entry still exists */
    foreach($objects as $key => $entry){
      $ldap->cat($entry['gosaSnapshotDN'][0]);
      if($ldap->count()){
        unset($objects[$key]);
      }
    }

    /* Format result as requested */
    if($raw) {
      return($objects);
    }else{
      $tmp = array();
      foreach($objects as $key => $entry){
        $tmp[base64_encode($entry['dn'])] = $entry['description'][0];
      }
    }
    return($tmp);
  }


  function hasSnapshots($dn)
  {
    return (count($this->getSnapshots($dn)) > 0);
  }


  function getSnapshots($dn, $raw= false)
  {
    // Empty if disabled
    if(!$this->enabled()){
      return(array());
    }

    /* Create an additional ldap object which
       points to our ldap snapshot server */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    // Load snapshot LDAP connection
    $snapshotLdap= $this->get_snapshot_link();
    if (!$snapshotLdap) {
      $snapshotLdap= $ldap;
    }

    // Initialize base
    $base= preg_replace("/".preg_quote($this->config->current['BASE'], '/')."$/",
                        "", $objectBase).$this->snapshotLdapBase;

    /* Fetch all objects with  gosaSnapshotDN=$dn */
    $snapshotLdap->cd($base);
    $snapshotLdap->ls("(&(objectClass=gosaSnapshotObject)(gosaSnapshotDN=".$dn."))",$base,
        array("gosaSnapshotType","gosaSnapshotTimestamp","gosaSnapshotDN","description"));

    /* Put results into a list and add description if missing */
    $objects= array();
    while($entry = $snapshotLdap->fetch()){
      if(!isset($entry['description'][0])){
        $entry['description'][0]  = "";
      }
      $objects[] = $entry;
    }

    /* Return the raw array, or format the result */
    if($raw){
      return($objects);
    }else{
      $tmp = array();
      foreach($objects as $entry){
        $tmp[base64_encode($entry['dn'])] = $entry['description'][0];
      }
    }
    return($tmp);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
