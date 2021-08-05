<?php

class filterOPSIHOSTS {

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
    {
        // Append opsi stuff ...
        global $config;
        $ui = get_userinfo();
        $entries = array();

        if(class_available('opsi') && $base == $config->current['BASE']){

            // Explode filter into attribute and value to search for.
            $attr = $value = "";
            $filter = preg_replace("/\*/",".*",$filter);
            if(preg_match("/=/", $filter)) {
                list($fAttr,$fValue) = preg_split("/=/", $filter);
            }

            // Check ACLs and opsi handle
            $opsi_acl = $ui->get_permissions($base,"opsi/opsiProperties");
            $opsi = new opsi($config);
            $objects = array();
            if($opsi instanceof opsi && $opsi->enabled() && preg_match("/r/",$opsi_acl)){

                // Get list of opsi hosts 
                $opsi_clients = $opsi->get_hosts_for_system_management();
                if($opsi->is_error()){
                    msg_dialog::display(_("Error"),msgPool::siError($opsi->get_error()),ERROR_DIALOG);
                }else{
                    foreach($opsi_clients as $entry){
                        if(empty($fAttr) || preg_match("/{$fValue}/i",$entry[$fAttr][0])){
                            $entry['objectClass'] = array("FAKE_OC_OpsiHost");
                            if(!isset($entry['description'])) $entry['description'] = "";
                            $item = array('count' => count($entry));
                            foreach($entry as  $name => $value){
                                $item[] = $name;
                                $item[$name] = $value;
                            }
                            $entries[] = $item;
                        }
                    }
                }
            }
        }
        return($entries);
    }
}

?>
