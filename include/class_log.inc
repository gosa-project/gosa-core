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


/*! \brief   The logging base class
  \author  Fabian Hickert <hickert@gonicus.de>
  \version 2.6
  \date    11.04.2007

  This is the base class for the GOsa logging functionality.
  All logging should lead to this class. 
 */
class log {

  var $LOG_TO_MYSQL = FALSE;
  var $LOG_TO_SYSLOG= FALSE;
  var $config;

 /*! \brief logging constructor

    \param action         One of these values (modify|create|remove|snapshot|copy)
    \param objecttype     represents the current edited objecttype, like users/user
    \param object         represents the current edited object dn
    \param changes_array  An array containing names of all touched attributes
    \param result         A status message, containing errors or success messages 

    \sa log()
   */
  function log($action,$objecttype,$object,$changes_array = array(),$result = "")
  {
    if(!is_array($changes_array)){
      trigger_error("log(string,string,string,array(),bool). Forth parameter must be an array.");
      $changes_array = array();
    }

    $entry = array(); 
    if(!session::is_set('config')){
      $entry['user']= "unkown";
    }else{

      $this->config = session::get('config');
      $ui           = get_userinfo(); 
      $entry['user']= @$ui->dn;
    }

    /* Create string out of changes */
    $changes  ="";
    foreach($changes_array as $str ){
      $changes .= $str.",";
    }
    $changes = preg_replace("/,$/","",$changes );
    
    /* Create data object */
    $entry['timestamp'] = time();
    $entry['action']    = $action;
    $entry['objecttype']= $objecttype;
    $entry['object']    = $object;
    $entry['changes']   = $changes;
    $entry['result']    = $result;

    if(!isset($this->config->current['LOGGING']) && empty($entry['user'])){
      $entry['user']  = "unknown";
    }
 
    /* Check if all given values are valid */
    $msgs = @log::check($entry);
    if(count($msgs)){
      foreach($msgs as $msg){
        trigger_error("Logging failed, reason was: ".$msg);
        msg_dialog::display(_("Internal error"), sprintf(_("Logging failed: %s"), $msg), ERROR_DIALOG);
      }
      
    }else{

      if(!isset($this->config->current['LOGGING'])){
        $this->log_into_syslog($entry);
      }else{

        /* Start logging for configured methods */
        if(isset($this->config->current['LOGGING']) && preg_match("/(^|,)syslog(,|$)/i",$this->config->current['LOGGING'])){
          $this->log_into_syslog($entry);
        }
        if(isset($this->config->current['LOGGING']) && preg_match("/(^|,)mysql(,|$)/i",$this->config->current['LOGGING'])){
          $res = $this->log_into_db($entry);

          if(!$res){
            global $config;
            $config->current['LOGGING'] = preg_replace("/(^|,)mysql(,|$)/",",",$config->current['LOGGING']);
            msg_dialog::display(_("Logging to MySQL disabled"),_("The logging to a MySQL database is now disabled for this session of GOsa, due to communication errors with the specified logging database."), INFO_DIALOG);
          }
        }
      }
    }
  }


  function check($entry = array())
  {
    $msgs = array();

    if(!isset($entry['action']) || !in_array($entry['action'],array("modify","create","remove","copy","snapshot","view","security","debug"))){
      $msgs[] = sprintf(_("Invalid option '%s' specified."), $entry['action']);
    }

    if(!isset($entry['objecttype']) || empty($entry['objecttype'])){
      $msgs[] = _("Specified objectType is empty or invalid");
    }
  
    return($msgs);
  }

   
  /* This function is used to into the systems syslog */
  function log_into_syslog($entry)
  {
    $str = $entry['user']." ".$entry['action']." ".$entry['object']." from type ".$entry['objecttype']." ".$entry['changes']." : Result was ".$entry['result'];
    gosa_log($str);
  }


  /* Log into configured logging databses.*/
  function log_into_db($entry)
  {
    if(isset($this->config->data['SERVERS']['LOGGING'])){
      $servers = $this->config->data['SERVERS']['LOGGING'];
    }else{
      msg_dialog::display(_("Error"), _("You have enabled the logging into mysql database, but there are no logging servers available."), ERROR_DIALOG);
      return(FALSE);
    }

    foreach($servers as $server_name => $server){
 
      $con = @mysql_pconnect($server_name,$server['USER'],$server['PWD']);
      if(!$con){
        msg_dialog::display(_("Error"), sprintf(_("Cannot connect to logging server '%s'."),$server_name), ERROR_DIALOG);
        return(FALSE);
      }else{
        $db = mysql_select_db($server['DB'],$con);
        if(!$db){
          msg_dialog::display(_("Error"), sprintf(_("Cannot select database '%s' on server '%s': %s"),$server['DB'],$server['SERVER'], mysql_error($con)), ERROR_DIALOG);
          return(FALSE);
        }else{

          /* Create mysql syntax */
          $query ="INSERT INTO gosa_log 
                    (timestamp,user,action,objecttype,object,changes,result)
                   VALUES 
                    (
                        \"".addslashes($entry['timestamp'])."\", 
                        \"".addslashes($entry['user'])."\", 
                        \"".addslashes($entry['action'])."\", 
                        \"".addslashes($entry['objecttype'])."\", 
                        \"".addslashes($entry['object'])."\", 
                        \"".addslashes($entry['changes'])."\", 
                        \"".addslashes($entry['result'])."\" 
                    );
              ";
          $res = mysql_query($query,$con);
          if(!$res){
            msg_dialog::display(_("Error"), sprintf(_("Cannot query database '%s' on server '%s': %s"),$server['DB'],$server['SERVER'], mysql_error($con)), ERROR_DIALOG);
            return(FALSE);
          } 
        }
        mysql_close($con);
      }
    }
    return(TRUE);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
