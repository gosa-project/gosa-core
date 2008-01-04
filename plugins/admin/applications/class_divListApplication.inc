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

  function divListApplication (&$config,&$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Application", "application");
   
    $this->selectedRelease = get_ou('applicationou').session::get('CurrentMainBase');
 
    $this->parent       = &$parent;
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
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }


    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Application name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

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

    /* Get all departments within this subtree */
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
    $tdeps= $ui->get_module_departments("application");
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
    $ui       = get_userinfo();
    $acl      = $ui->get_permissions("cn=dummy,".$this->selectedBase,"application/application");
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"application");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* And the rest, a base selection box */
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
        title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";

    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s.= "...|<input class='center' type='image' src='images/list_new_app' alt=''>".
        "&nbsp;"._("Application")."|appl_new|\n";
    }

    /* Multiple options */
    $s.= "..|---|\n";
    $s.= "..|<img src='images/edittrash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

    /* Add multiple copy & cut icons */
    if(is_object($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s.= "..|---|\n";
      $s.= "..|<img src='images/editcopy.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Copy")."|"."multiple_copy_systems|\n";
      $s.= "..|<img src='images/editcut.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Cut")."|"."multiple_cut_systems|\n";

      if($this->parent->CopyPasteHandler->entries_queued()){
        $img = "<img border='0' class='center' src='images/editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."|editPaste|\n";
      }else{
        $img = "<img border='0' class='center' src='images/cant_editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."\n";
      }
    }

    /* Add snapshot icons */
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s .= "..|---|\n";
      $s .= $this->get_snapshot_header(TRUE);
    }

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);

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

    /* set Page header */
    $action_col_size = 80;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /********************
      Attach objects
     ********************/
    
    $ui = get_userinfo();
    foreach($list as $key => $val){

      $acl = $ui->get_permissions($val['dn'],"application/application");
      $acl_all= $ui->has_complete_category_acls($val['dn'],"application");

      /* Create action icons */
      $actions= "";

      /* Add Copy & Paste icon */
      if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
        /* Only add cut icon, if we are allowed to move this user */
        if(preg_match("/m/",$acl)){
          $actions.= "<input class='center' type='image'
            src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";

          $actions.= "<input class='center' type='image'
            src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
        }
      }

      /* Add edit icon */
      $actions.= "<input class='center' type='image'
        src='images/edit.png' alt='"._("edit")."' name='appl_edit_%KEY%' title='"._("Edit this entry")."'>";


      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions.= $this->GetSnapShotActions($val['dn']);
      }

      /* If we are allowed to remove the application account, display remove icon */
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image'
          src='images/edittrash.png' alt='"._("delete")."' name='appl_del_%KEY%' title='"._("Delete this entry")."'>";
      }else{
        $actions.= "<img src='images/empty.png' alt='&nbsp;'>";
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
      $field1 = array("string" => sprintf($applimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
      $this->AddElement(array($field0,$field1,$field2,$field3));
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
    $appfilter = session::get('appfilter');
    $appfilter['release']  = $this->selectedRelease;
    session::set('appfilter',$appfilter);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
