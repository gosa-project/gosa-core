<?php

class goto_task extends plugin
{
  /* Definitions */
  var $plHeadline     = "System mass deployment";
  var $plDescription  = "This does something";
 
  var $parent   = NULL;
  var $data     = array();

  var $attributes = array("MACADDRESS","ID","STATUS","HEADERTAG");

  function goto_task(&$config, &$parent, $data = array())
  {
    /* Set parent object */
    $this->parent = &$parent;
    $this->data = $data;

    /* Intialize plugin */
    $this->config = &$config;
  }


  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("Actions",$this->parent->get_actions());
    foreach($this->attributes as $attr){
      if(isset($this->data[$attr])){
        $smarty->assign($attr,$this->data[$attr]);
      }else{
        $smarty->assign($attr,"");
      }
    }
    return ($smarty->fetch (get_template_path('goto_task.tpl', TRUE)));
  }


  function save_object()
  {
    if(isset($_POST['HeaderTag'])){
      $possible_tags = $this->parent->get_actions();
      $tag = $_POST['HeaderTag'];
      if(isset($possible_tags[$tag])){
        $this->data['HEADERTAG'] = $tag;
      }
    }
    foreach(array("status","macaddress") as $attr){
      if(isset($_POST[$attr])){
        $this->data[strtoupper($attr)] = $_POST[$attr];
      }
    }
  }

  function save()
  {
    $tmp = array();   
    foreach(array("STATUS","MACADDRESS","HEADERTAG") as $attr){
      if(isset($this->data[$attr])){
        $tmp[$attr] = $this->data[$attr];
      }
    }
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
