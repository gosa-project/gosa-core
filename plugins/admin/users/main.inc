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

if ($remove_lock){
  if(session::is_set('userManagement')){
    $userManagement = session::get('userManagement');
    $userManagement->remove_lock();
    session::un_set ('userManagement');
  }
} else {
  /* Create usermanagement object on demand */
  if (!session::is_set('userManagement') ||
     (isset($_GET['reset']) && $_GET['reset'] == 1)){
    $userManagement= new userManagement ($config, $ui);
    $userManagement->set_acl_category("users");
    session::set('userManagement',$userManagement);
  }
  $userManagement = session::get('userManagement');
  $userManagement->save_object();
  $output= $userManagement->execute();

  /* Page header*/
  if (session::is_set('objectinfo')){
    $display= print_header(get_template_path($userManagement->plIcon),
                           _("User administration"),
                           "<img alt=\"\" class=\"center\" src=\"".
                           get_template_path('images/lists/locked.png').
                           "\"> ".@LDAP::fix(session::get('objectinfo')));
  } else {
    $display= print_header(get_template_path($userManagement->plIcon),
                          _("User administration"));
  }

  /* Generate dialog output */
  $display.= $output;

  /* Reset requested? */
  if (isset($_GET['reset']) && $_GET['reset'] == 1){
    del_lock ($ui->dn);
    session::un_set ('userManagement');
  }

  /* Show and save dialog */
  session::set('userManagement',$userManagement);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
