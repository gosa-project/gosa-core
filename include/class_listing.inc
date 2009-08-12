<?php

class listing {

  var $xmlData;
  var $entries;
  var $departmentBrowser= false;
  var $departmentRootVisible= false;
  var $multiSelect= false;
  var $template;
  var $summary;
  var $header= array();
  var $colprops= array();
  var $filters= array();
  static $objectTypes;


  function listing($filename)
  {
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }

    // Register build in filters
    $this->registerFilter("objectType", "listing::filterObjectType");
    $this->registerFilter("link", "listing::filterLink");
    $this->registerFilter("actions", "listing::filterActions");
  }


  function registerFilter($name, $call)
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
        listing::$objectTypes[$otype['objectClass']]= $this->xmlData['definition']['objectType'][$index];
      }
    }

    // Parse layout per column
    $this->colprops= $this->parseLayout($this->xmlData['table']['layout']);

    // Prepare table headers
    $this->header= array();
    if (isset($this->xmlData['table']['column'])){
      foreach ($this->xmlData['table']['column'] as $index => $config) {
        if (isset($config['header'])) {
          $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">".xml::getLocalizedAttribute($config['header'], $GLOBALS['t_language'])."</td>";
        } else {
          $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">&nbsp;</td>";
        }
      }
    }

    return true;  
  }


  function render()
  {
echo "Fill summary, handle empty lists differently, alternating, sorting, post_id<br>";
    $summary= "empty";

    // Initialize list
    $result= "<table summary='$summary' style='width:600px;height:450px;' cellspacing='0' id='t_scrolltable'>
<tr><td class='scrollhead'><table summary='' style='width:100%;' cellspacing='0' id='t_scrollhead'>";
    $num_cols= count($this->colprops) + ($this->multiSelect?1:0);

    // Build list header
    $result.= "<tr>";
    if ($this->multiSelect) {
      $result.= "<td class='listheader' style='width:20px;'><input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."' onClick='toggle_all_(\"item_selected_[0-9]*$\",\"select_all\");' ></td>";
    }
    foreach ($this->header as $header) {
      $result.= $header;
    }

    // Add 13px for scroller
    $result.= "<td class='listheader' style='width:13px;border-right:0px;'>&nbsp;</td></table></td></tr>";

    // New table for the real list contents
    $result.= "<tr><td colspan='$num_cols' class='scrollbody'><div style='width:600px;height:430px;' id='d_scrollbody' class='scrollbody'><table summary='' style='height:100%;width:581px;' cellspacing='0' id='t_scrollbody'>";

    // Fill with contents
    foreach ($this->entries as $row => $entry){
      $result.="<tr class='rowxp".($row&1)."'>";

      // Render multi select if needed
      if ($this->multiSelect) {
        $result.="<td style='text-align:center;width:20px;' class='list1'>&nbsp;</td>";
      }

      foreach ($this->xmlData['table']['column'] as $index => $config) {
        $result.="<td ".$this->colprops[$index]." class='list0'>".$this->renderCell($config['value'], $entry, $row)."</td>";
      }
      $result.="</tr>";
    }

    // Need to fill the list if it's not full (nobody knows why this is 22 ;-))
    if (count($this->entries) < 22) {
      $result.= "<tr>";
      for ($i= 0; $i<=$num_cols; $i++) {
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

    $result.= "</table></div></td></tr></table>";

    return $result;
  }


  function setEntries($entries)
  {
    $this->entries= &$entries;
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

        // pid is replaced by the current post_id
        if ($param == "pid") {
          #TODO: get post_id 
          $params[]= "99";
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
          $params[]= $config["dn"];
        }
      }

      // Replace information
      $data= preg_replace('/'.preg_quote($match[0]).'/', call_user_func_array(array($cl, $method), $params), $data);
    }

    return $data;
  }


  static function filterObjectType($dn, $classes)
  {
    // Walk thru classes and return on first match
    $result= "&nbsp;";
    $prio= 99;
    foreach ($classes as $objectClass) {
      if (isset(listing::$objectTypes[$objectClass])){
        if (!isset(listing::$objectTypes[$objectClass]["priority"])){
          $result= "<img class='center' title='$dn' src='".listing::$objectTypes[$objectClass]["image"]."'>";
          return $result; 
        }

        if (listing::$objectTypes[$objectClass]["priority"] < $prio){
          $prio= listing::$objectTypes[$objectClass]["priority"];
          $result= "<img class='center' title='$dn' src='".listing::$objectTypes[$objectClass]["image"]."'>";
        }
      }
    }

    return $result;
  }


  static function filterLink()
  {
    $result= "&nbsp;";

    $row= func_get_arg(0);
    $pid= func_get_arg(1);
    $dn= func_get_arg(2);
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
      return("<a href='?plug=".$_GET['plug']."&amp;post_id=$pid&amp;act=edit_$row' title='$dn'>$trans</a>");
    }

    return $result;
  }


  static function filterActions()
  {
    return "TBD";
  }
}

?>
