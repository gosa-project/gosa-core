<?php
class filterSystemByIp  extends filterLDAP{

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= array(""))
    {

        // IP Range selection 
        $filterByIP = preg_match("/__FilterByIP__/", $filter);
        $filter = preg_replace("/__FilterByIP__/","",$filter);
        $ipstart = "";
        $ipstop = "";
        if(preg_match("/__IPSTART__=/", $filter)){
            $ipstart = trim(preg_replace("/^.*__IPSTART__=([^_]*)__.*$/","\\1",$filter),"*");
        }
        if(preg_match("/__IPSTOP__=/", $filter)){
            $ipstop = trim(preg_replace("/^.*__IPSTOP__=([^_]*)__.*$/","\\1",$filter),"*");
        }


        // Get entries 
        $filter= preg_replace("/__IPSTART.*$/","",$filter);
        $entries = filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);


        // Filter entries by IP-Address
        if($filterByIP){
            foreach($entries as $key => $entry){ 
                if(!isset($entry['ipHostNumber'])){
                    unset($entries[$key]);
                    continue;
                }
                if(!tests::is_in_ip_range($ipstart,$ipstop, $entry['ipHostNumber'][0])){
                    unset($entries[$key]);
                    continue;
                }
            }
        }
        return(array_values($entries));
    }

}
?>
