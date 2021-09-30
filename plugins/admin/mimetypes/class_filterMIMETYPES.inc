<?php

class filterMIMETYPES extends filterLDAP{

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
    {
        $res = filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);

        foreach($res as $id => $entry){
            if(isset($entry['FAIstate']) && preg_match("/freeze/i", $entry['FAIstate'][0])){
                $res[$id]['objectClass'][] = 'FAKE_OC_freeze';
            }else{
                $res[$id]['objectClass'][] = 'FAKE_OC_branch';
            }
        }
        return($res);
    }
}
?>
