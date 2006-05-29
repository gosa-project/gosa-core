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

  /* Assign headline image */
  $smarty->assign ('headimage', get_template_path('images/password.png'));
  $display= "";

  /* Check for interaction */
  if ($_SERVER["REQUEST_METHOD"] == "POST"){
    if (isset($_POST['password_finish'])){
      $message= array();

      /* Is current password correct? */
      if ($_POST['current_password'] != ""){
        $tldap = new LDAP($ui->dn, $_POST['current_password'],
            $config->current['SERVER'],
            isset($config->current['RECURSIVE']) && $config->current['RECURSIVE'] == "true",
            isset($config->current['TLS'])
            && $config->current['TLS'] == "true");
        if ($tldap->error != "Success"){
          $message[]= _("The password you've entered as your current password doesn't match the real one.");
        }
      } else {
        $message[]= _("You need to specify your current password in order to proceed.");
      }

      /* Do new and repeated password fields match? */
      if ($_POST['new_password'] != $_POST['repeated_password']){
        $message[]= _("The passwords you've entered as 'New password' and 'Repeated new password' do not match.");
      } else {
        if ($_POST['new_password'] == ""){
          $message[]= _("The password you've entered as 'New password' is empty.");
        }
      }

      /* Password policy fulfilled? */
      if (isset($config->data['MAIN']['PWDIFFER'])){
        $l= $config->data['MAIN']['PWDIFFER'];
        if (substr($_POST['current_password'], 0, $l) == substr($_POST['new_password'], 0, $l)){
          $message[]= _("The password used as new and current are too similar.");
        }
      }
      if (isset($config->data['MAIN']['PWMINLEN'])){
        if (strlen($_POST['new_password']) < $config->data['MAIN']['PWMINLEN']){
          $message[]= _("The password used as new is to short.");
        }
      }

      $ca= get_permissions ($ui->dn, $ui->subtreeACL);
      $ca= get_module_permission($ca, "user", $ui->dn);
      if (chkacl($ca, "password") != ""){
        $message[]= _("You have no permissions to change your password.");
      }

      if (count ($message) != 0){
        /* Show error message and continue editing */
        show_errors($message);
      } else {

        /* Passed quality check, just try to change the password now */
        $output= "";
        if (isset($config->data['MAIN']['EXTERNALPWDHOOK'])){
          exec($config->data['MAIN']['EXTERNALPWDHOOK']." ".$ui->username." ".
               $_POST['current_password']." ".$_POST['new_password'], $resarr);
          if(count($resarr) > 0) {
            $output= join('\n', $resarr);
          }
        }
        if ($output != ""){
          $message[]= _("External password changer reported a problem: ".$output);
          show_errors($message);
        } else {		
          change_password ($ui->dn, $_POST['new_password']);
          gosa_log ("User/password has been changed");
          $ui->password= $_POST['new_password'];
          $_SESSION['ui']= $ui;
          $display= $smarty->fetch(get_template_path("changed.tpl", TRUE));
        }
      }
    }
  }

  if ($display == ""){
    if((isset($config->data['MAIN']['ACCOUNT_EXPIRATION'])) &&
      !preg_match('/true/i', $config->data['MAIN']['ACCOUNT_EXPIRATION'])){
      $display= $smarty->fetch(get_template_path("password.tpl", TRUE));
    }else{
      $expired= ldap_expired_account($config, $ui->dn, $ui->username);
      
      if($expired == 4){
        $display= $smarty->fetch(get_template_path("nochange.tpl", TRUE));
      }else{
        $display= $smarty->fetch(get_template_path("password.tpl", TRUE));
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
