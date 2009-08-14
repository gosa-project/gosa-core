<?php

class listing {

  var $xmlData;
  var $entries;
  var $departmentBrowser= false;
  var $departmentRootVisible= false;
  var $multiSelect= false;
  var $template;
  var $headline;
  var $module;
  var $header= array();
  var $colprops= array();
  var $filters= array();
  var $pid;
  var $objectTypes;


  function listing($filename)
  {
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }

    // Register build in filters
    $this->registerElementFilter("objectType", "listing::filterObjectType");
    $this->registerElementFilter("link", "listing::filterLink");
    $this->registerElementFilter("actions", "listing::filterActions");

    // Initialize pid
    $this->pid= preg_replace("/[^0-9]/", "", microtime(TRUE));
  }


  function registerElementFilter($name, $call)
  {
    if (!isset($this->filters[$name])) {
      $this->filters[$name]= $call;
      return true;
    }

    return false;
  }


  function load($filename)
  {
    $contents = file_get_contents($filename);
    $this->xmlData= xml::xml2array($contents, 1);

    if (!isset($this->xmlData['list'])) {
      return false;
    }

    $this->xmlData= $this->xmlData["list"];

    // Load some definition values
    foreach (array("departmentBrowser", "departmentRootVisible", "multiSelect") as $token) {
      if (isset($this->xmlData['definition'][$token]) &&
          $this->xmlData['definition'][$token] == "true"){
        $this->$token= true;
      }
    }

    // Fill objectTypes
    if (isset($this->xmlData['definition']['objectType'])) {
      foreach ($this->xmlData['definition']['objectType'] as $index => $otype) {
        $this->objectTypes[$otype['objectClass']]= $this->xmlData['definition']['objectType'][$index];
      }
    }

    // Parse layout per column
    $this->colprops= $this->parseLayout($this->xmlData['table']['layout']);

    // Prepare table headers
    $this->header= array();
    if (isset($this->xmlData['table']['column'])){
      foreach ($this->xmlData['table']['column'] as $index => $config) {
        if (isset($config['label'])) {
          $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">"._($config['label'])."</td>";
        } else {
          $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">&nbsp;</td>";
        }
      }
    }

    // Assign headline/module
    $this->headline= _($this->xmlData['definition']['label']);
    $this->module= $this->xmlData['definition']['module'];

    return true;  
  }


  function render()
  {
echo "filter for images, action menu, sorting, department browsing, filter base handling, bottom list info, copy'n paste handler, snapshot handler<br>";

    // Initialize list
    $result= "<input type='hidden' value='$this->pid' name='PID'>";
    $result.= "<div class='contentboxb' id='listing_container' style='border-top:1px solid #B0B0B0;'>";
    $result.= "<table summary='$this->headline' style='width:600px;height:450px;' cellspacing='0' id='t_scrolltable'>
<tr><td class='scrollhead'><table summary='' style='width:100%;' cellspacing='0' id='t_scrollhead'>";
    $num_cols= count($this->colprops) + ($this->multiSelect?1:0);

    // Build list header
    $result.= "<tr>";
    if ($this->multiSelect) {
      $result.= "<td class='listheader' style='width:20px;'><input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."' onClick='toggle_all_(\"listing_selected_[0-9]*$\",\"select_all\");' ></td>";
    }
    foreach ($this->header as $header) {
      $result.= $header;
    }

    // Add 13px for scroller
    $result.= "<td class='listheader' style='width:13px;border-right:0px;'>&nbsp;</td></table></td></tr>";

    // New table for the real list contents
    $result.= "<tr><td colspan='$num_cols' class='scrollbody'><div style='width:600px;height:430px;' id='d_scrollbody' class='scrollbody'><table summary='' style='height:100%;width:581px;' cellspacing='0' id='t_scrollbody'>";

    // No results? Just take an empty colspanned row
    if (count($this->entries) == 0) {
      $result.= "<tr class='rowxp0'><td class='list1nohighlight' colspan='$num_cols' style='height:100%;border-right:0px;width:100%;'>&nbsp;</td></tr>";
    }

    // Fill with contents
    foreach ($this->entries as $row => $entry){
      $result.="<tr class='rowxp".($row&1)."'>";

      // Render multi select if needed
      if ($this->multiSelect) {
        $result.="<td style='text-align:center;width:20px;' class='list0'><input type='checkbox' id='listing_selected_$row' name='listing_selected_$row'></td>";
      }

      foreach ($this->xmlData['table']['column'] as $index => $config) {
        $result.="<td ".$this->colprops[$index]." class='list0'>".$this->renderCell($config['value'], $entry, $row)."</td>";
      }
      $result.="</tr>";
    }

    // Need to fill the list if it's not full (nobody knows why this is 22 ;-))
    if (count($this->entries) < 22) {
      $result.= "<tr>";
      for ($i= 0; $i<$num_cols; $i++) {
        if ($i == 0) {
          $result.= "<td class='list1nohighlight' style='height:100%;'>&nbsp;</td>";
          continue;
        }
        if ($i != $num_cols-1) {
          $result.= "<td class='list1nohighlight''>&nbsp;</td>";
        } else {
          $result.= "<td class='list1nohighlight' style='border-right:0px'>&nbsp;</td>";
        }
      }
      $result.= "</tr>";
    }

    $result.= "</table></div></td></tr></table></div>";

    $smarty= get_smarty();
    $smarty->assign("FILTER", $this->filter->render());
    $smarty->assign("LIST", $result);

    // Assign navigation elements
    $nav= $this->renderNavigation();
    foreach ($nav as $key => $html) {
      $smarty->assign($key, $html);
    }

    // Assign action menu
    $smarty->assign("ACTIONS", $this->renderActionMenu());

    // Assign separator
    $smarty->assign("SEPARATOR", "<img src='images/lists/seperator.png' alt='-' align='middle' height='16' width='1' class='center'>");

    // Assign summary
    $smarty->assign("HEADLINE", $this->headline);

    return ($smarty->fetch(get_template_path($this->xmlData['definition']['template'], true)));
  }


  function setFilter($filter)
  {
    $this->filter= &$filter;
    $this->entries= $this->filter->query();
  }


  function update()
  {
    // Do not do anything if this is not our PID
    # DISABLED because the object is not in the session
    #if(isset($_REQUEST['PID']) && $_REQUEST['PID'] != $this->pid) {
    #  return;
    #}

    $this->entries= $this->filter->query();
  }


  function parseLayout($layout)
  {
    $result= array();
    $layout= preg_replace("/^\|/", "", $layout);
    $layout= preg_replace("/\|$/", "", $layout);
    $cols= split("\|", $layout);
    foreach ($cols as $index => $config) {
      if ($config != "") {
        $components= split(';', $config);
        $config= "";
        foreach ($components as $part) {
          if (preg_match("/^r$/", $part)) {
            $config.= "text-align:right;";
            continue;
          }
          if (preg_match("/^l$/", $part)) {
            $config.= "text-align:left;";
            continue;
          }
          if (preg_match("/^c$/", $part)) {
            $config.= "text-align:center;";
            continue;
          }
          if (preg_match("/^[0-9]+(|px|%)$/", $part)) {
            $config.= "width:$part;";
            continue;
          }
        }

        $result[$index]= " style='$config' ";
      } else {
        $result[$index]= null;
      }
    }

    return $result;
  }


  function renderCell($data, $config, $row)
  {
    // Replace flat attributes in data string
    for ($i= 0; $i<$config['count']; $i++) {
      $attr= $config[$i];
      $value= "";
      if (is_array($config[$attr])) {
        $value= $config[$attr][0];
      } else {
        $value= $config[$attr];
      }
      $data= preg_replace("/%\{$attr\}/", $value, $data);
    }

    // Watch out for filters and prepare to execute them
    $data= $this->processElementFilter($data, $config, $row);

    return $data;
  }


  function processElementFilter($data, $config, $row)
  {
    preg_match_all("/%\{filter:([^(]+)\((.*)\)\}/", $data, $matches, PREG_SET_ORDER);

    foreach ($matches as $match) {
      if (!isset($this->filters[$match[1]])) {
        continue;
      }
      $cl= preg_replace('/::.*$/', '', $this->filters[$match[1]]);
      $method= preg_replace('/^.*::/', '', $this->filters[$match[1]]);

      // Prepare params for function call
      $params= array();
      preg_match_all('/"[^"]+"|[^,]+/', $match[2], $parts);
      foreach ($parts[0] as $param) {

        // Row is replaced by the row number
        if ($param == "row") {
          $params[]= $row;
        }

        // pid is replaced by the current PID
        if ($param == "pid") {
          $params[]= $this->pid;
        }

        // Fixie with "" is passed directly
        if (preg_match('/^".*"$/', $param)){
          $params[]= preg_replace('/"/', '', $param);
        }

        // LDAP variables get replaced by their objects
        for ($i= 0; $i<$config['count']; $i++) {
          if ($param == $config[$i]) {
            $values= $config[$config[$i]];
            if (is_array($values)){
              unset($values['count']);
            }
            $params[]= $values;
          }
        }

        // Move dn if needed
        if ($param == "dn") {
          $params[]= LDAP::fix($config["dn"]);
        }
      }

      // Replace information
      if ($cl == "listing") {
        // Non static call - seems to result in errors
        $data= @preg_replace('/'.preg_quote($match[0]).'/', call_user_func_array(array($this, "$method"), $params), $data);
      } else {
        // Static call
        $data= preg_replace('/'.preg_quote($match[0]).'/', call_user_func_array(array($cl, $method), $params), $data);
      }
    }

    return $data;
  }


  function filterObjectType($dn, $classes)
  {
    // Walk thru classes and return on first match
    $result= "&nbsp;";
    $prio= 99;
    foreach ($classes as $objectClass) {
      if (isset($this->objectTypes[$objectClass])){
        if (!isset($this->objectTypes[$objectClass]["priority"])){
          $result= "<img class='center' title='".LDAP::fix($dn)."' src='".$this->objectTypes[$objectClass]["image"]."'>";
          return $result; 
        }

        if ($this->objectTypes[$objectClass]["priority"] < $prio){
          $prio= $this->objectTypes[$objectClass]["priority"];
          $result= "<img class='center' title='".LDAP::fix($dn)."' src='".$this->objectTypes[$objectClass]["image"]."'>";
        }
      }
    }

    return $result;
  }


  function filterActions($row, $dn)
  {
    return "TBD";
  }


  function filterLink()
  {
    $result= "&nbsp;";

    $row= func_get_arg(0);
    $pid= func_get_arg(1);
    $dn= LDAP::fix(func_get_arg(2));
    $params= array(func_get_arg(3));

    // Collect sprintf params
    for ($i = 4;$i < func_num_args();$i++) {
      $val= func_get_arg($i);
      if (is_array($val)){
        $params[]= $val[0];
        continue;
      }
      $params[]= $val;
    }

    $result= "&nbsp;";
    $trans= call_user_func_array("sprintf", $params);
    if ($trans != "") {
      return("<a href='?plug=".$_GET['plug']."&amp;PID=$pid&amp;act=listing_edit_$row' title='$dn'>$trans</a>");
    }

    return $result;
  }


  function renderNavigation()
  {
    $result= array();
    $enableBack = true;
    $enableRoot = true;
    $enableHome = true;

    $ui = get_userinfo();

    /* Check if base = first available base */
    $deps = $ui->get_module_departments($this->module);

    if(!count($deps) || $deps[0] == $this->filter->base){
      $enableBack = false;
      $enableRoot = false;
    }

    $listhead ="";

    /* Check if we are in users home  department */
    if(!count($deps) ||$this->filter->base == get_base_from_people($ui->dn)){
      $enableHome = false;
    }

    /* Draw root button */
    if($enableRoot){
      $result["ROOT"]= "<input class='center' type='image' src='images/lists/root.png' align='middle' ".
                       "title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>";
    }else{
      $result["ROOT"]= "<img src='images/lists/root_grey.png' class='center' alt='"._("Root")."'>";
    }

    /* Draw back button */
    if($enableBack){
      $result["BACK"]= "<input class='center' type='image' align='middle' src='images/lists/back.png' ".
                       "title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>";
    }else{
      $result["BACK"]= "<img src='images/lists/back_grey.png' class='center' alt='"._("Up")."'>";
    }

    /* Draw home button */
    if($enableHome){
      $result["HOME"]= "<input class='center' type='image' align='middle' src='images/lists/home.png' ".
                       "title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>";
    }else{
      $result["HOME"]= "<img src='images/lists/home_grey.png' class='center' alt='"._("Home")."'>";
    }

    /* Draw reload button, this button is enabled everytime */
    $result["RELOAD"]= "<input class='center' type='image' src='images/lists/reload.png' align='middle' ".
                       "title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>";

    return ($result);
  }


  function getAction()
  {
    // Do not do anything if this is not our PID
    # DISABLED because the object is not in the session
    #if(isset($_REQUEST['PID']) && $_REQUEST['PID'] != $this->pid) {
    #  return;
    #}

    $result= array("targets" => array(), "action" => "");

    // Filter GET with "act" attributes
    if (isset($_GET['act'])) {
      $key= validate($_GET['act']);
      $target= preg_replace('/^listing_[a-zA-Z_]+_([0-9]+)$/', '$1', $key);
      if (isset($this->entries[$target]['dn'])) {
        $result['action']= preg_replace('/^listing_([a-zA-Z_]+)_[0-9]+$/', '$1', $key);
        $result['targets'][]= $this->entries[$target]['dn'];
      }
      return $result;
    }

    // Filter POST with "listing_" attributes
    foreach ($_POST as $key => $prop) {

      // Capture selections
      if (preg_match('/^listing_selected_[0-9]+$/', $key)) {
        $target= preg_replace('/^listing_selected_([0-9]+)$/', '$1', $key);
        if (isset($this->entries[$target]['dn'])) {
          $result['targets'][]= $this->entries[$target]['dn'];
        }
        continue;
      }

      // Capture action with target - this is a one shot
      if (preg_match('/^listing_[a-zA-Z_]+_[0-9]+(|_x)$/', $key)) {
        $target= preg_replace('/^listing_[a-zA-Z_]+_([0-9]+)(|_x)$/', '$1', $key);
        if (isset($this->entries[$target]['dn'])) {
          $result['action']= preg_replace('/^listing_([a-zA-Z_]+)_[0-9]+(|_x)$/', '$1', $key);
          $result['targets']= array($this->entries[$target]['dn']);
        }
        break;
      }

      // Capture action without target
      if (preg_match('/^listing_[a-zA-Z_]+(|_x)$/', $key)) {
        $result['action']= preg_replace('/^listing_([a-zA-Z_]+)(|_x)$/', '$1', $key);
        continue;
      }
    }

    // Filter POST with "act" attributes -> posted from action menu
    if (isset($_POST['act']) && $_POST['act'] != '') {
      $result['action']= validate($_POST['act']);
    }

    return $result;
  }


  function renderActionMenu()
  {
    // Don't send anything if the menu is not defined
    if (!isset($this->xmlData['actionmenu']['action'])){
      return "";
    }

    // Load shortcut
    $actions= &$this->xmlData['actionmenu']['action'];
    $result= "<input type='hidden' name='act' id='actionmenu' value=''>".
             "<ul class='level1' id='root'><li><a href='#'>Aktionen&nbsp;<img ".
             "border=0 src='images/lists/sort-down.png'></a>";

    // Build ul/li list
    $result.= $this->recurseActions($actions);

    return "<div id='pulldown'>".$result."</li></ul><div>";
  }


  function recurseActions($actions)
  {
    static $level= 2;
    $result= "<ul class='level$level'>";
    $separator= "";

    foreach ($actions as $action) {

      // Skip the entry completely if there's no permission to execute it
      if (!$this->hasActionPermission($action, $this->filter->base)) {
        continue;
      }

      // Fill image if set
      $img= "";
      if (isset($action['image'])){
        $img= "<img border=0 src='".$action['image']."'>&nbsp;";
      }

      if ($action['type'] == "separator"){
        $separator= " style='border-top:1px solid #AAA' ";
        continue;
      }

      // Dive into subs
      if ($action['type'] == "sub" && isset($action['action'])) {
        $level++;
        if (isset($action['label'])){
          $result.= "<li$separator><a href='#'>$img"._($action['label'])."&nbsp;<img border='0' src='images/forward-arrow.png'></a>";
        }
        $result.= $this->recurseActions($action['action'])."</li>";
        $level--;
        $separator= "";
        continue;
      }

      // Render entry elseways
      if (isset($action['label'])){
        $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"".$action['name']."\";mainform.submit();'>$img"._($action['label'])."</a></li>";
      }

      // Check for special types
      switch ($action['type']) {
        case 'copypaste':
          echo "actionmenu: copypaste missing<br>";
          break;

        case 'snapshot':
          echo "actionmenu: snapshot missing<br>";
          break;

        case 'daemon':
          echo "actionmenu: daemon missing<br>";
          break;
      }

      $separator= "";
    }

    $result.= "</ul>";
    return $result;
  }


  function hasActionPermission($action, $dn)
  {
    $ui= get_userinfo();

    if (isset($action['acl'])) {
      $acls= $action['acl'];
      if (!is_array($acls)) {
        $acls= array($acls);
      }

      // Every ACL has to pass
      foreach ($acls as $acl) {
        $module= $this->module;
        $acllist= array();

        // Split for category and plugins if needed
        // match for "[rw]" style entries
        if (preg_match('/^\[([rwcdm]+)\]$/', $acl, $match)){
          $aclList= array($match[1]);
        }

        // match for "users[rw]" style entries
        if (preg_match('/^([a-zA-Z0-9]+)\[([rwcdm]+)\]$/', $acl, $match)){
          $module= $match[1];
          $aclList= array($match[2]);
        }

        // match for "users/user[rw]" style entries
        if (preg_match('/^([a-zA-Z0-9]+\/[a-zA-Z0-9]+)\[([rwcdm]+)\]$/', $acl, $match)){
          $module= $match[1];
          $aclList= array($match[2]);
        }

        // match "users/user[userPassword:rw(,...)*]" style entries
        if (preg_match('/^([a-zA-Z0-9]+\/[a-zA-Z0-9]+)\[([a-zA-Z0-9]+:[rwcdm]+(,[a-zA-Z0-9]+:[rwcdm]+)*)\]$/', $acl, $match)){
          $module= $match[1];
          $aclList= split(',', $match[2]);
        }

        // Walk thru prepared ACL by using $module
        foreach($aclList as $sAcl) {
          $checkAcl= "";

          // Category or detailed permission?
          if (strpos('/', $module) === false) {
            if (preg_match('/([a-zA-Z0-9]+):([rwcdm]+)/', $sAcl, $m) ) {
              $checkAcl= $ui->get_permissions($dn, $module, $m[1]);
              $sAcl= $m[2];
            } else {
              $checkAcl= $ui->get_permissions($dn, $module, '0');
            }
          } else {
            $checkAcl= $ui->get_category_permissions($dn, $module);
          }

          // Split up remaining part of the acl and check if it we're
          // allowed to do something...
          $parts= str_split($sAcl);
          foreach ($parts as $part) {
            if (strpos($checkAcl, $part) === false){
              return false;
            }
          }

        }
      }
    }

    return true;
  }
}

?>
