<?php

class filterLDAPBlacklist {

  static function query($parent,$base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    $result = filterLDAP::query($parent,$base, $scope, $filter, $attributes, $category, $objectStorage);
    return(filterLDAPBlacklist::filterByBlacklist($result));
  }

  static function filterByBlacklist($entries)
  {
    if(session::is_set('filterBlacklist')){
      $blist = session::get('filterBlacklist');
      foreach($blist as $attr_name => $attr_values){
        foreach($attr_values as $match){
          foreach($entries as $id => $entry){
            if(isset($entry[$attr_name])){
              $test = $entry[$attr_name];
              if(!is_array($test)) $test = array($test);
              if(in_array($match, $test)) unset($entries[$id]);
            }
          }
        }
      }
    }
    return(array_values($entries));
  }
}

?>
