<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_plugin.inc 13896 2009-07-07 07:06:37Z hickert $$
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

class listingSortIterator implements Iterator {
  private $data;

  public function __construct($data, $direction, $attribute, $type= "string") {
    global $_sortAttribute;
    global $_sortType;
    $_sortAttribute= $attribute; 
    $_sortType= $type; 

    // Inline function to 
    function attrSort($ao, $bo) {
      global $_sortAttribute;
      global $_sortType;

      // Extract values from ao and bo
      $a= $b= "";
      if (isset($ao[$_sortAttribute])) {
        $a= $ao[$_sortAttribute];
        if (is_array($a)) {
          $a= $a[0];
        }
      }
      if (isset($bo[$_sortAttribute])) {
        $b= $bo[$_sortAttribute];
        if (is_array($b)) {
          $b= $b[0];
        }
      }

      // Take a look at the several types
      switch ($_sortType) {
        case 'string':
          return strcoll($a, $b);

        case 'integer':
          return $b - $a;

        case 'date':
          if ($a == "") {
            $a= "31.12.0000";
          }
          if ($b == "") {
            $b= "31.12.0000";
          }
          list($d, $m, $y)= explode('.', $a);
          $a= (int)sprintf("%04d%02d%02d", $y, $m, $d);
          list($d, $m, $y)= explode('.', $b);
          $b= (int)sprintf("%04d%02d%02d", $y, $m, $d);
          return $b-$a;

        // Sort for string by default
        default:
          return strcoll($a, $b);
      }
    }

    // Sort for attribute
    if ($attribute != "") {
      uasort($data, "attrSort");
    }

    // Invert if direction is set
    if ($direction) {
      $this->data= array_reverse($data, true);
    } else {
      $this->data= $data;
    }
  }

  function rewind(): void {
    reset($this->data);
  }

  function current(): mixed {
    return current($this->data);
  }

  function key(): mixed {
    return key($this->data);
  }

  function next(): void {
    next($this->data);
  }

  function valid(): bool {
    return key($this->data) !== null;
  }
}

?>
