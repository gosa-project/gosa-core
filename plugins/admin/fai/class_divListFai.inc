<?php

class divListFai extends MultiSelectWindow
{
  /* Current base */
  var $selectedBase       = "";
  var $selectedBranch     = "main";
  var $AvailableBranches  = array();
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $ShowProfiles;
  var $ShowTemplates;
  var $ShowScripts;
  var $ShowHooks;
  var $ShowVariables;
  var $ShowPackages;
  var $ShowPartitions;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  var $SaveAdditionalVars = array("selectedBranch");

  function divListFai ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Fai");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set default base */
    if(!isset($_SESSION['CurrentMainBase'])){
      $_SESSION['CurrentMainBase'] = $this->config->current['BASE'];
    }
    $this->selectedBase = $_SESSION['CurrentMainBase'];

    /* Set list strings */
    $this->SetTitle(_("List of FAI classes"));
    $this->SetSummary(_("This table displays all FAI classes in the selected tree."));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to create, delete and edit FAI classes."));

    $this->EnableAplhabet   (true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* set Page header */
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Name of FAI class"),  "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Class type"),         "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:80px;border-right:0px;text-align:right;'"));

    $this->AddCheckBox("ShowProfiles"   , _("Display FAI profile objects")    ,_("Show profiles")     ,true);
    $this->AddCheckBox("ShowTemplates"  , _("Display FAI template objects")   ,_("Show templates")    ,true);
    $this->AddCheckBox("ShowScripts"    , _("Display FAI scripts")            ,_("Show scripts")      ,true);
    $this->AddCheckBox("ShowHooks"      , _("Display FAI hooks")              ,_("Show hooks")        ,true);
    $this->AddCheckBox("ShowVariables"  , _("Display FAI variables")          ,_("Show variables")    ,true);
    $this->AddCheckBox("ShowPackages"   , _("Display FAI packages")           ,_("Show packages")     ,true);
    $this->AddCheckBox("ShowPartitions" , _("Display FAI partitions")         ,_("Show partitions")   ,true);

    /* Add SubSearch checkbox */
    //$this->AddCheckBox(SEPERATOR);
    //$this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display users matching"),"*" , true);
  }

  function AddUserBoxToFilter($position)
  {
    $str = "";
    if($position  == 2){

      /* Check if there are post commands available for fai management.
       * If not, grey out freeze/branch and release remove buttons.
       */
      $r = ("" != search_config($this->config->data,"faiManagement","POSTREMOVE"));
      $c = ("" != search_config($this->config->data,"faiManagement","POSTCREATE"));

      $smarty = get_smarty();
      $smarty->assign("allow_create", $c);
      $smarty->assign("allow_remove", $r);
      $smarty->assign("selectedBranch",$this->selectedBranch);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("branches",$this->AvailableBranches);
      $str = $smarty->fetch(get_template_path('branch_selector.tpl', TRUE));
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
    $listhead.=      " <input class='center' type='image' src='images/fai_new_profile.png' align='middle'
      title='"._("New profile")."' name='Create_profile' alt='"._("P")."'>&nbsp;".

      "<img class='center' src=\"images/list_seperator.png\" alt=\"\" align=\"middle\" height=\"16\" width=\"1\">&nbsp;".

      " <input class='center' type='image' src='images/fai_new_partitionTable.png' align='middle'
      title='"._("New partition table")."' name='Create_partition' alt='"._("PT")."'>&nbsp;".

      " <input class='center' type='image' src='images/fai_new_script.png' align='middle'
      title='"._("New scripts")."' name='Create_script' alt='"._("S")."'>&nbsp;".

      " <input class='center' type='image' src='images/fai_new_hook.png' align='middle'
      title='"._("New hooks")."' name='Create_hook' alt='"._("H")."'>&nbsp;".

      " <input class='center' type='image' src='images/fai_new_variable.png' align='middle'
      title='"._("New variables")."' name='Create_variable' alt='"._("V")."'>&nbsp;".

      " <input class='center' type='image' src='images/fai_new_template.png' align='middle'
      title='"._("New templates")."' name='Create_template' alt='"._("T")."'>&nbsp;".

      " <input class='center' type='image' src='images/fai_new_packages.png' align='middle'
      title='"._("New package list")."' name='Create_package' alt='"._("PK")."'>&nbsp;".

      $Copy_Paste.

      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".

      _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;".

      "</div>";

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
    /********************
      Variable init
     ********************/

     $objects = array(
           "FAIpartitionTable"  => array("IMG"=> "images/fai_partitionTable.png", "NAME"=>_("Partition table"),"KZL"=> "PT", "VAR"=>"ShowPartitions"),
           "FAIpackageList"     => array("IMG"=> "images/fai_packages.png",       "NAME"=>_("Package list") ,  "KZL"=> "PL", "VAR"=>"ShowPackages"),
           "FAIscript"          => array("IMG"=> "images/fai_script.png",         "NAME"=>_("Scripts") ,       "KZL"=> "S",  "VAR"=>"ShowScripts"),
           "FAIvariable"        => array("IMG"=> "images/fai_variable.png",       "NAME"=>_("Variables") ,     "KZL"=> "V",  "VAR"=>"ShowVariables"),
           "FAIhook"            => array("IMG"=> "images/fai_hook.png",           "NAME"=>_("Hooks"),          "KZL"=> "H",  "VAR"=>"ShowHooks"),
           "FAIprofile"         => array("IMG"=> "images/fai_profile.png",        "NAME"=>_("Profile") ,       "KZL"=> "P",  "VAR"=>"ShowProfiles"),
           "FAItemplate"        => array("IMG"=> "images/fai_template.png",       "NAME"=>_("Templates") ,     "KZL"=> "T",  "VAR"=>"ShowTemplates"));

    $editlink ="<a href='?plug=".$_GET['plug']."&amp;edit_entry=%KEY%' title='%TITLE%'>%NAME%</a>";

    /********************
      Attach objects
     ********************/

    $cnts = array();
    foreach($objects as $key => $data){
      $cnts[$key] = 0;
    }

     foreach($list as $key => $value){
       $info     = "";
       $img      = "";
       $type     = $value['type'];
       $abort    =false;

       $cnts[$type] ++;

       if(isset($objects[$type])){
         $img   = "<img class='center' src='".$objects[$type]['IMG']."' title='".$objects[$type]['NAME']."' alt='".$objects[$type]['KZL']."'>";
         $info  = $objects[$type]['NAME'];
         $var   = $objects[$type]['VAR'];
       }else{
         $img   = "<img class='center' src='images/empty.png' alt=''>";
         $info  = "";
         $var   = "";
       }

       if((isset($value['description']))&&(!empty($value['description']))){
         $desc= " [".$value['description']."]";
       }else{
         $desc= "";
       }

       /* Create action icons */
       $action ="";
       if($this->parent->CopyPasteHandler){
         if($value['FAIstate'] != "freeze"){
           $action .= "<input class='center' type='image'
             src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
         }
         $action.= "<input class='center' type='image'
           src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
       }

       if($value['FAIstate'] == "freeze"){
         $action .= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";
         $edi = $editlink;
         $acti = $action;
       }else{
         $action .= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";
         $action .= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'
           name='entry_delete_%KEY%' title='"._("Delete class")."'>";
         $acti = $action;
         $edi = $editlink;
       }

       /* Cutted objects should be displayed in light grey */
       $display = $value['cn'].$desc;
       if(is_object($this->parent->CopyPasteHandler)){
         if($this->parent->CopyPasteHandler->cutCurrent && $value['dn'] == $this->parent->CopyPasteHandler->objectdn){
           $display = "<font color='#999999'>".$display."</font>";
         }
       }

       $field1 = array("string" => $img , "attach" => "style='text-align:center;width:20px;'");
       $field2 = array("string" => preg_replace(array("/%KEY%/","/%NAME%/","/%TITLE%/"),array($key,$display,preg_replace('/ /', '&nbsp;', @LDAP::fix($value['dn']))),$edi) , "attach" => "style=''");
       $field3 = array("string" => $info, "attach" => "style='width:200px;'");
       $field4 = array("string" => preg_replace("/%KEY%/",$key,$acti) , "attach" => "style='width:80px;border-right:0px;text-align:right;'");
       $this->AddElement(array($field1,$field2,$field3,$field4));
     }

     /* Create summary string for list footer */
     $num_deps=0;
     if(!$this->SubSearch){
       $num_deps = count($this->Added_Departments);
     }

     $profile_str    = _("Number of listed profiles");
     $partition_str  = _("Number of listed partitions");
     $script_str     = _("Number of listed scripts");
     $hook_str       = _("Number of listed hooks");
     $variable_str   = _("Number of listed variables");
     $template_str   = _("Number of listed templates");
     $package_str    = _("Number of listed packages");
     $dep_str        = _("Number of listed departments");

     $str = "<img class='center' src='".$objects['FAIprofile']['IMG']."'
       title='".$profile_str."' alt='".$profile_str."'>&nbsp;".$cnts['FAIprofile']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAIpartitionTable']['IMG']."'
       title='".$partition_str."' alt='".$partition_str."'>&nbsp;".$cnts['FAIpartitionTable']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAIscript']['IMG']."'
       title='".$script_str."' alt='".$script_str."'>&nbsp;".$cnts['FAIscript']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAIhook']['IMG']."'
       title='".$hook_str."' alt='".$hook_str."'>&nbsp;".$cnts['FAIhook']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAIvariable']['IMG']."'
       title='".$variable_str."' alt='".$variable_str."'>&nbsp;".$cnts['FAIvariable']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAItemplate']['IMG']."'
       title='".$template_str."' alt='".$template_str."'>&nbsp;".$cnts['FAItemplate']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='".$objects['FAIpackageList']['IMG']."'
       title='".$package_str."' alt='".$package_str."'>&nbsp;".$cnts['FAIpackageList']."&nbsp;&nbsp;&nbsp;&nbsp;";
     $str.= "<img class='center' src='images/folder.png'
       title='".$dep_str."' alt='".$dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";
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
    $_SESSION['faifilter']['branch'] = $this->selectedBranch; 
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
