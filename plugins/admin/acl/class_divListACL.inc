<?php

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
    MultiSelectWindow::MultiSelectWindow($config, "ACLs", "acl");
    
    $this->parent       = &$parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of acl"));
    $this->SetSummary(_("List of acls"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to edit and delete selected acls."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col */
    $this->action_col_size = 120;

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
      onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"&nbsp;",          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("Summary")));
    $this->AddHeader(array("string"=>_("Actions"),      "attach" => "style='width:".$this->action_col_size."px;border-right:0px;text-align:right;'"));

    /* Set dynamically filter checkboxes */
    $this->set_filter_objectclasses();   
 
    /* Add SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);


    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display acls matching"),"*" , true);
  }


  function GenHeader()
  {
     /* Prepare departments,
       which are shown in the listbox on top of the listbox
     */
    $options= "";
    $Copy_Paste = "";

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
    $tdeps= $ui->get_module_departments("acl");
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

    /* Allow copy & paste for roles */ 
    $acl_all  = $ui->has_complete_category_acls($this->selectedBase,"acl");
    $acl      = $ui->get_permissions($this->selectedBase,"acl/acl");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead .=  
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;";


    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    /* Append create options */
    if(preg_match("/c/",$acl)) {
      $s.= "...|<input class='center' type='image' src='images/list_new_acl_role.png' alt=''>".
        "&nbsp;"._("Role")."|new_acl_role|\n";
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
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $str = $this->get_snapshot_header(TRUE) ;
      if(!empty($str)){
        $s .= "..|---|\n";
        $s .= $str;
      }
    }

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

    foreach($list as $key => $acl_entry){

      /* Get copy & paste icon */
      $acl        = $this->ui->get_permissions($acl_entry['dn'],"acl/acl");
      $acl_all    = $this->ui->has_complete_category_acls($acl_entry['dn'],"acl");
      $action     = "";

      if(in_array("gosaRole",$acl_entry['objectClass'])){

        /* Role handling */
        $editlink       = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=list_edit_role'>%s</a>";
        $list_left_icon = "<img src='images/select_acl_role.png' alt='"._("Role")."'>";
        $cn = htmlentities(utf8_decode($acl_entry['cn'][0])) ;

        if(isset($acl_entry['description'][0])){
          $cn .= "&nbsp;[".$acl_entry['description'][0]."]";
        }

        /* Add copy & paste handler */
        if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
          $action.= "<input class='center' type='image'
            src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
          $action.= "<input class='center' type='image'
            src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
        }

        /* Add actions depending on acls */
        if(preg_match("/r/", $acl)){
          $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     
            name='list_acl_role_edit_%KEY%' title='"._("Edit acl role")."'>";
        }

        /* Add snapshot functionality */
        if(preg_match("/(c.*w|w.*c)/", $acl_all)){
          $action.= $this->GetSnapShotActions($acl_entry['dn']);
        }

        if(preg_match("/d/", $acl)){
          $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 
            name='list_acl_role_del_%KEY%' title='"._("Delete acl role")."'>";
        }
      }else{

        /* Acl handling */
        $editlink       = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=list_edit_entry'>%s</a>";
        $list_left_icon = "<img src='images/select_acl.png' alt='"._("Acl")."'>";
        $cn = htmlentities(utf8_decode($acl_entry['dn'])); 

        /* Add actions depending on acls */
        if(preg_match("/r/", $acl)){
          $action.= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     
            name='list_acl_edit_%KEY%' title='"._("Edit acl")."'>";
        }
        if(preg_match("/d/", $acl)){
          $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 
            name='list_acl_del_%KEY%' title='"._("Delete acl")."'>";
        }
      }

      if(in_array("gosaRole",$acl_entry['objectClass'])){
        $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
            "attach" => "style='width:20px;'");
      }else{
        $field0 = array("string" => "", "attach" => "style='width:20px;'");
      }

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
  
      $field1 = array("string" => $list_left_icon, "attach"=> "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display)."&nbsp;","attach" => "");
      $field3 = array("string" => preg_replace("/%KEY%/",$key,$action), "attach"=> "style='text-align:right;width:".$this->action_col_size."px;border-right:0px;'");
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
