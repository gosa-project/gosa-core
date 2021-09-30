<?php

class rsyslog extends plugin
{
  var $plHeadline= "System logs";
  var $plDescription= "View recorded systemlogs";
  var $matIcon = "summarize";

  var $servers = array();
  var $selected_server = "";
  var $selected_host = "all";
  var $selected_priority = "all";

  var $startTime = "";
  var $stopTime  = "";

  var $search_for ="";

  var $sort_value  = "DeviceReportedTime";
  var $sort_type  = "DESC";
  var $limit  = "0";
  var $limits = array(20,50,100,200,500,1000,'-');
  var $page   = 0;

  function __construct(&$config, $dn= NULL)
  {
      $this->initTime = microtime(TRUE);

      // Create statistic table entry
      stats::log('plugin', $class = get_class($this), $category = array($this->acl_category),  $action = 'open',
              $amount = 1, $duration = (microtime(TRUE) - $this->initTime));


    $this->config= &$config;
    $this->ui = get_userinfo();

    $this->startTime = date("d.m.Y", (time() - 7*24*60*60));
    $this->stopTime  = date("d.m.Y", time());

    // Get list of rsyslog servers 
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("objectClass=goLogDBServer",array("cn","goLogAdmin","gosaLogDB","goLogPassword"));
    while($attrs = $ldap->fetch()){
      if(empty($this->selected_server)) $this->selected_server = $attrs['cn'][0];
      $s = array('cn' => $attrs['cn'][0], 'dn' => $attrs['dn'], 'Password' => '', 'status' => 'ok');
      $s['User'] = $attrs['goLogAdmin'][0]; 
      $s['Database'] = $attrs['gosaLogDB'][0]; 
      $s['Hosts'] = array();
      $s['Priorities'] = array();
      $s['ACL'] = "";
      if(isset($attrs['goLogPassword'])) $s['Password'] = $attrs['goLogPassword'][0];
      $this->servers[$attrs['cn'][0]] = $s;
    }

    // Check for installed mysql extension, if missing abort
    $this->mysqli_extension_installed = is_callable("mysqli_connect");
    if(!$this->mysqli_extension_installed) return;

    // Test server connetivity 
    $ui = get_userinfo();
    foreach($this->servers as $name => $server)  
    {
      $link = @mysqli_connect($server['cn'], $server['User'], $server['Password'], $server['Database']);
      if ($link === FALSE){
        new log("debug","gosa_logging","dbconnect",array(),@mysqli_error());
        $this->servers[$name]['status'] = "connectfailed";
        $this->servers[$name]['error']  = @mysqli_error();
        continue;
      }else{
        $this->servers[$name]['status'] = "ok";
        $this->servers[$name]['error']  = "";

        // Detect ACLs
        $this->servers[$name]['ACL'] =  $ui->get_permissions($server['dn'], 'server/rSyslogServer','viewEntries');

        // Get list of Hosts using this log server..
        if(preg_match('/r/', $this->servers[$name]['ACL'])){
          $query = "SELECT distinct(FromHost) FROM SystemEvents;";
          $res = @mysqli_query($link,$query);
          if($res){
            while($attrs = @mysqli_fetch_assoc($res)){
              $this->servers[$name]['Hosts'][$attrs['FromHost']] = $attrs['FromHost'];  
            }
          }else{
            $this->servers[$name]['status'] = "query_failed";
            $this->servers[$name]['error']  = @mysqli_error();
          }
          $query = "SELECT distinct(Priority) FROM SystemEvents ORDER BY Priority;";
          $res = @mysqli_query($link, $query);
          if($res){
            while($attrs = @mysqli_fetch_assoc($res)){
              $this->servers[$name]['Priorities'][$attrs['Priority']] = $attrs['Priority'];  
            }
          }else{
            $this->servers[$name]['status'] = "query_failed";
            $this->servers[$name]['error']  = @mysqli_error();
          }
        }
      }
    }
  }

  function execute()
  {
    plugin::execute();

    $smarty = get_smarty();

    $ui = get_userinfo();

    $theme = getThemeName();
    
    if(!isset($this->servers[$this->selected_server]['Hosts']) || !count($this->servers[$this->selected_server]['Hosts'])){
      $hosts = array('-' => _("-"));
    }else{
      $hosts = array('all' => _("All"));
      $hosts = array_merge($hosts,$this->servers[$this->selected_server]['Hosts']);
    }
    if(!isset($this->servers[$this->selected_server]['Priorities']) || !count($this->servers[$this->selected_server]['Priorities'])){
      $priorities = array('-' => _("-"));
    }else{
      $priorities = array('all' => _("All"));
      foreach($this->servers[$this->selected_server]['Priorities'] as $id => $name){
        $priorities[$id] = $this->severity2string($name);
      }
    }

    $result =$this->get_result();

    switch ($theme) {
      case 'classic':
        $downimg = image('images/lists/sort-down.png');
        $upimg = image('images/lists/sort-up.png');
        break;
      
      default:
        $downimg = image("<i class='material-icons md-18'>keyboard_arrow_down</i>");
        $upimg = image("<i class='material-icons md-18'>keyboard_arrow_up</i>");
        break;
    }

    $smarty->assign("servers",          set_post($this->servers));
    $smarty->assign("hosts",            set_post($hosts));
    $smarty->assign("priorities",       set_post($priorities));
    $smarty->assign("selected_server",  set_post($this->selected_server));
    $smarty->assign("selected_host",    set_post($this->selected_host));
    $smarty->assign("selected_priority",set_post($this->selected_priority));
    $smarty->assign("search_for",       set_post($this->search_for));
    $smarty->assign("startTime",        set_post($this->startTime));
    $smarty->assign("stopTime",         set_post($this->stopTime));
    $smarty->assign("sort_type",        set_post($this->sort_type));
    $smarty->assign("sort_value",       set_post($this->sort_value));
    $smarty->assign("limits",           set_post($this->limits));
    $smarty->assign("limit",            set_post($this->limit));
    $smarty->assign("page",             set_post($this->page));
    $smarty->assign("plug_id",          set_post($_GET['plug']));
    $smarty->assign("downimg",          $downimg);
    $smarty->assign("upimg",            $upimg);
    $smarty->assign("result",           $result);
    $smarty->assign("matches",          sprintf(_("%s entries match the filter"), $result["count"]));
    if($this->limits[$this->limit] != '-'){
      $smarty->assign("page_sel", range_selector($result['count'],$this->page,$this->limits[$this->limit]));
    }else{
      $smarty->assign("page_sel", "");
    }

    return($smarty->fetch(get_template_path("rSyslog.tpl", TRUE)));
  }


    function logToRss()
    {
        $result = array();
        foreach($this->servers as $name => $server){
            $link = @mysqli_connect($server['cn'], $server['User'], $server['Password'], $server['Database']);
            if ($link === FALSE){
                new log("debug","gosa_logging","dbconnect",array(),@mysqli_error());
                $this->servers[$name]['status'] = "connectfailed";
                $this->servers[$name]['error']  = @mysqli_error();
            }else{
                $this->servers[$name]['status'] = "ok";
                $this->servers[$name]['error']  = "";
            }

            if($link){
                $query = "SELECT * FROM SystemEvents WHERE Message like '%GOsa%' ORDER BY ID DESC LIMIT 50";
                $res = mysqli_query($link, $query);
                if($res){
                    while($attrs = mysqli_fetch_assoc($res)){
                        $result[] = $attrs; 
                    }
                }
            }
        }
        $source =
            '<?xml version="1.0" encoding="utf-8"?>'.
            ' <rdf:RDF'.
            '  xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"'.
            '  xmlns="http://purl.org/rss/1.0/"'.
            '  xmlns:dc="http://purl.org/dc/elements/1.1/">'.
            ' <channel rdf:about="Gosa">'.
            '  <description>GOsa</description>'.
            '  <link>gosa-project.org</link>'.
            '  <title>GOsa</title>'.
            '  <dc:date>'.date('d.m.Y H:i:s').'</dc:date>'.
            ' </channel>';

        foreach($result as $entry){

            $message = $entry['ReceivedAt']." - ".htmlentities($entry['FromHost'].": ".$entry['Message']);
            $source .= "
                <item>
                <title>{$message}</title>
                <description>{$message}</description>
                <pubDate>2010-10-25T14:27:39Z</pubDate>
                </item>";
        }
        $source .= "\n</rdf:RDF>";
        return($source);
    }



  function get_result()
  {
    $result = array();
    $result['entries'] = array();
    $result['status']  = "ok";
    $result['count']   = 0;
    $result['error']   = "";

    // Check whether the selected server exists.
    if($this->selected_server == "unknown" || !isset($this->servers[$this->selected_server])){
      $this->servers["unknown"]['status'] = sprintf(_("The selected server '%s' does not exists!"),$this->selected_server);
      $this->servers["unknown"]["cn"] = "-";
      return($result);
    }

    // Check database connectivity    
    $name = $this->selected_server;



    $server = $this->servers[$name];
    $link = @mysqli_connect($server['cn'], $server['User'], $server['Password'], $server['Database']);
    if ($link === FALSE){
      new log("debug","gosa_logging","dbconnect",array(),@mysqli_error());
      $this->servers[$name]['status'] = "connectfailed";
      $this->servers[$name]['error']  = @mysqli_error();
    }else{
      $this->servers[$name]['status'] = "ok";
      $this->servers[$name]['error']  = "";
    }

    // Get log entries matching the selected filter
    if(isset($this->servers[$name]['ACL']) && preg_match('/r/', $this->servers[$name]['ACL'])){

      $host = "%%";
      if($this->selected_host != "all") $host = $this->selected_host;
      $prio = "%%";
      if($this->selected_priority != "all") $prio = $this->selected_priority;

      $filter = "FROM SystemEvents WHERE 1=1 ";
      if($host != "%%"){
        $filter.= " AND FromHost like '".mysqli_real_escape_string($host)."' ";
      }
      if($prio != "%%"){
        $filter.= "AND Priority like '".mysqli_real_escape_string($prio)."' ";
      }
      $filter.= "AND DeviceReportedTime >= '".(date("Y.m.d 00:00:00", strtotime($this->startTime)))."' ";     
      $filter.= "AND DeviceReportedTime <= '".(date("Y.m.d 23:59:59", strtotime($this->stopTime)))."' ";     

      if(!empty($this->search_for)){
          $filter.= "AND ( ( SysLogTag like '%".mysqli_real_escape_string($this->search_for).
              "%' ) OR  ( Message like '%".mysqli_real_escape_string($this->search_for)."%' ) ) ";
      }

      // Detect amount of matching entries 
      $query = "SELECT count(ID) as `amount` ".$filter;
      $res = @mysqli_query($link, $query);
      if($res && $attrs = @mysqli_fetch_assoc($res)){ 
        $result['count'] = $attrs['amount'];
      }else{
        $this->servers[$name]['status'] = "query_failed";
        $this->servers[$name]['error']  = @mysqli_error();
      }

      if($result['count']){

        if($result['count'] < $this->page) $this->page = 0;

        $filter.= "ORDER BY ".$this->sort_value." ".$this->sort_type.", DeviceReportedTime ".$this->sort_type." ";
        if($this->limits[$this->limit]!= "-"){
          $filter.= "LIMIT ".$this->page.", ".$this->limits[$this->limit];
        }else{
          $this->page = 0;
        }

        $query = "SELECT * ".$filter;
        $res = mysqli_query($query, $link);
        if($res){
          while($attrs = @mysqli_fetch_assoc($res)){
            $attrs['DeviceReportedTime']= $this->time2local($attrs['DeviceReportedTime']);
            $attrs['Facility']= $this->facility2string($attrs['Facility']);
            $attrs['Priority']= $this->severity2string($attrs['Priority']);
            $attrs['SysLogTag']= preg_replace("/:$/", "", $attrs['SysLogTag']);

            foreach($attrs as $key => $val){
              $attrs[$key] = htmlentities($val, ENT_COMPAT, 'UTF-8');
            }

            $result['entries'][] = $attrs;
          }
          $this->servers[$name]['status'] = "ok";
          $this->servers[$name]['error']  = "";
        }else{
          $this->servers[$name]['status'] = "query_failed";
          $this->servers[$name]['error']  = @mysqli_error();
        }
      }
    }
    $result['status'] =$this->servers[$name]['status'];
    $result['error'] =$this->servers[$name]['error'];
    return($result);
  }


  function save_object()
  {
    // Get timestamps
    foreach(array("stopTime","startTime") as $attr){
      if(isset($_POST[$attr])) $this->$attr = get_post($attr);
    }
    
    // Get server from post
    if( isset($_POST['selected_server']) && isset($this->servers[$_POST['selected_server']])){ 
      $this->selected_server = get_post('selected_server');
    }

    // Get Host
    if( isset($this->servers[$this->selected_server]['Hosts']) &&  isset($_POST['selected_host']) && 
        ($_POST['selected_host'] == "all" || 
         in_array_strict($_POST['selected_host'],$this->servers[$this->selected_server]['Hosts']))){
      $this->selected_host = get_post('selected_host');
    }

    // Get priority from post
    if( isset($this->servers[$this->selected_server]['Priorities']) &&  isset($_POST['selected_priority']) && 
        ($_POST['selected_priority'] == "all" || 
         in_array_strict($_POST['selected_priority'],$this->servers[$this->selected_server]['Priorities']))){
      $this->selected_priority = get_post('selected_priority');
    }
      
    // Get serach string
    if(isset($_POST['search_for'])){
      $this->search_for = trim(get_post('search_for'));
    } 

    // Get page navigation attributes  
    if(isset($_GET['start'])) $this->page = $_GET['start'];
    if(isset($_POST['limit']) && isset($this->limits[$_POST['limit']])){
      $this->limit = get_post('limit');
    }

    // Get sorting attributes 
    if(isset($_GET['sort_value']) && in_array_strict($_GET['sort_value'], 
          array("DeviceReportedTime","FromHost","Facility","Priority","Message","SysLogTag"))){
      $sort_value = $_GET['sort_value'];
      if($this->sort_value == $sort_value){
        if($this->sort_type == "ASC"){
          $this->sort_type="DESC"; 
        }else{
          $this->sort_type="ASC"; 
        }
      } 
      $this->sort_value=$sort_value; 
    } 
  }


  function severity2string($severity)
  {
    $map= array( 0 => _("Emergency"),
                 1 => _("Alert"),
                 2 => _("Critical"),
                 3 => _("Error"),
                 4 => _("Warning"),
                 5 => _("Notice"),
                 6 => _("Informational"),
                 7 => _("Debug") );

    if (isset($map[$severity])) {
      return $map[$severity];
    }

    return sprintf(_("Unknown (%s)"), $severity);
  }


  function facility2string($facility)
  {
    $map= array( 0 => _("Kernel"),
                 1 => _("User level"),
                 2 => _("Mail system"),
                 3 => _("System daemon"),
                 4 => _("Security"),
                 5 => _("Internal"),
                 6 => _("Printing"),
                 7 => _("News"),
                 8 => _("UUCP"),
                 9 => _("Cron"),
                10 => _("Security"),
                11 => _("FTP"),
                12 => _("NTP"),
                13 => _("Log audit"),
                14 => _("Log alert"),
                15 => _("Clock"),
                16 => "Local0",
                17 => "Local1",
                18 => "Local2",
                19 => "Local3",
                20 => "Local4",
                21 => "Local5",
                22 => "Local6",
                23 => "Local7" );

    if (isset($map[$facility])) {
      return $map[$facility];
    }

    return sprintf(_("Unknown (%s)"), $facility);
  }


  function time2local($str)
  {
    list($date, $time)= explode(" ", $str);
    list($y, $m, $d)= explode("-", $date);
    return "$d.$m.$y $time";
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
