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
  var $view_logged = FALSE;

  function logview (&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;

    /* Get global filter config */
    if (!session::is_set("logfilter")){
      $logfilter= array(
          "time" => "1",
          "log_level" => "!All",
          "host" => "!All",
          "regex" => "*");

      session::set("logfilter", $logfilter);
    }

    $this->ui = get_userinfo();
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if(!$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","logview/".get_class($this),$this->dn);
    }

    $logfilter= session::get("logfilter");
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

    /* Assign select option content */
    foreach( array("host", "log_level", "time", "regex") as $type){
      $smarty->assign("$type", $logfilter[$type]);
    }

    /* Test connection to log database */
    if (!isset($this->config->data['SERVERS']['LOG'])){
      msg_dialog::display(_("Warning"), sprintf(_("No %s servers defined!"), "syslog"), WARNING_DIALOG);
      return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));

    }elseif(!is_callable("mysql_connect")){
      msg_dialog::display(_("Configuration error"), sprintf(_("Missing %s PHP extension!"), "mysql"), WARNING_DIALOG);
      return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));

    }else{

      /* Cehck connection informations */	
      $cfg= $this->config->data['SERVERS']['LOG'];

      /* Open link to database  and check if it is valid */	
      $link = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
      if ($link === FALSE){
	msg_dialog::display(_("Error"), sprintf(_("Cannot connect to %s database!"), "syslog"), ERROR_DIALOG);
        new log("debug","logview","",array(),@mysql_error());
        return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
      }

      /* check of log database is available */
      if (! @mysql_select_db("gomon")){
        msg_dialog::display(_("Error"), sprintf(_("Cannot select %s database!"), "syslog"), ERROR_DIALOG);
        new log("debug","logview","",array(),@mysql_error());
        return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
      }

      /* Get Host list, if still empty */
      if(count($this->hostlist) == 0){

        /* Query database and check results */
        $query= "SELECT DISTINCT host FROM golog LIMIT 200;";
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
        $result = @mysql_query($query);
        if ($result === false){
	  msg_dialog::display(_("Error"), _("Cannot obtain host list from syslog database!"), ERROR_DIALOG);
          new log("debug","logview","",array(),@mysql_error());
          return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
        }

        /* Add hostnames to list */	
        while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){
          $this->hostlist[$line['host']]= $line['host'];
        }
        $this->hostlist['!All']= _("All");
        ksort($this->hostlist);
        $smarty->assign("hostlist", $this->hostlist);
      }

      /* Get log level list */
      if(count($this->loglevellist) == 0){

        /* Try to get all used log level types */
        $query= "SELECT DISTINCT log_level FROM golog LIMIT 200;";
        @DEBUG (DEBUG_MYSQL, __LINE__, __FUNCTION__, __FILE__, $query, "Database query");
        $result = @mysql_query($query);
        if ($result === false){
          new log("debug","logview","",array(),@mysql_error());
	  msg_dialog::display(_("Error"), _("Database query failed!"), ERROR_DIALOG);
          return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
        }

        /* Add each etry to log level list */
        while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){
          $this->loglevellist[$line['log_level']]= $line['log_level'];
        }
        $this->loglevellist['!All']= _("All");
        ksort($this->loglevellist);
        $smarty->assign("loglevellist", $this->loglevellist);
      }
    }

    /* Set list of available time sequences */
    if(count($this->tilist) == 0){

      /* Time interval */
      $this->tilist= array("0" => _("one hour"), "1" => _("6 hours"),
          "2" => _("12 hours"), "3" => _("24 hours"),
          "4" => _("2 days"), "5" => _("one week"),
          "6" => _("2 weeks"), "7" => _("one month"));
      $smarty->assign("tilist", $this->tilist);
    }

    $smarty->assign("regex", $logfilter['regex']);


    /* Get acls */
    
    $tmp_cat_bases = $this->ui->get_module_departments("logview");
    $all_acls = "";
    foreach($tmp_cat_bases as $acl_base){
      $all_acls .= $this->ui->get_permissions($acl_base,"logview/logview");
    }
    if(count($tmp_cat_bases) == 0 || !preg_match("/r/",$all_acls)){
      $res = "<tr>
        <td colspan=4>
        "._("You have insufficient permissions to view syslog entries.")."
        </td>
        </tr>";

      /* Show main page */
      $smarty->assign("range_selector", "");
      $smarty->assign("search_result", $res);
      return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
    }

    /* Query stuff */
    $res= "";
    $cfg    = $this->config->data['SERVERS']['LOG'];
    $tmp    = set_error_handler('dummy_error_handler');
    $link   = @mysql_pconnect($cfg['SERVER'], $cfg['LOGIN'], $cfg['PASSWORD']);
    set_error_handler($tmp);

    /* Test connection object && create up query string */
    if ($link === FALSE){
      msg_dialog::display(_("Error"), sprintf(_("Cannot connect to %s database!"), "syslog"), ERROR_DIALOG);
      new log("debug","logview","",array(),@mysql_error());
    } else {
      if (! @mysql_select_db("gomon")){
        msg_dialog::display(_("Error"), sprintf(_("Cannot select %s database!"), "syslog"), ERROR_DIALOG);
        new log("debug","logview","",array(),@mysql_error());
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
        if ($logfilter['host'] == '!All'){
          $hf= "";
        } else {
          $hf= "AND host='".$logfilter['host']."'";
        }

        /* Order setting */
        if ($this->sort_direction == "down"){
          $desc= "DESC";
          $sort_sign = "\\/";
        } else {
          $desc= "";
          $sort_sign="/\\";
        }
        $end= date ("YmdHis");
        $query_base= "  FROM 
          golog 
          WHERE 
          message like '".preg_replace("/\*/","%",$logfilter['regex'])."' 
          $ll 
          $hf 
          AND 
          time_stamp <= $end AND time_stamp >= $start";

        /* Get number of entries */
        $query= "SELECT COUNT(*)".$query_base.";";
        $result = @mysql_query($query);
        if(!$result){
          new log("debug","logview","",array(),@mysql_error());
        }
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
        if(!$result){
          new log("debug","logview","",array(),@mysql_error());
        }
        /* Display results */
        $mod= 0;

        /* Add entries to result str */
        while ($line = mysql_fetch_array($result, MYSQL_ASSOC)){

          /* Toggle background color */
          if ( ($mod++) & 1){
            $col= "background-color: #ECECEC;";
          } else {
            $col= "background-color: #F5F5F5;";
          }

          $res.=" <tr style=\"$col\">\n";
          $res.="   <td style=\"text-align:center\">
            <img alt=\"".$line['log_level']."\" 
            src=\"".get_template_path('images/log_'.strtolower($line['log_level'])).".png\" 
            title=\"Log level is '".$line['log_level']."'\">
            </td>";
          $res.="   <td>".
            $line['host']."
            </td>"; 
            $res.=" <td>".
            $line['time_stamp']."
            </td>";
          $res .= "<td width=\"100%\">".
            $line['message']."
            </td>";
          $res.=" </tr>\n";
        }
        mysql_close($link);
        $smarty->assign("mode".$this->sort, $sort_sign);
        $smarty->assign("host", $logfilter['host']);
        $smarty->assign("log_level", $logfilter['log_level']);

        $smarty->assign("search_result", $res);
      }
    }

    /* Show main page */
    return ($smarty->fetch (get_template_path('contents.tpl', TRUE)));
  }


  function save_object()
  {
    /* Get submitted range */	
    if(isset($_POST['EntriesPerPage'])){
      if(is_numeric($_POST['EntriesPerPage'])){
        $this->range = $_POST['EntriesPerPage'];
      }
    }

    /* Save data */
    $logfilter= session::get("logfilter");

    /* Get actual select boxe values */
    $logfilter_changed = 0;
    foreach( array("host", "time", "log_level", "regex") as $type){
    
      /* Set new value and test if value has changed */
      $last[$type] = $logfilter[$type];
      if (isset($_POST[$type])){
        $logfilter[$type]= $_POST[$type];
      }
  
      if ($last[$type] != $logfilter[$type]){
        $logfilter_changed = 1;
      }
    }

    /* Filter regex values */
    if ($logfilter['regex'] == ""){
      $logfilter['regex']= '%';
    } else {
      $new = preg_replace('/\*\**/', '*', $logfilter['regex']);
      $logfilter['regex']= $new;
    }

    /* Store filter values */
    session::set("logfilter", $logfilter);

    /* Set start value */
    if (isset($_GET['start'])){
      $this->start= (int)$_GET['start'];
    }
  
    /* Reset page number if filter has changed */
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
  }


  /* Return plugin informations for acl handling
     #FIXME You can only read attributes within this report plugin */
  static function plInfo()
  {
    return (array(
        "plShortName"   => _("Log view"),
        "plDescription" => _("Log view addon"),
        "plSelfModify"  => FALSE,
        "plDepends"     => array(),
        "plPriority"    => 0,
        "plSection"     => array("addon"),
        "plCategory"    => array("logview" => array("objectClass" => "none", "description" => _("System logs"))),

        "plProvidedAcls" => array()
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
