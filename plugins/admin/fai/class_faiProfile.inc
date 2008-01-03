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

  /* Specific attributes */
  var $old_cn           = "";
  var $cn               = "";       // The class name for this object
  var $description      = "";       // The description for this set of partitions
  var $is_dialog        = false;    // specifies which buttons will be shown to save or abort
  var $FAIclass         = "";       // Classnames used for this profile
  var $FAIclasses       = array();  // Contains classname seperated in an array
  var $FAIAllclasses    = array();  // Contains all possible Classnames
  var $ui               ;
  var $FAIstate      = "";
  var $view_logged      = FALSE;

  function faiProfile(&$config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);
    $ldap=$this->config->get_ldap_link();

    $this->ui = get_userinfo();

    /* Parse ldap attribute to get all assigned classes */
    $tmp = split(" ",$this->FAIclass);
    $tmp2 = array();
    foreach($tmp as $class){
      if(!empty($class)){
        $tmp2[trim($class)] = trim($class);
      }
    }

    if(isset($this->attrs['FAIstate'][0])){
      $this->FAIstate = $this->attrs['FAIstate'][0];
    }

    /* Sort assigned classes */ 
    if(is_array($tmp2)){
      foreach($tmp2 as $class){
        $this->FAIclasses[$class]=$class;
      }
    }

    $categories = array("FAIscript","FAItemplate","FAIhook","FAIvariable","FAIpartitionTable","FAIpackageList");

    /* Build filter */
    $filter= "";
    foreach ($categories as $cat){
      $filter.= "(objectClass=$cat)";
    }
    
    /* Get ldap connection */ 
    $base = $_SESSION['CurrentMainBase'];
    $ldap->cd($base);
    $sort = array();

    /* search all FAI classes */
    $ldap->search("(|$filter)",array("*"));
    while($attrs = $ldap->fetch()){

      /* Sort by categorie */
      foreach($categories as $cat){
        if(in_array($cat,$attrs['objectClass'])){

          /* Append entry */
          $this->FAIAllclasses[$attrs['cn'][0]][$cat]=$attrs;
  
          /* Create sort array, because the array above is a multidimensional array, and can't be sorted by php sorting functions*/
          $sort[strtolower($attrs['cn'][0])] = $attrs['cn'][0];
        }
      }
    } 

    /* Sort the sort array */
    //ksort($sort);

    /* Reorder the FAIclasses array */
    foreach($sort as $name){
      $tmp[$name] =$this->FAIAllclasses[$name];
    }

    /* Assign sorted classes */
    $this->FAIAllclasses = array();
    $this->FAIAllclasses = $tmp;

    if($dn != "new"){
      $this->dn =$dn;
    }
    $this->old_cn   = $this->cn;
  }


  /* Combine new array, used for up down buttons */
  function combineArrays($ar0,$ar1,$ar2)
  {
    $ret = array();
    if(is_array($ar0))
      foreach($ar0 as $ar => $a){
        $ret[$ar]=$a;
      }
    if(is_array($ar1))
      foreach($ar1 as $ar => $a){
        $ret[$ar]=$a;
      }
    if(is_array($ar2))
      foreach($ar2 as $ar => $a){
        $ret[$ar]=$a;
      }
    return($ret);
  }


  function acl_base_for_current_object($dn)
  {
    if($dn == "new"){
      if($this->dn == "new"){
        $dn= $_SESSION['CurrentMainBase'];
      }else{
        $dn = $this->dn;
      }
    }
    return($dn);
  }


  /* returns position in array */
  function getpos($atr,$attrs)
  {
    $i = 0;
    foreach($attrs as $attr => $name)    {
      $i++;
      if($attr == $atr){
        return($i);
      }
    }
    return(-1);
  }

  /* Transports the given Arraykey one position up*/
  function ArrayUp($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == 1))){
      $before = array_slice($attrs,0,($pos-2));
      $mitte  = array_reverse(array_slice($attrs,($pos-2),2));
      $unten  = array_slice($attrs,$pos);
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }


  /* Transports the given Arraykey one position down*/
  function ArrayDown($atr,$attrs)
  {
    $ret = $attrs;
    $pos = $this->getpos($atr,$attrs) ;
    $cn = count($attrs);
    if(!(($pos == -1)||($pos == $cn))){
      $before = array_slice($attrs,0,($pos-1));
      $mitte  = array_reverse(array_slice($attrs,($pos-1),2));
      $unten  = array_slice($attrs,($pos+1));
      $ret = array();
      $ret = $this->combineArrays($before,$mitte,$unten);
    }
    return($ret);
  }

  /* class one position up */
  function catUp($id)
  {
    /* Get all cats depinding on current dir */
    $cats = $this->FAIclasses;
    $this->FAIclasses =$this->ArrayUp($id,$cats);
  }

  /* Class one position down */
  function catDown($id)
  {
    /* Get all cats depinding on current dir */
    $cats = $this->FAIclasses;
    $this->FAIclasses =$this->ArrayDown($id,$cats);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","fai/".get_class($this),$this->dn);
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $s_entry = "";
    $s_action = "";

    /* Remove class name From list */
    $sort_once = false;
    foreach($_POST as $name => $post){
      if(preg_match("/DEL_/i",$name) && $this->acl_is_writeable("FAIclass")){
        $s_action = "delete";
        $s_entry  = preg_replace("/DEL_/","",$name);
        $s_entry  = base64_decode(preg_replace("/_.*$/","",$s_entry));
      }elseif(preg_match("/Add_class/i",$name)&& $this->acl_is_writeable("FAIclass")){
        $s_action  = "add";
      }elseif(preg_match("/DelClass/i",$name) && $this->acl_is_writeable("FAIclass")){
        $s_action  = "delete";
        $s_entry = $_POST['FAIclass'];
      }elseif(preg_match("/AddClass/i",$name) && $this->acl_is_writeable("FAIclass")){
        $s_action  = "add";
      }

      /* Check if a list element should be pushed one position up */
      if((preg_match("/sortup_/",$name))&&(!$sort_once) && $this->acl_is_writeable("FAIclass")){
        $sort_once = true;
        $val = preg_replace("/sortup_/","",$name);
        $val = preg_replace("/_.*$/","",$val);
        $val = base64_decode($val);
        $this->catUp($val);
      }
      
      /* Check if a list element should be pushed one position down */
      if((preg_match("/sortdown_/",$name))&&(!$sort_once) && $this->acl_is_writeable("FAIclass")){
        $sort_once = true;
        $val = preg_replace("/sortdown_/","",$name);
        $val = preg_replace("/_.*$/","",$val);
        $val = base64_decode($val);
        $this->catDown($val);
      }

    }

    if($s_action == "delete" && $this->acl_is_writeable("FAIclass")){
      unset($this->FAIclasses[$s_entry]);
    }

    if($s_action == "add" && $this->acl_is_writeable("FAIclass")){
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
        $this->dialog=FALSE;
        //ksort($this->FAIclasses);
      }
    }

    /* Cancel Dialog */
    if(isset($_POST['CancelSubObject'])){
      $this->is_dialog=false;
      unset($this->dialog);
      $this->dialog=FALSE;
    }

    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }

    $divlist  =new divSelectBox("Profile");
    $divlist->SetSummary(_("This list displays all assigned class names for this profile."));

    /* item images */
    $objTypes['FAIhook']            = "<img src='images/fai_hook.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAItemplate']        = "<img src='images/fai_template.png' title='"._("Template bundle")."' alt=''>";
    $objTypes['FAIscript']          = "<img src='images/fai_script.png' title='"._("Script bundle")."' alt=''>";
    $objTypes['FAIvariable']        = "<img src='images/fai_variable.png' title='"._("Variable bundle")."' alt=''>";
    $objTypes['FAIpackageList']        = "<img src='images/fai_packages.png' title='"._("Packages bundle")."' alt=''>";
    $objTypes['FAIpartitionTable']  = "<img src='images/fai_partitionTable.png' title='"._("Partition table")."' alt=''>";

    /* Delete button */
    $actions = "<input type='image' src='images/edittrash.png' title='"._("Remove class from profile")."' name='DEL_%KEY%'>"; 
    
    /* Up down buttons */
    $linkupdown = "&nbsp;<input type='image' name='sortup_%s'   alt='up'    title='"._("Up")."'   src='images/sort_up.png' align='top' >";
    $linkupdown.= "<input type='image' name='sortdown_%s' alt='down'  title='"._("Down")."' src='images/sort_down.png' >";

    /* Append fai classes to divlist */
    if($this->acl_is_readable("FAIclass")){
      foreach($this->FAIclasses as $usedClass){
        $str = "&nbsp;";
        $act = "";

        if(isset($this->FAIAllclasses[$usedClass])){
          foreach($this->FAIAllclasses[$usedClass] as $class => $obj){
            $str.= $objTypes[$class]; 
          }
        }

        $field1 = array("string"=> $usedClass,"attach"=>"");
        $field2 = array("string"=> $str,"attach"=>"");
        if(($this->FAIstate != "freeze") && $this->acl_is_writeable("FAIclass")){
          $field3 = array("string"=> preg_replace("/%KEY%/",base64_encode($usedClass),$actions).
              preg_replace("/%s/",base64_encode($usedClass),$linkupdown),
              "attach"=>"style='border-right:none;'");
        }else{
          $field3 = array("string"=>"&nbsp;", "attach"=>"style='border-right:none;'");
        }
        $divlist->AddEntry(array($field1,$field2,$field3));
      }
    }

    $smarty->assign("FAIclasses"  ,$this->FAIclasses);
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

    
    $dn = $this->acl_base_for_current_object($this->dn);
    $smarty->assign("sub_object_is_addable",
        preg_match("/c/",$this->ui->get_permissions($dn,"fai/faiScriptEntry")) &&
        !preg_match("/freeze/",$this->FAIstate));

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    $display.= $smarty->fetch(get_template_path('faiProfile.tpl', TRUE));
    return($display);
  }

  function remove_from_parent()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($this->dn);

#    $use_dn = str_ireplace( get_release_dn($this->dn), $_SESSION['faifilter']['branch'], $this->dn);
    $use_dn = preg_replace("/".normalizePreg(get_release_dn($this->dn))."/i", $_SESSION['faifilter']['branch'], $this->dn);
    if($_SESSION['faifilter']['branch'] == "main"){
      $use_dn = $this->dn;
    }

    new log("remove","fai/".get_class($this),$use_dn,$this->attributes);
    prepare_to_save_FAI_object($use_dn,array(),true);
    $this->handle_post_events("remove");    
  }


  /* Save data to object 
   */
  function save_object()
  {
    plugin::save_object();
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(count($this->FAIclasses) == 0){
      $message[]=_("Please assign at least one class to this  profile.");
    }

    if(empty($this->cn)){
      $message[]=_("Please enter a valid name.");
    }

    $ldap = $this->config->get_ldap_link();

    $base = $_SESSION['faifilter']['branch'];
    if($base == "main"){
      $base =$_SESSION['CurrentMainBase'];
    }
    $base = get_ou('faiprofileou').$base;
  
    $ldap->cd($base);
    if ($this->old_cn == ""){
      $ldap->ls("(&(objectClass=FAIprofile)(cn=".$this->cn."))",$base,array("*"));
    } else {
      $ldap->ls("(&(objectClass=FAIprofile)(cn=".$this->cn.")(!cn=".$this->old_cn."))",$base,array("*"));
    }

    if($ldap->count() || isset($this->FAIAllclasses[$this->cn])){
      $message[]=_("There is already a FAI object with this class name defined.");
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

    prepare_to_save_FAI_object($this->dn,$this->attrs);
   
    if($this->initially_was_account){
      new log("modify","fai/".get_class($this),$this->dn,$this->attributes);
    }else{
      new log("create","fai/".get_class($this),$this->dn,$this->attributes);
    }
 
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of FAI/profile with dn '%s' failed."),$this->dn));

    /* Do object tagging */
    $this->handle_object_tagging();
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of FAI/profile with dn '%s' failed."),$this->dn));
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    /* Parse ldap attribute to get all assigned classes */
    $tmp = split(" ",$this->FAIclass);
    $tmp2 = array();
    foreach($tmp as $class){
      if(!empty($class)){
        $tmp2[trim($class)] = trim($class);
      }
    }

    /* Sort assigned classes */
    if(is_array($tmp2)){
      foreach($tmp2 as $class){
        $this->FAIclasses[$class]=$class;
      }
    }
  }


  /* Return plugin informations for acl handling */ 
  static function plInfo()
  {
    return (array( 
          "plShortName" => _("Profile"),
          "plDescription" => _("FAI profile"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 30,
          "plSection"     => array("administration"),
          "plCategory"    => array("fai"),
          "plProvidedAcls" => array(
            "cn"                => _("Name"),
            "description"       => _("Description"),
            "FAIclass"          => _("FAI classes"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
