<?php

class faxreport extends plugin
{
    /* Definitions */
    var $plHeadline     = "FAX Reports";
    var $plDescription  = "View the FAX report or single documents that have been received";
    var $plIcon         = "plugins/gofax/images/reports.png";
    var $matIcon        = "report";

    /* For internal use */
    var $start          = 0;
    var $search_for     = "*";
    var $search_base    = "";
    var $year           = "";
    var $month          = "";
    var $sort           = 1;
    var $sort_direction = "down";
    var $ui             = NULL;
    var $range          = 20;
    var $view_logged    = FALSE;

    /* Constant stuff */
    var $status= array( "SENT", "MAILED", "SERROR", "RERROR", "SBLOCK", "RBLOCK",
        "DELETED", "REQUEUED", "DISABLED", "PRINTED", "DIVERTED",
        "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
        "UNDEFINED");
    var $fields= array("uid", "queuing_time", "status", "sender_id", "receiver_id", "pages");

    /* these vars will be stored in session to be able to remember last search config */
    var $attributes_SO= array("search_for","month","year",
        "start","year","month","sort","sort_direction","range");

    var $objectclasses= array();
    var $fax_users    = array();

    // An array containing all valid fax accounts (uid => dn)
    // This enables us to perform correct ACL checks later. 
    // We require "users/viewFaxEntries" acls for to users dn, if he exists.
    //  and "users/viewFaxEntries" acls for the LDAP-Base if the uid is 
    //  unknown (The uid is not represented by a valid FAX account).
    var $uidToDN = array();

    /* Create class */
    function __construct(&$config, &$ui)
    {
        $this->initTime = microtime(TRUE);

        /* Include config object */
        $this->config       = $config;
        $this->ui           = &$ui;
        $this->search_base  = $this->config->current['BASE'];
        $this->year         = date("Y");
        $this->month        = date("m");

        // Get global filter config and set class vars , or create a filter 
        if (!session::is_set("faxreportfilter")){
            $faxreportfilter = array();
            foreach($this->attributes_SO as $name){
                $faxreportfilter[$name] = $this->$name;
            } 
            session::set("faxreportfilter",$faxreportfilter);
        }else{
            $faxreportfilter = session::get("faxreportfilter");
            foreach($this->attributes_SO as $name){
                $this->$name = $faxreportfilter[$name];
            }
        }

        // Get ALL valid FAX-Accounts and their dns, this allows us to perform correct
        //  permissions checks later. 
        $filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(objectClass=goFaxAccount)(uid=*))";
        $tmp= get_list($filter, "users/viewFaxEntries", $this->search_base, 
            array("uid"), GL_SUBSEARCH | GL_NO_ACL_CHECK);
        $this->uidToDN = array();
        foreach($tmp as $attrs){
            $this->uidToDN[$attrs['uid'][0]] = $attrs['dn'];
        }

        // Create statistic table entry
        stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
                $amount = 1, $duration = (microtime(TRUE) - $this->initTime));

    }


    /* Create Filter & Search & Display results */
    function execute()
    {
        /* Call parent execute */
        plugin::execute();

        /* Log view */
        if(!$this->view_logged){
            $this->view_logged = TRUE;
            new log("view","users/".get_class($this),$this->dn);
        }

        // Variable initialisation 

        /* Create months */ 
        $months= array();
        for($i = 1 ; $i <= 12 ; $i ++ ){
            $months[$i] = _(date("F",gmmktime(0,0,0,$i,1)));
        }

        /* Create years */
        $current= date("Y");
        $years= array();
        for ($y= $current - 5; $y<=$current; $y++){
            $years[]= $y;
        }


        // Set smarty defaults  
        $smarty= get_smarty();
        $smarty->assign("search_for"		, set_post($this->search_for));
        $smarty->assign("months"			  , $months);
        $smarty->assign("month_select"	, $this->month);
        $smarty->assign("years"				  , $years);
        $smarty->assign("year_select"		, $this->year);
        $smarty->assign("search_result"	, "");


        // Check database accessibility 
        if(!isset($this->config->data['SERVERS']['FAX'])){
            msg_dialog::display(_("Error"), _("No FAX server found!"), ERROR_DIALOG);
            return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
        }elseif(!is_callable("mysqli_connect")){
            msg_dialog::display(_("Configuration error"), sprintf(
                _("Missing %s PHP extension!"), "mysqli"), WARNING_DIALOG);
            return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
        }else{
            /* Connecting, selecting database */
            $cfg	= $this->config->data['SERVERS']['FAX'];
            $link 	= @mysqli_connect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD'], "gofax");
            if ($link === FALSE){
                msg_dialog::display(_("Error"), sprintf(
                    _("Cannot connect to %s database!"), "<b>".$cfg['SERVER'].":"."GOfax"."</b>"), ERROR_DIALOG);
                return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
            }

            if (! mysqli_query($link, "SELECT * FROM faxlog;")){
                msg_dialog::display(_("Error"), sprintf(
                    _("Cannot query %s database!"), "GOfax"), ERROR_DIALOG);
                return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
            }
        }		


        // Perform a deatil view 
        $detail = -1;
        foreach($_POST as $name => $desc){
            if(preg_match("/^detail_/", $name)){
                $detail = postDecode(preg_replace("/^detail_/","",$name));
                break;
            }
        }
        if ($detail != -1){

            // Query for the requested fay entry 
            $query = "SELECT id,uid,date_format(queuing_time, '%Y%m%d%H%i%s') ".
                "as queuing_time,status,sender_id,sender_msn,receiver_id,".
                "receiver_msn,pages,status_message,transfer_time FROM faxlog WHERE id='".$detail."';";

            $cfg= $this->config->data['SERVERS']['FAX'];
            $result = @mysqli_query($link, $query);
            if ($result === false){
                msg_dialog::display(_("Error"), sprintf(
                    _("Cannot query %s database!"), "GOfax"), ERROR_DIALOG);
                @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query failed");
                return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
            }
            $line = mysqli_fetch_array($result, MYSQL_ASSOC);
            mysqli_close($link);

            // Get dn to check ACLs for
            // Existing ldap-user -> use its dn  
            // Not existing user  -> use ldap base dn  
            $dn = $this->search_base;
            if(isset($this->uidToDN[$line['uid']])){
                $dn = $this->uidToDN[$line['uid']];
            }

            // We do not have any ACLs for this entry, so continue.
            $acls = $this->ui->get_permissions($dn,"users/viewFaxEntries","");
            if(!preg_match("/r/",$acls)){
                msg_dialog::display(_("Permission error"), 
                _("You have no permission to view this FAX id!"), ERROR_DIALOG);
                return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
            }

            // Check permissions for each attribute
            $parts= array( "id", "uid", "queuing_time", "status", "sender_id", "sender_msn",
                "receiver_id", "receiver_msn", "pages", "status_message", "transfer_time" );
            foreach ($parts as $vname) {
                $final="fax_$vname";
                $v_acl = $this->ui->get_permissions($dn,"users/viewFaxEntries",preg_replace("/_/","",$vname));
                if($line[$vname] != "" && preg_match("/r/i", $v_acl)){
                    $smarty->assign("$final", set_post($line[$vname]));
                } else {
                    $smarty->assign("$final", "-");
                }
            }
            $format= _("Y-M-D");
            $queuing_time= $line['queuing_time'];

            $plug = (isset($_GET['plug'])) ? '?plug='.$_GET['plug'] : '';
            $smarty->assign("plug", $plug);
            $smarty->assign("detail", set_post($detail));
            $date= preg_replace("/Y/", substr($queuing_time,0,4), $format);
            $date= preg_replace("/M/", substr($queuing_time,4,2), $date);
            $date= preg_replace("/D/", substr($queuing_time,6,2), $date);
            $smarty->assign("date", $date);
            $smarty->assign("time", substr($queuing_time,8,2).":".
                substr($queuing_time,10,2).":".
                substr($queuing_time,12,2));

            return($smarty->fetch(get_template_path('detail.tpl', TRUE)));
        }


        // Convert search filter into useable string
        $fax_users= array();
        $s = preg_replace("/\*/","",$this->search_for);
        if(!empty($s)) $s = "*{$s}*"; else $s="*";

        // Collect potential 'uid's that match the filter.
        $filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))".
            "(objectClass=goFaxAccount)".
            "(|(uid=$s)(l=$s)(homePhone=$s)".
            "(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
            "(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)".
            "(title=$s)))";
        $res = get_list($filter, "users/viewFaxEntries", $this->search_base, array("uid"), GL_SUBSEARCH );
        $fax_users = array();
        foreach($res as $attrs){
           $fax_users[ $attrs['dn']]= $attrs["uid"][0];
        }

        // Search for entries mathcing the filter
        $userfilter = "uid like '".preg_replace("/\*/","%%", $s)."' OR ";
        $userfilter.= "sender_id like '".preg_replace("/\*/","%%", $s)."' OR ";
        $userfilter.= "receiver_id like '".preg_replace("/\*/","%%", $s)."' OR ";

        // ... and additionally search for a status value.
        $id = array_search($this->search_for, $this->status);
        if($id !== FALSE){
            $userfilter.= "status = '".$id."' OR ";
        }

        // Add collectod users to the query
        foreach ($fax_users as $user){
            $userfilter.= "uid = '$user' OR ";
        }
        $userfilter= "(".preg_replace("/OR $/", "", $userfilter).")";

        // Add date settings
        $desc = ($this->sort_direction == "down")? "": "DESC";
        $start= date ("YmdHis", mktime(0,0,0,$this->month,1,$this->year));
        $end=   date ("YmdHis", mktime(23,59,59,$this->month+1,0,$this->year));
        $query_entries = "SELECT id,uid,date_format(queuing_time, '%Y%m%d%H%i%s') as queuing_time,".
            "status,sender_id,receiver_id,pages FROM faxlog ".
            "WHERE {$userfilter} AND queuing_time <= {$end} AND ".
            "queuing_time >= {$start} ".
            "ORDER BY ".$this->fields[$this->sort].
            " {$desc } ";


        $cfg= $this->config->data['SERVERS']['FAX'];
        $link = @mysqli_connect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD'], "gofax");

        // Get the results 
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query");
        $result =@mysqli_query($link, $query_entries);
        if ($result === false){
            msg_dialog::display(_("Error"), sprintf(_("Cannot query %s database!"), "GOfax"), ERROR_DIALOG);
            return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
        }

        // Restricted attributes will not be displayed, this will be displayed instead */
        $no_acl = image('images/lists/locked.png','',
            _("Insufficient permissions to view this attribute"));

        // Add found entries to result list.
        $report_list= array();
        $user_ids = array_flip($fax_users);
        while ($line = mysqli_fetch_array($result, MYSQL_ASSOC)) {

            // Get dn to check ACLs for
            // Existing ldap-user -> use its dn  
            // Not existing user  -> use ldap base dn  
            $dn = $this->search_base;
            if(isset($this->uidToDN[$line['uid']])){
                $dn = $this->uidToDN[$line['uid']];
            }

            // We do not have any ACLs for this entry, so continue.
            $tmp = $this->ui->get_permissions($dn,"users/viewFaxEntries","");
            if(empty($tmp)) continue;

            // Hide field for which we have no permissions
            foreach(array("pages","receiverid","senderid","status","queuingtime","detailedView") as $attr){
                $var = $attr."ACL";
                $$var = $this->ui->get_permissions($dn,"users/viewFaxEntries",$attr);
            }

            // Create date 
            if((!empty($line["queuing_time"])) && preg_match("/r/",$queuingtimeACL)){
                $hour=  substr($line["queuing_time"], 8, 2);
                $minute=substr($line["queuing_time"], 10, 2);
                $format= _("Y-M-D");
                $date= preg_replace("/Y/", substr($line["queuing_time"], 0, 4), $format);
                $date= preg_replace("/M/", substr($line["queuing_time"], 4, 2), $date);
                $date= preg_replace("/D/", substr($line["queuing_time"], 6, 2), $date);
                $str_date = $date." ".$hour.":".$minute;
            }else{
                $str_date = $no_acl;
            }

            /* Create entry html str */
            $str = " \n<td class='list0'>".$line["uid"]."</td>";
            $str.= " \n<td class='list0'>".$str_date."</td>";

            /* Add Status td */
            if(preg_match("/r/",$statusACL)){
                $str.=" \n<td class='list0'>".$this->status[$line["status"]]."</td>";
            }else{
                $str.=" \n<td class='list0'>".$no_acl."</td>";
            }

            /* Add sender_id td */
            if(preg_match("/r/",$senderidACL)){
                $str.=" \n<td class='list0'>".htmlentities($line["sender_id"],ENT_COMPAT,'UTF-8')."</td>";
            }else{
                $str.=" \n<td class='list0'>".$no_acl."</td>";
            }

            /* Add receiver_id td */
            if(preg_match("/r/",$receiveridACL)){
                $str.=" \n<td class='list0'>".htmlentities($line["receiver_id"],ENT_COMPAT,'UTF-8')."</td>";
            }else{
                $str.=" \n<td class='list0'>".$no_acl."</td>";
            }

            /* Add receiver_id td */
            if(preg_match("/r/",$pagesACL)){
                $str.=" \n<td class='list0'>".$line["pages"]."</td>";
            }else{
                $str.=" \n<td class='list0'>".$no_acl."</td>";
            }
            /* Create entry html str */
            if(preg_match("/r/",$detailedViewACL)){
                $str.= " \n<td class='list0' style='border-right: 0pt none;'>".
                    image('images/info_small.png', 'detail_'.postEncode($line["id"]))."</td>";
            }else{
                $str.= " \n<td class='list0' style='border-right: 0pt none;'>&nbsp;</td>";
            }
            $report_list[] =  $str;
        }
        mysqli_close($link);

        $entry_count = count($report_list);
        if($this->start >=$entry_count) $this->start =0;

        /* Generate output */
        $mod= 0;
        $output= "";
        $report_list = array_slice($report_list, $this->start, $this->range);
        foreach ($report_list as $val){
            $output.= "\n<tr>{$val}</tr>";
        }
        $smarty->assign("search_result", $output);
        $smarty->assign("range_selector", 
            range_selector($entry_count, $this->start, $this->range,"EntriesPerPage"));

        // Add sorting links
        $smarty->assign("plug", "?plug=".validate($_GET['plug']));
        for($i= 0; $i<7; $i++){
            $smarty->assign("mode$i", "");
        }
        $smarty->assign("mode".$this->sort, image("images/lists/sort-".$this->sort_direction.".png"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }


    // Save posted filter settings
    function save_object()
    {
        $faxreportfilter = session::get("faxreportfilter");
        if(isset($_POST['EntriesPerPage'])){
            $this->range = get_post('EntriesPerPage');
        }

        if (isset($_GET['start'])){
            $this->start= (int)$_GET['start'];
        }

        /* Adapt sorting */
        if (isset($_GET['sort'])){
            if ($this->sort == (int)$_GET['sort']){
                if ($this->sort_direction == "down"){
                    $this->sort_direction= "up";
                } else {
                    $this->sort_direction= "down";
                }
            }
            $this->sort= (int)$_GET['sort'];
            if ($this->sort < 0 || $this->sort > 5){
                $this->sort= 0;
            }
        }
        foreach( array("year", "month", "search_for") as $type){
            if (isset($_POST[$type])){
                $faxreportfilter[$type]= get_post($type);

                /* reset start page, if filter has changed */ 
                if(!isset($_GET['start'])){
                    $this->start = 0;
                }
            }
            $this->$type= $faxreportfilter[$type];

        }
        foreach($this->attributes_SO as $name){
            $faxreportfilter[$name] = $this->$name;
        }  
        session::set("faxreportfilter",$faxreportfilter);
    }
}


class viewFaxEntries extends plugin {

    static function plInfo()
    {

        return (array(
            "plShortName"     => _("View FAX reports"),
            "plDescription"   => _("View FAX reports")."&nbsp;<i>"._("All entries are read-only")."</i>",
            "plRequirements"=> array(
                'activePlugin' => 'faxreport', 
                'ldapSchema' => array('goFaxAccount' => '>=1.0.4'),
                'onFailureDisablePlugin' => array('faxreport','gofaxAccount','goFaxServer')
                ),
            "plSelfModify"    => TRUE,
            "plDepends"       => array(),
            "plPriority"      => 89,                                 // Position in tabs
            "plSection"       => array("administration"),                     // This belongs to personal
            "plCategory"      => array("users"),
            "plOptions"       => array(),

            "plProvidedAcls" => array(
                "detailedView"   => _("Detailed view and download"),
                "id"             => _("Fax ID"),
                "uid"            => _("The username"),
                "queuingtime"    => _("Date")."&nbsp;/&nbsp;"._("Time"),
                "status"         => _("Status"),
                "senderid"       => _("Sender ID"),
                "sendermsn"      => _("Sender MSN"),
                "receiverid"     => _("Receiver ID"),
                "receivermsn"    => _("Receiver MSN"),
                "pages"          => _("Number of pages"),
                "statusmessage"  => _("Status Message"),
                "transfertime"   => _("Transfer time"))
            ));
    }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
