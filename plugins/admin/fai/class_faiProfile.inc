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
    $tmp3 = $tmp2 = array();
    foreach($tmp as $class){
      if(!empty($class)){
        if(preg_match("/|/",$class)){
          $o = split("\|",$class);
          $tmp3[$o[1]] = trim($o[0]);
        }else{
          $tmp2[trim($class)] = trim($class);
        }
      }
    }
    $this->FAIclasses = array();
    ksort($tmp3);
    if(is_array($tmp3)){
      foreach($tmp3 as $class){
        $this->FAIclasses[$class]=$class;
      }
    }
    
    if(is_array($tmp2)){
      foreach($tmp2 as $class){
        $this->FAIclasses[$class]=$class;
      }
    }

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


  /* Combine new array */
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

  /* TRansports the geiven Arraykey one position up*/
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


  /* TRansports the geiven Arraykey one position up*/
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

  function catUp($id)
  {
    /* Get all cats depinding on current dir */
    $cats = $this->FAIclasses;
    $this->FAIclasses =$this->ArrayUp($id,$cats);
  }

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
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
      }elseif(preg_match("/Add_class/i",$name)){
        $s_action  = "add";
      }elseif(preg_match("/DelClass/i",$name)){
        $s_action  = "delete";
        $s_entry = $_POST['FAIclass'];
      }elseif(preg_match("/AddClass/i",$name)){
        $s_action  = "add";
      }

      if((preg_match("/sortup_/",$name))&&(!$sort_once)){
        $sort_once = true;
        $val = preg_replace("/sortup_/","",$name);
        $val = preg_replace("/_.*$/","",$val);
        $val = base64_decode($val);
        $this->catUp($val);
      }
      
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
        ksort($this->FAIclasses);
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

    $objTypes['FAIhook']            = "<img src='images/fai_hook.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAItemplate']        = "<img src='images/fai_template.png' title='"._("Template bundle")."' alt=''>";
    $objTypes['FAIscript']          = "<img src='images/fai_script.png' title='"._("Script bundle")."' alt=''>";
    $objTypes['FAIvariable']        = "<img src='images/fai_variable.png' title='"._("Variable bundle")."' alt=''>";
    $objTypes['FAIpackages']        = "<img src='images/fai_packages.png' title='"._("Packages bundle")."' alt=''>";
    $objTypes['FAIpartitionTable']  = "<img src='images/fai_partitionTable.png' title='"._("Partition table")."' alt=''>";

    $actions = "<input type='image' src='images/editdelete.png' title='"._("Remove class from profile")."' name='DEL_%KEY%'>"; 

    $linkupdown = "&nbsp;<input type='image' name='sortup_%s'   alt='up'    title='"._("Up")."'   src='images/sort_up.png' align='top' >";
    $linkupdown.= "<input type='image' name='sortdown_%s' alt='down'  title='"._("Down")."' src='images/sort_down.png' >";

    foreach($this->FAIclasses as $usedClass){
      $str = "&nbsp;";


      if(isset($this->FAIAllclasses[$usedClass])){
        foreach($this->FAIAllclasses[$usedClass] as $class => $obj){
          $str.= $objTypes[$class]; 
        }
      }

          
  
      $field1 = array("string"=> $usedClass,"attach"=>"");
      $field2 = array("string"=> $str,"attach"=>"");
      $field3 = array("string"=> preg_replace("/%KEY%/",$usedClass,$actions).preg_replace("/%s/",base64_encode($usedClass),$linkupdown),"attach"=>"style='border-right:none;'");
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

    if(empty($this->cn)){
      $message[]=_("Please enter a valid name.");
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
    $i = 0 ; 
    foreach($this->FAIclasses as $class){
      $i ++ ; 
      $this->FAIclass.=$class."|".$i." ";
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
