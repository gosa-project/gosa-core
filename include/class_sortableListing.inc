<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_listing.inc 15087 2010-01-06 13:45:49Z hickert $$
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

class sortableListing {
  private $header= null;
  private $colspecs= null;
  private $reorderable= true;
  private $width= "400px";
  private $height= "100px";
  private $cssclass= "";
  private $id;

  private $data= array();
  private $columns= 0;
  private $deleteable= false;
  private $editable= false;
  private $instantDelete= true;
  private $action;
  private $targets;
  private $mapping;
  private $current_mapping;
  private $active_index;

  function sortableListing($data= array())
  {
    global $config;

    // Save data to display
    $this->setData($data);

    // Generate instance wide unique ID
    $tmp= gettimeofday();
    $this->id= 'l'.md5($tmp['sec']);
  }


  function setData($data)
  {
    if (!is_array($data)) {
      die ("sortableList needs an array as data!");
    }

    // Transfer information
    $this->data= $data;

    // Create initial mapping
    $this->mapping= array_keys($data);

    // Find the number of coluns
    reset($this->data);
    $first= current($this->data);
    if (is_array($first)) {
      $this->columns= count($first);
    } else {
      $this->columns= 1;
    }
  }


  function setWidth($width)
  {
    $this->width= $width;
  }


  function setInstantDelete($flag)
  {
    $this->instantDelete= $flag;
  }


  function setEditable($flag)
  {
    $this->editable= $flag;
  }


  function setDeleteable($flag)
  {
    $this->deleteable= $flag;
  }


  function setHeight($height)
  {
    $this->height= $height;
  }


  function setCssClass($css)
  {
    $this->cssclass= $css;
  }


  function setHeader($header)
  {
    $this->header= $header;
  }


  function setColspecs($specs)
  {
    $this->colspecs= $specs;
  }


  function render()
  {
    $result= "<div class='sortableListContainer' id='scroll_".$this->id."' style='width: ".$this->width."; height: ".$this->height."'>\n";
    $result.= "<table border='0' cellpadding='0' cellspacing='0' width='100%' height='100%' style='position: relative;'".(!empty($this->cssclass)?" class='".$this->cssclass."'":"").">\n";
    $action_width= 0;
    $edit_image= $this->editable?"<input class='center' type='image' src='images/lists/edit.png' alt='"._("Edit")."' name='%ID' title='"._("Edit this entry")."'>":"";
    $delete_image= $this->deleteable?"<input class='center' type='image' src='images/lists/trash.png' alt='"._("Delete")."' name='%ID' title='"._("Delete this entry")."'>":"";

    // Do we need colspecs?
    $action_width= ($this->editable?20:0) + ($this->deleteable?20:0);
    if ($this->colspecs) {
      $result.= " <colgroup>\n";
      for ($i= 0; $i<$this->columns; $i++) {
        $result.= "  <col width='".(isset($this->colspecs[$i])?$this->colspecs[$i]:"*")."'/>\n";
      }

      // Extend by another column if we've actions specified
      if ($action_width) {
        $result.= "  <col width='$action_width'/>\n";
      }
      $result.= " </colgroup>\n";
    }

    // Do we need a header?
    if ($this->header) {
      $result.= " <thead>\n  <tr>\n";
      for ($i= 0; $i<$this->columns; $i++) {
        $result.= "   <th>".(isset($this->header[$i])?$this->header[$i]:"")."</th>";
      }
      if ($action_width) {
        $result.= "<th>&nbsp;</th>";
      }
      $result.= "\n  </tr>\n </thead>\n";
    }

    // Render table body
    $result.= " <tbody id='".$this->id."'>\n";
    foreach ($this->mapping as $nr => $row) {
      $result.= "  <tr class='sortableListItem".($nr&1?'Odd':'')."' id='item_".$this->id."_$nr'>\n";
      foreach ($this->data[$row] as $column) {
        $result.= "   <td>".htmlentities($column)."</td>\n";
      }
      if ($action_width) {
        $result.= "<td>".str_replace('%ID', "edit_".$this->id."_$nr", $edit_image).
                         str_replace('%ID', "del_".$this->id."_$nr", $delete_image)."</td>";
      }
      $result.= "  </tr>\n";
    }

    $result.= " </tbody>\n</table>\n</div>\n";
    $result.= " <input type='hidden' name='reorder_".$this->id."' id='reorder_".$this->id."'>\n";

    // Append script stuff if needed
    $result.= '<script type="text/javascript" language="javascript">';
    $result.= ' function updateOrder(){';
    $result.= '    var ampcharcode= \'%26\';';
    $result.= '    var serializeOpts = Sortable.serialize(\''.$this->id.'\')+"='.$this->id.'";';
    $result.= '    $("reorder_'.$this->id.'").value= serializeOpts;';
    $result.= '    document.mainform.submit();';
    $result.= ' }';
    $result.= 'Position.includeScrollOffsets = true;';
    $result.= ' Sortable.create(\''.$this->id.'\',{tag:\'tr\', ghosting:false,constraint:\'vertical\', scroll:\'scroll_'.$this->id.'\',onUpdate : updateOrder})';
    $result.= '</script>';

    return $result;
  }


  function update()
  {
    // Do not do anything if we're not posted
    if (isset($_POST['reorder_'.$this->id])){

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
        return;
      }

      // Delete requested?
      foreach ($_POST as $key => $value) {
        if (preg_match('/^del_'.$this->id.'_([0-9]+)_x.*$/', $key, $matches)) {
          $this->active_index= $this->mapping[$matches[1]];
          $this->action= "delete";
          if ($this->instantDelete) {
            $this->deleteEntry($this->active_index);
          }
        }
      }

      // Edit requested?
      foreach ($_POST as $key => $value) {
        if (preg_match('/^edit_'.$this->id.'_([0-9]+)_x.*$/', $key, $matches)) {
          $this->active_index= $this->mapping[$matches[1]];
          $this->action= "edit";
        }
      }

    }
  }


  function getAction()
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


  function deleteEntry($id)
  {
    $index= array_search($id, $this->mapping);
    if ($index !== false) {
      unset($this->mapping[$index]);
      $this->mappings= array_values($this->mappings);
    }
  }


}
