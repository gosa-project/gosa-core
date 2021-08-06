<?php

class filterServerService {

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
    {
        $plugins = ServerService::getServiceList();
        $list = array();
        foreach($plugins as $name => $obj){
            if($obj->is_account){
                $data = $plugins[$name]->getListEntry();
                $data['objectClass'] = array('FAKE_OC_ServerService'); 
                $data['name'] = $name;
                if($data['AllowStart'])   $data['objectClass'][] = "AllowStart";
                if($data['AllowStop'])    $data['objectClass'][] = "AllowStop";
                if($data['AllowRestart']) $data['objectClass'][] = "AllowRestart";
                if($data['AllowRemove'])  $data['objectClass'][] = "AllowRemove";
                if($data['AllowEdit'])    $data['objectClass'][] = "AllowEdit";
                foreach($data as $key => $value){
                    if(!is_array($data[$key])) $data[$key] = array($data[$key]);
                    $list[$name][] = $key;
                    $list[$name][$key] = $data[$key];
                    $list[$name][$key]['count'] = count($data[$key]);
                    $list[$name]['count'] = count($data);
                    $list[$name]['dn'] = $name;
                }
            }
        }
        return(array_values($list));
    }
}

?>
