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

/* Clear display */
$display= "";

if (!$remove_lock){

  /* Reset requested? */
  if (isset($_POST['edit_cancel']) ||
      (isset($_GET['reset']) && $_GET['reset'] == 1)){

    del_lock ($ui->dn);
    sess_del ('edit');
    sess_del ('password');
  }

  /* Create password object on demand */
  if (!session::is_set('password') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    session::set('password', new password ($config, $ui->dn));
  }
  $password = session::get('password');

  /* Execute formular */
  $display.= $password->execute ();

  /* Page header*/
  $display= print_header(get_template_path('images/password.png'),
                         _("Password settings"), "").$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
