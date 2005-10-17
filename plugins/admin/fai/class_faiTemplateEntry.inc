<?php

class faiTemplateEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("Object_cn","Object_description","Object_FAItemplateFile","Object_FAItemplatePath","Object_FAImode","Object_user","Object_group","Object_binary","Object_FAIowner");
  var $objectclasses= array();

  var $orig_cn              = "";

  var $Object_dn            = "";
  var $Object_cn            = "";
  var $Object_FAItemplateFile   = "";
  var $Object_FAItemplatePath   = "";
  var $Object_description   = "";
  var $Object_status        = "new";
  var $Object_FAImode       = "0640";
  var $Object_FAIowner      = "root.root";
  var $Object_user          = "root";
  var $Object_group         = "root";
  var $Object_binary        = false;
  
  function faiTemplateEntry ($config, $dn= NULL,$object=false)
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
    $this->Object_user = explode( '.', $this->Object_FAIowner );
    $this->Object_group = $this->Object_user[1];
    $this->Object_user = $this->Object_user[0];
    $_SESSION['binary'] = $this->Object_FAItemplateFile;
    $_SESSION['binarytype'] = 'octet-stream';
    $_SESSION['binaryfile'] = basename( $this->Object_FAItemplatePath );
    
    $this->Object_FAImode= sprintf("%0.4s", $this->Object_FAImode)." ";
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $smarty->assign("rand", rand(0, 10000));
    $display = "";

    if(isset($_POST['TmpFileUpload'])){
      if($str=file_get_contents($_FILES['FAItemplateFile']['tmp_name'])){
        $this->Object_FAItemplateFile = $str;

        /* If we don't have a filename set it from upload filename. */
        if( 0 == strlen( $this->Object_FAItemplatePath ) )
          $this->Object_FAItemplatePath = $_FILES['FAItemplateFile']['name'];
      }
    
    }
    
    $status= _("no file uploaded yet");
    if(strlen($this->Object_FAItemplateFile)){
      $status= sprintf(_("exists in database (size: %s bytes)"),strlen($this->Object_FAItemplateFile));
    }
    $smarty->assign("status",$status);

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

    /* Assign file modes */
    $tmode= "$this->Object_FAImode ";
    foreach (array("s", "u", "g", "o") as $type){
      $current= substr($tmode, 0, 1);
      $tmode=   preg_replace("/^./", "", $tmode);
      $nr= 1;
      while ($nr < 5){
        if ($current & $nr){
          $smarty->assign($type.$nr, "checked");
        } else {
          $smarty->assign($type.$nr, "");
        }
        $nr+= $nr;
      }
    }

    $smarty->assign("Object_FAItemplateFile","");

    $display.= $smarty->fetch(get_template_path('faiTemplateEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if (!isset($_POST['Object_FAItemplatePath'])){
      return;
    }
    if(isset($_POST['SubObjectFormSubmitted'])){
      foreach($this->attributes as $attrs){
        if($attrs == "Object_FAItemplateFile") 
          continue;
        if($attrs == "Object_FAIowner") {
          $this->$attrs = $_POST["Object_user"] . '.' . $_POST["Object_group"];
          continue;
        }
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = "";
        }
      }
    }

    /* Save mode */
    $tmode= "";
    foreach (array("s", "u", "g", "o") as $type){
      $nr= 1;
      $dest= 0;
      while ($nr < 5){
        if (isset($_POST["$type$nr"])){
          $dest+= $nr;
        }
        $nr+= $nr;
      }
      $tmode= $tmode.$dest;
    }
    $this->Object_FAImode= $tmode;
  }

  /* Check supplied data */
  function check()
  {
    $message= array();
    if(empty($this->Object_FAItemplateFile)){
      $message[]=_("Please specify a value for attribute 'file'.");
    } 

    if(empty($this->Object_FAItemplatePath)){
      $message[]=_("Please specify a value for attribute 'path'.");
    } 
  
    if(empty($this->Object_cn)){
      $message[] = _("Please enter a name.");
    }

    if(empty($this->Object_user)){
      $message[] = _("Please enter a user.");
    }
    elseif(preg_match("/[^0-9a-z]/i",$this->Object_user)){
      $message[] = _("Please enter a valid user. Only a-Z 0-9 are allowed.");
    }

    if(empty($this->Object_group)){
      $message[] = _("Please enter a group.");
    }
    elseif(preg_match("/[^0-9a-z]/i",$this->Object_group)){
      $message[] = _("Please enter a valid group. Only a-Z 0-9 are allowed.");
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
