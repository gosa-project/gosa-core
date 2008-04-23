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

class divListDepartment extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  
  /* Subsearch checkbox */
  var $SubSearch          = false;

  var $parent             ;
  var $ui                 ;

  function divListDepartment (&$config, &$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Department", "department");
    
    $this->parent       = &$parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of departments"));
    $this->SetSummary(_("List of departments"));
    $this->EnableAplhabet   (true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);



   /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
               onClick='toggle_all_(\"^item_selected_[a-zA-Z0-9\\/\=]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string"=>_("Department name"), "attach" => "style=''"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'"));


    /* Add SubSearch checkbox */    
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching department names"), "*" , true);
  }


  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
    */
    $options= "";

    /* Get all departments within this subtree */
    $ui= get_userinfo();
    $first = "";
    $found = FALSE;
    $base = $this->config->current['BASE'];

    /* Add base */
    $tmp = array();
    $tmp[] = array("dn"=>$this->config->current['BASE']);
    $tmp=  array_merge($tmp,get_list("(&(|(ou=*)(description=*))(objectClass=gosaDepartment))", $this->module, $base,
                    array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH));

    $deps = array();
    foreach($tmp as $tm){
      $deps[$tm['dn']] = $tm['dn'];
    }

    /* Load possible departments */
    $ui= get_userinfo();
    $tdeps= $ui->get_module_departments("department");
    $ids = $this->config->idepartments;
    $first = "";
    $found = FALSE;
    foreach($ids as $dep => $name){
      if(isset($deps[$dep]) && in_array_ics($dep, $tdeps)){

        /* Keep first base dn in mind, we could need this
         *  info if no valid base was found
         */
        if(empty($first)) {
          $first = $dep['dn'];
        }

        $value = $ids[$dep];
        if ($this->selectedBase == $dep){
          $found = TRUE;
          $options.= "<option selected='selected' value='".$dep."'>$value</option>";
        } else {
          $options.= "<option value='".$dep."'>$value</option>";
        }
      }
    }

    /* The currently used base is not visible with your acl setup.
     * Set base to first useable base.
     */
    if(!$found){
      $this->selectedBase = $first;
    }

    /* Generate list head */
    $ui = get_userinfo();
    $acl = $ui->get_permissions("ou=dummy,".$this->selectedBase  ,"department/department");
      
    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    $listhead .=  
      " "._("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/lists/submit.png' align='middle' ".
      "   title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";

    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s.= "...|<input class='center' type='image' src='images/list_new_department' alt=''>".
        "&nbsp;"._("Department")."|dep_new|\n";
    }
    $s.= "..|---|\n";
    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";


    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    $ui = get_userinfo();
    foreach($list as $key => $val) {

      $actions= "";      

      $acl = $ui->get_permissions($this->config->departments[$key] ,"department/department");
      if(preg_match("/r/",$acl)){
        $actions.= "<input class='center' type='image' src='images/lists/edit.png'
          alt='"._("edit")."'   name='dep_edit_%KEY%' title='"._("Edit this entry")."'>";
      }

      if(preg_match("/c/",$acl)){
        $actions.= "<input class='center' type='image' src='images/lists/trash.png'
          alt='"._("delete")."'   name='dep_del_%KEY%' title='"._("Delete this entry")."'>";
      }

      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      /* check if this department contains sub-departments
         Display different image in this case
       */
      $non_empty="";
      $nkey= normalizePreg($key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match('/$nkey\/.*/',$keyd)){
          $non_empty="full";
        }
      }

      $title = preg_replace('/ /', '&nbsp;', @LDAP::fix($this->config->departments[$key]));

      if($val == "."){
        $field01 = array("string" => "&nbsp;",
            "attach" => "style='width:20px;'");
      }else{
        $field01 = array("string" => "<input type='checkbox' id='item_selected_".base64_encode($key)."' 
            name='item_selected_".base64_encode($key)."'>" ,
            "attach" => "style='width:20px;'");
      }

      $field0 = array("string" => "<img src='images/".$non_empty."lists/folder.png' title='"._("department")."' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $field1 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style='' title='".$title."'");
      $field2 = array("string" => preg_replace("/%KEY%/", base64_encode($key), $actions), "attach" => "style='width:60px;border-right:0px;text-align:
          right;'");

      $this->AddElement( array($field01,$field0,$field1,$field2));
    }

    /* Create summary string for list footer */
    $num_deps=count($list);
    $num_dep_str = _("Number of listed departments");
    $str = "<img class='center' src='images/lists/folder.png'
              title='".$num_dep_str."' alt='".$num_dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";
    $this->set_List_Bottom_Info($str);

  }
  function Save()
  {
    MultiSelectWindow::Save();
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
