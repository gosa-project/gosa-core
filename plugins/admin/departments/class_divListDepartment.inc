<?php

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

  function divListDepartment ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Department");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of departments"));
    $this->SetSummary(_("List of departments"));
    $this->EnableAplhabet   (true);

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit selected departments. Having a large size of departments, you might prefer the range selectors on   top of the department list."));
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string"=>_("Department name"), "attach" => "style=''"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'"));


    /* Add SubSearch checkbox */    
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

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

    /* Generate list head */
      $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
        " <input type='image' class='center' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='".       _("Root")."'>&nbsp;".
        " <input type='image' class='center' src='images/list_up.png' align='middle' title='"._("Go up one department")."' name='dep_back' alt='"._("Up").  "'>&nbsp;".
        " <input type='image' class='center' src='images/list_home.png' align='middle' title='"._("Go to users home department")."' name='dep_home' alt='". _("Home")."'>&nbsp;".
        " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='".      _("Submit")."'>&nbsp;".
        " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
        " <input class='center' type='image' src='images/list_new_department.png' align='middle' alt='"._("Create new department")."' name='dep_new'>&nbsp; ".
        " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
        _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
        " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
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
    $actions= "<input class='center' type='image' src='images/edit.png'
      alt='"._("edit")."'   name='dep_edit_%KEY%' title='"._("Edit this entry")."'>";
    $actions.= "<input class='center' type='image' src='images/edittrash.png'
      alt='"._("delete")."'   name='dep_del_%KEY%' title='"._("Delete this entry")."'>";

    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    foreach($list as $key => $val) {
      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/".$keys."\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      $title = preg_replace('/ /', '&nbsp;', @LDAP::fix($this->config->departments[$key]));
      $field0 = array("string" => "<img src='images/".$non_empty."folder.png' title='"._("department")."' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $field1 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style='' title='".$title."'");
      $field2 = array("string" => preg_replace("/%KEY%/", base64_encode($key), $actions), "attach" => "style='width:60px;border-right:0px;text-align:
          right;'");

      $this->AddElement( array($field0,$field1,$field2));
    }
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
