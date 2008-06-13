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

class divListOGroup extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $UserGroups         ;
  var $GroupGroups        ;
  var $ApplicationGroups  ;
  var $DepartmentGroups   ; 
  var $ServerGroups       ;
  var $WorkstationGroups  ;
  var $WindowsGroups  ;
  var $TerminalGroups     ;
  var $PrinterGroups      ;
  var $PhoneGroups        ;
  

  /* Subsearch checkbox */
  var $SubSearch          ;
  var $parent             ;
  var $ui                 ;

  function divListOGroup (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "OGroups", "ogroups");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of object groups"));
    $this->SetSummary(_("List of object groups"));
    $this->EnableAplhabet(true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 90;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38; 
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Object group")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Properties"), "attach" => "style='width:136px;'"));
    $this->AddHeader(array("string" => _("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
    $str2 = _("Show %s");
    $this->AddCheckBox("UserGroups" ,       sprintf($str2,  _("user groups")), 
                                            sprintf($str2,  _("user groups")), true);
    $this->AddCheckBox("GroupGroups" ,      sprintf($str2,  _("nested groups")),
                                            sprintf($str2,  _("nested groups")), true);
    $this->AddCheckBox("ApplicationGroups", sprintf($str2,  _("application groups")),
                                            sprintf($str2,  _("application groups")), true);
    $this->AddCheckBox("DepartmentGroups" , sprintf($str2,  _("department groups")),
                                            sprintf($str2,  _("department groups")), true);
    $this->AddCheckBox("ServerGroups" ,     sprintf($str2,  _("server groups")),
                                            sprintf($str2,  _("server groups")), true);
    $this->AddCheckBox("WorkstationGroups", sprintf($str2,  _("workstation groups")),
                                            sprintf($str2,  _("workstation groups")), true);
    $this->AddCheckBox("WindowsGroups",     sprintf($str2,  _("windows workstation groups")),
                                            sprintf($str2,  _("windows workstation groups")), true);
    $this->AddCheckBox("TerminalGroups" ,   sprintf($str2,  _("terminal groups")),
                                            sprintf($str2,  _("terminal groups")), true);
    $this->AddCheckBox("PrinterGroups" ,    sprintf($str2,  _("printer groups")),
                                            sprintf($str2,  _("printer groups")), true);
    $this->AddCheckBox("PhoneGroups" ,      sprintf($str2,  _("phone groups")),
                                            sprintf($str2,  _("phone groups")), true);

    /* Add SubSearch checkbox */    
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex", _("Regular expression for matching group names"), "*" , true);
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
    $tdeps= $ui->get_module_departments("ogroups");
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

    /* Add a seperator after displaying c&p and snapshot icons ? */
    $add_sep = false;
  
    /* Get copy & paste icon */
    $acls  = $ui->get_permissions($this->selectedBase,"ogroups/ogroup");
    $acl_all= $ui->has_complete_category_acls($this->selectedBase,"ogroups");


    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
  
    /* Add department selector */
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/lists/submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";


    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acls)) {
      $s .= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Create")."|\n";
      $s.= "...|<input class='center' type='image' src='plugins/ogroups/images/new.png' alt=''>".
        "&nbsp;"._("Object group")."|group_new|\n";
    }

    /* Multiple options */
    $s.= "..|---|\n";
#   $s.= "..|<img src='images/lists/edit.png' alt='' border='0' class='center'>".
#     "&nbsp;"._("Edit")."|"."multiple_edit|\n";
    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple_ogroups/|\n";

    /* Add event tag */
    if(class_available("DaemonEvent")){
      $events  = DaemonEvent::get_event_types(USER_EVENT);
      if(count($events['BY_CLASS'])){
        $s.= "..|---|\n";
        foreach($events['BY_CLASS'] as $name => $event){
          $s.= "..|".$event['MenuImage']."&nbsp;".$event['s_Menu_Name']."|event_".$name."\n";
        }
      }
    }

    /* Add Copy & Paste header */
    $s .= $this->parent->get_copypaste_header($this->selectedBase,$this->module);

    /* Add snapshot icons */
    $s .= $this->parent->get_snapshot_header($this->selectedBase,$this->module);    

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
    /* Add Copy & Paste buttons if copy&paste is enabled
     */
    // Defining Links
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='plugins/groups/images/groups.png' alt='User'    title='%s'>";
    $mailimg  = "<img class='center' src='plugins/ogroups/images/mail.png'             alt='M'  title='"._("Mail")   ."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 90;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38; 
    }
  
    $ui = get_userinfo();

    // Assigning ogroups
    foreach($list as $key => $val){

      $actions = "";
      
      /* Create action icons */
      $acl    = $ui->get_permissions($val['dn'],"ogroups/ogroup");
      $acl_all= $ui->has_complete_category_acls($val['dn'],"ogroups");

      /* Add copy & cut functionality */
      $actions.= $this->parent->get_copypaste_action($val['dn'],"ogroups","ogroup");

      $actions.= "<input class='center' type='image'
        src='images/lists/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";
    
      $actions.= $this->parent->get_snapshot_action($val['dn'],$this->module);

      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image'
          src='images/lists/trash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";
      }else{
        $actions.= "<img class='center' src='images/empty.png' alt='&nbsp;'
          title='".msgPool::permDelete()."'>";
      }

      if(isset($val['mail'])){
        $mail = $mailimg;
      }else{
        $mail = $empty;
      }
      $title = "title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'";
      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }

      /* Cutted objects should be displayed in light grey */
      $display = $val['cn'][0].$desc;
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $val['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");
      $field1 = array("string" => "<img src='plugins/ogroups/images/ogroup.png' alt='"._("Object group")."' ".$title.">", "attach" => "style='text-align:center;width: 20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY/", $key, $this->parent->convert_list($val))."&nbsp;".$mail, "attach" => "style='width:136px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field0,$field1,$field2,$field3,$field4));
    }
   
 
    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_objs = count($list);

    $num_obj_str = _("Number of listed object groups");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='plugins/ogroups/images/ogroup.png'
              title='".$num_obj_str."' alt='".$num_obj_str."'>&nbsp;".$num_objs."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/lists/folder.png'
              title='".$num_dep_str."' alt='".$num_dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";

    $this->set_List_Bottom_Info($str);
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
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
