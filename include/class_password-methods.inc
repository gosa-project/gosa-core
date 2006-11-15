<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004  Cajus Pollmeier

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
 */


class passwordMethod
{
  var $config = false;
  var $attrs= array();

  // Konstructor
  function passwordMethod($config)
  {
  }

  // Loads Methods in annother way as  get_available_methods do, (For setup ..)
  // and loads them,.
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
  function get_available_methods()
  {
    $ret =false;
    $all = get_declared_classes();
    $i = 0;
    foreach($all as $one) {
      if(preg_match('/passwordMethod/i', $one) && !preg_match("/^passwordMethod$/i", $one)){
        $name = preg_replace ("/passwordMethod/i", "", $one);
        $test = new $one(false);
        if($test->is_available()) {
          $plugname= strtolower(preg_replace ("/passwordMethod/i","",$one));
          $ret['name'][$i]= $plugname;
          $ret['class'][$i]=$one;
          $ret[$i]['name']= $plugname;
          $ret[$i]['class']= $one;
          $ret[$plugname]=$one;                    
          $i++;
        }
      }
    }
    return($ret);
  }

}

// change_password, changes the Password, of the given dn
function change_password ($dn, $password, $mode=0, $hash= "")
{
  global $config;
  $newpass= "";

  // Get all available encryption Methods 

  // NON STATIC CALL :)
  $tmp = new passwordMethod($_SESSION['config']);
  $available = $tmp->get_available_methods();

  // read current password entry for $dn, to detect the encryption Method
  $ldap       = $config->get_ldap_link();
  $ldap->cat ($dn, array("shadowLastChange", "userPassword", "uid"));
  $attrs      = $ldap->fetch ();

  // Set encryption type to clear if required 
  if (isset($attrs['userPassword'][0]) && preg_match('/^[^{}]+$/', $attrs['userPassword'][0]) && $hash == ""){
    $hash= "clear";
  }

  // Detect the encryption Method
  if ( (isset($attrs['userPassword'][0]) &&  preg_match ("/^{([^}]+)}(.+)/", $attrs['userPassword'][0], $matches)) ||  $hash != ""){

    /* Check for supported algorithm */
    mt_srand((double) microtime()*1000000);

    /* Extract used hash */
    if ($hash == ""){
      $hash= strtolower($matches[1]);
    }


    // Crypt with the detected Method
    $test = new  $available[$hash]($config);
    $test->attrs= $attrs;
    $newpass =  $test->generate_hash($password);

  } else {
    // Crypt it by default 
    $test = new  $available['md5']($config);
    $newpass =  $test->generate_hash($password);
  }

  // Update shadow timestamp? 
  if (isset($attrs["shadowLastChange"][0])){
    $shadow= (int)(date("U") / 86400);
  } else {
    $shadow= 0;
  }

  // Write back modified entry 
  $ldap->cd($dn);
  $attrs= array();

  // Not for groups 
  if ($mode == 0){

    if ($shadow != 0){
      $attrs['shadowLastChange']= $shadow;
    }

    // Create SMB Password 
    $attrs= generate_smb_nt_hash($password);
  }

  $attrs['userPassword']= array();
  $attrs['userPassword']= $newpass;


  $ldap->modify($attrs);


  if ($ldap->error != 'Success') {
    print_red(sprintf(_("Setting the password failed. LDAP server says '%s'."),
          $ldap->get_error()));
  } else {

    /* Find postmodify entries for this class */
    $command= search_config($config->data['MENU'], "password", "POSTMODIFY");

    if ($command != ""){
      /* Walk through attribute list */
      $command= preg_replace("/%userPassword/", $password, $command);
      $command= preg_replace("/%dn/", $dn, $command);

      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");
        exec($command);
      } else {
        $message= sprintf(_("Command '%s', specified as POSTMODIFY for plugin '%s' doesn't seem to exist."), $command, "password");
        print_red ($message);
      }
    }
  }
}


// Return something like array['sambaLMPassword']= "lalla..." 
function generate_smb_nt_hash($password)
{
  global $config;
  $tmp= $config->data['MAIN']['SMBHASH']." ".escapeshellarg($password);
  @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $tmp, "Execute");

  exec($tmp, $ar);
  flush();
  reset($ar);
  $hash= current($ar);
  if ($hash == "")
  {
    print_red (_("Setting for SMBHASH in gosa.conf is incorrect! Can't change Samba password."));
  }
  else 
  {
    list($lm,$nt)= split (":", trim($hash));

    if ($config->current['SAMBAVERSION'] == 3)
    {
      $attrs['sambaLMPassword']= $lm;
      $attrs['sambaNTPassword']= $nt;
      $attrs['sambaPwdLastSet']= date('U');
      $attrs['sambaBadPasswordCount']= "0";
      $attrs['sambaBadPasswordTime']= "0";
    } else {
      $attrs['lmPassword']= $lm;
      $attrs['ntPassword']= $nt;
      $attrs['pwdLastSet']= date('U');
    }
    return($attrs);
  }
} 

function crypt_single($string,$enc_type )
{
  if(!class_exists("passwordMethod")){
    require_once("class_password-methods.inc");
  }
  return( passwordMethod::crypt_single_str($string,$enc_type));
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
