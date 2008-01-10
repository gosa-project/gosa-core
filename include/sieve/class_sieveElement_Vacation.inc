<?php

class sieve_vacation 
{
  var $days     = FALSE;
  var $subject  = FALSE;
  var $from     = "";
  var $mime     = "";
  var $handle   = "";
  var $reason   = "\"I am not available, currently.\"";
  var $addresses= array();
  var $object_id= -1;
  var $Expert   = FALSE;
  var $parent   = NULL;

  function sieve_vacation($data,$object_id,$parent)
  {
    $this->parent = $parent;
    $this->object_id = $object_id;
    $this->parent->add_require("vacation");

    /* Usage:   vacation [":days" number] [":subject" string]
       [":from" string] [":addresses" string-list]
       [":mime"] [":handle" string] <reason: string> */

    /* Not all attribute types are supported by the sieve class right now */
    $known_attrs = array(":days",":subject",":from",":mime",":handle");

    /* skip if empty */
    if(($data === NULL) || !is_array($data)) return;

    /* Walk through elements */
    $p= count($data['ELEMENTS']);
    for ($i= 0; $i < $p; $i++){

      /* get current element */
      $node = $data['ELEMENTS'][$i];

      /* Check if tag is in the specified list of attributes */
      if($node['class'] == "tag" && in_array($node['text'],$known_attrs)){

        $var = preg_replace("/\:/","",$node['text']);
        $this->$var = $data['ELEMENTS'][$i+1]['text'];
        $i ++;
      }

      /* Check for addresses */
      if($node['class'] == "tag" && $node['text'] == ":addresses") {
        $this->addresses = array();
        $i ++;

        /* Multiple or single address given */
        if($data['ELEMENTS'][$i]['class'] == "left-bracket"){
          while($data['ELEMENTS'][$i]['class'] != "right-bracket" && ($i < count($data['ELEMENTS']))){
            $i ++;
            if($data['ELEMENTS'][$i]['class'] == "quoted-string"){
              $this->addresses[] = preg_replace("/\"/i","",$data['ELEMENTS'][$i]['text']);
            }
          }
        }else{
	       $this->addresses[] =  preg_replace("/\"/i","",$data['ELEMENTS'][$i]['text']);
        }
      }

      /* Add the vacation message */
      if(in_array($node['class'],array("quoted-string","multi-line"))){

        $tmp = sieve_get_strings($data['ELEMENTS'],$i);
        $strs= $tmp['STRINGS'];
 
        $data = ""; 
        foreach($strs as $str){
          $data .= $str;
        }
        $this->reason = $data;
      }
    }
  }

  function get_sieve_script_part()
  {
    $str = "vacation ";
    if($this->days){
      $str.= ":days ".$this->days;
    }

    if(count($this->addresses)){
      $str .= ":addresses ".sieve_create_strings($this->addresses);
      if($this->subject){
        $str.= ":subject ".sieve_create_strings($this->subject);
      }
    }
    if($this->mime){
      $str.= ":mime ".sieve_create_strings($this->mime);
    }
  
    /* Append reason and ensure that this will be 
     *  handled as multiline text element 
     *  by adding a "\n" new line 
     */
    $str .= "\n ".sieve_create_strings($this->reason."\n");
    return($str." ; \n");
  }

  function save_object()
  {
    /* Get release date */
    if(isset($_POST['vacation_release_'.$this->object_id])){
      $this->days = stripslashes($_POST['vacation_release_'.$this->object_id]);
    }

    /* Check if we want to toggle the expert mode */
    if(isset($_POST['Toggle_Expert_'.$this->object_id])){
      $this->Expert = !$this->Expert;
    }

    /* Get release date */
    if(isset($_POST['vacation_receiver_'.$this->object_id])){
      $vr = stripslashes ($_POST['vacation_receiver_'.$this->object_id]);
      $tmp = array();
      $tmp2 = split(",",$vr);
      foreach($tmp2 as $val){
        $ad = trim($val);
        if(!empty($ad)){
          $tmp[] = $ad;
        }
      }
      $this->addresses = $tmp;
    }

    /* Get reason */
    if(isset($_POST['vacation_reason_'.$this->object_id])){
      $vr = stripslashes ($_POST['vacation_reason_'.$this->object_id]);
      $this->reason = trim($vr);
    }
  }

  function check()
  {
    $msgs = array();
    $err = FALSE;
    foreach($this->addresses as $addr){
      if(!tests::is_email($addr)){
        $err = true;
      }
    }
    if($err){
      $msgs[] = _("Alternative sender address must be a valid email addresses.");
    }
    return($msgs);
  }

  function execute()
  {
    $Addresses = "";
    foreach($this->addresses as $key){
      $Addresses .= $key.", ";
    }
    $Addresses = preg_replace("/,$/","",trim($Addresses));

    $smarty = get_smarty();
    $smarty->assign("LastError",$this->check());
    $smarty->assign("LastErrorCnt",count($this->check()));
    $smarty->assign("Reason",$this->reason);
    $smarty->assign("Addresses",$Addresses);
    $smarty->assign("Subject",$this->subject);
    $smarty->assign("Days",$this->days);
    $smarty->assign("ID",$this->object_id);
    $smarty->assign("Expert",$this->Expert);

    $object_container = $smarty->fetch(get_template_path("templates/object_container.tpl",TRUE,dirname(__FILE__)));
    $object= $smarty->fetch(get_template_path("templates/element_vacation.tpl",TRUE,dirname(__FILE__)));
    $str = preg_replace("/%%OBJECT_CONTENT%%/",addcslashes($object,"\\"),$object_container);
    return($str);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
