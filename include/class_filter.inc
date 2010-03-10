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

class filter {

  var $xmlData;
  var $searches= array();
  var $search;
  var $category= "";
  var $objectStorage= array();
  var $base= "";
  var $scope= "";
  var $query;
  var $value= "";
  var $initial= false;
  var $scopeMode= "auto";
  var $converter= null;
  var $pid;


  function filter($filename)
  {
    global $config;

    // Load eventually passed filename
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }

    $this->pid= preg_replace("/[^0-9]/", "", microtime(TRUE)); 
  }


  function load($filename)
  {
    $contents = file_get_contents($filename);
    $this->xmlData= xml::xml2array($contents, 1);

    if (!isset($this->xmlData['filterdef'])) {
      return false;
    }

    $this->xmlData= $this->xmlData["filterdef"];

    // Load filter
    if (isset($this->xmlData['search'])) {

      // Array conversion
      if (!is_array($this->xmlData['search'])) {
        $searches= array($this->xmlData['search']);
      } else {
        $searches= $this->xmlData['search'];
      }

      /* Store available searches */
      foreach ($this->xmlData['search'] as $search) {

        /* Do multi conversation */ 
        if (!is_array($search['query'])){
          $search['query']= array($search['query']);
        }

        /* Store search */
        $this->searches[$search['label']]= $search;

      }
    } else {
      return false;
    }

    // Transfer scope
    $this->scopeMode= $this->xmlData['definition']['scope'];
    if ($this->scopeMode == "auto") {
      $this->scope= "one";
    } else {
      $this->scope= $this->scopeMode;
    }

    // Transfer initial value
    if (isset($this->xmlData['definition']['initial']) && $this->xmlData['definition']['initial'] == "true"){
      $this->initial= true;
    }

    // Transfer category
    if (isset($this->xmlData['definition']['category'])){
      $this->category= $this->xmlData['definition']['category'];
    }

    // Set default search mode
    $this->setSearch($this->xmlData['definition']['default']);

    return true;  
  }


  function setSearch($method)
  {
    // Move information
    if (isset($this->searches[$method])) {
      $this->query= $this->searches[$method]['query'];
      if (!isset($this->query[0])) {
        $this->query= array($this->query);
      }
      $this->search= $method;
    } else {
      die ("Invalid search module!");
    }
  }


  function getTextfield($element)
  {
    $tag= $element['tag'];
    $size= 30;
    if (isset($element['size'])){
      $size= $element['size'];
    }
    $maxlength= 30;
    if (isset($element['maxlength'])){
      $maxlength= $element['maxlength'];
    }
    $result= "<input class='filter_textfield' id='$tag' name='$tag' type='text' size='$size' maxlength='{$maxlength}' value='".$this->elementValues[$tag]."'>";
    if (isset($element['autocomplete'])) {
      $frequency= "0.5";
      $characters= "1";
      if (isset($element['autocomplete']['frequency'])) {
        $frequency= $element['autocomplete']['frequency'];
      }
      if (isset($element['autocomplete']['characters'])) {
        $characters= $element['autocomplete']['characters'];
      }
      $result.= "<div id='autocomplete$tag' class='autocomplete'></div>".
        "<script type='text/javascript'>".
        "new Ajax.Autocompleter('$tag', 'autocomplete$tag', 'autocomplete.php', { minChars: $characters, frequency: $frequency });".
        "</script>";

      $this->autocompleters[$tag]= $element['autocomplete'];
    }
    return $result;
  }


  function getCurrentBase()
  {
    if (isset($this->search->base) && (string)$this->search->scope != "auto") {
      return false;
    }

    return $this->base;
  }


  function getCurrentScope()
  {
    if (isset($this->search->scope) && (string)$this->search->scope != "auto") {
      return (string)$this->search->scope;
    }

    return $this->scope;
  }


  function setConverter($hook)
  {
    $this->converter= $hook;
  }


  function setObjectStorage($storage)
  {
    $this->objectStorage= $storage;    
  }


  function setBase($base)
  {
    $this->base= $base;
  }


  function setCurrentScope($scope)
  {
    $this->scope= $scope;
  }


  function renderApply()
  {
    return ("<input type='submit' name='apply' value='"._("Apply filter")."'>");
  }


  function renderScope()
  {
    $checked= $this->scope == "sub"?" checked":"";
    return "<input type='checkbox' id='SCOPE' name='SCOPE' value='1' onClick='document.mainform.submit();'$checked>&nbsp;<LABEL for='SCOPE'>"._("Search in subtrees")."</LABEL>";
  }


  function render()
  {
    $content= "<table class='filter-wrapper'><tr><td>".$this->renderFilterMenu()."</td><td>";
    $content.= "<div class='search-filter'><input type='text' name='search_filter' value='".$this->value."'></div>".
      "&nbsp;<button class='search-filter' type='submit' title='"._("Search")."'>".image("images/find.png")."</button></td></tr></table>";

    // Return meta data
    return ("<input type='hidden' name='FILTER_PID' value='".$this->pid."'>".$content);
  }


  function query()
  {
    global $class_mapping;
    $result= array();

    // Return empty list if initial is not set
    if (!$this->initial) {
      $this->initial= true;
      return $result;
    }

    // Go thru all queries and merge results
    foreach ($this->query as $query) {
      if (!isset($query['backend']) || !isset($query['filter']) || !isset($query['attribute'])) {
        die("No backend specified in search config.");
      }

      // Is backend available?
      $backend= "filter".$query['backend'];
      if (!isset($class_mapping["$backend"])) {
        die("Invalid backend specified in search config.");
      }

      // Load filter and attributes
      $filter= $query['filter'];
      $attributes= $query['attribute'];

      // Handle converters if present
      if ($this->converter) {
        preg_match('/([^:]+)::(.*)$/', $this->converter, $m);
        $filter= call_user_func(array($m[1], $m[2]), preg_replace('/\$/', $this->value, $filter));
      }

      // Do not replace escaped \$ - This is required to be able to search for e.g. windows machines.
      if ($this->value == "") {
        $filter= preg_replace("/\\$/", '*', $filter);
      } else {
        $filter= preg_replace("/\\$/", "*".normalizeLdap($this->value)."*", $filter);
      }

      $result= array_merge($result, call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $attributes, $this->category, $this->objectStorage));
    }

    return ($result);
  }


  function update()
  {
    if (isset($_POST['FILTER_PID']) && $_POST['FILTER_PID'] == $this->pid) {

      // Save input field
      if (isset($_POST['search_filter'])) {
        $this->value= validate($_POST['search_filter']);
      }

      // Save scope if needed
      if ($this->scopeMode == "auto") {
        $this->scope= isset($_POST['SCOPE'])?"sub":"one";
      }
    }

  }


  function getCompletitionList($config, $value="*")
  {
    global $class_mapping;
    $res= array();

    // Is backend available?
    $backend= "filter".$config['backend'];
    if (!isset($class_mapping["$backend"])) {
      die("Invalid backend specified in search config.");
    }

    // Load filter and attributes
    $filter= $config['filter'];
    $attributes= $config['attribute'];
    if (!is_array($attributes)) {
      $attributes= array($attributes);
    }

    // Make filter
    $filter= preg_replace("/\\$/", normalizeLdap($value), $filter);
    if (isset($config['base']) && isset($config['scope']) && isset($config['category'])) {
      $result= call_user_func(array($backend, 'query'), $config['base'], $config['scope'], $filter, $attributes,
          $config["category"], $config["objectStorage"]);
    } else {
      $result= call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $attributes,
          $this->category, $this->objectStorage);
    }

    foreach ($result as $entry) {
      foreach ($attributes as $attribute) {
        if (is_array($entry[$attribute])) {
          for ($i= 0; $i<$entry[$attribute]['count']; $i++) {
            if (mb_stristr($entry[$attribute][$i], $value)) {
              $res[]= $entry[$attribute][$i];
            }
          }
        } else {
          $res[]= $entry[$attribute];
        }
      }
    }

    return $res;
  }


  function processAutocomplete()
  {
    global $class_mapping;
    $result= array();

    // Introduce maximum number of entries
    $max= 25;

    if(isset($this->searches[$this->search]['autocomplete'])){
      $result= $this->getCompletitionList($this->searches[$this->search]['autocomplete'], $_POST['search_filter']);
      $result= array_unique($result);
      asort($result);

      echo '<ul>';
      foreach ($result as $entry) {
        echo '<li>'.mark($_POST['search_filter'], $entry).'</li>';
        if ($max-- == 0) {
          break;
        }
      }

      echo '</ul>';
    }
  }


  function getObjectBase($dn)
  {
    global $config;
    $base= "";

    // Try every object storage
    $storage= $this->objectStorage;
    if (!is_array($storage)){
      $storage= array($storage);
    }
    foreach ($storage as $location) {
      $pattern= "/^[^,]+,".preg_quote($location, '/')."/i";
      $base= preg_replace($pattern, '', $dn);
    }

    /* Set to base, if we're not on a correct subtree */
    if (!isset($config->idepartments[$base])){
      $base= $config->current['BASE'];
    }

    return $base;
  }



  function renderFilterMenu()
  {
    // Load shortcut
    $result= "<input type='hidden' name='act' id='filter' value=''><div style='display:none'><input type='submit' name='exec_filter' id='exec_filter' value=''></div>".
      "<ul class='level1' id='filter-root'><li><a href='#'>".image("images/filter.png")._("...")."&nbsp;".image("images/lists/sort-down.png")."</a>";

    // Build ul/li list
    $separator= " style='border-top:1px solid #AAA' ";
    $result.= "<ul class='level2'>";
    $result.= "<li><a href='#'>Dummy placeholder</a></li>";
    $result.= "<li$separator><a href='#' onClick='document.getElementById(\"filter\").value= \"config-filter\";mainform.submit();'>".image("images/configure.png")."&nbsp;"._("Edit filters")."...</a></li>";

#############################
print_a(userFilter::getFilter($category=array()));
print_a($this->searches);
#############################

#$result.= "<li$separator><a href='#' onClick='document.getElementById(\"actionmenu\").value= \"".$action['name']."\";mainform.submit();'>$img"._($action['label'])."</a></li>";

# AUTOCOMPLETER and getTextbox

    $result.= "</ul>";

    $script= '<script type="text/javascript" language="JavaScript">var menu2; menu2= new Menu("filter-root", "menu2", configMenu)</script>';

    return "<div id='filtermenu'>".$result."</li></ul><div>$script";
  }


  function getFixedFilters()
  {
    return array_keys($this->searches);
  }

}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
