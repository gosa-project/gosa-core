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
  var $searchStrPKG = "Please enter your search string here";
  var $selected     = array();

  /* Array with available class names */
  var $FAIAllclasses        = array();

  function faiPackageEntry ($config, $dn= NULL,$strID=false,$selected=array())
  {
    plugin::plugin ($config, $dn);
    $this->strID = $strID;

    if(isset($_SESSION['searchStrPKG'])){
      $this->searchStrPKG=$_SESSION['searchStrPKG'];
    }

    $this->selected=$selected;
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
                              array("string"=>"Description","attach"=>"style='border-right:0px;'")));

    if(isset($_GET['search'])){
      $this->searchStrPKG= $_GET['search']."*";
    }

    $pkgs = $this->genPkgs();
    unset($this->categories);
    foreach($pkgs as $name => $entry){
      $cat = $entry[2];
      if(!empty($cat)){
        if(!isset($this->categories[$cat])){  
          $this->categories[$cat] = true;
        }
        if(isset($_POST['chkchange'])){
          $this->searchStrPKG = $_POST['searchStrPKG'];
          if(isset($_POST['CAT'.$cat])){
            $this->categories[$cat]=true;
          }else{
            $this->categories[$cat]=false;
          }
        }
      } 
    }

    ksort($this->categories);

    $CHKS = "<table><tr><input type='hidden' value='chkchange' name='chkchange'>";
    $i = 0; 
    foreach($this->categories as $cat => $selected){
      if($i == 3){
        $CHKS .="</tr><tr>";
        $i = 0;
      }
      $i ++ ;
      if($selected){
        $CHKS .="<td><input type='checkbox' value='1' name='CAT".$cat."' checked onclick='document.mainform.submit()'>&nbsp;".$cat."&nbsp;</td>";
      }else{
        $CHKS .="<td><input type='checkbox' value='1' name='CAT".$cat."' onclick='document.mainform.submit()'>&nbsp;".$cat."&nbsp;</td>";
      }
    }
    $CHKS .= "</td></tr></table>";


    foreach($_POST as $name => $value){
      if(preg_match("/^wasonpage_/i",$name)){
        $name = preg_replace("/^wasonpage_/","",$name);
        $n =base64_decode($name);

        if(isset($_POST['use_'.$name])){
          $this->selected[$n] = $n; 
        }else{
          if(isset($this->selected[$n])){
            unset($this->selected[$n]);
          }
        }

      }
    }

    foreach($pkgs as $pkg){
      if(preg_match("/^".str_replace("*",".*",$this->searchStrPKG)."/i",$pkg[0])){
        if($this->categories[$pkg[2]]){
          if(in_array($pkg[0],$this->selected)){
            $check = " checked ";
          }else{
            $check = "";
          }
          $chkbox = "<input type='checkbox' value='1' name='use_".base64_encode($pkg[0])."' ".$check." onclick='document.mainform.submit()'>
                   <input type='hidden' value='1' name='wasonpage_".base64_encode($pkg[0])."'>";
          $divlist->AddEntry(array(array("string"=>$chkbox),
                                 array("string"=>$pkg[0]),
                               array("string"=>$pkg[1]),
                               array("string"=>$pkg[3],"attach"=>"style='border-right:0px'")));
        }
      }
    }

    $_SESSION['searchStrPKG']       = $this->searchStrPKG;
    $smarty->assign("CHKS"          , $CHKS);
    $smarty->assign("searchStrPKG"  , $this->searchStrPKG);
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
    foreach($_POST as $name => $value){
      if(preg_match("/^wasonpage_/i",$name)){
        $name = preg_replace("/^wasonpage_/","",$name);
        $n =base64_decode($name);
        if(isset($_POST['use_'.$name])){
          $this->selected[$n] = $n;
        }else{
          if(isset($this->selected[$n])){
            unset($this->selected[$n]);
          }
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
    return($this->selected);
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
    $this->buffer = array();
    /* Only read this file if it wasn't read before */
    if(!isset($this->buffer[$strID])){
      $str = "";
      /* Parse every line and create an array */
      while(!feof($fp)){
        $str= fgets($fp,512);
        $stra= split("\|",$str);
        if(count($stra)==4){
          $a_ret[$stra[0]] = $stra;
          }
        }
        /* Save our Data, to avoid reading it again */
      $this->buffer[$strID]=$a_ret;

      ksort($a_ret);

      return($a_ret);
    }else{
      return $this->buffer[$strID];
    }
  }


}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
