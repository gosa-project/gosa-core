<?php

/* Sieve comment tag */
class sieve_comment 
{
  var $data = "";
  var $object_id= -1;
  var $small  = TRUE;

  function get_sieve_script_part()
  {
    $str = "\n/*".$this->data."*/";
    return($str);
  } 
    
  function sieve_comment($data,$object_id)
  {
    $this->object_id = $object_id;

    if($data === NULL){
      $data = array('ELEMENTS' => array(array('class' => "quoted-string" ,"text" => "/*"._("Your comment here")."*/")));
    }

    foreach($data['ELEMENTS'] as $node){

      $text = $node['text'];
  
      /* Convert \t to spaces  */ 
      $text = preg_replace("#\t#","    ",$text);
      
      /* Remove comment indicator '#' but keep spaces */
      $text = preg_replace("/^([ ]*)\#/","\\1",$text);
     
      /* Remove comment indicator '/ *' */ 
      $text = preg_replace("#\/\*#","",$text);
      
      /* Remove comment indicator '* /' */ 
      $text = preg_replace("#\*\/#","",$text);

      $this->data .= $text."\n";
    }
    $this->data = rtrim($this->data)."\n";
  }

  function check()
  {
    return(array())  ;
  }

  function save_object()
  {
    if(isset($_POST['comment_'.$this->object_id])){
      $cm = stripslashes( $_POST['comment_'.$this->object_id]);
      $cm = preg_replace("/\*\//","* /",$cm);
      $this->data = $cm;
    }
  
    if(isset($_POST['toggle_small_'.$this->object_id])){
      $this->small = !$this->small;
    }
  }

  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $smarty->assign("Small", $this->small);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $Comment = $this->data;

    if($this->small){
      $Comment = nl2br(preg_replace("/ /","&nbsp;",$Comment));
    }
 
    /* Create html object */ 
    $smarty->assign("Comment",$Comment);
    $smarty->assign("ID",$this->object_id);
    $object = $smarty->fetch(get_template_path("templates/element_comment.tpl",TRUE,dirname(__FILE__)));    
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
