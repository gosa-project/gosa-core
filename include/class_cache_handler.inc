<?php

class gosa_cache 
{
  var $c_memcache = NULL;
  var $b_connected= FALSE; 

  function __construct()
  {
    $this->connect();
  }

  
  function connect()
  {
    $this->close();
    if(class_exists("Memcache")){
      $this->c_memcache = new Memcache;
      $res = $this->c_memcache->pconnect("localhost",11211);
      if(!$res){
        $this->b_connected = FALSE;
        $this->c_memcache  = NULL;
      }else{
        $this->b_connected = TRUE;
      }
    }
  }  

  function close()
  {
    if($this->b_connected){
      $this->c_memcache->close();
    }
  }
  

  function save($key,$value)
  {
    if($this->b_connected){
      $this->c_memcache->set($key,$value);
    }
  }

  function load($key)
  {
    if($this->b_connected){
      return($this->c_memcache->get($key));
    }
  }

  function remove($key)
  {
    if($this->b_connected){
      return($this->c_memcache->delete($key));
    }
  }

  function status()
  {
    if($this->b_connected){ 
      return($this->c_memcache->getStats());
    }else{
      return("");
    }
  }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
