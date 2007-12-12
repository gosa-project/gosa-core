<?php

class sieve_fileinto 
{
  var $data     = "";
  var $object_id= -1;
  var $options  = array();
  var $parent   = NULL;
  var $user_mode= FALSE;

  function save_object()
  {
    $mbs = $this->get_mail_boxes();
    
    if(isset($_POST['fileinto_'.$this->object_id])){
      $mb = stripslashes($_POST['fileinto_'.$this->object_id]);

      /* Depending on the user mode we only accept 
       *  existing mailboxes 
       */
      if($this->user_mode){
        $this->data = $mb;
      }else{
        if(in_array_ics($mb,$mbs)){
          $this->data = $mb; 
        }
      }

      /* Check Mode */
      if(isset($_POST['user_mode_'.$this->object_id])){
        $this->user_mode = !$this->user_mode;
      }
    }
  }

  function sieve_fileinto($data,$object_id,$parent)
  {
    $this->object_id = $object_id;
    $this->parent = $parent;
    $this->parent->add_require("fileinto");

    $mbs = $this->get_mail_boxes();
      
    /* Set the default mailbox */
    if($data === NULL){
      $data = array('ELEMENTS' => array(array('class' => "quoted-string" ,"text" => $mbs[key($mbs)])));
    }

    /* Load element contents, should normaly be only one string 
     *  but if we found more than one, just append the following strings.
     */
    for($i = 0 ; $i < count($data['ELEMENTS']) ; $i++){
      $tmp = sieve_get_strings($data['ELEMENTS'],$i);
      $i  = $i + $tmp['OFFSET'];
      foreach($tmp['STRINGS'] as $str){
        $this->data .= $str;
      }
    }

    /* Set user mode to active, so we are able to insert 
     *  the destination mail folder manually 
     */
    if(!in_array_ics($this->data,$mbs)){
      $this->user_mode = TRUE;
    }
  }

  function get_sieve_script_part()
  {
    $tmp = "";
    $tmp.= "\"".$this->data."\", ";
    $tmp = preg_replace("/,$/","",trim($tmp));
    $tmp = preg_replace ("/\"\"/","\"",$tmp);
    return("fileinto ".$tmp.";");
  } 
    
  function execute()
  {
    $smarty = get_smarty();
    $smarty->assign("Selected",htmlentities($this->data));
    $smarty->assign("Boxes", $this->get_mail_boxes());
    $smarty->assign("User_Mode", $this->user_mode);
    $smarty->assign("ID", $this->object_id);
    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_fileinto.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);

    return($str);
  }

  function check()
  {
    return(array());
  }

  function get_mail_boxes()
  {
    $list  = $this->parent->parent->parent->parent->mailboxList;
    return($list);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
