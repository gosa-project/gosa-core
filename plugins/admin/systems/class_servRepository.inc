<?php

class servrepository extends plugin
{
  /* CLI vars */
  var $cli_summary          = "Manage server basic objects";
  var $cli_description      = "Some longer text\nfor help";
  var $cli_parameters       = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  //  var $ignore_account         = TRUE;
  var $attributes             = array("FAIrepository");
  var $objectclasses          = array("FAIrepositoryServer");
  
  /* Search filter */
  var $regex                  = "*";

  /* Configurationdialog for repositories */
  var $dialog                 = NULL;

  /* Repositories */
  var $repositories           = array();
  var $FAIrepository           = array();

  function servrepository ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    $this->repositories = array();
    if(isset($this->attrs['FAIrepository'])){
      for($i = 0; $i < $this->attrs['FAIrepository']['count']; $i++){
        $tmp = split("\|",$this->attrs['FAIrepository'][$i]);
        $tmp2 = array();  
        $tmp3 = array();   

        $tmp2['ParentServer'] = $tmp[1];
        if(empty($tmp[1])){
          $tmp2['ParentServer'] = "none";
        }
        $tmp2['Url']          = $tmp[0];
        $tmp2['Release']      = $tmp[2];

        $tmp3 = split(",",$tmp[3]);

        foreach($tmp3 as $sec){
          $tmp2['Sections'][$sec]=$sec;
        }    
        $this->repositories[$tmp[2]]=$tmp2;      
      }
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    /* Smarty vars*/
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("launchimage",  get_template_path('images/small_filter.png'));
    $smarty->assign("tree_image",   get_template_path('images/tree.png'));
    $smarty->assign("alphabet",     generate_alphabet());
    $smarty->assign("apply",        apply_filter());
    $smarty->assign("regex",        $this->regex);


    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account){
      $display= $this->show_header(_("Remove FAI repository extension."),
          _("This server has FAI repository features enabled. You can disable them by clicking below."));
    } else {
      $display= $this->show_header(_("Add FAI repository extension."),
          _("This server has FAI repository features disabled. You can enable them by clicking below."));
      return ($display);
    }
 
    /*
      ADD / EDIT Repository
      Dialog Handling
    */
    
    $once = false;
    foreach($_POST as $name => $value){
      if((preg_match("/^delete_/",$name))&&(!$once)){
        if(isset($this->repositories[$value])){
          $once = true;
          unset($this->repositories[$value]);
        }
      }

      if((preg_match("/^edit_/",$name))&&(!$once)){
        if(isset($this->repositories[$value])){
          $once = true;
          $obj = $this->repositories[$value];
      
          /* to be able to detect if this was renamed */
          $obj['initialy_was'] = $obj['Release'];
          $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
          $this->dialog->acl = $this->acl;
        }
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="open")&&(isset($_GET['id']))){
      $obj = $this->repositories[$_GET['id']];
      $obj['initialy_was'] = $obj['Release'];
      $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
      $this->dialog->acl = $this->acl;
    }
 
    if(isset($_POST['AddRepository'])){
      $this->dialog = new servRepositorySetup($this->config,$this->dn);
      $this->dialog->acl = $this->acl;
    }

    if(isset($_POST['repository_setup_save'])){
      $this->dialog->save_object();
      if(($this->dialog->is_new_name())&&(isset($this->repositories[$this->dialog->GetName()]))){
        print_red(_("This name is already in use."));
      }else

      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          print_red($msg);
        }
      }else{
        $obj = $this->dialog->save();
        $this->dialog = NULL;
        $this->is_dialog= false;
        $this->repositories[$obj['Release']]=$obj;        
      }
    }

    if(isset($_POST['repository_setup_cancel'])){
      $this->dialog=NULL;
      $this->is_dialog = false;
    }
   
    if($this->dialog != NULL){
      $this->dialog->save_object();
      $this->is_dialog = true;
      return($this->dialog->execute());
    }

    /*
      Repository setup dialog handling /END
    */

    $divlist = new divSelectBox("repositories");
    $divlist->setHeight(400);
    $link   = "<a href='?plug=".$_GET['plug']."&act=open&id=%s'>%s</a>";
    $edit   = "<input type='image' value='%s' name='edit_%s'   src='images/edit.png'>&nbsp;";
    $delete = "<input type='image' value='%s' name='delete_%s' src='images/edittrash.png'>";
   
    foreach($this->repositories as $name => $reps){
      $str = " ";
      foreach($reps['Sections'] as $sec){
        $str.=$sec." ";  
      }    
  
      $divlist->AddEntry(array(
                              array("string"=>preg_replace("/%s/",$name,$link)),
                              array("string"=>sprintf($link,$name,_("Sections")." :".$str)),
                              array("string"=>preg_replace("/%s/",$name,$edit.$delete),"attach"=>"style='border-right:0px;'")
                              ));
    }
 
    $smarty -> assign("Repositories",$divlist->DrawList());

    $display.= $smarty->fetch(get_template_path('servRepository.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    plugin::remove_from_parent();    
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    
    $ldap->cat($this->dn);
    
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);      
      $this->handle_post_events("modify");
    }
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();

    $arr = array();
    foreach($this->repositories as $servername => $conf){
      $str = "";
      foreach($conf['Sections'] as $sec){
        $str.=$sec.",";
      }
      $str=preg_replace("/,$/","",$str);
        
      if($conf['ParentServer']=="none"){
        $conf['ParentServer'] ="";
      }    
    
      $arr[]=$conf['Url']."|".$conf['ParentServer']."|".$conf['Release']."|".$str;
    }
    $this->attrs['FAIrepository'] = $arr;

    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    
    $ldap->cat($this->dn);
    
    if($ldap->count()){
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);      
      $this->handle_post_events("modify");
    }else{
      $ldap->cd ($this->config->current['BASE']);
      $ldap->create_missing_trees($this->dn);
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
