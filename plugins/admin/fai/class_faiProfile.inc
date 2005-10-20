<?php

class faiProfile extends plugin
{
  /* CLI vars */
  var $cli_summary      = "Manage server basic objects";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account   = TRUE;

  /* Attributes for this Object */
  var $attributes       = array("cn","description","FAIclass");

  /* ObjectClasses for this Object*/
  var $objectclasses    = array("top","FAIclass","FAIprofile");

  /* Class name of the Ldap ObjectClass for the Sub Object */
  var $subClass         = "FAIscriptEntry";
  var $subClasses       = array("top","FAIclass","FAIscriptEntry");

  /* Specific attributes */
  var $old_cn           = "";
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $dialog           = NULL;     // a dialog, e.g. new disk dialog
  var $FAIclass         = "";       // Classnames used for this profile
  var $FAIclasses       = array();  // Contains classname seperated in an array
  var $FAIAllclasses    = array();  // Contains all possible Classnames

  function faiProfile($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);
    $ldap=$this->config->get_ldap_link();

    $tmp = split(" ",$this->FAIclass);
    
    foreach($tmp as $class){
      if(!empty($class)){
        $this->FAIclasses[trim($class)] = trim($class);
      }
    }
    natcasesort($this->FAIclasses);
    reset($this->FAIclasses) ;
 
    $categories = array("FAIscript","FAItemplate","FAIhook","FAIvariable","FAIpartitionTAble","FAIpackage");
  
    $sort = array();
    
    $base = $_SESSION['faifilter']['base'];
    $ldap->cd($base);
    $ldap->search("(objectClass=*)",array("*"));
    while($attrs = $ldap->fetch()){
      foreach($categories as $cat){
        if(in_array($cat,$attrs['objectClass'])){
          $this->FAIAllclasses[$attrs['cn'][0]][$cat]=$attrs;
          $sort[strtolower($attrs['cn'][0])] = $attrs['cn'][0];
        }
      }
    } 
    ksort($sort);
    foreach($sort as $name){
      $tmp[$name] =$this->FAIAllclasses[$name];
    }
    $this->FAIAllclasses = array();
    $this->FAIAllclasses = $tmp;

    if($dn != "new"){
      $this->dn =$dn;
    }
    $this->old_cn   = $this->cn;
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $s_entry = "";
    $s_action = "";

    /* Remove class name From list */
    foreach($_POST as $name => $post){
      if(preg_match("/DEL_/i",$name)){
        $s_action = "delete";
        $s_entry  = preg_replace("/DEL_/","",$name);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
      }elseif(preg_match("/Add_class/i",$name)){
        $s_action  = "add";
      }
    }

    if($s_action == "delete"){
      unset($this->FAIclasses[$s_entry]);
    }

    if($s_action == "add"){
      $this->dialog = new faiProfileEntry($this->config,$this->dn,$this->FAIclasses);
      $this->is_dialog  =true;
    }
  
    /* Save Dialog */
    if(isset($_POST['SaveSubObject'])){
      $this->dialog->save_object();
      $msgs= $this->dialog->check();
      if(count($msgs)){
        print_red($msgs);
      }else{
        $ret = $this->dialog->save();
        foreach($ret as $class){
          $this->FAIclasses[$class] =$class; 
        }
        $this->is_dialog=false;
        unset($this->dialog);
        $this->dialog=NULL;
      }
    }

    /* Cancel Dialog */
    if(isset($_POST['CancelSubObject'])){
      $this->is_dialog=false;
      unset($this->dialog);
      $this->dialog=NULL;
    }
    
    if(isset($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    $divlist  =new divlist("Profile");
    $divlist->SetSummary(_("This list displays all assigned class names for this profile."));
    $divlist->SetEntriesPerPage(10);

    $divlist->SetHeader(array(array("string"=>"Class name"),
                              array("string"=>"Objects","attach"=>"style='width:120px;'"),
                              array("string"=>"Options","attach"=>"style='border-right:none;width:60px;'")));
    $listhead = 
    "<div style='background:#F0F0F9;padding:5px;'>".
    "<input type='image' align='middle' src='images/editpaste.png' title='"._("Append new class names")."' alt='"._("Add")."' name='Add_class'>&nbsp;".
    "</div>";

    $objTypes['FAIhook']            = "<image src='images/fai_hook.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAItemplate']        = "<image src='images/fai_template.png' title='"._("Template bundle")."' alt=''>";
    $objTypes['FAIscript']          = "<image src='images/fai_script.png' title='"._("Script bundle")."' alt=''>";
    $objTypes['FAIvariable']        = "<image src='images/fai_variable.png' title='"._("Variable bundle")."' alt=''>";
    $objTypes['FAIpackages']        = "<image src='images/fai_packages.png' title='"._("Packages bundle")."' alt=''>";
    $objTypes['FAIpartitionTable']  = "<image src='images/fai_partitionTable.png' title='"._("Partition table")."' alt=''>";

    $actions = "<input type='image' src='images/editdelete.png' title='"._("Remove class from profile")."' name='DEL_%KEY%'>"; 

    foreach($this->FAIclasses as $usedClass){
      $str = "";
      if(isset($this->FAIAllclasses[$usedClass])){
        foreach($this->FAIAllclasses[$usedClass] as $class => $obj){
          $str.= $objTypes[$class]; 
        }
      }

      $field1 = array("string"=> $usedClass,"attach"=>"");
      $field2 = array("string"=> $str,"attach"=>"");
      $field3 = array("string"=> preg_replace("/%KEY%/",$usedClass,$actions),"attach"=>"style='border-right:none;'");
      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    $smarty->assign("divlisthead" ,$listhead);
    $smarty->assign("divlist"     ,$divlist->DrawList());

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


    $display.= $smarty->fetch(get_template_path('faiProfile.tpl', TRUE));
    return($display);
  }
  
  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->rmdir_recursive($this->dn);
    $this->handle_post_events("remove");    
  }

  /* Save data to object 
   */
  function save_object()
  {
    plugin::save_object();
    foreach($this->attributes as $attrs){
      if(isset($_POST[$attrs])){
        $this->$attrs = $_POST[$attrs];
      }
    }
  }


  /* Check supplied data */
  function check()
  {
    $message= array();
    
    if(count($this->FAIclasses) == 0){
      $message[]=_("Please assign at least one class to this  profile.");
    }
 
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($_SESSION['faifilter']['base']);
    $ldap->search("(&(objectClass=FAIprofile)(cn=".$this->cn.")(!cn=".$this->old_cn."))",array("*"));
 
    if($ldap->count()){
      $message[]=_("There is already a profile with this class name defined.");
    }

    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    plugin::save();
 
    $ldap = $this->config->get_ldap_link();

    $this->FAIclass = "";
    foreach($this->FAIclasses as $class){
      $this->FAIclass.=$class." ";
    }

    $this->attrs['FAIclass']=trim($this->FAIclass);

    $ldap->cat($this->dn);
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $ldap->modify($this->attrs);
    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error());
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
