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

  function divListFai (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Fai", "fai");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set default base */
    if(!isset($_SESSION['CurrentMainBase'])){
      session::set('CurrentMainBase',$this->config->current['BASE']);
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

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
#   if($this->parent->snapshotEnabled()){
#     $action_col_size += 32;
#   }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;",                "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Name of FAI class"),  "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Class type"),         "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("Actions"),            "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

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
      $r = $this->config->search("faiManagement", "POSTREMOVE",array('menu','tabs'));
      $c = $this->config->search("faiManagement", "POSTCREATE",array('menu','tabs'));

      $smarty = get_smarty();
      $smarty->assign("allow_create", $c);
      $smarty->assign("allow_remove", $r);
      $smarty->assign("selectedBranch",$this->selectedBranch);
      $smarty->assign("branchimage","images/branch.png");
      $smarty->assign("branches",$this->AvailableBranches);
      $plug_id = 0;       
      if(isset($_GET['plug'])){
        $plug_id = $_GET['plug'];
      }
      $smarty->assign("plug_id",$plug_id);
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
    $tdeps= $ui->get_module_departments("fai");
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

    /* Add seperator */
    $add_sep = false;

    /* Get complete fai acls, to be able to check if we must show or hide the snapshot abilities */ 
    $acl_all = $ui->has_complete_category_acls($this->selectedBase,$this->module);

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add additional seperator */
    if($add_sep){
      $listhead .=" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";
    }

    $listhead .=  _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle'
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit")."'>&nbsp;";


    $s  = ".|"._("Actions")."|\n";
    $s .= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";

    $acl = $ui->get_permissions($this->selectedBase,"fai/faiProfile");
    if(preg_match("/c/",$acl)){
      $s.= "...|<input class='center' type='image' src='images/fai_new_profile.png' alt=''>".
        "&nbsp;"._("Profile")."|Create_profile|\n";
    }

    /* Append create icons for each fai component */
    $arr = array(
        array("images/fai_new_partitionTable.png" , _("Partition table")  ,"Create_partition" , _("PT") , "faiPartitionTable"),
        array("images/fai_new_script.png"         , _("Scripts")          ,"Create_script"    , _("S")  , "faiScript"),
        array("images/fai_new_hook.png"           , _("Hooks")            ,"Create_hook"      , _("H")  , "faiHook"),
        array("images/fai_new_variable.png"       , _("Variables")        ,"Create_variable"  , _("V")  , "faiVariable"),
        array("images/fai_new_template.png"       , _("Templates ")       ,"Create_template"  , _("I")  , "faiTemplate"),
        array("images/fai_new_packages.png"       , _("Package list")     ,"Create_package"   , _("PK") , "faiPackage"));

    foreach($arr as $ar){
      $acl = $ui->get_permissions($this->selectedBase,"fai/".$ar[4]);
      if(preg_match("/c/",$acl)){
        $s.= "...|<input class='center' type='image' src='".$ar[0]."' alt=''>".
          "&nbsp;".$ar[1]."|".$ar[2]."|\n";
      }
    }

    /* Multiple options */
    $s.= "..|<img src='images/edittrash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

    /* Add multiple copy & cut icons */
    if(is_object($this->parent->CopyPasteHandler)){
      $s.= "..|---|\n";
      $s.= "..|<img src='images/editcopy.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Copy")."|"."multiple_copy_systems|\n";
      $s.= "..|<img src='images/editcut.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Cut")."|"."multiple_cut_systems|\n";
    }

    /* Add copy & paste icons, currently disabled, this ability is not implemeneted yet */
    if(preg_match("/(c.*w|w.*c)/",$acl_all) &&  isset($this->parent->CopyPasteHandler) && $this->parent->CopyPasteHandler){
      $s .= "..|---|\n";
      if($this->parent->CopyPasteHandler->entries_queued()){
        $img = "<img border='0' class='center' src='images/editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."|editPaste|\n";
      }else{
        $img = "<img border='0' class='center' src='images/cant_editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."\n";
      }
    }

#   /* Add snapshot icons, if we are allowed to write and create the complete fai module ($ui->has_complete_category_acls) */ 
#   if(preg_match("/(c.*w|w.*c)/",$acl_all)){ 
#     if($this->parent->snapshotEnabled()){
#       $s .= "..|---|\n";
#       $s .= $this->get_snapshot_header(TRUE);
#     }
#   }

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  /* so some basic settings */
  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
    $this->AvailableBranches = $this->parent->getBranches();
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

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 80;
#   if($this->parent->snapshotEnabled()){
#     $action_col_size += 32;
#   }

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

       /* Add copy & cut icons */
       $ui = get_userinfo();
       $action ="";
       $acl_all = $ui->has_complete_category_acls($this->selectedBase,$this->module);
       if(preg_match("/(c.*w|w.*c)/",$acl_all) && $this->parent->CopyPasteHandler){
         $action .= "<input class='center' type='image'
           src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
         $action.= "<input class='center' type='image'
           src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
       }

       if($value['FAIstate'] == "freeze"){
         $action.=  "<img src='images/closedlock.png' alt='F' class='center'><img src='images/empty.png' width='38' height='2' class='center' alt=''>";
         $action.= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";
       }else{
         $action.= "<input class='center' type='image' src='images/edit.png'  alt='"._("edit")."'
           name='entry_edit_%KEY%' title='"._("Edit class")."'>";

#        if(preg_match("/(c)/",$value['acl']) && preg_match("/(w)/",$value['acl'])){
#          $action.= $this->GetSnapShotActions($value['dn']);;
#        }

         if(preg_match("/d/",$value['acl'])){
           $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'
             name='entry_delete_%KEY%' title='"._("Delete class")."'>";
         }
       }

       /* Cutted objects should be displayed in light grey */
       $display = $value['cn'].$desc;
       if($this->parent->CopyPasteHandler){
         foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
           if($queue_data['dn'] == $value['dn']) {
             $display = "<font color='#999999'>".$display."</font>";
             break;
           }
         }
       }
       $edi = $editlink;
       $acti = $action;

       /* Create each field */
       $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
           "attach" => "style='width:20px;'");
       $field1 = array("string" => $img , "attach" => "style='text-align:center;width:20px;'");
       $field2 = array("string" => preg_replace(array("/%KEY%/","/%NAME%/","/%TITLE%/"),array($key,$display,preg_replace('/ /', '&nbsp;', @LDAP::fix($value['dn']))),$edi) , "attach" => "style=''");
       $field3 = array("string" => $info, "attach" => "style='width:200px;'");
       $field4 = array("string" => preg_replace("/%KEY%/",$key,$acti) , "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
       $this->AddElement(array($field0,$field1,$field2,$field3,$field4));
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
