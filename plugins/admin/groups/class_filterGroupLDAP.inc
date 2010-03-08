<?php

class filterGroupLDAP {

  static function query($parent,$base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    $result= array();
    $menu= array();
    $dn2index= array();
    $config= session::global_get('config');
    $ldap= $config->get_ldap_link(TRUE);
    $flag= ($scope == "sub")?GL_SUBSEARCH:0;
    $tmp= filterGroupLDAP::get_list($parent,$base, $filter, $attributes, $category, $objectStorage, $flag | GL_SIZELIMIT);

    // Sort out menu entries, but save info
    $index= 0;
    foreach ($tmp as $entry) {
      if (in_array_ics("posixGroup", $entry['objectClass'])) {
        $result[$index]= $entry;
        $dn2index[$entry['dn']]= $index;
        $index++;
      } else {
        foreach ($objectStorage as $storage) {
          $group= preg_replace("/^.*,([^,]+),".preg_quote("$storage$base")."$/", '$1', $entry['dn']);
          $group= "$group,$storage$base";

          // The current group implementation has no multiple storage settings - so break here
          break;
        }
        $menu[$group]= true;
      }
    }

    // Move menu information to menu
    foreach ($menu as $dn => $dummy) {
      if(isset($dn2index[$dn])){
        $result[$dn2index[$dn]]["objectClass"][]= "gotoMenuGroup";
      }
    }

    return $result;
  }


  static function get_list($parent,$base, $filter, $attributes, $category, $objectStorage, $flags= GL_SUBSEARCH)
  {
    $filter= "(|(|(objectClass=gotoMenuEntry)(objectClass=gotoSubmenuEntry))$filter)";
    return filterLDAP::get_list($base, $filter, $attributes, $category, $objectStorage, $flags);
  }


}

?>
