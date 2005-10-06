<?php

class faiPackageEntry extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage server basic objects";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account= TRUE;
  var $attributes   = array();
  var $objectclasses= array();
  var $strID        = "";  

  var $buffer       = array();
  var $categories   = array();
  var $searchStr    = "*";

  /* Array with available class names */
  var $FAIAllclasses        = array();

  function faiPackageEntry ($config, $dn= NULL,$strID=false)
  {
    plugin::plugin ($config, $dn);
    $this->strID = $strID;

  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

    $divlist = new divlist("PackageEntry");
    $divlist->SetSummary(_("This list displays all assigned package names for this repository settings."));
    $divlist->SetEntriesPerPage(20);
    $divlist->SetHeader(array(array("string"=>"&nbsp;"),
                              array("string"=>"Name"),
                              array("string"=>"Version"),
                              array("string"=>"Description")));

    if(isset($_GET['search'])){
      $this->searchStr= $_GET['search']."*";
    }

    $pkgs = $this->genPkgs();
    foreach($pkgs as $cat => $entries){
      if(!isset($this->categories[$cat])){  
        $this->categories[$cat] = false;
      }
      if(isset($_POST['chkchange'])){
        $this->searchStr = $_POST['searchStr'];
        if(isset($_POST['CAT'.$cat])){
          $this->categories[$cat]=true;
        }else{
          $this->categories[$cat]=false;
        }
      } 
  
    }

    $CHKS = "<table><tr><td><input type='hidden' value='chkchange' name='chkchange'>";
    foreach($this->categories as $cat => $selected){
      if($selected){
        $CHKS .="<input type='checkbox' value='1' name='CAT".$cat."' checked>&nbsp;".$cat."&nbsp;";
      }else{
        $CHKS .="<input type='checkbox' value='1' name='CAT".$cat."'>&nbsp;".$cat."&nbsp;";
      }
    }
    $CHKS .= "<br><input type='submit' name='refresh0' value='"._("refresh")."'> </td></tr></table>";

    $packages = array();
    foreach($this->categories as $cat => $use){
      if($use){
        $packages = array_merge($packages,$pkgs[$cat]);
      }
    }
/*
    foreach($_POST as $name => $value){
      if(preg_match("/^wasonpage_/i",$name)){
        $name = preg_replace("/^wasonpage_/","",$name);
        if(isset($_POST['use_'.$name])){
          print "was checked";
        }
      }
    
    }

*/
    foreach($packages as $pkg){
      if(preg_match("/^".str_replace("*",".*",$this->searchStr)."/i",$pkg[0])){
      $chkbox = "<input type='checkbox' value='1' name='use_".base64_encode($pkg[0])."'>
                 <input type='hidden' value='1' name='wasonpage_".base64_encode($pkg[0])."'>";
      $divlist->AddEntry(array(array("string"=>$chkbox),
                               array("string"=>$pkg[0]),
                               array("string"=>$pkg[1]),
                               array("string"=>$pkg[3])));
      }
    }

    $smarty->assign("CHKS"          , $CHKS);
    $smarty->assign("searchStr"     , $this->searchStr);
    $smarty->assign("faihead"       , "");
    $smarty->assign("failist"       , $divlist->DrawList());
    $smarty->assign("infoimage"     , get_template_path('images/info.png'));
    $smarty->assign("launchimage"   , get_template_path('images/launch.png'));
    $smarty->assign("alphabet"      , generate_alphabet());
    $smarty->assign("apply"         , apply_filter(TRUE));
    $smarty->assign("search_image"  , get_template_path('images/search.png'));

    $display.= $smarty->fetch(get_template_path('faiPackageEntry.tpl', TRUE));
    return($display);
  }

  /* Save data to object */
  function save_object()
  {
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

    /* Generate listbox friendly SubObject list
  */
  function genPkgs(){
    /* Generate a list off available packages for this mirror, section and release
     */
    $strID = $this->strID;    // This var contains the path to the parsed package file
    
    /* Open the packelist */
    $fp = fopen($strID,"r");  
    $a_ret = array();
    /* Only read this file if it wasn't read before */
    if(!isset($this->buffer[$strID])){
      $str = "";
      /* Parse every line and create an array */
      while(!feof($fp)){
        $str= fgets($fp,512);
        $stra= split("\|",$str);
        if(count($stra)==4){
          $a_ret[$stra[2]][] = $stra;
          } 
        }
        
        /* Save our Data, to avoid reading it again */
      $this->buffer[$strID]=$a_ret;
      return($a_ret);
    }else{
      return $this->buffer[$strID];
    }
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
