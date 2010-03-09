<?php

class filterEditEntry extends plugin 
{
  public $entry = array();
  public $name = "";
  public $description = "";
  public $selectedCategories = array();
  public $share = FALSE;
  public $filter = "(objectClass=*)";

  public $availableCategories = array();
  public $orig_name = "";

  function __construct($entry, $categories)
  {
    $this->availableCategories = $categories;
    if($entry){
      $this->entry = $entry;
      $this->name = $entry['name'];
      $this->description = $entry['description'];
      $this->selectedCategories = $entry['categories'];
      $this->filter = $entry['filter'];
      $this->share = in_array("share",$entry['flags']);
    }

    $this->orig_name = $this->name;
  }

  function getOriginalName()
  {
    return($this->orig_name);
  }

  function getCurrentName()
  {
    return($this->name);
  }

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
