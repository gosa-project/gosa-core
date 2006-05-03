<?php

class faiSummaryTab extends plugin{

  var $Classes = array();
  var $Release = "";
  var $Releases= array();
  var $Result  = array();
  var $objs	 = array();
  var $ObjectList = "";

  var $FAIclass;
  var $attributes = array("FAIclass");

  function faiSummaryTab($config,$dn)
  {
    plugin::plugin($config,$dn);

    $this->objs = array(	"FAIscript" 		=> array(	"Image"	=>"images/fai_script.png",
          "Tree"	=>"ou=scripts,") ,
        "FAIhook" 			=> array(	"Image"	=>"images/fai_hook.png" ,
          "Tree"	=>"ou=hooks,"),
        "FAIvariable" 		=> array(	"Image"	=>"images/fai_variable.png",
          "Tree"	=>"ou=variables,") ,
        "FAItemplate" 		=> array(	"Image"	=>"images/fai_template.png",
          "Tree"	=>"ou=templates,") ,
        "FAIpartitionTable" => array(	"Image"	=>"images/fai_partitionTable.png",
          "Tree"	=>"ou=disk,") ,
        "FAIpackageList" 	=> array(	"Image" =>"images/fai_packages.png",
          "Tree"	=>"ou=packages,") ,
        "FAIprofile" 		=> array(	"Image"	=>"images/fai_profile.png",
          "Tree"	=>"ou=profiles,"));

    if(count($this->FAIclass)){


      /* Set Release */
      if(preg_match("/:/",$this->FAIclass)){
        if(preg_match("/:/",$this->FAIclass)){
          $this->Release =trim( preg_replace("/^.*:/","",$this->FAIclass));
        }
      }

      /* Get classes */
      $tmp = split("\ ", preg_replace("/:.*$/","",$this->FAIclass));
      foreach($tmp as $class){
        if(!empty($class)){
          $this->Classes[]= trim($class);
        }
      }
    }

    $_SESSION['faiSummaryTab']['UniqueID'] = 0; 

    $this->Releases = array_flip($this->getBranches());

    if(count($this->Classes) && (!empty($this->Release))){
      foreach($this->Classes as $class){
        $this->Result[$class] = $this->resolveObject($class);
      }
    }
  }

  /* returns an array with all possibly release types */
  function getBranches($base = false,$prefix = "")
  {
    $ret = array("main"=>"/");
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $faifilter = $_SESSION['faifilter'];
    if(!$base){
      $base= "ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
    }
    $ldap->cd($base);
    $ldap->ls("(objectClass=FAIbranch)",$base);
    while($attrs = $ldap->fetch()){
      if((isset($attrs['ou']))&&($attrs['dn']!=$base)){
        $ret[$attrs['dn']]= $prefix.$attrs['ou'][0];
        $ret = array_merge($ret,$this->getBranches($attrs['dn'],"".$prefix.$attrs['ou'][0]."/"));
      }
    }
    return($ret);
  }


  function execute() 
  {
    $display = "";	
    $smarty = get_smarty();
    $this->ObjectList = $this->createSummary($this->Result);
    $smarty->assign("objectList",$this->ObjectList);
    $display.= $smarty->fetch (get_template_path('faiSummary.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }

  /* Create output of our resolved objects */
  function createSummary($data)
  {
 
    /* Don't generate any output, if there is no result */ 
    $str ="<table style='width:100%;background:#F5F5F5;'> ";
    if(!count($this->Result)){
      $str.="<tr><td>"; 
      $str.= (_("This object has no FAI classes assigned."));
      $str.= "</tr></td></table>";
      return($str);
    }

    /* walk through all classes */	
    foreach($data as $key => $entry){

      /* Check every kind of object type */
      foreach($this->objs as $ok => $obj){

        /* check of objectType is used */
        if(isset($entry[$ok])){
          $str.="<tr><td style='padding-left:10px;'>";

          /* Show open or close icon */	
          if( ! $entry[$ok]['Open'] ){
            $str .= "<input type='image' class='center' src='images/expand.png' name='open_".$entry[$ok]['id']."'>&nbsp;";
            $str .="<img src='".$obj['Image']."'>&nbsp;";
            $str .= "<b>".$entry[$ok]['Name']."</b>";
            $str .= "&nbsp;".$entry[$ok]['String1'];
          }else{
            $str .= "<input type='image' class='center' src='images/sort_down.png' name='close_".$entry[$ok]['id']."'>&nbsp;";
            $str .="<img src='".$obj['Image']."'>&nbsp;";
            $str .= "<b>".$entry[$ok]['Name']."</b>";
            $str .= "&nbsp;".$entry[$ok]['String1'];
            if(!empty($entry[$ok]['String2'])){
              $str .= "</td></tr><tr><td style='padding-left:50px;'>".$entry[$ok]['String2'];
            }

            if(isset($data[$key][$ok]['data'])){
              $str.=$this->createSummary($data[$key][$ok]['data']);
            }
          }
          $str .="</td></tr>";
        }
      }
    }
    $str .="</table>";	
    return($str);
  }

  /* resolve specified object to append it to our object tree
     calls prepare_"FAIclass"
   */
  function resolveObject($class)
  {
    $ldap 	= $this->config->get_ldap_link();
    $dn 	= $this->Releases [$this->Release];

    $res = array();

    foreach($this->objs as $key =>  $obj){
      $ldap->ls("(&(objectClass=".$key.")(cn=".$class."))",$obj['Tree'].$dn,array("*"));
      while($attrs = $ldap->fetch()){
        switch($key){
          case "FAIprofile": 		$tmp = $this->prepare_FAIprofile($attrs);break;
          case "FAIscript": 		$tmp = $this->prepare_FAIscript($attrs);break;
          case "FAIhook": 		$tmp = $this->prepare_FAIhook($attrs);break;
          case "FAIvariable":		$tmp = $this->prepare_FAIvariable($attrs);break;
          case "FAItemplate":		$tmp = $this->prepare_FAItemplate($attrs);break;
          case "FAIpackageList":	$tmp = $this->prepare_FAIpackageList($attrs);break;
          case "FAIpartitionTable":	$tmp = $this->prepare_FAIpartitionTable($attrs);break;
          default : $tmp = array(); print_red(sprintf(_("Undefined FAI object '%s'."),$key));
        }
        if(isset($tmp['Name'])){
          $res[$key] = $tmp 	;
        }
      }
    }
    return($res);
  }

  /* Prepare fai script */
  function prepare_FAIscript($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* get subentries */	
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(&(objectClass=FAIscriptEntry)(FAIpriority=*))",array("cn","description","FAIpriority"));
    $scripts  = array();

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      $scripts[$attrs['FAIpriority'][0]][] = $attrs;	
    }

    /* Sort by priority */
    ksort($scripts);

    /* append them to our list */	
    $image = "<image class='center' border=0 src='".$this->objs['FAIscript']['Image']."'>&nbsp;";

    foreach($scripts as $prio => $script){
      foreach($script as $attrs){
        $res['String2'] .="<a href='?plug=".$_GET['plug']."&display=FAIscript&id=".base64_encode($attrs['dn'])."'>";
        if(isset($attrs['description'][0])){
          $res['String2'] .= $image.$attrs['FAIpriority'][0]."&nbsp;-&nbsp;".$attrs['cn'][0]." [".$attrs['description'][0]."]<br>";			
        }else{
          $res['String2'] .= $image.$attrs['FAIpriority'][0]."&nbsp;-&nbsp;".$attrs['cn'][0]."<br>";			
        }
        $res['String2'] .="</a>";
      }
    }
    return($res);
  }

  /* Prepare fai script */
  function prepare_FAIpartitionTable($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* get subentries */	
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIpartitionDisk)",array("cn","description"));
    $disks = array();

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      $disks[$attrs['cn'][0]] = $attrs;	
      $disks[$attrs['cn'][0]]['partitions'] = array();	
    }

    /* Sort by priority */
    foreach($disks as $key => $disk){
      $ldap->cd($disk['dn']);
      $ldap->search("(objectClass=FAIpartitionEntry)",array("*"));
      while($attrs = $ldap->fetch()){
        if(!isset($attrs['FAIfsOptions'][0])){
          $attrs['FAIfsOptions'][0] = "";
        }
        $disks[$key]['partitions'][$attrs['cn'][0]] = $attrs;
      }
    }

    
    $image = "<image class='center' border=0 src='".$this->objs['FAIpartitionTable']['Image']."'>&nbsp;";
    foreach($disks as $disk){
      $res['String2'] .= "<div 'width:100%;padding-left:10px'>";
      $res['String2'] .= $image."<b>".$disk['cn'][0] ."</b>";
      
      if(isset($disk['description'][0])){
        $res['String2'] .= "&nbsp;[".$disk['description'][0]."]" ;
      }
      $res['String2'] .= "</div>";
  
      $res['String2'] .= "<table cellspacing=1 style='background:#FFFFFF;'>";
      $res['String2'] .= "<tr style='background-color:#BBBBBB;'>";
      $res['String2'] .= "<td>"._("No.")."</td><td>"._("Name")."</td><td>"._("FS options")."</td>";
      $res['String2'] .= "<td>"._("Mount options")."</td><td>"._("Size in MB")."</td>";
      $res['String2'] .= "<td>"._("Mount point")."</td><td>"._("Type")."</td>";
      $res['String2'] .= "</tr>";
      $i = 0;
      foreach($disk['partitions'] as $partition){
        $i = !$i;
        if($i){
          $c = " class='rowxp1'";
          $d = " class='list1' style='border:0px;'";
        }else{
          $c = " class='rowxp0'";
          $d = " class='list0' style='border:0px;'";
        }

        $res['String2'] .= "<tr $c style='height:10px;'>";
        $res['String2'] .= "<td $d>".$partition['FAIpartitionNr'][0]."</td><td $d>".$partition['cn'][0]."</td><td $d>".$partition['FAIfsType'][0]."</td>";
        $res['String2'] .= "<td $d>".$partition['FAIfsOptions'][0]."</td><td $d>".$partition['FAIpartitionSize'][0]."</td>";
        $res['String2'] .= "<td $d>".$partition['FAImountPoint'][0]."</td><td $d>".$partition['FAIpartitionType'][0]."</td>";
        $res['String2'] .= "</tr>";
      }
      $res['String2'] .= "</table>";
    }

    return($res);
  }


  /* Create template entry */	
  function prepare_FAItemplate($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* get subentries */	
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAItemplateEntry)",array("cn","description"));
    $scripts  = array();

    /* append them to our list */	
    $image = "<image class='center' border=0 src='".$this->objs['FAItemplate']['Image']."'>&nbsp;";

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      $res['String2'] .="<a href='?plug=".$_GET['plug']."&display=FAItemplate&id=".base64_encode($attrs['dn'])."'>";
      if(isset($attrs['description'][0])){
        $res['String2'] .= $image.$attrs['cn'][0]." [".$attrs['description'][0]."]";			
      }else{
        $res['String2'] .= $image.$attrs['cn'][0];			
      }
      $res['String2'] .="</a><br>";
    }
    return($res);
  }


  /* Create a package list  */	
  function prepare_FAIpackageList($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* append them to our list */	
    $image = "<image class='center' border=0 src='".$this->objs['FAIpackageList']['Image']."'>&nbsp;";

    /* add selected packages to our list */		
    if(isset($data['FAIpackage']['count'])) {	
      unset($data['FAIpackage']['count']);
      foreach($data['FAIpackage'] as $entry){
        $res['String2'].= $image.$entry." <br>";
      }	
    }
    return($res);
  }


  /* Create a variable entry */	
  function prepare_FAIvariable($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* get subentries */	
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIvariableEntry)",array("cn","description","FAIvariableContent"));
    $scripts  = array();

    /* append them to our list */	
    $image = "<image class='center' border=0 src='".$this->objs['FAIvariable']['Image']."'>&nbsp;";

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      if(isset($attrs['description'][0])){
        $res['String2'] .= $image.$attrs['cn'][0]." [".$attrs['description'][0]."]";			
      }else{
        $res['String2'] .= $image.$attrs['cn'][0];			
      }
      if(isset($attrs['FAIvariableContent'][0])){
        $res['String2'] .= " \"".$attrs['FAIvariableContent'][0]."\" ";
      }
      $res['String2'] .= "<br>";

    }
    return($res);
  }

  /* Create a hook entry */	
  function prepare_FAIhook($data)
  {
    /* Set basic attributes */
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* get subentries */	
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIhookEntry)",array("cn","description"));
    $scripts  = array();

    /* append them to our list */	
    $image = "<image class='center' border=0 src='".$this->objs['FAIhook']['Image']."'>&nbsp;";

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      $res['String2'] .="<a href='?plug=".$_GET['plug']."&display=FAIhook&id=".base64_encode($attrs['dn'])."'>";
      if(isset($attrs['description'][0])){
        $res['String2'] .= $image.$attrs['cn'][0]." [".$attrs['description'][0]."]<br>";			
      }else{
        $res['String2'] .= $image.$attrs['cn'][0]."<br>";			
      }
      $res['String2'] .="</a>";
    }
    return($res);
  }

  /* Create a new Profile entry */
  function prepare_FAIprofile($data)
  {
    /* initialise basics */
    $res = array();
    $res['Name']	= $data['cn'][0];
    if(isset($data['description'][0])){
      $res['String1'] = $data['description'][0];
    }else{
      $res['String1'] = "";
    }
    $res['Open'] 	= false;
    $res['id']		= ($_SESSION['faiSummaryTab']['UniqueID'] ++);
    $res['Resolved']= true;
    $res['String2'] = "";

    /* Append empty data objects named by member classes */
    $res['data'] = array();
    if(isset($data['FAIclass'][0])){
      $tmp = split("\ ",$data['FAIclass'][0]);
      foreach($tmp as $class){
        $res['data'][trim($class)] = array();
      }
    }
    return($res);
  }



  /* Open the tag with the given id .. 
     so the entries of this tag will be shown */
  function openTag($data, $id)
  {
    /* Walk through objects */
    if(!count($data)) return(array());
    foreach($data as $key => $dat){

      /* Check all types of objects (FAIhook, FAIscript ..)*/
      foreach($this->objs as $ok => $obj){

        /* If this type of object is assigned */
        if(isset($dat[$ok])){

          /* check given id */
          if($dat[$ok]['id'] == $id ){

            /* Id was $id so set this leaf to open */
            $data[$key][$ok]['Open'] = true;

            /* Check if the sub leafs are already readed 
               if not, resolve the classes and append them to our tree */
            if(isset($dat[$ok]['data'])){

              foreach($data[$key][$ok]['data'] as $key1 => $dat1){
                if(!count($dat1)){
                  $tmp = $this->resolveObject($key1);
                  foreach($tmp as $r => $e)
                    $data[$key][$ok]['data'][$key1][$r] = $e;
                }
              }
            }
          }
          /* Search sub tags for given id $id */
          if(isset($data[$key][$ok]['data'])){
            $data[$key][$ok]['data'] = $this->openTag($data[$key][$ok]['data'],$id);	
          }
        }
      }
    }
    return($data);
  }

  /* Walk through all tags and close tag with id = $id*/
  function closeTag($data, $id)
  {
    $found = false;

    /* The parameter data is given as specified */
    // ClassName ['FAIprofile']['data']  

    /* Walk through all given classes */
    foreach($data as $key => $dat){

      /* Walk through all possible objectClasses */
      foreach($this->objs as $ok => $obj){

        /* If class is set, check if id matches */
        if(isset($dat[$ok])){
          if($dat[$ok]['id'] == $id ){
            $data[$key][$ok]['Open'] = false;
          }

          /* Check subobjects for $id */
          if(isset($dat[$ok]['data'])){
            $data[$key][$ok]['data'] = $this->closeTag( $data[$key][$ok]['data'], $id);
          }
        }
      }
    }
    return($data);
  }

  /* Capture posts, and call opnen close tag */
  function save_object()
  {
    $once = true;
    foreach($_POST as $name => $value ){

      /* check for open tag request */
      if(preg_match("/^open_/",$name) && $once){
        $once  = false;
        $value = preg_replace("/^open_/","",$name);
        $value = preg_replace("/_.*$/","",$value); 
        $this->Result = $this->openTag($this->Result,$value);
      }
      /* Check for close tag request */
      if(preg_match("/^close_/",$name) && $once){
        $once  = false;
        $value = preg_replace("/^close_/","",$name);
        $value = preg_replace("/_.*$/","",$value); 
        $this->Result = $this->closeTag($this->Result,$value);
      }
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
