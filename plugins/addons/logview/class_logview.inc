<?php

class logview extends plugin
{
  /* Definitions */
  var $plHeadline= "System logs";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $start= 0;
  var $sort= 2;
  var $sort_direction= "down";
  var $hostlist= array();
  var $loglevellist= array();
  var $tilist= array();
  var $fields= array("log_level", "host", "time_stamp", "message");
  var $last= array("log_level", "host", "time", "regex");
  var $range = 25;

  function logview ($config, $dn= NULL)
  {
	/* Include config object */
	$this->config= $config;

        /* Get global filter config */
        if (!is_global("logfilter")){
                $logfilter= array("time" => "1",
                                "log_level" => "!All",
                                "host" => "!All",
                                "regex" => "*");
                register_global("logfilter", $logfilter);
        }


  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

	$logfilter= get_global("logfilter");
	$smarty= get_smarty();
	$smarty->assign("search_result", "");
	$smarty->assign("plug", "?plug=".validate($_GET['plug']));
	$smarty->assign("search_image", get_template_path('images/search.png'));
	$smarty->assign("time_image", get_template_path('images/time.png'));
	$smarty->assign("server_image", get_template_path('images/server.png'));
	$smarty->assign("log_image", get_template_path('images/log_warning.png'));
	$smarty->assign("ruleset_image", get_template_path('images/edit.png'));
	$smarty->assign("launchimage", get_template_path('images/launch.png'));
	$smarty->assign("hostlist", $this->hostlist);
	$smarty->assign("loglevellist", $this->loglevellist);
	$smarty->assign("tilist", $this->tilist);
	$smarty->assign("mode0", "");
	$smarty->assign("mode1", "");
	$smarty->assign("mode2", "");
	$smarty->assign("mode3", "");

	$logfilter['regex']= preg_replace('/\%/', '*', $logfilter['regex']);
	foreach( array("host", "log_level", "time", "regex") as $type){
		$smarty->assign("$type", $logfilter[$type]);
	}

	if (!isset($this->config->data['SERVERS']['LOG'])){
		print_red (_("No LOG servers defined!"));
		return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
	}elseif(!is_callable("mysql_connect")){
		print_red(_("There is no mysql extension available, please check your php setup."));
		return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
	}else{
		$cfg= $this->config->data['SERVERS']['LOG'];
		
		$link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
		if ($link === FALSE){
			print_red(_("Can't connect to log database, no logs can be shown!"));
			return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
		}
		if (! @mysql_select_db("gomon")){
			print_red(_("Can't select log database for log generation!"));
			return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
		}

		/* Host list */
		$query= "SELECT DISTINCT host FROM golog LIMIT 200;";
		@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
		$result = @mysql_query($query);
		if ($result === false){
			print_red(_("Query for log database failed!"));
			return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
		}
		
		if(count($this->hostlist) == 0){
			while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){
				$this->hostlist[$line['host']]= $line['host'];
			}
			$this->hostlist['!All']= _("All");
			ksort($this->hostlist);
			$smarty->assign("hostlist", $this->hostlist);
		}

		/* Log level list */
		$query= "SELECT DISTINCT log_level FROM golog LIMIT 200;";
		@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
		$result = @mysql_query($query);
		if ($result === false){
			print_red(_("Query for log database failed!"));
			return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
		}
		
		if(count($this->loglevellist) == 0){
			while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){
				$this->loglevellist[$line['log_level']]= $line['log_level'];
			}
			$this->loglevellist['!All']= _("All");
			ksort($this->loglevellist);
			$smarty->assign("loglevellist", $this->loglevellist);
		}
	}

	if(count($this->tilist) == 0){
		/* Time interval */
		$this->tilist= array("0" => _("one hour"), "1" => _("6 hours"),
				"2" => _("12 hours"), "3" => _("24 hours"),
				"4" => _("2 days"), "5" => _("one week"),
				"6" => _("2 weeks"), "7" => _("one month"));
		$smarty->assign("tilist", $this->tilist);
	}
	
	if(isset($_POST['EntriesPerPage'])){
		$this->range = $_POST['EntriesPerPage'];
	}

	/* Save data */
	$logfilter= get_global("logfilter");
	$logfilter_changed = 0;
	foreach( array("host", "time", "log_level", "regex") as $type){
		$last[$type] = $logfilter[$type];
		if (isset($_POST[$type])){
			$logfilter[$type]= $_POST[$type];
		}
		if ($last[$type] != $logfilter[$type]){
			$logfilter_changed = 1;
		}
	}
	$smarty->assign("regex", $logfilter['regex']);
	if ($logfilter['regex'] == ""){
		$logfilter['regex']= '%';
	} else {
		$logfilter['regex']= preg_replace('/\*/', '%', $logfilter['regex']);
	}
	register_global("logfilter", $logfilter);

	if (isset($_GET['start'])){
		$this->start= (int)$_GET['start'];
	}
	if ($logfilter_changed > 0){
		$this->start= 0;
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
		if ($this->sort < 0 || $this->sort > 3){
			$this->sort= 0;
		}
	}

	/* Query stuff */
	$res= "";
	$cfg= $this->config->data['SERVERS']['LOG'];
	$tmp= set_error_handler('dummy_error_handler');
	$link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
	set_error_handler($tmp);
	if ($link === FALSE){
		print_red(_("Can't connect to log database, no logs can be shown!"));
	} else {
		if (! @mysql_select_db("gomon")){
			print_red(_("Can't select log database for log generation!"));
		} else {

			/* Assemble time query */
			switch ($logfilter['time']){
				case '0':
					$start= date ("YmdHis", time() - 3600);
					break;
					;;
				case '1':
					$start= date ("YmdHis", time() - 21600);
					break;
					;;
				case '2':
					$start= date ("YmdHis", time() - 43200);
					break;
					;;
				case '3':
					$start= date ("YmdHis", time() - 86400);
					break;
					;;
				case '4':
					$start= date ("YmdHis", time() - 172800);
					break;
					;;
				case '5':
					$start= date ("YmdHis", time() - 604800);
					break;
					;;
				case '6':
					$start= date ("YmdHis", time() - 1209600);
					break;
					;;
				case '7':
					$start= date ("YmdHis", time() - 2419200);
					break;
					;;
			}

			/* Assemble log level query */
			if ($logfilter['log_level'] == '!All'){
				$ll= "";
			} else {
				$ll= "AND log_level='".$logfilter['log_level']."'";
			}
			$smarty->assign("log_level", $logfilter['log_level']);
			if ($logfilter['host'] == '!All'){
				$hf= "";
			} else {
				$hf= "AND host='".$logfilter['host']."'";
			}
			$smarty->assign("host", $logfilter['host']);

			/* Order setting */
			if ($this->sort_direction == "down"){
				$desc= "DESC";
				$sort_sign = "\\/";
			} else {
				$desc= "";
				$sort_sign="/\\";
			}
			$smarty->assign("mode".$this->sort, $sort_sign);

			$query_base= " FROM golog WHERE message like '".$logfilter['regex']."' $ll $hf AND time_stamp >= $start";

			/* Get number of entries */
			$query= "SELECT COUNT(*)".$query_base.";";
			$result = @mysql_query($query);
			$line= mysql_fetch_array($result, MYSQL_ASSOC);
			$count= $line['COUNT(*)'];
			if ($count > 25){
				$smarty->assign("range_selector", range_selector($count, $this->start, $this->range,"EntriesPerPage"));
			} else {
				$smarty->assign("range_selector", "");
			}

			/* Query results */
			$query= "SELECT *".$query_base." ORDER BY ".$this->fields[$this->sort]." $desc LIMIT ".$this->start.",".$this->range.";";
			@DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
			$result = @mysql_query($query);

			/* Display results */
			$mod= 0;
			while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){
				if ( ($mod++) & 1){
					$col= "background-color: #ECECEC;";
				} else {
					$col= "background-color: #F5F5F5;";
				}

				$res.="<tr style=\"$col\">\n";
				$res.="<td style=\"text-align:center\">
					<img alt=\"".$line['log_level']."\" src=\"".get_template_path('images/log_'.strtolower($line['log_level'])).".png\" title=\"Log level is '".$line['log_level']."'\"></td><td>".$line['host']."</td>";
				$res.="<td>".$line['time_stamp']."</td><td width=\"100%\">".$line['message']."</td>";
				$res.="</tr>\n";
			}
			mysql_close($link);
			$smarty->assign("search_result", $res);
		}
	}
	$logfilter['regex']= preg_replace('/\%/', '*', $logfilter['regex']);
	foreach( array("host", "log_level", "time", "regex") as $type){
		$smarty->assign("$type", $logfilter[$type]);
	}

	/* Show main page */
	return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }

}

?>
