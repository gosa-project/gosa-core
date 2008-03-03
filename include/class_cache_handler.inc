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
