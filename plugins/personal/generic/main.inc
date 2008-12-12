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

/* Remove locks created by this plugin
*/
$lock_msg="";
if ($remove_lock || isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
  if(session::is_set('user') && session::is_set('edit')){
    del_lock($ui->dn);
  }
}

/* Remove this plugin from session
*/
if ( $cleanup ){
  session::un_set('user');
  session::un_set('edit');

}else{

  /* Reset requested? */
  if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
    session::un_set ('edit');
    session::un_set ('user');
  }

  /* Create user object on demand */
  if (!session::is_set('user') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    $user= new user ($config, $ui->dn);
    $user->enable_CSN_check();
    $user->set_acl_base($ui->dn);
    $user->set_acl_category("users");
    session::set('user',$user);
  }
  $user = session::get('user');

  /* save changes back to object */
  if (session::is_set('edit')){
    $user->save_object ();
  }

  /* Enter edit mode? */
  if ((isset($_POST['edit'])) && (!session::is_set('edit')) &&preg_match("/w/",$ui->get_permissions($ui->dn,"users/user"))){

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

  /* Perform password change */
  if (isset($_POST['password_finish'])){

    /* For security reasons, check if user is allowed to set password again */
    if ($user->acl_is_writeable("userPassword") || $user->acl_is_createable()){

      /* Check input and feed errors into 'message' */
      $message= array();

      /* Sanity checks... */
      if ($_POST['new_password'] != $_POST['repeated_password']){

        /* Matching passwords in new and repeated? */
        $message[]= _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
      } else {

        /* Empty password is not permitted by default. */
        if ($_POST['new_password'] == ""){
          $message[]= msgPool::required(_("New password"));
        }
      }

      /* Errors, or change password? */
      if (count($message) != 0){

        /* Show error message and continue editing */
        msg_dialog::displayChecks($message);
        $display.= $smarty->fetch(get_template_path('password.tpl', TRUE));
        return ($display);
      }
      change_password ($user->dn, $_POST['new_password'], 0, $user->pw_storage);
      new log("modify","users/password","",array(),"Password has been changed");

    } else {

      /* Missing permissions, show message */
      msg_dialog::display(_("Error"), _("You have no permission to set your password!"), ERROR_DIALOG);
    }

    del_lock ($ui->dn);
    session::un_set ('user');
  }

  /* save changes to LDAP and disable edit mode */
  if (isset($_POST['edit_finish'])){

    /* Perform checks */
    $message= $user->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $user->save ();
      del_lock ($ui->dn);
      session::un_set ('edit');

      /* Need to reset the password? */
      if($user->password_change_needed()){
        /* Save object */
        session::set('user',$user);

        $display.= $smarty->fetch(get_template_path('password.tpl', TRUE));
        return ($display);
      } else {
        session::un_set ('user');
      }

    } else {
      /* Errors found, show message */
      msg_dialog::displayChecks($message);
    }
  }

  /* Execute formular */
  if($lock_msg){
    $display.= $lock_msg;
  }else{
    $display.= $user->execute ();
  }

  /* Store changes  in session */
  if (session::is_set('edit')){
    session::set('user',$user);
  }

  $info = "";

  /* Show page footer depending on the mode */
  if (!$user->cert_dialog && !$user->picture_dialog && !$user->dialog && $user->is_account && empty($lock_msg)){
    $display.= "<p class=\"plugbottom\">";

    /* Are we in edit mode? */
    if (session::is_set('edit')){
      $display.= "<input type=submit name=\"edit_finish\" style=\"width:80px\" value=\"".msgPool::okButton()."\">\n";
      $display.= "&nbsp;";
      $display.= "<input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";

      $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/lists/locked.png').
        "\"> ".$ui->dn."&nbsp;";
    } else {
      /* 'viewid' may be set by i.e. the phonelist plugin. This
         means that we want to go back... */
      if (isset ($viewid)){
        $str= msgPool::backButton();
        $fn = "back";
      } else {
        $str= msgPool::editButton();
        $fn = "edit";
        $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/lists/unlocked.png').
          "\"> ".$ui->dn."&nbsp;";
      }

      /* Only display edit button if there is at least one attribute editable */
      if(preg_match("/r/",$ui->get_permissions($ui->dn,"users/user"))){
        if ($fn == "edit"){
          $info.= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/lists/on.png')."\"> ".
            msgPool::clickEditToChange();
          $display.= "<input type=submit name=\"$fn\" value=\"$str\">\n";
        }
      }

      $display.= "<input type=\"hidden\" name=\"ignore\">\n";
    }
    $display.= "</p>\n";
  }

  /* Page header*/
  $display= print_header(get_template_path('plugins/generic/images/plugin.png'),
      _("Generic user information"), $info).$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
