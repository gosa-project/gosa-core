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

class userinfo
{
  var $dn;
  var $ip;
  var $username;
  var $cn;
  var $uid;
  var $gidNumber= -1;
  var $language= "";
  var $config;
  var $gosaUnitTag= "";
  var $subtreeACL= array();
  var $ACL= array();
  var $ocMapping= array();
  var $groups= array();
  var $result_cache =array();

  /* get acl's an put them into the userinfo object
     attr subtreeACL (userdn:components, userdn:component1#sub1#sub2,component2,...) */
  function userinfo(&$config, $userdn){
    $this->config= &$config;
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($userdn,array('sn', 'givenName', 'uid', 'gidNumber', 'preferredLanguage', 'gosaUnitTag'));
    $attrs= $ldap->fetch();

    if (isset($attrs['givenName'][0]) && isset($attrs['sn'][0])){
      $this->cn= $attrs['givenName'][0]." ".$attrs['sn'][0];
    } else {
      $this->cn= $attrs['uid'][0];
    }
    if (isset($attrs['gidNumber'][0])){
      $this->gidNumber= $attrs['gidNumber'][0];
    }

    /* Assign user language */
    if (isset($attrs['preferredLanguage'][0])){
      $this->language= $attrs['preferredLanguage'][0];
    }

    if (isset($attrs['gosaUnitTag'][0])){
      $this->gosaUnitTag= $attrs['gosaUnitTag'][0];
    }

    $this->dn= $userdn;
    $this->uid= $attrs['uid'][0];
    $this->ip= $_SERVER['REMOTE_ADDR'];

    /* Initialize ACL_CACHE */
    session::set('ACL_CACHE',array());
    $this->reset_acl_cache();
  }


  public function reset_acl_cache()
  {
    /* Initialize ACL_CACHE */
    session::set('ACL_CACHE',array());
  }

  function loadACL()
  {
    $this->ACL= array();    
    $this->groups= array();    
    $this->result_cache =array();
    $this->reset_acl_cache();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Get member groups... */
    $ldap->search("(&(objectClass=posixGroup)(memberUid=".$this->uid."))", array('dn'));
    while ($attrs= $ldap->fetch()){
      $this->groups[$attrs['dn']]= $attrs['dn'];
    }

    /* Crawl through ACLs and move relevant to the tree */
    $ldap->search("(objectClass=gosaACL)", array('dn', 'gosaAclEntry'));
    $aclp= array();
    $aclc= array();
    while ($attrs= $ldap->fetch()){

      /* Insert links in ACL array */
      $aclp[$attrs['dn']]= substr_count($attrs['dn'], ',');
      $aclc[$attrs['dn']]= array();
      $ol= array();
      for($i= 0; $i<$attrs['gosaAclEntry']['count']; $i++){
        $ol= array_merge($ol, @acl::explodeAcl($attrs['gosaAclEntry'][$i]));
      }
      $aclc[$attrs['dn']]= $ol;
    }

    /* Resolve roles here. 
     */
    foreach($aclc as $dn => $data){
      foreach($data as $prio => $aclc_value)  {
        if($aclc_value['type'] == "role"){

          unset($aclc[$dn][$prio]);

          $ldap->cat($aclc_value['acl'],array("gosaAclTemplate"));
          $attrs = $ldap->fetch();

          if(isset($attrs['gosaAclTemplate'])){
            for($i= 0; $i<$attrs['gosaAclTemplate']['count']; $i++){
              $tmp = @acl::explodeAcl($attrs['gosaAclTemplate'][$i]);  

              foreach($tmp as $new_acl){
                $new_acl['members'] = $aclc_value['members'];
                $aclc[$dn][] =$new_acl;
              }
            }      
          }
        }
      }
    }

    /* ACL's read, sort for tree depth */
    asort($aclp);

    /* Sort in tree order */
    foreach ($aclp as $dn => $acl){
      /* Check if we need to keep this ACL */
      foreach($aclc[$dn] as $idx => $type){
        $interresting= FALSE;
        
        /* No members? This is good for all users... */
        if (!count($type['members'])){
          $interresting= TRUE;
        } else {

          /* Inspect members... */
          foreach ($type['members'] as $grp => $grpdsc){
            /* Some group inside the members that is relevant for us? */
            if (in_array_ics(preg_replace('/^G:/', '', $grp), $this->groups)){
              $interresting= TRUE;
            }

            /* User inside the members? */
            if (preg_replace('/^U:/', '', $grp) == $this->dn){
              $interresting= TRUE;
            }
          }
        }

        if ($interresting){
          if (!isset($this->ACL[$dn])){
            $this->ACL[$dn]= array();
          }
          $this->ACL[$dn][$idx]= $type;
        }
      }

    }

  }


  function get_category_permissions($dn, $category)
  {
    /* Get list of objectClasses and get the permissions for it */
    $acl= "";
    if (isset($this->ocMapping[$category])){
      foreach($this->ocMapping[$category] as $oc){
        $acl.= $this->get_permissions($dn, $category."/".$oc);
      }
    }else{
      trigger_error("ACL request for an invalid category (".$category.").");
    }

    return ($acl);
  }


  function get_permissions($dn, $object, $attribute= "", $skip_write= FALSE)
  {
    /* Push cache answer? */
    $ACL_CACHE = &session::get('ACL_CACHE');
    if (isset($ACL_CACHE["$dn+$object+$attribute"])){

      /* Remove write if needed */
      if ($skip_write){
        $ret = preg_replace('/w/', '', $ACL_CACHE["$dn+$object+$attribute"]);
      }else{
        $ret = $ACL_CACHE["$dn+$object+$attribute"];
      } 
      return($ret);
    }

    $acl= array("r" => "", "w" => "", "c" => "", "d" => "", "m" => "", "a" => "");

    /* Build dn array */
    $path= split(',', $dn);
    $path= array_reverse($path);

    /* Walk along the path to evaluate the acl */
    $cpath= "";
    foreach ($path as $element){

      /* Clean potential ACLs for each level */
      $acl= $this->cleanACL($acl);

      if ($cpath == ""){
        $cpath= $element;
      } else {
        $cpath= $element.','.$cpath;
      }
      if (isset($this->ACL[$cpath])){

        /* Inspect this ACL, place the result into ACL */
        foreach ($this->ACL[$cpath] as $subacl){

          /* Reset? Just clean the ACL and turn over to the next one... */
          if ($subacl['type'] == 'reset'){
            $acl= $this->cleanACL($acl, TRUE);
            continue;
          }

          if($subacl['type'] == "role") {
            echo "role skipped";
            continue;
          }

          /* Per attribute ACL? */
          if (isset($subacl['acl'][$object][$attribute])){
            $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl'][$object][$attribute]);
            continue;
          }

          /* Per object ACL? */
          if (isset($subacl['acl'][$object][0])){
            $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl'][$object][0]);
            continue;
          }

          /* Global ACL? */
          if (isset($subacl['acl']['all'][0])){
            $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl']['all'][0]);
            continue;
          }

          /* If attribute is "", we want to know, if we've *any* permissions here... */
          if ($attribute == "" && isset($subacl['acl'][$object])){
            foreach($subacl['acl'][$object] as $attr => $dummy){
              $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl'][$object][$attr]);
            }
            continue;
          }

        }
      }
    }

    /* Assemble string */
    $ret= "";
    foreach ($acl as $key => $value){
      if ($value != ""){
        $ret.= $key;
      }
    }

    $ACL_CACHE["$dn+$object+$attribute"]= $ret;

    /* Remove write if needed */
    if ($skip_write){
      $ret= preg_replace('/w/', '', $ret);
    }
    return ($ret);
  }


  /* Extract all departments that are accessible (direct or 'on the way' to an
     accessible department) */
  function get_module_departments($module)
  {
    /* Use cached results if possilbe */
    $ACL_CACHE = session::get('ACL_CACHE');
    if(isset($ACL_CACHE['MODULE_DEPARTMENTS'][serialize($module)])){
      return($ACL_CACHE['MODULE_DEPARTMENTS'][serialize($module)]);
    }

    global $plist;

    $objects= array();
    $deps= array();

    /* Extract all relevant objects for this module from plist */
    foreach ($plist->info as $object => $info){
      if (!isset($info['plCategory'])){
        continue;
      }
      foreach ($info['plCategory'] as $idx => $data){
        if (preg_match('/^[0-9]+$/', $idx)){
          if ($data == $module){
            $objects[$object]= $object;
          }
        } else {
          if ($idx == $module){
            $objects[$object]= $object;
          }
        }
      }
    }

    /* For all gosaDepartments */
    foreach ($this->config->departments as $dn){
      $acl= array("r" => "", "w" => "", "c" => "", "d" => "", "m" => "", "a" => "");

      /* Build dn array */
      $path= split(',', $dn);
      $path= array_reverse($path);

      /* Walk along the path to evaluate the acl */
      $cpath= "";
      foreach ($path as $element){

        /* Clean potential ACLs for each level */
        $acl= $this->cleanACL($acl);

        if ($cpath == ""){
          $cpath= $element;
        } else {
          $cpath= $element.','.$cpath;
        }
        if (isset($this->ACL[$cpath])){

          /* Inspect this ACL, place the result into ACL */
          foreach ($this->ACL[$cpath] as $subacl){

            /* Reset? Just clean the ACL and turn over to the next one... */
            if ($subacl['type'] == 'reset'){
              $acl= $this->cleanACL($acl, TRUE);
              continue;
            }
    
            if($subacl['type'] == 'role'){
              echo "role skipped";
              continue;
            }

            /* Per object ACL? */
            foreach ($objects as $object){
              if (isset($subacl['acl']["$module/$object"])){
                foreach($subacl['acl']["$module/$object"] as $attribute => $dcl){
                  $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl']["$module/$object"][$attribute]);
                }
              }
            }

            /* Global ACL? */
            if (isset($subacl['acl']["$module/all"][0])){
              $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl']["$module/all"][0]);
              continue;
            }

            /* Global ACL? */
            if (isset($subacl['acl']["all"][0])){
              $acl= $this->mergeACL($acl, $subacl['type'], $subacl['acl']["all"][0]);
              continue;
            }
          }
        }
      }

      /* Add department, if we have (some) permissions for the required module */
      foreach ($acl as $val){
        if ($val != ""){
          $deps[]= $dn;
          break;
        }
      }
    }

    $ACL_CACHE = &session::get('ACL_CACHE');
    $ACL_CACHE['MODULE_DEPARTMENTS'][serialize($module)] = $deps;
    return ($deps);
  }


  function mergeACL($acl, $type, $newACL)
  {
    if (strpos($newACL, 'w') !== FALSE && strpos($newACL, 'r') === FALSE){
      $newACL .= "r";
    }

    foreach(str_split($newACL) as $char){

      /* Ignore invalid characters */
      if (!preg_match('/[rwcdm]/', $char)){
        continue;
      }

      /* Skip permanent and subtree entries */
      if (preg_match('/[sp]/', $acl[$char])){
        continue;
      }

      switch ($type){
        case 'psub':
          $acl[$char]= 'p';
          break;

        case 'sub':
          $acl[$char]= 's';
          break;

        case 'one':
          $acl[$char]= 1;
          break;

        case 'base':
          if ($acl[$char] != 1){
            $acl[$char]= 0;
          }
          break;
      }
    }

    return ($acl);
  }


  function cleanACL($acl, $reset= FALSE)
  {
    foreach ($acl as &$value){

      /* Reset removes everything but 'p' */
      if ($reset && $value != 'p'){
        $value= "";
        continue;
      }

      /* Decrease tree level */
      if (is_int($value)){
        if ($value){
          $value--;
        } else {
          $value= "";
        }
      }
    }

    return ($acl);
  }


  /* #FIXME This could be logical wrong or could be optimized in the future
     Return combined acls for a given category. 
     All acls will be combined like boolean AND 
      As example ('rwcdm' + 'rcd' + 'wrm'= 'r') 
    
     Results will be cached in $this->result_cache.
      $this->result_cache will be resetted if load_acls is called.
  */
  function has_complete_category_acls($dn,$category)
  {
    $acl    = "rwcdm";
    $types  = "rwcdm";

    if(!is_string($category)){
      trigger_error("category must be string");   
      $acl = "";
    }else{
      if(!isset($this->result_cache['has_complete_category_acls'][$dn][$category]))   {
        if (isset($this->ocMapping[$category])){
          foreach($this->ocMapping[$category] as $oc){

            /* Skip objectClass '0' (e.g. users/0) get_permissions will ever return '' ??  */
            if($oc == "0") continue;
            $tmp =  $this->get_permissions($dn, $category."/".$oc);
            for($i = 0 ; $i < strlen($types); $i++) {
              if(!preg_match("/".$types[$i]."/",$tmp)){ 
                $acl = preg_replace("/".$types[$i]."/","",$acl);
              }
            }
          }
        }else{
          trigger_error("Invalid type of category ".$category);
          $acl = "";
        }
        $this->result_cache['has_complete_category_acls'][$dn][$category] = $acl;
      }else{
        $acl = $this->result_cache['has_complete_category_acls'][$dn][$category];
      }
    }
    return($acl);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
