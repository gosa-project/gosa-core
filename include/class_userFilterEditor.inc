<?php

class userFilterEditor extends plugin 
{
  public $pathTitle= "Edit";

  // The original entry else array(), allows us to perform existence checks.
  public $entry = array();

  // The values
  public $name = "";
  public $description = "";
  public $parent = "";
  public $selectedCategories = array();
  public $share = FALSE;
  public $enabled = TRUE;
  public $queries = array();

  // The list of all categories mangaged by the current filter object.
  // Used in the grop-down box.
  public $availableCategories = array();
  public $fixedFilters = array();
  public $orig_name = "";


  /*! \brief    Instantiate the filter editing dialog. 
   *            Parses the filter info into editable data.
   */
  function __construct($entry, $categories, $fixedFilters)
  {
    $this->availableCategories = $categories;
    $this->fixedFilters = $fixedFilters;
    if($entry){
      $this->entry = $entry;
      $this->parent = $entry['parent'];
      $this->name = $entry['name'];
      $this->description = $entry['description'];
      
      foreach($entry['queries'] as $query){
        $query['filter'] = userFilterEditor::_autoIndentFilter($query['filter'], "  ");
        $this->queries[] = $query;
      }
      $this->selectedCategories = $entry['categories'];
      $this->share = in_array("share",$entry['flags']);
      $this->enable = in_array("enable",$entry['flags']);
    }
    $this->orig_name = $this->name;
  }


  /*! \brief    Automatic indent indentation for filters.
   */
  static function _autoIndentFilter($str, $indent = " ")
  {
    // Remove line breaks and escaped brackets 
    $str = preg_replace('/[\t ]*\n[\t ]*/', "", $str);
    $str = preg_replace('/\\\\\\(/', "::OPEN::", $str);
    $str = preg_replace('/\\\\\\)/', "::CLOSE::", $str);
   
    // Add a line break infront of every bracket 
    $str = preg_replace('/\\(/', "\n(", $str);
    $str = preg_replace('/\\)/', ")\n", $str);

    // Split by linebreaks
    $lines = preg_split("/\n/", $str);
    $str = "";
    $i = 0;
 
    // Walk trough search blocks 
    foreach($lines as $line){
      $line = trim($line);
      if(empty($line)) continue;

      // Go back one level in indentation  
      if(!preg_match("/\\(.*\\)/", $line) && preg_match('/\\)$/', $line)){
        $i --;
      }

      $str.= "\n";
      $str = str_pad($str,strlen($str)+$i, $indent); 
      $str.= $line;

      // Go one level deeper in indentation 
      if(!preg_match("/\\(.*\\)/", $line) && preg_match('/^\\(/', $line)){
        $i ++;
      }
    }
    $str = preg_replace('/::OPEN::/', '\(', $str);
    $str = preg_replace('/::CLOSE::/', '\)', $str);
    return($str);
  }


  /*! \brief    Retunrs the filters original name 
   *  @param    The original name of the filter (if none was given 
   *             an empty string is returned)
   */
  function getOriginalName()
  {
    return($this->orig_name);
  }


  /*! \brief    Retunrs the filters name.
   *  @param    The name of the filter
   */
  function getCurrentName()
  {
    return($this->name);
  }


  /*! \brief    Generates the <HTML> content, to edit the filter settings.
   *  @return   String  HTML form.
   */
  function execute()
  {
    plugin::execute();

    // Build up HTML compliant html output
    $queries = array();
    foreach($this->queries as $key => $query){
      $query['filter'] =  htmlentities($query['filter'],ENT_COMPAT,'UTF-8');
      $queries[$key] = $query; 
    }  

    $smarty = get_smarty();
    $smarty->assign('parent', $this->parent);
    $smarty->assign('name', htmlentities($this->name,ENT_COMPAT,'UTF-8'));
    $smarty->assign('queries', $queries);
    $smarty->assign('share', $this->share);
    $smarty->assign('enable', $this->enabled);
    $smarty->assign('description', htmlentities($this->description,ENT_COMPAT,'UTF-8'));
    $smarty->assign('selectedCategories', $this->selectedCategories);
    $smarty->assign('availableCategories', $this->availableCategories);
    $smarty->assign('fixedFilters', $this->fixedFilters);
    return($smarty->fetch(get_template_path('userFilterEditor.tpl', FALSE)));
  }


  /*! \brief    Keep values entered in the input form of the dialog. (POST/GET)
   */
  function save_object()
  {
    if(isset($_POST['userFilterEditor'])){

      // Get posted strings
      foreach(array('name','description', 'parent') as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }

      // Filter needs special handling, it may contain charactes like < and >
      //  wich are stipped out by get_post() && validate()
      foreach($this->queries as $key => $query){
        if(isset($_POST['filter_'.$key])){
          $f = mb_convert_encoding($_POST['filter_'.$key], 'UTF-8');
          if(get_magic_quotes_gpc()){
            $f = stripcslashes($f);
          }
          $this->queries[$key]['filter'] = $f;
          $this->queries[$key]['backend'] = get_post('backend_'.$key);
        }
      }
      
      foreach($this->queries as $key => $query){
        if(isset($_POST['removeQuery_'.$key])){
          unset($this->queries[$key]);
          $this->queries = array_values($this->queries);
        }   
      }

      // Get posted flags 
      $this->share = isset($_POST['shareFilter']);
      $this->enable = isset($_POST['enableFilter']);

      // Get additional category  
      if(isset($_POST['addCategory'])){
        if(isset($_POST['manualCategory']) && !empty($_POST['manualCategory'])){
          $this->selectedCategories[] = get_post('manualCategory');
        }elseif(isset($_POST['availableCategory']) && !empty($_POST['availableCategory'])){
          $this->selectedCategories[] = get_post('availableCategory');
        }
      }

      // Remove categories
      if(isset($_POST['delCategory']) && isset($_POST['usedCategory'])){
        foreach($_POST['usedCategory'] as $cat){
          if(isset($this->selectedCategories[$cat])) unset($this->selectedCategories[$cat]);
        }
      }

      // Add new query 
      if(isset($_POST['addQuery'])){
        $this->queries[] = array('backend'=>'filterLDAP', 'filter' => '(objectClass=*)');
      }
    }
  }

  
  /*! \brief    Validate user input 
   *  @return   Array   An Array containing potential error messages
   */
  function check()
  {
    $msgs = plugin::check();
  
    // Check if the name is given
    if(empty($this->name)){
      $msgs[] = msgPool::required(_("Name"));
    }elseif(preg_match("/[^a-z0-9]/i", $this->name)){
      
      // Check for a valid name, no special chars here - in particular no ; 
      $msgs[] = msgPool::invalid(_("Name"), $this->name,"/[a-z0-9]/i");
    }  

    // Description is a must value.
    if(empty($this->description)){
      $msgs[] = msgPool::required(_("Description"));
    }

    // Count the number of opening and closing brackets - exclude escaped ones.
    foreach($this->queries as $key => $query){
      $f = preg_replace('/\\\\[\(\)]/',"",$query['filter']);
      $o = substr_count($f, '(');
      $c = substr_count($f, ')');
      if($o != $c){
        $msgs[] = sprintf(_("Please check your filter #%s. You have '%s' opening and '%s' closing brackets!"), ($key+1),$o, $c);
      }
    }

    return($msgs);
  }


  /*! \brief    Transforms the entered values into a filter object (array) which is useable
   *             for the userFilter overview dialog.
   *  @return   Returns transformed filter data.
   */
  function save()
  {
    $ret= array();
    $ret['parent'] = $this->parent;
    $ret['name'] = $this->name;
    $ret['description'] = $this->description;
    $ret['categories'] = $this->selectedCategories;
    $ret['queries'] = $this->queries;
    $ret['flags'] = array();
    if($this->share){
      $ret['flags'][] = "share";
    }
    if($this->enable){
      $ret['flags'][] = "enable";
    }
    return($ret);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
