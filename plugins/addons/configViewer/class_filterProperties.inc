<?php

class filterCONFIGPROPERTIES {

  static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
  {
    global $config;
    
    $all = $config->configRegistry->getAllProperties();
    $ret = array();
    foreach($all as $property){

        $entry = array();
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'cn', $property->getName());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'objectClass', $property->getStatus());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'status', $property->getStatus());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'description', $property->getDescription());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'value', $property->getValue());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'default', $property->getDefault());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'check', $property->getCheck());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'class', $property->getClass());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'type', $property->getType());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'migrate', $property->getMigrate());
        $entry = filterCONFIGPROPERTIES::fakeLdapResult($entry, 'group', $property->getGroup());
        $entry['dn'] = $property->getName();

        $found =TRUE;
        if(!empty($filter)){
            $tests = preg_split("/§/", $filter);
            foreach($tests as $test){
                list($name,$value) = preg_split("/=/",$test);
                $value =preg_replace("/\*/",'',$value);
                if(empty($value)) $value='.*';
                if(!isset($entry[$name][0]) || !preg_match("/{$value}/",$entry[$name][0])){
                    $found = false;
                }
            } 
        }
       if($found) $ret[] = $entry; 
    }

    return($ret);
  }

    static function fakeLdapResult($result,$name, $value){
        if(!is_array($value)){
            $value = array($value);
        }
        $value['count'] = count($value);
        $result[] = $name;
        $result[$name] = $value;
        if(!isset($result['count'])){
            $result['count'] =0;
        }
        $result['count'] ++;
        return($result);
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
