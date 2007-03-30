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

  /* Constant stuff */
  var $status= array( "SENT", "MAILED", "SERROR", "RERROR", "SBLOCK", "RBLOCK",
      "DELETED", "REQUEUED", "DISABLED", "PRINTED", "DIVERTED",
      "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
      "UNDEFINED");
  var $fields= array("uid", "queuing_time", "status", "sender_id", "receiver_id", "pages");

  /* these vars will be stored in session to be able to remember last search config */
  var $attributes_SO= array("search_for","search_base","month","year","start","year","month","sort","sort_direction","range");
  var $objectclasses= array();

  /* Create class */
  function faxreport ($config, $ui)
  {
    /* Include config object */
    $this->config       = $config;
    $this->ui           = $ui;
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
      print_red(_("No fax extension defined in your sever configuration, no reports can be shown!"));
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

      $parts= array( "id", "uid", "queuing_time", "status", "sender_id", "sender_msn",
          "receiver_id", "receiver_msn", "pages", "status_message", "transfer_time" );
      foreach ($parts as $vname) {
        $final="fax_$vname";
        if ($line[$vname] != ""){
          $smarty->assign("$final", $line[$vname]);
        } else {
          $smarty->assign("$final", "-");
        }
      }
      $queuing_time= $line['queuing_time'];

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
      $ldap->search ("(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))".
          "(objectClass=goFaxAccount)".
          "(|(uid=$s)(l=$s)(homePhone=$s)".
          "(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
          "(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)".
          "(title=$s)))");

      $fax_users= array();
      while ($attrs= $ldap->fetch()){
        $ui = get_userinfo();
        $acl= get_permissions ($ui->dn, $ui->subtreeACL);
        $acl2= get_module_permission($acl, "faxreport", $attrs['dn']);
        
        if (chkacl ($acl2, "faxreport") == ""){
          $fax_users[]= $attrs["uid"][0];
        }
      }

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

      $report_list= array();
      while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
        $hour=  substr($line["queuing_time"], 8, 2);
        $minute=substr($line["queuing_time"], 10, 2);
        $format= _("Y-M-D");
        $date= preg_replace("/Y/", substr($line["queuing_time"], 0, 4), $format);
        $date= preg_replace("/M/", substr($line["queuing_time"], 4, 2), $date);
        $date= preg_replace("/D/", substr($line["queuing_time"], 6, 2), $date);


        $report_list[]= "<td class=\"phonelist\"><a href=\"main.php?plug=".validate($_GET['plug'])."&amp;detail=".
          $line["id"]."\"><img alt=\"\" align=\"middle\" border=0 src=\"".get_template_path('images/info_small.png')."\">&nbsp;".$line["uid"]."</a></td>".
          "<td>$date $hour:$minute</td>".
          "<td>".$this->status[$line["status"]]."</td>".
          "<td>".$line["sender_id"]."</td>".
          "<td>".$line["receiver_id"]."</td>".
          "<td>".$line["pages"]."</td>";
      }

      $this->report_list= $report_list;
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
      }
      $this->$type= $faxreportfilter[$type];
    }
    foreach($this->attributes_SO as $name){
      $faxreportfilter[$name] = $this->$name;
    }  
    register_global("faxreportfilter",$faxreportfilter);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
