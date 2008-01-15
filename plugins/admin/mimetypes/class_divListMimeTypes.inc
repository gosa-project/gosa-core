<?php

class divListMimeTypes extends MultiSelectWindow
{
  /* Current base */
  var $selectedBase       = "";
  var $selectedRelease    = "main";
  var $AvailableReleases  = array();
  var $departments        = array();
  var $parent               ;
  var $ui                   ;

  /* Regex */
  var $Regex              = "*";

  /* Subsearch checkbox */
  var $SubSearch;

  /* These vars should be saved too */
  var $SaveAdditionalVars = array("selectedRelease");


  function divListMimeTypes ($config,$parent)
  {
    /* Create divlist and setup */
    MultiSelectWindow::MultiSelectWindow($config, "Mimetypes", "mimetypes");
  
    /* initialize required attributes */ 
    $this->selectedRelease  = "ou=mime,".$_SESSION['CurrentMainBase'];
    $this->parent           = $parent;
    $this->ui               = get_userinfo();
    $this->AvailableReleases= $this->parent->getReleases($this->selectedBase);

    /* Set list strings */
    $this->SetTitle       (_("List of defined mime types"));
    $this->SetSummary     (_("List of defined mime types"));
    $this->SetInformation (_("This menu allows you to add, edit and remove selected mime types. You may want to use the range selector on top of the mime type listbox, when working with a large number of mime types."));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $action_col_size = 80;

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Mime type name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display mime types matching"),"*" , true);
  }


  /* This function allows us to add a user defined filter part at position $position*/
  function AddUserBoxToFilter($position)
  {
    $str = "";
    if(($position  == 2) && ($this->parent->IsReleaseManagementActivated($this->config))){
      $smarty = get_smarty();
      $smarty->assign("selectedRelease",$this->selectedRelease);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("releases",$this->AvailableReleases);
      $str = $smarty->fetch(get_template_path('release_select.tpl', TRUE,dirname(__FILE__)));
    }
    return($str);
  }


  /* Create list header, with create / copy & paste etc*/
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

    $listhead = MultiSelectWindow::get_default_header();
    $listhead .= " <input class='center' type='image' align='middle' src='images/list_new_mime.png' alt='"._("new").
      "' title='"._("Create new mime type")."' name='mime_new'>&nbsp;";
    $add_sep = true;
    $listhead.=$Copy_Paste;

    /* And at least add a department selection box */
    $listhead .=  _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
        title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";

    $listhead .="</div>";;

    $this->SetListHeader($listhead);
  }


  /* Some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
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
    $mimeimg  = "<img class='center' src='images/select_mimetype.png' alt='A'  title='"._("Mime type")."'>";
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    /* set Page header */
    $action_col_size = 80;

    /********************
      Attach objects
     ********************/

    foreach($list as $key => $val){

      /* Create action icons */
      $actions = "";

      $dn= $val['dn'];
      $acl= get_permissions ($dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "mimetype", $dn);

      /* Append copy&paste icons if allowed */
      if($acl == "#all#" && $this->parent->CopyPasteHandler){
        $actions .= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $actions.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='mime_edit_%KEY%' title='"._("Edit this entry")."'>";

      $actions.= "<input class='center' type='image'
        src='images/edittrash.png' alt='"._("delete")."' name='mime_del_%KEY%' title='"._("Delete this entry")."'>";

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

      $field1 = array("string" => sprintf($mimeimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
      $this->AddElement(array($field1,$field2,$field3));
    }


    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_objs = count($list);

    $num_obj_str = _("Number of listed mimetypes");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='images/select_mimetype.png'
              title='".$num_obj_str."' alt='".$num_obj_str."'>&nbsp;".$num_objs."&nbsp;&nbsp;&nbsp;&nbsp;";
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

    /* check if returned selectedRelease is a valid release.
        If it isn't set to a valid release */
    $this->AvailableReleases = $this->parent->getReleases($this->selectedBase);
    if(!isset($this->AvailableReleases[$this->selectedRelease])){
      $this->selectedRelease =key($this->AvailableReleases);
    }
    $_SESSION['mimefilter']['release']  = $this->selectedRelease;
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
