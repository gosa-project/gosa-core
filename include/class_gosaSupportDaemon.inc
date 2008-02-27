<?php
/*

   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2008  Fabian Hickert

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


Function overview:

   __construct              - Create a new deamon handle. 
   connect                  - Connect to deamon socket.
   disconnect               - Disconnect from socket.
   set_error                - Sets a new error.
   is_error                 - Returns TRUE if there was an error.
   get_error                - Returns the last error or "".
   get_queued_entries       - Returns all queued entries, with limitations.
   ids_exist                - Checks if the given id exists.
   get_entries_by_id        - Returns a set of entries.
   id_exists                - Checks if a set entries exists.
   get_entry_by_id          - Returns a single entry.
   remove_entries           - Remove a set of entries.
   remove_entry             - Removes a single entry.
   update_entries           - Updates a set of entries.
   xml_to_array             - XML to Array. 
   number_of_queued_entries - Returns the number of currently queued entries.   
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
    @param string   Host    The Host where the deamon is running on.  
    @param integer  Port    The port which the deamon use.
    @param string   Key     The encryption string.
    @param boolean  Connect Directly connect to deamon socket.
    @param float    Timeout The timelimit for all socket actions.
   */
  public function __construct($connect=TRUE,$timeout=0.2)
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


  /*! \brief  Establish deamon connection. 
    @return boolean Returns true if the connection was succesfully established. 
   */
  public function connect()
  {
    $this->o_sock = new Socket_Client($this->s_host,$this->i_port,TRUE,$this->f_timeout);
    if($this->o_sock->connected()){ 
      $this->o_sock->setEncryptionKey($this->s_encryption_key); 
      $this->is_connected = TRUE;
    }else{
      $this->s_error = $this->o_sock->get_error();
      $this->b_error = TRUE;
      $this->disconnect();
    }
    return($this->is_connected);
  }


  /*! \brief  Disconnect from gosa deamon.
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
    return($this->s_error);
  }


  /*! \brief  Returns an array containing all queued entries.
    @return Array All queued entries as an array.
   */
  public function get_queued_entries($from=-1,$to=-1,$sort="timestamp DESC")
  {
    $this->b_error = FALSE;
    $this->s_error = "";
    $ret = array();

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <target>GOSA</target>
      <source>GOSA</source>
      <where>
      <clause>
        <phrase>
        <operator>ne</operator>
        <HEADERTAG>*</HEADERTAG>
        </phrase>
      </clause>
      </where>
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
      $entries = $this->xml_to_array($str);
      if(isset($entries['XML']) && is_array($entries['XML'])){

        /* Check if returned values represent a valid answer */
        if($entries['XML']['HEADER'] == "answer"){
          
          /* Unset header tags */
          foreach(array("HEADER","SOURCE","TARGET") as $type){
            unset($entries['XML'][$type]);
          }
          $ret = $entries['XML']; 
        }
      }
    }
    
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
    $this->b_error = FALSE;
    $this->s_error = "";

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
  public function get_entries_by_id($ids)
  {
    if(!is_array($ids)){
      trigger_error("Requires an array as parameter.");
      return;
    }
    $this->b_error = FALSE;
    $this->s_error = "";

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

    $this->b_error = FALSE;
    $this->s_error = "";
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
  
    $this->b_error = FALSE;
    $this->s_error = "";
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
    $this->b_error = FALSE;
    $this->s_error = "";

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
    $this->b_error = FALSE;
    $this->s_error = "";

    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str = $this->o_sock->read();
      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        return(TRUE);
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
    $this->b_error = FALSE;
    $this->s_error = "";

    $xml_msg = "<xml>
      <header>gosa_delete_jobdb_entry</header>
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
      $str = $this->o_sock->read();
      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        return(TRUE);
      }
    }
    return(FALSE);
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
        $this->set_error(_("Could not parse XML."));
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
  public function update_entries($ids,$entry)
  {
    $this->b_error = FALSE;
    $this->s_error = "";
    if(!is_array($ids)){
      trigger_error("Requires an array as first parameter.");
      return;
    }

    if(!is_array($entry)){
      trigger_error("Requires an array as second parameter.");
      return;
    }

    $attr = "";
    foreach($entry as $name => $entry){
      $attr.="<".strtolower($name).">".$entry."</".strtolower($name).">\n";
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
      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        return(TRUE);
      }
    }
    return(FALSE);
  }


  /*! \brief  Returns the number of currently queued objects.
      @return Integer  
   */
  public function number_of_queued_entries()
  {
    $xml_msg ="<xml><header>gosa_count_jobdb</header><target>GOSA</target><source>GOSA</source></xml>";
    $this->connect();
    if($this->connect()){
      $this->o_sock->write($xml_msg);
      $str     = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str);
      if(isset($entries['XML'])){
        return($entries['XML']['COUNT']);
      }
    }
    return(-1);
  } 


  /*! \brief  Returns an array containing all queued entries.
    @return Array All queued entries as an array.
   */
  public function _send($data, $answer_expected= FALSE)
  {
    $this->b_error = FALSE;
    $this->s_error = "";
    $ret = array();

    if($this->connect()){
      $this->o_sock->write($data);
      if ($answer_expected){
        $str = trim($this->o_sock->read());
        $entries = $this->xml_to_array($str);
        if(isset($entries['XML']) && is_array($entries['XML'])){
          $ret = $entries; 
        }
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
      $xml_message.= "<$key>$value</$key>";
    }

    /* Multiple targets? */
    if (!is_array($to)){
	$to_targets= array($to);
    } else {
	$to_targets= $to;
    }

    /* Build target strings */
    foreach($to_target as $to){
	$target.= "<target>$to</target>";
    }

    return $d->_send("<xml><header>$header</header><source>GOSA</source>$target".$xml_message."</xml>",$answer_expected);
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
