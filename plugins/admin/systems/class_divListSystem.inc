<?php

class divListSystem extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();
  var $do_iso             = FALSE;

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

  function divListSystem ($config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"System");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    if (search_config($config->data['TABS'], "workgeneric", "ISOCMD")){
      $this->do_iso= TRUE;
    }

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

    /* set Page header */
    $this->AddHeader(array("string"=>"&nbsp;","attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string"=>_("System / Department")));
    $this->AddHeader(array("string"=>_("Actions"),"attach"=>"style='width:70px;border-right:0px;'"));

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
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Ignore subtrees"), false);

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
    foreach ($this->config->idepartments as $key => $value){
      if ($this->selectedBase == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();
    $listhead.=" <input class='center' type='image' align='middle' src='images/select_new_terminal.png'
        name='newsystem_terminal'    alt='"._("New Terminal template")."' title='"._("New Terminal")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_workstation.png'
        name='newsystem_workstation' alt='"._("New Workstation template")."' title='"._("New Workstation")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_server.png'     name='newsystem_server'      alt='"._("New Server")."'
        title='"._("New Server")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_printer.png'    name='newsystem_printer'     alt='"._("New Printer")."'
        title='"._("New Printer")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_phone.png'      name='newsystem_phone'       alt='"._("New Phone")."'
        title='"._("New Phone")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_component.png'  name='newsystem_component'   alt='"._("New Component")."'
        title='"._("New Component")."'>".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;"._("Base")."&nbsp;".
    " <select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
    " <input class='center' type='image' src='images/list_submit.png' align='middle'
        title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;".
    "</div>";
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

    $empty    ="&nbsp;";
    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    // Test Every Entry and generate divlist Array
    foreach($terminals as $key => $val){

      $acl= get_permissions ($val['dn'], $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "terminal", $val['dn']);

      $action= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit system")."'>";
      if(chkacl($acl,"delete") ==""){
        $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete system")."'>";
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

      if(((in_array("gotoTerminal",$val['objectClass']))||(in_array("gotoWorkstation",$val['objectClass']))) && chkacl($acl,"password") == ""){
        $action2 = "<input class='center' type='image' src='images/certs.png' alt='"._("Password")."'   name='user_setpwd_%KEY%' title='"._("Set root       password")."'>";
      }else{
        $action2 = "";
      }

      if($this->do_iso && in_array("gotoWorkstation",$val['objectClass'])){
        $action2= "<input class='center' type='image' src='images/cdrom.png' alt='"._("Create CD")."'   name='gen_cd_%KEY%' title='"._("Create FAI CD")."'>".$action2;
      }

      /* Add Message generated by mangement->reload*/  
      if(isset($val['message'])){
        $display.= "  (".$val['message'].")";
      }

      $img    = $this->parent->convert_list($val);
      $field1 = array("string" => sprintf($img['img'],$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action2.$action), "attach" => "style='width:70px;border-right:0px;text-align:right;'");
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
