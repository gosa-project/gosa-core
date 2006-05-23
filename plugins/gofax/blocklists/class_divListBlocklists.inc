<?php

class divListBlocklist extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* checkboxes */
  var $ShowSendBocklists      ;
  var $ShowReceiveBlocklists  ;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListBlocklist ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Blocklists");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of blocklists"));
    $this->SetSummary(_("List of blocklists"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit selected blocklists. Having a large size of lists, you might prefer the range selectors on top of the select box."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Blocklist name")." / "._("Department"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" ));

    /* Add Checkboxes / SubSearch checkbox */
    $this->AddCheckBox("ShowSendBocklists"     , _("Select to see send blocklists"),    _("Show send blocklists"),true);
    $this->AddCheckBox("ShowReceiveBlocklists" , _("Select to see receive blocklists"), _("Show receive blocklists"),true);
  
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching list names"),"*" , true);
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
    
    /* NEW LIST MANAGMENT */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' src='images/list_root.png' align='middle' 
          title='"._("Go to root department")."' name='dep_root' alt='".       _("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_back.png' 
          title='"._("Go up one department")."' alt='"._("Up")."'              name='dep_back'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_home.png' 
          title='"._("Go to users department")."' alt='"._("Home")."'          name='dep_home'>&nbsp;".
      " <input class='center' type='image' src='images/list_reload.png' align='middle' 
          title='"._("Reload list")."' name='submit_department' alt='".      _("Submit")."'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_new_blocklist.png' 
          title='"._("Create new blocklist")."' alt='"._("New Blocklist")."' name='user_new'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>".
          "&nbsp;"._("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
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
    $action= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit user")."'>";
    $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete user")."'>";

    // User and Template  Images
    $blockimg = "<img class='center' src='images/list_blocklist.png' alt='User' title='%s'>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Append to list */
    foreach($list as $key => $val){
      // Generate Array to Add
      $display= "[".$val["cn"][0]."]";
      $field1 = array("string" => sprintf($blockimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action), "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $this->AddElement( array($field1,$field2,$field3));
    }
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
