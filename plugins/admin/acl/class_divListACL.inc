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

class divListACL extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes */

  /* Subsearch checkbox */
  var $SubSearch              = false;

  var $parent             ;
  var $ui                 ;
  var $action_col_size;
  

  function divListACL (&$config, &$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "ACL", array("acl","aclroles"));
    
    $this->parent       = &$parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of acls"));
    $this->SetSummary(_("List of acls"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col */
    $this->action_col_size = 120;

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
      onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Summary")));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$this->action_col_size."px;border-right:0px;text-align:right;'"));

    /* Set dynamically filter checkboxes */
    $this->set_filter_objectclasses();   
 
    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);


    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display acls matching"),"*" , true);
  }


  function GenHeader()
  {
    /* Get all departments within this subtree */
    $ui= get_userinfo();
    $base = $this->config->current['BASE'];
    $options  = $this->create_department_list($this->module);

    /* Allow copy & paste for roles */ 
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"acl");
    $acl      = $ui->get_permissions($this->selectedBase,"acl/acl");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead .=  
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/lists/submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;";


    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s .= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Create")."|\n";
      $s.= "...|<input class='center' type='image' src='plugins/acl/images/new-role.png' alt=''>".
        "&nbsp;"._("Role")."|new_acl_role|\n";
    }

    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";


    /* Add Copy & Paste header */
    $s .= $this->parent->get_copypaste_header($this->selectedBase,$this->module);

    $s .= $this->parent->get_snapshot_header($this->selectedBase,$this->module);

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    $ui = get_userinfo();
    foreach($list as $key => $acl_entry){

      $action     = "";

      /* Handle ACL role entries
       */
      if(in_array("gosaRole",$acl_entry['objectClass'])){

        $acl        = $this->ui->get_permissions($acl_entry['dn'],"aclroles/aclRole");
        $acl_all    = $this->ui->has_complete_category_acls($acl_entry['dn'],"aclroles");

        $editlink       = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=list_edit_role'>%s</a>";
        $list_left_icon = "<img src='plugins/acl/images/role.png' alt='"._("Role")."'>";
        $cn = htmlentities(utf8_decode($acl_entry['cn'][0])) ;

        if(isset($acl_entry['description'][0])){
          $cn .= "&nbsp;[".$acl_entry['description'][0]."]";
        }

        /* Add copy & cut functionality */
        $action.= $this->parent->get_copypaste_action($acl_entry['dn'],"aclroles","aclRole");

        /* Add actions depending on acls */
        if(preg_match("/r/", $acl)){
          $action.= "<input class='center' type='image' src='images/lists/edit.png' alt='"._("edit")."'     
            name='list_acl_role_edit_%KEY%' title='"._("Edit acl role")."'>";
        }

        /* Add snapshot functionality */
        $action.= $this->parent->get_snapshot_action($acl_entry['dn'],"aclroles");

      }else{

        /* Handle non-role ACL entries
         */
        $acl        = $this->ui->get_permissions($acl_entry['dn'],"acl/acl");
        $acl_all    = $this->ui->has_complete_category_acls($acl_entry['dn'],"acl");

        /* Acl handling */
        $editlink       = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=list_edit_entry'>%s</a>";
        $list_left_icon = "<img src='images/lists/locked.png' alt='"._("ACL")."'>";
        $cn = htmlentities(utf8_decode($acl_entry['dn'])); 

        /* Add actions depending on acls */
        if(preg_match("/r/", $acl)){
          $action.= "<input class='center' type='image' src='images/lists/edit.png' alt='"._("edit")."'     
            name='list_acl_edit_%KEY%' title='"._("Edit acl")."'>";
        }
      }

      /* Add delete actions */
      if(preg_match("/d/", $acl)){
        $action.= "<input class='center' type='image' src='images/lists/trash.png' alt='"._("delete")."' 
          name='list_acl_del_%KEY%' title='"._("Delete acl")."'>";
      }
      
      /* Add a checkbox in front of each entry 
       */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
          "attach" => "style='width:20px;'");

      /* Cutted objects should be displayed in light grey */
      $display = $cn;
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $acl_entry['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      $field1 = array("string" => $list_left_icon,
          "attach"=> "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display)."&nbsp;",
          "attach" => "");
      $field3 = array("string" => preg_replace("/%KEY%/",$key,$action),
          "attach"=> "style='text-align:right;width:".$this->action_col_size."px;border-right:0px;'");
      $this->AddElement(array($field0,$field1,$field2,$field3));
    }
  }


  function Save()
  {
    MultiSelectWindow :: Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow::save_object();   
  }


  /* Add some additional checkboxes to the filter part, this should be generated dynamically. */
  function set_filter_objectclasses()
  { 
  }
  
  /* Return objectClass names used in filter part */  
  function get_filter_objectclasses()
  {
    return(array("gosaAccount","gosaDepartment"));
  }



}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
