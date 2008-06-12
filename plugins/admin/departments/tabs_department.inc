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
  var $moved = FALSE;
  var $base_name = "department";

  function deptabs($config, $data, $dn,$category)
  {
    tabs::tabs($config, $data, $dn,$category);

    /* Detect the base class  (The classs which extends from department)
     */
    $class = "department";
    foreach($this->by_object as $name => $object){
      if($object instanceOf department){
        $this->base_name = get_class($object);
        break;
      }
    }


    /* Add references/acls/snapshots */
    $this->addSpecialTabs();
    $this->by_object['acl']->skipTagging= TRUE;;
  }

  function check($ignore_account= FALSE)
  {
    return (tabs::check(TRUE));
  }


  /*! \brief Check if the department must must be moved 
      return Boolean TRUE if we have to move the dep, else FALSE
   */
  function am_i_moved()
  {
    return(FALSE);
    if($this->moved) return(FALSE);
    $baseobject= &$this->by_object[$this->base_name];

    $namingAttr       = $baseobject->namingAttr;
    $orig_namingAttr  = $baseobject->orig_namingAttr;

    $value       = preg_replace('/,/', '\,', $baseobject->$namingAttr);  
    $orig_value  = preg_replace('/,/', '\,', $baseobject->$orig_namingAttr);  

    $new_dn   = @LDAP::convert($namingAttr.'='.$value.','.$baseobject->base);
    $old_dn   = @LDAP::convert($namingAttr.'='.$orig_value.','.$baseobject->orig_base);

    print_a(array($new_dn,$old_dn));

    if ($this->dn != $new_dn && $this->dn != "new"){
      return(TRUE); 
    }
    return(FALSE);
  }

 
  /*! \brief  Checks if the department was moved successfully 
      @return Boolean TRUE if the dep was already moved.
   */ 
  function move_done()
  {
    return($this->moved);
  }

  
  /*! \brief  Initiate recursive move 
   */
  function move_me()
  {
    if(!$this->am_i_moved()) return;
    $baseobject= &$this->by_object[$this->base_name];
    $ou       = preg_replace('/,/', '\,', $baseobject->ou);  
    $new_dn   = @LDAP::convert('ou='.$ou.','.$baseobject->base);
    $old_ou   = preg_replace('/,/', '\,', $baseobject->orig_ou);
    $old_dn   = @LDAP::convert('ou='.$ou.','.$baseobject->orig_base);
    if ($this->dn != $new_dn && $this->dn != "new"){
      $baseobject->recursive_move($this->dn, $new_dn);
      $this->moved= TRUE;
    }
  }


  function save($ignore_account= FALSE)
  {
    $baseobject= &$this->by_object[$this->base_name];

    $namingAttr       = $baseobject->namingAttr;

    $nAV      = preg_replace('/,/', '\,', $baseobject->$namingAttr); 
    $new_dn   = @LDAP::convert($namingAttr.'='.$nAV.','.$baseobject->base);

    /* Move group? */
    if ($this->dn != $new_dn && $this->dn != "new"){
        $baseobject->move($this->dn,$new_dn);
    } 

    $this->dn= $new_dn;
    $baseobject->dn= $this->dn;
    if (!$ignore_account){
      tabs::save(TRUE);
    }
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
