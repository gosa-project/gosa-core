<?php

class faiPackageNew extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage server basic objects";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $ignore_account     = TRUE;
  var $attributes         = array();
  var $objectclasses      = array();
 
  var $mirrors            = array(); // The possible mirror,release and section combinations
  var $servers            = array(); // All available server
  var $sections           = array(); // All sections
  var $releases           = array(); // Releases 

  var $FAIdebianRelease   = "";     // The selected release
  var $FAIdebianSection   = array();     // selected section
  var $FAIdebianMirror    = "";     // selected mirror


  var $obj;

  function faiPackageNew ($config, $dn= NULL,$mirrors,$servers,$sections,$releases)
  {
    plugin::plugin ($config, $dn);
    $this->mirrors  = $mirrors;
    $this->servers  = $servers;
    $this->sections = $sections;
    $this->releases = $releases;

    foreach($this->sections as $sec){
      $this->sections[$sec]  =false;
    }
  }

  function execute()
  {
    /* Fill templating stuff */
    $smarty     = get_smarty();
    $display = "";

    // reset grayout options 
    $disableRelease   = false;
    $disableMirror    = false;
    $disableContinue  = false;
    $disableSection   = false;

    /* Post handling for Section, Release and Mirror Setup */
    if(isset($_POST['SetRepository'])){
      if((isset($_POST['FAIdebianReleaseS']))&&(!empty($_POST['FAIdebianReleaseS']))){
        $this->FAIdebianRelease = $_POST['FAIdebianReleaseS'];
      }
      if((isset($_POST['FAIdebianMirrorS']))&&(!empty($_POST['FAIdebianMirrorS']))){
        $this->FAIdebianMirror = $_POST['FAIdebianMirrorS'];
      }

      foreach($this->sections as $sec => $val){
        if(isset($_POST[$sec])){
          $this->sections[$sec]=true;
          $this->FAIdebianSection[$sec]=$sec;
        }else{
          $this->sections[$sec]=false;
        }
      }
    
    }

    /* Grayout settings */    
    if(empty($this->FAIdebianRelease)){
      $disableSection = $disableMirror  = $disableContinue = true;
    }elseif(empty($this->FAIdebianSection)){
      $disableRelease = $disableMirror  = $disableContinue = true;
    }elseif(empty($this->FAIdebianMirror)){
      $disableRelease = $disableSection = $disableContinue = true;
    }else{
      $disableRelease = $disableSection =  true;
    }

    /* check servers matching release and section settings */
    $availableServer = array();
    foreach($this->mirrors as $mir){
      if($mir['release'] == $this->FAIdebianRelease){
        $pass = true;
        foreach($this->FAIdebianSection as $sec){
          if($sec != $mir['section']){
            $pass = false;
          }
        if($pass){
            $availableServer[$mir['mirror']]=$mir['mirror'];
          }
        }
      }
    }

    /* If no servers matching the settings, allow setting release and section again */
    if(($disableMirror==false)&&(count($availableServer)==0)){
      $this->FAIdebianRelease = $this->FAIdebianMirror = "";
      $this->FAIdebianSection = array();
      $disableSection = $disableMirror  = $disableContinue = true;
      $disableRelease = false;
    }

    /* Create checkboxes for the avaliable sections */
    $strsec = "<table><tr>";
    foreach($this->sections as $sec => $val){

      /* Only add this section if it is supported by the selected release */
      $goon = false;
      foreach($this->mirrors as $mir){

        /* Is there a release with this section ? */
        if((($mir['release'] == $this->FAIdebianRelease )&&($mir['section'] == $sec))){
          $goon = true; 
        }
      }

      if($goon){
        if($this->sections[$sec] == true ){
          $chk = " checked ";
        }else{
          $chk = "";
        }

        /* Add disabled if needed */
        if($disableSection){
          $strsec .= "<td><input type='checkbox' name='".$sec."' disabled ".$chk.">".$sec."</td>";
        }else{
          $strsec .= "<td><input type='checkbox' name='".$sec."' ".$chk.">".$sec."</td>";
        }
      }
    }
    $strsec .= "</table>";
    
    /* Add --- to mirror selectbox if it is empty */
    if(count($availableServer)==0){
      $availableServer=array("---");
    }

    /* Tell smarty the grayout settigs */
    $smarty->assign("ReleaseACL"  , "");
    $smarty->assign("MirrorACL"   , "");
    $smarty->assign("ContinueACL" , "");
    $smarty->assign("SectionACL"  , "");

    if($disableContinue){
      $smarty->assign("ContinueACL"," disabled ");
    }
    if($disableRelease){
      $smarty->assign("ReleaseACL"," disabled ");
    }
    if($disableMirror){
      $smarty->assign("MirrorACL"," disabled ");
    }
    if($disableSection){
      $smarty->assign("SectionACL"," disabled ");
    }

    $smarty->assign("mirrors" ,$availableServer);
    $smarty->assign("mirror"  ,$this->FAIdebianMirror);
    $smarty->assign("releases",$this->releases);
    $smarty->assign("release" ,$this->FAIdebianRelease);
    $smarty->assign("sections",$strsec);
    $smarty->assign("section" ,$this->FAIdebianSection);

    $display.= $smarty->fetch(get_template_path('faiPackageNew.tpl', TRUE));
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
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
