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
    if(!session::global_is_set('config')){
      $entry['user']= "unkown";
    }else{

      $this->config = session::global_get('config');
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

    if(!isset($this->config) || ( $this->config->get_cfg_value("logging") == ""  && empty($entry['user']))){
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

      if (isset ($this->config)){
        if ($this->config->get_cfg_value("logging") == ""){
          $this->log_into_syslog($entry);
        }else{

          /* Start logging for configured methods */
          if(preg_match("/(^|,)syslog(,|$)/i",$this->config->get_cfg_value("logging"))){
            $this->log_into_syslog($entry);
          }
          if(preg_match("/(^|,)mysql(,|$)/i",$this->config->get_cfg_value("logging"))){
            $this->log_into_db($entry);
          }
        }
      }
    }
  }


  function check($entry = array())
  {
    $msgs = array();

    if(!isset($entry['action']) || !in_array($entry['action'],array("modify","create","remove","copy","snapshot","view","security","debug"))){
      $msgs[] = sprintf(_("Invalid option '%s' specified!"), $entry['action']);
    }

    if(!isset($entry['objecttype']) || empty($entry['objecttype'])){
      $msgs[] = _("Specified objectType is empty or invalid!");
    }
  
    return($msgs);
  }

   
  /* This function is used to into the systems syslog */
  function log_into_syslog($entry)
  {
    $str = $entry['user']." ".$entry['action']." ".$entry['object']." from type ".$entry['objecttype']." ".$entry['changes']." : Result was ".$entry['result'];
    gosa_log($str);
  }


  function disable_mysql_log($server,$error)
  {
    global $config;
    msg_dialog::display(_("Error"), $error, ERROR_DIALOG);
    msg_dialog::display(_("MySQL error"),sprintf(_("Logging to MySQL database will be disabled for server '%s'!"),$server), INFO_DIALOG);
    unset($config->data['SERVERS']['LOGGING'][$server]) ;
    $this->config = $config;
  }


  /* Log into configured logging databses.*/
  function log_into_db($entry)
  {
    if(isset($this->config->data['SERVERS']['LOGGING'])){
      $servers = $this->config->data['SERVERS']['LOGGING'];
    }else{
      return(FALSE);
    }

    /* Log into each configured server 
     */
    foreach($servers as $server_name => $server){

      $error = "";
   
      /* Connect to the database 
       */
      $con = @mysql_pconnect($server_name,$server['USER'],$server['PWD']);
      if(!$con){
        $error = msgPool::dbconnect(_("MySQL logging"),mysql_error());
        $this->disable_mysql_log($server_name,$error);
        continue;
      }

      /* Check if the database is available 
       */
      $db = @mysql_select_db($server['DB'],$con);
      if(!$db){
        $error = msgPool::dbselect(_("MySQL logging"),mysql_error());
        $this->disable_mysql_log($server_name,$error);
        continue;
      }

      /* Check if our current location is already registerd 
         in this case get its id.
         If it wasn't registered yet, create it. 
       */
      $base = mysql_escape_string($this->config->current['BASE']);
      $query= "SELECT id FROM gosa_locations WHERE location=\"".$base."\";";
      $res  = mysql_query($query);  
      if(!$res){
        $error = msgPool::dbquery(_("MySQL logging"),mysql_error());
        $this->disable_mysql_log($server_name,$error);
        continue;
      }

      $location_id = -1;
      while($attrs = mysql_fetch_assoc($res)){
        $location_id = $attrs['id'];
        break;
      }

      /* No location found that matches our location.
         Create it.
       */
      if($location_id == -1){
        $query = "INSERT INTO gosa_locations (location) VALUES ('".$base."');";
        if(!mysql_query($query,$con)){
          $error = msgPool::dbquery(_("MySQL logging"),mysql_error());
          $this->disable_mysql_log($server_name,$error);
          continue;
        }

        /* Try to detect the location again 
         */ 
        $query= "SELECT id FROM gosa_locations WHERE location=\"".$base."\";";
        $res  = mysql_query($query);
        $location_id = -1;
        while($attrs = mysql_fetch_assoc($res)){
          $location_id = $attrs['id'];
          break;
        }
        if($location_id == -1){
          $error = sprintf(_("Cannot add location to the database!")."<br><br>"._("Error").": %s",mysql_error($con));
          $this->disable_mysql_log($server_name,$error);
          continue;
        }
      }  

      /* Create mysql syntax */
      $query ="INSERT INTO gosa_log 
        (timestamp,user,action,objecttype,object,changes,result,location_id)
        VALUES 
        (
         \"".mysql_escape_string($entry['timestamp'])."\", 
         \"".mysql_escape_string($entry['user'])."\", 
         \"".mysql_escape_string($entry['action'])."\", 
         \"".mysql_escape_string($entry['objecttype'])."\", 
         \"".mysql_escape_string($entry['object'])."\", 
         \"".mysql_escape_string($entry['changes'])."\", 
         \"".mysql_escape_string($entry['result'])."\", 
         \"".mysql_escape_string($location_id)."\" 
        );
      ";
      $res = mysql_query($query,$con);
      if(!$res){
        $error = dbquery(_("MySQL logging"), mysql_error());
        $this->disable_mysql_log($server_name,$error);
        continue;
      } 
      if(is_resource($con)){
        mysql_close($con);
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
