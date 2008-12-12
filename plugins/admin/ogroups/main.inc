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
  if(session::is_set('ogroup')){
    $ogroup = session::get('ogroup');
    $ogroup->remove_lock();
  }
} else {

  /* Create object object on demand */
  if (!session::is_set('ogroup') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    session::set('ogroup',new ogroupManagement ($config));
  }
  $ogroup = session::get('ogroup');

  /* Execute formular */
  $ogroup->save_object();
  $output= $ogroup->execute ();

  /* Page header*/
  if (get_object_info() != ""){
    $display= print_header(get_template_path('plugins/ogroups/images/plugin.png'),
                          _("Object groups"),
                          "<img alt=\"\" align=\"middle\" src=\"".
                          get_template_path('images/lists/locked.png').
                          "\"> ".LDAP::fix(get_object_info()));
  } else {
    $display= print_header(get_template_path('plugins/ogroups/images/plugin.png'), _("Object groups"));
  }

  $display.= $output;

  /* Store changes  in session */
  session::set('ogroup',$ogroup);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
