<?php

class faiScriptEntry extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array("cn","description","FAIpriority","FAIscript");
  var $objectclasses= array("FAIscriptEntry");

  var $orig_cn          = "";
  var $dn               = "";
  var $cn               = "";
  var $FAIpriority      = "";
  var $FAIscript        = "";
  var $description      = "";
  var $status           = "new";
  var $parent           = false;
  var $is_parent_saved  = false;
  
  function faiScriptEntry ($config, $dn)
  {
    /* Dn is dn currently assigned to this object 
        'new' if object wasn't saved yet
     */
    plugin::plugin ($config, $dn);
    if($dn != "new"){
      $this->orig_cn         = $this->cn;
    }else{
      $this->status          = "new";
      $this->orig_cn         = false;
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

  /* Delete me, and all my subtrees
  */
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->rmdir_recursive($this->dn);
    show_ldap_error($ldap->get_error());
    $this->handle_post_events("remove");
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
  
    if(empty($this->cn)){
      $message[] = _("Please enter a name.");
    }

    if(preg_match("/[^0-9a-z]/i",$this->cn)){
      $message[] = _("Please enter a valid name. Only a-Z 0-9 are allowed.");
    }
 
    return ($message);
  }
 
  function save()
  {
    plugin::save();

    /* Get ldap connection */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);

    /* First : Check if parent was already saved ... */
    $ldap->cd($this->parent->dn);
    $ldap->cat($this->parent->dn);
    
    /* no entry was found. So save our base object first */
    if($ldap->count()==0){
      $this->parent->save();
    } 

    /* Generate this->dn */ 
      /* if !orig_dn -> This is a new object */
    if($this->orig_cn == false){
      $this->dn = "cn=".$this->cn.",".$this->parent->dn;
      $mode = "add";
    }else{
      /* This is an already existing entry, check if cn was changed */
      if($this->orig_cn != $this->cn ){
        /* rename */
        $ldap->cd($this->dn);
        $ldap->rmdir($this->dn);
        $this->dn = "cn=".$this->cn.",".$this->parent->dn;
        $mode = "add";
      }else{
        /* modify */
        $mode = "modify";
      }
    }

    $ldap->cd($this->config->current['BASE']);
    $ldap->create_missing_trees($this->parent->dn);
    $ldap->cd ($this->dn);
    $ldap->$mode($this->attrs);
    show_ldap_error($ldap->get_error());
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
