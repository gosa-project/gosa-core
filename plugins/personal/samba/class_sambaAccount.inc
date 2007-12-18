<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004-2005 Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class sambaAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Samba";
  var $plDescription= "This does something";
  var $view_logged = FALSE;

  /* CLI vars */
  var $cli_summary= "Manage users samba account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Switch for Samba version */
  var $samba3= FALSE;
  var $uidNumber= 65535;
  var $gidNumber= 65535;

  /* Samba 2 attributes */
  var $pwdLastSet= "0";
  var $logonTime= "0";
  var $logoffTime= "2147483647";
  var $kickoffTime= "2147483647";
  var $pwdCanChange= "0";
  var $pwdMustChange= "0";
  var $password_expires= 0;
  var $acctFlags= "[UX        ]";
  var $smbHome= "";
  var $homeDrive= "";
  var $scriptPath= "";
  var $profilePath= "";
  var $rid= "";
  var $primaryGroupID= "";

  /* Samba 3 attributes */
  var $SID= "";
  var $ridBase= 0;
  var $sambaSID= "";
  var $sambaPwdLastSet= "0";
  var $sambaLogonTime= "0";
  var $sambaLogoffTime= "2147483647";
  var $sambaKickoffTime= "2147483647";
  var $sambaPwdCanChange= "";
  var $sambaPwdMustChange= "0";
  var $sambaAcctFlags= "[UX        ]";
  var $sambaHomePath= "";
  var $sambaHomeDrive= "";
  var $sambaLogonScript= "";
  var $sambaProfilePath= "";
  var $sambaPrimaryGroupSID= "";
  var $sambaDomainName= "";
  var $sambaUserWorkstations= "";
  var $sambaBadPasswordCount= "";
  var $sambaBadPasswordTime= "";
  var $sambaPasswordHistory= "";
  var $sambaLogonHours= "";
  var $orig_sambaDomainName= "";
  var $sambaMungedDial= "";
  var $mungedObject;

  /* Helper */
  var $show_ws_dialog= FALSE;
  var $logon_time_set= 0;
  var $logoff_time_set= 0;
  var $kickoff_time_set= 0;

  /* attribute list for save action */
  var $ctxattributes= array();
  var $attributes= array();
  var $objectclasses= array();
  
  var $uid= "";
  var $CopyPasteVars = array("kickoff_time_set","logoff_time_set","logon_time_set","mungedObject","orig_sambaDomainName");

  var $multiple_support = TRUE;

  function sambaAccount (&$config, $dn= NULL)
  {
    /* Load attributes depending on the samba version */
    $this->samba3= ($config->current['SAMBAVERSION'] == 3);

    if ($this->samba3){
      $this->attributes= array ("sambaSID", "sambaPwdLastSet", "sambaLogonTime",
          "sambaLogoffTime", "sambaKickoffTime", "sambaPwdCanChange",
          "sambaPwdMustChange", "sambaAcctFlags", "uid", "sambaMungedDial",
          "sambaHomePath", "sambaHomeDrive", "sambaLogonScript",
          "sambaProfilePath", "sambaPrimaryGroupSID", "sambaDomainName",
          "sambaUserWorkstations", "sambaPasswordHistory",
          "sambaLogonHours", "sambaBadPasswordTime",
          "sambaBadPasswordCount");
      $this->objectclasses= array ("sambaSamAccount");
      $this->mungedObject= new sambaMungedDial;
      $this->ctxattributes= $this->mungedObject->ctxattributes;
    } else {
      $this->attributes= array ("pwdLastSet", "logonTime", "logoffTime", "kickoffTime",
          "pwdCanChange", "pwdMustChange", "acctFlags", "profilePath", "uid",
          "smbHome", "homeDrive", "scriptPath", "rid", "primaryGroupID");
      $this->objectclasses= array ("sambaAccount");
    }

    plugin::plugin ($config, $dn);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

    /* Get samba Domain in case of samba 3 */
    if ($this->samba3 && $this->sambaSID != ""){
      $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase","sambaDomainName"));
      if ($ldap->count() != 0){
        $attrs= $ldap->fetch();
        $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];
        if ($this->sambaDomainName == ""){
          $this->sambaDomainName= $attrs['sambaDomainName'][0];
        }
      } else {
        if ($this->sambaDomainName == ""){
          $this->sambaDomainName= "DEFAULT";
        }
        $this->ridBase= $this->config->current['RIDBASE'];
        $this->SID= $this->config->current['SID'];
      }

      /* Save in order to compare later on */
      $this->orig_sambaDomainName= $this->sambaDomainName;
    }

    /* Fill mungedDial field */
    if ($this->samba3 && isset($this->attrs['sambaMungedDial'])){
      $this->mungedObject->load($this->sambaMungedDial);
    }

    /* Password expiery */
    if(isset($this->attrs['sambaPwdMustChange']) &&
        $this->attrs['sambaPwdMustChange'][0] != 0){
      $this->password_expires= 1;
    }

    if(isset($this->attrs['sambaLogonTime']) && ! (
        $this->attrs['sambaLogonTime'][0] == 0 ||
        $this->attrs['sambaLogonTime'][0] == 2147483647
      )){
      $this->logon_time_set= 1;
    }
    if(isset($this->attrs['sambaLogoffTime']) && ! (
        $this->attrs['sambaLogoffTime'][0] == 0 ||
        $this->attrs['sambaLogoffTime'][0] == 2147483647
      )){
      $this->logoff_time_set= 1;
    }
    
    /* Account expiery */
    if(isset($this->attrs['sambaKickoffTime']) && ! (
        $this->attrs['sambaKickoffTime'][0] == 0 ||
        $this->attrs['sambaKickoffTime'][0] == 2147483647
      )){
      $this->kickoff_time_set= 1;
    }

    /* Get global filter config */
    if (!is_global("sambafilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $sambafilter= array( "depselect" => $base, "regex" => "*");
      register_global("sambafilter", $sambafilter);
    }

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }
    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\"src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no samba extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }

    $display ="";
    if(!$this->multiple_support_active){

      /* Show tab dialog headers */
      $display= "";
      if ($this->parent !== NULL){
        if ($this->is_account){
          $display= $this->show_disable_header(_("Remove samba account"),
              _("This account has samba features enabled. You can disable them by clicking below."));
        } else {
          $obj= $this->parent->by_object['posixAccount'];

          /* Samba3 dependency on posix accounts are enabled
             in the moment, because I need to rely on unique
             uidNumbers. There'll be a better solution later
             on. */
          if ($obj->is_account){
            $display= $this->show_enable_header(_("Create samba account"),
                _("This account has samba features disabled. You can enable them by clicking below."));
          } else {
            $display= $this->show_enable_header(_("Create samba account"),
                _("This account has samba features disabled. Posix features are needed for samba accounts, enable them first."), TRUE);
          }
          return ($display);
        }
      }
    }

    $SkipWrite = (!isset($this->parent) || !$this->parent) && !isset($_SESSION['edit']);

    /* Open Samaba Logong hours dialog */
    if(isset($_POST['SetSambaLogonHours']) && $this->samba3 && $this->acl_is_writeable("sambaLogonHours")){
      $this->dialog = new sambaLogonHours($this->config,$this->dn,$this->sambaLogonHours);
    }

    /* Cancel dialog */
    if(isset($_POST['cancel_logonHours'])){
      $this->dialog = FALSE;
    }

    /* Save selected logon hours */
    if(isset($_POST['save_logonHours'])){
      $this->dialog->save_object();
      if($this->acl_is_writeable("sambaLogonHours")){
        $this->sambaLogonHours = $this->dialog->save();
      }
      $this->dialog = FALSE;
    }

    /* Display dialog */
    if((isset($this->dialog)) && (is_object($this->dialog))){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /* Prepare templating */
    $smarty= get_smarty();

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $var => $rest){
      $smarty->assign($var."ACL",$this->getacl($var,$SkipWrite));
    }

    if ($this->sambaPwdMustChange=="0"){
      $date= getdate();
    } else {
      $date= getdate($this->sambaPwdMustChange);
    }

    if ($this->sambaLogonTime=="2147483647" || $this->sambaLogonTime=="0"){
      $sambaLogonTime_date= getdate();
    } else {
      $sambaLogonTime_date= getdate($this->sambaLogonTime);
    }
    
    if ($this->sambaLogoffTime=="2147483647" || $this->sambaLogoffTime=="0"){
      $sambaLogoffTime_date= getdate();
    } else {
      $sambaLogoffTime_date= getdate($this->sambaLogoffTime);
    }
    
    if ($this->sambaKickoffTime=="2147483647" || $this->sambaKickoffTime=="0"){
      $sambaKickoffTime_date= getdate();
    } else {
      $sambaKickoffTime_date= getdate($this->sambaKickoffTime);
    }

    /* Remove user workstations? */
    if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list'])){

      if($this->acl_is_writeable("sambaUserWorkstations",$SkipWrite)){

        $tmp= $this->sambaUserWorkstations;
        foreach($_POST['workstation_list'] as $name){
          $tmp= preg_replace("/$name/", '', $tmp);
          $this->is_modified= TRUE;
        }
        $tmp= preg_replace('/,+/', ',', $tmp);
        $this->sambaUserWorkstations= trim($tmp, ',');
      }
    }

    /* Add user workstation? */
    if (isset($_POST["add_ws"])){
      if($this->acl_is_writeable("sambaUserWorkstations",$SkipWrite)){
        $this->show_ws_dialog= TRUE;
        $this->dialog= TRUE;
      }
    }

    /* Add user workstation finished? */
    if (isset($_POST["add_ws_finish"]) || isset($_POST["add_ws_cancel"])){
      $this->show_ws_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Add user workstation? */
    if (isset($_POST["add_ws_finish"]) && isset($_POST['wslist'])){
      $tmp= $this->sambaUserWorkstations;
      foreach($_POST['wslist'] as $ws){
        $tmp.= ",$ws";
      }
      $tmp= preg_replace('/,+/', ',', $tmp);
      $this->sambaUserWorkstations= trim($tmp, ',');
      $this->is_modified= TRUE;
    }

    /* Show ws dialog */
    if ($this->show_ws_dialog){

      /* Save data */
      $sambafilter= get_global("sambafilter");
      foreach( array("depselect", "regex") as $type){
        if (isset($_POST[$type])){
          $sambafilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $sambafilter['regex']= $s;
      }
      register_global("sambafilter", $sambafilter);

      /* Get workstation list */
      $exclude= "";
      foreach(split(',', $this->sambaUserWorkstations) as $ws){
        $exclude.= "(cn=$ws$)";
      }
      if ($exclude != ""){
        $exclude= "(!(|$exclude))";
      }
      $regex= $sambafilter['regex'];
      $filter= "(&(objectClass=sambaSAMAccount)$exclude(uid=*$)(|(uid=$regex)(cn=$regex)))";
      $res= get_list($filter, "winworkstation", $sambafilter['depselect'], array("uid"), GL_SUBSEARCH | GL_SIZELIMIT);
        
      $wslist= array();
      foreach ($res as $attrs){
        $wslist[]= preg_replace('/\$/', '', $attrs['uid'][0]);
      }
      asort($wslist);

      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("depselect", "regex") as $type){
        $smarty->assign("$type", $sambafilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("wslist", $wslist);
      $smarty->assign("apply", apply_filter());
      $display= $smarty->fetch (get_template_path('samba3_workstations.tpl', TRUE,
                                dirname(__FILE__)));
      return ($display);
    }

    /* Fill calendar */
    $days= array();
    for($d= 1; $d<32; $d++){
      $days[]= $d;
    }
    $years= array();
    for($y= $date['year']-4; $y<$date['year']+4; $y++){
      $years[]= $y;
    }
    $months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"));
    $smarty->assign("day", $date["mday"]);
    $smarty->assign("days", $days);
    $smarty->assign("months", $months);
    $smarty->assign("month", $date["mon"]-1);
    $smarty->assign("years", $years);
    $smarty->assign("year", $date["year"]);
    
    $sambaLogonTime_days= array();
    for($d= 1; $d<32; $d++){
      $sambaLogonTime_days[]= $d;
    }
    $sambaLogonTime_years= array();
    for($y= $date['year']-4; $y<$date['year']+4; $y++){
      $sambaLogonTime_years[]= $y;
    }
    $sambaLogonTime_months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"));
    $smarty->assign("sambaLogonTime_day", $sambaLogonTime_date["mday"]);
    $smarty->assign("sambaLogonTime_days", $sambaLogonTime_days);
    $smarty->assign("sambaLogonTime_months", $sambaLogonTime_months);
    $smarty->assign("sambaLogonTime_month", $sambaLogonTime_date["mon"]-1);
    $smarty->assign("sambaLogonTime_years", $sambaLogonTime_years);
    $smarty->assign("sambaLogonTime_year", $sambaLogonTime_date["year"]);
    
    $sambaLogoffTime_days= array();
    for($d= 1; $d<32; $d++){
      $sambaLogoffTime_days[]= $d;
    }
    $sambaLogoffTime_years= array();
    for($y= $date['year']-4; $y<$date['year']+4; $y++){
      $sambaLogoffTime_years[]= $y;
    }
    $sambaLogoffTime_months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"));
    $smarty->assign("sambaLogoffTime_day", $sambaLogoffTime_date["mday"]);
    $smarty->assign("sambaLogoffTime_days", $sambaLogoffTime_days);
    $smarty->assign("sambaLogoffTime_months", $sambaLogoffTime_months);
    $smarty->assign("sambaLogoffTime_month", $sambaLogoffTime_date["mon"]-1);
    $smarty->assign("sambaLogoffTime_years", $sambaLogoffTime_years);
    $smarty->assign("sambaLogoffTime_year", $sambaLogoffTime_date["year"]);
    
    $sambaKickoffTime_days= array();
    for($d= 1; $d<32; $d++){
      $sambaKickoffTime_days[]= $d;
    }
    $sambaKickoffTime_years= array();
    for($y= $date['year']-4; $y<$date['year']+4; $y++){
      $sambaKickoffTime_years[]= $y;
    }
    $sambaKickoffTime_months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"));
    //$smarty->assign("sambaKickoffTime_day", $sambaKickoffTime_date["mday"]-1);
    $smarty->assign("sambaKickoffTime_day", $sambaKickoffTime_date["mday"]); // hickert
    $smarty->assign("sambaKickoffTime_days", $sambaKickoffTime_days);
    $smarty->assign("sambaKickoffTime_months", $sambaKickoffTime_months);
    $smarty->assign("sambaKickoffTime_month", $sambaKickoffTime_date["mon"]-1);
    $smarty->assign("sambaKickoffTime_years", $sambaKickoffTime_years);
    $smarty->assign("sambaKickoffTime_year", $sambaKickoffTime_date["year"]);
     
    /* Fill boxes */
    if ($this->samba3){
      $domains= array();
      foreach($this->config->data['SERVERS']['SAMBA'] as $name => $content){
        $domains[]= $name;
      }
      $smarty->assign("domains", $domains);
    }
    $letters= array();
    for ($i= 68; $i<91; $i++){
      $letters[]= chr($i).":";
    }
    $smarty->assign("drives", $letters);

    /* Fill terminal server settings */
    if ($this->samba3){
      foreach ($this->ctxattributes as $attr){
        /* Fill common attributes */
        if (isset($this->mungedObject->ctx[$attr])){
          $smarty->assign("$attr", $this->mungedObject->ctx[$attr]);
          // Set field  to blank if value is 0
          if(in_array($attr, array("CtxMaxConnectionTime", "CtxMaxDisconnectionTime", "CtxMaxIdleTime"))) {
            if($this->mungedObject->ctx[$attr] == 0) {
              $smarty->assign("$attr", "");
            }
          }
        } else {
          $smarty->assign("$attr", "");
        }
      }

      /* Assign enum values for preset items */
      $shadowModeVals= array( "0" => _("disabled"),
          "1" => _("input on, notify on"),
          "2" => _("input on, notify off"),
          "3" => _("input off, notify on"),
          "4" => _("input off, nofify off"));

      $brokenConnModeVals= array( 	"0" => _("disconnect"),
          "1" => _("reset"));

      $reConnModeVals= array( "0" => _("from any client"),
          "1" => _("from previous client only"));

      /* Fill preset items */
      $smarty->assign("shadow", $shadowModeVals);
      $smarty->assign("brokenconn", $brokenConnModeVals);
      $smarty->assign("reconn", $reConnModeVals);

      /* Fill preset items with values */
      $smarty->assign("shadowmode", $this->mungedObject->getShadow());
      $smarty->assign("brokenconnmode", $this->mungedObject->getBrokenConn());
      $smarty->assign("reconnmode", $this->mungedObject->getReConn());

      if($_SESSION['js']){
        /* Set form elements to disabled/enable state */
        $smarty->assign("tsloginstate", $this->mungedObject->getTsLogin()?"":"disabled");

        $smarty->assign("inheritstate", "");
        if($this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite)){
          $smarty->assign("inheritstate", $this->mungedObject->getInheritMode()?"disabled":"");
        }
  

  
      }else{
        $smarty->assign("tsloginstate", "");
        $smarty->assign("inheritstate", "");
      }      

      /* Set checkboxes to checked or unchecked state */
      $smarty->assign("tslogin", $this->mungedObject->getTsLogin()?"checked":"");
      $smarty->assign("inherit", $this->mungedObject->getInheritMode()?"checked":"");
      $smarty->assign("connectclientdrives",
                      $this->mungedObject->getConnectClientDrives()?"checked":"");
      $smarty->assign("connectclientprinters",
                      $this->mungedObject->getConnectClientPrinters()?"checked":"");
      $smarty->assign("defaultprinter",
                      $this->mungedObject->getDefaultPrinter()?"checked":"");
      $smarty->assign("CtxMaxConnectionTimeF",
                      $this->mungedObject->getCtxMaxConnectionTimeF()?"checked":"");
      $smarty->assign("CtxMaxDisconnectionTimeF",
                      $this->mungedObject->getCtxMaxDisconnectionTimeF()?"checked":"");
      $smarty->assign("CtxMaxIdleTimeF",
                      $this->mungedObject->getCtxMaxIdleTimeF()?"checked":"");

      /* Fill sambaUserWorkstations */
      $ws= split(",", $this->sambaUserWorkstations);
      sort($ws);
      
      /* Tidy checks for empty option, and smarty will produce one if array[0]="" */
      if(($ws[0]=="")&&(count($ws)==1)) $ws=array();


      $smarty->assign("workstations", $ws);
    }

    /* Variables */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
    }

    /* 'sambaAcctFlags' checkboxes */
    /* Check for 'lock-account'-flag: 'D' or 'L' */
    if (is_integer(strpos($this->sambaAcctFlags, "D")) ||
        is_integer(strpos($this->sambaAcctFlags, "L"))) {
        $smarty->assign("flagsD", "checked");
    } else {
        $smarty->assign("flagsD", "");
    }
    
    /* Check for no_password_required flag 'N' */
    if (is_integer(strpos($this->sambaAcctFlags, "N"))) {
        $smarty->assign("flagsN", "checked");
    } else {
        $smarty->assign("flagsN", "");
    }

    if($this->samba3){
      if ($this->sambaPwdCanChange=="1"){
        $smarty->assign("flagsP", "checked");
      } else {
        $smarty->assign("flagsP", "");
      }
    }else{
      if ($this->pwdCanChange=="1"){
        $smarty->assign("flagsP", "checked");
      } else {
        $smarty->assign("flagsP", "");
      }
    }

    if ($this->password_expires=="1"){
      $smarty->assign("flagsC", "checked");
    } else {
      $smarty->assign("flagsC", "");
    }
    if ($this->logon_time_set=="1"){
      $smarty->assign("flagsT", "checked");
    } else {
      $smarty->assign("flagsT", "");
    }
    if ($this->logoff_time_set=="1"){
      $smarty->assign("flagsO", "checked");
    } else {
      $smarty->assign("flagsO", "");
    }
    if ($this->kickoff_time_set=="1"){
      $smarty->assign("flagsK", "checked");
    } else {
      $smarty->assign("flagsK", "");
    }
   

    /* In case of javascript, disable some fields on demand */
    if ($this->samba3){
      foreach($this->mungedObject->getOnDemandFlags() as $key => $value) {
        $smarty->assign("$key", "$value");
      }
    }


    foreach($this->attributes as $attr){
      if(in_array($attr,$this->multi_boxes)){
        $smarty->assign("use_".$attr,TRUE);
      }else{
        $smarty->assign("use_".$attr,FALSE);
      }
    }
    foreach(array("allow_pwchange","tslogin","CtxWFHomeDir","CtxWFHomeDirDrive","CtxWFProfilePath",
          "inherit","CtxWorkDirectory","CtxInitialProgram","CtxMaxConnectionTimeF","CtxMaxConnectionTime","CtxMaxDisconnectionTimeF",
          "CtxMaxDisconnectionTime","CtxMaxIdleTimeF","CtxMaxIdleTime","connectclientdrives",
          "onnectclientprinters","defaultprinter","shadow","brokenconn",
          "reconn","allow_pwchange","connectclientprinters","no_password_required","temporary_disable", 
          "password_expires","logon_time_set","logoff_time_set","kickoff_time_set","SetSambaLogonHours",
          "workstation_list") as $attr){
      if(in_array($attr,$this->multi_boxes)){
        $smarty->assign("use_".$attr,TRUE);
      }else{
        $smarty->assign("use_".$attr,FALSE);
      }
    }

    /* Show main page */
    $smarty->assign("multiple_support",$this->multiple_support_active);
    if ($this->samba3){
      $display.= $smarty->fetch (get_template_path('samba3.tpl', TRUE, dirname(__FILE__)));
    } else {
      $display.= $smarty->fetch (get_template_path('samba2.tpl', TRUE, dirname(__FILE__)));
    }

    return ($display);
  }

  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
   if (!$this->initially_was_account){
     return;
   }
    
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    plugin::remove_from_parent();

    /* Keep uid attribute for gosaAccount */
    unset($this->attrs['uid']);
    unset($this->attrs['uidNumber']);
    unset($this->attrs['gidNumber']);

    /* Remove objectClass for sambaIdmapEntry */
    $tmp= array();
    for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
      if ($this->attrs['objectClass'][$i] != 'sambaIdmapEntry'){
        $tmp[]= $this->attrs['objectClass'][$i];
      }
    }
    $this->attrs['objectClass']= $tmp;

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/samba account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove", array("uid" => $this->uid));
  }


  /* Check for input problems */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if ($this->samba3){

      /* Strings */
      foreach (array( "sambaHomePath" => _("Home directory"),
            "sambaProfilePath" => _("Profile path")) as $key => $val){
        if (!$this->mungedObject->is_samba_path($this->$key)){
          $message[]= sprintf(_("The value specified as '%s' contains invalid characters!"), $val);
        }
      }

      /* Numeric values */
      foreach (array(	"CtxMaxConnectionTime" => _("Connection"),
            "CtxMaxDisconnectionTime" => _("Disconnection"),
            "CtxMaxIdleTime" => _("IDLE")) as $key => $val){

        if (isset($this->mungedObject->ctx[$key]) && !is_id($this->mungedObject->ctx[$key]) && $val != 0){
          $message[]= sprintf(_("The timeout property '%s' is checked and contains invalid or no characters!"), $val);
        }
      }

      /* Too many workstations? Windows usrmgr only supports eight */
      if (substr_count($this->sambaUserWorkstations, ",") >= 8){
        $message[]= _("The windows user manager only allows eight clients. You've specified more than eight.");
      }
    }

    return ($message);
  }


  /* Save data to object */
  function save_object()
  {

    $SkipWrite = (!isset($this->parent) || !$this->parent) && !isset($_SESSION['edit']);

    /* We only care if we are on the sambaTab... */
    if (isset($_POST['sambaTab'])){
      plugin::save_object();

      /* Take care about access options */
      if ($this->acl_is_writeable("sambaAcctFlagsL",$SkipWrite) || ($this->acl_is_writeable("sambaAcctFlagsN",$SkipWrite))){
        if ($this->samba3){
          $attrname= "sambaPwdCanChange";
        } else {
          $attrname= "pwdCanChange";
        }
        if (isset($_POST["allow_pwchange"]) && $_POST["allow_pwchange"] == 1){
          $tmp= 1;
        } else {
          $tmp= 0;
        }
        if ($this->$attrname != $tmp){
          $this->is_modified= TRUE;
        }
        $this->pwdCanChange= $tmp;
        $this->sambaPwdCanChange= $tmp;
      }
      $tmp= "UX";
      if (isset($_POST["no_password_required"])){
        if ($_POST["no_password_required"] == 1){
          $tmp.= "N";
        }
      }
      if (isset($_POST["password_expires"])){
        if ($_POST["password_expires"] == 1){
          $this->password_expires= 1;
        }
      } else {
        $this->password_expires= 0;
      }
      if (isset($_POST["temporary_disable"])){
        if ($_POST["temporary_disable"] == 1){
          if (is_integer(strpos($this->sambaAcctFlags, "L"))) {
            $tmp.= "L";
          } else {
            $tmp.= "D";
          }
        }
      }
      if (isset($_POST["logon_time_set"])){
        if ($_POST["logon_time_set"] == 1){
          $this->logon_time_set= 1;
        }
      } else {
        $this->logon_time_set= 0;
      }
      if (isset($_POST["logoff_time_set"])){
        if ($_POST["logoff_time_set"] == 1){
          $this->logoff_time_set= 1;
        }
      } else {
        $this->logoff_time_set= 0;
      }
      if (isset($_POST["kickoff_time_set"])){
        if ($_POST["kickoff_time_set"] == 1){
          $this->kickoff_time_set= 1;
        }
      } else {
        $this->kickoff_time_set= 0;
      }
      
      $fill= "";
      for ($i= strlen($tmp); $i<12; $i++){
        $fill.= " ";
      }

      $tmp= "[$tmp$fill]";

      /* Only save if acl's are set */
      if ($this->acl_is_writeable("sambaAcctFlagsL",$SkipWrite) || ($this->acl_is_writeable("sambaAcctFlagsN",$SkipWrite))){
        if ($this->samba3){
          $attrname= "sambaAcctFlags";
        } else {
          $attrname= "acctFlags";
        }
        if ($this->$attrname != $tmp){
          $this->is_modified= TRUE;
        }
        $this->$attrname= $tmp;
      }

      /* Save sambaDomain attribute */
      if ($this->acl_is_writeable("sambaDomainName",$SkipWrite) && $this->samba3 && isset ($_POST['sambaDomainName'],$SkipWrite)){
        $this->sambaDomainName= validate($_POST['sambaDomainName']);
      }

      /* Save CTX values */
      if ($this->samba3){

        /* Save obvious values */
        foreach($this->ctxattributes as $val){
          if (isset($_POST[$val]) && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite)){
            if (get_magic_quotes_gpc()) {
              $this->mungedObject->ctx[$val]= stripcslashes(validate($_POST[$val]));
            } else {
              $this->mungedObject->ctx[$val]= validate($_POST[$val]);
            }
          }
        }

        /* Save checkbox states. */
        $this->mungedObject->setTsLogin(!isset($_POST['tslogin'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        // Need to do some index checking to avoid messages like "index ... not found"
        if(isset($_POST['brokenconn'])) {
          $this->mungedObject->setBrokenConn($_POST['brokenconn'] == '1'
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        }
        if(isset($_POST['reconn'])) {
          $this->mungedObject->setReConn($_POST['reconn'] == '1'
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        }
        $this->mungedObject->setInheritMode(isset($_POST['inherit'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setCtxMaxConnectionTimeF(!isset($_POST['CtxMaxConnectionTimeF'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setCtxMaxDisconnectionTimeF(
                        !isset($_POST['CtxMaxDisconnectionTimeF']) 
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setCtxMaxIdleTimeF(!isset($_POST['CtxMaxIdleTimeF'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setConnectClientDrives(isset($_POST['connectclientdrives'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setConnectClientPrinters(isset($_POST['connectclientprinters'])  
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));
        $this->mungedObject->setDefaultPrinter(isset($_POST['defaultprinter'])
                        && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite));

        /* Save combo boxes. Takes two values */
        if(isset($_POST['reconn'])) {
          $this->mungedObject->setShadow(isset($_POST['shadow']) && $this->acl_is_writeable("AllowLoginOnTerminalServer",$SkipWrite),$_POST['shadow']);
        }

        /* Check for changes */
        if ($this->sambaMungedDial != $this->mungedObject->getMunged()){
          $this->is_modified= TRUE;
        }
      }
    }
  }


  /* Save to LDAP */
  function save()
  {
    /* Load uid and gid of this 'dn' */
    $ldap= $this->config->get_ldap_link();
    $ldap->cat($this->dn, array('uidNumber', 'gidNumber'));
    $tmp= $ldap->fetch();
    $this->uidNumber= $tmp['uidNumber'][0];
    $this->gidNumber= $tmp['gidNumber'][0];

    plugin::save();

    /* Remove objectClass for sambaIdmapEntry */
    $tmp= array();
    for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
      if ($this->attrs['objectClass'][$i] != 'sambaIdmapEntry'){
        $tmp[]= $this->attrs['objectClass'][$i];
      }
    }
    $this->attrs['objectClass']= $tmp;

    /* Generate rid / primaryGroupId */
    if ($this->samba3){
      if (!isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])){
        print_red (_("Warning: This account has an undefined samba SID assigned. The problem can not be fixed by GOsa!"));
      } else {
        $this->SID= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
        $this->ridBase= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'];
      }

      /* Need to generate a new uniqe uid/gid combination? */
      if ($this->sambaSID == "" || $this->orig_sambaDomainName != $this->sambaDomainName){
        $uidNumber= $this->uidNumber;
        while(TRUE){
          $sid= $this->SID."-".($uidNumber*2 + $this->ridBase);
          $ldap->cd($this->config->current['BASE']);
          $ldap->search("(sambaSID=$sid)", array("sambaSID"));
          if ($ldap->count() == 0){
            break;
          }
          $uidNumber++;
        }
        $this->attrs['sambaSID']= $sid;

        /* Check for users primary group */
        $ldap->cd($this->config->current['BASE']);
        $ldap->search("(&(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))", array("cn"));
        if ($ldap->count() != 1){
          print_red(_("Warning: Can't identify users primary group - no conversion to a samba group possible!"));
        } else {
          $attrs= $ldap->fetch();
          $g= new group($this->config, $ldap->getDN());
          if ($g->sambaSID == ""){
            $g->sambaDomainName= $this->sambaDomainName;
            $g->smbgroup= TRUE;
            $g->save ();
          }
          $this->attrs['sambaPrimaryGroupSID']= $g->sambaSID;
        }
      }

      if ($this->sambaHomeDrive == ""){
        $this->attrs["sambaHomeDrive"]= array();
      }

      /* Generate munged dial value */
      $this->attrs["sambaMungedDial"]= $this->mungedObject->getMunged();

      /* User wants me to fake the idMappings? This is useful for
         making winbind resolve the user names in a reasonable amount
         of time in combination with larger databases. */
      if (isset($this->config->current['SAMBAIDMAPPING']) &&
          preg_match('/true/i', $this->config->current['SAMBAIDMAPPING'])){
        $this->attrs['objectClass'][]= "sambaIdmapEntry";
      }


      /* Password expiery */
      if ($this->password_expires == "1"){
        $this->attrs['sambaPwdMustChange']= $this->sambaPwdMustChange;
      } else {
        $this->attrs['sambaPwdMustChange']= array();
      }
      /* Make sure not to save zero in sambaPwdLastset */
      if ($this->sambaPwdLastSet != "0"){
        $this->attrs['sambaPwdLastSet']= $this->sambaPwdLastSet;
      } else {
        $this->attrs['sambaPwdLastSet']= array();
      }
      /* Account expiery */
      if ($this->logon_time_set == "1"){
        $this->attrs['sambaLogonTime']= $this->sambaLogonTime;
      } else {
        $this->attrs['sambaLogonTime']= array();
      }
      if ($this->logoff_time_set == "1"){
        $this->attrs['sambaLogoffTime']= $this->sambaLogoffTime;
      } else {
        $this->attrs['sambaLogoffTime']= array();
      }
      if ($this->kickoff_time_set == "1"){
        # Add one day in unixtime format to be compatible with usrmgr
        //$this->attrs['sambaKickoffTime']= $this->sambaKickoffTime + 86400; 
        $this->attrs['sambaKickoffTime']= $this->sambaKickoffTime; //hickert 
      } else {
        $this->attrs['sambaKickoffTime']= array();
      }
    } else {
    /* Not samba3 */
      $this->attrs['rid']= $this->uidNumber*2 + 1000;
      $this->attrs['primaryGroupID']= $this->gidNumber*2 +1001;

      if ($this->homeDrive == ""){
        $this->attrs["homeDrive"]= array();
      }

      /* Password expiery */
      if ($this->password_expires == "1"){
        $this->attrs['pwdMustChange']= $this->pwdMustChange;
      } else {
        $this->attrs['pwdMustChange']= 2147483647;
      }
      /* Make sure not to save zero in pwdLastset */
      if ($this->pwdLastSet != "0"){
        $this->attrs['pwdLastSet']= $this->pwdLastSet;
      } else {
        $this->attrs['pwdLastSet']= array();
      }
      /* Account expiery */
      if ($this->logon_time_set == "1"){
        $this->attrs['logonTime']= $this->logonTime;
      } else {
        $this->attrs['logonTime']= array();
      }
      if ($this->logoff_time_set == "1"){
        $this->attrs['logoffTime']= $this->logoffTime;
      } else {
        $this->attrs['logoffTime']= array();
      }
      if ($this->kickoff_time_set == "1"){
        # Add one day in unixtime format to be compatible with usrmgr
        $this->attrs['kickoffTime']= $this->kickoffTime + 86400;
      } else {
        $this->attrs['kickoffTime']= array();
      }
    }

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/samba account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify", array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add", array("uid" => $this->uid));
    }
  }


  /* Force password set, if this account doesn't have any samba passwords  */
  function password_change_needed()
  {
    if(!$this->initially_was_account && $this->is_account){
      $ldap = $this->config->get_ldap_link();
      $ldap->cat($this->dn,array("sambaLMPassword","sambaNTPassword"));
      $attrs = $ldap->fetch();
      if(!isset($attrs['sambaLMPassword']) || !isset($attrs['sambaNTPassword'])){
        return(TRUE);
      }
    }
    return(FALSE);
  }


  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);
    $this->sambaSID= "";
    $this->sambaPrimaryGroupSID= "";

      /* Fill mungedDial field */
    if ($this->samba3 && isset($this->attrs['sambaMungedDial'])){
      $this->mungedObject->load($this->sambaMungedDial);
    }

    /* Password expiery */
    if(isset($this->attrs['sambaPwdMustChange']) &&
        $this->attrs['sambaPwdMustChange'][0] != 0){
      $this->password_expires= 1;
    }

    if(isset($this->attrs['sambaLogonTime']) && ! (
        $this->attrs['sambaLogonTime'][0] == 0 ||
        $this->attrs['sambaLogonTime'][0] == 2147483647
      )){
      $this->logon_time_set= 1;
    }
    if(isset($this->attrs['sambaLogoffTime']) && ! (
        $this->attrs['sambaLogoffTime'][0] == 0 ||
        $this->attrs['sambaLogoffTime'][0] == 2147483647
      )){
      $this->logoff_time_set= 1;
    }

    /* Account expiery */
    if(isset($this->attrs['sambaKickoffTime']) && ! (
        $this->attrs['sambaKickoffTime'][0] == 0 ||
        $this->attrs['sambaKickoffTime'][0] == 2147483647
      )){
      $this->kickoff_time_set= 1;
    }

    /* Get global filter config */
    if (!is_global("sambafilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $sambafilter= array( "depselect" => $base, "regex" => "*");
      register_global("sambafilter", $sambafilter);
    }
  }

  
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Samba"),
          "plDescription"   => _("Samba settings"),
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 5,
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "AllowLoginOnTerminalServer"  => _("Allow login on terminal server"),
            "InheritClientConfig"         => _("Inherit client config"),

            "sambaPwdCanChange"     => _("Allow user to change password") ,
            "sambaAcctFlagsN"            => _("Login from windows client requires no password"),
            "sambaAcctFlagsL"            => _("Lock samba account"),


            "sambaKickoffTime"      => _("Account expires") ,
            "sambaPwdMustChange"    => _("Password expires") ,

            "sambaLogonTime"        => _("Limit Logon Time") ,
            "sambaLogoffTime"       => _("Limit Logoff Time") ,
            "sambaLogonHours"       => _("Logon hours") ,

            "sambaHomePath"         => _("Generic home directory") ,
            "sambaHomeDrive"        => _("Generic samba home drive") ,
            "sambaLogonScript"      => _("Generic script path") ,
            "sambaProfilePath"      => _("Generic profile path") ,
            "sambaDomainName"       => _("Domain") ,
            "sambaUserWorkstations" => _("Allow connection from")))
              );
  }    

  function enable_multiple_support()
  {
    plugin::enable_multiple_support();
    if($this->samba3){
      $this->multiple_support_active = TRUE;
    }else{
      $this->multiple_support_active = FALSE;
    }
  } 

  function multiple_save_object()
  {
    $this->save_object();
    plugin::multiple_save_object();
    foreach(array("allow_pwchange","tslogin","CtxWFHomeDir","CtxWFHomeDirDrive","CtxWFProfilePath",
          "inherit","CtxWorkDirectory","CtxInitialProgram","CtxMaxConnectionTimeF","CtxMaxConnectionTime","CtxMaxDisconnectionTimeF",
          "CtxMaxDisconnectionTime","CtxMaxIdleTimeF","CtxMaxIdleTime","connectclientdrives",
          "onnectclientprinters","defaultprinter","shadow","brokenconn",
          "reconn","allow_pwchange","connectclientprinters","no_password_required","temporary_disable",
          "password_expires","logon_time_set","logoff_time_set","kickoff_time_set","SetSambaLogonHours",
          "workstation_list") as $attr){
      if(isset($_POST["use_".$attr])){
        $this->multi_boxes[] = $attr;
      }
    }
  }

  function multiple_execute()
  {
    return($this->execute());
  } 

  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();

    /* Terminal Server  */
    if(in_array("tslogin",$this->multi_boxes)){
      $ret['tslogin'] = $this->mungedObject->getTsLogin();
    }
    if(in_array("CtxWFHomeDirDrive",$this->multi_boxes)){
      $ret['CtxWFHomeDirDrive'] = $this->mungedObject->ctx['CtxWFHomeDirDrive'];
    }
    if(in_array("CtxWFHomeDir",$this->multi_boxes)){
      $ret['CtxWFHomeDir'] = $this->mungedObject->ctx['CtxWFHomeDir'];
    }
    if(in_array("CtxWFProfilePath",$this->multi_boxes)){
      $ret['CtxWFProfilePath'] = $this->mungedObject->ctx['CtxWFProfilePath'];
    }

    if(in_array("inherit",$this->multi_boxes)){
      $ret['inherit'] = $this->mungedObject->getInheritMode();
    }       
    if(in_array("CtxInitialProgram",$this->multi_boxes)){
      $ret['CtxInitialProgram'] = $this->mungedObject->ctx['CtxInitialProgram'];
    } 
    if(in_array("CtxWorkDirectory",$this->multi_boxes)){
      $ret['CtxWorkDirectory'] = $this->mungedObject->ctx['CtxWorkDirectory'];
    } 

    /* Time Limits. Be careful here, there are some negations  */
    if(in_array("CtxMaxConnectionTimeF",$this->multi_boxes)){
      $ret["CtxMaxConnectionTimeF"]   =  !$this->mungedObject->getCtxMaxConnectionTimeF();
      if(!$ret["CtxMaxConnectionTimeF"]){
        $ret["CtxMaxConnectionTime"]   =  $this->mungedObject->ctx['CtxMaxConnectionTime'];
      }
    }
    if(in_array("CtxMaxDisconnectionTimeF",$this->multi_boxes)){
      $ret["CtxMaxDisconnectionTimeF"]=  !$this->mungedObject->getCtxMaxDisconnectionTimeF();
      if(!$ret["CtxMaxDisconnectionTimeF"]){
        $ret["CtxMaxDisconnectionTime"]=  $this->mungedObject->ctx['CtxMaxDisconnectionTime'];
      }
    }
    if(in_array("CtxMaxIdleTimeF",$this->multi_boxes)){
      $ret["CtxMaxIdleTimeF"]         =  !$this->mungedObject->getCtxMaxIdleTimeF();
      if(!$ret["CtxMaxIdleTimeF"]){
        $ret["CtxMaxIdleTime"]         =  $this->mungedObject->ctx['CtxMaxIdleTime'];
      }
    }

    /* Client Devices */
    if(in_array("connectclientdrives",$this->multi_boxes)){
      $ret["connectclientdrives"]     =  $this->mungedObject->getConnectClientDrives();
    }
    if(in_array("connectclientprinters",$this->multi_boxes)){
      $ret["connectclientprinters"]   =  $this->mungedObject->getConnectClientPrinters();
    }
    if(in_array("defaultprinter",$this->multi_boxes)){
      $ret["defaultprinter"]          =  $this->mungedObject->getDefaultPrinter();
    }

    /* Misc */
    if(in_array("shadow",$this->multi_boxes)){
      $ret["shadow"]    =$this->mungedObject->getShadow();
    }
    if(in_array("brokenconn",$this->multi_boxes)){
      $ret["brokenconn"]=$this->mungedObject->getBrokenConn();
    }
    if(in_array("reconn",$this->multi_boxes)){
      $ret["reconn"]    =$this->mungedObject->getReConn();
    }

    print_a($ret);
    return($ret);
  }

  function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);

    /* Enable disabled terminal login, this is inverted somehow */
    if(isset($values['tslogin']))   $this->mungedObject->setTsLogin(!$values['tslogin']);
  
    /* Imherit client configuration */
    if(isset($values['inherit']))   $this->mungedObject->setInheritMode($values['inherit']);
  
    /* Get all ctx values posted */
    $ctx = array("CtxWFHomeDirDrive","CtxWFHomeDir","CtxWFProfilePath","CtxInitialProgram","CtxWorkDirectory",
                 "CtxMaxConnectionTime","CtxMaxDisconnectionTime","CtxMaxIdleTime");
    foreach($ctx as $attr){
      if(isset($values[$attr])){
        $this->mungedObject->ctx[$attr] = $values[$attr] ;
      }
    }

    if(isset($values['CtxMaxConnectionTimeF']))   $this->mungedObject->setCtxMaxConnectionTimeF($values['CtxMaxConnectionTimeF']);
    if(isset($values['CtxMaxDisconnectionTimeF']))$this->mungedObject->setCtxMaxDisconnectionTimeF($values['CtxMaxDisconnectionTimeF']);
    if(isset($values['CtxMaxIdleTimeF']))         $this->mungedObject->setCtxMaxIdleTimeF($values['CtxMaxIdleTimeF']);

    if(isset($values['connectclientdrives']))   $this->mungedObject->setConnectClientDrives($values['connectclientdrives']);
    if(isset($values['connectclientprinters'])) $this->mungedObject->setConnectClientPrinters($values['connectclientprinters']);
    if(isset($values['defaultprinter']))        $this->mungedObject->setDefaultPrinter($values['defaultprinter']);

    if(isset($values['shadow']))        $this->mungedObject->setShadow($values['shadow'],$values['shadow']);
    if(isset($values['brokenconn']))    $this->mungedObject->setBrokenConn($values['brokenconn'],$values['brokenconn']);
    if(isset($values['reconn']))        $this->mungedObject->setReConn($values['reconn'],$values['reconn']);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
