<?php

class faiProfileEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array();
  var $objectclasses= array();

  var $FAIAllclasses        = array();

  function faiProfileEntry ($config, $dn= NULL,$used=array(),$objects=false)
  {
    plugin::plugin ($config, $dn);

    $base = $_SESSION['faifilter']['base'];

    $categories = array("FAIscript","FAItemplate","FAIhook","FAIvariable","FAIpartitionTAble","FAIpackage");

    $base = $_SESSION['faifilter']['base'];
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($base);
    $ldap->search("(objectClass=*)",array("*"));
    while($attrs = $ldap->fetch()){
      if((isset($attrs['cn'][0]))&&(!in_array($attrs['cn'][0],$used))){
        foreach($categories as $cat){
          if(in_array($cat,$attrs['objectClass'])){
            $this->FAIAllclasses[$attrs['cn'][0]]['objects'][$cat]=$cat;
            $this->FAIAllclasses[$attrs['cn'][0]]['status']=false;
          }
        }
      }
    }

    if (1==1||!is_global("SUBfaifilter")){
      $SUBfaifilter= array("base" => "ou=fai,ou=configs,ou=systems,".$base, "Sregex" => "*");
      $SUBfaifilter['SShowTemplates'] = false;
      $SUBfaifilter['SShowScripts']   = false;
      $SUBfaifilter['SShowHooks']     = false;
      $SUBfaifilter['SShowVariables'] = false;
      $SUBfaifilter['SShowPackages']  = false;
      $SUBfaifilter['SShowPartitions']= false;
      register_global("SUBfaifilter", $SUBfaifilter);
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

    $SUBfaifilter = $_SESSION['SUBfaifilter'];

    if(isset($_POST['apply'])){
      foreach($SUBfaifilter as $key => $val){
        if(!isset($_POST[$key])){
          $SUBfaifilter[$key] = false;
        }else{
          $SUBfaifilter[$key] =$_POST[$key];
        }
      }
    }

    $_SESSION['SUBfaifilter']= $SUBfaifilter;

    foreach($_POST as $name => $value){
      foreach($this->FAIAllclasses as $class => $obj){
        if(isset($_POST["ON_PAGE_".$class])){
          if(isset($_POST['USE_'.$class])){
            $this->FAIAllclasses[$class]['status']=true;  
          }else{
            $this->FAIAllclasses[$class]['status']=false;  
          }    
        }
      }
    }

    $objTypes['FAIhook']            = "<image src='images/fai_hook.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAItemplate']        = "<image src='images/fai_template.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAIscript']          = "<image src='images/fai_script.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAIvariable']        = "<image src='images/fai_variable.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAIpackages']        = "<image src='images/fai_packages.png' title='"._("Hook bundle")."' alt=''>";
    $objTypes['FAIpartitionTable']  = "<image src='images/fai_partitionTable.png' title='"._("Hook bundle")."' alt=''>";

    foreach($this->attributes as $attrs){
      $smarty->assign($attrs,stripslashes($this->$attrs));
    }

    foreach($SUBfaifilter as $key => $val){
      if(($key != "base")&&($key != "Sregex")){
        if($val){
          $smarty->assign($key."CHK", " checked ");
        }else{
          $smarty->assign($key."CHK", "");
        }
      }else{
        $smarty->assign($key,$val);
      }
    }
    $divlist = new divlist("ProfileEntry");
    $divlist->SetSummary(_("This list displays all assigned class names for this profile."));
    $divlist->SetEntriesPerPage(20);
    $divlist->SetHeader(array(array("string"=>"Class name"),
                              array("string"=>"Objects")));

    $action = "<input type='hidden' name='ON_PAGE_%KEY%' value='1'>
              <input type='checkbox' name='USE_%KEY%' value='%KEY%' onChange='document.mainform.submit();' %CHECK%>";

    foreach($this->FAIAllclasses as $usedClass => $classes){

      $abort = false;

      if(($SUBfaifilter['SShowScripts'])&&(!in_array('FAIscript',$classes['objects']))){
        $abort = "true";
      }
      if(($SUBfaifilter['SShowTemplates'])&&(!in_array('FAItemplate',$classes['objects']))){
        $abort = "true";
      }
      if(($SUBfaifilter['SShowHooks'])&&(!in_array('FAIhook',$classes['objects']))){
        $abort = "true";
      }
      if(($SUBfaifilter['SShowVariables'])&&(!in_array('FAIvariable',$classes['objects']))){
        $abort = "true";
      }
      if(($SUBfaifilter['SShowPartitions'])&&(!in_array('FAIpartitionTable',$classes['objects']))){
        $abort = "true";
      }
      if(($SUBfaifilter['SShowPackages'])&&(!in_array('FAIpackages',$classes['objects']))){
        $abort = "true";
      }

      if(!$abort){
        $str = "";
        foreach($classes['objects'] as $class => $obj){
          $str.= $objTypes[$obj];
        }

        if($classes['status']==true){
          $action_check = preg_replace("/%CHECK%/"," checked ",$action);
        }else{
          $action_check = preg_replace("/%CHECK%/","",$action);
        }      

        $field1 = array("string"=> preg_replace("/%KEY%/",$usedClass,$action_check).$usedClass,"attach"=>"");
        $field2 = array("string"=> $str,"attach"=>"");
        $divlist->AddEntry(array($field1,$field2));
      }
    }

    $smarty->assign("faihead"       , "");
    $smarty->assign("failist"       , $divlist->DrawList());
    $smarty->assign("infoimage"     , get_template_path('images/info.png'));
    $smarty->assign("launchimage"   , get_template_path('images/launch.png'));
    $smarty->assign("alphabet"      , generate_alphabet());
    $smarty->assign("apply"         , apply_filter(TRUE));
    $smarty->assign("search_image"  , get_template_path('images/search.png'));

    $display.= $smarty->fetch(get_template_path('faiProfileEntry.tpl', TRUE));
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
    return ($message);
  }

  function save()
  {
    $tmp = array();
    foreach($this->FAIAllclasses as $class => $obj){
      if($obj['status']==true){
        $tmp[$class]=$class;
      }
    }
    return($tmp);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
