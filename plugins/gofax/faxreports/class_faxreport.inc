<?php

class faxreport extends plugin
{
  /* Definitions */
  var $plHeadline     = "FAX Reports";
  var $plDescription  = "This does something";

  /* For internal use */
  var $start          = 0;
  var $search_for     = "*";
  var $search_base    = "";
  var $year           = "";
  var $month          = "";
  var $sort           = 1;
  var $sort_direction = "down";
  var $report_list    = array();
  var $ui             = NULL;
  var $range          = 20;
  var $view_logged = FALSE;

  /* Constant stuff */
  var $status= array( "SENT", "MAILED", "SERROR", "RERROR", "SBLOCK", "RBLOCK",
      "DELETED", "REQUEUED", "DISABLED", "PRINTED", "DIVERTED",
      "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
      "UNDEFINED");
  var $fields= array("uid", "queuing_time", "status", "sender_id", "receiver_id", "pages");

  /* these vars will be stored in session to be able to remember last search config */
  var $attributes_SO= array("search_for","search_base","month","year","start","year","month","sort","sort_direction","range");
  var $objectclasses= array();
  var $fax_users    = array();


  /* Create class */
  function faxreport (&$config, &$ui)
  {
    /* Include config object */
    $this->config       = $config;
    $this->ui           = &$ui;
    $this->search_base  = get_base_from_people($ui->dn);
    $this->year         = date("Y");
    $this->month        = date("m");

    /* Get global filter config and set class vars , 
       or create a filter */
    if (!is_global("faxreportfilter")){
      $faxreportfilter = array();
      foreach($this->attributes_SO as $name){
        $faxreportfilter[$name] = $this->$name;
      } 
      register_global("faxreportfilter",$faxreportfilter);
    }else{
      $faxreportfilter = get_global("faxreportfilter");
      foreach($this->attributes_SO as $name){
        $this->$name = $faxreportfilter[$name];
      }
    }
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

    /************ 
      Variable initialisation 
     ************/
 
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

    
    /************ 
      Set smarty defaults  
     ************/

    $smarty= get_smarty();
    $smarty->assign("launchimage"		, get_template_path('images/launch.png'));
    $smarty->assign("search_image"  , get_template_path('images/search.png'));
    $smarty->assign("search_for"		, $this->search_for);
    $smarty->assign("bases"				  , $this->config->idepartments);
    $smarty->assign("base_select"		, $this->search_base);
    $smarty->assign("months"			  , $months);
    $smarty->assign("month_select"	, $this->month);
    $smarty->assign("years"				  , $years);
    $smarty->assign("year_select"		, $this->year);
    $smarty->assign("search_result"	, "");

    
    /************ 
      Check database accessibility 
     ************/

    /* Some checks */
    if(!isset($this->config->data['SERVERS']['FAX'])){
      print_red(_("No fax extension defined in your server configuration, no reports can be shown!"));
      return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }elseif(!is_callable("mysql_connect")){
      print_red(_("There is no mysql extension available, please check your php setup."));
      return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }else{
      /* Connecting, selecting database */
      $cfg	= $this->config->data['SERVERS']['FAX'];
      $link 	= @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
      if ($link === FALSE){
        print_red(_("Can't connect to fax database, no reports can be shown!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }
      if (! @mysql_select_db("gofax")){
        print_red(_("Can't select fax database for report generation!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

      if (! mysql_query("SELECT * FROM faxlog;")){
        print_red(_("Can't query fax table 'faxlog' for report generation!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }
    }		

    
    /************ 
      Perform a deatil view 
     ************/

    /* Do detail view? */
    if (isset($_GET['detail'])){

      /* Create query */
      $query = "SELECT id,uid,date_format(queuing_time, '%Y%m%d%H%i%s') as queuing_time,status,sender_id,sender_msn,receiver_id,".
        "receiver_msn,pages,status_message,transfer_time FROM faxlog WHERE id=".$_GET['detail'].";";

      /* Connecting, selecting database */
      $cfg= $this->config->data['SERVERS']['FAX'];

      /* Check if everything went ok*/
      $result = @mysql_query($query);
      if ($result === false){
        print_red(_("Query for fax database failed!"));
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query failed");
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

      $line = mysql_fetch_array($result, MYSQL_ASSOC);
      mysql_close($link);

      if (!preg_match ("/'".$line["uid"]."'/", $this->userfilter)){
        print_red (_("You have no permission to retrieve informations about this fax id!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

      /* Check acls */
      $fax_uids = array_flip($this->fax_users);
      $uid = $line['uid'];
      $dn = $fax_uids[$uid];
      $acls = $this->ui->get_permissions($dn,"faxreport/faxreport");
      if(!preg_match("/r/",$acls)){
        print_red (_("You have no permission to retrieve informations about this fax id!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

      $parts= array( "id", "uid", "queuing_time", "status", "sender_id", "sender_msn",
          "receiver_id", "receiver_msn", "pages", "status_message", "transfer_time" );

      foreach ($parts as $vname) {

        $final="fax_$vname";
        if($vname != "uid"){
          $v_acl = $this->ui->get_permissions($dn,"faxreport/faxreport",preg_replace("/_/","",$vname));
        }else{
          $v_acl = "r";
        }

        if ($line[$vname] != "" && preg_match("/r/",$v_acl)){
          $smarty->assign("$final", $line[$vname]);
        } else {
          $smarty->assign("$final", "-");
        }
      }
      $queuing_time= $line['queuing_time'];

      /* The user is allowed to download all fax images from those users 
          that are listed in $_SESSION['fuserfilter'] 
         Don't forget to check getfax.php if you change somthing here */
      $_SESSION['fuserfilter']= $this->userfilter;
      $smarty->assign("plug", "?plug=".validate($_GET['plug']));
      $smarty->assign("detail", validate($_GET['detail']));

      $format= _("Y-M-D");
      $date= preg_replace("/Y/", substr($queuing_time,0,4), $format);
      $date= preg_replace("/M/", substr($queuing_time,4,2), $date);
      $date= preg_replace("/D/", substr($queuing_time,6,2), $date);
      $smarty->assign("date", $date);
      $smarty->assign("time", substr($queuing_time,8,2).":".
          substr($queuing_time,10,2).":".
          substr($queuing_time,12,2));
      return($smarty->fetch(get_template_path('detail.tpl', TRUE)));
    }

    
    /************ 
      Search for uids matching the filter  
     ************/

    /* Search button has been pressed */
    if ($this->search_for != ""){

      if (is_integer (strpos($this->search_for, "*"))){
        $s= $this->search_for;
      } else {
        $s= "*".$this->search_for."*";
      }
      $ldap= $this->config->get_ldap_link();
      $ldap->cd ($this->search_base);

      /* Perform ldap search for potential users */
      $filter= "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))".
          "(objectClass=goFaxAccount)".
          "(|(uid=$s)(l=$s)(homePhone=$s)".
          "(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
          "(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)".
          "(title=$s)))";
  
      $res = get_list($filter, "users", $this->search_base, array("uid"), GL_SUBSEARCH );

      /* Check if we are allowed to collect fax data */
      $fax_users= array();
      foreach($res as $attrs){
        $acl = $this->ui->get_permissions($attrs['dn'],"users/user","uid");
        if(preg_match("/r/",$acl)){
          $fax_users[ $attrs['dn']]= $attrs["uid"][0];
        }
      }

      $this->fax_users = $fax_users;

      /* Prepare SQL query */
      $this->userfilter= "";
      foreach ($fax_users as $user){
        $this->userfilter.= "uid = '$user' OR ";
      }
      $this->userfilter= preg_replace("/OR $/", "", $this->userfilter);
    }

    /************ 
      Create filter  
     ************/

    /* Perform SQL query */
    if ($this->userfilter){
      if ($this->sort_direction == "down"){
        $desc= "DESC";
      } else {
        $desc= "";
      }
      $start= date ("YmdHis", mktime(0,0,0,$this->month,1,$this->year));
      $end=   date ("YmdHis", mktime(23,59,59,$this->month+1,0,$this->year));
      $query = "SELECT id,uid,date_format(queuing_time, '%Y%m%d%H%i%s') as queuing_time,status,sender_id,receiver_id,pages FROM faxlog ".
        "WHERE ( ".$this->userfilter." ) AND queuing_time <= $end AND ".
        "queuing_time >= $start ORDER BY ".$this->fields[$this->sort]." $desc;";

      if(!is_callable("mysql_connect")){
        print_red("There is no mysql extension configured in your php setup.");
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

    
    /************ 
      Create results  
     ************/

      /* Connecting, selecting database */
      $cfg= $this->config->data['SERVERS']['FAX'];
      $link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
      
      @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query");
      $result = @mysql_query($query);
      if ($result === false){
        print_red(_("Query for fax database failed!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }

      $this->report_list= array();
      $user_ids = array_flip($fax_users);
      while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {

        /* Check permissions for each field */
        $dn = $user_ids[$line['uid']];

        /* Hide restricted attributes */ 
        foreach(array("pages","receiverid","senderid","status","queuingtime","detailedView") as $attr){
          $var = $attr."ACL";
          $$var = $this->ui->get_permissions($dn,"faxreport/faxreport",$attr);
        }

        /* Restricted attributes will not be displayed, this will be displayed instead */
        $no_acl = "<img class='center' src='images/closedlock.png' 
                    title='"._("Insufficient permissions to view this attribute")."' alt='"._("Insufficient permissions")."'>";

        /* Create date */ 
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

        if(preg_match("/r/",$detailedViewACL)){
          $str = "<td class=\"phonelist\">
            <a href=\"main.php?plug=".validate($_GET['plug'])."&amp;detail=".$line["id"]."\">
            <img class='center' alt=\"\" align=\"middle\" border=0 src=\"".get_template_path('images/info_small.png')."\">
            &nbsp;".$line["uid"].
            "</a>
            </td>
            <td>$str_date</td>";
        }else{
          $str = "<td>
            <img class='center' alt=\"\" align=\"middle\" border=0 src=\"".get_template_path('images/info_small.png')."\">
            &nbsp;".$line["uid"]."</td>
            <td>$str_date</td>";
        }
        
        /* Add Status td */
        if(preg_match("/r/",$statusACL)){
          $str.="<td>".$this->status[$line["status"]]."</td>";
        }else{
          $str.="<td>".$no_acl."</td>";
        }

        /* Add sender_id td */
        if(preg_match("/r/",$senderidACL)){
          $str.="<td>".$line["sender_id"]."</td>";
        }else{
          $str.="<td>".$no_acl."</td>";
        }

        /* Add receiver_id td */
        if(preg_match("/r/",$receiveridACL)){
          $str.="<td>".$line["receiver_id"]."</td>";
        }else{
          $str.="<td>".$no_acl."</td>";
        }

        /* Add receiver_id td */
        if(preg_match("/r/",$pagesACL)){
          $str.="<td>".$line["pages"]."</td>";
        }else{
          $str.="<td>".$no_acl."</td>";
        }
      $this->report_list[] =  $str;
      }

      mysql_close($link);
    }

    /************ 
      Create output out of results  
     ************/

    /* Generate output */
    $mod= 0;
    $output= "";
    foreach ($this->report_list as $val){
      if ($mod < $this->start) {
        $mod++;
        continue;
      }
      if ($mod >= ($this->start + $this->range)){
        $mod++;
        break;
      }
      if ( ($mod++) & 1){
        $col= "background-color: #ECECEC;";
      } else {
        $col= "background-color: #F5F5F5;";
      }
      $output.= "<tr style=\"height:22px; $col\">$val</tr>";
    }

    
    /************ 
      Display results  
     ************/

    if (isset($fax_users) && count($fax_users)){
      $smarty->assign("search_result", $output);
      $smarty->assign("range_selector", range_selector(count($this->report_list), $this->start, $this->range,"EntriesPerPage"));
    }else{
      $smarty->assign("search_result", "");
    }

    /* Show main page */
    $smarty->assign("plug", "?plug=".validate($_GET['plug']));
    for($i= 0; $i<7; $i++){
      $smarty->assign("mode$i", "");
    }
    $smarty->assign("mode".$this->sort, "<img alt=\"\" src=\"images/sort_".$this->sort_direction.
        ".png\" border=0 align=middle>");
    return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
  }


  /* Save ui input, and store it in $_SESSION 
     to remember last search next time*/
  function save_object()
  {
    $faxreportfilter = get_global("faxreportfilter");
    if(isset($_POST['EntriesPerPage'])){
      $this->range = $_POST['EntriesPerPage'];
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
    foreach( array("year", "month", "search_for", "search_base") as $type){
      if (isset($_POST[$type])){
        $faxreportfilter[$type]= $_POST[$type];

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
    register_global("faxreportfilter",$faxreportfilter);
  }


  /* Return plugin informations for acl handling
    #FIXME You can only read attributes within this report plugin */
  static function plInfo()
  {
  
    return (array(
          "plShortName"     => _("Fax report"),
          "plDescription"   => _("Fax report")."&nbsp;<i>"._("All entries are readonly")."</i>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array(),
          "plPriority"      => 1,                                 // Position in tabs
          "plSection"       => array("administration"),                     // This belongs to personal
          "plCategory"      => array("faxreport" => array("description" => _("Fax reports"),
                                                         "objectClass" => array())),
          "plOptions"       => array(),

          "plProvidedAcls" => array(
            "detailedView"   => _("Detailed view"),
            "id"             => _("Fax ID"),
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
