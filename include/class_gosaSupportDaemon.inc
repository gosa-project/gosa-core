<?php

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
    $this->o_sock->setEncryptionKey($this->s_encryption_key); 
    if($this->o_sock->connected()){ 
      $this->is_connected = TRUE;
    }else{
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
  public function get_queued_entries()
  {
    $this->b_error = FALSE;
    $this->s_error = "";

    $xml_msg = "<xml><header>gosa_query_jobdb</header><where><status>*</status></where></xml>";
    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str);
      if(!isset($entries['XML'])){
        $this->set_error("Couldn't parse xml.");
        $this->disconnect();
        return;
      }else{
        $ret = array_values($entries['XML']); 
      }
      return($ret);
    }
    $this->set_error("Could not establish socket connection.");
    $this->disconnect();
    return;
  }


  /*! \brief  Checks if the given id is in use.
      @param  Integer The ID of the entry.
      @return Boolean TRUE if entry exists. 
   */
  public function id_exists($id)
  {
    if(!is_numeric($id)){
      trigger_error("gosaSupportDaemon::get_entry() requires an integer value as ID parameter.");
      return;
    }

    $this->b_error = FALSE;
    $this->s_error = "";
    $xml_msg = "<xml>
                  <header>gosa_query_jobdb</header>
                  <where>id</where>
                  <id>".$id."</id>
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
  public function get_entry($id)
  {
    if(!is_numeric($id)){
      trigger_error("gosaSupportDaemon::get_entry() requires an integer value as ID parameter.");
      return;
    }

    $this->b_error = FALSE;
    $this->s_error = "";
    $xml_msg = "<xml>
                  <header>gosa_query_jobdb</header>
                  <where>id</where>
                  <id>".$id."</id>
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
                  <where>id</where>
                  <id>".$id."</id>
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
          eval($php_stmt);
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
  public function update_entry($id,$entry)
  {
    $this->b_error = FALSE;
    $this->s_error = "";
    if(!is_numeric($id)){
      trigger_error("Requires an integer value as ID parameter.");
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
                    <id>".$id."</id> 
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
