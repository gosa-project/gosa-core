<?php

class filterConfigManagement {

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
    {

        echo $base;

        // Walk through the data array till we reach the correct level, then 
        //  create the list of entries.
        $data = session::get('CONFIG_ITEM');

        // Prepare filter, strip out regex regex related chars, except for *
        $filter = preg_replace('/\*/','____STAR____', $filter);
        $filter = preg_quote($filter,'/');
        $filter = preg_replace('/____STAR____/','.*', $filter);

        // Add entries 
        $ret = array();
        foreach($data['children'] as $item){
            filterDeviceItems::addEntry($ret, $item, $scope == 'sub', $filter);
        }
        return($ret);
    }

    static function addEntry(&$ret, $item, $recursive = FALSE, $filter)
    {
        if(preg_match("/".$filter."/",$item['name'])){
            $entry = array();
            $entry['dn'] = $item['id'];
            $entry[] = 'dn';
            $entry['cn'] = array($item['name'], 'count' => 1);
            $entry[] = 'cn';
            $entry['id'] = array($item['id'], 'count' => 1);
            $entry[] = 'id';
            $entry['description'] = array($item['type'], 'count' => 1);
            $entry[] = 'description';
            $entry['objectClass'] = array($item['type'],'count' => 1);
            $entry[] = 'objectClass';
            $entry['count'] = 5;
            $ret[] = $entry;
        }

        if($recursive && isset($item['children']) && count($item['children'])){
            foreach($item['children'] as $item){
                filterDeviceItems::addEntry($ret, $item, $recursive,$filter);
            }
        }

    }
}

?>
