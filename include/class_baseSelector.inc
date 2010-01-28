<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2010 GONICUS GmbH
 *
 * ID: $$Id: class_listing.inc 15296 2010-01-26 08:27:39Z cajus $$
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

class baseSelector {

  private $base;
  private $pid;
  protected $tree;
  protected $pathMapping;

  // It would be better to get a dn -> [name, description, type] array
  // to avoid these tasks be done several times. Skipping for the moment.
  #'dc' => 'plugins/departments/images/domain.png',
  #'dc' => 'plugins/departments/images/dc.png',
  #'l' => 'plugins/departments/images/locality.png',
  #'c' => 'plugins/departments/images/country.png',
  #'o' => 'plugins/departments/images/organization.png',
  #'ou' => 'plugins/departments/images/folder.png',

  function __construct($bases, $base= "")
  {
    // Initialize pid
    $this->pid= preg_replace("/[^0-9]/", "", microtime(TRUE));

    // Transfer data
    $this->setBases($bases);
    $this->setBase($base);
    $this->update();
  }


  function setBase($base)
  {
    if (isset($this->pathMapping[$base])) {
      $this->base= $base;
    } else {
      die("Invalid base selected");
    }
  }


  function setBases($bases)
  {
    global $config;

    $this->pathMapping= array();
    $selected= $this->base == $config->current['BASE']?"Selected":"";
    $first= true;
    $last_indent= 2;

    foreach ($bases as $base => $dummy) {

      // Build path style display
      $elements= explode(',', substr($base, 0, strlen($base) - strlen($config->current['BASE'])));
      $elements= array_reverse($elements, true);

      $this->pathMapping[$base]= $base == $config->current['BASE']? '/' : ldap::fix(preg_replace('/(^|,)[a-z0-9]+=/i', '/', implode(',', $elements)));
    }

    // Save bases to session for autocompletion
    session::global_set('pathMapping', $this->pathMapping);
  }


  function update()
  {
    global $config;
    $this->tree= "<input type='text' size='30' name='bs_input_".$this->pid."' id='bs_input_".$this->pid."' onfocus=\"\$('bs_".$this->pid."').hide()\" onmouseover=\"\$('bs_".$this->pid."').show();pos = Element.positionedOffset('bs_input_".$this->pid."');\$('bs_".$this->pid."').setStyle({left: (pos[0]), top: (pos[1]+ Element.getHeight('bs_input_".$this->pid."'))});\" onmouseout=\"rtimer= Element.hide.delay(0.25, 'bs_".$this->pid."')\" value='".$this->pathMapping[$this->base]."'>";

    $selected= $this->base == $config->current['BASE']?"Selected":"";
    $this->tree.= "<div class='treeList' style='display:none' id='bs_".$this->pid."' onmouseover=\"window.clearTimeout(rtimer);\" onmouseout=\"rtimer= Element.hide.delay(0.25, 'bs_".$this->pid."')\"><a class='treeList$selected' href='#'>/ ["._("Root")."]</a><ul class='treeList'>\n";
    $first= true;
    $last_indent= 2;

    foreach ($this->pathMapping as $base => $dummy) {

      // Skip root for tree
      if ($base == $config->current['BASE']) {
        continue;
      }

      // Build path style display
      $elements= explode(',', substr($base, 0, strlen($base) - strlen($config->current['BASE'])));

      $indent= count($elements);
      if (!$first && ($indent == $last_indent)) {
        $this->tree.= "</li>\n";
      }
      if ($indent > $last_indent) {
        $this->tree.= "<ul>\n";
      }
      if ($indent < $last_indent) {
        for ($i= 0; $i < ($last_indent-$indent); $i++) {
          $this->tree.= "</li></ul>\n";
        }
        $this->tree.= "</li>\n";
      }
      $selected= $this->base == $base?" class='treeListSelected'":"";
      $this->tree.= "<li><a$selected href='#'>".str_replace(' ', '&nbsp;', ldap::fix(preg_replace('/^[a-z0-9]+=([^,]+),.*$/i', '$1', $base)))."</a>";

      $last_indent= $indent;
      $first= false;
    }

    // Close tree
    for ($i= 0; $i<$last_indent; $i++) {
      $this->tree.= "</li></ul>\n";
    }
    $this->tree.= "</div>\n";
  }

  function render()
  {
    $result= "";
    return $this->tree;
  }


  function getBase()
  {
    return $this->base;
  }


  function getAction()
  {
    // Do not do anything if this is not our PID, or there's even no PID available...
    #if(!isset($_REQUEST['PID']) || $_REQUEST['PID'] != $this->pid) {
    #  return;
    #}
  }

}

?>
