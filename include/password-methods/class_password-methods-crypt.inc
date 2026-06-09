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

define("CRYPT_DES_AVAIL", 0x0); // Starts with nothing. All libcrypt supports des. Any number & 0x0 == 0x0
define("CRYPT_BSDI_AVAIL", 0x1); // starts with "_"
define("CRYPT_MD5_AVAIL", 0x2); // starts with "$1$"
define("CRYPT_NT_AVAIL", 0x4); // starts with "$3$"
define("CRYPT_SHA1_AVAIL", 0x8); // starts with "$sha1"
define("CRYPT_BLOWFISH_AVAIL", 0x10); // starts with "$2$". obsolete in most dists
define("CRYPT_BLOWFISH_A_AVAIL", 0x20); // starts with "$2a$"
define("CRYPT_BLOWFISH_X_AVAIL", 0x40); // starts with "$2x$"
define("CRYPT_BLOWFISH_B_AVAIL", 0x80); // starts with "$2b$"
define("CRYPT_BLOWFISH_Y_AVAIL", 0x80); // starts with "$2y$". equivalent to "$2b$"
define("CRYPT_SHA256_AVAIL", 0x100); // starts with "$5$"
define("CRYPT_SHA512_AVAIL", 0x200); // starts with "$6$"
define("CRYPT_SCRYPT_AVAIL", 0x400); // starts with "$7$"
define("CRYPT_YESCRYPT_AVAIL", 0x800); // starts with "$y$"
define("CRYPT_GOST_YESCRYPT_AVAIL", 0x1000); // starts with "$gy$"

/*
 * Yescrypt is only available in libxcrypt. Not all distributions support it.
 * So I consider to add it next as crypt/unknown along with further methods.
 */

class passwordMethodCrypt extends passwordMethod
{
  function __construct($config)
  {
  }


  function is_available()
  {
    return function_exists("crypt");
  }

  function is_system_crypt_available()
  {
    return function_exists("syetem_crypt");
  }

  function is_system_crypt_gensalt_available()
  {
    return function_exists("crypt_gensalt");
  }

  function is_method_available(int $method)
  {
    static int $methods = 
      empty(crypt_gensalt('_', 0, NULL)) ? 0x0 : CRYPT_BSDI_AVAIL |
      empty(crypt_gensalt('$1$', 0, NULL)) ? 0x0 : CRYPT_MD5_AVAIL |
      empty(crypt_gensalt('$3$', 0, NULL)) ? 0x0 : CRYPT_NT_AVAIL |
      empty(crypt_gensalt('$sha1', 0, NULL)) ? 0x0 : CRYPT_SHA1_AVAIL |
      empty(crypt_gensalt('$2$', 0, NULL)) ? 0x0 : CRYPT_BLOWFISH_AVAIL |
      empty(crypt_gensalt('$2a$', 0, NULL)) ? 0x0 : CRYPT_BLOWFISH_A_AVAIL |
      empty(crypt_gensalt('$2x$', 0, NULL)) ? 0x0 : CRYPT_BLOWFISH_X_AVAIL |
      empty(crypt_gensalt('$2b$', 0, NULL)) ? 0x0 : CRYPT_BLOWFISH_B_AVAIL |
      empty(crypt_gensalt('$5$', 0, NULL)) ? 0x0 : CRYPT_SHA256_AVAIL |
      empty(crypt_gensalt('$6$', 0, NULL)) ? 0x0 : CRYPT_SHA512_AVAIL |
      empty(crypt_gensalt('$7$', 0, NULL)) ? 0x0 : CRYPT_SCRYPT_AVAIL |
      empty(crypt_gensalt('$y$', 0, NULL)) ? 0x0 : CRYPT_YESCRYPT_AVAIL |
      empty(crypt_gensalt('$gy$', 0, NULL)) ? 0x0 : CRYPT_GOST_YESCRYPT_AVAIL;
    return ($method & $methods == $method);
  }

  function create_template_hash($attrs)
  {
    return($this->generate_hash('N0T$3T4N0W').'N0T$3T4N0W');
  }


  function generate_hash($pwd)
  {
    bool $use_system_gensalt = is_system_crypt_gensalt_available();
    bool $use_system_crypt = is_system_crypt_available();
    if ($this->hash == "crypt/standard-des"){
      $salt = "";
      if($use_system_gensalt && is_method_available(CRYPT_DES_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        for ($i = 0; $i < 2; $i++) {
            $salt .= get_random_char();
        }
      }
    }

    if ($this->hash == "crypt/enhanced-des"){
      $salt = "_";
      if($use_system_gensalt && is_method_available(CRYPT_BSDI_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        for ($i = 0; $i < 8; $i++) {
            $salt .= get_random_char();
        }
      }
    }

    if ($this->hash == "crypt/md5"){
      $salt = "\$1\$";
      if($use_system_gensalt && is_method_available(CRYPT_MD5_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        for ($i = 0; $i < 8; $i++) {
            $salt .= get_random_char();
        }
        $salt .= "\$";
      }
    }

    if ($this->hash == "crypt/blowfish"){
      $salt = "\$2a\$";
      if($use_system_gensalt && is_method_available(CRYPT_BLOWFISH_A_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        $salt .= "07\$" // crypt_gensalt() will ignore this part
        for ($i = 0; $i < CRYPT_SALT_LENGTH; $i++) {
            $salt .= get_random_char();
        }
        $salt .= "\$";
      }
    }

    if ($this->hash == "crypt/sha256"){
      $salt = "\$5\$";
      if($use_system_gensalt && is_method_available(CRYPT_SHA256_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        for ($i = 0; $i < 16; $i++) {
            $salt .= get_random_char();
        }
        $salt .= "\$";
      }
    }

    if ($this->hash == "crypt/sha512"){
      $salt = "\$6\$";
      if($use_system_gensalt && is_method_available(CRYPT_SHA512_AVAIL)){
        $salt = crypt_gensalt($salt, 0, NULL);
      }else{
        for ($i = 0; $i < 16; $i++) {
            $salt .= get_random_char();
        }
        $salt .= "\$";
      }
    }

    if($this->hash == "crypt/unknown" && $use_system_crypt && $use_system_gensalt){
      $salt = crypt_gensalt(NULL, 0, NULL);
    }

    return $use_system_crypt ? "{CRYPT}".system_crypt($pwd, $salt) : "{CRYPT}".crypt($pwd, $salt);
  }


  function get_hash_name()
  {
    $hashes= array();
    if (CRYPT_STD_DES == 1) {
      $hashes[]= "crypt/standard-des";
    }

    if (CRYPT_EXT_DES == 1) {
      $hashes[]= "crypt/enhanced-des";
    }

    if (CRYPT_MD5 == 1) {
      $hashes[]= "crypt/md5";
    }

    if (CRYPT_BLOWFISH == 1) {
      $hashes[]= "crypt/blowfish";
    }

    if (CRYPT_SHA256 == 1) {
      $hashes[]= "crypt/sha256";
    }

    if (CRYPT_SHA512 == 1) {
      $hashes[]= "crypt/sha512";
    }

    if (CRYPT_UNKNOWN == 1) {
      $hashes[]= "crypt/unknown";
    }
    return $hashes;
  }


  function _extract_method($password_hash)
  {
    if (!preg_match('/^{crypt}/i', $password_hash)){
      return "";
    }

    $password_hash= preg_replace('/^{[^}]+}!?/', '', $password_hash);

    if (preg_match("/^[a-zA-Z0-9.\/][a-zA-Z0-9.\/]/", $password_hash)){
      return "crypt/standard-des";
    }

    if (preg_match("/^_[a-zA-Z0-9.\/]/", $password_hash)){
      return "crypt/enhanced-des";
    }
    
    if (preg_match('/^\$1\$/', $password_hash)){
      return "crypt/md5";
    }

    if (preg_match('/^(\$2\$|\$2a\$)/', $password_hash)){
      return "crypt/blowfish";
    }

    if (preg_match('/^\$5\$/', $password_hash)){
      return "crypt/sha256";
    }

    if (preg_match('/^\$6\$/', $password_hash)){
      return "crypt/sha512";
    }
    return "crypt/unknown";
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
