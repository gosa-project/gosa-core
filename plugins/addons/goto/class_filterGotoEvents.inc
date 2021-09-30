<?php
class filterGotoEvents extends  filterLDAP {
    static function query($base, $scope, $filter, $attributes, $category, $objectStorage= array(""))
    {
        global $config;

        $o_queue = NULL;
        $events  = DaemonEvent::get_event_types( SYSTEM_EVENT);

        /* Get tags that will be used in queue searches */
        $event_tags = array("none");
        foreach($events['SCHEDULED'] as $evt){
            $event_tags[] = $evt['s_Queued_Action'];
        }

        $entries = array();

        /* Assign entries by id.
         */
        $data= array();
        $ui = get_userinfo();


        $ui= get_userinfo();
        $acl_base = $config->current['BASE'];
        $acl_is_writeable = preg_match('/w/', $ui->get_permissions($acl_base, "gotomasses/gotomasses", "0"));
        $acl_is_removeable = preg_match('/d/', $ui->get_permissions($acl_base, "gotomasses/gotomasses", "0"));
        $acl_is_readable = preg_match('/r/', $ui->get_permissions($acl_base, "gotomasses/gotomasses", "0"));

        foreach($entries as $entry){

            // Detect entry status 
            $entry['objectClass'][] = "FAKE_OC__gotoEvent";

            /* If WAITING add priority action
             */
            if(in_array_strict($entry['STATUS'],array("waiting")) && $acl_is_writeable){
                $entry['objectClass'][] = "FAKE_OC__prioUp";
                $entry['objectClass'][] = "FAKE_OC__prioDown";
                $entry['objectClass'][] = "FAKE_OC__prioPause";
            }

            /* If PAUSED add resume action
             */
            if(in_array_strict($entry['STATUS'],array("paused")) && $acl_is_writeable){
                $entry['objectClass'][] = "FAKE_OC__prioResume";
            }

            /* If PAUSED or WAITING add execution action
             */
            if(in_array_strict($entry['STATUS'],array("paused","waiting")) && $acl_is_writeable){
                $entry['objectClass'][] = "FAKE_OC__processNow";
            }

            /* Add logview button, currently ever.
             */
            if($acl_is_readable){
                $entry['objectClass'][] = "FAKE_OC__viewLogs";
            }

            /* If PAUSED or WAITING add edit action
             */
            if(in_array_strict($entry['STATUS'],array("waiting")) && $acl_is_writeable){
                $entry['objectClass'][] = "FAKE_OC__edit";
            }


            /* If PROCESSING add abort action
             */
            if(in_array_strict($entry['STATUS'],array("processing")) && preg_match("/install/",$entry['HEADERTAG']) && $acl_is_writeable){
                $entry['objectClass'][] = "FAKE_OC__abort";
            }

            /* If WAITING or ERROR add remove action
             */
            if( $acl_is_removeable && in_array_strict($entry['STATUS'],array("waiting","error","processed"))){
                $entry['objectClass'][] = "FAKE_OC__remove";
            }
            if($acl_is_writeable && in_array_strict($entry['STATUS'],array("processing")) && !preg_match("/install/",$entry['HEADERTAG'])){
                $entry['objectClass'][] = "FAKE_OC__remove";
            }

            $item = array();
            $item['count'] = 0;
            $item['EVENT'] = $entry;
            $item['dn']= $entry['MACADDRESS'].",".$entry['TIMESTAMP'];

            foreach($entry as $name => $value){
                $item[] = $name;
                if(!is_array($value)){
                    $item[$name]=array('count' => 1, $value);
                }else{
                    $item[$name] = $value;
                    $item[$name]['count'] = count($value);
                }
                $item['count'] ++;
            }


#     /* Skip entries which will be removed within the next seconds */
#     if(isset($entry['MACADDRESS']) && in_array_strict($entry['MACADDRESS'],$recently_removed)){
#       continue;
#     }
            $data[] = $item;
        }

        return($data);
    }
}
?>
