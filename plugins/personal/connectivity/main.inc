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

if (!$remove_lock){
  /* Reset requested? */
  if (isset($_POST['edit_cancel']) ||
      (isset($_GET['reset']) && $_GET['reset'] == 1)){

    del_lock ($ui->dn);
    sess_del ('edit');
    sess_del ('connectivity');
  }

  /* Create connectivity object on demand */
  if (!session::is_set('connectivity') || (isset($_GET['reset']) &&
      $_GET['reset'] == 1)){
    $_SESSION['connectivity']= new connectivity ($config, $ui->dn);
    $_SESSION['connectivity']->enable_CSN_check();
    $_SESSION['connectivity']->set_acl_base($ui->dn);
    $_SESSION['connectivity']->set_acl_category("users");

    /* Adjust acl's to mode */
    foreach ($_SESSION['connectivity']->plugin_name as $name){
      $_SESSION['connectivity']->plugin[$name]->set_acl_category( "users");
      $_SESSION['connectivity']->plugin[$name]->set_acl_base($ui->dn);
    }
  }
  $connectivity= $_SESSION['connectivity'];

  /* save changes back to object */
  if (isset ($_SESSION['edit'])){
    $connectivity->save_object ();
  }

  /* Enter edit mode? */
  if (isset($_POST['edit'])){

    /* Check locking */
    if ((($username= get_lock($ui->dn)) != "") && (!session::is_set('edit'))){
      session::set('back_plugin',$plug);
      gen_locked_message ($username, $ui->dn);
      exit ();
    }


    /* Lock the current entry */
    add_lock ($ui->dn, $ui->dn);
    session::set('dn',$ui->dn);
    session::set('edit',TRUE);
  }

  /* save changes to LDAP and disable edit mode */
  if (isset($_POST['edit_finish'])){

    /* Perform checks */
    $message= $connectivity->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $connectivity->save ();
      del_lock ($ui->dn);
      sess_del ('edit');

      /* Remove from session */
      sess_del ('connectivity');
    } else {
      /* Errors found, show message */
      show_errors ($message);
    }
  }

  /* Execute formular */
  $display= $connectivity->execute ();

  /* Store changes  in session */
  if (isset ($_SESSION['edit'])){
    session::set('connectivity',$connectivity);
  }

  $info= "";

  $display.="<div align='right'>";
  if (session::is_set('edit')){
    $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
    $display.= "&nbsp;";
    $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
    $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png').
      "\"> ".$ui->dn."&nbsp;";
  } else {
    $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/openlock.png').
      "\"> ".$ui->dn."&nbsp;";

    $info.= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/lamp.png').
      "\">"._("Click the 'Edit' button below to change informations in this dialog");
    $display.= "<input type=submit name=\"edit\" value=\""._("Edit")."\">\n";
    $display.= "<input type=\"hidden\" name=\"ignore\">\n";
  }
  $display.="</div>";

  /* Page header*/
  $display= print_header(get_template_path('images/proxy.png'),
      _("Connectivity"), $info).$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
