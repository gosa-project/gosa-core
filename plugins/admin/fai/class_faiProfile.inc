<?php

class faiProfile extends plugin
{
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

  var $FAIstate       = "";
  var $base           = "";
  var $release        = "";
  var $copy_paste_mode= false;
  var $cut_paste_mode = false;

  var $CopyPasteVars  = array("FAIclass","FAIclasses","FAIAllclasses");

  function faiProfile($config, $dn= NULL)
  {
    /* Load Attributes */
    plugin::plugin ($config, $dn);
    $ldap=$this->config->get_ldap_link();

    $this->acl = "#all#";

    if($this->dn != "new"){
      /* Set acls
       */
      $ui   = get_userinfo();
      $acl  = get_permissions ($this->dn, $ui->subtreeACL);
      $acli = get_module_permission($acl, "FAIclass", $this->dn);
      $this->acl=$acli;
    }

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

    /* Search only in fai tree */
    $ObjectTypes = array(
        "FAIpartitionTable"  => array("OU"=>"ou=disk,"       ),
        "FAIpackageList"     => array("OU"=>"ou=packages,"   ),
        "FAIscript"          => array("OU"=>"ou=scripts,"    ),
        "FAIvariable"        => array("OU"=>"ou=variables,"  ),
        "FAIhook"            => array("OU"=>"ou=hooks,"      ),
#        "FAIprofile"         => array("OU"=>"ou=profiles,"  ),
        "FAItemplate"        => array("OU"=>"ou=templates,"  ));

    $base= "ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
    if($_SESSION['faifilter']['branch']!="main"){
      $base = $_SESSION['faifilter']['branch'];
    }

    /* Get ldap connection */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($base);
    $sort = array();
    /* Capture objects from given base */
    $result = array();
    foreach($ObjectTypes as $oc => $data){
      $ou = $data['OU'].$base;

      $ldap->ls("(objectClass=".$oc.")",$ou,array("cn","objectClass","dn"));
      while($attrs = $ldap->fetch()){
        $this->FAIAllclasses[$attrs['cn'][0]][$oc]=$attrs;
        $sort[strtolower($attrs['cn'][0])] = $attrs['cn'][0];
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
    /* Fill templating stuff */
    $smarty= get_smarty();
    $display= "";

    $s_entry = "";
    $s_action = "";

    /* Remove class name From list */
    $sort_once = false;
    foreach($_POST as $name => $post){
      if(preg_match("/DEL_/i",$name)){
        $s_action = "delete";
        $s_entry  = preg_replace("/DEL_/","",$name);
        $s_entry  = base64_decode(preg_replace("/_.*$/","",$s_entry));
      }elseif(preg_match("/Add_class/i",$name)){
        $s_action  = "add";
      }elseif(preg_match("/DelClass/i",$name)){
        $s_action  = "delete";
        $s_entry = $_POST['FAIclass'];
      }elseif(preg_match("/AddClass/i",$name)){
        $s_action  = "add";
      }

      /* Check if a list element should be pushed one position up */
      if((preg_match("/sortup_/",$name))&&(!$sort_once)){
        $sort_once = true;
        $val = preg_replace("/sortup_/","",$name);
        $val = preg_replace("/_.*$/","",$val);
        $val = base64_decode($val);
        $this->catUp($val);
      }
      
      /* Check if a list element should be pushed one position down */
      if((preg_match("/sortdown_/",$name))&&(!$sort_once)){
        $sort_once = true;
        $val = preg_replace("/sortdown_/","",$name);
        $val = preg_replace("/_.*$/","",$val);
        $val = base64_decode($val);
        $this->catDown($val);
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
        //ksort($this->FAIclasses);
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
    foreach($this->FAIclasses as $usedClass){
      $str = "&nbsp;";

      if(isset($this->FAIAllclasses[$usedClass])){
        foreach($this->FAIAllclasses[$usedClass] as $class => $obj){
          $str.= $objTypes[$class]; 
        }
      }
  
      $field1 = array("string"=> $usedClass,"attach"=>"");
      $field2 = array("string"=> $str,"attach"=>"");
      if($this->FAIstate != "freeze"){
        $field3 = array("string"=> preg_replace("/%KEY%/",base64_encode($usedClass),$actions).
            preg_replace("/%s/",base64_encode($usedClass),$linkupdown),
            "attach"=>"style='border-right:none;'");
      }else{
        $field3 = array("string"=>"&nbsp;", "attach"=>"style='border-right:none;'");
      }
      $divlist->AddEntry(array($field1,$field2,$field3));
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

    foreach($this->attributes as $attr){
      if(($this->FAIstate == "freeze") || (chkacl($this->acl,$attr)!= "")){
        $smarty->assign($attr."ACL"," disabled ");
      }else{
        $smarty->assign($attr."ACL","  ");
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
    show_ldap_error($ldap->get_error(), _("Removing FAI profile failed"));
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
    /* Call common method to give check the hook */
    $message= plugin::check();

    if($this->copy_paste_mode){

      /* If this is a new script, check if a script with this name already exists */
      if(!empty($this->release) && ($this->copy_paste_mode || $this->cut_paste_mode) ){

        /* Check if current name is already used for fai scripts in selected release */
        $dn = 'cn='.$this->cn.",ou=profiles,".$this->release;
        $ldap = $this->config->get_ldap_link();
        $ldap->cat($dn);
        if($ldap->count() || in_array($this->cn,$this->FAIAllclasses)){

          $r =convert_department_dn($this->release);;
          $message[] = sprintf(_("Can't insert a script named '%s' in '%s' there is already a script with the given name."),$this->cn,$r);
        }
      }
    }else{
      if(count($this->FAIclasses) == 0){
        $message[]=_("Please assign at least one class to this  profile.");
      }

      if(empty($this->cn)){
        $message[]=_("Please enter a valid name.");
      }

      $ldap = $this->config->get_ldap_link();
      $release = "ou=profiles,ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
      if($_SESSION['faifilter']['branch']!="main"){
        $release = "ou=profiles,".$_SESSION['faifilter']['branch'];
      }
      $ldap->cd($release);
      if ($this->old_cn == ""){
        $ldap->search("(&(objectClass=FAIprofile)(cn=".$this->cn."))",array("*"));
      } else {
        $ldap->search("(&(objectClass=FAIprofile)(cn=".$this->cn.")(!cn=".$this->old_cn."))",array("*"));
      }
      if($ldap->count() || isset($this->FAIAllclasses[$this->cn])){
        $message[]=_("There is already a FAI object with this class name defined.");
      }
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

    /* Copy & Paste : Ensure that FAIstate is copied too */
    if($this->copy_paste_mode && preg_match("/freeze/",$this->FAIstate)){
      $this->attrs['FAIstate'] = $this->FAIstate;
    }

    $ldap->cat($this->dn,array("objectClass"));
    if($ldap->count()!=0){
      /* Write FAIscript to ldap*/
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
    }else{
      /* Write FAIscript to ldap*/
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
    }
    show_ldap_error($ldap->get_error(), _("Saving FAI profile failed"));

    /* Do object tagging */
    $this->handle_object_tagging();
    show_ldap_error($ldap->get_error());
  }

  
  /* return copy & paste dialog
   */
  function getCopyDialog()
  {
    /* Ask for cn */
    $smarty = get_smarty();
    $smarty->assign("cn" ,$this->cn);
    $str = $smarty->fetch(get_template_path("paste_fai_object.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }

  /* Get posted cn */
  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
