<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) Fabian Hickert 

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class faiSummaryTab extends plugin{

  var $Classes        = array();
  var $Release        = "";
  var $Releases       = array();
  var $Result         = array();
  var $objs	          = array();
  var $ObjectList     = "";
  var $is_dialog      = false;
  var $InitCalled     = false;
  var $usedClasses    = array();
  
  function faiSummaryTab($config,$dn)
  {
    plugin::plugin($config,$dn);

    /* Base object configuration */
    $this->objs = array(	
        "FAIscript" 		    => array(	"Image"	=>"images/fai_script.png",        "Name"=>_("Scripts")      ,"Tree"	=>"ou=scripts,") ,
        "FAIhook" 			    => array(	"Image"	=>"images/fai_hook.png" ,         "Name"=>_("Hooks")        ,"Tree"	=>"ou=hooks,"),
        "FAIvariable" 		  => array(	"Image"	=>"images/fai_variable.png",      "Name"=>_("Variables")    ,"Tree"	=>"ou=variables,") ,
        "FAItemplate" 		  => array(	"Image"	=>"images/fai_template.png",      "Name"=>_("Templates")    ,"Tree"	=>"ou=templates,") ,
        "FAIpartitionTable" => array(	"Image"	=>"images/fai_partitionTable.png","Name"=>_("Partition table")  ,"Tree"	=>"ou=disk,") ,
        "FAIpackageList" 	  => array(	"Image" =>"images/fai_packages.png",      "Name"=>_("Package list") ,"Tree"	=>"ou=packages,") ,
        "FAIprofile" 		    => array(	"Image"	=>"images/fai_profile.png",       "Name"=>_("Profiles")     ,"Tree"	=>"ou=profiles,"));
  }


  /* Init the fai tree with objects.
     If this method is called, all assigned classes and the release name 
      will be read from parent object. */
  function init()
  {
    $this->InitCalled = true;
    $this->Result = array();
    $_SESSION['faiSummaryTab']['UniqueID'] = 0; 
    $tmp = $this->getBranches();
    
    /* Get classes & release name 
       There are 2 different tabs which use the summary tab
       faiProfile / Workstations */
    
    /* Check if given parent object is fai profile tabs */
    if(isset($this->parent->by_name['faiProfile'])){
      
      /* Get Classes and release */
      $this->Classes = $this->parent->by_object['faiProfile']->FAIclasses;
      $str = preg_replace("/^.*ou=profiles,/","",$this->dn);
      if(isset($tmp[$str])){
        $this->Release = $tmp[$str];
      }
  
    /* Check if parent tab is work tab */
    }elseif(isset($this->parent->by_name['workstartup'])){
      $this->Classes = $this->parent->by_object['workstartup']->FAIclass;
      $this->Release = $this->parent->by_object['workstartup']->FAIrelease;
      $tmp = array_flip($tmp);
      if(!isset($tmp[$this->Release])) {
        $this->Release = "";
      }
    }

    /* Set all available releases */
    $this->Releases = array_flip($this->getBranches());
    $this->usedClasses = array();

    /* Check the configuration */
    if(count($this->Classes) && (!empty($this->Release))){
      foreach($this->Classes as $class){
        $this->resolveObject($class);
      }
    }
  }


  /* Capture posts, and call opnen close tag */
  function save_object()
  {
    $once = true;

    if(isset($_GET['expand']) && isset($_GET['id'])){
      if(isset($this->Result[$_GET['id']])){
        $this->Result[$_GET['id']]['Open'] =true  ;
      }
    }

    if(isset($_GET['compress']) && isset($_GET['id'])){
      if(isset($this->Result[$_GET['id']])){
        unset($this->Result[$_GET['id']]['Open']);
      }
    }

    foreach($_POST as $name => $value ){

      /* check for open tag request */
      if(preg_match("/^open_/",$name) && $once){
        $once  = false;
        $value = preg_replace("/^open_/","",$name);
        $value = preg_replace("/_.*$/","",$value); 
        $this->Result[$value]['Open'] =true  ;
      }
      /* Check for close tag request */
      if(preg_match("/^close_/",$name) && $once){
        $once  = false;
        $value = preg_replace("/^close_/","",$name);
        $value = preg_replace("/_.*$/","",$value); 
        unset($this->Result[$value]['Open']);
      }
      if(preg_match("/^reloadList/",$name)){
        $this->InitCalled = false;
      }
    }
  }


  /* returns an array with all possibly release types */
  function getBranches($base = false,$prefix = "")
  {
    $ret = array("main"=>"/");
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
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


  /* This function calls all nescessary functions to generate the fai class summary tree */
  function execute() 
  {
    /* Check if we must (re)init the this tab */
    if(!$this->InitCalled){
      $this->init();
    }

    if(isset($_GET['show'])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cat(base64_decode($_GET['id']), array('FAIscript', 'FAIhook', 'FAItemplate'));
      $attrs = $ldap->fetch();
      if($_GET['type'] == "FAIscript"){
        $str = $attrs['FAIscript'][0];
      }elseif($_GET['type'] == "FAIhook"){
        $str = $attrs['FAIscript'][0];
      }elseif($_GET['type'] == "FAItemplate"){
        $str = $attrs['FAItemplateFile'][0];
      }
      echo nl2br($str);
      exit();
    }

    /* Get smarty class & assign created summary results */
    $smarty = get_smarty();
    $this->ObjectList = $this->createSummary($this->Result);
    $smarty->assign("objectList",$this->ObjectList);
    return($smarty->fetch (get_template_path('faiSummary.tpl', TRUE, dirname(__FILE__))));
  }


  /* Create output of from resolved fai objects */
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
      
      $image = "<img src='".$this->objs[$key]['Image']."' class='center' alt='".$this->objs[$key]['Name']."' border=0>";
      if(!isset($entry['Open'])){
        $str .= "<tr><td colspan=2>
                    <a href='?plug=".$_GET['plug']."&expand&id=".$key."'>
                  <img border=0 class='center' src='images/expand.png' alt='"._("Open")."'>&nbsp;";
        $str .= $image."&nbsp;";
        $str .= "<b>".$this->objs[$key]['Name']."</b></b></td></tr>";
      }else{
        $str .= "<tr><td colspan=2>
                    <a href='?plug=".$_GET['plug']."&compress&id=".$key."'>
                   <img border=0 class='center' src='images/sort_down.png' alt='"._("Close")."'>&nbsp;";
        $str .= $image."&nbsp;";
        $str .= "<b>".$this->objs[$key]['Name']."</b></a></td></tr>";
   
        /* Display FAItemplate FAIhook FAIscript entries */ 
        if(in_array($key,array("FAItemplate","FAIhook","FAIscript"))){

          $nums = array();          

          if($key == "FAIscript"){
            foreach($entry['Entries'] as $scripts){
              foreach($scripts as $script){
                
                if(!isset($nums[$script['cn'][0]])){
                  $nums[$script['cn'][0]]= 0;
                } 
                $nums[$script['cn'][0]] ++;
            
                $tmp[$script['FAIpriority'][0].$script['cn'][0].$script['CLASS']] = $script;
              }
            }
            krsort($tmp);;
            
            $entry['Entries'] = $tmp;
          }else{
            $tmp = array();
            foreach($entry['Entries'] as $script){
              $tmp[$script['cn'][0].$script['CLASS']] = $script;
              if(!isset($nums[$script['cn'][0]])){
                $nums[$script['cn'][0]]= 0;
              } 
              $nums[$script['cn'][0]] ++;
            }
             ksort($tmp);
            $entry['Entries'] = $tmp;
          }

          foreach($entry['Entries'] as $cn => $data){

            if(isset($nums[$data['cn'][0]]) && ($nums[$data['cn'][0]] > 1)){
              $str .="<tr style='background: #EE3434;'><td style='padding-left:20px;'>";
            }else{
              $str .="<tr><td style='padding-left:20px;'>";
            }

            $str .= "<a target='_blank' href='?plug=".$_GET['plug']."&show&type=".$key."&id=".base64_encode($data['dn'])."'>";
            $str .= $image."&nbsp;";
            if(isset($data['FAIpriority'][0])){
              $str .= "(".$data['FAIpriority'][0].")";
            }

            $str .= "&nbsp;".$data['cn'][0];
            if(isset($data['description'][0])){
              $str .= " [".$data['description'][0]."]";
            }
            $str .= "</a>";
            $str .= "</td><td><i>".$data['CLASS']."</i>";
            $str .= "</td></tr>";
          }
        }

        /* Create variable entries */
        if(in_array($key,array("FAIvariable"))) {
          foreach($entry['Entries'] as $cn => $data){
            $str .="<tr><td style='padding-left:20px;'>";
            $str .= $image."&nbsp;".$data['cn'][0];
            if(isset($data['description'][0])){
              $str .= " [".$data['description'][0]."] ";
            }
            $str .=" = '".$data['FAIvariableContent'][0]."'";
            $str .= "</td><td><i>".$data['CLASS']."</i>";
            $str .="</td></tr>";
          }
        }

        /* Create packagelist entries */ 
        if(in_array($key,array("FAIpackageList"))) {
          ksort($entry['Entries']);
          foreach($entry['Entries'] as $data){
            $str .="<tr><td style='padding-left:20px;'>";
            $str .= $image."&nbsp;".$data['cn'][0];
            $str .= "</td><td><i>".$data['CLASS']."</i>";
            $str .= "</td></tr>";;
          }
        }

        /* Create partition table entries */
        if(in_array($key,array("FAIpartitionTable"))) {
          foreach($entry['Entries'] as $cn => $data){
            $str .= "<tr><td style='padding-left:20px;'>";
            $str .= $image."&nbsp;".$data['name'];
            $str .= "</td><td><i>".$data['CLASS']."</i>";
            $str .= "</td></tr>";
            ksort($data['partitions']);

            $str .= "<tr><td colspan=2  style='padding-left:20px;'>";
            $str .= "<table cellspacing=0 cellpadding=2 style='background:#FFFFFF;border:1px solid #B0B0B0'>
                      <tr style='background-color:#DBDBDB;'>
                       <td class='tbhead'>"._("No.")."</td>
                       <td class='tbhead'>"._("Name")."</td>
                       <td class='tbhead'>"._("FS options")."</td>
                       <td class='tbhead'>"._("Mount options")."</td>
                       <td class='tbhead'>"._("Size in MB")."</td>
                       <td class='tbhead'>"._("Mount point")."</td>
                       <td class='tbrhead'>"._("Type")."</td>
                      </tr>";

            $i = 0;
            foreach($data['partitions'] as $key => $part){
              $i ++;

              if($i%2 == 1){
                $c = " class='rowxp1'";
                $d = " class='list1'";
              }else{
                $c = " class='rowxp0'";
                $d = " class='list0'";
              }

              $str.="<tr $c style='height:10px;'>
                      <td $d>".$i."</td> 
                      <td $d>".$part['cn'][0]."</td> 
                      <td $d>".$part['FAIfsType'][0]."</td> 
                      <td $d>".$part['FAIfsOptions'][0]."</td> 
                      <td $d>".$part['FAIpartitionSize'][0]."</td> 
                      <td $d>".$part['FAImountPoint'][0]."</td> 
                      <td $d style='border:none'>".$part['FAIpartitionType'][0]."</td> 
                    </tr>";
            }
            $str .="</table>";
          }
        }
      }
    }
    $str .="</table>";	
    return($str);
  }


  /* resolve specified object to append it to our object tree */
  function resolveObject($class)
  {
    $ldap 	= $this->config->get_ldap_link();
    $dn 	  = $this->Releases [$this->Release];
    $res    = array();
 
    /* Try to fetch all types of fai objects with the given cn  */
    foreach($this->objs as $key =>  $obj){
      
      /* Check for objects with cn=$class 
          and add them to our list */
      $ldap->ls("(&(objectClass=".$key.")(cn=".$class."))",$obj['Tree'].$dn,array("*"));
      while($attrs = $ldap->fetch()){

        if(!isset($this->usedClasses[$key][$class])){
          $this->usedClasses[$key][$class] = true;
          switch($key){
            case "FAIprofile": 		        $this->prepare_FAIprofile($attrs);break;
            case "FAIscript": 		        $this->prepare_FAIscript($attrs);break;
            case "FAIhook": 		          $this->prepare_FAIhook($attrs);break;
            case "FAIvariable":		        $this->prepare_FAIvariable($attrs);break;
            case "FAItemplate":		        $this->prepare_FAItemplate($attrs);break;
            case "FAIpackageList":	      $this->prepare_FAIpackageList($attrs);break;
            case "FAIpartitionTable":	    $this->prepare_FAIpartitionTable($attrs);break;
          }
        }
      }
    }
    return($res);
  }


  /* Prepare fai script */
  function prepare_FAIscript($data)
  {
    if(isset($this->Result['FAIscript']['Entries'])){
      $current = $this->Result['FAIscript']['Entries'];
    }else{
      $current = array();
    }
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIscriptEntry)",array("cn","description","FAIpriority"));
    while($attrs = $ldap->fetch()){
      $attrs['CLASS'] = $data['cn'][0];
      $current[$attrs['FAIpriority'][0]][] = $attrs;
    }
    $this->Result['FAIscript']['Entries'] = $current ;
  }


  /* Prepare fai script */
  function prepare_FAIpartitionTable($data)
  {
    if(isset($this->Result['FAIpartitionTable']['Entries'])){
      $current = $this->Result['FAIpartitionTable']['Entries'];
    }else{
      $current = array();
    }

    /* get subentries */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIpartitionDisk)",array("cn","description"));
    $disks = array();

    /* Create an array which is sortable by priority */
    while($attrs = $ldap->fetch()){
      $attrs['CLASS'] = $data['cn'][0];
      $disks[$attrs['cn'][0]]= $attrs;
      if(isset($attrs['description'][0])){
        $disks[$attrs['cn'][0]]['name'] = $attrs['cn'][0]." [".$attrs['description'][0]."]";
      }else{
        $disks[$attrs['cn'][0]]['name'] = $attrs['cn'][0];
      }
      $disks[$attrs['cn'][0]]['dn'] = $attrs['dn'];
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
        $attrs['CLASS'] = $data['cn'][0];
        $disks[$key]['partitions'][$attrs['cn'][0]] = $attrs;
      }
    }
    $this->Result['FAIpartitionTable']['Entries'] = $disks;
  }


  /* Create template entry */	
  function prepare_FAItemplate($data)
  {
    $current = array();
    if(isset($this->Result['FAItemplate']['Entries'])){
      $current = $this->Result['FAItemplate']['Entries'];
    }

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAItemplateEntry)",array("cn","description"));
    while($attrs = $ldap->fetch()){
      $attrs['CLASS'] = $data['cn'][0];
      $current[] = $attrs;
    }
    $this->Result['FAItemplate']['Entries'] = $current ;
  }


  /* Create a package list  */	
  function prepare_FAIpackageList($data)
  {
    $current = array();
    if(isset($this->Result['FAIpackageList']['Entries'])){
      $current = $this->Result['FAIpackageList']['Entries'];
    }
    if(isset($data['FAIpackage'])){
      unset($data['FAIpackage']['count']);
      foreach($data['FAIpackage'] as $pkg){
        $attrs['CLASS'] = $data['cn'][0];
        $attrs['cn'][0] = $pkg;
        $current[$pkg] = $attrs;
      }
    }
    $this->Result['FAIpackageList']['Entries'] = $current ;
  }


  /* Create a variable entry */	
  function prepare_FAIvariable($data)
  {
    $current = array();
    if(isset($this->Result['FAIvariable']['Entries'])){
      $current = $this->Result['FAIvariable']['Entries'];
    }
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIvariableEntry)",array("cn","description","FAIvariableContent"));
    while($attrs = $ldap->fetch()){
      $attrs['CLASS'] = $data['cn'][0];
      $current[] = $attrs;
    }
    $this->Result['FAIvariable']['Entries'] = $current ;
  }

  /* Create a hook entry */	
  function prepare_FAIhook($data)
  {
    $current = array();
    if(isset($this->Result['FAIhook']['Entries'])){
      $current = $this->Result['FAIhook']['Entries'];
    }
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($data['dn']);
    $ldap->search("(objectClass=FAIhookEntry)",array("cn","description"));
    while($attrs = $ldap->fetch()){
      $attrs['CLASS'] = $data['cn'][0];
      $current[$attrs['cn'][0]] = $attrs;
    }
    $this->Result['FAIhook']['Entries'] = $current ;
  }


  /* Create a new Profile entry */
  function prepare_FAIprofile($data)
  {
    $classes = split("\ ",$data['FAIclass'][0]);
    foreach($classes as $class){
      $class = trim($class);
      $this->resolveObject($class);
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
