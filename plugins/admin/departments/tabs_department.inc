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

class deptabs extends tabs
{
  var $base= "";

  function deptabs($config, $data, $dn,$category)
  {
    tabs::tabs($config, $data, $dn,$category);
    $this->base= $this->by_object['department']->base;

    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
  }

  function check($ignore_account= FALSE)
  {
    return (tabs::check(TRUE));
  }


  function save($ignore_account= FALSE)
  {
    $baseobject= $this->by_object['department'];
    $ou= preg_replace('/,/', '\,', $baseobject->ou);
    $new_dn= @LDAP::convert('ou='.$ou.','.$baseobject->base);
    
    if($ignore_account){
      if ($this->dn != $new_dn && $this->dn != "new"){
        /* if( new_dn is subtree of this->dn ) */
        $cnt1 = count(split(",",$this->dn));
        $cnt2 = count(split(",",$new_dn));
        if((strstr($new_dn,$this->dn))&&($cnt1<$cnt2)){
          msg_dialog::display(_("LDAP error"), _("Moving LDAP tree failed: destination tree is a subtree of the source!"), ERROR_DIALOG);
        }else{
          /* Prepare to be moved */
          $baseobject->recursive_move($this->dn, $new_dn);			
          $this->by_object['department'] = $baseobject;
          return;
        }
      }
    }
    $this->dn= $new_dn;
    $baseobject->dn= $this->dn;
    $this->by_object['department']= $baseobject;
    return(tabs::save(TRUE));
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
