<?php

class gosaSupportDaemon
{
  private $o_sock       = NULL;
  private $s_host       = "";
  private $i_port       = 0;
  private $i_timeout    = 1;

  private $is_connected     = FALSE;
  private $s_encryption_key = "";

  private $entries  = array();
  private $pointer  = 0;

  private $s_error  = "";
  private $b_error  = FALSE;

  public function __construct($host,$port,$key="secret-gosa-password",$connect=TRUE,$timeout=1)
	{
    $this->s_host    = $host;
    $this->i_port    = $port;
    $this->i_timeout = $timeout;
    $this->s_encryption_key = $key;
  }
  
  public function connect()
  {
    $this->o_sock = new Socket_Client($this->s_host,$this->i_port,TRUE,$this->i_timeout);
    $this->o_sock->setEncryptionKey($this->s_encryption_key); 
    if($this->o_sock->connected()){ 
      $this->is_connected = TRUE;
    }else{
      $this->is_connected = FALSE;
    }
  }

  public function disconnect()
  {
    $this->o_sock->close();
    $this->is_connected = FALSE;
  }


  public function __reload()
  {
    echo "<b>Reload</b>";
    $this->entries = array();
    $xml_msg = "<xml><header>gosa_query_jobdb</header><where><status>*</status></where></xml>";
    $this->connect();
    if($this->is_connected){
      $this->o_sock->write($xml_msg);
      $str = trim($this->o_sock->read());
      $entries = $this->xml_to_array($str);
      if(!isset($entries['XML'])){
        print_a($entries);
        echo htmlentities($str);
        $this->set_error("Couldn't parse xml.");
        $this->disconnect();
        return(FALSE);
      }else{
        $ret = array_values($entries['XML']); 
      }
      $this->entries = $ret;
      $this->disconnect();
      return(TRUE);
    }
    $this->set_error("Could not establish socket connection.");
    return(FALSE);
  }

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

  public function load()
  {
    return($this->__reload());
  }

  public function fetch()
  {
    if(isset($this->entries[$this->pointer])){
      $p = $this->entries[$this->pointer];
      $this->pointer ++;
      return($p);
    }
    return(FALSE);
  }

  private function set_error($str)
  {
    $this->b_error = TRUE;
    $this->s_error = $str;
  }

  public function is_error()
  {
    return($this->b_error);
  }

  public function get_error()
  {
    return($this->s_error);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
