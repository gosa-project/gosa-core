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

class divListGroup extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $UserRegex          = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $ShowPrimaryGroups;
  var $ShowSambaGroups;  
  var $ShowApplicationGroups; 
  var $ShowMailGroups;
  var $ShowFunctionalGroups;

  /* Subsearch checkbox */
  var $SubSearch              = false;

  var $parent             ;
  var $ui                 ;

  function divListGroup (&$config, &$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Groups", "groups");
    
    $this->parent       = &$parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of groups"));
    $this->SetSummary(_("List of groups"));
    $this->EnableAplhabet   (true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Name")."&nbsp;/&nbsp;"._("Department")));
    $this->AddHeader(array("string"=>_("Properties"),"attach"=>"style='width:136px;'"));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:".$action_col_size."px;border-right:0px;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
    $this->AddCheckBox("ShowPrimaryGroups",     _("Select to see groups that are primary groups of users"), 
                                                  sprintf(_("Show %s"),_("primary groups")),     true);
    $this->AddCheckBox("ShowSambaGroups",       msgPool::selectToView(_("samba groups mappings"),"enabled"), 
                                                  sprintf(_("Show %s"),_("samba groups")),              true);
    $this->AddCheckBox("ShowApplicationGroups", msgPool::selectToView(_("application settings"),"enabled"),     
                                                  sprintf(_("Show %s"),_("application groups")),        true);
    $this->AddCheckBox("ShowMailGroups",        msgPool::selectToView(_("mail settings"),"enabled"), 
                                                  sprintf(_("Show %s"),_("mail groups")),               true);
    $this->AddCheckBox("ShowFunctionalGroups",  _("Select to see normal groups that have only functional aspects"),
                                                  sprintf(_("Show %s"),_("functional groups")),     true);
    $this->AddCheckBox(SEPERATOR);

    /* Add SubSearch checkbox */    
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"),msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching group names"), "*" , true);
    $this->AddRegex   ("UserRegex", _("User name of which groups are shown"),         "*" , false, "images/lists/search-user.png");
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
    $tdeps= $ui->get_module_departments("groups");
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
  
    /* Get acls */
    $acl   = $ui->get_permissions($this->selectedBase,"groups/group");


    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    
    /* Add department selection */
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input type='image' class='center' src='images/lists/submit.png' title='"._("Submit department")."' name='submit_department' alt='"._("Submit").
      "'>&nbsp;";

    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s .= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Create")."|\n";
      $s.= "...|<input class='center' type='image' src='plugins/groups/images/new.png' alt=''>".
        "&nbsp;"._("Group")."|group_new|\n";
    }

    /* Multiple options */
    $s.= "..|---|\n";
    $s.= "..|<img src='images/lists/edit.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Edit")."|"."multiple_edit|\n";
    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

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

    /* Add multiple copy & cut icons */
    $acl_all= $ui->has_complete_category_acls($this->selectedBase,"groups");

    if(is_object($this->parent->CopyPasteHandler) && preg_match("/r/",$acl_all)){
      $s.= "..|<img src='images/lists/copy.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Copy")."|"."multiple_copy_systems|\n";
      $s.= "..|<img src='images/lists/cut.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Cut")."|"."multiple_cut_systems|\n";
    }

    /* Copy & paste icons */
    if(is_object($this->parent->CopyPasteHandler) && $this->parent->CopyPasteHandler->entries_queued()){
      $img = "<img border='0' class='center' src='images/lists/paste.png' alt=''>";
      $s.="..|".$img."&nbsp;"._("Paste")."|editPaste|\n";
    }else{
      $img = "<img border='0' class='center' src='images/lists/paste-grey.png' alt=''>";
      $s.="..|".$img."&nbsp;"._("Paste")."\n";
    }

    /* Add snapshot functionality */
    $s .= $this->parent->get_snapshot_header($this->selectedBase,$this->module);

    $this->SetDropDownHeaderMenu($s);

    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($groups)
  {
    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    // image Buttons
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='plugins/groups/images/groups.png' alt='User'    title='%s'>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Assign extension images */
    $posiximg = "<input type='image' class='center' src='plugins/groups/images/groups.png'
                 name='group_group_edit_%KEY%' alt='P'  title='"._("Posix")  ."'>";
    $mailimg  = "<input type='image' class='center' src='plugins/groups/images/mail.png'
                 name='mailgroup_group_edit_%KEY%' alt='M'  title='"._("Mail")   ."'>";
    $sambaimg = "<input type='image'  class='center' src='plugins/groups/images/samba.png'
                 name='group_group_edit_%KEY%' alt='S'  title='"._("Samba")  ."'>";
    $applimg  = "<input type='image'  class='center' src='plugins/groups/images/menu.png'
                 name='appgroup_group_edit_%KEY%' alt='A'  title='"._("Application")."'>";
    $phoneimg = "<input type='image'  class='center' src='plugins/groups/images/asterisk.png'
                 name='group_group_edit_%KEY%' alt='Ph' title='"._("Phone")  ."'>";
    $envimg   = "<input type='image'  class='center' src='plugins/groups/images/environment.png'
                 name='environment_group_edit_%KEY%' alt='E' title='"._("Environment")  ."'>";

    // Space
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    $ui = get_userinfo();

    /* Fetch all application menu entries 
     */
    $base = get_groups_ou().$this->selectedBase;
    $ldap = $this->config->get_ldap_link();
    $ldap->cd ($base);
    $ldap->search("(objectClass=gotoMenuEntry)",array("dn"));
    $configured_apps = array();
    while($entry = $ldap->fetch()){
      $dn = preg_replace("/^.*(cn=[^,]+,".normalizePreg(get_groups_ou()).")/","\\1",$entry['dn']);
      $configured_apps[$dn] = $dn;
    }

    // Test Every Entry and generate divlist Array
    foreach($groups as $key => $val){
    
      /* Create action icons - copy & paste icons */
      $acl = $ui->get_permissions($val['dn'],"groups/group");
      $acl_all  = $ui->has_complete_category_acls($val['dn'],"groups");

      $actions= "";

      if($ui->is_cutable($val['dn'],"groups","group") && $this->parent->CopyPasteHandler){
        $actions .= "<input class='center' type='image'
          src='images/lists/cut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
      }else{
        $actions.="<img src='images/empty.png' alt=' ' class='center'>&nbsp;";
      }

      if($ui->is_copyable($val['dn'],"groups","group") && $this->parent->CopyPasteHandler){
        $actions.= "<input class='center' type='image'
          src='images/lists/copy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }else{
        $actions.="<img src='images/empty.png' alt=' ' class='center'>&nbsp;";
      }

      /* Add edit icon */
      $actions.= "<input class='center' type='image'
        src='images/lists/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";

      /* Add snapshot functionality */
      $actions.= $this->parent->get_snapshot_action($val['dn'],$this->module);
  
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image'
          src='images/lists/trash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";
      }else{
        $actions.= "<img class='center' src='images/empty.png' alt='&nbsp;'
          title='"._("Not allowed")."'>";
      }

      $posix=$mail=$samba=$appl=$phone=$enviro=$empty;

      if(isset($val['objectClass'])){
        if(in_array("posixGroup",           $val['objectClass']))   $posix = $posiximg;
        if(in_array("gosaMailAccount",      $val['objectClass']))   $mail  = $mailimg;
        if(in_array("sambaGroupMapping",    $val['objectClass']))   $samba = $sambaimg;
        if(in_array("goFonPickupGroup",     $val['objectClass']))   $phone = $phoneimg;
        if(in_array("gotoEnvironment",      $val['objectClass']))   $enviro= $envimg;
      }

      if(isset($configured_apps[$val['dn']])){
          $appl  = $applimg;
      }

      $title = "title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'";

      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }

      /* FAIrelease tag from groupApplications */
      if(isset($val['FAIrelease'][0])){
        $desc .= " (".$val['FAIrelease'][0].")";
      }

      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
          "attach" => "style='width:20px;'");

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

      $field1 = array("string" => sprintf($userimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $posix."&nbsp;".$enviro."&nbsp;".$mail."&nbsp;".$samba."&nbsp;".$appl."&nbsp;".$phone),     "attach" => "style='width:136px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field0,$field1,$field2,$field3,$field4));
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_grps = count($groups);

    $num_grp_str = _("Number of listed groups");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='plugins/groups/images/groups.png'
              title='".$num_grp_str."' alt='".$num_grp_str."'>&nbsp;".$num_grps."&nbsp;&nbsp;&nbsp;&nbsp;";
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
