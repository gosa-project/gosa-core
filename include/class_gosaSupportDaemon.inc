<?php


/*
   __construct        - Create a new deamon handle. 
   connect            - Connect to deamon socket.
   disconnect         - Disconnect from socket.
   set_error          - Sets a new error.
   is_error           - Returns TRUE if there was an error.
   get_error          - Returns the last error or "".
   get_queued_entries - Returns all queued entries, with limitations.
   ids_exist          - Checks if the given id exists.
   get_entries_by_id  - Returns a set of entries.
   id_exists          - Checks if a set entries exists.
   get_entry_by_id    - Returns a single entry.
   remove_entries     - Remove a set of entries.
   remove_entry       - Removes a single entry.
   update_entry       - Updates a single entry.
   xml_to_array       - XML to Array. 
*/



class gosaSupportDaemon
{
  private $o_sock       = NULL;
  private $s_host       = "";
  private $i_port       = 0;
  private $f_timeout    = 0.2;

  private $is_connected     = FALSE;
  private $s_encryption_key = "";

  private $s_error  = "";
  private $b_error  = FALSE;


  /*! \brief  Creates a new gosaSupportDaemon object.
    @param string   Host    The Host where the deamon is running on.  
    @param integer  Port    The port which the deamon use.
    @param string   Key     The encryption string.
    @param boolean  Connect Directly connect to deamon socket.
    @param float    Timeout The timelimit for all socket actions.
   */
  public function __construct($host,$port,$key="secret-gosa-password",$connect=TRUE,$timeout=0.2)
  {
    $this->s_host    = $host;
    $this->i_port    = $port;
    $this->f_timeout = $timeout;
    $this->s_encryption_key = $key;
    if($connect){
      $this->connect();
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
      $this->error = $this->o_sock->get_error();
      $this->is_connected = FALSE;
    }
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
  public function get_queued_entries($from=0,$to=10)
  {
    $this->b_error = FALSE;
    $this->s_error = "";

    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <where>
      <clause>
      <connector>and</connector>
      <phrase>
      <operator>gt</operator>
      <ROWID>-1</ROWID>
      </phrase>
      </clause>
      </where>
      <limit>
      <from>".$from."</from>
      <to>".$to."</to>
      </limit>
      </xml>";

    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str);

      if(!array_key_exists("XML",$entries)){
        $this->set_error("!!!Couldn't parse xml.");
        $this->disconnect();
        return;
      }else{
        if(!is_array($entries['XML'])) {
          $ret = array();
        }else{
          $ret = $entries['XML']; 
        }
      }
      return($ret);
    }
    $this->set_error("Could not establish socket connection.");
    $this->disconnect();
    return;
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

    $this->connect();
    if(!$this->is_connected){
      $this->set_error("Could not establish socket connection.");
    }else{
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str); 
      if(isset($entries['XML'])){
        foreach($entries['XML'] as $entry){
          $ret[] = $entry['ID'];
        }
        $this->disconnect();
        return($ret);
      }
    }
    $this->disconnect();
    return(FALSE);
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
      <where>
      <clause>
      <connector>or</connector>";
    foreach($ids as $id){
      $xml_msg .= "<phrase>
        <operator>eq</operator>
        <id>".$id."</id>
        </phrase>";
      $ret[$id] = FALSE;
    }
    $xml_msg .= "</clause>
      </where>
      </xml>";

    $this->connect();
    if(!$this->is_connected){
      $this->set_error("Could not establish socket connection.");
    }else{
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str); 
      if(!isset($entries['XML'])){
        $this->set_error("Entry with id (".$id.") not found.");
        $this->disconnect();
      }else{
        $ret = $entries['XML'];
        return($ret);
      }
    }
    return;
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
      <where>
      <clause>
      <phrase>
      <operator>eq</operator>
      <id>".$id."</id>
      </phrase>
      </clause>
      </where>
      </xml>";
    $this->connect();
    if(!$this->is_connected){
      $this->set_error("Could not establish socket connection.");
    }else{
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str); 
      if(isset($entries['XML']['ANSWER1'])){
        $this->disconnect();
        return(TRUE);
      }
    }
    $this->disconnect();
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
    $xml_msg = "<xml>
      <header>gosa_query_jobdb</header>
      <where>
      <clause>
      <phrase>
      <operator>eq</operator>
      <id>".$id."</id>
      </phrase>
      </clause>
      </where>
      </xml>";
    $this->connect();
    if(!$this->is_connected){
      $this->set_error("Could not establish socket connection.");
    }else{
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str); 
      if(!isset($entries['XML']['ANSWER1'])){
        $this->set_error("Entry with id (".$id.") not found.");
        $this->disconnect();
      }else{
        $ret = $entries['XML']['ANSWER1'];
        return($ret);
      }
    }
    return;
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

    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      return(TRUE);
    }
    $this->set_error("Could not establish socket connection.");
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
      <where>
      <clause>
      <phrase>
      <operator>eq</operator>
      <id>".$id."</id>
      </phrase>
      </clause>
      </where>
      </xml>";
    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      return(TRUE);
    }
    $this->set_error("Could not establish socket connection.");
    return(FALSE);
  }


  /*! \brief  Parses the given xml string into an array 
    @param  String XML string  
    @return Array Returns an array containing the xml structure. 
   */
  function xml_to_array($xml)
  {
    $params = array();
    $level = array();
    $parser  = xml_parser_create_ns();
    xml_parse_into_struct($parser, $xml, $vals, $index);

    $err_id = xml_get_error_code($parser);
    if($err_id){
      $this->set_error(xml_error_string(xml_get_error_code($parser)));
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
    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      $str      = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str);
      if(!empty($str)){
        return(TRUE);
      }
      return(FALSE);
    }
    $this->set_error("Could not establish socket connection.");
    return(FALSE);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
