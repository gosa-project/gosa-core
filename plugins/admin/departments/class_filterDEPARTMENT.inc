<?php

class filterDEPARTMENT {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= array(""))
  {
    $config= session::global_get('config');
    $ldap= $config->get_ldap_link(TRUE);

    $result = array();
    $flag= ($scope == "sub")?GL_SUBSEARCH:0;
    if(!($flag & GL_SUBSEARCH)){
      $ldap->cat($base);
      $result[] = $ldap->fetch();
    }

    $result= array_merge($result,filterLDAP::get_list($base, $filter, $attributes, $category, $objectStorage, $flag | GL_SIZELIMIT));
    return $result;
  }
}

?>
