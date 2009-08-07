<?php

class filter {

  var $xmlData;
  var $elements= array();
  var $elementValues= array();
  var $alphabetElements= array();
  var $search;
  var $definition;
  var $category= "";
  var $objectStorage= array();
  var $objectBase= "";
  var $base= "";
  var $bases= array();
  var $scope= "";
  var $query;
  var $baseMode= false;
  var $scopeMode= "auto";
  var $alphabet= null;
  var $templatePath= "";
  var $target= "";

  function filter($filename)
  {
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }
  }

  function load($filename)
  {
    // Load data into array
    $xmlData= simplexml_load_file($filename);
    if ($xmlData === false) {
      return false;
    }

    // Load definition
    if (isset($xmlData->definition)) {
      foreach ($xmlData->definition as $entry) {
        if (!isset($entry->target) || !isset($entry->template)) {
          return false;
        }

        // Move information
        $this->templatePath= (string)$entry->template;
        $this->target= (string)$entry->target;

        // One is enough
        break;
      }
    }

    // Load filter
    if (isset($xmlData->search)) {
      foreach ($xmlData->search as $entry) {
        if (!isset($entry->query) || !isset($entry->base) || !isset($entry->scope)) {
          return false;
        }

        // Move information
        $this->baseMode= (string)$entry->base;
        $this->scopeMode= (string)$entry->scope;
        $this->query= $entry->query;

        // One is enough
        break;
      }
    }

    // Generate formular data
    if (isset($xmlData->element)) {
      foreach ($xmlData->element as $element) {

        // Ignore elements without type
        if (!isset($element->type)) {
          next;
        }

        $tag= (string)$element->tag;

        // Store element for quick access
        $this->elements[$tag] = $element;

        // Preset elementValues with default values if exist
        if (isset($element->default)) {
          $this->elementValues[$tag] = (string)$element->default;
        } else {
          $this->elementValues[$tag] = "";
        }

        // Does this element react on alphabet links?
        if (isset($element->alphabet) && (string)$element->alphabet == "true") {
          $this->alphabetElements[]= $tag;
        }
      }
    }

    // Save definition
    if (isset($xmlData->definition)) {
     $this->definition = $xmlData->definition;
    }

    // Save search
    if (isset($xmlData->search)) {
     $this->search = $xmlData->search;
    }

    return true;  
  }


  function getTextfield($element)
  {
    $result= "<input class='filter_textfield' name='$element' type='text' size='30' maxlength='30' value='".$this->elementValues[$element]."'>";
    return $result;
  }


  function getCheckbox($element)
  {
    $checked= "";
    if ($this->elementValues[$element] == "true") {
      $checked= " checked";
    }

    $result= "<input class='filter_checkbox' name='$element' type='checkbox' onClick='document.mainform.submit();' value='true'$checked>";
    return $result;
  }


  function getCombobox($element)
  {
    $result= "<select name='".$element->tag."' size='1' onClick='document.mainform.submit();'>";
    foreach ($element->value as $key=>$value) {
      $selected= "";
      if ($this->elementValues[(string)$element->tag] == $value->key) {
        $selected= " selected";
      }
      $result.= "<option value='".$value->key."'$selected>{t}".$value->set."{/t}</option>";
    }
    $result.= "</select>";

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


  function setBases($bases) {
    $this->bases= $bases;    
  }


  function setObjectStorage($storage) {
    $this->objectStorage= $storage;    
  }


  function setObjectBase($base) {
    $this->objectBase= $base;    
  }


  function setCategory($category) {
    $this->category= $category;    
  }


  function setCurrentBase($base) {
    $this->base= $base;
  }


  function setCurrentScope($scope) {
    $this->scope= $scope;
  }


  function renderBase()
  {
    $result= "<select name='currentMainBase' onChange='mainform.submit()' size='1'>";

    foreach ($this->bases as $key=>$value) {
      $selected= "";
      if ($key == $this->base) {
        $selected= " selected";
      }
      $result.= "<option value='".$key."'$selected>".$value."</option>";
    }
    $result.= "</select>";

    return $result;
  }


  function renderAlphabet($columns= 10)
  {
    // Return pre-rendered alphabet if available
    if ($this->alphabet) {
      return ($this->alphabet);
    }

    $characters= _("*ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    $alphabet= "";
    $c= 0;

    /* Fill cells with charaters */
    for ($i= 0, $l= mb_strlen($characters, 'UTF8'); $i<$l; $i++){
      if ($c == 0){
        $alphabet.= "<tr>";
      }

      $ch = mb_substr($characters, $i, 1, "UTF8");
      $alphabet.= "<td><a class=\"alphaselect\" href=\"main.php?plug=".
        validate($_GET['plug'])."&amp;filter=".$ch."\">&nbsp;".$ch."&nbsp;</a></td>";

      if ($c++ == $columns){
        $alphabet.= "</tr>";
        $c= 0;
      }
    }

    /* Fill remaining cells */
    while ($c++ <= $columns){
      $alphabet.= "<td>&nbsp;</td>";
    }

    /* Save alphabet */
    $this->alphabet= "<table width='100%'>$alphabet</table>";

    return ($this->alphabet);
  }


  function renderApply()
  {
    return ("<input type='submit' name='apply' value='"._("Apply filter")."'>");
  }


  function renderScope()
  {
    $checked= $this->scope == "sub"?" checked":"";
    return "<input type='checkbox' name='SCOPE' value='1' onClick='document.mainform.submit();'$checked>&nbsp;"._("Search in subtrees");
  }


  function renderFilter()
  {
    $smarty= get_smarty();
    $smarty->assign("ALPHABET", $this->renderAlphabet());
    $smarty->assign("APPLY", $this->renderApply());
    $smarty->assign("SCOPE", $this->renderScope());
    $smarty->assign("BASE", $this->renderBase());

    // Load template and replace elementsHtml[]
    foreach ($this->elements as $tag => $element) {
      $htmlCode= "";
      switch ($element->type) {
        case "textfield":
          $htmlCode = $this->getTextfield($tag);
          break;

        case "checkbox":
          $htmlCode = $this->getCheckbox($tag);
          break;

        case "combobox":
          $htmlCode = $this->getCombobox($element);
          break;

        default:
          die ("Unknown element type specified!");
      }
      $smarty->assign("$tag", $htmlCode);
    }

    // Load template
    return ("<input type='hidden' name='FILTER_LOADED' value='1'>".$smarty->fetch(get_template_path("filter/$this->templatePath")));
  }


  function query()
  {
    global $class_mapping;
    $result= array();

    // Go thru all queries and merge results
    foreach ($this->query as $query) {
      if (!isset($query->backend) || !isset($query->filter) || !isset($query->attribute)) {
        die("No backend specified in search config.");
      }

      // Is backend available?
      $backend= "filter".(string)$query->backend;
      if (!isset($class_mapping["$backend"])) {
        die("Invalid backend specified in search config.");
      }

      // Load filter and attributes
      $filter= $query->filter;
      $attributes= array();
      foreach($query->attribute as $attr) {
        $attributes[]= (string)$attr;
      }

      // Generate final filter
      foreach ($this->elements as $tag => $element) {
        $e_set= (string)$element->set;
        $e_unset= (string)$element->unset;

        if ($this->elementValues[$tag] == "") {
          $e_unset= preg_replace('/\$/', normalizeLdap($this->elementValues[$tag]), $e_unset);
          $filter= preg_replace("/\\$$tag/", $e_unset, $filter);
        } else {
          $e_set= preg_replace('/\$/', normalizeLdap($this->elementValues[$tag]), $e_set);
          $filter= preg_replace("/\\$$tag/", $e_set, $filter);
        }
      }

      $result= array_merge($result, call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $attributes,
                               $this->category, $this->objectStorage, $this->objectBase));
    }
    

    return ($result);
  }


  function isValid()
  {
    foreach ($this->elements as $tag => $element) {
      if (isset($element->regex)){
        if (!preg_match('/'.(string)$element->regex.'/', $this->elementValues[$tag])){
          return false;
        }
      }
    }
    return true;
  }


  function update()
  {

    /* React on alphabet links if needed */
    if (isset($_GET['filter'])){
      $s= mb_substr(validate($_GET['filter']), 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      foreach ($this->alphabetElements as $tag) {
        $this->elementValues[$tag]= $s;
      }
    }

    if (isset($_POST['FILTER_LOADED'])) {
      // Load post values and adapt filter, base and scope accordingly - but
      // only if we didn't get a _GET
      foreach ($this->elements as $tag => $element) {
        if (isset($_POST[$tag])){
          $this->elementValues[$tag]= validate($_POST[$tag]);
        } else {
          $this->elementValues[$tag]= "";
        }
      }
    }

    // Save base
    if (isset($_POST['BASE']) && $this->baseMode == "true") {
      $base= validate($_POST['BASE']);
      if (isset($this->bases[$base])) {
        $this->base= $base;
      }
    }

    // Save scope if needed
    if ($this->scopeMode == "auto") {
      $this->scope= isset($_POST['SCOPE'])?"sub":"one";
    }
  }

}

?>
