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
 
  var $sections           = array(); // All sections
  var $releases           = array(); // Releases 

  var $FAIdebianRelease   = "";           // The selected release
  var $FAIdebianSection   = array();      // selected section

  var $obj;

  function faiPackageNew ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
  }

  function execute()
  {
	  /* Call parent execute */
	  plugin::execute();
    $display = "";
    $smarty = get_smarty(); 
    $atr = $this->getServerInfos(); 
    foreach($atr as $release => $sections){
      $releases[$release]=$release;
    }
    if(!empty($this->FAIdebianRelease)){
      $sections = $atr[$this->FAIdebianRelease] ;
    }else{
      $sections = array();
    }

    /* Fill templating stuff */
    if((empty($this->FAIdebianRelease))||(count($this->FAIdebianSection)==0)){
      $smarty->assign("goon"," disabled ");
    }else{
      $smarty->assign("goon"," ");
    }

    $smarty->assign("releases"    ,$releases);
    $smarty->assign("release" ,$this->FAIdebianRelease);
    $smarty->assign("sections",$this->generateSections($sections));

    $display.= $smarty->fetch(get_template_path('faiPackageNew.tpl', TRUE));
    return($display);
  }

  function generateSections($secs)
  {
    $str = "";
    if(is_array($secs)){
      sort($secs);
      $secs= array_unique($secs);
      foreach($secs as $section){
        if(in_array($section,$this->FAIdebianSection)){
          $str .= "<input type='checkbox' value='".$section."' name='section_".$section."' checked>".$section."<br>";
        }else{
          $str .= "<input type='checkbox' value='".$section."' name='section_".$section."'>".$section."<br>";
        }
        $str .="<input type='hidden' name='wasOnPage_".$section."'>";
      }
    }
    return($str); 
  }

  function getServerInfos()
  {
    $ret = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=FAIrepositoryServer)",array("*"));
    while($attrs = $ldap->fetch()){
      if(isset($attrs['FAIrepository'])){
        for($i =0 ; $i < $attrs['FAIrepository']['count']; $i++){
          $obj = $attrs['FAIrepository'][$i];
          $tmp = split("\|",$obj);
          if(count($tmp)==4){
            foreach(split(",",$tmp[3]) as $sec){
              if(!empty($sec)){
                $ret[$tmp[2]][] =  $sec;
              }
            }
          }
        } 
      }
    }
    return($ret);
  }


  /* Save data to object */
  function save_object()
  {
    /* Post handling for Section, Release  Setup */
    if(isset($_POST['YesItIsPostedPackages'])){

      if((isset($_POST['FAIdebianReleaseS']))&&(!empty($_POST['FAIdebianReleaseS']))){
        $this->FAIdebianRelease = $_POST['FAIdebianReleaseS'];
      }
        
      foreach($_POST as $name => $value){
        if(preg_match("/wasOnPage_/",$name)){
          $nm = preg_replace("/wasOnPage_/","",$name);
          if(isset($_POST['section_'.$nm])){
            $this->FAIdebianSection[$nm] = $nm; 
          }else{
            if(isset($this->FAIdebianSection[$nm])){
              unset($this->FAIdebianSection[$nm]);
            }
          }
        }
      }
    }
    /* Remove all section that are not available at the currently selected server */ 
    $atr = $this->getServerInfos();
    foreach($this->FAIdebianSection as $section){
      if(!in_array($section,$atr[$this->FAIdebianRelease])){
        unset($this->FAIdebianSection[$section]);
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
    return(array("FAIdebianRelease"=>$this->FAIdebianRelease,"FAIdebianSection"=>$this->FAIdebianSection));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
