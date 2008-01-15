<?php

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
  var $TerminalGroups     ;
  var $PrinterGroups      ;
  var $PhoneGroups        ;
  

  /* Subsearch checkbox */
  var $SubSearch          ;
  var $parent             ;
  var $ui                 ;

  function divListOGroup ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"OGroups");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of object groups"));
    $this->SetSummary(_("List of object groups"));
    $this->EnableAplhabet(true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, edit or remove selected groups. You may want to use the range selector on top of the group listbox, when working with  a large number of groups."));
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Name of object groups")." / "._("Departments"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Properties"), "attach" => "style='width:136px;'"));
    $this->AddHeader(array("string" => _("Actions"), "attach" => "style='width:107px;border-right:0px;text-align:right;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
    $this->AddCheckBox("UserGroups" ,       _("Select to see groups containing users")       , _("Show groups containing users"), true);
    $this->AddCheckBox("GroupGroups" ,      _("Select to see groups containing groups")      , _("Show groups containing groups"), true);
    $this->AddCheckBox("ApplicationGroups", _("Select to see groups containing applications"), _("Show groups containing applications"), true);
    $this->AddCheckBox("DepartmentGroups" , _("Select to see groups containing departments") , _("Show groups containing departments"), true);
    $this->AddCheckBox("ServerGroups" ,     _("Select to see groups containing servers")     , _("Show groups containing servers"), true);
    $this->AddCheckBox("WorkstationGroups", _("Select to see groups containing workstations"), _("Show groups containing workstations"), true);
    $this->AddCheckBox("TerminalGroups" ,   _("Select to see groups containing terminals")   , _("Show groups containing terminals"), true);
    $this->AddCheckBox("PrinterGroups" ,    _("Select to see groups containing printer")     , _("Show groups containing printer"), true);
    $this->AddCheckBox("PhoneGroups" ,      _("Select to see groups containing phones")      , _("Show groups containing phones"), true);

    /* Add SubSearch checkbox */    
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching group names"), "*" , true);
  }


  function GenHeader()
  {
    /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    if($this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
    }else{
      $Copy_Paste ="";
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead.=  " <input class='center' type='image' align='middle' src='images/list_new_ogroup.png' 
          title='"._("Create new object group")."' alt='"._("new")."' name='group_new'>".
      $Copy_Paste.
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
          title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";
  
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
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='images/select_groups.png' alt='User'    title='%s'>";
    $mailimg  = "<img class='center' src='images/mailto.png'             alt='M'  title='"._("Mail")   ."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    // Assigning ogroups
    foreach($list as $key => $val){

      $acl= get_permissions ($val['dn'], $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "ogroup", $val['dn']);
  
      /* Create action icons */
      $actions = "";
      if($this->parent->CopyPasteHandler && $acl == "#all#"){
        $actions.= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $actions.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }
      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";

      if(chkacl($acl,"delete") == ""){
        $actions.= "<input class='center' type='image'
          src='images/edittrash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";
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
      if(isset($this->parent->CopyPasteHandler) && $this->parent->CopyPasteHandler->cutCurrent && $val['dn'] == $this->parent->CopyPasteHandler->objectdn){
        $display = "<font color='#999999'>".$display."</font>";
      }

      $field1 = array("string" => "<img src='images/list_ogroup.png' alt='"._("Object group")."' ".$title.">", "attach" => "style='text-align:center;width: 20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY/", $key, $this->parent->convert_list($val))."&nbsp;".$mail, "attach" => "style='width:136px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:107px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field1,$field2,$field3,$field4));
    }
   
 
    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_objs = count($list);

    $num_obj_str = _("Number of listed object groups");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='images/select_ogroup.png'
              title='".$num_obj_str."' alt='".$num_obj_str."'>&nbsp;".$num_objs."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/folder.png'
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
