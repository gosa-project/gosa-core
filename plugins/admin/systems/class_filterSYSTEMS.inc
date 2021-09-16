<?php

class filterSYSTEMS {

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= "")
    {
        $ui = get_userinfo();
        global $config;

        // Check whether the arp handling active or not
        $arp_handling_active = $config->pluginEnabled("ArpNewDevice");

        $entries = filterLDAP::query($base, $scope, $filter, $attributes, $category, $objectStorage);


        foreach($entries as $key => $entry){
            if(class_available("ArpNewDevice") &&
                preg_match("/".preg_quote(get_ou("ArpNewDevice", "systemIncomingRDN"),'/')."/i", $entry['dn'])){

                // Detect incoming types
                if(in_array_strict('gotoWorkstation', $entry['objectClass'])){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_NewWorkstation';
                    $entries[$key]['cn'][0].= " <i>("._("New workstation").")</i>";
                }elseif(in_array_strict('gotoTerminal', $entry['objectClass'])){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_NewTerminal';
                    $entries[$key]['cn'][0].= " <i>("._("New terminal").")</i>";
                }elseif($arp_handling_active && 
                        in_array_strict('GOhard', $entry['objectClass']) && 
                        !isset($entry['gotoMode'][0])){ 
                    $entries[$key]['objectClass'][] = 'FAKE_OC_ArpNewDevice';
                    $entries[$key]['cn'][0].= " <i>("._("New unknown device").")</i>";
                }elseif(in_array_strict('GOhard', $entry['objectClass'])){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_NewDevice';
                    $entries[$key]['cn'][0].= " <i>("._("New device").")</i>";
                }

                // Detect SAMBA machines
            }elseif(in_array_strict('sambaSamAccount', $entry['objectClass']) && preg_match("/\$$/", $entry['cn'][0])){
                $entries[$key]['objectClass'][] = 'FAKE_OC_winstation';

                // Detect Workstation & templates
            }elseif(in_array_strict('gotoWorkstation', $entry['objectClass'])){
                $pwd_acl = $ui->get_permissions($entry['dn'],'workstation/workgeneric','userPassword');
                if(preg_match("/w/",$pwd_acl)){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_PWD_changeAble';
                }

                // Detect Terminal & templates
            }elseif(in_array_strict('gotoTerminal', $entry['objectClass'])){
                $pwd_acl = $ui->get_permissions($entry['dn'],'terminal/termgeneric','userPassword');
                if(preg_match("/w/",$pwd_acl)){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_PWD_changeAble';
                }

                // Detect Server & templates
            }elseif(in_array_strict('goServer', $entry['objectClass'])){
                $pwd_acl = $ui->get_permissions($entry['dn'],'server/servgeneric','userPassword');
                if(preg_match("/w/",$pwd_acl)){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_PWD_changeAble';
                }

                // Detect ieee802Device
            }elseif(in_array_strict('ieee802Device', $entry['objectClass'])){
                $pwd_acl = $ui->get_permissions($entry['dn'],'component/componentGeneric','userPassword');
                if(preg_match("/w/",$pwd_acl)){
                    $entries[$key]['objectClass'][] = 'FAKE_OC_PWD_changeAble';
                }
            }

            // Detect object states
            $state = 'none';
            foreach(array('gotoWorkstation','gotoTerminal','goServer') as $type){
                if(in_array_strict($type, $entry['objectClass'])){

                    if(isset($entry['FAIstate'][0])){
                        switch (preg_replace('/:.*$/', '', $entry['FAIstate'][0])) {
                            case 'error':
                                $state= 'error';
                                break;
                            case 'installing':
                            case 'install':
                            case 'sysinfo':
                            case 'softupdate':
                            case 'scheduledupdate':
                                $state= 'busy';
                                break;
                        }
                    }elseif(isset($entry['gotoMode'][0]) && preg_match("/locked/i", $entry['gotoMode'][0])){
                        $state = "locked";
                    }
                }
            }

            if(isset($entries[$key])){    
                $entries[$key][$entries[$key]['count']]= 'state';
                $entries[$key]['state']= $state;
                $entries[$key]['count'] ++;
            }
        }
        return($entries);
    }
}

?>
