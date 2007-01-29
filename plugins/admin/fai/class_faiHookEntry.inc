<?php

class faiHookEntry extends plugin
{
  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description","FAIscript","FAItask");
  var $objectclasses= array();

  var $orig_cn              = "";
  var $tasks                = array("chboot", "configure", "debconf", "extrbase", "faiend", "finish",
                                    "install", "instsoft", "mirror", "mountdisks", "partition", "prepareapt",
                                    "savelog", "softupdate", "sysinfo","updatebase");
  var $dn            = "";
  var $cn            = "";
  var $FAItask       = "";
  var $FAIscript     = "";
  var $description   = "";
  var $status        = "new";

  var $FAIstate      = "";
  
  function faiHookEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = $name;
        $this->$oname=$value;
      }

      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }

    }elseif(is_array($object)){
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
        $str = utf8_encode(file_get_contents($_FILES['ImportFile']['tmp_name']));
        $this->FAIscript = $str;
      }
    }

    /* Create download button*/
    if($this->dn != "new"){
      $smarty->assign("DownMe","<a href='getFAIscript.php?id=".base64_encode($this->dn)."'>
        <input type='button' value='"._("Download")."'>
        </a>");
    }else{
      $smarty->assign("DownMe","");  
    }

    $used_tasks = $this->parent->getUsedFAItask($this->cn);
    $tasks = $this->tasks;
    foreach($this->tasks as $id => $task){
      if(in_array($task,$used_tasks)){
        unset($tasks[$id]);
      }
    }
    $smarty->assign("tasks", $tasks);

     /* Magic quotes GPC, escapes every ' " \, to solve some security risks
     * If we post the escaped strings they will be escaped again
     */
    foreach($this->attributes as $attrs){
      if(get_magic_quotes_gpc()){
        $smarty->assign($attrs,stripslashes($this->$attrs));
      }else{
        $smarty->assign($attrs,$this->$attrs);
      }
    }

    foreach($this->attributes as $attr){
      if(($this->FAIstate == "freeze") || (chkacl($this->acl,$attr)!= "")){
        $smarty->assign($attr."ACL"," disabled ");
      }else{
        $smarty->assign($attr."ACL","  ");
      }
    }

    $display.= $smarty->fetch(get_template_path('faiHookEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if((isset($_POST['SubObjectFormSubmitted'])) && ($this->FAIstate != "freeze")){
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
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(empty($this->FAIscript)) {
      $message[]=_("Please enter a value for script.");
    }

    if(empty($this->cn)){
      $message[] = _("Please enter a name.");
    }

    return ($message);
  }
 
  function save()
  {
    $tmp=array();
    foreach($this->attributes as $attrs){ 
      $tmp[$attrs] = $this->$attrs;
    }

    /* Strip out dos newlines */
    $tmp['FAIscript']= strtr($this->FAIscript, array("\x0D" => ""));

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
