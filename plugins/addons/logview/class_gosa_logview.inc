<?php

class gosa_logview extends plugin
{
  /* Definitions */
  var $plHeadline= "System logs";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes     = array();
  var $objectclasses  = array();

  var $server         = "";
  var $action         = "modify";
  var $time           = 2;
  var $regex          = "*";

  var $start  = 0;
  var $sort   = 1;

  var $sort_direction = "down";
  var $fields         = array("action","timestamp","user","objecttype","object","result","repeated");
  var $range          = 25;
  var $view_logged    = FALSE;

  var $timezone_offset  = 0;

  var $uid_map = array();
  

  function gosa_logview (&$config, $dn= NULL)
  {
    /* Include config object */
    $this->config= &$config;
    $this->ui = get_userinfo();

    /* Restore last filter settings */
    $logview = session::get('logview');
    foreach(array("action","server","time","regex") as $attr){
      if(isset($logview['gosa_log'][$attr])){
        $this->$attr = $logview['gosa_log'][$attr];
      }
    }

    $tz = timezone::get_default_timezone();
    $this->timezone_offset = $tz['value'] * 60 * 60 ;

    $ldap = $this->config->get_ldap_link();
    $ldap->search("(&(objectClass=person)(uid=*))",array("uid","dn"));
    while($attrs = $ldap->fetch()){
      $this->uid_map[$attrs['dn']] = $attrs['uid'][0];
    }
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

    /* Time interval */
    $date_select = array(
        "0" => _("one hour"), "1" => _("6 hours"),
        "2" => _("12 hours"), "3" => _("24 hours"),
        "4" => _("2 days"),   "5" => _("one week"),
        "6" => _("2 weeks"),  "7" => _("one month"));


    $time = time();
    $date_select_ = array(
        "0" => $time - 3600, 
        "1" => $time - 21600,
        "2" => $time - 43200,
        "3" => $time - 86400,
        "4" => $time - 172800,
        "5" => $time - 604800,
        "6" => $time - 1209600,
        "7" => $time - 2419200);

    $smarty= get_smarty();
    $smarty->assign("search_result", "");
    $smarty->assign("plug", "?plug=".validate($_GET['plug']));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("time_image", get_template_path('images/time.png'));
    $smarty->assign("server_image", get_template_path('images/server.png'));
    $smarty->assign("log_image", get_template_path('images/log_warning.png'));
    $smarty->assign("ruleset_image", get_template_path('images/edit.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("date_select", $date_select);
    $smarty->assign("actions", array());
    $smarty->assign("direction", "up");
    $smarty->assign("mode0", "");
    $smarty->assign("mode1", "");
    $smarty->assign("mode2", "");
    $smarty->assign("mode3", "");
    $smarty->assign("mode4", "");
    $smarty->assign("mode5", "");
    $smarty->assign("mode6", "");

    /* Assign select option content */
    foreach( array("server", "action", "time", "regex") as $type){
      $smarty->assign("$type", $this->$type);
    }

    /**** 
     * Check if defined servers 
     *  and mysql extension 
     ****/
    if (!isset($this->config->data['SERVERS']['LOGGING']) || !count($this->config->data['SERVERS']['LOGGING'])){
      print_red (_("No GOsa LOG servers defined!"));
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }
    
    if(!is_callable("mysql_connect")){
      print_red(_("There is no mysql extension available, please check your php setup."));
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    /****
     * Get Configurations 
     ****/
    $list_of_servers = $this->config->data['SERVERS']['LOGGING']; 
    $servers         = array();
    foreach($list_of_servers as $servername => $data){
      $servers[$servername] = $servername;
    }
    $smarty->assign("servers", $servers);
    
    /* Set a default server, if there is currently no valid server selected */
    if(empty($this->server) || !in_array($this->server,$servers)){
      $this->server = key($servers);
    }
   
    /**** 
     * Connect to currently selected server 
     ****/
    $cfg = $list_of_servers[$this->server];
    $link = @mysql_pconnect($this->server, $cfg['USER'], $cfg['PWD']);
    if ($link === FALSE){
      print_red(_("Can't connect to log database, no logs can be shown!"));
      new log("debug","logview","",array(),@mysql_error());
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    /* check of log database is available */
    if (! @mysql_select_db($cfg['DB'])){
      print_red(_("Can't select log database for log generation!"));
      new log("debug","logview","",array(),@mysql_error());
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }

    
    /****
     * Get all action types 
     ****/

    $actions = array("!ALL" => _("All"));
    $query = "SELECT distinct(action) FROM gosa_log;";
    $res = @mysql_query($query);
    while($attrs = @mysql_fetch_assoc($res)){
      $actions[$attrs['action']] = $attrs['action'];
    } 


    /****
     * Check Acls 
     ****/

    $ui = get_userinfo();
    $dn = $list_of_servers[$this->server]['DN'];
    $acls = $ui->get_permissions($dn,"server/gosaLogServer","viewEntries");
    
    if(!preg_match("/r/",$acls)){
      $res = "<tr>
        <td colspan=".count($this->fields).">
        "._("You have insufficient permissions to view syslog entries.")."
        </td>
        </tr>";

      /* Show main page */
      $smarty->assign("range_selector", "");
      $smarty->assign("search_result", $res);
      return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
    }


    /****
     * Query stuff 
     ****/
    $res= "";
    $cfg    = $list_of_servers[$this->server];
    $link   = @mysql_pconnect($this->server, $cfg['USER'], $cfg['PWD']);

    /* Test connection object && create up query string */
    if ($link === FALSE){
      print_red(_("Can't connect to log database, no logs can be shown!"));
      new log("debug","logview","",array(),@mysql_error());
    } else {

      if (! @mysql_select_db($cfg['DB'])){
        print_red(_("Can't select log database for log generation!"));
        new log("debug","logview","",array(),@mysql_error());
      } else {

        /* Prepare order setting */
        if ($this->sort_direction == "down"){
          $desc= "DESC";
          $sort_sign = "<img src='images/sort_down.png' alt='\/' class='center' title='down' border=0>";
        } else {
          $desc= "";
          $sort_sign = "<img src='images/sort_up.png' alt='/\' class='center' title='up' border=0>";
        }

        /* Get start time */
        $start = $date_select_[$this->time];
        
        /* Prepare search filter */
        $sql_regex =trim(preg_replace("/\*/","%",$this->regex));
        $sql_regex = "%".trim($sql_regex,"%")."%";

        /* Create search filter */
        $query_base= "FROM  gosa_log WHERE timestamp >= $start ";
  
        /* Append action filter */
        if($this->action != "!ALL"){
          $query_base .=" AND action like '".$this->action."' ";
        }

        /* Append search filter */
        if($sql_regex != "%%"){
          $query_base .=" AND ( result like '".$sql_regex."' OR user like '".$sql_regex."') ";
        }

        /* Get number of entries */
        $query= "SELECT COUNT(`user`) ".$query_base.";";
        $result = mysql_query($query);
        if(!$result){
          new log("debug","logview","",array(),@mysql_error());
        }
        $line= mysql_fetch_array($result, MYSQL_ASSOC);
        $count= $line['COUNT(`user`)'];
        if ($count > 25){
          $smarty->assign("range_selector", range_selector($count, $this->start, $this->range,"EntriesPerPage"));
        } else {
          $smarty->assign("range_selector", "");
        }

        /* Query results that will be displayed */
        $query= "SELECT * ".$query_base." ORDER BY ".$this->fields[$this->sort]." ".$desc." LIMIT  ".$this->start.",".$this->range.";";
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

          $base = ", ".substr($this->config->current['BASE'],0,5)."...";
  
          if(isset($this->uid_map[$line['user']])){
            $user = $this->uid_map[$line['user']];
          }else{
            $user = preg_replace("/,".normalizePreg($this->config->current['BASE'])."/",$base,$line['user']);
          }
          $object = preg_replace("/,".normalizePreg($this->config->current['BASE'])."/",$base,$line['object']);

          $res.=" <tr style=\"$col\">\n";
          $res.="   <td title='".$line['objecttype']."'>".$line['action']."</td>";
          $res.="   <td>".date("H:i:s d.m.Y",($line['timestamp'] + $this->timezone_offset))."</td>";
          $res.="   <td title='".$line['user']."'>".preg_replace("/,/",", ",$user)."</td>"; 
          $res.="   <td title='".$line['objecttype']."'>".$line['objecttype']."</td>"; 
          $res.="   <td title='".$line['object']."'>".preg_replace("/,/",", ",$object)."</td>"; 
          $res.="   <td>".$line['result']."</td>";
#         if($line['repeated'] == 1){
#           $res.="   <td>-</td>";
#         }else{
#           $res.="   <td>".($line['repeated']-1)."</td>";
#         }
          $res.=" </tr>\n";
        }
        mysql_close($link);
        $smarty->assign("mode".$this->sort, $sort_sign);
        $smarty->assign("search_result", $res);
      }
    }

    $smarty->assign("regex", $this->regex);
    $smarty->assign("server",$this->server);
    $smarty->assign("servers",$servers);
    $smarty->assign("action",$this->action);
    $smarty->assign("actions",$actions);
    $smarty->assign("date_select", $date_select);

    $smarty->assign("direction", $this->sort_direction);

    /* Show main page */
    return ($smarty->fetch (get_template_path('gosa_log_contents.tpl', TRUE)));
  }


  function save_object()
  {
    if(isset($_POST['gosa_log_contents_posted'])){

      /* Get submitted range */	
      if(isset($_POST['EntriesPerPage'])){
        if(is_numeric($_POST['EntriesPerPage'])){
          $this->range = $_POST['EntriesPerPage'];
        }
      }

      /* Get actual select boxe values */
      $logfilter_changed = FALSE;
      foreach( array("server", "action","time","regex") as $type){
        if(isset($_POST[$type])){
          $this->$type = $_POST[$type];   
        }
      }

      /* Filter regex values */
      if ($this->regex == ""){
        $this->regex= '*';
      } else {
        $new = preg_replace('/\*\**/', '*', $this->regex);
        $this->regex= $new;
      }

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

        if($_GET['direction'] == "up"){
          $this->sort_direction = "down";
        }else{
          $this->sort_direction = "up";
        }

        $this->sort= (int)$_GET['sort'];
        if ($this->sort < 0 || $this->sort > count($this->fields)){
          $this->sort= 0;
        }
      }

      /* Save attributes in Sessiob, so we are able to recontruct filter on plugin reload */
      $logview = session::get('logview');
      $logview['gosa_log'] = array();
      foreach(array("action","server","time","regex") as $attr){
        $logview['gosa_log'][$attr] = $this->$attr;
      }
      session::set('logview',$logview);
    }
  }


  /* Return plugin informations for acl handling
     #FIXME You can only read attributes within this report plugin 
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
  */
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
