<?php

class divListSystem extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $UserRegex          = "*";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $ShowServers;
  var $ShowTerminals;
  var $ShowWorkstations;
  var $ShowWinWorkstations;
  var $ShowPrinters;
  var $ShowDevices;
  var $ShowPhones;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListSystem (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config, "System", array("server",
                                                                  "workstation",
                                                                  "terminal",
                                                                  "phone",
                                                                  "incoming",
                                                                  "printer"));

    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of systems"));
    $this->SetSummary(_("List of systems"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();
    $this->SetInformation(_("This menu allows you to add, remove and change the properties of specific systems. You can only add systems which have already been started once."));

    $this->EnableAplhabet   (true);

    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 110;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("System / Department")));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:".$action_col_size."px;border-right:0px;'"));

    /*                  Text        ,Value    ,Name         ,Is selected */
    $this->AddCheckBox("ShowServers",         _("Select to see servers"),            _("Show servers"),          true);
    $this->AddCheckBox("ShowTerminals",       _("Select to see Linux terminals"),    _("Show terminals") ,       true);
    $this->AddCheckBox("ShowWorkstations",    _("Select to see Linux workstations"), _("Show workstations"),     true);
    $this->AddCheckBox("ShowWinWorkstations", _("Select to see MicroSoft Windows based workstations"), _("Show windows based workstations"),true);
    $this->AddCheckBox("ShowPrinters",        _("Select to see network printers"),    _("Show network printers") ,true);
    $this->AddCheckBox("ShowPhones",          _("Select to see VOIP phones"),         _("Show phones") ,          true);
    $this->AddCheckBox("ShowDevices",         _("Select to see network devices"),     _("Show network devices"),  true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Display systems matching"),"*" , true);
    $this->AddRegex   ("UserRegex", _("Display systems of user"), "*" , false, "images/search_user.png");
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
    $first = "";
    $found = FALSE;
    $department = $departments = array();
    foreach($this->module as $module){
      $d = $ui->get_module_departments($module);      
      foreach($d as $department){
        $departments[$department] = $department;
      }
    }

    /* Load possible departments */
    $ui= get_userinfo();
    $ids = $this->config->idepartments;
    $first = "";
    $found = FALSE;
    foreach($ids as $dep => $name){
      if(isset($deps[$dep]) && in_array_ics($dep, $departments)){

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

    /* If there is at least one c (Create) and one w (Write) acl in this combination
        display the snapshot paste icon. BUT check the correct acls in the management plugin */
    $all_module_acls = "";
    foreach($this->module as $module){
      $all_module_acls .= $ui->has_complete_category_acls($this->selectedBase,$module);
    }

    /* Must we add an additional seperator */
    $add_sep = false;
   
    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add base selection */
    $listhead .= _("Base")."&nbsp; <select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle'
      title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;";

    /* Handle create icons */ 
    $s = ".|"._("Actions")."|\n";
    $s.= "..|<img src='images/list_new.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Create")."|\n";
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"terminal/termgeneric"))){
      $s.= "...|<img src='images/select_terminal.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Terminal")."|"."newsystem_terminal|\n";
    }
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"workstation/workgeneric"))){
      $s.= "...|<img src='images/select_workstation.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Workstation")."|"."newsystem_workstation|\n";
    }
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"workstation/workgeneric"))){
      $s.= "...|<img src='images/select_server.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Server")."|"."newsystem_server|\n";
    }
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"phone/phoneGeneric"))){
      $s.= "...|<img src='images/select_phone.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Phone")."|"."newsystem_phone|\n";
    }
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"printer/printgeneric"))){
      $s.= "...|<img src='images/select_printer.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Printer")."|"."newsystem_printer|\n";
    }
    if(preg_match("/c/",$ui->get_permissions($this->selectedBase,"component/componentGeneric"))){
      $s.= "...|<img src='images/select_component.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Component")."|"."newsystem_component|\n";
    }
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

    /* Get copy & paste icon */
    $Copy_Paste ="";
    if(preg_match("/(c.*w|w.*c)/",$all_module_acls) &&  $this->parent->CopyPasteHandler){
      $s .= "..|---|\n";
      if($this->parent->CopyPasteHandler->entries_queued()){
        $img = "<img border='0' class='center' src='images/editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."|editPaste|\n";
      }else{
        $img = "<img border='0' class='center' src='images/cant_editpaste.png' alt=''>";
        $s.="..|".$img."&nbsp;"._("Paste")."\n";
      }
    }

    /* Add snapshot icons */
    if(preg_match("/(c.*w|w.*c)/",$all_module_acls)){
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

  function setEntries($terminals)
  {
    $img1  = "<img class='center' src='images/printer.png'            alt='C' title='"._("Cups Server")  ."'>";
    $img2  = "<img class='center' src='images/scanner.png'            alt='L' title='"._("Log Db") ."'>";
    $img3  = "<img class='center' src='images/select_terminal.png'    alt='L' title='"._("Syslog Server") ."'>";
    $img4  = "<img class='center' src='images/mailto.png'             alt='M' title='"._("Mail Server")  ."'>";
    $img5  = "<img class='center' src='images/select_phone.png'       alt='I' title='"._("Imap Server") ."'>";
    $img6  = "<img class='center' src='images/fax_small.png'          alt='F' title='"._("Nfs Server")   ."'>";
    $img7  = "<img class='center' src='images/select_winstation.png'  alt='K' title='"._("Kerberos Server") ."'>";
    $img8  = "<img class='center' src='images/select_phone.png'       alt='A' title='"._("Asterisk Server") ."'>";
    $img9  = "<img class='center' src='images/fax_small.png'          alt='F' title='"._("Fax Server") ."'>";
    $img10 = "<img class='center' src='images/save.png'               alt='L' title='"._("Ldap Server") ."'>";

    $empty    ="<img src='images/empty.png' style='width:16px ; height: 16px;' class='center' alt=''>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 110;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 38;
    }

    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* ACL mapping */
    $ui = get_userinfo();
    $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs",      "ACL"=> "terminal/termgeneric"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs",      "ACL"=> "workstation/workgeneric"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs",      "ACL"=> "server/servgeneric"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs",     "ACL"=> "printer/printgeneric"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs",     "ACL"=> "phone/phoneGeneric"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs",       "ACL"=> "winworkstation/wingeneric"),
          "incoming"    => array("CLASS"=>"",             "TABCLASS" =>"",              "ACL"=> "incoming/systems"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs", "ACL"=> "component/componentGeneric"));


    $cnts = array("component" => 0 , "terminal" => 0 , "workstation" => 0 , 
                  "printer" => 0 , "phone" => 0 , "server" => 0,
                  "NewDevice" => 0, "winstation"=> 0);

    // Test Every Entry and generate divlist Array
    foreach($terminals as $key => $val){

      /* Get system type, it is used to fetch the acls for the current object.
          "winstation" acls are stored as 2winworkstation", so we have to map this here */
      $type     = $this->parent->get_system_type($val);
      if($type == "winstation") {
        $acl_type = "winworkstation";
      }else{
        $acl_type = $type;
      }

      /* Map NewDevice acls to workstation acls */
      if($type == "NewDevice" || $type == "ArpNewDevice"){
        $type     = "incoming";
        $acl_type = "incoming";
      }

      /* Get complete category acls */
      $acl_all  = $ui->has_complete_category_acls($val['dn'],$acl_type) ;
  
      /* Get specific generic acls */
      $acl      = $ui->get_permissions($val['dn'],$tabs[$type]['ACL']);

      $action ="";
      if(($this->parent->CopyPasteHandler) && preg_match("/(c.*w|w.*c)/",$acl_all)){
        $action.= "<input class='center' type='image'
          src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
        $action.= "<input class='center' type='image'
          src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      }
      $action.= "<input class='center' type='image' src='images/edit.png' 
                    alt='"._("edit")."'     name='system_edit_%KEY%' title='"._("Edit system")."'>";
      if(preg_match("/(c.*w|w.*c)/",$acl_all)){
        $action.= $this->GetSnapShotActions($val['dn']);
      }else{
        $action.= $empty."&nbsp;".$empty."&nbsp;";
      }


      if(preg_match("/d/",$acl)){
        $action.= "<input class='center' type='image' src='images/edittrash.png' 
          alt='"._("delete")."'   name='system_del_%KEY%' title='"._("Delete system")."'>";
      }else{
        $action.= $empty;    
      }

      /* Generate picture list, which is currently disabled */
      if(in_array("goCupsServer"    ,$val['objectClass'])) $cups    = $img1;   else $cups  =$empty;
      if(in_array("goLogDBServer"   ,$val['objectClass'])) $logdb   = $img2;   else $logdb =$empty;
      if(in_array("goSyslogServer"  ,$val['objectClass'])) $syslog  = $img3;   else $syslog=$empty;
      if(in_array("goImapServer"    ,$val['objectClass'])) $imap    = $img4;   else $imap  =$empty;
      if(in_array("sambaSamAccount" ,$val['objectClass'])) $samba   = $img5;   else $samba =$empty;
      if(in_array("goShareServer"   ,$val['objectClass'])) $nfs     = $img6;   else $nfs   =$empty;
      if(in_array("goKrbServer"     ,$val['objectClass'])) $krb     = $img7;   else $krb   =$empty;
      if(in_array("goFonServer"     ,$val['objectClass'])) $fon     = $img8;   else $fon   =$empty;
      if(in_array("goFaxServer"     ,$val['objectClass'])) $fax     = $img9;   else $fax   =$empty;
      if(in_array("goLdapServer"    ,$val['objectClass'])) $ldap     = $img10;   else $ldap   =$empty;

      $pics = $cups.$logdb.$syslog.$imap.$samba.$nfs.$krb.$fon.$fax.$ldap;
      $pics = ""; // Pictures currently hidden

      $val['cn'][0]= preg_replace('/\$$/', '', $val['cn'][0]);

      // Generate Array to Add
      $dsc= "";
      if (isset($val['description'][0])){
        $dsc= " [".$val['description'][0]."]";
      }
      if((isset($val['is_new']))&&(!empty($val['is_new']))){
        $display= $val["cn"][0]." ".$val['is_new'].$dsc;
      }else{
        $display= $val["cn"][0].$dsc;
      }

     
      /* Check if this is a terminal/workstation && if we are allowed to change the gotoRootPasswd */
      $pwd_acl =""; 
      if(in_array("gotoWorkstation",$val['objectClass'])){
        $pwd_acl = $ui->get_permissions($val['dn'],"workstation/workgeneric","gotoRootPasswd");
      }    
      if(in_array("gotoTerminal",$val['objectClass'])){
        $pwd_acl = $ui->get_permissions($val['dn'],"terminal/termgeneric","gotoRootPasswd");
      }    
      if(preg_match("/w/",$pwd_acl)){
        $action2 = "<input class='center' type='image' src='images/certs.png' alt='"._("Password")."'   name='system_setpwd_%KEY%' title='"._("Set root password")."'>";
      }else{
        $action2 = $empty;
      }


      if(in_array("gotoWorkstation",$val['objectClass'])){
        $action2= "<input class='center' type='image' src='images/cdrom.png' alt='"._("Create CD")."'   
                      name='gen_cd_%KEY%' title='"._("Create FAI CD")."'>&nbsp;".$action2;
      }else{
        $action2= $empty.$action2;
      }

      if(isset($val['message'])){
        $display.= "  (".$val['message'].")";
      }

      $img    = $this->parent->convert_list($val);

      /* Cutted objects should be displayed in light grey */
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $val['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      $type = $this->parent->get_system_type($val);
      if($type == "ArpNewDevice"){
        $type = "NewDevice";
      }
      $cnts[$type] ++;


      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");

      $field1 = array("string" => sprintf($img['img'],$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action2.$action), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'");
      $this->AddElement( array($field0,$field1,$field2,$field3));
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }

    $num_ser_str = _("Number of listed servers");
    $num_ws_str  = _("Number of listed workstations");
    $num_ter_str = _("Number of listed terminals");
    $num_pho_str = _("Number of listed phones");
    $num_prt_str = _("Number of listed printers");
    $num_com_str = _("Number of listed network components");
    $num_new_str = _("Number of listed new devices");
    $num_wws_str = _("Number of listed windows workstations");
    $num_dep_str = _("Number of listed departments");

    $str = "<img class='center' src='images/select_server.png'
      title='".$num_ser_str."' alt='".$num_ser_str."'>&nbsp;".$cnts['server']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_workstation.png'
      title='".$num_ws_str."' alt='".$num_ws_str."'>&nbsp;".$cnts['workstation']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_terminal.png'
      title='".$num_ter_str."' alt='".$num_ter_str."'>&nbsp;".$cnts['terminal']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_phone.png'
      title='".$num_pho_str."' alt='".$num_pho_str."'>&nbsp;".$cnts['phone']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_printer.png'
      title='".$num_prt_str."' alt='".$num_prt_str."'>&nbsp;".$cnts['printer']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_component.png'
      title='".$num_com_str."' alt='".$num_com_str."'>&nbsp;".$cnts['component']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_newsystem.png'
      title='".$num_new_str."' alt='".$num_new_str."'>&nbsp;".$cnts['NewDevice']."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/select_winstation.png'
      title='".$num_wws_str."' alt='".$num_wws_str."'>&nbsp;".$cnts['winstation']."&nbsp;&nbsp;&nbsp;&nbsp;";
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
