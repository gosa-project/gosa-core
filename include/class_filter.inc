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

  var $searches= array();
  var $xmlSearches= array();
  var $attributes= array();
  var $search;
  var $defaultSearch;
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
    $xmlData= xml::xml2array($contents, 1);

    if (!isset($xmlData['filterdef'])) {
      return false;
    }

    $xmlData= $xmlData["filterdef"];

    // Load filter
    if (isset($xmlData['search'])) {

      // Array conversion
      if (!isset($xmlData['search'][0])) {
        $searches= array($xmlData['search']);
      } else {
        $searches= $xmlData['search'];
      }

      /* Store available searches */
      foreach ($searches as $search) {

        /* Do multi conversation */ 
        if (!isset($search['query'][0])){
          $search['query']= array($search['query']);
        }

        /* Store search */
        $this->xmlSearches[$search['tag']]= $search;

      }
    } else {
      return false;
    }

    // Transfer scope
    $this->scopeMode= $xmlData['definition']['scope'];
    if ($this->scopeMode == "auto") {
      $this->scope= "one";
    } else {
      $this->scope= $this->scopeMode;
    }

    // Transfer attributes
    $this->attributes= $xmlData['definition']['attribute'];
    if (!is_array($this->attributes)) {
      $this->attributes= array($this->attributes);
    }

    // Transfer initial value
    if (isset($xmlData['definition']['initial']) && $xmlData['definition']['initial'] == "true"){
      $this->initial= true;
    }

    // Transfer category
    if (isset($xmlData['definition']['category'])){
      $this->category= $xmlData['definition']['category'];
    }
    if (!is_array($this->category)) {
      $this->category= array($this->category);
    }

    // Initialize searches and default search mode
    $this->defaultSearch= $xmlData['definition']['default'];
    $this->reloadFilters();
    $this->setSearch($this->defaultSearch);

    return true;  
  }


  function reloadFilters()
  {
    $this->searches= array_merge($this->xmlSearches, userFilter::getFilter($this->category));
    $this->setSearch($this->search);
  }


  function setSearch($method= null)
  {
    $patch= null;

    // Maybe our search method has gone?
    if (!isset($this->searches[$method])) {
      $method= $this->defaultSearch;
    }

    // Try to use it, but bail out if there's no help...
    if (isset($this->searches[$method])) {
      $this->query= $this->searches[$method]['query'];
      $this->search= $method;
    } else {
      die ("Invalid search module!");
    }
  }


  function getTextfield($tag, $value= "", $element= null)
  {
    $size= 30;
    $maxlength= 30;
    $result= "<input class='filter_textfield' id='$tag' name='$tag' type='text' size='$size' maxlength='{$maxlength}' value='".$value."'>";
    if ($element && isset($element['autocomplete'])) {
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

      $this->autocompleters[$tag]= $element;
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



  function render()
  {
    $content= "<table class='filter-wrapper'><tr><td>".$this->renderFilterMenu()."</td><td>";
    $content.= "<div class='search-filter'>".$this->getTextfield('search_filter', $this->value, $this->searches[$this->search])."</div>".
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
      if (!isset($query['backend']) || !isset($query['filter'])) {
        die("No backend specified in search config.");
      }

      // Is backend available?
      $backend= "filter".$query['backend'];
      if (!isset($class_mapping["$backend"])) {
        die("Invalid backend specified in search config.");
      }

      // Load filter and attributes
      $filter= $query['filter'];

      // Handle converters if present
      if ($this->converter) {
        preg_match('/([^:]+)::(.*)$/', $this->converter, $m);
        if ($this->value == "") {
          $filter= call_user_func(array($m[1], $m[2]), preg_replace('/\$/', "*", $filter));
        } else {
          $filter= call_user_func(array($m[1], $m[2]), preg_replace('/\$/', $this->value, $filter));
        }
      }

      // Do not replace escaped \$ - This is required to be able to search for e.g. windows machines.
      if ($this->value == "") {
        $filter= preg_replace("/\\$/", '*', $filter);
      } else {
        $filter= preg_replace("/\\$/", "*".normalizeLdap($this->value)."*", $filter);
      }

      $result= array_merge($result, call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $this->attributes, $this->category, $this->objectStorage));
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
      if ($this->scopeMode == "auto" && isset($_POST['act']) && $_POST['act'] == "toggle-subtree") {
        $this->scope= ($this->scope == "one")?"sub":"one";
      }

      // Switch filter?
      if (isset($_POST['act'])) {
        foreach ($this->searches as $tag => $cfg) {
          if ($_POST['act'] == "filter-$tag") {
            $this->setSearch($tag);
            break;
          }
        }
      }
    }

  }


  function getCompletitionList($config, $value="*")
  {
    global $class_mapping;
    $res= array();

    // Load result attributes
    $attributes= $config['autocomplete']['attribute'];
    if (!is_array($attributes)) {
      $attributes= array($attributes);
    }

    // Do the query
    $result= array();

    // Is backend available?
    # FIXME
    $queries= $config['query'];
    if (!isset($queries[0])){
      $queries= array($queries);
    }
    foreach ($queries as $query) {
      $backend= "filter".$query['backend'];
      if (!isset($class_mapping["$backend"])) {
        die("Invalid backend specified in search config.");
      }
      $filter= preg_replace("/\\$/", "*".normalizeLdap($value)."*", $query['filter']);

      $result= array_merge($result, call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $attributes,
          $this->category, $this->objectStorage));
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
      $result= $this->getCompletitionList($this->searches[$this->search], $_POST['search_filter']);
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
      "<ul class='level1' id='filter-root'><li><a href='#'>".image("images/filter.png").image("images/lists/sort-down.png")."</a>";

    // Build ul/li list
    $separator= " style='border-top:1px solid #AAA' ";
    $result.= "<ul class='level2'>";

    // Build in filters
    foreach ($this->xmlSearches as $tag => $config) {
      if ($tag == $this->search) {
        $result.= "<li><a href='#'>".image("images/checked.png")."&nbsp;"._($config['label'])."</a></li>";
      } else {
        $result.= "<li><a href='#' onClick='document.getElementById(\"filter\").value= \"filter-$tag\";mainform.submit();'>".image("images/empty.png")."&nbsp;"._($config['label'])."</a></li>";
      }
    }

    // User defined filters
    $first= true;
    foreach (userFilter::getFilter($this->category) as $tag => $config) {
      if ($tag == $this->search) {
        $result.= "<li".($first?$separator:"")."><a href='#'>".image("images/checked.png")."&nbsp;"._($config['description'])."</a></li>";
      } else {
        $result.= "<li".($first?$separator:"")."><a href='#' onClick='document.getElementById(\"filter\").value= \"filter-$tag\";mainform.submit();'>".image("images/empty.png")."&nbsp;"._($config['description'])."</a></li>";
      }

      $first= false;
    }

    // Render scope if set to auto
    if ($this->scopeMode == "auto") {
      $result.= "<li$separator><a href='#' onClick='document.getElementById(\"filter\").value= \"toggle-subtree\";mainform.submit();'>".($this->scope=="one"?image("images/empty.png"):image("images/checked.png"))."&nbsp;"._("Search in subtrees")."</a></li>";
    }


    // Edit filter menu
    $result.= "<li$separator><a href='#' onClick='document.getElementById(\"filter\").value= \"config-filter\";mainform.submit();'>".image("images/configure.png")."&nbsp;"._("Edit filters")."...</a></li>";

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
