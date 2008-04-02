<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_sudo.inc 9975 2008-03-25 14:09:30Z hickert $$
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

class sudo_options extends plugin
{
  /* Group attributes */
  var $sudoOption = array();

  var $attributes    = array("sudoOption");
  var $is_account = TRUE;
  
  var $options = array();

  function sudo_options(&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    foreach(array("sudoOption") as $attr){
      $this->$attr = array();
      if(isset($this->attrs[$attr])){
        $tmp = array();
        for($i = 0 ; $i < $this->attrs[$attr]['count']; $i++){
          $tmp[] = $this->attrs[$attr][$i];
        }
        $this->$attr = $tmp;
      }
    }

    $options = array();
    $option[]= array('NAME' => 'long_otp_prompt' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'ignore_dot' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_always' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_badpass' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_user' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_host' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_perms' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'tty_tickets' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'authenticate' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'root_sudo' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'log_host' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'log_year' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'shell_noargs' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'set_home' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'always_set_home' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'path_info' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'preserve_groups' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'fqdn' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'insults' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'requiretty' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_editor' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'rootpw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'runaspw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'targetpw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'set_logname' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'stay_setuid' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_reset' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'use_loginclass' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'noexec' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'ignore_local_sudoers' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passwd_tries' , 'TYPE' => 'INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'loglinelen' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestamp_timeout' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passwd_timeout' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'umask' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailsub' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'badpass_message' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestampdir' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestampowner' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passprompt' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'runas_default' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog_goodpri' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog_badpri' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'editor' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'noexec_file' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'lecture' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'lecture_file' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'logfile' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailerpath' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailerflags' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailto' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'exempt_group' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'verifypw' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'listpw' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_check' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_delete' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_keep' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'long_otp_prompt' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'ignore_dot' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_always' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_badpass' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_user' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_host' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mail_no_perms' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'tty_tickets' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'authenticate' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'root_sudo' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'log_host' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'log_year' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'shell_noargs' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'set_home' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'always_set_home' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'path_info' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'preserve_groups' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'fqdn' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'insults' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'requiretty' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_editor' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'rootpw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'runaspw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'targetpw' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'set_logname' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'stay_setuid' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_reset' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'use_loginclass' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'noexec' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'ignore_local_sudoers' , 'TYPE' => 'BOOLEAN' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passwd_tries' , 'TYPE' => 'INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'loglinelen' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestamp_timeout' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passwd_timeout' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'umask' , 'TYPE' => 'BOOL_INTEGER' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailsub' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'badpass_message' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestampdir' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'timestampowner' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'passprompt' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'runas_default' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog_goodpri' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog_badpri' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'editor' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'noexec_file' , 'TYPE' => 'STRINGS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'lecture' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'lecture_file' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'logfile' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'syslog' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailerpath' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailerflags' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'mailto' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'exempt_group' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'verifypw' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'listpw' , 'TYPE' => 'STRING_BOOL' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_check' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_delete' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $option[]= array('NAME' => 'env_keep' , 'TYPE' => 'LISTS' , 'DEFAULT' => '');
    $this->options = $option;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
  
    
    /*********************
       NEGATE values 
     *********************/ 
    foreach($_POST as $name => $value){
      if(preg_match("/^neg_/",$name)){
        $attr = preg_replace("/^neg_([^_]*)_.*$/","\\1",$name);
        $value= preg_replace("/^neg_[^_]*_([0-9]*)_.*$/","\\1",$name);
  
        $attrs = $this->$attr;
        if(isset( $attrs[$value])){
          $v =  $attrs[$value];
          if(preg_match("/^!/",$v)){
            $attrs[$value] = preg_replace("/^!/","",$v);
          }else{
            $attrs[$value] = "!".$v;
          }
          $this->$attr = $attrs;  
        }
        break; // Do it once, image inputs will be posted twice
      }
    }
  
    /*********************
       Delete values 
     *********************/ 
    foreach($_POST as $name => $value){
      if(preg_match("/^del_/",$name)){
        $attr = preg_replace("/^del_([^_]*)_.*$/","\\1",$name);
        $value= preg_replace("/^del_[^_]*_([0-9]*)_.*$/","\\1",$name);
        $attrs = $this->$attr;
        if(isset( $attrs[$value])){
          unset($attrs[$value]);
          $this->$attr = $attrs;  
        }
        break; // Do it once, image inputs will be posted twice
      }
    }

    $smarty = get_smarty();
    $smarty->assign("map",  array("STRINGS" => _("string"), "BOOLEAN" => _("bool"),
      "INTEGER" => _("integer") , "BOOL_INTEGER" => _("integer")."-"._("bool") ,
      "STRING_BOOL" => _("string")."-"._("bool"),"LISTS" => _("lists")));
    $smarty->assign("sudoOption",$this->sudoOption);
    $smarty->assign("options",$this->options);
    return($smarty->fetch(get_template_path('options.tpl', TRUE)));
  }

  function remove_from_parent()
  {
  }

  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }

  /* Save to LDAP */
  function save()
  {
    plugin::save(); 
    $this->cleanup();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);;
  }

  function check()
  {
    $message = plugin::check();
    return ($message);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
