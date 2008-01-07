<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2005  Benoit Mortier

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

$display    = "";
$lock_msg   = "";

if (!$remove_lock){
  /* Reset requested? */
  if (isset($_POST['edit_cancel']) || 
      (isset($_GET['reset']) && $_GET['reset'] == 1)){
    del_lock ($ui->dn);
    session::un_set ('edit');
    session::un_set ('nagiosAccount');
  }

  /* Create mail object on demand */
  if (!session::is_set('nagiosAccount') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    $nagiosAccount= new nagiosAccount ($config, $ui->dn);
    $nagiosAccount->enable_CSN_check();
    $nagiosAccount->set_acl_base($ui->dn);
    $nagiosAccount->set_acl_category("users");
    session::set('nagiosAccount',$nagiosAccount);
  }
  $nagiosAccount = session::get('nagiosAccount');

  /* save changes back to object */
  if (session::is_set('edit')){
    $nagiosAccount->save_object ();
  }

  /* Enter edit mode? */
  if (isset($_POST['edit'])){

    /* Check locking */
    if (($username= get_lock($ui->dn)) != ""){
      session::set('back_plugin',$plug);
      session::set('LOCK_VARS_TO_USE',array("/^edit$/","/^plug$/"));
      $lock_msg =  gen_locked_message ($username, $ui->dn);

    }else{

      /* Lock the current entry */
      add_lock ($ui->dn, $ui->dn);
      session::set('dn',$ui->dn);
      session::set('edit',TRUE);
    }
  }

  /* save changes to LDAP and disable edit mode */
  if (isset($_POST['edit_finish'])){

    /* Perform checks */
    $message= $nagiosAccount->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $nagiosAccount->save ();
      del_lock ($ui->dn);
      session::un_set ('edit');

      /* Remove object */
      session::un_set ('nagiosAccount');
    } else {
      /* Errors found, show message */
      show_errors ($message);
    }
  }

  /* Execute formular */
  if($lock_msg){
    $display.= $lock_msg;
  }else{
    $display.= $nagiosAccount->execute ();
  }

  /* Store changes  in session */
  if (session::is_set('edit')){
    session::set('nagiosAccount',$nagiosAccount);
  }

  $info= "";
  /* Show page footer depending on the mode */
  if ($nagiosAccount->is_account && empty($lock_msg)){
    $display.= "<p class=\"plugbottom\">";

    /* Are we in edit mode? */
    if (session::is_set('edit')){
      $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
      $display.= "&nbsp;";
      $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
      $info= "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png').
             "\"> ".$ui->dn."&nbsp;";
    } else {
      $info= "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/openlock.png').
             "\"> ".$ui->dn."&nbsp;";

      if(preg_match("/w/",$ui->get_permissions($ui->dn,"users/nagiosAccount"))){
        $info.= "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/lamp.png').
          "\"> "._("Click the 'Edit' button below to change informations in this dialog");
        $display.= "<input type=submit name=\"edit\" value=\""._("Edit")."\">\n";
      }
      $display.= "<input type=\"hidden\" name=\"ignore\">\n";
    }
    $display.= "</p>\n";
  }

  /* Page header*/
  $display= print_header(get_template_path('images/monitoring.png'), _("Nagios settings"), $info).$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
