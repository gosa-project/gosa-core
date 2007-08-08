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

  function divListConference ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Conferences");
    
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

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" =>_("Name - Number"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Owner"), "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("PIN"), "attach" => "style='width:50px;'"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'"));

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
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead.= " <input class='center' type='image' align='middle' src='images/list_new_conference.png' 
      title='"._("Create new conference")."' alt='"._("New conference")."' name='user_new'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit").     "'>&nbsp;".
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
    $actions = "<input class='center' type='image' src='images/edit.png'     
      alt='"._("edit")."'    name='conf_edit_%KEY%' title='"._("Edit this entry")."'>";
    $actions.= "<input class='center' type='image' src='images/edittrash.png' 
      alt='"._("delete")."'   name='conf_del_%KEY%'  title='"._("Delete this entry")."'>";
    $actions2= "<input class='center' type='image' src='images/edittrash.png' 
      alt='"._("delete")."'   name='conf_del_%KEY%'  title='"._("Delete this entry")."'>";
    $linkopen= "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    $userimg  = "<img class='center' src='images/select_conference.png' alt='User' title='%s'>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
  
        /* Insert conferneces*/
    foreach($list as $conferencekey => $conference ){

      if((isset($conference['goFonPIN'][0]))&&(($this->parent->ui->dn==$conference['goFonConferenceOwner'][0])||($this->parent->acl=="#all#"))){
        $pin = "<img class='center' src='images/list_password.png' title='PIN : ".$conference['goFonPIN'][0]."' alt='PIN'>";
      }else{
        $pin = "&nbsp;";
      }

      if(isset($conference['telephoneNumber'][0])){
        $number = " - ".$conference['telephoneNumber'][0];
      }else{
        $number = "&nbsp; - ?";
      }

      $ldap= $this->parent->config->get_ldap_link();
      $ldap->cat($conference['goFonConferenceOwner'][0], array('cn'));
      $data = $ldap->fetch();
      if(isset($data['cn'][0])){
        $cn = $data['cn'][0];
      }else{
        $cn ="";
      }

      /* Create the display name */
      $d_name = $conference['cn'][0];
      if(isset($conference['description'][0])){
        $d_name.="&nbsp;[".$conference['description'][0]."] &nbsp;";
      }
      $d_name .=$number;


      $title = " title='".preg_replace("/ /","&nbsp;",@LDAP::fix($data['dn']))."' ";

      $a_field1 = array("string"=>sprintf($userimg,_("Conference")), "attach" => $title." style='text-align:center;width:20px;'");

      if(($this->parent->acl=="#all#")||(($this->parent->ui->dn==$conference['goFonConferenceOwner'][0])&&(chkacl($this->parent->acl,"goFonConferenceOwner")==""))){
        $a_field2 = array("string"=>sprintf($editlink,$conferencekey,$d_name ), "attach" => $title." style=''");
      }else{
        $a_field2 = array("string"=>$d_name , "attach" => $title." style=''");
      }
      $a_field3 = array("string"=> $cn, "attach" =>  $title." style='width:200px;'");
      $a_field4 = array("string"=> $pin, "attach" =>  $title." style='width:50px;'");

      if(($this->parent->ui->dn==$conference['goFonConferenceOwner'][0])&&(chkacl($this->parent->acl,"goFonConferenceOwner")=="")){
        $a_field5 = array("string"=>preg_replace("/%KEY%/",$conferencekey,$actions), "attach" =>  $title."style='width:60px;border-right:0px;text-align:right;'");
      }elseif(chkacl($this->parent->acl,"goFonConferenceOwner")==""){
        $a_field5 = array("string"=>preg_replace("/%KEY%/",$conferencekey,$actions2), "attach" =>  $title."style='width:52px;border-right:0px;text-align:right;'");
      }else{
        $a_field5 = array("string"=>"&nbsp;", "attach" =>  $title."style='width:52px;border-right:0px;text-align:right;'");
      }

      $this->AddElement(array($a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
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
