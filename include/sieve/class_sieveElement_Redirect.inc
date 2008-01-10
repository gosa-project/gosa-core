<?php
class sieve_redirect 
{
  var $data = "";
  var $object_id = -1;
  var $LastError   ="";
  
  function save_object()
  {
    if(isset($_POST['redirect_to_'.$this->object_id])){
      $rt = stripslashes($_POST['redirect_to_'.$this->object_id]);

      $rt = trim($rt);
      $this->data = $rt;
    }
  }


  function check()
  {
    $msgs = array();
    
    if(!tests::is_email($this->data)){
      $msgs[] =_("Please specify a valid email address.");
    }
    
    return($msgs);
  }

  function sieve_redirect($data,$object_id)
  {
    $this->object_id = $object_id;

    if($data === NULL){
      $data = array('ELEMENTS' => array(array('class' => "quoted-string" ,"text" => _("Place a mail address here"))));
    }

    for($i = 0 ; $i < count($data['ELEMENTS']) ; $i++){
      $tmp = sieve_get_strings($data['ELEMENTS'],$i);
      $i  = $i + $tmp['OFFSET'];
      foreach($tmp['STRINGS'] as $str){
        $this->data .= $str;
      }
    }
  }


  function get_sieve_script_part()
  {
    return("redirect ".sieve_create_strings($this->data).";");
  } 
   
 
  function execute()
  {
    $values = htmlentities($this->data);
    $smarty = get_smarty();
    $smarty->assign("ID", $this->object_id);
    $smarty->assign("Destinations" , $values);
    $smarty->assign("LastError" , $this->check());
    $smarty->assign("LastErrorCnt" , count($this->check()));
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_redirect.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
