<?php

class fonreport extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone Reports";
  var $plDescription= "This does something";

  /* For internal use */
  var $start				  = 0;
  var $search_for		  = "*";
  var $search_base	  = "";
  var $fields				  = array("calldate", "src", "dst", "channel", "lastapp", "disposition", "duration");
  var $year					  = "";
  var $month				  = "";
  var $sort					  = 0;
  var $sort_direction	= "down";
  var $report_list		= array();
  var $userfilter			= "";
  var $ui					    = NULL;
  var $range 				  = 20;
  var $EntryPerPage   = 20;
  var $selected_server = "";

  /* attribute list for save action */
  var $attributes_SO  = array("start","search_for","search_base","range","month","sort_direction","sort","year");
  var $objectclasses  = array();

  
  /* Construct class */
  function fonreport ($config, $ui)
  {
    /* Include config object */
    $this->config			= $config;
    $this->ui				  = $ui;
    $this->search_base= get_base_from_people($ui->dn);
  
    $this->month      = date("m");
    $this->year       = date("Y");

    /* Use filter settings if we have already searched */
    if (!is_global("fonfilter")){
      $fonfilter = array();
      foreach($this->attributes_SO as $name){
        $fonfilter[$name]=$this->$name;
      }
      register_global("fonfilter", $fonfilter);
    }else{
      $fonfilter = get_global("fonfilter");
      foreach($this->attributes_SO as $name){
        $this->$name = $fonfilter[$name];
      }
    }
  }


  /* Save ui interactions and store results in session, 
      to remember settings */
  function save_object()
  {
    $fonfilter= get_global("fonfilter");
    if(isset($_POST['EntryPerPage'])){
      $this->range = $_POST['EntryPerPage'];
    }
    if (isset($_GET['start'])){
      $this->start= (int)$_GET['start'];
    }
    foreach( array("year", "month", "search_for", "search_base","selected_server") as $type){
      if (isset($_POST[$type])){
        $this->$type= $_POST[$type];
      }
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
      if ($this->sort < 0 || $this->sort > 6){
        $this->sort= 0;
      }
    }

    /* remove unwanted tags */
    $this->search_for = stripslashes(preg_replace("/[^0-9a-z\*\+ \-]/i","",$this->search_for));
  
    foreach($this->attributes_SO as $name){
      $fonfilter[$name] = $this->$name;
    }
    register_global("fonfilter", $fonfilter);
  }


  /* Search & display results */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* GVet template engine */
    $smarty= get_smarty();


    /*****************
      Variable Init
     *****************/

    $fields_str = "";

    $months= array();
    for($i = 1 ; $i <= 12 ; $i ++ ){
      $months[$i] = _(date("F",gmmktime(0,0,0,$i)));
    }

    /* Prepare template */
    $current= date("Y");
    $years= array();
    for ($y= $current - 5; $y<=$current; $y++){
      $years[$y]= $y;
    }

    /*****************
      Smarty 
     *****************/

    foreach($this->fields as $field){
      if(chkacl($this->acl,$field)==""){
        $smarty->assign($field."ACL","");	
      }else{
        $smarty->assign($field."ACL"," disabled ");	
      }
    }

    $smarty->assign("plug",           "?plug=".validate($_GET['plug']));
    $smarty->assign("launchimage",    get_template_path('images/launch.png'));
    $smarty->assign("search_image",   get_template_path('images/search.png'));
    $smarty->assign("search_for",     $this->search_for);
    $smarty->assign("bases",          $this->config->idepartments);
    $smarty->assign("base_select",    $this->search_base);
    $smarty->assign("months",         $months);
    $smarty->assign("month_select",   $this->month);
    $smarty->assign("years",          $years);
    $smarty->assign("year_select",    $this->year);
    $smarty->assign("search_result",  "");


    /*****************
      Check Database , Table , Connection 
     *****************/

    // Collect servers and allow to select the server in the ui.
    $servers = array();
    foreach($this->config->data['SERVERS']['FON'] as $key => $server){
      $servers[$server['SERVER']] = $server['SERVER'];
    }
    $smarty->assign("servers",  $servers);
    $smarty->assign("selected_server",  $this->selected_server);


    /* Connecting, selecting database */
    if (!isset($this->config->data['SERVERS']['FON'][0])){
      print_red(_("Can't connect to phone database, no reports can be shown!"));
      return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }elseif(!is_callable("mysql_connect")){
      print_red(_("There is no mysql extension available, please check your php setup."));
      return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }else{

      // Get CFG for the selected server, if empty use first.
      if($this->selected_server == ""){
        $cfg= $this->config->data['SERVERS']['FON'][0];
      }else{
        foreach($this->config->data['SERVERS']['FON'] as $server){
          if($server['SERVER'] == $this->selected_server){
            $cfg = $server; 
          }
        }
      }
      
      // Try to connect 
      $link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
      if ($link === FALSE){
        print_red(_("Can't connect to phone database, no reports can be shown!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }
      if (! @mysql_select_db("gophone")){
        print_red(_("Can't select phone database for report generation!"));
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
      }
    }


    /*****************
      Get Query String && Search
     *****************/

    $query  = $this->CreateQuerySyntax();
    $link   = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);

    @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,$query, "Database query");

    $result = @mysql_query($query);
    if ($result === false){
      print_red(_("Query for phone database failed!"));
      return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
    }

    /*****************
      Fetch results 
     *****************/
  
    $report_list= array();
    while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
      $append_str = "";
      $hour=  substr($line["calldate"], 11, 2);
      $minute=substr($line["calldate"], 14, 2);
      $format= _("Y-M-D");
      $date= preg_replace("/Y/", substr($line["calldate"], 0, 4), $format);
      $date= preg_replace("/M/", substr($line["calldate"], 5, 2), $date);
      $date= preg_replace("/D/", substr($line["calldate"], 8, 2), $date);
      $append_str .= 	"<td>$date $hour:$minute</td>";
      foreach(array("src","dst","channel","lastapp","disposition") as $atr){
        if(isset($line[$atr])){
          $append_str .= 	"<td>".$line[$atr]."</td>";
        }
      }
      if(isset($line['duration'])){
        $append_str .= 	"<td>".$this->gen_duration($line["duration"])."</td>";
      }
      $report_list[] = $append_str;
    }

    $this->report_list= $report_list;
    @mysql_close($link);


    /*****************
      Create list of results  
     *****************/

    /* Generate output */
    $mod      = 0;
    $output   = "";
    if(count($this->report_list) < $this->start){
      $this->start = 0;
    }
    foreach($this->report_list as $val){
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

    /*****************
      Tell smarty what we have found  
     *****************/

    if ($output != ""){
      $smarty->assign("search_result", $output);
      $smarty->assign("range_selector", range_selector(count($this->report_list), $this->start,$this->range,"EntryPerPage"));
    } else {
      $smarty->assign("search_result", "");
    }

    /* Show main page */
    for($i= 0; $i<7; $i++){
      $smarty->assign("mode$i", "");
    }
    $smarty->assign("mode".$this->sort, "<img alt=\"\" src=\"images/sort_".$this->sort_direction.".png\" border=0 align=middle>");

    return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
  }

  
  /* Create duration string   12'11" */
  function gen_duration($seconds)
  {
    if ($seconds / 60 > 1){
      $minutes= (int)($seconds / 60);
      $seconds= $seconds % 60;
      return ("$minutes&rsquo;$seconds&rdquo;");
    }
    return ("$seconds&rdquo;");
  }


  /* Create WHERE part for our mysql serach */
  function GetUidMatchingFilter()
  {
    $ldap       = $this->config->get_ldap_link();
    $ldap->cd     ($this->search_base);
    $s          = $this->search_for;

    $s          = preg_replace("/\%/","",$s);  
    $s2         = preg_replace("/\*/","%",$s);  

    /* Perform ldap search for potential users */
    $ldap->search ("(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(objectClass=gofonAccount)".
        "(|(uid=$s)(l=$s)(homePhone=$s)(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
        "(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)(title=$s)))" ,array("uid"));
    $str = " AND (";
    $fields = array("dstchannel","channel");
    if($ldap->count()){
      while ($attrs= $ldap->fetch()){
        $uid =  $attrs["uid"][0];
        foreach($fields as $name){
          $str .= $name." like '%".$uid."%' OR ";
        }
      }
    }
    $str .= " channel     like '%".$s."%' OR  
              dstchannel  like '%".$s."%' OR  
              dst         like '".$s2."' OR 
              src         like '".$s2."' OR 
              lastapp     like '".$s2."')"; 
    return($str);
  }


  /* Create query string */
  function CreateQuerySyntax()
  {
    /* Get extended search filter which contain uids and so on */
    $uidstring = $this->GetUidMatchingFilter();

    /* Create string with all fields seperated by ,*/
    $fields_str ="";
    foreach($this->fields as $field){
      if($field == "calldate") continue;	
      if(chkacl($this->acl,$field)==""){
        $fields_str .= $field.", ";
      }
    }
    $fields_str = preg_replace("/, $/","",$fields_str);

    /* Create Sort tag */
    if ($this->sort_direction == "down"){
      $desc= "DESC";
    } else {
      $desc= "ASC";
    }
    /* Create times */
    $start= date ("YmdHis", mktime(0,0,0,$this->month,1,$this->year));
    $end=   date ("YmdHis", mktime(23,59,59,($this->month +1),0,$this->year));
    $query = "SELECT ".$fields_str.",calldate FROM cdr ".
      "WHERE   
      calldate <= $end 
      AND 
      calldate >= $start 
      ". $uidstring." 
      ORDER BY ".$this->fields[$this->sort]." $desc;";
    return($query);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
