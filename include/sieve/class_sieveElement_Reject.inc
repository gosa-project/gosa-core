<?php

class sieve_reject 
{
  var $data = "";
  var $object_id = -1;
  var $parent = NULL;

  function save_object()
  {
    if(isset($_POST['reject_message_'.$this->object_id])){
      $msg = stripslashes($_POST['reject_message_'.$this->object_id]);
      $this->data = $msg;
    }
  }

  function check()
  {
    $msgs = array();
    if(preg_match("/\"/",$this->data)){
      $msgs [] = _("Invalid character found, quotes are not allowed in a reject message.");
    }
    return($msgs);
  }

  function sieve_reject($data,$object_id,$parent)
  {
    $this->object_id = $object_id;
    $this->parent = $parent;
    $this->parent->add_require("reject");

    /* If the given data is emtpy 
     *  (This is the case when we add new elements in the ui) 
     * Set a default text. 
     */
    if($data === NULL){
      $this->data = _("Your reject text here");
    }else{

      for($i = 0 ; $i < count($data['ELEMENTS']) ; $i++){
        $tmp = sieve_get_strings($data['ELEMENTS'],$i);
        $i  = $i + $tmp['OFFSET'];
        foreach($tmp['STRINGS'] as $str){
          $this->data .= $str;
        }
      }
    }
  }

  function get_sieve_script_part()
  {
    return("reject ".sieve_create_strings($this->data).";");
  } 

  function execute()
  {
    /* check if this will be a 
     *   - single string ""
     *   - or a multi line text: ... ; 
     */
    $Multiline = preg_match("/\n/",$this->data);

    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $smarty->assign("Message",$this->data);
    $smarty->assign("Multiline",$Multiline);
    $smarty->assign("LastError" , $this->check());
    $smarty->assign("LastErrorCnt" , count($this->check()));
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_reject.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
