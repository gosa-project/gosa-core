<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class baseSelectDialog extends MultiSelectWindow
{
  var $selectedBase        = ""; // Used for navigation 
  var $base_selection_regex = "*"; // Used regex ... 
  var $BaseToUse         = false;  // used to specify the selected base, 
                                      // false if none is selected

  var $allowedBases         = array();
  var $parent               = NULL;
 
  function baseSelectDialog (&$config,$parent,$onlyAllowThisBases = array())
  {
    $module= "all";
    MultiSelectWindow::MultiSelectWindow($config, "BASEselectWindow", $module);
   
    $this->parent = $parent;
 
    $this->selectedBase = $config->current['BASE'];
    $this->allowedBases  = $onlyAllowThisBases;

    $this->SetTitle("Base");
    $this->SetSummary(_("Choose a base"));
    $this->SetListHeader("<div style='background:#F0F0F9;padding:5px;'>".
        " <input class='center' type='image' src='images/lists/root.png' align='middle'
        title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
        " <input class='center' type='image' align='middle' src='images/lists/back.png'
        title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
        " <input class='center' type='image' align='middle' src='images/lists/home.png'
        title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
        " <input class='center' type='image' src='images/lists/reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
        "</div>");

    $this->SetInformation(_("Step in the prefered tree and click save to use the current subtree as base. Or click the image at the end of each entry."));

    $this->EnableAplhabet   (true);
    $this->EnableCloseButton(true);
    $this->EnableSaveButton (true);

    $this->SetSaveButtonString(_("Use"));
    $this->SetCloseButtonString(_("Cancel"));

    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Base")));
    $this->AddHeader(array("string"=>_("Action"),"attach"=>"style='width:50px;border-right:0px;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
//  $this->AddCheckBox("Doesnothing","servers","Doesnothing",true);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("base_selection_regex",_("Filter entries with this syntax"),"*"      , true);
  }

  function execute()
  {
    $this->setEntries();
    return($this->Draw());
  }

  function setEntries()
  {
    $this->ClearElementsList();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->selectedBase);
    $ldap->ls("(&(objectClass=gosaDepartment)
                 (|(ou=".$this->base_selection_regex.")
                   (cn=".$this->base_selection_regex.")
                   (description=".$this->base_selection_regex.")))",
              $this->selectedBase,array("ou","description","cn"));

    $link = "<a href='?plug=".$_GET['plug']."&open_dep=%s'>%s</a>";

    $base_back          = preg_replace("/^[^,]+,/","",$this->selectedBase);
    $base_back          = convert_department_dn($base_back);
  
    /* Add departments, to be able to switch into them
     */
    while($attrs = $ldap->fetch()){
 
      $key = $attrs['dn']  ;
      $val = $attrs['ou'][0];
    
      if(count($this->allowedBases) != 0){
        if(!isset($this->allowedBases[$key])){
          continue;
          break;
        }
      }
 
      /* Append description */ 
      if(isset($attrs['description'][0])){
        $val.=" [".$attrs['description'][0]."]";
      }

      /* Add missing entries ... */
      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      /* check if this department contains sub-departments
         Display different image in this case
       */
      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd ){
        if(preg_match("/,".$keys."/",$keyd)){
          $non_empty="full";
        }
      }

      /* Add to divlist */
      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", 
                      "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($link,base64_encode($key),$val), "attach" => "style=''");
      $field3 = array("string" => sprintf("&nbsp;<input title='"._("Select this base")."' type='image' img src='images/save.png' name='usebase_%s'>",base64_encode($key)), 
                      "attach" => "style='width:50px;border-right:0px;text-align:right;'");
      $this->AddElement(array($field1,$field2,$field3));
    }
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
    $this->BaseToUse = $this->selectedBase;
  }

  function isSelected() 
  {
    return($this->BaseToUse);
  }

  function setCurrentBase($base)
  {
    $this->selectedBase = $base;
  }

  function save_object()
  {
    /* Save current base */
    $old_base = $this->selectedBase;
  
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object();   
    
    if(isset($_GET['open_dep'])){
      $this->selectedBase = base64_decode($_GET['open_dep']);
    } 

    $s_action ="";
    foreach($_POST as $key => $value){
      if(preg_match("/^dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/^dep_root.*/",$key)){
        $s_action="root";
      }elseif(preg_match("/^dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/^usebase_/",$key)){
        $tmp = preg_replace("/^usebase_/","",$key);
        $tmp = preg_replace("/_.*$/","",$tmp);
        $tmp = base64_decode($tmp);
        $this->BaseToUse = $tmp;
      }
    }

    $ui= get_userinfo();
    /* Homebutton is posted */
    if($s_action=="home"){
      $this->selectedBase=(preg_replace("/^[^,]+,/","",$ui->dn));
      $this->selectedBase=(preg_replace("/^[^,]+,/","",$this->selectedBase));
    }

    /* back to the roots ^^ */
    if($s_action=="root"){
      $this->selectedBase=($this->config->current['BASE']);
    }

    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$this->selectedBase);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $this->selectedBase= $this->config->departments[trim($base_back)];
      }else{
        $this->selectedBase= $this->config->departments["/"];
      }
    }

    /* Restore old base, if selected base is not allowed */
    if(count($this->allowedBases) && !isset($this->allowedBases[$this->selectedBase])){
      $this->selectedBase = $old_base;
    }
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
