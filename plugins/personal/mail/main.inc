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

$lock_msg ="";
$display  =""; 
if (!$remove_lock){
  /* Reset requested? */
  if (isset($_POST['edit_cancel']) || 
      (isset($_GET['reset']) && $_GET['reset'] == 1)){
    del_lock ($ui->dn);
    sess_del ('edit');
    sess_del ('mailAccount');
  }

  /* Create mail object on demand */
  if (!isset($_SESSION['mailAccount']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    $_SESSION['mailAccount']= new mailAccount ($config, $ui->dn);
    $_SESSION['mailAccount']->enable_CSN_check();
    $_SESSION['mailAccount']->set_acl_base($ui->dn);
    $_SESSION['mailAccount']->set_acl_category("users");
  }
  $mailAccount= $_SESSION['mailAccount'];

  /* save changes back to object */
  if (isset ($_SESSION['edit'])){
    $mailAccount->save_object ();
  }

  /* Enter edit mode? */
  if (isset($_POST['edit']) && preg_match("/w/",$ui->get_permissions($ui->dn,"users/mailAccount"))){

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
    $message= $mailAccount->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $mailAccount->save ();
      del_lock ($ui->dn);
      sess_del ('edit');

      /* Remove object */
      sess_del ('mailAccount');
    } else {
      /* Errors found, show message */
      show_errors ($message);
    }
  }

  /* Execute formular */
  if($lock_msg){
    $display.= $lock_msg;
  }else{
    $display.= $mailAccount->execute ();
  }

  /* Store changes  in session */
  if (isset ($_SESSION['edit'])){
    session::set('mailAccount',$mailAccount);
  }

  $info= "";
  /* Show page footer depending on the mode */
  if (!$mailAccount->forward_dialog && $mailAccount->is_account && empty($lock_msg)){
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
      
        $info.= "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/lamp.png').
            "\"> "._("Click the 'Edit' button below to change informations in this dialog");

        if(preg_match("/w/",$ui->get_permissions($ui->dn,"users/mailAccount"))){
          $display.= "<input type=submit name=\"edit\" value=\""._("Edit")."\">\n";
        }

      $display.= "<input type=\"hidden\" name=\"ignore\">\n";
    }
    $display.= "</p>\n";
  }

  /* Page header*/
  $display= print_header(get_template_path('images/email.png'), _("User mail settings"), $info).$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
