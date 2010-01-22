<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2010 GONICUS GmbH
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

define ('LIST_NORMAL', 0);
define ('LIST_MARKED', 1);
define ('LIST_DISABLED', 2);

class sortableListing {
  private $header= null;
  private $colspecs= null;
  private $reorderable= true;
  private $width= "400px";
  private $height= "100px";
  private $cssclass= "";
  private $id;

  private $data= array();
  private $keys= array();
  private $modes= array();
  private $displayData= array();
  private $columns= 0;
  private $deleteable= false;
  private $editable= false;
  private $colorAlternate= false;
  private $instantDelete= true;
  private $action;
  private $mapping;
  private $current_mapping;
  private $active_index;
  private $scrollPosition= 0;
  private $sortColumn= 0;
  private $sortDirection= array();

  private $acl= "";
  private $modified= false;

  public function sortableListing($data= array(), $displayData= null, $modes= null, $reorderable= false)
  {
    global $config;

    // Save data to display
    $this->setListData($data, $displayData, $modes);

    // Generate instance wide unique ID
    $tmp= gettimeofday();
    $this->id= 'l'.md5($tmp['sec']);

    // Set reorderable flag
    $this->reorderable= $reorderable;
    if (!$reorderable) {
      $this->sortData();
    }
  }


  public function setListData($data, $displayData= null, $modes= null)
  {
    // Save data to display
    $this->setData($data);
    if (!$displayData) {
      $displayData= array();
      foreach ($data as $key => $value) {
        $displayData[$key]= array($value);
      }
    }
    if (!$modes) {
      $modes= array();
      foreach ($data as $key => $value) {
        $modes[$key]= LIST_NORMAL;
      }
    }

    $this->setDisplayData($displayData);
    $this->setModes($modes);
  }


  private function setData($data)
  {
    $this->data= $data;
  }


  private function setModes($modes)
  {
    $this->modes= $modes;
  }


  private function setDisplayData($data)
  {
    if (!is_array($data)) {
      die ("sortableList needs an array as data!");
    }

    // Transfer information
    $this->displayData= array_values($data);
    $this->keys= array_keys($data);

    // Create initial mapping
    $this->mapping= range(0, abs(count($this->keys)-1));
    $this->current_mapping= $this->mapping;

    // Find the number of coluns
    reset($this->displayData);
    $first= current($this->displayData);
    if (is_array($first)) {
      $this->columns= count($first);
    } else {
      $this->columns= 1;
    }

    // Preset sort orders to 'down'
    for ($column= 0; $column<$this->columns; $column++) {
      $this->sortDirection[]= true;
    }
  }


  public function setWidth($width)
  {
    $this->width= $width;
  }


  public function setInstantDelete($flag)
  {
    $this->instantDelete= $flag;
  }


  public function setColorAlternate($flag)
  {
    $this->colorAlternate= $flag;
  }


  public function setEditable($flag)
  {
    $this->editable= $flag;
  }


  public function setDeleteable($flag)
  {
    $this->deleteable= $flag;
  }


  public function setHeight($height)
  {
    $this->height= $height;
  }


  public function setCssClass($css)
  {
    $this->cssclass= $css;
  }


  public function setHeader($header)
  {
    $this->header= $header;
  }


  public function setColspecs($specs)
  {
    $this->colspecs= $specs;
  }


  public function render()
  {
    $result= "<div class='sortableListContainer' id='scroll_".$this->id."' style='min-width:".$this->width.";height: ".$this->height."'>\n";
    $result.= "<table border='0' cellpadding='0' cellspacing='0' width='100%' height='100%' ".(!empty($this->cssclass)?" class='".$this->cssclass."'":"").">\n";
    $action_width= 0;
    if (strpos($this->acl, 'w') === false) {
      $edit_image= $this->editable?"<img class='center' src='images/lists/edit-grey.png' alt='"._("Edit")."'>":"";
    } else {
      $edit_image= $this->editable?"<input class='center' type='image' src='images/lists/edit.png' alt='"._("Edit")."' name='%ID' id='%ID' title='"._("Edit this entry")."'>":"";
    }
    if (strpos($this->acl, 'd') === false) {
      $delete_image= $this->deleteable?"<img class='center' src='images/lists/trash-grey.png' alt='"._("Delete")."'>":"";
    } else {
      $delete_image= $this->deleteable?"<input class='center' type='image' src='images/lists/trash.png' alt='"._("Delete")."' name='%ID' title='"._("Delete this entry")."'>":"";
    }

    // Do we need colspecs?
    $action_width= ($this->editable?20:0) + ($this->deleteable?20:0);
    if ($this->colspecs) {
      $result.= " <colgroup>\n";
      for ($i= 0; $i<$this->columns; $i++) {
        $result.= "  <col style='width:".(isset($this->colspecs[$i])?$this->colspecs[$i]:"*")."'/>\n";
      }

      // Extend by another column if we've actions specified
      if ($action_width) {
        $result.= "  <col style='width:".$action_width."px'/>\n";
      }
      $result.= " </colgroup>\n";
    }

    // Do we need a header?
    if ($this->header) {
      $result.= " <thead>\n  <tr>\n";
      $first= " style='border:0'";
      for ($i= 0; $i<$this->columns; $i++) {
        $link= "href='?plug=".$_GET['plug']."&amp;PID=".$this->id."&amp;act=SORT_$i'";
        $sorter= "";
        if ($i == $this->sortColumn){
          $sorter= "&nbsp;<img border='0' title='".($this->sortDirection[$i]?_("Up"):_("Down"))."' src='images/lists/sort-".($this->sortDirection[$i]?"up":"down").".png' align='top'>";
        }

        if ($this->reorderable) {
          $result.= "   <th$first>".(isset($this->header[$i])?$this->header[$i]:"")."</th>";
        } else {
          $result.= "   <th$first><a $link>".(isset($this->header[$i])?$this->header[$i]:"")."$sorter</a></th>";
        }
        $first= "";
      }
      if ($action_width) {
        $result.= "<th>&nbsp;</th>";
      }
      $result.= "\n  </tr>\n </thead>\n";
    }

    // Render table body if we've read permission
    $result.= " <tbody id='".$this->id."'>\n";
    $reorderable= $this->reorderable?"":" style='cursor:default'";
    if (strpos($this->acl, 'r') !== false) {
      foreach ($this->mapping as $nr => $row) {
        $editable= $this->editable?" onclick='$(\"edit_".$this->id."_$nr\").click()'":"";

        $id= "";
        switch ($this->modes[$row]) {
          case LIST_DISABLED:
            $id= " sortableListItemDisabled";
            $editable= false;
            break;
          case LIST_MARKED:
            $id= " sortableListItemMarked";
            break;
        }

        $result.= "  <tr class='sortableListItem".((($nr&1)||!$this->colorAlternate)?'':'Odd')."$id' id='item_".$this->id."_$nr'$reorderable>\n";
        $first= " style='border:0'";

        foreach ($this->displayData[$row] as $column) {
          $result.= "   <td$editable$first>".htmlentities($column)."</td>\n";
          $first= "";
        }

        if ($action_width) {
          $result.= "<td>".str_replace('%ID', "edit_".$this->id."_$nr", $edit_image).
                           str_replace('%ID', "del_".$this->id."_$nr", $delete_image)."</td>";
        }

        $result.= "  </tr>\n";
      }
    }

    // Add spacer
    $result.= "  <tr class='sortableListItemFill' style='height:100%'><td style='border:0'></td>";
    $num= $action_width?$this->columns:$this->columns-1;
    for ($i= 0; $i<$num; $i++) {
      $result.= "<td class='sortableListItemFill'></td>";
    }
    $result.= "</tr>\n";

    $result.= " </tbody>\n</table>\n</div>\n";
    $result.= " <input type='hidden' name='PID' value='".$this->id."' id='PID'>\n";
    $result.= " <input type='hidden' name='position_".$this->id."' id='position_".$this->id."'>\n";
    $result.= " <input type='hidden' name='reorder_".$this->id."' id='reorder_".$this->id."'>\n";

    // Append script stuff if needed
    $result.= '<script type="text/javascript" language="javascript">';
    if ($this->reorderable) {
      $result.= ' function updateOrder(){';
      $result.= '    var ampcharcode= \'%26\';';
      $result.= '    var serializeOpts = Sortable.serialize(\''.$this->id.'\')+"='.$this->id.'";';
      $result.= '    $("reorder_'.$this->id.'").value= serializeOpts;';
      $result.= '    document.mainform.submit();';
      $result.= ' }';
      $result.= 'Position.includeScrollOffsets = true;';
      $result.= ' Sortable.create(\''.$this->id.'\',{tag:\'tr\', ghosting:false, constraint:\'vertical\', scroll:\'scroll_'.$this->id.'\',onUpdate : updateOrder});';
    }
    $result.= '$("scroll_'.$this->id.'").scrollTop= '.$this->scrollPosition.';';
    $result.= 'var box = $("scroll_'.$this->id.'").onscroll= function() {$("position_'.$this->id.'").value= this.scrollTop;}';
    $result.= '</script>';

    return $result;
  }


  public function update()
  {
    // Do not do anything if this is not our PID, or there's even no PID available...
    if(!isset($_REQUEST['PID']) || $_REQUEST['PID'] != $this->id) {
      return;
    }

    // Filter GET with "act" attributes
    if (!$this->reorderable && isset($_GET['act'])) {
      $key= validate($_GET['act']);
      if (preg_match('/^SORT_([0-9]+)$/', $key, $match)) {
        // Switch to new column or invert search order?
        $column= $match[1];
        if ($this->sortColumn != $column) {
          $this->sortColumn= $column;
        } else {
          $this->sortDirection[$column]= !$this->sortDirection[$column];
        }

        // Update mapping according to sort parameters
        $this->sortData();
      }
    }

    // Do not do anything if we're not posted - or have no permission
    if (strpos($this->acl, 'w') !== false && isset($_POST['reorder_'.$this->id])){

      if (isset($_POST['position_'.$this->id]) && is_numeric($_POST['position_'.$this->id])) {
        $this->scrollPosition= $_POST['position_'.$this->id];
      }

      // Move requested?
      $move= $_POST['reorder_'.$this->id];
      if ($move != "") {
        preg_match_all('/=([0-9]+)[&=]/', $move, $matches);
        $this->action= "reorder";
        $tmp= array();
        foreach ($matches[1] as $id => $row) {
          $tmp[$id]= $this->mapping[$row];
        }
        $this->mapping= $tmp;
        $this->current_mapping= $matches[1];
        $this->modified= true;
        return;
      }
    }

    // Delete requested?
    if (strpos($this->acl, 'd') !== false){
      foreach ($_POST as $key => $value) {
        if (preg_match('/^del_'.$this->id.'_([0-9]+)_x.*$/', $key, $matches)) {
          $this->active_index= $this->mapping[$matches[1]];
          $this->action= "delete";
          if ($this->instantDelete) {
            $this->deleteEntry($this->active_index);
          }
        }
      }
    }

    // Edit requested?
    if (strpos($this->acl, 'w') !== false){
      foreach ($_POST as $key => $value) {
        if (preg_match('/^edit_'.$this->id.'_([0-9]+)_x.*$/', $key, $matches)) {
          $this->active_index= $this->mapping[$matches[1]];
          $this->action= "edit";
        }
      }
    }
  }


  public function getAction()
  {
    // Do not do anything if we're not posted
    if(!isset($_POST['reorder_'.$this->id])) {
      return;
    }

    // For reordering, return current mapping
    if ($this->action == 'reorder') {
      return array("targets" => $this->current_mapping, "mapping" => $this->mapping, "action" => $this->action);
    }

    // Edit and delete
    $result= array("targets" => array($this->active_index), "action" => $this->action);

    return $result;
  }


  private function deleteEntry($id)
  {
    // Remove mapping
    $index= array_search($id, $this->mapping);
    if ($index !== false) {
      unset($this->mapping[$index]);
      $this->mapping= array_values($this->mapping);
      $this->modified= true;
    }
  }


  public function getMaintainedData()
  {
    $tmp= array();

    foreach ($this->mapping as $src => $dst) {
      $tmp[$this->keys[$dst]]= $this->data[$dst];
    }

    return $tmp;
  }


  public function isModified()
  {
    return $this->modified;
  }


  public function setAcl($acl)
  {
    $this->acl= $acl;
  }


  public function sortData()
  {
    // Extract data
    $tmp= array();
    foreach($this->displayData as $item) {
      if (isset($item[$this->sortColumn])){
        $tmp[]= $item[$this->sortColumn];
      } else {
        $tmp[]= "";
      }
    }

    // Sort entries
    if ($this->sortDirection[$this->sortColumn]) {
      asort($tmp);
    } else {
      arsort($tmp);
    }

    // Adapt mapping accordingly
    $this->mapping= array();
    foreach ($tmp as $key => $value) {
      $this->mapping[]= $key;
    }
  }


  public function addEntry($entry, $displayEntry= null, $key= null)
  {
    // Only add if not already there
    if (!$key) {
      if (in_array($entry, $this->data)) {
        return;
      }
    } else {
      if (isset($this->data[$key])) {
        return;
      }
    }

    // Prefill with default value if not specified
    if (!$displayEntry) {
      $displayEntry= array($entry);
    }

    // Append to data and mapping
    if ($key) {
      $this->data[$key]= $entry;
      $this->keys[]= $key;
    } else {
      $this->data[]= $entry;
      $this->keys[]= count($this->mapping);
    }
    $this->displayData[]= $displayEntry;
    $this->mapping[]= count($this->mapping);
    $this->modified= true;
  }


  public function getKey($index) {
    return isset($this->keys[$index])?$this->keys[$index]:null;
  }

}
