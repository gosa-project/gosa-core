<?php

class divListConference extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* checkboxes */
  var $ShowSendBocklists      ;
  var $ShowReceiveMacros  ;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListConference (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Conferences", "gofonconference");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of conference rooms"));
    $this->SetSummary(_("List of conference rooms"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit selected phone conferences. Having a large number of phone conferences, you might prefer the range selectors on top of the conferences list."));

    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 100;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
      onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" =>_("Name - Number"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Owner"), "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("PIN"), "attach" => "style='width:50px;'"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Add Checkboxes / SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",      _("Regular expression for matching conference names"),"*" , true);
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
    $tdeps= $ui->get_module_departments("gofonconference");
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
    $acls     = $ui->get_permissions($this->selectedBase,"gofonconference/conference");
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"gofonconference") ;

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add the rest (base select ....)*/
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit").     "'>&nbsp;";

    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    /* Append create options */
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s.= "...|<input class='center' type='image' src='images/list_new_conference.png' alt=''>".
        "&nbsp;"._("Conference")."|conference_new|\n";
    }

    /* Multiple options */
    $s.= "..|---|\n";
    $s.= "..|<img src='images/edittrash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

    /* Add multiple copy & cut icons */
    if(is_object($this->parent->CopyPasteHandler)){
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
    if(preg_match("/(c.*w|w.*c)/",$acls)){
      $s .= "..|---|\n";
      $s .= $this->get_snapshot_header(TRUE);
    }

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
    $linkopen= "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    $userimg  = "<img class='center' src='images/select_conference.png' alt='User' title='%s'>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 100;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* Insert conferneces*/
    foreach($list as $conferencekey => $conference ){

      $acl      = $this->ui->get_permissions($conference['dn'],"gofonconference/conference");
      $acl_all  = $this->ui->has_complete_category_acls($conference['dn'],"gofonconference");

      /* You will need at least read access for the 
          current conference informations to display it */  
      if(!preg_match("/r/",$acl)){
        continue;
      }

      $actions ="";
      if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions.= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $actions.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }

      /* Add edit icon - This is allowed when we have at least read access. */
      $actions.= "<input class='center' type='image' src='images/edit.png'     
                    alt='"._("edit")."'    name='conference_edit_%KEY%' title='"._("Edit this entry")."'>";

      /* Add snapshot icon - This is allowed when we have create and write access for the current entry */
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $actions.= $this->GetSnapShotActions($conference['dn']);
      }

      /* Create delete link - Only if we are allowed to delete this entry */
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image' src='images/edittrash.png' 
          alt='"._("delete")."'   name='conference_del_%KEY%'  title='"._("Delete this entry")."'>";
      }

      /* Display PIN icon, only if we are the owner of the given conference */
      $owner = $this->parent->ui->dn==$conference['goFonConferenceOwner'][0];
      $p_acl = $this->ui->get_permissions($conference['dn'],"gofonconference/conference","goFonPIN");
      if((isset($conference['goFonPIN'][0])) && $owner && preg_match("/r/",$p_acl)){
        $pin = "<img class='center' src='images/list_password.png' title='PIN : ".$conference['goFonPIN'][0]."' alt='PIN'>";
      }else{
        $pin = "&nbsp;";
      }

      /* Display numer of the conference if we are allowed to view it */
      $n_acl = $this->ui->get_permissions($conference['dn'],"gofonconference/conference","telephoneNumber");
      if(isset($conference['telephoneNumber'][0]) && preg_match("/r/",$n_acl)){
        $number = " - ".$conference['telephoneNumber'][0];
      }else{
        $number = "&nbsp; - ?";
      }

      /* Get conference owner name */
      $ldap= $this->parent->config->get_ldap_link();
      $ldap->cat($conference['goFonConferenceOwner'][0], array('cn'));
      $data = $ldap->fetch();
      if(isset($data['cn'][0])){
        $cn = $data['cn'][0];
      }else{
        $cn = _("Unknown"); 
      }  

      /* Create title */
      $title = " title='".preg_replace("/ /","&nbsp;",@LDAP::fix($data['dn']))."' ";

      /* Cutted objects should be displayed in light grey */
      $display = $conference['cn'][0].$number;
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $conference['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$conferencekey."' name='item_selected_".$conferencekey."'>" ,
          "attach" => "style='width:20px;'");
      $a_field1 = array("string"=>sprintf($userimg,_("Conference")), "attach" => $title." style='text-align:center;width:20px;'");
      $a_field2 = array("string"=>sprintf($editlink,$conferencekey,$display), "attach" => $title." style=''");
      $a_field3 = array("string"=> $cn , "attach" =>  $title." style='width:200px;'");
      $a_field4 = array("string"=> $pin, "attach" =>  $title." style='width:50px;'");
      $a_field5 = array("string"=> preg_replace("/%KEY%/",$conferencekey,$actions), 
                        "attach"=> $title."style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field0,$a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_objs = count($list);

    $num_obj_str = _("Number of listed conferences");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='images/select_conference.png'
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
    MultiSelectWindow :: save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
