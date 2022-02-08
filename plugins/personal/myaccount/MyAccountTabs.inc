<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: tabs_user.inc 14136 2009-08-26 08:02:49Z hickert $$
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

class MyAccountTabs extends tabs
{
  var $password_change_needed = FALSE;

  function remove_lock()
  {
    echo "Implement remove Lock !!!!!";;
  }

  function save($ignore_account= FALSE)
  {
      /* Check for new 'dn', in order to propagate the
         'dn' to all plugins */
      $baseobject= $this->by_object['user'];
      $baseobject->update_new_dn();
      if ($this->dn != 'new'){

          $new_dn= $baseobject->new_dn;

          if ($this->dn != $new_dn){

              /* Udpate acls */
              $baseobject->update_acls($this->dn,$new_dn);
              $baseobject->move($this->dn, $new_dn);
              $this->by_object['user']= $baseobject;

              /* Did we change ourselves? Update ui object. */
              change_ui_dn($this->dn, $new_dn);
          }
      }

      $this->dn= $baseobject->new_dn;

      return tabs::save();
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
