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

class password extends plugin
{
  /* Definitions */
  var $plHeadline     = "Password";
  var $plDescription  = "This does something";

  function password($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin($config, $dn, $parent);
  }


  function execute()
  {
    plugin::execute();
    $smarty = get_smarty();
    $ui = get_userinfo();

    /* Display expiration template */
    if((isset($this->config->data['MAIN']['ACCOUNT_EXPIRATION'])) &&
        preg_match('/true/i', $this->config->data['MAIN']['ACCOUNT_EXPIRATION'])){
      $expired= ldap_expired_account($this->config, $ui->dn, $ui->username);
      if($expired == 4){
        return($smarty->fetch(get_template_path("nochange.tpl", TRUE)));
      }
    }
  
    /* Pwd change requested */
    if (isset($_POST['password_finish'])){

      /* Should we check different characters in new password */
      $check_differ = isset($this->config->data['MAIN']['PWDIFFER']);
      $differ       = @$this->config->data['MAIN']['PWDIFFER'];

      /* Enable length check ? */
      $check_length = isset($this->config->data['MAIN']['PWMINLEN']);
      $length       = @$this->config->data['MAIN']['PWMINLEN'];

      /* Call external password quality hook ?*/
      $check_hook   = isset($this->config->data['MAIN']['EXTERNALPWDHOOK']);
      $hook         = @$this->config->data['MAIN']['EXTERNALPWDHOOK']." ".$ui->username." ".$_POST['current_password']." ".$_POST['new_password'];
      if($check_hook){
        exec($hook,$resarr);
        $check_hook_output = "";
        if(count($resarr) > 0) {
          $check_hook_output= join('\n', $resarr);
        }
      }

      /* Check given values */    
      if(empty($_POST['current_password'])){
        print_red(_("You need to specify your current password in order to proceed."));
      }elseif ($_POST['new_password'] != $_POST['repeated_password']){
        print_red(_("The passwords you've entered as 'New password' and 'Repeated new password' do not match."));
      } elseif ($_POST['new_password'] == ""){
        print_red(_("The password you've entered as 'New password' is empty."));
      }elseif($check_differ && (substr($_POST['current_password'], 0, $differ) == substr($_POST['new_password'], 0, $differ))){
        print_red(_("The password used as new and current are too similar."));
      }elseif($check_length && (strlen($_POST['new_password']) < $length)){
        print_red(_("The password used as new is to short."));
      }elseif($check_hook && $check_hook_output != ""){
        print_red(_("External password changer reported a problem: ".$output));
      }else{

        /* Try to connect via current password */
        $tldap = new LDAP(
            $ui->dn, 
            $_POST['current_password'],
            $this->config->current['SERVER'],
            isset($this->config->current['RECURSIVE'])  && preg_match("/true/i",$this->config->current['RECURSIVE']),
            isset($this->config->current['TLS'])        && preg_match("/true/i",$this->config->current['TLS']));

        /* connection Successfull ? */
        if ($tldap->error != "Success"){
          print_red(_("The password you've entered as your current password doesn't match the real one."));
        }else{

          /* Check GOsa permissions */
          $ca= get_permissions ($ui->dn, $ui->subtreeACL);
          $ca= get_module_permission($ca, "user", $ui->dn);
          if (chkacl($ca, "password") != ""){
            print_red(_("You have no permissions to change your password."));
          }else{
            change_password ($ui->dn, $_POST['new_password']);
            gosa_log ("User/password has been changed");
            $ui->password= $_POST['new_password'];
            $_SESSION['ui']= $ui;
            return($smarty->fetch(get_template_path("changed.tpl", TRUE)));
          }
        }
      }
    }

    return($smarty->fetch(get_template_path("password.tpl", TRUE)));
  } 
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
