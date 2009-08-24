<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class listing {

  var $xmlData;
  var $entries;
  var $departments= array();
  var $departmentBrowser= false;
  var $departmentRootVisible= false;
  var $multiSelect= false;
  var $template;
  var $headline;
  var $module;
  var $base;
  var $sortDirection= null;
  var $sortColumn= null;
  var $sortAttribute;
  var $sortType;
  var $numColumns;
  var $baseMode= false;
  var $bases= array();
  var $header= array();
  var $colprops= array();
  var $filters= array();
  var $pid;
  var $objectTypes= array();
  var $objectTypeCount= array();
  var $copyPasteHandler= null;
  var $snapshotHandler= null;


  function listing($filename)
  {
    global $config;

    // Initialize pid
    $this->pid= preg_replace("/[^0-9]/", "", microtime(TRUE));

    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }

    // Set base for filter
    $this->base= session::global_get("CurrentMainBase");
    if ($this->base == null) {
      $this->base= $config->current['BASE'];
    }
    $this->refreshBasesList();

    // Move footer information
    $this->showFooter= ($config->get_cfg_value("listSummary") == "true");

    // Register build in filters
    $this->registerElementFilter("objectType", "listing::filterObjectType");
    $this->registerElementFilter("departmentLink", "listing::filterDepartmentLink");
    $this->registerElementFilter("link", "listing::filterLink");
    $this->registerElementFilter("actions", "listing::filterActions");
  }


  function setCopyPasteHandler($handler)
  {
    $this->copyPasteHandler= &$handler;
  }


  function setSnapshotHandler($handler)
  {
    $this->snapshotHandler= &$handler;
  }


  function setFilter($filter)
  {
    $this->filter= &$filter;
    if ($this->departmentBrowser){
      $this->departments= $this->getDepartments();
    }
    $this->filter->setBase($this->base);
    $this->entries= $this->filter->query();
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
    foreach (array("departmentBrowser", "departmentRootVisible", "multiSelect", "baseMode") as $token) {
      if (isset($this->xmlData['definition'][$token]) &&
          $this->xmlData['definition'][$token] == "true"){
        $this->$token= true;
      }
    }

    // Fill objectTypes from departments and xml definition
    $types = departmentManagement::get_support_departments();
    foreach ($types as $class => $data) {
      $this->objectTypes[]= array("label" => $data['TITLE'],
                                  "objectClass" => $data['OC'],
                                  "image" => $data['IMG']);
    }
    $this->categories= array();
    if (isset($this->xmlData['definition']['objectType'])) {
      if(isset($this->xmlData['definition']['objectType']['label'])) {
        $this->xmlData['definition']['objectType']= array($this->xmlData['definition']['objectType']);
      }
      foreach ($this->xmlData['definition']['objectType'] as $index => $otype) {
        $this->objectTypes[]= $this->xmlData['definition']['objectType'][$index];
        if (isset($this->xmlData['definition']['objectType'][$index]['category'])){
          $this->categories[]= $this->xmlData['definition']['objectType'][$index]['category'];
        }
      }
    }

    // Parse layout per column
    $this->colprops= $this->parseLayout($this->xmlData['table']['layout']);

    // Prepare table headers
    $this->renderHeader();

    // Assign headline/module
    $this->headline= _($this->xmlData['definition']['label']);
    $this->module= $this->xmlData['definition']['module'];
    if (!is_array($this->categories)){
      $this->categories= array($this->categories);
    }

    return true;  
  }


  function renderHeader()
  {
    $this->header= array();

    // Initialize sort?
    $sortInit= false;
    if (!$this->sortDirection) {
      $this->sortColumn= 0;
      if (isset($this->xmlData['definition']['defaultSortColumn'])){
        $this->sortColumn= $this->xmlData['definition']['defaultSortColumn'];
      } else {
        $this->sortAttribute= "";
      }
      $this->sortDirection= array();
      $sortInit= true;
    }

    if (isset($this->xmlData['table']['column'])){
      foreach ($this->xmlData['table']['column'] as $index => $config) {
        // Initialize everything to one direction
        if ($sortInit) {
          $this->sortDirection[$index]= false;
        }

        $sorter= "";
        if ($index == $this->sortColumn && isset($config['sortAttribute']) &&
            isset($config['sortType'])) {
          $this->sortAttribute= $config['sortAttribute'];
          $this->sortType= $config['sortType'];
          $sorter= "&nbsp;<img border='0' title='".($this->sortDirection[$index]?_("Up"):_("Down"))."' src='images/lists/sort-".($this->sortDirection[$index]?"up":"down").".png' align='top'>";
        }
        $sortable= (isset($config['sortAttribute']));

        $link= "href='?plug=".$_GET['plug']."&amp;PID=".$this->pid."&amp;act=SORT_$index'";
        if (isset($config['label'])) {
          if ($sortable) {
            $this->header[$index]= "<td class='listheader' ".$this->colprops[$index]."><a $link>"._($config['label'])."$sorter</a></td>";
          } else {
            $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">"._($config['label'])."</td>";
          }
        } else {
          if ($sortable) {
            $this->header[$index]= "<td class='listheader' ".$this->colprops[$index]."><a $link>&nbsp;$sorter</a></td>";
          } else {
            $this->header[$index]= "<td class='listheader' ".$this->colprops[$index].">&nbsp;</td>";
          }
        }
      }
    }
  }

  function render()
  {
    // Check for exeeded sizelimit
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    // Initialize list
    $result= "<input type='hidden' value='$this->pid' name='PID'>\n";
    $result.= "<div class='contentboxb' id='listing_container' style='border-top:1px solid #B0B0B0;'>\n";
    $result.= "<table summary='$this->headline' style='width:600px;height:450px;' cellspacing='0' id='t_scrolltable'>
<tr><td class='scrollhead'><table summary='' style='width:100%;' cellspacing='0' id='t_scrollhead'>\n";
    $this->numColumns= count($this->colprops) + ($this->multiSelect?1:0);

    // Build list header
    $result.= "<tr>\n";
    if ($this->multiSelect) {
      $result.= "<td class='listheader' style='width:20px;'><input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."' onClick='toggle_all_(\"listing_selected_[0-9]*$\",\"select_all\");' ></td>\n";
    }
    foreach ($this->header as $header) {
      $result.= $header;
    }

    // Add 13px for scroller
    $result.= "<td class='listheader' style='width:13px;border-right:0px;'>&nbsp;</td></table></td></tr>\n";

    // New table for the real list contents
    $result.= "<tr><td colspan='$this->numColumns' class='scrollbody'><div style='width:600px;height:430px;' id='d_scrollbody' class='scrollbody'><table summary='' style='height:100%;width:581px;' cellspacing='0' id='t_scrollbody'>\n";

    // No results? Just take an empty colspanned row
    if (count($this->entries) + count($this->departments) == 0) {
      $result.= "<tr class='rowxp0'><td class='list1nohighlight' colspan='$this->numColumns' style='height:100%;border-right:0px;width:100%;'>&nbsp;</td></tr>";
    }

    // Line color alternation
    $alt= 0;
    $deps= 0;

    // Draw department browser if configured and we're not in sub mode
    if ($this->departmentBrowser && $this->filter->scope != "sub") {
      // Fill with department browser if configured this way
      $departmentIterator= new departmentSortIterator($this->departments, $this->sortDirection[$this->sortColumn]);
      foreach ($departmentIterator as $row => $entry){
        $result.="<tr class='rowxp".($alt&1)."'>";

        // Render multi select if needed
        if ($this->multiSelect) {
          $result.="<td style='text-align:center;width:20px;' class='list1'>&nbsp;</td>";
        }

        // Render defined department columns, fill the rest with some stuff
        $rest= $this->numColumns - 1;
        foreach ($this->xmlData['table']['department'] as $index => $config) {
          $colspan= 1;
          if (isset($config['span'])){
            $colspan= $config['span'];
          }
          $result.="<td colspan='$colspan' ".$this->colprops[$index]." class='list1'>".$this->renderCell($config['value'], $entry, $row)."</td>";
          $rest-= $colspan;
        }

        // Fill remaining cols with nothing
        $last= $this->numColumns - $rest;
        for ($i= 0; $i<$rest; $i++){
          $result.= "<td ".$this->colprops[$last+$i-1]." class='list1'>&nbsp;</td>";
        }
        $result.="</tr>";

        $alt++;
      }
      $deps= $alt;
    }

    // Fill with contents, sort as configured
    foreach ($this->entries as $row => $entry) {
      $trow ="<tr class='rowxp".($alt&1)."'>\n";

      // Render multi select if needed
      if ($this->multiSelect) {
        $trow.="<td style='text-align:center;width:20px;' class='list0'><input type='checkbox' id='listing_selected_$row' name='listing_selected_$row'></td>\n";
      }

      foreach ($this->xmlData['table']['column'] as $index => $config) {
        $renderedCell= $this->renderCell($config['value'], $entry, $row);
        $trow.="<td ".$this->colprops[$index]." class='list0'>".$renderedCell."</td>\n";

        // Save rendered column
        $sort= preg_replace('/.*>([^<]+)<.*$/', '$1', $renderedCell);
        if (preg_match('/</', $sort)){
          $sort= "";
        }
        $this->entries[$row]["_sort$index"]= $sort;
      }
      $trow.="</tr>\n";

      // Save rendered entry
      $this->entries[$row]['_rendered']= $trow;

      $alt++;
    }

    // Complete list by sorting entries for _sort$index and appending them to the output
    $entryIterator= new listingSortIterator($this->entries, $this->sortDirection[$this->sortColumn], "_sort".$this->sortColumn, $this->sortType);
    foreach ($entryIterator as $row => $entry){
      $result.= $entry['_rendered'];
    }

    // Need to fill the list if it's not full (nobody knows why this is 22 ;-))
    $emptyListStyle= (count($this->entries) + $deps == 0)?"border:0;":"";
    if ((count($this->entries) + $deps) < 22) {
      $result.= "<tr>";
      for ($i= 0; $i<$this->numColumns; $i++) {
        if ($i == 0) {
          $result.= "<td class='list1nohighlight' style='$emptyListStyle height:100%;'>&nbsp;</td>";
          continue;
        }
        if ($i != $this->numColumns-1) {
          $result.= "<td class='list1nohighlight' style='$emptyListStyle'>&nbsp;</td>";
        } else {
          $result.= "<td class='list1nohighlight' style='border-right:1px solid #AAA;$emptyListStyle'>&nbsp;</td>";
        }
      }
      $result.= "</tr>";
    }

    $result.= "</table></div></td></tr>";

    // Add the footer if requested
    if ($this->showFooter) {
      $result.= "<tr><td class='scrollhead'><table summary='' style='width:100%' cellspacing='0' id='t_scrollfoot'><tr><td class='listfooter' style='border-bottom:0px;'>";

      foreach ($this->objectTypes as $objectType) {
        if (isset($this->objectTypeCount[$objectType['label']])) {
          $label= _($objectType['label']);
          $result.= "<img class='center' src='".$objectType['image']."' title='$label' alt='$label'>&nbsp;".$this->objectTypeCount[$objectType['label']]."&nbsp;&nbsp;&nbsp;&nbsp;";
        }
      }

      $result.= "<td class='listfooter' style='width:13px;border-right:0px;'>&nbsp;</td></table></td></tr>";
    }

    $result.= "</table></div>";

    $smarty= get_smarty();
    $smarty->assign("FILTER", $this->filter->render());
    $smarty->assign("SIZELIMIT", print_sizelimit_warning());
    $smarty->assign("LIST", $result);

    // Assign navigation elements
    $nav= $this->renderNavigation();
    foreach ($nav as $key => $html) {
      $smarty->assign($key, $html);
    }

    // Assign action menu / base
    $smarty->assign("ACTIONS", $this->renderActionMenu());
    $smarty->assign("BASE", $this->renderBase());

    // Assign separator
    $smarty->assign("SEPARATOR", "<img src='images/lists/seperator.png' alt='-' align='middle' height='16' width='1' class='center'>");

    // Assign summary
    $smarty->assign("HEADLINE", $this->headline);

    return ($smarty->fetch(get_template_path($this->xmlData['definition']['template'], true)));
  }


  function update()
  {
    global $config;
    $ui= get_userinfo();

    // Reset object counter
    $this->objectTypeCount= array();

    // Do not do anything if this is not our PID
    if(isset($_REQUEST['PID']) && $_REQUEST['PID'] != $this->pid) {
      return;
    }

    // Save base
    if (isset($_POST['BASE']) && $this->baseMode == true) {
      $base= validate($_POST['BASE']);
      if (isset($this->bases[$base])) {
        $this->base= $base;
      }
    }

    // Override the base if we got a message from the browser navigation
    if ($this->departmentBrowser && isset($_GET['act'])) {
      if (preg_match('/^department_([0-9]+)$/', validate($_GET['act']), $match)){
        if (isset($this->departments[$match[1]])){
          $this->base= $this->departments[$match[1]]['dn'];
        }
      }
    }

    // Filter GET with "act" attributes
    if (isset($_GET['act'])) {
      $key= validate($_GET['act']);
      if (preg_match('/^SORT_([0-9]+)$/', $key, $match)) {
        // Switch to new column or invert search order?
        $column= $match[1];
        if ($this->sortColumn != $column) {
          $this->sortColumn= $column;
        } else {
          $this->sortDirection[$column]= !$this->sortDirection[$column];
        }

        // Allow header to update itself according to the new sort settings
        $this->renderHeader();
      }
    }

    // Override base if we got signals from the navigation elements
    $action= "";
    foreach ($_POST as $key => $value) {
      if (preg_match('/^(ROOT|BACK|HOME)_x$/', $key, $match)) {
        $action= $match[1];
        break;
      }
    }

    // Navigation handling
    if ($action == 'ROOT') {
      $deps= $ui->get_module_departments($this->module);
      $this->base= $deps[0];
    }
    if ($action == 'BACK') {
      $deps= $ui->get_module_departments($this->module);
      $base= preg_replace("/^[^,]+,/", "", $this->base);
      if(in_array_ics($base, $deps)){
        $this->base= $base;
      }
    }
    if ($action == 'HOME') {
      $ui= get_userinfo();
      $this->base= get_base_from_people($ui->dn);
    }

    // Reload departments
    if ($this->departmentBrowser){
      $this->departments= $this->getDepartments();
    }

    // Update filter and refresh entries
    $this->filter->setBase($this->base);
    $this->entries= $this->filter->query();
  }


  function getBase($base)
  {
    $this->base= $base;
  }


  function setBase()
  {
    return $this->base;
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

    // Save number of columns for later use
    $this->numColumns= count($cols);

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

    // Replace all non replaced %{...} instances because they
    // are non resolved attributes or filters
    $data= preg_replace('/%{[^}]+}/', '&nbsp;', $data);

    return $data;
  }


  function renderBase()
  {
    $result= "<select name='BASE' onChange='mainform.submit()' size='1'>";
    $firstDN= null;
    $found= false;

    foreach ($this->bases as $key=>$value) {
      // Keep first entry to fall back eventually
      if(!$firstDN) {
        $firstDN= $key;
      }

      // Prepare to render entry
      $selected= "";
      if ($key == $this->base) {
        $selected= " selected";
        $found= true;
      }
      $result.= "<option value='".$key."'$selected>".$value."</option>";
    }
    $result.= "</select>";

    // Reset the currently used base to the first DN we found if there
    // was no match.
    if(!$found){
      $this->base = $firstDN;
    }

    return $result;
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


  function getObjectType($types, $classes)
  {
    // Walk thru types and see if there's something matching
    foreach ($types as $objectType) {
      $ocs= $objectType['objectClass'];
      if (!is_array($ocs)){
        $ocs= array($ocs);
      }

      $found= true;
      foreach ($ocs as $oc){
        if (preg_match('/^!(.*)$/', $oc, $match)) {
          $oc= $match[1];
          if (in_array($oc, $classes)) {
            $found= false;
          }
        } else {
          if (!in_array($oc, $classes)) {
            $found= false;
          }
        }
      }

      if ($found) {
        return $objectType;
      }
    }

    return null;
  }


  function filterObjectType($dn, $classes)
  {
    // Walk thru classes and return on first match
    $result= "&nbsp;";

    $objectType= $this->getObjectType($this->objectTypes, $classes);
    if ($objectType) {
      $result= "<img class='center' title='".LDAP::fix($dn)."' src='".$objectType["image"]."'>";
      if (!isset($this->objectTypeCount[$objectType['label']])) {
        $this->objectTypeCount[$objectType['label']]= 0;
      }
      $this->objectTypeCount[$objectType['label']]++;
    }
    return $result;
  }


  function filterActions($dn, $row, $classes)
  {
    // Do nothing if there's no menu defined
    if (!isset($this->xmlData['actiontriggers']['action'])) {
      return "&nbsp;";
    }

    // Go thru all actions
    $result= "";
    $actions= $this->xmlData['actiontriggers']['action'];
    foreach($actions as $action) {
      // Skip the entry completely if there's no permission to execute it
      if (!$this->hasActionPermission($action, $dn)) {
        continue;
      }

      // If there's an objectclass definition and we don't have it
      // add an empty picture here.
      if (isset($action['objectclass'])){
        $objectclass= $action['objectclass'];
        if (preg_match('/^!(.*)$/', $objectclass, $m)){
          $objectclass= $m[1];
          if(in_array($objectclass, $classes)) {
            $result.= "<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
            continue;
          }
        } else {
          if(!in_array($objectclass, $classes)) {
            $result.= "<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
            continue;
          }
        }
      }

      // Render normal entries as usual
      if ($action['type'] == "entry") {
        $label= $this->processElementFilter($action['label'], $this->entries[$row], $row);
        $image= $this->processElementFilter($action['image'], $this->entries[$row], $row);
        $result.="<input class='center' type='image' src='$image' title='$label' ".
                 "name='listing_".$action['name']."_$row' style='padding:1px'>";
      }

      // Handle special types
      if ($action['type'] == "copypaste" || $action['type'] == "snapshot") {

        $objectType= $this->getObjectType($this->objectTypes, $this->entries[$row]['objectClass']);
        $category= $class= null;
        if ($objectType) {
          $category= $objectType['category'];
          $class= $objectType['class'];
        }

        if ($action['type'] == "copypaste") {
          $result.= $this->renderCopyPasteActions($row, $this->entries[$row]['dn'], $category, $class);
        } else {
          $result.= $this->renderSnapshotActions($row, $this->entries[$row]['dn'], $category, $class);
        }
      }
    }

    return $result;
  }


  function filterDepartmentLink($row, $dn, $description)
  {
    $attr= $this->departments[$row]['sort-attribute'];
    $name= $this->departments[$row][$attr];
    if (is_array($name)){
      $name= $name[0];
    }
    $result= sprintf("%s [%s]", $name, $description[0]);
    return("<a href='?plug=".$_GET['plug']."&amp;PID=$this->pid&amp;act=department_$row' title='$dn'>$result</a>");
  }


  function filterLink()
  {
    $result= "&nbsp;";

    $row= func_get_arg(0);
    $pid= $this->pid;
    $dn= LDAP::fix(func_get_arg(1));
    $params= array(func_get_arg(2));

    // Collect sprintf params
    for ($i = 3;$i < func_num_args();$i++) {
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
                       "title='"._("Go to root department")."' name='ROOT' alt='"._("Root")."'>";
    }else{
      $result["ROOT"]= "<img src='images/lists/root_grey.png' class='center' alt='"._("Root")."'>";
    }

    /* Draw back button */
    if($enableBack){
      $result["BACK"]= "<input class='center' type='image' align='middle' src='images/lists/back.png' ".
                       "title='"._("Go up one department")."' alt='"._("Up")."' name='BACK'>";
    }else{
      $result["BACK"]= "<img src='images/lists/back_grey.png' class='center' alt='"._("Up")."'>";
    }

    /* Draw home button */
    if($enableHome){
      $result["HOME"]= "<input class='center' type='image' align='middle' src='images/lists/home.png' ".
                       "title='"._("Go to users department")."' alt='"._("Home")."' name='HOME'>";
    }else{
      $result["HOME"]= "<img src='images/lists/home_grey.png' class='center' alt='"._("Home")."'>";
    }

    /* Draw reload button, this button is enabled everytime */
    $result["RELOAD"]= "<input class='center' type='image' src='images/lists/reload.png' align='middle' ".
                       "title='"._("Reload list")."' name='REFRESH' alt='"._("Submit")."'>";

    return ($result);
  }


  function getAction()
  {
    // Do not do anything if this is not our PID
    if(isset($_REQUEST['PID']) && $_REQUEST['PID'] != $this->pid) {
      return;
    }

    $result= array("targets" => array(), "action" => "");

    // Filter GET with "act" attributes
    if (isset($_GET['act'])) {
      $key= validate($_GET['act']);
      $target= preg_replace('/^listing_[a-zA-Z_]+_([0-9]+)$/', '$1', $key);
      if (isset($this->entries[$target]['dn'])) {
        $result['action']= preg_replace('/^listing_([a-zA-Z_]+)_[0-9]+$/', '$1', $key);
        $result['targets'][]= $this->entries[$target]['dn'];
      }

      // Drop targets if empty
      if (count($result['targets']) == 0) {
        unset($result['targets']);
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

    // Drop targets if empty
    if (count($result['targets']) == 0) {
      unset($result['targets']);
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
             "border=0 class='center' src='images/lists/sort-down.png'></a>";

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
        $img= "<img border='0' class='center' src='".$action['image']."'>&nbsp;";
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
          $result.= $this->renderCopyPasteMenu($separator);
          break;

        case 'snapshot':
          $result.= $this->renderSnapshotMenu($separator);
          break;

        case 'exporter':
          $result.= $this->renderExporterMenu($separator);
          break;

        case 'daemon':
          $result.= $this->renderDaemonMenu($separator);
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
        $aclList= array();

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


  function refreshBasesList()
  {
    global $config;
    $ui= get_userinfo();

    // Do some array munching to get it user friendly
    $ids= $config->idepartments;
    $d= $ui->get_module_departments($this->module);
    $k_ids= array_keys($ids);
    $deps= array_intersect($d,$k_ids);

    // Fill internal bases list
    $this->bases= array();
    foreach($k_ids as $department){
      $this->bases[$department] = $ids[$department];
    }
  }


  function getDepartments()
  {
    $departments= array();
    $ui= get_userinfo();

    // Get list of supported department types
    $types = departmentManagement::get_support_departments();

    // Load departments allowed by ACL
    $validDepartments = $ui->get_module_departments($this->module);

    // Build filter and look in the LDAP for possible sub departments
    // of current base
    $filter= "(&(objectClass=gosaDepartment)(|";
    $attrs= array("description", "objectClass");
    foreach($types as $name => $data){
      $filter.= "(objectClass=".$data['OC'].")";
      $attrs[]= $data['ATTR'];
    }
    $filter.= "))";
    $res= get_list($filter, $this->module, $this->base, $attrs, GL_NONE | GL_SIZELIMIT);

    // Analyze list of departments
    foreach ($res as $department) {
      if (!in_array($department['dn'], $validDepartments)) {
        continue;
      }

      // Add the attribute where we use for sorting
      $oc= null;
      foreach(array_keys($types) as $type) {
        if (in_array($type, $department['objectClass'])) {
          $oc= $type;
          break;
        }
      }
      $department['sort-attribute']= $types[$oc]['ATTR'];

      // Move to the result list
      $departments[]= $department;
    }

    return $departments;
  }


  function renderCopyPasteMenu($separator, $copy= true, $cut= true)
  {
    // We can only provide information if we've got a copypaste handler
    // instance
    if(!(isset($this->copyPasteHandler) && is_object($this->copyPasteHandler))){
      return "";
    }

    // Presets
    $result= "";
    $read= $paste= false;
    $ui= get_userinfo();

    // Switch flags to on if there's at least one category which allows read/paste
    foreach($this->categories as $category){
      $read= $read || preg_match('/r/', $ui->get_category_permissions($this->base, $category));
      $paste= $paste || $ui->is_pasteable($this->base, $category) == 1;
    }


    // Draw entries that allow copy and cut
    if($read){

      // Copy entry
      if($copy){
        $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"copy\";mainform.submit();'><img src='images/lists/copy.png' alt='' border='0' class='center'>&nbsp;"._("Copy")."</a></li>";
        $separator= "";
      }

      // Cut entry
      if($cut){
        $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"cut\";mainform.submit();'><img src='images/lists/cut.png' alt='' border='0' class='center'>&nbsp;"._("Cut")."</a></li>";
        $separator= "";
      }
    }

    // Draw entries that allow pasting entries
    if($paste){
      if($this->copyPasteHandler->entries_queued()){
        $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"paste\";mainform.submit();'><img src='images/lists/paste.png' alt='' border='0' class='center'>&nbsp;"._("Paste")."</a></li>";
      }else{
        $result.= "<li$separator><a href='#'><img src='images/lists/paste-grey.png' alt='' border='0' class='center'>&nbsp;"._("Paste")."</a></li>";
      }
    }
    
    return($result);
  }


  function renderCopyPasteActions($row, $dn, $category, $class, $copy= true, $cut= true)
  {
    // We can only provide information if we've got a copypaste handler
    // instance
    if(!(isset($this->copyPasteHandler) && is_object($this->copyPasteHandler))){
      return "";
    }

    // Presets
    $ui = get_userinfo();
    $result = "";

    // Render cut entries
    if($cut){
      if($ui->is_cutable($dn, $category, $class)){
        $result .= "<input class='center' type='image'
          src='images/lists/cut.png' alt='"._("Cut")."' name='listing_cut_$row' title='"._("Cut this entry")."' style='padding:1px'>";
      }else{
        $result.="<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
      }
    }

    // Render copy entries
    if($copy){
      if($ui->is_copyable($dn, $category, $class)){
        $result.= "<input class='center' type='image'
          src='images/lists/copy.png' alt='"._("Copy")."' name='listing_copy_$row' title='"._("Copy this entry")."' style='padding:1px'>";
      }else{
        $result.="<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
      }
    }

    return($result);
  }


  function renderSnapshotMenu($separator)
  {
    // We can only provide information if we've got a snapshot handler
    // instance
    if(!(isset($this->snapshotHandler) && is_object($this->snapshotHandler))){
      return "";
    }

    // Presets
    $result = "";
    $ui = get_userinfo();

    if($this->snapshotHandler->enabled() && $ui->allow_snapshot_restore($this->base, $this->module)){

      // Check if there is something to restore
      $restore= false;
      foreach($this->snapshotHandler->getSnapshotBases() as $base){
        $restore= $restore || count($this->snapshotHandler->getDeletedSnapshots($base)) > 0;
      }

      // Draw icons according to the restore flag
      if($restore){
        $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"restore\";mainform.submit();'><img src='images/lists/restore.png' alt='' border='0' class='center'>&nbsp;"._("Restore snapshots")."</a></li>";
      }else{
        $result.= "<li$separator><a href='#'><img src='images/lists/restore_grey.png' alt='' border='0' class='center'>&nbsp;"._("Restore snapshots")."</a></li>";
      }
    }

    return($result);
  }


  function renderExporterMenu($separator)
  {
    // Presets
    $result = "";

    // Draw entries
    $result.= "<li$separator><a href='#'><img border='0' class='center' src='images/lists/export.png'>&nbsp;"._("Export list")."&nbsp;<img border='0' src='images/forward-arrow.png'></a><ul class='level3'>";

    // Export CVS as build in exporter
    $result.= "<li><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"exportCSV\";mainform.submit();'><img border='0' class='center' src='plugins/lists/exportCSV.png'>&nbsp;CSV</a></li>";

    // Finalize list
    $result.= "</ul></li>";

    return($result);
  }


  function renderSnapshotActions($row, $dn, $category, $class, $copy= true, $cut= true)
  {
    // We can only provide information if we've got a snapshot handler
    // instance
    if(!(isset($this->snapshotHandler) && is_object($this->snapshotHandler))){
      return "";
    }

    // Presets
    $result= "";
    $ui = get_userinfo();

    // Only act if enabled here
    if($this->snapshotHandler->enabled()){

      // Draw restore button
      if ($ui->allow_snapshot_restore($dn, $category)){

        // Do we have snapshots for this dn?
        if($this->snapshotHandler->hasSnapshots($dn)){
          $result.= "<input class='center' type='image' src='images/lists/restore.png' ".
                     "alt='"._("Restore snapshot")."' name='listing_restore_$row' title='".
                     _("Restore snapshot")."' style='padding:1px'>";
        } else {
          $result.= "<img src='images/lists/restore_grey.png' alt=' ' class='center' style='padding:1px'>";
        }
      }

      // Draw snapshot button
      if($ui->allow_snapshot_create($dn, $category)){
          $result.= "<input class='center' type='image' src='images/snapshot.png' ".
                     "alt='"._("Create snapshot")."' name='listing_snapshot_$row' title='".
                     _("Create a new snapshot from this object")."' style='padding:1px'>";
      }else{
          $result.= "<img src='images/empty.png' alt=' ' class='center' style='padding:1px'>";
      }
    }

    return($result);
  }


  function renderDaemonMenu($separator)
  {
    $result= "";

    // If there is a daemon registered, draw the menu entries
    if(class_available("DaemonEvent")){
      $events= DaemonEvent::get_event_types_by_category($this->categories);
      if(isset($events['BY_CLASS']) && count($events['BY_CLASS'])){
        foreach($events['BY_CLASS'] as $name => $event){
          $result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value=\"$name\";mainform.submit();'>".$event['MenuImage']."&nbsp;".$event['s_Menu_Name']."</a></li>";
          $separator= "";
        }
      }
    }

    return $result;
  }

}

?>
