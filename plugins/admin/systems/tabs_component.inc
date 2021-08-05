<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: tabs_component.inc 19274 2010-07-29 13:55:08Z hickert $$
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

class componenttabs extends tabs
{

  function __construct($config, $data, $dn,$category,$hide_refs = FALSE, $hide_acls = FALSE)
  {
    tabs::__construct($config, $data, $dn,$category,$hide_refs, $hide_acls);

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function save_object($save_current= FALSE)
  {
    tabs::save_object($save_current);

    $baseobject = $this->by_object['componentGeneric'];
    $baseobject->netConfigDNS->cn= $baseobject->cn;
  }


  function save($ignore_account= FALSE)
  {
    /* Check for new 'dn', in order to propagate the
       'dn' to all plugins */
    $baseobject= $this->by_object['componentGeneric'];
    $cn      = preg_replace('/,/', '\,', $baseobject->cn);
    $cn      = preg_replace('/"/', '\"', $cn);
    $this->dn= "cn={$cn},".get_ou("componentGeneric", "componentRDN").$baseobject->base;

    if($this->dn != $baseobject->orig_dn && $baseobject->orig_dn != "new"){

      /* Udpate acls */
      $baseobject->update_acls($baseobject->orig_dn,$this->dn);
      $baseobject->move($baseobject->orig_dn,$this->dn);
    }

    $baseobject->dn= $this->dn;

    foreach ($this->by_object as $key => $obj){
      $this->by_object[$key]->dn= $this->dn;
    }

    tabs::save(TRUE);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
