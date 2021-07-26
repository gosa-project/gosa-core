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

class Socket_Client
{
  private $host 	= "";
  private $port 	= "";
  private $timeout= "";
  private $errno	= "";
  private $errstr	= "";
  private $b_data_send = FALSE;
  private $handle	= NULL;
  private $bytes_read = 0;
  private $error = "";
  private $is_error   = FALSE;
  private $b_encrypt  = FALSE;

  /* Crypto information */
  private $ckey= "";
  private $iv;
  private $openssl_cipher = "aes-256-cbc";
  private $iv_len = -1;


  public function __construct($host, $port, $connect = TRUE, $timeout = 3){
    $this->host= $host;
    $this->port= $port;
    $this->timeout= $timeout;
    $this->reset_error();

    $this->iv_len = openssl_cipher_iv_length($openssl_cipher);

    /* Connect if needed */
    if($connect){
      $this->open();
    }
  }


  public function setEncryptionKey($key)
  {
    if ($this->connected()){
      $this->ckey = md5($key);
      $this->b_encrypt = TRUE;
    }

    return($this->b_encrypt);
  }


  private function encrypt($data)
  {
    if($this->b_encrypt){
      $data = base64_encode(openssl_encrypt($data, $this->openssl_cipher, $this->ckey, OPENSSL_RAW_DATA, $this->iv));
    }
    return($data);
  }


  private function decrypt($data)
  {
    /* decrypt data */
    if($this->b_encrypt && strlen($data)){
      $data = openssl_decrypt(base64_decode($data), $this->openssl_cipher, $this->ckey, OPENSSL_RAW_DATA | OPENSSL_ZERO_PADDING, $this->iv);
    }
    return($data);
  }


  public function connected()
  {
    return ($this->handle == TRUE);
  }


  public function open()
  {
    $this->reset_error();
    $this->handle = @fsockopen($this->host, $this->port, $this->errno, $this->errstr, $this->timeout);
    if(!$this->handle){
      $this->handle = NULL;
      $this->set_error(sprintf(_("Socket connection to %s:%s failed: %s"), bold($this->host), bold($this->port), $this->errstr));
    }else{
      $this->b_data_send = TRUE;

      /* Create the IV and determine the keysize length */
      $this->iv = substr(md5('GONICUS GmbH'),0, $this->iv_len);
    }
  }


  public function set_error($str)
  {
    $this->is_error =TRUE;
    $this->error=$str;
  }


  public function reset_error()
  {
    $this->is_error =FALSE;
    $this->error = "";
  }


  public function is_error()
  {
    return($this->is_error);
  }


  public function get_error()
  {
    return $this->error;
  }


  public function write($data){
    if($this->handle){
      $data = $this->encrypt($data);
      fputs($this->handle, $data."\n");
      $this->b_data_send = TRUE;
    }else{
      $this->b_data_send = FALSE;
    }

    return $this->b_data_send;
  }


  public function read()
  {
    // Output the request results
    $this->reset_error();
    $str = "";
    $data = "test";
    socket_set_timeout($this->handle,$this->timeout);			
    stream_set_blocking($this->handle,0);
    $start = microtime(TRUE);

    /* Read while 
     * nothing was read yet
     * the timelimit reached
     * there is not data left on the socket.
     */
    while(TRUE){
      usleep(10000);
      $data = fread($this->handle, 1024000);
      if($data && strlen($data)>0) {
        $str .= $data;
      } else {
        if(strlen($str) != 0){
          
          /* We got <xml> but </xml> is still missing, keep on reading */
          if(preg_match("/<\/xml>/",$this->decrypt($str))){
            break;
          }
        }
      }
      if((microtime(TRUE) - $start) > $this->timeout ){     
        $this->set_error(sprintf(_("Socket timeout of %s seconds reached!"), bold($this->timeout)));     
        break;
      }
    }
    $this->bytes_read = strlen($str);
    $this->b_data_send = FALSE;
    $str = $this->decrypt($str);
    return($str);	
  }


  public function bytes_read()
  {
    return $this->bytes_read;
  }


  public function close()
  {
    if($this->handle){
      fclose($this->handle);
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
