<?php

class fonreport extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone Reports";
  var $plDescription= "This does something";

  /* For internal use */
  var $start= 0;
  var $search_for= "*";
  var $search_base= "";
  var $year= "";
  var $month= "";
  var $sort= 0;
  var $sort_direction= "down";
  var $report_list= array();
  var $userfilter= "";
  var $ui= NULL;
  var $fields= array("calldate", "src", "dst", "channel", "lastapp", "disposition", "duration");
  var $range = 20;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function fonreport ($config, $ui)
  {
	  /* Include config object */
	  $this->config= $config;
	  $this->ui= $ui;

	  /* Try to get matching search base for user provided
		 by 'dn' */
	  $tmp= preg_replace ("/^[^,]+,[^,]+,/", "", $ui->dn);
	  $sb= array_search($tmp, $this->config->departments);
	  if ($sb === FALSE){
		  $sb= "/";
	  }
	  $this->search_base= $sb;

	  /* Get global filter config */
	  if (!is_global("fonfilter")){
		  $ui= get_userinfo();
		  $base= get_base_from_people($ui->dn);
		  $fonfilter= array("year" => date("Y"),
				  "month" => date("m"),
				  "search_base" => $base,
				  "search_for" => "*");
		  register_global("fonfilter", $fonfilter);
	  }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

	/* GVet template engine */
	$smarty= get_smarty();

	foreach($this->fields as $field){
		if(chkacl($this->acl,$field)==""){
			$smarty->assign($field."ACL","");	
		}else{
			$smarty->assign($field."ACL"," disabled ");	
		}
	}

	$months= array();
	$months[1]= _("January"); $months[2]= _("February"); $months[3]= _("March"); $months[4]= _("April");
	$months[5]= _("May"); $months[6]= _("June"); $months[7]= _("July"); $months[8]= _("August");
	$months[9]= _("September"); $months[10]= _("October"); $months[11]= _("November"); $months[12]= _("December");

	/* Prepare template */
	$current= date("Y");
	$years= array();
	for ($y= $current - 5; $y<=$current; $y++){
		$years[]= $y;
	}

	$smarty->assign("plug"			, "?plug=".validate($_GET['plug']));
	$smarty->assign("launchimage"	, get_template_path('images/launch.png'));
	$smarty->assign("search_image"	, get_template_path('images/search.png'));
	$smarty->assign("search_for"	, $this->search_for);
	$smarty->assign("bases"			, $this->config->idepartments);
	$smarty->assign("base_select"	, $this->search_base);
	$smarty->assign("months"		, $months);
	$smarty->assign("month_select"	, $this->month);
	$smarty->assign("years"			, $years);
	$smarty->assign("year_select"	, $this->year);
	$smarty->assign("search_result"	, "");
 	
	/* Connecting, selecting database */
	if (!isset($this->config->data['SERVERS']['FON'])){
		print_red(_("Can't connect to phone database, no reports can be shown!"));
		return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
	}elseif(!is_callable("mysql_connect")){
		print_red(_("There is no mysql extension available, please check your php setup."));
		return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
	}else{

		$cfg= $this->config->data['SERVERS']['FON'];
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


	if(isset($_POST['EntryPerPage'])){
		$this->range = $_POST['EntryPerPage'];
	}

	  $fonfilter= get_global("fonfilter");
	  foreach( array("year", "month", "search_for", "search_base") as $type){
		  if (isset($_POST[$type])){
			  $fonfilter[$type]= $_POST[$type];
		  }
		  $this->$type= $fonfilter[$type];
	  }
	  register_global("fonfilter", $fonfilter);

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

	  /* Search button has been pressed */
	  if ($this->search_for != ""){
		  $this->start= 0;

		  if (is_integer (strpos($this->search_for, "*"))){
			  $s= $this->search_for;
		  } else {
			  $s= "*".$this->search_for."*";
		  }
		  $ldap= $this->config->get_ldap_link();
		  $ldap->cd ($this->search_base);

		  /* Perform ldap search for potential users */
		  $ldap->search ("(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))".
				  "(objectClass=gofonAccount)".
				  "(|(uid=$s)(l=$s)(homePhone=$s)".
				  "(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
				  "(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)".
				  "(title=$s)))");

		  $fon_users= array();
		  while ($attrs= $ldap->fetch()){
			  $acl= get_permissions ($ldap->getDN(), $this->ui->subtreeACL);
			  $acl= get_module_permission($acl, "fax", $ldap->getDN());

			  if (chkacl ($acl, "faxReport") == ""){
				  $fax_users[]= $attrs["uid"][0];
			  }
		  }

		  /* Prepare SQL query */
		  $this->userfilter= "";
		  foreach ($fon_users as $user){
			  $this->userfilter.= "uid = '$user' OR ";
		  }
		  $this->userfilter= preg_replace("/OR $/", "", $this->userfilter);
	  }

	$fields_str = "calldate, ";

	foreach($this->fields as $field){
		if($field == "calldate") continue;	
		if(chkacl($this->acl,$field)==""){
			$fields_str .= $field.", ";
		}
	}

	$fields_str = preg_replace("/, $/","",$fields_str);

	if ($this->sort_direction == "down"){
		$desc= "DESC";
	} else {
		$desc= "";
	}
	$start= date ("YmdHis", mktime(0,0,0,$this->month,1,$this->year));
	$end=   date ("YmdHis", mktime(23,59,59,$this->month+1,0,$this->year));
	$query = "SELECT ".$fields_str." FROM cdr ".
		"WHERE calldate <= $end AND ".
		"calldate >= $start ORDER BY ".$this->fields[$this->sort]." $desc;";

	$cfg= $this->config->data['SERVERS']['FON'];
	$link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);

	@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,
			$query, "Database query");

	$result = @mysql_query($query);
	if ($result === false){
		print_red(_("Query for phone database failed!"));
		return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
	}
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
#   }

	/* Generate output */
	$mod= 0;
	if (isset($_GET['start'])){
		$this->start= (int)$_GET['start'];
	}

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
	$smarty->assign("mode".$this->sort, "<img alt=\"\" src=\"images/sort_".$this->sort_direction.
			".png\" border=0 align=middle>");
	$smarty->assign("month_select"	, $this->month);
	$smarty->assign("years"			, $years);
	$smarty->assign("year_select"	, $this->year);
	$smarty->assign("search_result"	, "");
	return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
  }

  function gen_duration($seconds)
  {
  	if ($seconds / 60 > 1){
		$minutes= (int)($seconds / 60);
		$seconds= $seconds % 60;
		return ("$minutes&rsquo;$seconds&rdquo;");
	}
	
  	return ("$seconds&rdquo;");
  }

}

?>
