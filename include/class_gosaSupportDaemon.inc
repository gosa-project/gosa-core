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

class gosaSupportDaemon
{
  private $s_host       = "";
  private $i_port       = 0;
  private $s_encryption_key = "";

  private $o_sock       = NULL;
  private $f_timeout    = 2;
  private $s_error      = "";
  private $b_error      = FALSE;

  private $is_connected     = FALSE;


  /*! \brief  Creates a new gosaSupportDaemon object.
    @param string   Host    The Host where the daemon is running on.  
    @param integer  Port    The port which the daemon use.
    @param string   Key     The encryption string.
    @param boolean  Connect Directly connect to daemon socket.
    @param float    Timeout The timelimit for all socket actions.
   */
  public function __construct($connect=TRUE,$timeout=10)
  {
    #FIXME: bad idea about referencing global variables from within classes
    global $config;

    # load from config, store statically
    if (isset($config->current['GOSA_SI'])){

      if ($this->s_host == ""){
        $this->s_host= preg_replace("/^.*@([^:]+):.*$/", "$1", $config->current['GOSA_SI']);
        $this->i_port= preg_replace("/^.*@[^:]+:(.*)$/", "$1", $config->current['GOSA_SI']);
        $this->s_encryption_key = preg_replace("/^(.*)@[^:]+:.*$/", "$1", $config->current['GOSA_SI']);
      }

      $this->f_timeout = $timeout;
      if($connect){
        $this->connect();
      }
    }
  }


  /*! \brief  Establish daemon connection. 
    @return boolean Returns true if the connection was succesfully established. 
   */
  public function connect()
  {
    if(!empty($this->s_host) && !empty($this->i_port)){
      $this->o_sock = new Socket_Client($this->s_host,$this->i_port,TRUE,$this->f_timeout);
      if($this->o_sock->connected()){ 
        $this->o_sock->setEncryptionKey($this->s_encryption_key); 
        $this->is_connected = TRUE;
      }else{
        $this->set_error($this->o_sock->get_error());
        $this->disconnect();
        new log("debug","gosaSupportDaemon::connect()", "Cannot connect to si-server", array(),$this->get_error());
      }
    }else{
      $this->set_error(msgPool::cmdnotfound("GOSA_SI",_("GOsa support daemon")));
    }
    return($this->is_connected);
  }


  /*! \brief  Disconnect from gosa daemon.
   */
  public function disconnect()
  {
    $this->o_sock->close();
    $this->is_connected = FALSE;
  }


  /*! \brief  Sets an error message, which can be returned with get_error().
    @param  string  The Error message,
   */
  private function set_error($str)
  {
    $this->b_error = TRUE;
    $this->s_error = $str;
  }


  /*! \brief  Sets an error message, which can be returned with get_error().
    @param  string  The Error message,
   */
  private function reset_error()
  {
    $this->b_error = FALSE;
    $this->s_error = "";
  }


  /*! \brief  Checks if an error occured.
    @return boolean returns TRUE or FALSE, whether there is an error or not.
   */
  public function is_error()
  {
    return($this->b_error);
  }


  /*! \brief  Returns the last error. 
    @return Returns the last error.
   */
  public function get_error()
  {
    $str = $this->s_error;
    $str = preg_replace("/ /","&nbsp;",$str);
    return($str);
  }


  public function FAI_get_kernels($release)
  {
    $xml_msg = 
      "<xml>".
      "<header>gosa_get_available_kernel</header>".
      "<source>GOSA</source>".
      "<target>GOSA</target>".
      "<release>".$release."</release>".
      "</xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","GOsa-si",
                get_class($this)."::".__FUNCTION__, array(),
                "FAILED error was ".$this->get_error());
            return($ret);
          }

          /* Unset header tags */
          $ret = $entries['XML'];
          foreach($ret as $key => $entry){
            if(!preg_match("/^answer/i",$key)){
              unset($ret[$key]);
            }
          }
        }
      }
    }
    return($ret);
  }


  public function FAI_get_package_sections($release)
  {
    $xml_msg = "<xml><header>gosa_query_packages_list</header><target>GOSA</target><source>GOSA</source>".
      "<select>distinct section</select>".
      "<where><clause><phrase><distribution>".$release."</distribution></phrase></clause></where></xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","GOsa-si",
                get_class($this)."::".__FUNCTION__, array(),
                "FAILED error was ".$this->get_error());
            return($ret);
          }

          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET","SESSION_ID") as $type){
            if(isset($entries['XML'][$type])){
              unset($entries['XML'][$type]);
            }
          }
          $ret = $entries['XML'];
        }
      }
    }
    return($ret);
  }


  public function FAI_get_packages($release,$attrs,$package,$from=-1,$to=-1)
  {
    $this->reset_error();
    $ret = array();

    /* Check Parameter */
    if(!is_array($attrs) || !count($attrs)){
      trigger_error("Second parameter must be an array. With at least one attribute name.");
      return($ret);
    }

    /* Check Parameter */
    if(!is_array($package)){
      trigger_error("Third parameter must be an array. With at least one attribute name.");
      return($ret);
    }

    /* Create list of attributes to fetch */
    $attr = ""; 
    foreach($attrs as $at){
      $attr.= "<select>".$at."</select>";
    }

    /* If no package is given, search for all */
    if(!count($package)) $package = array("%");

    /* Create limit tag */
    if($from == -1){
      $limit =""; 
    }else{
      $limit = "<limit><from>".$from."</from><to>".$to."</to></limit>";
    }

    /* Create list of attributes to fetch */
    $pkgs = ""; 
    foreach($package as $pkg){
      $pkgs .="<phrase><operator>like</operator><package>".$pkg."</package></phrase>";
    }

    $xml_msg = "<xml><header>gosa_query_packages_list</header><target>GOSA</target><source>GOSA</source>".
      $attr.
      "<where>
      <clause><phrase><distribution>".$release."</distribution></phrase></clause>
      <clause><connector>OR</connector>
      ".$pkgs."
      </clause>
      </where>".
      $limit.
      "</xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","GOsa-si",
                get_class($this)."::".__FUNCTION__, array(),
                "FAILED error was ".$this->get_error());
            return($ret);
          }

          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET","SESSION_ID") as $type){
            if(isset($entries['XML'][$type])){
              unset($entries['XML'][$type]);
            }
          }
          $ret = $entries['XML'];
        }
      }
    }
    return($ret);

    
  }


  public function FAI_get_server($name = "")
  {
    $this->reset_error();

    $xml_msg = "<xml><header>gosa_query_fai_server</header><target>GOSA</target><source>GOSA</source></xml>";
    $ret = array();
    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","GOsa-si", 
              get_class($this)."::".__FUNCTION__, array(),
              "FAILED error was ".$this->get_error());
            return($ret);
          }

          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET","SESSION_ID") as $type){
            if(isset($entries['XML'][$type])){
              unset($entries['XML'][$type]);
            }
          }
          $ret = $entries['XML']; 
        }
      }
    }
    return($ret);
  }


  public function FAI_get_classes($name)
  {
    $this->reset_error();
    $xml_msg = "<xml><header>gosa_query_fai_release</header><target>GOSA</target><source>GOSA</source>".
                  "<where><clause><phrase><release>".$name."</release></phrase></clause></where></xml>";;
    $ret = array();
    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","GOsa-si", 
              get_class($this)."::".__FUNCTION__, array($name),
              "FAILED error was ".$this->get_error());
            return($ret);
          }

          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET","SESSION_ID") as $type){
            if(isset($entries['XML'][$type])){
              unset($entries['XML'][$type]);
            }
          }
          $ret = $entries['XML']; 
        }
      }
    }
    return($ret);
  }


  /*! \brief  Returns an array containing all queued entries.
    @return Array All queued entries as an array.
   */
  public function get_queued_entries($event_types = array("*"),$from=-1,$to=-1,$sort="timestamp DESC")
  {
    $this->reset_error();
    $ret = array();

    $tags = "";
    foreach($event_types as $type){
      $tags .= "<phrase><headertag>".$type."</headertag></phrase>";
    }
    if(count($event_types) > 1){
      $tags = "<connector>or</connector>".$tags;
    }
    if(count($event_types)){
      $tags = "<where><clause>".$tags."</clause></where>";
    }

    $xml_msg = 
      "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      ".$tags."

      <orderby>".$sort."</orderby>";
    if($from != -1 && $to != -1){
      $xml_msg.= "
        <limit>
        <from>".$from."</from>
        <to>".$to."</to>
        </limit>";
    }
    $xml_msg.= "
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if(isset($entries['XML'])){
          
          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET") as $type){
            unset($entries['XML'][$type]);
          }
          $ret = $entries['XML']; 
        }
      }
    }
    
    /* Remove session ID. No one is interested in this... */
    unset($ret['SESSION_ID']);

    return($ret);
  }


  /*! \brief  Checks if the given ids are used queue ids.
    @param  Array   The ids we want to check..
    @return Array   An array containing all ids as index and TRUE/FALSE as value. 
   */
  public function ids_exist($ids)
  {
    if(!is_array($ids)){
      trigger_error("Requires an array as parameter.");
      return;
    }
    $this->reset_error();

    $ret = array();

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <connector>or</connector>";
    foreach($ids as $id){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <id>".$id."</id>
        </phrase>";
    }
    $xml_msg .= "</clause>
      </where>
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){
        foreach($entries['XML'] as $entry){
          if(isset($entry['ID'])){
            $ret[] = $entry['ID'];
          }
        }
      }
    }
    return($ret);
  }


  /*! \brief  Returns an entry containing all requested ids.
    @param  Array   The IDs of the entries we want to return.
    @return Array   Of the requested entries. 
   */
  public function get_entries_by_mac($macs)
  {
    if(!is_array($macs)){
      trigger_error("Requires an array as parameter.");
      return;
    }
    $this->reset_error();

    $ret = array();

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <connector>or</connector>";
    foreach($macs as $mac){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <macaddress>".$mac."</macaddress>
        </phrase>";
    }
    $xml_msg .= "</clause>
      </where>
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str); 
      if(isset($entries['XML'])){
        foreach($entries['XML'] as $name => $entry){
          if(preg_match("/^ANSWER[0-9]*$/",$name)){
            $ret[$name] = $entry;
          }
        }
      }
    }
    return($ret);
  }


  /*! \brief  Returns an entry containing all requested ids.
    @param  Array   The IDs of the entries we want to return.
    @return Array   Of the requested entries. 
   */
  public function get_entries_by_id($ids)
  {
    if(!is_array($ids)){
      trigger_error("Requires an array as parameter.");
      return;
    }
    $this->reset_error();

    $ret = array();

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <connector>or</connector>";
    foreach($ids as $id){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <id>".$id."</id>
        </phrase>";
    }
    $xml_msg .= "</clause>
      </where>
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str); 
      if(isset($entries['XML'])){
        foreach($entries['XML'] as $name => $entry){
          if(preg_match("/^ANSWER[0-9]*$/",$name)){
            $ret[$name] = $entry;
          }
        }
      }
    }
    return($ret);
  }


  /*! \brief  Checks if the given id is in use.
    @param  Integer The ID of the entry.
    @return Boolean TRUE if entry exists. 
   */
  public function id_exists($id)
  {
    if(!is_numeric($id)){
      trigger_error("Requires an integer as parameter.");
      return;
    }

    $this->reset_error();

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <phrase>
      <operator>eq</operator>
      <id>".$id."</id>
      </phrase>
      </clause>
      </where>
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return(FALSE);
      }

      $entries = $this->xml_to_array($str); 
      if( isset($entries['XML']['HEADER']) && 
          $entries['XML']['HEADER']=="answer" && 
          isset($entries['XML']['ANSWER1'])){
        return(TRUE);
      }
    }
    return(FALSE);
  }


  /*! \brief  Returns an entry from the gosaSupportQueue
    @param  Integer The ID of the entry we want to return.
    @return Array   Of the requested entry. 
   */
  public function get_entry_by_id($id)
  {
    if(!is_numeric($id)){
      trigger_error("Requires an integer as parameter.");
      return;
    }
    $this->reset_error();
  
    $ret = array();
    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <phrase>
      <operator>eq</operator>
      <id>".$id."</id>
      </phrase>
      </clause>
      </where>
      </xml>";
    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str); 
      if( isset($entries['XML']['HEADER']) &&
          $entries['XML']['HEADER']=="answer" &&
          isset($entries['XML']['ANSWER1'])){
        $ret = $entries['XML']['ANSWER1'];
      }
    }
    return($ret);
  }


  /*! \brief  Removes a set of entries from the GOsa support queue. 
    @param  Array The IDs to remove.
    @return Boolean True on success.
   */
  public function remove_entries($ids)
  {
    if(!is_array($ids)){
      trigger_error("Requires an array as parameter.");
      return;
    }

    $this->reset_error();

    $ret = array();

    $xml_msg = "<xml>
      <header>gosa_delete_jobdb_entry</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <connector>or</connector>";
    foreach($ids as $id){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <id>".$id."</id>
        </phrase>";
    }
    $xml_msg .= "</clause>
      </where>
      </xml>";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = $this->o_sock->read();

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return($ret);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) || isset($entries['COUNT'])){
        new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::remove_entries()", $ids,"SUCCESS");
        return(TRUE);
      }else{
        new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::remove_entries()", $ids,"FAILED ".$this->get_error());
      }
    }
    return(FALSE);
  }



  /*! \brief  Removes an entry from the GOsa support queue. 
    @param  Integer The ID of the entry we want to remove.
    @return Boolean True on success.
   */
  public function remove_entry($id)
  {
    return($this->remove_entries(array($id)));
  }


  /*! \brief  Parses the given xml string into an array 
    @param  String XML string  
    @return Array Returns an array containing the xml structure. 
   */
  private function xml_to_array($xml)
  {
    $params = array();
    $level = array();
    $parser  = xml_parser_create_ns();
    xml_parse_into_struct($parser, $xml, $vals, $index);

    $err_id = xml_get_error_code($parser);
    if($err_id){
      xml_parser_free($parser);
    }else{
      xml_parser_free($parser);

      foreach ($vals as $xml_elem) {
        if ($xml_elem['type'] == 'open') {
          if (array_key_exists('attributes',$xml_elem)) {
            list($level[$xml_elem['level']],$extra) = array_values($xml_elem['attributes']);
          } else {
            $level[$xml_elem['level']] = $xml_elem['tag'];
          }
        }
        if ($xml_elem['type'] == 'complete') {
          $start_level = 1;
          $php_stmt = '$params';
          while($start_level < $xml_elem['level']) {
            $php_stmt .= '[$level['.$start_level.']]';
            $start_level++;
          }
          $php_stmt .= '[$xml_elem[\'tag\']] = $xml_elem[\'value\'];';
          @eval($php_stmt);
        }
      }
    }

    if(!isset($params['XML'])){
      if (!array_key_exists('XML', $params)){
        $this->set_error(_("Cannot not parse XML!"));
      }
      $params = array("COUNT" => 0);
    }

    return($params); 
  }


  /*! \brief  Updates an entry with a set of new values, 
    @param  Integer The ID of the entry, we want to update.
    @param  Array   The variables to update.   
    @return Boolean Returns TRUE on success. 
   */
  public function update_entries($ids,$data)
  {
    $this->reset_error();
    if(!is_array($ids)){
      trigger_error("Requires an array as first parameter.");
      return;
    }

    if(!is_array($data)){
      trigger_error("Requires an array as second parameter.");
      return;
    }

    $attr = "";
    foreach($data as $key => $value){
      if(is_array($value)){
        foreach($value as $sub_value){
          $attr.= "<$key>".strtolower($sub_value)."</$key>\n";
        }
      }else{
        $attr.= "<$key>".strtolower($value)."</$key>\n";
      }
    }

    $xml_msg = "<xml>
      <header>gosa_update_status_jobdb_entry</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
      <connector>or</connector>";
    foreach($ids as $id){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <id>".$id."</id>
        </phrase>";
    }
    $xml_msg .= "</clause>
      </where>
      <update>
      ".$attr." 
      </update>
      </xml>";

    if($this->connect()){

      $this->o_sock->write($xml_msg);
      $str      = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return(FALSE);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        if(isset($entries['XML']['ERROR_STRING'])) {
          $this->set_error($entries['XML']['ERROR_STRING']);
          new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::update_entries()", $ids,"FAILED setting (".$attr.") error was ".$this->get_error());
          return(FALSE);
        }
        new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::update_entries()", $ids,"SUCCESS");
        return(TRUE);
      }
    }
    return(FALSE);
  }


  /*! \brief  Returns the number of currently queued objects.
      @return Integer  
   */
  public function number_of_queued_entries($event_types)
  {
    $tags = "";
    foreach($event_types as $type){
      $tags .= "<phrase><headertag>".$type."</headertag></phrase>";
    }
    if(count($event_types) > 1){
      $tags = "<connector>or</connector>".$tags;
    }
    if(count($event_types)){
      $tags = "<where><clause>".$tags."</clause></where>";
    }


    $xml_msg =
      "<xml>".
      "<header>gosa_query_jobdb</header>".
      "<target>GOSA</target>".
      "<source>GOSA</source>".
      "<select> count ID</select>".
      $tags.
      "</xml>";

    $xml_msg ="<xml><header>gosa_count_jobdb</header><target>GOSA</target><source>GOSA</source></xml>";
    $this->connect();
    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str     = trim($this->o_sock->read());

      /* Check if something went wrong while reading */
      if($this->o_sock->is_error()){
        $this->set_error($this->o_sock->get_error());
        return(0);
      }

      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        return($entries['XML']['COUNT']);
      }
    }
    return(-1);
  } 


  public function send_data($header, $to, $data= array(), $answer_expected = FALSE)
  {
    $xml_message= "";

    /* Prepare data */
    foreach ($data as $key => $value){
      if(is_array($value)){
        foreach($value as $sub_value){
          $xml_message.= "<$key>$sub_value</$key>";
        }
      }else{
        $xml_message.= "<$key>$value</$key>";
      }
    }

    /* Multiple targets? */
    if (!is_array($to)){
      $to_targets= array($to);
    } else {
      $to_targets= $to;
    }

    /* Build target strings */
    $target ="";
    foreach($to_targets as $to){
      $target.= "<target>$to</target>";
    }

    return $this->_send("<xml><header>$header</header><source>GOSA</source>$target".$xml_message."</xml>",$answer_expected);
  }


  /* Allows simply appending a new DaemonEvent 
   */
  public function append($event)
  {
    if(!($event instanceof DaemonEvent)){
      return(FALSE);
    }
  
    $this->reset_error();

    /* Add to queue if new 
     */
    if($event->is_new()){

      $request_answer = FALSE;
      if($event->get_type() == SCHEDULED_EVENT){
        $action = $event->get_schedule_action();
      }elseif($event->get_type() == TRIGGERED_EVENT){
        $action = $event->get_trigger_action();
      }else{
        trigger_error("Unknown type of queue event given.");
        return(FALSE);
      }

      /* Get event informations, like targets..
       */
      $targets    = $event->get_targets();
      $data       = $event->save();

      /* Append an entry for each target 
       */
      foreach($targets as $target){
        $data['macaddress'] = $target;
        $this->send_data($action,$target,$data,$request_answer);

        if($this->is_error()){
          return(FALSE);
        }
      }
      return(TRUE);
    }else{

      /* Updated edited entry.
       */
      $id                 = $event->get_id();
      $data               = $event->save();
      return($this->update_entries(array($id),$data));
    }

    return(FALSE);
  }


/*! \brief  Returns an array containing all queued entries.
    @return Array All queued entries as an array.
   */
  public function _send($data, $answer_expected= FALSE)
  {
    $this->reset_error();
    $ret = array();

    if($this->connect()){
      $this->o_sock->write($data);
      if ($answer_expected){
        $str = trim($this->o_sock->read());

        /* Check if something went wrong while reading */
        if($this->o_sock->is_error()){
          $this->set_error($this->o_sock->get_error());
          return($ret);
        }

        $entries = $this->xml_to_array($str);
        if(isset($entries['XML']) && is_array($entries['XML'])){
          $ret = $entries;
          if(isset($entries['XML']['ERROR_STRING'])) {
            $this->set_error($entries['XML']['ERROR_STRING']);
            new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::_send()", array($data=>$data),"FAILED ".$this->get_error());
          }else{
            new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::_send()", array($data=>$data),"SUCCESS");
          }
        }
      }else{
        new log("debug","DaemonEvent (IDS) ", "gosaSupportDaemon::_send()", array($data=>$data),"Fire & forget, not result.! ".$this->get_error());
      }
    }
    return($ret);
  }


  static function send($header, $to, $data= array(), $answer_expected = FALSE)
  {
    $xml_message= "";

    /* Get communication object */
    $d= new gosaSupportDaemon(TRUE,10);

    /* Prepare data */
    foreach ($data as $key => $value){
      if(is_array($value)){
        foreach($value as $sub_val){
          $xml_message.= "<$key>$sub_value</$key>";
        }
      }else{
        $xml_message.= "<$key>$value</$key>";
      }
    }

    /* Multiple targets? */
    if (!is_array($to)){
      $to_targets= array($to);
    } else {
      $to_targets= $to;
    }

    /* Build target strings */
    $target ="";
    foreach($to_targets as $to){
      $target.= "<target>$to</target>";
    }

    return $d->_send("<xml><header>$header</header><source>GOSA</source>$target".$xml_message."</xml>",$answer_expected);
  }


  /*! \brief  Removes all jobs from the queue that are tiggered with a specific macAddress.
      @param  String  $mac  The mac address for which we want to remove all jobs.      
   */
  function clean_queue_from_mac($mac)
  {
    global $config;

    /* First of all we have to check which jobs are startet 
     *  for $mac 
     */
    $xml_msg ="<xml><header>gosa_query_jobdb</header><target>GOSA</target><source>GOSA</source><where><clause><phrase><macaddress>".$mac."</macaddress></phrase></clause></where></xml>";  
    
    new log("debug","DaemonEvent ", "gosaSupportDaemon::clean_queue_from_mac()", array($mac => $mac)," start cleaning.");
 
    $data = $this->_send($xml_msg,TRUE);
    if(is_array($data) && isset($data['XML'])){
      $already_aborted = FALSE;
      foreach($data['XML']  as $name => $entry){
        if(preg_match("/answer[0-9]*/i",$name)){
          $entry['STATUS'] = strtoupper($entry['STATUS']);
          switch($entry['STATUS']){

            case 'PROCESSING' :

              /* Send abort event, but only once 
               */
              if($already_aborted){
                break;
              }elseif(class_available("DaemonEvent_faireboot")){
                $already_aborted = TRUE;
                $tmp = new DaemonEvent_faireboot($config);
                $tmp->add_targets(array($mac));
                $tmp->set_type(TRIGGERED_EVENT);
                if(!$this->append($tmp)){
                  msg_dialog::display(_("Error"), sprintf(_("Cannot send abort event for entry %s!"),$entry['ID']) , ERROR_DIALOG);
                  new log("debug","DaemonEvent ", "gosaSupportDaemon::clean_queue_from_mac()", array($mac => $mac),
                      "FAILED, could not send 'DaemonEvent_faireboot' for entry ID (".$entry['ID'].") - ".$this->get_error());
                }else{
                  new log("debug","DaemonEvent ", "gosaSupportDaemon::clean_queue_from_mac()", array($mac => $mac),
                      "SUCCESS, send 'DaemonEvent_faireboot' for entry ID (".$entry['ID'].")");
                }
                ;break;
              }else{
                /* Couldn't find abort event, just remove entry */
              }

            case 'WAITING':
            case 'ERROR':
            default :
            
              /* Simply remove entries from queue. 
               *  Failed or waiting events, can be removed without any trouble.
               */ 
              if(!$this->remove_entries(array($entry['ID']))){
                msg_dialog::display(_("Error"), sprintf(_("Cannot remove entry %s!"),$entry['ID']) , ERROR_DIALOG);
              }
              ;break;
          }
    
        }
      }
    }
  }


static function ping($target)
{
  if (tests::is_mac($target)){
    /* Get communication object */
    $d= new gosaSupportDaemon(TRUE,0.5);
    $answer= $d->_send("<xml><header>gosa_ping</header><source>GOSA</source><target>$target</target></xml>", TRUE);
    return (count($answer) ? TRUE:FALSE);
  }

  return (FALSE);
}

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
