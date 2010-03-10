<?php

class filterEditEntry extends plugin 
{
  // The original entry else array(), allows us to perform existence checks.
  public $entry = array();

  // The values
  public $name = "";
  public $description = "";
  public $selectedCategories = array();
  public $share = FALSE;
  public $filter = "(objectClass=*)";

  // The list of all categories mangaged by the current filter object.
  // Used in the grop-down box.
  public $availableCategories = array();
  public $orig_name = "";


  /*! \brief    Instantiate the filter editing dialog. 
   *            Parses the filter info into editable data.
   */
  function __construct($entry, $categories)
  {
    $this->availableCategories = $categories;
    if($entry){
      $this->entry = $entry;
      $this->name = $entry['name'];
      $this->description = $entry['description'];
      $this->filter = $entry['filter'];
      $this->selectedCategories = $entry['categories'];
      $this->share = in_array("share",$entry['flags']);
    }
    $this->orig_name = $this->name;
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
    $smarty = get_smarty();
    $smarty->assign('name', $this->name);
    $smarty->assign('filter', $this->filter);
    $smarty->assign('share', $this->share);
    $smarty->assign('description', $this->description);
    $smarty->assign('selectedCategories', $this->selectedCategories);
    $smarty->assign('availableCategories', $this->availableCategories);
    return($smarty->fetch(get_template_path('filterEditorEntry.tpl', FALSE)));
  }


  /*! \brief    Keep values entered in the input form of the dialog. (POST/GET)
   */
  function save_object()
  {
    if(isset($_POST['filterEditorEntry'])){

      // Get posted strings
      foreach(array('name','description','filter') as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }

      // Get posted flags 
      $this->share = isset($_POST['shareFilter']);

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
    }elseif(preg_match("/[^a-z0-9\-_ ]/i", $this->name)){
      
      // Check for a valid name, no special chars here - in particular no ; 
      $msgs[] = msgPool::invalid(_("Name"), $this->name,"/[a-z0-9\-_ ]/i");
    }  

    return($msgs);
  }


  /*! \brief    Transforms the entered values into a filter object (array) which is useable
   *             for the filterEditor overview dialog.
   *  @return   Returns transformed filter data.
   */
  function save()
  {
    $ret= array();
    $ret['name'] = $this->name;
    $ret['description'] = $this->description;
    $ret['categories'] = $this->selectedCategories;
    $ret['filter'] = $this->filter;
    $ret['flags'] = array();
    if($this->share){
      $ret['flags'][] = "share";
    }
    return($ret);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
