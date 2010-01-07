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

  function sortableListing($data= array())
  {
    global $config;

    // Save data to display
    $this->setData($data);

    // Generate instance wide unique ID
    $tmp= gettimeofday();
    $this->id= 'l'.md5($t['sec']);
  }


  function setData($data)
  {
    if (!is_array($data)) {
      die ("sortableList needs an array as data!");
    }

    // Transfer information
    $this->data= $data;

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
    $result= "<div class='sortableListContainer' style='width: ".$this->width."; height: ".$this->height."'>\n";
    $result.= "<table border='0' cellpadding='0' cellspacing='0' width='100%' height='100%' style='position: relative;'".(!empty($this->cssclass)?" class='".$this->cssclass."'":"").">\n";

    // Do we need colspecs?
    if ($this->colspecs) {
      $result.= " <colgroup>\n";
      for ($i= 0; $i<$this->columns; $i++) {
        $result.= "  <col width='".(isset($this->colspecs[$i])?$this->colspecs[$i]:"*")."'/>\n";
      }
      $result.= " </colgroup>\n";
    }

    // Do we need a header?
    if ($this->header) {
      $result.= " <thead>\n  <tr>\n";
      for ($i= 0; $i<$this->columns; $i++) {
        $result.= "   <th>".(isset($this->header[$i])?$this->header[$i]:"")."</th>\n";
      }
      $result.= "  </tr>\n </thead>\n";
    }

    // Render table body
    $result.= " <tbody id='".$this->id."'>\n";
    foreach ($this->data as $nr => $row) {
      $result.= "  <tr class='sortableListItem".($nr&1?'Odd':'')."' id='item_$nr'>\n";
      foreach ($row as $column) {
        $result.= "   <td>".htmlentities($column)."</td>\n";
      }
      $result.= "  </tr>\n";
    }

    $result.= " </tbody>\n</table>\n</div>\n";
    $result.= " <div id='dropmarker' class='dropmarker' style='display: none; width: ".(intval($this->width)-17)."px'\>\n";

    // Append script stuff if needed
    $result.= '<script type="text/javascript" language="javascript">';
    $result.= ' function updateOrder(){';
    $result.= '    var ampcharcode= \'%26\';';
    $result.= '    var serializeOpts = Sortable.serialize(\''.$this->id.'\')+ unescape(ampcharcode)+"key='.$this->id.'"+unescape(ampcharcode)+"update=products";';
    $result.= '    var options = {';
    $result.= '        method : \'post\',';
    $result.= '        parameters : serializeOpts';
    $result.= '        };';
//    $result.= '      // new Ajax.Request(\'Reorder.aspx\',options);';
    $result.= ' }';
    $result.= ' Sortable.create(\''.$this->id.'\',{tag:\'tr\', ghosting:true,constraint:\'vertical\', onUpdate : updateOrder,tree:true})';
    $result.= '</script>';

    return $result;
  }


  function update()
  {
  }

}
