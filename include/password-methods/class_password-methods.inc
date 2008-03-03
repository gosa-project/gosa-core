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

class passwordMethod
{
  var $config = false;
  var $attrs= array();

  // Konstructor
  function passwordMethod($config)
  {
  }


  function get_hash_name()
  {
  }


  // Loads Methods in annother way as  get_available_methods do, (For setup ..)
  // and loads them,.
  #FIXME: This stopped working after moving around pw-methods
  function get_available_methods_if_not_loaded($path_to_load="../include")
  {
    $oh = opendir($path_to_load);
    $i = 0; 
    $ret = false;
    while ($file = readdir($oh)) {
      $one = strtolower($file); 
      if((strstr($one,"class_password-methods-" ))&&($one[0]!=".")){
        require_once($file);
      }
    }
    return(passwordMethod::get_available_methods());
  }


  // Crypts a single string, with given Method
  function crypt_single_str($string,$method)
  {
    $available = passwordMethod::get_available_methods();
    if(!is_array($available))
    {
      $available = passwordMethod::get_available_methods_if_not_loaded();
    }

    $test = new  $available[$method](false);
    $newpass =  $test->generate_hash($string);
    return( $newpass); 
  }


  // this function returns all loaded classes for password encryption
  static function get_available_methods()
  {
    global $class_mapping, $config;
    $ret =false;
    $i =0;
    foreach($class_mapping as $class => $path) {
      if(preg_match('/passwordMethod/i', $class) && !preg_match("/^passwordMethod$/i", $class)){
        $name = preg_replace ("/passwordMethod/i", "", $class);
        $test = new $class($config);
        if($test->is_available()) {
          $plugname= $test->get_hash_name();
          $ret['name'][$i]= $plugname;
          $ret['class'][$i]=$class;
          $ret[$i]['name']= $plugname;
          $ret[$i]['class']= $class;
          $ret[$plugname]=$class;                    
          $i++;
        }
      }
    }
    return($ret);
  }
  

  // Method to let password backends remove additional information besides
  // the userPassword attribute
  function remove_from_parent()
  {
  }


  // Method to let passwords backends manage additional information
  // besides the userAttribute entry
  function set_password($password)
  {
  }


  // Return true if this password method provides a configuration dialog
  function is_configurable()
  {
    return FALSE;
  }


  // Provide a subdialog to configure a password method
  function configure()
  {
    return "";
  }

  
  // Save information to LDAP
  function save($dn)
  {
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
