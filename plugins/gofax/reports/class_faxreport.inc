<?php

class faxreport extends plugin
{
  /* Definitions */
  var $plHeadline= "FAX Reports";
  var $plDescription= "This does something";

  /* For internal use */
  var $start= 0;
  var $search_for= "*";
  var $search_base= "";
  var $year= "";
  var $month= "";
  var $sort= 1;
  var $sort_direction= "down";
  var $report_list= array();
  var $userfilter= "";
  var $ui= NULL;

  /* Constant stuff */
  var $status= array( "SENT", "MAILED", "SERROR", "RERROR", "SBLOCK", "RBLOCK",
			"DELETED", "REQUEUED", "DISABLED", "PRINTED", "DIVERTED",
			"UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
			"UNDEFINED");
  var $fields= array("uid", "queuing_time", "status", "sender_id", "receiver_id", "pages");

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  function faxreport ($config, $ui)
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
        if (!is_global("faxreportfilter")){
                $ui= get_userinfo();
                $base= get_base_from_people($ui->dn);
                $faxfilter= array("year" => date("Y"),
                                "month" => date("m"),
				"search_base" => $base,
				"search_for" => "*");
                register_global("faxreportfilter", $faxfilter);
        }
  }

  function execute()
  {
	/* Get template engine */
	$smarty= get_smarty();
	$faxfilter= get_global("faxreportfilter");
        foreach( array("year", "month", "search_for", "search_base") as $type){
                if (isset($_POST[$type])){
                        $faxfilter[$type]= $_POST[$type];
                }
		$this->$type= $faxfilter[$type];
        }
	register_global("faxreportfilter", $faxfilter);
  
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

	/* Do detail view? */
	if (isset($_GET['detail'])){
		$query = "SELECT id,uid,date_format(queuing_time, '%Y%m%d%H%i%s') as queuing_time,status,sender_id,sender_msn,receiver_id,".
			"receiver_msn,pages,status_message,transfer_time FROM faxlog ".
			"WHERE id=".$_GET['detail'].";";

		/* Connecting, selecting database */
		$cfg= $this->config->data['SERVERS']['FAX'];
		$link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
		if ($link === FALSE){
			print_red(_("Can't connect to fax database, no reports can be shown!"));
			return;
		}
		if (! @mysql_select_db("gofax")){
			print_red(_("Can't select fax database for report generation!"));
			return;
		}

		@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,
			$query, "Database query");
		$result = @mysql_query($query);
		if ($result === false){
			print_red(_("Query for fax database failed!"));
			return;
		}

		$line = mysql_fetch_array($result, MYSQL_ASSOC);
		mysql_close($link);

		if (!preg_match ("/'".$line["uid"]."'/", $this->userfilter)){
			print_red (_("You have no permission to retrieve informations about this fax id!"));
			return;
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
	        $smarty->assign("plug", validate($_GET['plug']));
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
			"(objectClass=goFaxAccount)".
			"(|(uid=$s)(l=$s)(homePhone=$s)".
			"(telephoneNumber=$s)(facsimileTelephoneNumber=$s)(mobile=$s)".
			"(pager=$s)(cn=$s)(givenName=$s)(sn=$s)(personalTitle=$s)".
			"(title=$s)))");

		$fax_users= array();
		while ($attrs= $ldap->fetch()){
			$acl= get_permissions ($ldap->getDN(), $this->ui->subtreeACL);
			$acl= get_module_permission($acl, "fax", $ldap->getDN());

			if (chkacl ($acl, "faxReport") == ""){
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

		/* Connecting, selecting database */
		$cfg= $this->config->data['SERVERS']['FAX'];
		$link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
		if ($link === FALSE){
			print_red(_("Can't connect to fax database, no reports can be shown!"));
			return;
		}
		if (! @mysql_select_db("gofax")){
			print_red(_("Can't select fax database for report generation!"));
			return;
		}

		@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__,
			$query, "Database query");
		$result = @mysql_query($query);
		if ($result === false){
			print_red(_("Query for fax database failed!"));
			return;
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
				$line["id"]."\"><img align=\"middle\" border=0 src=\"".get_template_path('images/info_small.png')."\">&nbsp;".$line["uid"]."</a></td>".
				"<td>$date $hour:$minute</td>".
				"<td>".$this->status[$line["status"]]."</td>".
				"<td>".$line["sender_id"]."</td>".
				"<td>".$line["receiver_id"]."</td>".
				"<td>".$line["pages"]."</td>";
		}

		$this->report_list= $report_list;
		mysql_close($link);
	}

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
		if ($mod >= ($this->start + 20)){
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

	/* Prepare template */
	$smarty->assign("search_for", $this->search_for);
        $smarty->assign("bases", $this->config->idepartments);
        $smarty->assign("base_select", $this->search_base);
	$months= array();
	$months[1]= _("January"); $months[2]= _("February"); $months[3]= _("March"); $months[4]= _("April");
	$months[5]= _("May"); $months[6]= _("June"); $months[7]= _("July"); $months[8]= _("August");
	$months[9]= _("September"); $months[10]= _("October"); $months[11]= _("November"); $months[12]= _("December");
        $smarty->assign("months", $months);
	$smarty->assign("month_select", $this->month);
	$current= date("Y");
	$years= array();
	for ($y= $current - 5; $y<=$current; $y++){
		$years[]= $y;
	}
        $smarty->assign("years", $years);
	$smarty->assign("year_select", $this->year);

	if (isset($fax_users) && count($fax_users)){
		$smarty->assign("search_result", $output);
        	$smarty->assign("range_selector", range_selector(count($this->report_list), $this->start, 20));
	}else{
		$smarty->assign("search_result", "");
	}

	

	/* Show main page */
        $smarty->assign("plug", validate($_GET['plug']));
	$smarty->assign("launchimage", get_template_path('images/launch.png'));
	$smarty->assign("search_image", get_template_path('images/search.png'));
	for($i= 0; $i<7; $i++){
		$smarty->assign("mode$i", "");
	}
	$smarty->assign("mode".$this->sort, "<img src=\"images/sort_".$this->sort_direction.
			".png\" border=0 align=middle>");
        return($smarty->fetch(get_template_path('contents.tpl', TRUE)));
  }

}

?>
