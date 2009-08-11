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

  function listing($filename)
  {
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }
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
    #print_a($this->xmlData);
    #print_a($this->entries);
echo "Fill summary, handle empty lists differently, alternating<br>";
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
    $result.= "<tr><td colspan='$num_cols' class='scrollbody'><div style='width:100%;height:430px;' id='d_scrollbody' class='scrollbody'><table summary='' style='height:100%;width:581px;' cellspacing='0' id='t_scrollbody'>";

    // Fill with contents
    $first= true;
    foreach ($this->entries as $entry){
      $result.="<tr class='rowxp1'>";
      foreach ($this->xmlData['table']['column'] as $config) {
        if ($first) {
          
        } else {
        }
      }
      $first= false;
    }
#    for ($i= 0; $i<10; $i++) {
#      $result.="<tr class='rowxp1'>";
#      $result.="<td  style='text-align:center;width:20px;'  class='list1'>&nbsp;</td>";
#      $result.="<td  style='text-align:center;width:20px;'  class='list1'><image src='images/lists/folder.png' class='center'></td>";
#      $result.="<td  style=''  class='list1'><a href='?plug=19&amp;post_id=12499984306&amp;act=dep_open&amp;dep_id=1'>Administrativa - [Funktionelle Daten]</a></td>";
#      $result.="<td  style='width:100px;'  class='list1'>&nbsp;</td>";
#      $result.="<td  style='width:158px;border-right:0px;'  class='list1'>&nbsp;</td>";
#      $result.="</tr>";
#    }

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

}

?>
