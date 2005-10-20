<?php

class faiHookEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("Object_cn","Object_description","Object_FAIscript","Object_FAItask");
  var $objectclasses= array();

  var $orig_cn              = "";
  var $tasks                = array("chboot", "configure", "debconf", "extrbase", "faiend", "finish",
                                    "install", "instsoft", "mirror", "mountdisks", "partition", "prepareapt",
                                    "savelog", "softupdate", "sysinfo"," updatebase");
  var $Object_dn            = "";
  var $Object_cn            = "";
  var $Object_FAItask       = "";
  var $Object_FAIscript     = "";
  var $Object_description   = "";
  var $Object_status        = "new";
  
  function faiHookEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = "Object_".$name;
        $this->$oname=$value;
      }
    }else{
      $this->Object_status = "new";
      $this->orig_cn       = false;
    }
  }


  function execute()
  {
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
        $str = utf8_encode(file_get_contents($_FILES['ImportFile']['tmp_name']));
        $this->Object_FAIscript = $str;
      }
    }
    $smarty->assign("tasks", $this->tasks);

     /* Magic quotes GPC, escapes every ' " \, to solve some security risks
     * If we post the escaped strings they will be escaped again
     */
    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
        $smarty->assign($attrs,stripslashes($this->$attrs));
      }else{
        $smarty->assign($attrs,($this->$attrs));
      }
    }

    $display.= $smarty->fetch(get_template_path('faiHookEntry.tpl', TRUE));
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
    }
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
    if(empty($this->Object_FAIscript)) {
      $message[]=_("Please enter a value for script.");
    }

    if(empty($this->Object_cn)){
      $message[] = _("Please enter a name.");
    }

    if(preg_match("/[^0-9a-z]/i",$this->Object_cn)){
      $message[] = _("Please enter a valid name. Only a-Z 0-9 are allowed.");
    }
  
    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $attr = preg_replace("/^Object_/","",$attrs);
      $tmp[$attr] = $this->$attrs;
    }

    if(($this->orig_cn)&&($tmp['cn']!=$this->orig_cn)){
      $tmp['remove']['from']  = $this->orig_cn;
      $tmp['remove']['to']    = $tmp['cn'];
    }
  
    $tmp['dn']      = $this->dn;  
    $tmp['status']  = $this->Object_status;  
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
