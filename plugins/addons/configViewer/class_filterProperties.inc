<?php

class filterProperties {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    print_a(array($base, $scope, $filter, $attributes, $category, $objectStorage));
    return(array());
  }

  static function unifyResult($result)
  {
    $res=array();
    foreach($result as $entry){
      if(!isset($res[$entry['dn']])){
        $res[$entry['dn']]=$entry;
      }
    }
    return(array_values($res)); 
  }
}

?>
