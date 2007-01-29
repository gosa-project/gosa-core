<?php

class faiTemplateEntry extends plugin
{
  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description","FAItemplateFile","FAItemplatePath","FAImode","user","group","binary","FAIowner");
  var $objectclasses= array();

  var $orig_cn              = "";

  var $dn            = "";
  var $cn            = "";
  var $FAItemplateFile   = "";
  var $FAItemplatePath   = "";
  var $description   = "";
  var $status        = "new";
  var $FAImode       = "0640";
  var $FAIowner      = "root.root";
  var $user          = "root";
  var $group         = "root";
  var $binary        = false;

  var $FAIstate      = "";
  
  function faiTemplateEntry ($config, $dn= NULL,$object=false)
  {
    plugin::plugin ($config, $dn);

    if((isset($object['cn'])) && (!empty($object['cn']))){
      $this->orig_cn= $object['cn'];
      $this->dn=$object['dn'];
      foreach($object as $name=>$value){
        $oname = $name;
        $this->$oname=$value;
      }

      if(isset($this->attrs['FAIstate'][0])){
        $this->FAIstate = $this->attrs['FAIstate'][0];
      }

    }else{
      $this->status = "new";
      $this->orig_cn= false;
    }

    $this->user = explode( '.', $this->FAIowner );
    $this->group = $this->user[1];
    $this->user = $this->user[0];
    $_SESSION['binary'] = stripslashes($this->FAItemplateFile);
    $_SESSION['binarytype'] = 'octet-stream';
    $_SESSION['binaryfile'] = basename( $this->FAItemplatePath );
    
    $this->FAImode= sprintf("%0.4s", $this->FAImode)." ";
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $smarty->assign("rand", rand(0, 10000));
    $display = "";

    if(isset($_POST['TmpFileUpload'])){
      if($str=file_get_contents($_FILES['FAItemplateFile']['tmp_name'])){
        $this->FAItemplateFile = addslashes($str);

        /* If we don't have a filename set it from upload filename. */
        if( 0 == strlen( $this->FAItemplatePath )){
          $this->FAItemplatePath = $_FILES['FAItemplateFile']['name'];
        }

        $_SESSION['binary']     = stripslashes($this->FAItemplateFile);
        $_SESSION['binarytype'] = 'octet-stream';
        $_SESSION['binaryfile'] = basename( $this->FAItemplatePath );
      }
    }
    
    $status= _("no file uploaded yet");

    $bStatus = false; // Hide download icon on default 
    
    if(strlen($this->FAItemplateFile)){
      $status= sprintf(_("exists in database (size: %s bytes)"),strlen( stripslashes( $this->FAItemplateFile)));
      $bStatus = true;  // Display download icon 
    }
    $smarty->assign("status",$status);
    $smarty->assign("bStatus",$bStatus);

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
    $tmode= "$this->FAImode ";
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

    $smarty->assign("FAItemplateFile","");

    foreach($this->attributes as $attr){
      if(($this->FAIstate == "freeze") || (chkacl($this->acl,$attr)!= "")){
        $smarty->assign($attr."ACL"," disabled ");
      }else{
        $smarty->assign($attr."ACL","  ");
      }
    }

    $display.= $smarty->fetch(get_template_path('faiTemplateEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
    if (!isset($_POST['FAItemplatePath'])){
      return;
    }
    if((isset($_POST['SubObjectFormSubmitted'])) && ($this->FAIstate != "freeze")){
      foreach($this->attributes as $attrs){
        if($attrs == "FAItemplateFile") 
          continue;
        if($attrs == "FAIowner") {
          $this->$attrs = $_POST["user"] . '.' . $_POST["group"];
          continue;
        }
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = "";
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
      $this->FAImode= $tmode;
    }
  }

  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(empty($this->FAItemplateFile)){
      $message[]=_("Please specify a value for attribute 'file'.");
    } 

    if(empty($this->FAItemplatePath)){
      $message[]=_("Please specify a value for attribute 'path'.");
    } 
  
    if(empty($this->cn)){
      $message[] = _("Please enter a name.");
    }

    if(empty($this->user)){
      $message[] = _("Please enter a user.");
    }
    elseif(preg_match("/[^0-9a-z]/i",$this->user)){
      $message[] = _("Please enter a valid user. Only a-z/0-9 are allowed.");
    }

    if(empty($this->group)){
      $message[] = _("Please enter a group.");
    }
    elseif(preg_match("/[^0-9a-z]/i",$this->group)){
      $message[] = _("Please enter a valid group. Only a-z/0-9 are allowed.");
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
