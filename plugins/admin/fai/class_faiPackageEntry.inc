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
  var $pkgs        = array();  

  var $buffer       = array();
  var $categories   = array();
  var $searchStrPKG = "";
  var $selected     = array();

  /* Array with available class names */
  var $FAIAllclasses        = array();

  function faiPackageEntry (&$config, $dn= NULL,$pkgs,$selected=array())
  {
    plugin::plugin ($config, $dn);
    
    $this->pkgs=$pkgs;

    if(session::is_set('searchStrPKG')){
      $this->searchStrPKG=$_SESSION['searchStrPKG'];
    }
    $this->selected=$selected;

    $this->searchStrPKG= _("Please enter your search string here");
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

    $divlist = new divlist("PackageEntry");
    $divlist->SetPluginMode();
    $divlist->SetSummary(_("This list displays all assigned package names for this repository settings."));
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeight("650px;");
    $divlist->SetHeader(array(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
                              array("string" => "Name", "attach" => "style='width:180px;'"),
                              array("string" => "Version", "attach" ),
                              array("string" => "Description", "attach" => "style='width:240px;border-right:0px;text-align:left;'")));

    if(isset($_GET['search'])){
      $this->searchStrPKG= $_GET['search']."*";
    }
    
    $pkgs= $this->pkgs;

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

    $this->searchStrPKG = preg_replace("/^\+/","",$this->searchStrPKG);
    if(empty($this->searchStrPKG)){
      $this->searchStrPKG = "*";
    }

    @ksort($this->categories);

$CHKS = "<input type='hidden' value='chkchange' name='chkchange'><table summary=''><tr>";
    $i = 0;
    if(is_array($this->categories)) 
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
    $CHKS .= "</tr></table>";


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

    /* Prepare search filter */
    $search_str = preg_replace("/\\\\\*/",".*",normalizePreg($this->searchStrPKG));

    $cnt = 0; 
    foreach($pkgs as $pkg){
      if($cnt > 200) {
        print_red(sprintf(_("Your specified search string '%s' returned too many results. Only the first 200 entries are shown, to keep the memory usage low."),$this->searchStrPKG));
        break;
      }
      if((preg_match("/^".$search_str."/i",$pkg[0]))||
         (preg_match("/^".$search_str."/i",$pkg[3]))){
        if($this->categories[$pkg[2]]){
          if(in_array($pkg[0],$this->selected)){
            $check = " checked ";
          }else{
            $check = "";
          }
          $cnt ++ ;
          $chkbox = "<input type='checkbox' value='1' name='use_".base64_encode($pkg[0])."' ".$check.">
                   <input type='hidden' value='1' name='wasonpage_".base64_encode($pkg[0])."'>";
          $divlist->AddEntry(array(array("string"=>$chkbox, "attach" => "style='text-align:center;width:20px;'"),
                               array("string"=>$pkg[0], "attach" => "style='width:180px;'"),
                               array("string"=>htmlentities($pkg[1])),
                               array("string"=>htmlentities($pkg[3]), "attach" => "style='width:221px;border-right:0px;text-align:left;'")));
        }
      }
    }

    session::set('searchStrPKG',$this->searchStrPKG);
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
    /* Call common method to give check the hook */
    $message= plugin::check();
        
    return ($message);
  }

  function save()
  {
    return($this->selected);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
