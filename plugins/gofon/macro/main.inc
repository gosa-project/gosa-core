<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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

if ($remove_lock){
  if(session::is_set('macroManagement')){
    $macroManagment = session::get('macroManagment');
    $macroManagment->remove_lock();
    del_lock ($ui->dn);
    sess_del ('macroManagment');
  }
} else {

  /* Create macroManagment object on demand */
  if (!session::is_set('macroManagment') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    session::set('macroManagment',new goFonMacro ($config, session::get('ui')));
  }

  /* Get object */
  $macroManagment = session::get('macroManagment');
  $macroManagment->save_object();
  $output= $macroManagment->execute();

  /* Page header*/
  if (session::is_set('objectinfo')){
    $display= print_header( get_template_path('images/macros.png'), 
                            _("Phone macro management"), 
                            "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> 
                            ".@LDAP::fix(session::get('objectinfo')));
  } else {
    $display= print_header(get_template_path('images/macros.png'), _("Phone macro management"));
  }

  /* Reset requested? */
  if (isset($_GET['reset']) && $_GET['reset'] == 1){
    del_lock ($ui->dn);
    sess_del ('macroManagment');
  }

  /* Show and save dialog */
  $display.= $output;
  session::set('macroManagment',$macroManagment);
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
