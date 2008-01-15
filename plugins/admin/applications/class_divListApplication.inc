<?php

class divListApplication extends MultiSelectWindow
{
  /* Current base */
  var $selectedBase       = "";
  var $selectedRelease    = "main";
  var $AvailableReleases  = array();
  var $departments        = array();
  var $parent               ;
  var $ui                   ;

  /* Regex */
  var $Regex           = "*";

  /* Subsearch checkbox */
  var $SubSearch;

  var $SaveAdditionalVars = array("selectedRelease");

  function divListApplication ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Application");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of Applications"));
    $this->SetSummary(_("This table displays all applications in the selected tree."));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, edit and remove selected applications. You may want to use the range selector on top of the application listbox, when working with a large number of applications."));

    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Application name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:80px;border-right:0px;text-align:right;'"));

    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
  }

  function AddUserBoxToFilter($position){
    $str = "";
    if(($position  == 2) && ($this->parent->IsReleaseManagementActivated($this->config))){
      $smarty = get_smarty();
      $smarty->assign("selectedRelease",$this->selectedRelease);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("releases",$this->AvailableReleases);
      $str = $smarty->fetch(get_template_path('release_select.tpl', TRUE));
    }
    return($str);
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

    /* Get copy & paste icon */
    if($this->parent->CopyPasteHandler){
      $Copy_Paste = $this->parent->CopyPasteHandler->generatePasteIcon();
    }else{
      $Copy_Paste ="";
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead.= " <input class='center' type='image' align='middle' src='images/list_new_app.png' alt='"._("new").
        "' title='"._("Create new application")."' name='appl_new'>&nbsp;".
      $Copy_Paste.
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
        title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";

    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
    $this->AvailableReleases = $this->parent->getReleases($this->selectedBase);
  }

  function setEntries($list)
  {
    /********************
      Variable init
     ********************/
  
    /* Create links */
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='images/select_groups.png' alt='User'    title='%s'>";
    $applimg  = "<img class='center' src='images/select_application.png' alt='A'  title='"._("Application")."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";


    /********************
      Attach objects
     ********************/

    foreach($list as $key => $val){

      $acl= get_permissions ($val['dn'], $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "application", $val['dn']);

      /* Get state */
      $state = "";
      if(isset($val['FAIstate'])){
        $state = $val['FAIstate'][0];
      }

      /* If this application is a freezed one, we must hide some action buttons */
      $is_freezed = preg_match("/freeze/i",$state);

      /* Create action icons */
      $actions = "";

      if($acl == "#all#" || preg_match('/#create#/', $acl)){
        if(($this->parent->CopyPasteHandler) && (!$is_freezed)){
          $actions.= "<input class='center' type='image'
            src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        }
        if($this->parent->CopyPasteHandler){
          $actions.= "<input class='center' type='image'
            src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
        }
      }
    
      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='appl_edit_%KEY%' title='"._("Edit this entry")."'>";
      if($is_freezed){
        $actions.= "<img src='images/empty.png' width='16' alt='&nbsp;' class='center'>";
      }else{

        if(chkacl($acl,"delete") == ""){
          $actions.= "<input class='center' type='image'
            src='images/edittrash.png' alt='"._("delete")."' name='appl_del_%KEY%' title='"._("Delete this entry")."'>";
        }
      }


      $title = "title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'";

      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }

      /* Cutted objects should be displayed in light grey */
      $display = $val['cn']['0'].$desc;
      if(isset($this->parent->CopyPasteHandler) && $this->parent->CopyPasteHandler->cutCurrent && $val['dn'] == $this->parent->CopyPasteHandler->objectdn){
        $display = "<font color='#999999'>".$display."</font>";
      }

      $field1 = array("string" => sprintf($applimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:80px;border-right:0px;text-align:right;'");
      $this->AddElement(array($field1,$field2,$field3));
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_apps = count($list);

    $num_app_str = _("Number of listed applications");
    $num_dep_str = _("Number of listed departments");
  
    $str = "<img class='center' src='images/select_application.png' 
              title='".$num_app_str."' alt='".$num_app_str."'>&nbsp;".$num_apps."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/folder.png' 
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
    $_SESSION['appfilter']['release']  = $this->selectedRelease;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
