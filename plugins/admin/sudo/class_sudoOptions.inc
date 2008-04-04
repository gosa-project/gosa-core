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

class sudoOption extends plugin
{
  /* Group attributes */
  var $sudoOption = array();

  var $attributes    = array("sudoOption");
  var $is_account = TRUE;
  
  var $options = array();

  function sudoOption(&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /****
      Create a list of known options
     ****/
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
    $option['mailsub']= array('NAME' =>'mailsub' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['badpass_message']= array('NAME' =>'badpass_message' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['timestampdir']=  array('NAME' =>'timestampdir' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['timestampowner']=  array('NAME' =>'timestampowner' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['passprompt']=  array('NAME' =>'passprompt' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['runas_default']= array('NAME' =>'runas_default' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['syslog_goodpri']=  array('NAME' =>'syslog_goodpri' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['syslog_badpri']= array('NAME' =>'syslog_badpri' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['editor']=  array('NAME' =>'editor' ,  'TYPE' => 'STRING' ,   'DEFAULT' => '');
    $option['noexec_file']= array('NAME' =>'noexec_file' ,   'TYPE' => 'STRING' ,   'DEFAULT' => '');
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

    /****
      Parse given sudoOption attributes 
     ****/
    $this->sudoOption = array();
    if(isset($this->attrs['sudoOption'])){
      for($i = 0 ; $i < $this->attrs['sudoOption']['count']; $i++){

        /****
          Detect attribute name/value/negation
         ****/
        $opt = $this->attrs['sudoOption'][$i];

        /* Get negation */ 
        $negation = FALSE;
        if(preg_match("/^!/",$opt)){
          $negation = TRUE; 
          $opt = preg_replace("/^!/","",$opt);
        }

        /* Get value / name*/
        $value    = "";
        if(preg_match("/=/",$opt)){
          $value  = preg_replace("/^[^=]*+=/","",$opt);
          $opt    = preg_replace("/=.*$/","",$opt);
        }

        /* Check if the given value is part of our options list.
           If it is not, add it as type STRING and display a warning.  
        */
        if(!isset($this->options[$opt])){
          $this->options[$opt]=array('NAME'=>$opt,'TYPE'=>'STRING','DEFAULT' => '');
          msg_dialog::display(_("Unknown option"),
              sprintf(_("The sudo option '%s' is unkown to GOsa, it is now temporarily added as type string."),
                $opt),WARNING_DIALOG);
        }

        /* Create internal sudoOption object */
        $option = array();
        $option['NAME']   = $opt;
        $option['VALUE']  = array($value);
        $option['NEGATE'] = $negation;

        /*  Special handling for mixed flag types. 
            Some attributes like (BOOL_INTEGER) can be TRUE/FALSE and INTEGER.
            This means, if the value is empty it is BOOL and $negation defines its boolean value.
         */
        if(in_array($this->options[$opt]['TYPE'],array("BOOL_INTEGER","STRING_BOOL"))){
          if(empty($value)){
            $option['NEGATE'] = FALSE;
            if($negation){
              $option['VALUE'] = array(0 => "FALSE");
            }else{
              $option['VALUE'] = array(0 => "TRUE");
            }
          }
        }

        if($this->options[$opt]['TYPE'] == "LISTS"){
           $this->sudoOption[$opt][] = $option;
        }

        /* Special handling for BOOLEAN values */
        if(in_array($this->options[$opt]['TYPE'],array("BOOLEAN"))){
          $option['NEGATE'] = FALSE;
          if($negation){
            $option['VALUE'] = array(0 => "FALSE");
          }else{
            $option['VALUE'] = array(0 => "TRUE");
          }
        }

        /* Append values */
        if(!isset($this->sudoOption[$opt])){
          $this->sudoOption[$opt] = $option;
        }
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /*****
      Handle Posts 
     *****/
    foreach($_POST as $name => $value){

      if(preg_match("/^negListOption_/",$name)){
        $opt = preg_replace("/^negListOption_/","",$name);
        $opt = preg_replace("/_[^_]*$/","",$opt);
        $id  = preg_replace("/^.*_([0-9])*$/","\\1",$opt);
        $opt = preg_replace("/_[0-9]*$/","",$opt);

        if(isset($this->sudoOption[$opt][$id])){
          $this->sudoOption[$opt][$id]['NEGATE'] = !$this->sudoOption[$opt][$id]['NEGATE']; 
        }
        break;
      }

      if(preg_match("/^negOption_/",$name)){
        $opt = preg_replace("/^negOption_/","",$name);
        $opt = preg_replace("/_[^_]*$/","",$opt);
        if(isset($this->sudoOption[$opt])){
          $val = $this->sudoOption[$opt]['VALUE'][0];

          /*****
            Negate STRING_BOOL && BOOL_INTEGER
           *****/
          if(in_array($this->options[$opt]['TYPE'],array('STRING_BOOL','BOOL_INTEGER'))){
            if(in_array($val, array("TRUE","FALSE"))){
              if($val == "TRUE"){
                $this->sudoOption[$opt]['VALUE'][0] = "FALSE";
              }else{
                $this->sudoOption[$opt]['VALUE'][0] = "TRUE";
              }
            }else{
               $this->sudoOption[$opt]['NEGATE'] = !$this->sudoOption[$opt]['NEGATE']; 
            }
          }

          /*****
            Negate STRING / INTEGER
           *****/
          if(in_array($this->options[$opt]['TYPE'],array('STRING','INTEGER'))){
            $this->sudoOption[$opt]['NEGATE'] = !$this->sudoOption[$opt]['NEGATE']; 
          }

          /*****
            Negate BOOLEAN
           *****/
          if(in_array($this->options[$opt]['TYPE'],array('BOOLEAN'))){
            if($val == "TRUE"){
              $this->sudoOption[$opt]['VALUE'][0] = "FALSE";
            }else{
              $this->sudoOption[$opt]['VALUE'][0] = "TRUE";
            }
          }
        }
        break;
      }

      /*****
        Remove options
       *****/
      if(preg_match("/^delOption_/",$name)){
        $opt = preg_replace("/^delOption_/","",$name);
        $opt = preg_replace("/_[^_]*$/","",$opt);
        if(isset($this->sudoOption[$opt])){
          unset($this->sudoOption[$opt]);
        }
        break;
      }

      /*****
        Remove LISTS options
       *****/
      if(preg_match("/^delListOption/",$name)){
        $opt = preg_replace("/^delListOption_/","",$name);
        $opt = preg_replace("/_[^_]*$/","",$opt);
        $id  = preg_replace("/^.*_([0-9])*$/","\\1",$opt);
        $opt = preg_replace("/_[0-9]*$/","",$opt);

        if(isset($this->sudoOption[$opt][$id])){
          unset($this->sudoOption[$opt][$id]);
        }
        if(!count($this->sudoOption[$opt])){
          unset($this->sudoOption[$opt]);
        }
        break;
      }
    }

 
    $smarty = get_smarty();
    $smarty->assign("map",  array("STRING" => _("string"), "BOOLEAN" => _("bool"),
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

    if(isset($_POST['add_option']) && isset($_POST['option'])){
      $opt = get_post("option");

      /* Append attribute only once, lists are handled below */
      if(isset($this->options[$opt]) && !isset($this->sudoOption[$opt])){
        $type = $this->options[$opt]['TYPE'];
        $val  = array($this->options[$opt]['DEFAULT']);
        $option = array("NAME" => $opt, "VALUE" => $val , "NEGATE" => FALSE);

        if($type == "LISTS"){
          $this->sudoOption[$opt][] = $option;
        }else{
          $this->sudoOption[$opt] = $option;
        }
      }

      if(isset($this->options[$opt]) && isset($this->sudoOption[$opt]) && $this->options[$opt]['TYPE'] == "LISTS"){
        $type = $this->options[$opt]['TYPE'];
        $val  = array($this->options[$opt]['DEFAULT']);
        $option = array("NAME" => $opt, "VALUE" => $val , "NEGATE" => FALSE);
        $this->sudoOption[$opt][] = $option;
      }
    }

    foreach($this->sudoOption as $name => $opt){

      /****
        Get posted value for BOOLEAN
       ****/
      if(in_array($this->options[$name]['TYPE'],array("BOOLEAN"))){
        if(isset($_POST['option_value__'.$name])){
          $this->sudoOption[$name]['VALUE'][0] = get_post('option_value__'.$name);
        }
      }

      /****
        Get posted value for STRING / INTEGER
       ****/
      if(in_array($this->options[$name]['TYPE'],array("STRING","INTEGER"))){
        if(isset($_POST['option_value__'.$name])){
          $this->sudoOption[$name]['VALUE'][0] = get_post('option_value__'.$name);
        }
      }

      /****
        Get posted value for STRING_BOOL / BOOL_INTEGER
       ****/
      if(in_array($this->options[$name]['TYPE'],array("BOOL_INTEGER","STRING_BOOL"))){
        if(isset($_POST['option_selection__'.$name])){
          $sel = get_post('option_selection__'.$name);
          $val = "";
          if(isset($_POST['option_value__'.$name])){
            $val = get_post('option_value__'.$name);
          }

          if($sel == "FALSE" || $sel == "TRUE"){
            $this->sudoOption[$name]['VALUE'] = array($sel);
            $this->sudoOption[$name]['NEGATE'] = FALSE;
          }else{
            $this->sudoOption[$name]['VALUE'] = array($val);
          }
        }
      }

      /****
        Get posted value for LISTS
       ****/
      if(in_array($this->options[$name]['TYPE'],array("LISTS"))){
        foreach($this->sudoOption[$name] as $entry_key => $entry){
          if(isset($_POST['list_value__'.$name.'_'.$entry_key])){
            $val = get_post('list_value__'.$name.'_'.$entry_key);
            $this->sudoOption[$name][$entry_key]['VALUE'][0] = $val;
          }
        } 
      }
    }
  }

  /* Save to LDAP */
  function save()
  {
    plugin::save(); 

    $this->attrs['sudoOption'] = array();
    foreach($this->sudoOption as $name => $opt){

      $type   = $this->options[$name]['TYPE'];

      if($type=="LISTS"){
        foreach($this->sudoOption[$name] as $entry_key => $entry){
          $val =  $entry['VALUE'][0];
          if(empty($val)){
            $option = $name;
          }else{
            $option = $name."=".$val;
          }
          if($entry['NEGATE']){
            $option = "!".$option;
          }
          $this->attrs['sudoOption'][] = $option;
        }
        continue;
      }

      $neg    = $opt['NEGATE'];
      $value  = $opt['VALUE'][0];
      $option = "";

      /****
        Save BOOLEAN
       ****/
      if(in_array($type,array("BOOLEAN"))){ 
        $option = $name;
        if($value == "FALSE"){
          $option = "!".$option;
        }
      }

      /****
        Save STRING / INTEGER
       ****/
      if(in_array($type,array("STRING","INTEGER"))){ 
        $option = $name."=".$value;
        if($neg){
          $option = "!".$option;
        }
      }

      /****
        Save STRING_BOOL / BOOL_INTEGER
       ****/
      if(in_array($type,array("STRING_BOOL","BOOL_INTEGER"))){
        if($value == "FALSE"){
          $option = "!".$name;
        }elseif($value == "TRUE"){
          $option = $name;
        }else{
          $option = $name."=".$value;
          if($neg){
            $option = "!".$option;
          }
        }
      }

      $this->attrs['sudoOption'][] = $option;
    }
    $this->cleanup();
    $ldap = $this->config->get_ldap_link();
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
