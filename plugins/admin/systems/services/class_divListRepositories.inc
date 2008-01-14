<?php

class divListRepository extends MultiSelectWindow
{
  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $parent             ;
  var $ui                 ;

  var $regex ="*";

  function divListRepository ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Repository", "server");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    $this->SetTitle("repositories");
    $this->SetSummary(_("List of configured repositories."));
    $this->SetInformation(_("This menu allows you to create, delete and edit repository settings."));
    $this->EnableCloseButton(FALSE);
    $this->EnableSaveButton(FALSE);
    $this->AddHeader(array("string"=>_("Release"),"attach"=>"style='width:80px;'"));
    $this->AddHeader(array("string"=>_("Sections")));
    $this->AddHeader(array("string"=>_("Options"),"attach"=>"style='border-right:0px;width:55px;'"));
    $this->AddRegex("regex",_("Regular expression for matching object names"),"*",TRUE);
    $this->EnableAplhabet(TRUE);
  }

  function GenHeader()
  {
    if($this->parent->acl_is_createable()){
      $this->SetListHeader("<input type=\"image\" src=\"images/fai_new_packages.png\" title=\""._("Add repository")."\"                 name=\"AddRepository\">");
    }
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    $link   = "<a href='?plug=".$_GET['plug']."&amp;act=open_repository&amp;id=%s'>%s</a>";
    $edit   = "<input type='image' value='%s' name='edit_%s'   src='images/edit.png'>&nbsp;";

    /* Hide delete icon, if delete is not allowed */
    if($this->parent->acl_is_removeable()){
      $delete = "<input type='image' value='%s' name='delete_%s' src='images/edittrash.png'>";
    }else{
      $delete = "<img src='images/empty.png' alt='&nbsp;'>";
    }

    foreach($list as $name => $reps){

      $str = " ";
      if(preg_match("/".str_replace("*",".*",$this->regex)."/",$reps['Release'])){
        foreach($reps['Sections'] as $sec){
          $str.=$sec." ";
        }
        $this->AddElement(array(
              array("string"=>sprintf($link,base64_encode($name),$name),"attach"=>"style='width:80px;'"),
              array("string"=>sprintf($link,base64_encode($name),_("Sections")." :".$str)),
              array("string"=>preg_replace("/%s/",base64_encode($name),$edit.$delete),"attach"=>"style='border-right:0px;width:55px;text-  align:right;'")
              ));
      }
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
