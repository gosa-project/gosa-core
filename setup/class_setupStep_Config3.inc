<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

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


class Step_Config3 extends setup_step
{
  var $header_image= "images/ldapserver.png";

  var $errorlvl   = TRUE;

  var $optional = array(
              "strict_units" => false,
              "forceglobals" => true,
              "forcessl" => false,
              "ldapstats" => false,
              "warnssl" => true,
              "ppd_path" => "/var/spool/ppd/",
              "ppd_path_active" => FALSE,
              "compile" =>  "/var/spool/gosa",
              "debuglevel" => 0,
              "session_lifetime" => 7200,
              "max_ldap_query_time" => "5.0",
              "max_ldap_query_time_active" => FALSE,

              "mailQueueScriptPath" => "/usr/bin/sudo /usr/local/sbin/mailqueue %action %id %server",
              "mailQueueScriptPath_active" => FALSE,

              "auto_network_hook" => "/usr/bin/net-resolver.sh",
              "auto_network_hook_active" => FALSE,

              "notifydir" => "",
              "notifydir_active" => FALSE,

              "kioskpath" => "/var/spool/kiosk",
              "kioskpath_active" => FALSE,

              "noprimarygroup"  => FALSE);

  var $attributes = array("errorlvl");

  function Step_Config3()
  {
    $this->update_strings();
  }


  function update_strings()
  {
    $this->s_title      = _("GOsa settings 3/3");
    $this->s_title_long = _("GOsa settings 3/3");
    $this->s_info       = _("Tweak some GOsa core behaviour");
  }
  

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("bool",array(FALSE => _("No"), TRUE => _("Yes")));
    $smarty->assign("optional", $this->optional);
    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty -> fetch (get_template_path("../setup/setup_config3.tpl")));
  }


  function save_object()
  {
    if(isset($_POST['step6a_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate(get_post($attr));
        }
      }
      
      foreach(array("strict_units","noprimarygroup","forceglobals","forcessl","ldapstats","warnssl","compile","debuglevel","session_lifetime") as $name){
        if(isset($_POST[$name])){
          $this->optional[$name] = get_post($name);
        }
      } 

      if(isset($_POST['ppd_path_active'])){
        $this->optional['ppd_path_active'] = TRUE;
        if(isset($_POST['ppd_path'])){
          $this->optional['ppd_path'] = get_post('ppd_path');
        }
      }else{
        $this->optional['ppd_path_active'] = FALSE;
      }

      if(isset($_POST['max_ldap_query_time_active'])){
        $this->optional['max_ldap_query_time_active'] = TRUE;
        if(isset($_POST['max_ldap_query_time'])){
          $this->optional['max_ldap_query_time'] = get_post('max_ldap_query_time');
        }
      }else{
        $this->optional['max_ldap_query_time_active'] = FALSE;
      }

      if(isset($_POST['mailQueueScriptPath_active'])){
        $this->optional['mailQueueScriptPath_active'] = TRUE;
        if(isset($_POST['mailQueueScriptPath'])){
          $this->optional['mailQueueScriptPath'] = get_post('mailQueueScriptPath');
        }
      }else{
        $this->optional['mailQueueScriptPath_active'] = FALSE;
      }

      if(isset($_POST['kioskpath_active'])){
        $this->optional['kioskpath_active'] = TRUE;
        if(isset($_POST['kioskpath'])){
          $this->optional['kioskpath'] = get_post('kioskpath');
        }
      }else{
        $this->optional['kioskpath_active'] = FALSE;
      }

      if(isset($_POST['auto_network_hook_active'])){
        $this->optional['auto_network_hook_active'] = TRUE;
        if(isset($_POST['auto_network_hook'])){
          $this->optional['auto_network_hook'] = get_post('auto_network_hook');
        }
      }else{
        $this->optional['auto_network_hook_active'] = FALSE;
      }

      if(isset($_POST['notifydir_active'])){
        $this->optional['notifydir_active'] = TRUE;
        if(isset($_POST['notifydir'])){
          $this->optional['notifydir'] = get_post('notifydir');
        }
      }else{
        $this->optional['notifydir_active'] = FALSE;
      }
    }

    $tmp = $this->check();
    if(count($tmp) == 0){
      $this->is_completed = TRUE;
    }else{
      $this->is_completed = FALSE;
    }
  }


  function check()
  {
    $message = array();

    if(!is_numeric( $this->optional['session_lifetime'])){
      $message[] = _("Session lifetime must be a numeric value.");
    }

    if(!is_numeric( $this->optional['max_ldap_query_time'])){
      $message[] = _("Maximal ldap query time must be a numeric value. ");
    }
  
    return($message);
  }
  

  /* Attributes that are interesting for configuration generation */
  function get_attributes()
  {
    $tmp = setup_step::get_attributes();
    foreach(array("optional") as $attr){
      $tmp[$attr]= $this->$attr;
    }
    return($tmp);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
