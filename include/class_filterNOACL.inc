<?php

class filterNOACL extends filterLDAPBlacklist{

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= array(""))
  {
    $config= session::global_get('config');
    $ldap= $config->get_ldap_link(TRUE);
    $flag= GL_NO_ACL_CHECK | GL_SIZELIMIT | (($scope == "sub")?GL_SUBSEARCH:0);
    $result= filterLDAP::get_list($base, $filter, $attributes, $category, $objectStorage, $flag);
    $result = (filterLDAPBlacklist::filterByBlacklist($result));
    return $result;
  }
}

?>
