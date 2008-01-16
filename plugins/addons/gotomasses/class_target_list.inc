<?php

class target_list extends MultiSelectWindow
{
  var $config;
  var $list         =array();
  var $Targets_used =array();

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";
  var $IP_start           = "0.0.0.0";
  var $IP_end             = "255.255.255.255";

  /* CheckBoxes, to change default values modify $this->AddCheckBox */
  var $ogroups        ;
  var $servers        ;
  var $workstations   ;


  /* Subsearch checkbox */
  var $SubSearch          ;
  var $IPMatch            ;
  var $parent             ;
  var $ui                 ;
 
 
  function target_list(&$config,$Targets_used)
  {
    MultiSelectWindow::MultiSelectWindow($config, "Targetselection", array("ogroup","gotomasses"));

    $this->Targets_used = $Targets_used;
    
    $this->SetInformation(  _("Select the target objects for your scheduled action."));
    $this->SetTitle(        _("Available targets"));
    $this->SetSummary(      _("Available targets"));
    $this->SetHeadpageMode(FALSE);
    
    /* set Page header */
    $chk = "<input type='checkbox' id='select_all' name='select_all'
               onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));

    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" => _("Object name"), "attach" => "style=''"));

    /* Text ,Value, Name, Is selected */
    $this->AddCheckBox("ogroups",       _("Select to see object groups"),  _("Show object groups"), true);
    $this->AddCheckBox("servers",       _("Select to see servers")  ,   _("Show servers"), true);
    $this->AddCheckBox("workstations",  _("Select to see workstations"),_("Show workstations"), true);

    /* Add SubSearch checkbox */
    $this->AddCheckBox(SEPERATOR);
    $this->AddCheckBox("SubSearch",  _("Select to search within subtrees"), _("Search in subtrees"), false);
    $this->AddCheckBox("IPMatch",    _("Select to search for a specific IP range only"), _("Match IP range"), false);

    /* Name,Text,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",     _("Regular expression for matching group names"), "*" , true);
    $this->AddRegex   ("IP_start",  _("IP range start"),  "0.0.0.0" , true);
    $this->AddRegex   ("IP_end",    _("IP range end"),    "255.255.255.255" , true);
    $this->EnableAplhabet(TRUE);
  }


  function GenHeader()
  {
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
    $tdeps= $ui->get_module_departments("ogroups");
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

    /* Get copy & paste icon */
    $acls  = $ui->get_permissions($this->selectedBase,"ogroups/ogroup");
    $acl_all= $ui->has_complete_category_acls($this->selectedBase,"ogroups");

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add department selector */
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle'
      title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;";
    $listhead .="</div>";

    $this->SetListHeader($listhead);
  }


  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
    $this->reload();
    $this->SetEntries($this->list);
    return($this->Draw());
  }

  
  function SetEntries($list)
  {
      /* Add Copy & Paste buttons if copy&paste is enabled
     */
    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    $ui = get_userinfo();

    // Assigning ogroups
    foreach($list as $key => $val){

      if(in_array($val['cn'][0],$this->Targets_used) || 
         isset($val['macAddress'][0]) && in_array($val['macAddress'][0],$this->Targets_used)) continue;

      $title = "title='".preg_replace('/ /', '&nbsp;', @LDAP::fix($val['dn']))."'";
      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }
      if(!isset($val['ipHostNumber'][0])){
        $desc.= "";
      }else{
        $desc.= " - ".$val['ipHostNumber'][0]."";
      }


      $img ="Hmm";
      if(in_array("goServer",$val['objectClass'])){
        $img = "<img src='images/select_server.png' alt='"._("Server")."' ".$title.">";
      }elseif(in_array("gotoWorkstation",$val['objectClass'])){
        $img = "<img src='images/select_workstation.png' alt='"._("Workstation")."' ".$title.">";
      }elseif(in_array("gosaGroupOfNames",$val['objectClass'])){
        $img = "<img src='images/list_ogroup.png' alt='"._("Object group")."' ".$title.">";
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$key."' name='item_selected_".$key."'>" ,
                      "attach" => "style='width:20px;'");
      $field1 = array("string" => $img, 
                      "attach" => "style='text-   align:center;width: 20px;'");
      $field2 = array("string" => $val['cn'][0].$desc, 
                      "attach" => "style='border-right:0px;' ".$title);

      $this->AddElement(array($field0,$field1,$field2));
    }
  }

  
  function save()
  { 
    $ret = array();
    $items = $this->list_get_selected_items();
    foreach($items as $item){
      $ret[] = $this->list[$item];
    }
    return($ret);
  }


  function save_object()
  {
    MultiSelectWindow::save_object();
  }

  
  function reload()
  {
    /* Set base for all searches && initialise some vars */
    $this->list= array();
    $base     = $this->selectedBase;
    $filter   = "";
    $Regex    = $this->Regex;
    $IP_start = $this->IP_start;
    $IP_end   = $this->IP_end;

    if($this->IPMatch){
      if(!tests::is_ip($IP_start)){
	msg_dialog::display(_("Error"), _("IP range is invalid!"), ERROR_DIALOG);
        return;
      }
      if(!tests::is_ip($IP_end)){
	msg_dialog::display(_("Error"), _("IP range is invalid!"), ERROR_DIALOG);
        return;
      }
    }
  

    $chk = array(
        "ogroups"       => "(&(objectClass=gosaGroupOfNames)(|(gosaGroupObjects=*S*)(gosaGroupObjects=*W*)))" ,
        "servers"       => "(objectClass=goServer)" ,
        "workstations"  => "(objectClass=gotoWorkstation)");

    /* Create filter */
    foreach($chk as $chkBox => $FilterPart){
      if($this->$chkBox){
        $filter .=  $FilterPart;
      }
    }
    $filter= "(&(cn=".$Regex.")(|".$filter."))";

    if($this->SubSearch){
      $res= get_list($filter, array("ogroups","workstations","servers"), $base, 
            array("cn","objectClass","gosaGroupObjects","ipHostNumber","description"), GL_SIZELIMIT | GL_SUBSEARCH);
    }else{
      $res= get_list($filter, "ogroups", get_groups_ou().$base, 
            array("cn","objectClass","ipHostNumber","description"), GL_SIZELIMIT );
      $res= array_merge($res,get_list($filter, "workstation",  get_ou('workstationou').$base, 
            array("cn","objectClass","ipHostNumber","description"), GL_SIZELIMIT ));
      $res= array_merge($res,get_list($filter, "server", get_ou('serverou').$base, 
            array("cn","objectClass","ipHostNumber","description"), GL_SIZELIMIT ));
    }

    $this->list= $res;
    ksort ($this->list);
    reset ($this->list);
    $tmp=array();
    foreach($this->list as $tkey => $val ){

      if($this->IPMatch){
        if(isset($val['ipHostNumber'][0])){
          if(tests::is_ip_range($IP_start,$val['ipHostNumber'][0]) && tests::is_ip_range($val['ipHostNumber'][0],$IP_end)){
            $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
          }
        }
      }else{
        $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
      }
    }
    ksort($tmp);
    $this->list=array();
    foreach($tmp as $val){
      $this->list[]=$val;
    }
    reset ($this->list);
  }

  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
