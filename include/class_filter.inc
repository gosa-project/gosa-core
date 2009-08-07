<?php

class filter {

  var $xmlData;
  var $elements= array();
  var $elementValues= array();
  var $alphabetElements= array();
  var $autocompleter= array();
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


  function filter($filename)
  {
    if (!$this->load($filename)) {
      die("Cannot parse $filename!");
    }
  }


  function load($filename)
  {
    $contents = file_get_contents($filename);
    $this->xmlData= xml::xml2array($contents, 1);

    if (!isset($this->xmlData['filter'])) {
      return false;
    }

    $this->xmlData= $this->xmlData["filter"];

    // Load filter
    if (isset($this->xmlData['search'])) {
      if (!isset($this->xmlData['search']['query'][0])){
        $this->xmlData['search']['query']= array($this->xmlData['search']['query']);
      }

      // Move information
      $entry= $this->xmlData['search'];
      $this->baseMode= $entry['base'];
      $this->scopeMode= $entry['scope'];
      $this->query= $entry['query'];
    } else {
      return false;
    }

    // Generate formular data
    if (isset($this->xmlData['element'])) {
      if (!isset($this->xmlData['element'][0])){
        $this->xmlData['element']= array($this->xmlData['element']);
      }
      foreach ($this->xmlData['element'] as $element) {

        // Ignore elements without type
        if (!isset($element['type']) || !isset($element['tag'])) {
          next;
        }

        $tag= $element['tag'];

        // Fix arrays
        if (isset($element['value']) && !isset($element['value'][0])) {
          $element['value']= array($element['value']);
        }

        // Store element for quick access
        $this->elements[$tag] = $element;

        // Preset elementValues with default values if exist
        if (isset($element['default']) && !is_array($element['default'])) {
          $this->elementValues[$tag] = $element['default'];
        } else {
          $this->elementValues[$tag] = "";
        }

        // Does this element react on alphabet links?
        if (isset($element['alphabet']) && $element['alphabet'] == "true") {
          $this->alphabetElements[]= $tag;
        }
      }
    }

    return true;  
  }


  function getTextfield($element)
  {
    $tag= $element['tag'];
    $result= "<input class='filter_textfield' id='$tag' name='$tag' type='text' size='30' maxlength='30' value='".$this->elementValues[$tag]."'>";
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


  function getCheckbox($element)
  {
    $tag= $element['tag'];
    $checked= "";
    if ($this->elementValues[$tag] == "true") {
      $checked= " checked";
    }

    $result= "<input class='filter_checkbox' name='$tag' type='checkbox' onClick='document.mainform.submit();' value='true'$checked>";
    return $result;
  }


  function getCombobox($element)
  {
    $result= "<select name='".$element['tag']."' size='1' onClick='document.mainform.submit();'>";
    foreach ($element['value'] as $value) {
      $selected= "";
      if ($this->elementValues[$element['tag']] == $value['key']) {
        $selected= " selected";
      }
      $result.= "<option value='".$value['key']."'$selected>{t}".$value['set']."{/t}</option>";
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
      switch ($element['type']) {
        case "textfield":
          $htmlCode = $this->getTextfield($element);
          break;

        case "checkbox":
          $htmlCode = $this->getCheckbox($element);
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
    return ("<input type='hidden' name='FILTER_LOADED' value='1'>".$smarty->fetch(get_template_path("filter/".$this->xmlData['definition']['template'])));
  }


  function query()
  {
    global $class_mapping;
    $result= array();

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

      // Generate final filter
      foreach ($this->elements as $tag => $element) {
        if (!isset($element['set']) || !isset($element['unset'])) {
          next;
        }
        $e_set= is_array($element['set'])?"":$element['set'];
        $e_unset= is_array($element['unset'])?"":$element['unset'];

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


  function processAutocomplete()
  {
    global $class_mapping;
    $result= array();

    foreach ($this->autocompleters as $tag => $config) {
      if(isset($_POST[$tag])){
      
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
        $filter= preg_replace("/\\$$tag/", normalizeLDAP($_POST[$tag]), $filter);
        $result= call_user_func(array($backend, 'query'), $this->base, $this->scope, $filter, $attributes,
                             $this->category, $this->objectStorage, $this->objectBase);

        echo '<ul>';
        foreach ($result as $entry) {
          foreach ($attributes as $attribute) {
            if (is_array($entry[$attribute])) {
              for ($i= 0; $i<$entry[$attribute]['count']; $i++) {
                echo '<li>'.$entry[$attribute][$i].'</li>';
              }
            } else {
              echo '<li>'.$entry[$entry][$attribute].'</li>';
            }
          }
        }

        echo '</ul>';
      }

    }
  }

}

?>
