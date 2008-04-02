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

    $options = array();

    $option['long_otp_prompt']= array('NAME' =>'long_otp_prompt' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['ignore_dot']=  array('NAME' =>'ignore_dot' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_always']= array('NAME' =>'mail_always' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_badpass']=  array('NAME' =>'mail_badpass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_user']=  array('NAME' =>'mail_no_user' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_host']=  array('NAME' =>'mail_no_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_perms']= array('NAME' =>'mail_no_perms' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['tty_tickets']= array('NAME' =>'tty_tickets' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['authenticate']=  array('NAME' =>'authenticate' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['root_sudo']= array('NAME' =>'root_sudo' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['log_host']=  array('NAME' =>'log_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['log_year']=  array('NAME' =>'log_year' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['shell_noargs']=  array('NAME' =>'shell_noargs' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['set_home']=  array('NAME' =>'set_home' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['always_set_home']= array('NAME' =>'always_set_home' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['path_info']= array('NAME' =>'path_info' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['preserve_groups']= array('NAME' =>'preserve_groups' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['fqdn']=  array('NAME' =>'fqdn' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['insults']= array('NAME' =>'insults' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['requiretty']=  array('NAME' =>'requiretty' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['env_editor']=  array('NAME' =>'env_editor' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['rootpw']=  array('NAME' =>'rootpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['runaspw']= array('NAME' =>'runaspw' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['targetpw']=  array('NAME' =>'targetpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['set_logname']= array('NAME' =>'set_logname' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['stay_setuid']= array('NAME' =>'stay_setuid' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['env_reset']= array('NAME' =>'env_reset' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['use_loginclass']=  array('NAME' =>'use_loginclass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['noexec']=  array('NAME' =>'noexec' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['ignore_local_sudoers']=  array('NAME' =>'ignore_local_sudoers' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['passwd_tries']=  array('NAME' =>'passwd_tries' ,  'TYPE' => 'INTEGER' ,   'DEFAULT' => '');
    $option['loglinelen']=  array('NAME' =>'loglinelen' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['timestamp_timeout']= array('NAME' =>'timestamp_timeout' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['passwd_timeout']=  array('NAME' =>'passwd_timeout' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['umask']= array('NAME' =>'umask' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['mailsub']= array('NAME' =>'mailsub' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['badpass_message']= array('NAME' =>'badpass_message' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['timestampdir']=  array('NAME' =>'timestampdir' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['timestampowner']=  array('NAME' =>'timestampowner' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['passprompt']=  array('NAME' =>'passprompt' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['runas_default']= array('NAME' =>'runas_default' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['syslog_goodpri']=  array('NAME' =>'syslog_goodpri' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['syslog_badpri']= array('NAME' =>'syslog_badpri' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['editor']=  array('NAME' =>'editor' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['noexec_file']= array('NAME' =>'noexec_file' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['lecture']= array('NAME' =>'lecture' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['lecture_file']=  array('NAME' =>'lecture_file' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['logfile']= array('NAME' =>'logfile' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['syslog']=  array('NAME' =>'syslog' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailerpath']=  array('NAME' =>'mailerpath' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailerflags']= array('NAME' =>'mailerflags' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailto']=  array('NAME' =>'mailto' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['exempt_group']=  array('NAME' =>'exempt_group' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['verifypw']=  array('NAME' =>'verifypw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['listpw']=  array('NAME' =>'listpw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['env_check']= array('NAME' =>'env_check' ,   'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    $option['env_delete']=  array('NAME' =>'env_delete' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    $option['env_keep']=  array('NAME' =>'env_keep' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    $option['long_otp_prompt']= array('NAME' =>'long_otp_prompt' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['ignore_dot']=  array('NAME' =>'ignore_dot' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_always']= array('NAME' =>'mail_always' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_badpass']=  array('NAME' =>'mail_badpass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_user']=  array('NAME' =>'mail_no_user' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_host']=  array('NAME' =>'mail_no_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['mail_no_perms']= array('NAME' =>'mail_no_perms' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['tty_tickets']= array('NAME' =>'tty_tickets' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['authenticate']=  array('NAME' =>'authenticate' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['root_sudo']= array('NAME' =>'root_sudo' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['log_host']=  array('NAME' =>'log_host' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['log_year']=  array('NAME' =>'log_year' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['shell_noargs']=  array('NAME' =>'shell_noargs' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['set_home']=  array('NAME' =>'set_home' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['always_set_home']= array('NAME' =>'always_set_home' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['path_info']= array('NAME' =>'path_info' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['preserve_groups']= array('NAME' =>'preserve_groups' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['fqdn']=  array('NAME' =>'fqdn' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['insults']= array('NAME' =>'insults' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['requiretty']=  array('NAME' =>'requiretty' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['env_editor']=  array('NAME' =>'env_editor' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['rootpw']=  array('NAME' =>'rootpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['runaspw']= array('NAME' =>'runaspw' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['targetpw']=  array('NAME' =>'targetpw' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['set_logname']= array('NAME' =>'set_logname' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['stay_setuid']= array('NAME' =>'stay_setuid' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['env_reset']= array('NAME' =>'env_reset' ,   'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['use_loginclass']=  array('NAME' =>'use_loginclass' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['noexec']=  array('NAME' =>'noexec' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['ignore_local_sudoers']=  array('NAME' =>'ignore_local_sudoers' ,  'TYPE' => 'BOOLEAN' ,   'DEFAULT' => '');
    $option['passwd_tries']=  array('NAME' =>'passwd_tries' ,  'TYPE' => 'INTEGER' ,   'DEFAULT' => '');
    $option['loglinelen']=  array('NAME' =>'loglinelen' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['timestamp_timeout']= array('NAME' =>'timestamp_timeout' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['passwd_timeout']=  array('NAME' =>'passwd_timeout' ,  'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['umask']= array('NAME' =>'umask' ,   'TYPE' => 'BOOL_INTEGER' ,  'DEFAULT' => '');
    $option['mailsub']= array('NAME' =>'mailsub' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['badpass_message']= array('NAME' =>'badpass_message' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['timestampdir']=  array('NAME' =>'timestampdir' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['timestampowner']=  array('NAME' =>'timestampowner' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['passprompt']=  array('NAME' =>'passprompt' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['runas_default']= array('NAME' =>'runas_default' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['syslog_goodpri']=  array('NAME' =>'syslog_goodpri' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['syslog_badpri']= array('NAME' =>'syslog_badpri' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['editor']=  array('NAME' =>'editor' ,  'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['noexec_file']= array('NAME' =>'noexec_file' ,   'TYPE' => 'STRINGS' ,   'DEFAULT' => '');
    $option['lecture']= array('NAME' =>'lecture' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['lecture_file']=  array('NAME' =>'lecture_file' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['logfile']= array('NAME' =>'logfile' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['syslog']=  array('NAME' =>'syslog' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailerpath']=  array('NAME' =>'mailerpath' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailerflags']= array('NAME' =>'mailerflags' ,   'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['mailto']=  array('NAME' =>'mailto' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['exempt_group']=  array('NAME' =>'exempt_group' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['verifypw']=  array('NAME' =>'verifypw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['listpw']=  array('NAME' =>'listpw' ,  'TYPE' => 'STRING_BOOL' ,   'DEFAULT' => '');
    $option['env_check']= array('NAME' =>'env_check' ,   'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    $option['env_delete']=  array('NAME' =>'env_delete' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    $option['env_keep']=  array('NAME' =>'env_keep' ,  'TYPE' => 'LISTS' ,   'DEFAULT' => '');
    ksort($option);
    $this->options = $option;


    $this->sudoOption = array();

    if(isset($this->attrs['sudoOption'])){
      for($i = 0 ; $i < $this->attrs['sudoOption']['count']; $i++){

        $negation = FALSE;
        $value    = "";

        $opt = $this->attrs['sudoOption'][$i];
        if(preg_match("/^!/",$opt)){
          $negation = TRUE; 
          $opt = preg_replace("/^!/","",$opt);
        }

        if(preg_match("/=/",$opt)){
          $value  = preg_replace("/^[^=]*+=/","",$opt);
          $opt    = preg_replace("/=.*$/","",$opt);
        }

        $option = array();
        $option['NAME']   = $opt;
        $option['VALUE']  = array($value);
        $option['NEGATE'] = $negation;

        if(!isset($this->sudoOption[$opt])){
          $this->sudoOption[$opt] = $option;
        }else{
          $this->sudoOption[$opt]['VALUE'][] = $value;
        }
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();
 
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
