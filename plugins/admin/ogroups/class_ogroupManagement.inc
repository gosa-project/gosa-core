<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004 Cajus Pollmeier

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
require "tabs_ogroups.inc";

class ogroupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Object groups";
  var $plDescription= "This does something";

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();
  var $obtypes= array();
  var $ogroup;

  var $CopyPasteHandler = NULL;
  var $DivListOGroup    = NULL;

  function ogroupManagement ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;
    $this->ui= get_userinfo();

    /* Copy & Paste enabled ?
     */
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE'] ))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }

    /* Div lsit management */
    $this->DivListOGroup = new divListOGroup($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /****************
      Variable intialisation && Check posts for commands  
     ****************/

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^group_edit_/","/^group_del_/");

    $smarty     = get_smarty();
    $s_action   = "";
    $s_entry    = "";

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/^group_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/^group_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/^group_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/^group_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^group_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^group_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/^group_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/group_chgpw_/i","",$key);
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }elseif(preg_match("/_group_edit_/",$key)){
        $type = preg_replace("/_group_edit_.*$/","",$key);
        $s_action="edit";
        $s_entry  = preg_replace("/".$type."_group_edit_/i","",$key);
        $_POST['arg'] = $type;
      }
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    // Edit if
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }


    /****************
      Copy & Paste handling 
     ****************/

    /* Only perform copy / paste if it is enabled
     */


    /********************
      Copy & Paste Handling  ...
     ********************/

    /* Only perform copy&paste requests if it is enabled
     */
    /* Get 'dn' from posted 'uid' */
    if(in_array_ics($s_action,array("editPaste","cut","copy")) || (isset($this->CopyPasteHandler) &&  $this->CopyPasteHandler->stillOpen())){

      if(isset($this->ogrouplist[trim($s_entry)]['dn'])){
        $dn= $this->ogrouplist[trim($s_entry)]['dn'];
      }else{
        $dn = $this->DivListOGroup->selectedBase;
      }

      $acl= get_permissions ($dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "ogroup", $dn);

      if($acl != "#all#"){
        print_red (_("You are not allowed to execute this method!"));
      }else{
        /* Display the copy & paste dialog, if it is currently open */
        $ret = $this->copyPasteHandling($s_action,$s_entry);
        if($ret){
          return($ret);
        }
      }
    }

    /****************
      Create a new object group 
     ****************/

    /* New group? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->ogroup= new ogrouptabs($this->config,
          $this->config->data['TABS']['OGROUPTABS'], $this->dn);
    }


    /****************
      Delete object group
     ****************/

    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->ogrouplist[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "ogroup", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the object group '%s'."), @LDAP::fix($this->dn)));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this object group!"));
      }
    }


    /****************
      Delete confirmed 
     ****************/

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->ogroup= new ogrouptabs($this->config,
            $this->config->data['TABS']['OGROUPTABS'], $this->dn);
        $this->ogroup->set_acl(array($this->acl));
        $this->ogroup->delete ();
        gosa_log ("Object group'".$this->dn."' has been removed");
        unset ($this->ogroup);
        $this->ogroup= NULL;
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this object group!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /****************
      Delete object group Canceled
     ****************/

    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /****************
      Edit group
     ****************/

    if (($s_action=="edit") && (! isset($this->ogroup->config)) ){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->ogrouplist[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Register grouptab to trigger edit dialog */
      $this->ogroup= new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'],
          $this->dn);
      $this->ogroup->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }


    /****************
      Edit finished save 
     ****************/

    /* Finish button has been pressed */
    if ((isset($_POST['edit_finish'])) && (isset($this->ogroup->config)) ){

      /* Check tabs, will feed message array */
      $message= $this->ogroup->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->ogroup->save();
        gosa_log ("Object group'".$this->dn."' has been saved");

        /* Group has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        unset ($this->ogroup);
        $this->ogroup= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /****************
      Cancel edit object group
     ****************/

    /* Cancel dialogs */
    if ((isset($_POST['edit_cancel'])) && (isset($this->ogroup->dn))){
      del_lock ($this->ogroup->dn);
      unset ($this->ogroup);
      $this->ogroup= NULL;
      unset($_SESSION['objectinfo']);
    }


    /****************
      Display open dialogs 
     ****************/

    /* Show dialog if object is present */
    if (isset($this->ogroup->config)){
      $display= $this->ogroup->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->ogroup->by_object[$this->ogroup->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /****************
      Display list 
     ****************/

    /* Display dialog with group list */
    $this->DivListOGroup->parent = $this;
    $this->DivListOGroup->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListOGroup->SubSearch){
      $this->DivListOGroup->AddDepartments($this->DivListOGroup->selectedBase,4);
    }
    $this->reload ();
    $this->DivListOGroup->setEntries($this->ogrouplist);
    return($this->DivListOGroup->Draw());
  }


  function convert_list($input)
  {
    $temp= "";

    $conv= array(   
        "U" => array("select_user.png"        ,_("User")        , "ogroup"),
        "G" => array("select_groups.png"      ,_("Group")       , "ogroup"),
        "A" => array("select_application.png" ,_("Application") , "ogroup"),
        "D" => array("select_department.png"  ,_("Department")  , "ogroup"),
        "S" => array("select_server.png"      ,_("Server")      , "ogroup"),
        "F" => array("select_phone.png"       ,_("Phone")       , "phonequeue"),
        "W" => array("select_workstation.png" ,_("Workstation") , "workstartup"),
        "T" => array("select_terminal.png"    ,_("Terminal")    , "termgroup"),
        "P" => array("select_printer.png"     ,_("Printer")     , "ogroup"));

    /* Assemble picture */
    $type= $input['gosaGroupObjects'][0];
    $type= preg_replace("/[^A-Z]/i","",$type);
    if (isset($type[0])){
      $p1['pic']= $conv[$type[0]][0];
      $p1['tit']= $conv[$type[0]][1];
      $p1['alt']= $type[0];
      $p1['lnk']= $conv[$type[0]][2];
    } else {
      $p1['pic']= "empty.png";
      $p1['tit']= "";
      $p1['alt']= "";
      $p1['lnk']= "";//$conv[$type[0]][2];
    }

    if (isset($type[1])){
      $p2['pic']= $conv[$type[1]][0];
      $p2['alt']= $type[1];
      $p2['tit']= $conv[$type[1]][1];
      $p2['lnk']= $conv[$type[1]][2];
    } else {
      $p2['pic']= "empty.png";
      $p2['alt']= "";
      $p2['tit']= "";
      $p2['lnk']= ""; //$conv[$type[0]][2];
    }
    $temp = "<input type='image' name='".$p1['lnk']."_group_edit_%KEY' class='center' 
      src='images/".$p1['pic']."' alt='".$p1['alt']."' title='".$p1['tit']."'>";
    $temp.= "<input type='image' name='".$p2['lnk']."_group_edit_%KEY'class='center' 
      src='images/".$p2['pic']."' alt='".$p2['alt']."' title='".$p2['tit']."'>";
    return ($temp);
  }


  function reload()
  {
    /* Set base for all searches && initialise some vars */
    $this->ogrouplist= array();
    $base     = $this->DivListOGroup->selectedBase;
    $filter   = "(gosaGroupObjects=[])";
    $Regex    = $this->DivListOGroup->Regex;

    $chk = array(
        "UserGroups"          => "(gosaGroupObjects=*U*)" ,
        "GroupGroups"         => "(gosaGroupObjects=*G*)" ,
        "ApplicationGroups"   => "(gosaGroupObjects=*A*)" ,
        "DepartmentGroups"    => "(gosaGroupObjects=*D*)" ,
        "ServerGroups"        => "(gosaGroupObjects=*S*)" ,
        "WorkstationGroups"   => "(gosaGroupObjects=*W*)" ,
        "TerminalGroups"      => "(gosaGroupObjects=*T*)" ,
        "PrinterGroups"       => "(gosaGroupObjects=*P*)" ,
        "PhoneGroups"         => "(gosaGroupObjects=*F*)" );
    /* Create filter */ 
    foreach($chk as $chkBox => $FilterPart){
      if($this->DivListOGroup->$chkBox){
        $filter .=  $FilterPart;
      }
    }
    $filter= "(&(cn=$Regex)(objectClass=gosaGroupOfNames)(|$filter))";

    if($this->DivListOGroup->SubSearch){
      $res= get_list($filter, $this->ui->subtreeACL, $base, array("cn","objectClass","gosaGroupObjects"), GL_SIZELIMIT | GL_SUBSEARCH);
    }else{
      $res= get_list($filter, $this->ui->subtreeACL, get_groups_ou().$base, array("cn","objectClass","gosaGroupObjects"), GL_SIZELIMIT );
    }

    $this->ogrouplist= $res;
    ksort ($this->ogrouplist);
    reset ($this->ogrouplist);
    $tmp=array();
    $tmp2 = array();
    foreach($this->ogrouplist as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
      $tmp2[strtolower($val['cn'][0]).$val['cn'][0]] = strtolower($val['cn'][0]).$val['cn'][0];
    }
    natcasesort($tmp2);
    $this->ogrouplist=array();
    foreach($tmp2 as $val){
      $this->ogrouplist[]=$tmp[$val];
    }
    reset ($this->ogrouplist);
  }

  function save_object()
  {
    $this->DivListOGroup->save_object();
  }

  
  function copyPasteHandling($s_action,$s_entry)
  {

    if($this->CopyPasteHandler){

      /* Paste copied/cutted object in here
       */
      if(($s_action == "editPaste") || ($this->CopyPasteHandler->stillOpen())){
        $this->CopyPasteHandler->save_object();
        $this->CopyPasteHandler->SetVar("base", $this->DivListOGroup->selectedBase);
        return($this->CopyPasteHandler->execute());
      }

      /* Copy current object to CopyHandler
       */
      if($s_action == "copy"){
        $this->CopyPasteHandler->Clear();
        $dn       =   $this->ogrouplist[$s_entry]['dn'];
        $obj      =   new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'], $dn);
        $objNew   =   new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'], "new");

        $types_of_tabs = $obj->by_object['ogroup']->gosaGroupObjects;
        $objNew->by_object['ogroup']->gosaGroupObjects = $types_of_tabs;    
        $objNew->reload($types_of_tabs);

        $this->CopyPasteHandler->Copy($obj,$objNew);
      }

      /* Copy current object to CopyHandler
       */
      if($s_action == "cut"){
        $this->CopyPasteHandler->Clear();
        $dn       =   $this->ogrouplist[$s_entry]['dn'];
        $obj      =   new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'], $dn);
        $this->CopyPasteHandler->Cut($obj);
      }
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
