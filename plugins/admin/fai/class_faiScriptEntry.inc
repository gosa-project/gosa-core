<?php

class faiScriptEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description","FAIpriority","FAIscript");
  var $objectclasses= array();

  var $orig_cn              = "";

  var $dn            = "";
  var $cn            = "";
  var $FAIpriority   = "0";
  var $FAIscript     = "";
  var $description   = "";
  var $status        = "new";

  
  function faiScriptEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = $name;
        $this->$oname=$value;
      }
    }else{
      if(count($object)){
        $this->orig_cn= $object['cn'];
        $this->dn=$object['dn'];
        foreach($object as $name=>$value){
          $oname = $name;
          $this->$oname=$value;
        }
      }else{

        $this->status = "new";
        $this->orig_cn       = false;
      }
    }
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";
  
    if(isset($_POST['ImportUpload'])){
      if(($_FILES['ImportFile']['error']!=0)){
        print_red(_("Please select a valid file."));
      }else
      if(($_FILES['ImportFile']['size']==0)){
        print_red(_("Selected file is empty."));
      }else{
        $str = utf8_encode(addslashes(file_get_contents($_FILES['ImportFile']['tmp_name'])));
        $this->FAIscript = $str;
      }
    }
  
    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
       $smarty->assign($attrs,stripslashes($this->$attrs));
      }else{
       $smarty->assign($attrs,($this->$attrs));
      } 
    }

    for($i =0 ; $i < 100 ; $i++){
      $FAIprioritys[$i]=$i;
    }
    $smarty->assign("FAIprioritys",$FAIprioritys);
    $display.= $smarty->fetch(get_template_path('faiScriptEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if(isset($_POST['SubObjectFormSubmitted'])){
      foreach($this->attributes as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = "";
        }
      }
      $this->FAIscript= recode("DOS..LATIN1", $this->FAIscript);
    }
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
  
    if(empty($this->cn)){
      $message[] = _("Please enter a name.");
    }

    if(empty($this->FAIscript)){
      $message[]=_("Please enter a script.");
    }

    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $tmp[$attrs] = $this->$attrs;
    }

    if(($this->orig_cn)&&($tmp['cn']!=$this->orig_cn)){
      $tmp['remove']['from']  = $this->orig_cn;
      $tmp['remove']['to']    = $tmp['cn'];
    }
  
    $tmp['dn']      = $this->dn;  
    $tmp['status']  = $this->status; 
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
