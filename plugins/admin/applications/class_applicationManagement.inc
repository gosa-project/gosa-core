<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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
require "tabs_application.inc";

class applicationManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Applications";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $apptabs= NULL;
  var $applications= array();
  var $ui= NULL;
  var $acl= "";

  var $CopyPasteHandler ;

  var $Release  = "";
  var $Releases = array();

  var $enableCopyPaste         = false;
  var $enableReleaseManagement = false;


  function applicationManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    if( (isset($this->config->data['MAIN']['ENABLECOPYPASTE'])) 
        && 
        (preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE'] ))){
      $this->enableCopyPaste = true;
    }

    $this->CopyPasteHandler = new CopyPasteHandler($this->config);

    /* Check if we should enable the release selection */
    $tmp = search_config($this->config->data,"faiManagement","CLASS");
    if(!empty($tmp)){
      $this->enableReleaseManagement = true;
    }

    /* Get global filter config */
    if (!is_global("appfilter")){
      $base= get_base_from_people($ui->dn);

      $appfilter= array("depselect" => $base,
                        "regex" => "*" ,
                        "release" => "ou=apps,".$base);

      register_global("appfilter", $appfilter);
    }

    $appfilter = get_global("appfilter");
    $this->Releases = $this->getReleases($appfilter['depselect']);
    $this->Release  = $appfilter['release'];
  }

  function getReleases($base)
  {
    $dn   = "ou=apps,".$base;

    $ret  = array();

    $ret ["ou=apps,".$base] = "/";

    $ldap = $this->config->get_ldap_link();
    $ldap->cd($dn);

    $ldap->search("objectClass=organizationalUnit",array("ou"));

    while($attrs = $ldap->fetch()){
      $str = str_replace($dn,"",$attrs['dn']);
      $tmp = array_reverse( split("ou=",$str));
      $str = "";
      foreach($tmp as $val){
        $val = trim(preg_replace("/,/","",$val));
        if(empty($val)) break;
        $str .= "/".$val;
      } 
      if(!empty($str)){
        $ret[$attrs['dn']]= preg_replace("/^\//","",$str);
      }
    }
    return($ret);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^appl_edit_/","/^appl_del_/");

    /* Save data */
    $appfilter    = get_global("appfilter");
    $smarty       = get_smarty();             // Smarty instance
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    
    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
      $s_action="open";
      $s_entry = base64_decode($_GET['dep_id']);
      $appfilter['depselect']= "".$this->config->departments[trim($s_entry)];
    }

    /* Get posted release */
    if(isset($_POST['select_release'])){
      $r = $_POST['select_release'];
      if((isset($this->Releases[$r])) && ($this->Release != $r)){
        $this->Release = $r;
        $appfilter = get_global("appfilter");
        $appfilter['release'] = $r;
        register_global("appfilter",$appfilter);      
      }
    }

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/appl_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/appl_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/appl_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/appl_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
        // Post for new
      }elseif(preg_match("/^dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/^appl_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/^dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/^dep_root.*/i",$key)){
        $s_action="root";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }
    }

    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Department changed? */
    if(isset($_POST['depselect']) && $_POST['depselect']){
      $appfilter['depselect']= $_POST['depselect'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $appfilter['depselect']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $appfilter['depselect']=(preg_replace("/^[^,]+,/","",$appfilter['depselect']));
    }

    if($s_action=="root"){
      $appfilter['depselect']=($this->config->current['BASE']);
    }

    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$appfilter['depselect']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $appfilter['depselect']= $this->config->departments[trim($base_back)];
      }else{
        $appfilter['depselect']= $this->config->departments["/"];
      }
    }

    if (isset($_POST['regex'])){
      $appfilter['regex']= $_POST['regex'];
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $appfilter['regex']= $s;
    }
    
    $this->Releases = $this->getReleases($appfilter['depselect']);
    if(!isset($this->Releases[$this->Release])){
      $this->Release = key($this->Releases);
      $appfilter['release'] = $this->Release;
      register_global("appfilter",$appfilter);
    }

    register_global("appfilter", $appfilter);

    /* Check sorting variable */
    $this->reload();
    $smarty= get_smarty();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }


    /* Only perform copy / paste if it is enabled 
     */
    if($this->enableCopyPaste){

      /* Paste copied/cutted object in here
       */
      if(($s_action == "editPaste") || ($this->CopyPasteHandler->stillOpen())){
        $this->CopyPasteHandler->save_object();
        $this->CopyPasteHandler->SetVar("base",$appfilter['depselect']);
        return($this->CopyPasteHandler->execute());
      }


      /* Copy current object to CopyHandler
       */
      if($s_action == "copy"){
        $this->CopyPasteHandler->Clear();
        $dn = $this->applications[$s_entry]['dn'];
        $obj    = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], $dn);
        $objNew = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], "new");
        $this->CopyPasteHandler->Copy($obj,$objNew);
      }


      /* Copy current object to CopyHandler
       */
      if($s_action == "cut"){
        $this->CopyPasteHandler->Clear();
        $dn = $this->applications[$s_entry]['dn'];
        $obj = new apptabs($this->config, $this->config->data['TABS']['APPSTABS'], $dn);
        $this->CopyPasteHandler->Cut($obj);
      }
    }

    /* New application? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->apptabs= new apptabs($this->config,
          $this->config->data['TABS']['APPSTABS'], $this->dn);
      $this->apptabs->set_acl(array(':all'));
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->apptabs->dn);
      unset ($this->apptabs);
      $this->apptabs= NULL;
      unset ($_SESSION['objectinfo']);
    }

    /* Finish apps edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->apptabs->config))){

      /* Check tabs, will feed message array */
      $this->apptabs->last= $this->apptabs->current;
      $this->apptabs->save_object();
      $message= $this->apptabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->apptabs->set_release($this->Release);
        $this->apptabs->save();
        gosa_log ("Application object'".$this->dn."' has been saved");

        /* Application has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new apps at
           this point. */
        $this->reload ();
        unset ($this->apptabs);
        $this->apptabs= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->apptabs->config))){

      /* Get 'dn' from posted 'applist', must be unique */
      $this->dn= $this->applications[$s_entry]['dn'];

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

      /* Register apptabs to trigger edit dialog */
      $this->apptabs= new apptabs($this->config,
          $this->config->data['TABS']['APPSTABS'], $this->dn);
      $this->apptabs->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Remove user was requested */
    if ($s_action == "del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->applications[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "application", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the application '%s'."), LDAP::fix($this->dn)));
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this application!"));
      }
    }

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_app_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->apptabs= new apptabs($this->config,
            $this->config->data['TABS']['APPSTABS'], $this->dn);
        $this->apptabs->set_acl(array($this->acl));
        $this->apptabs->delete ();
        gosa_log ("Application object'".$this->dn."' has been removed");
        unset ($this->apptabs);
        $this->apptabs= NULL;

        /* Group list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this application!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /* Delete application canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if (($this->apptabs) && (isset($this->apptabs->config))){
      $display= $this->apptabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->apptabs->by_object[$this->apptabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Show main page */

    /* Prepare departments */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($appfilter['depselect'] == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }

    if($this->enableCopyPaste){
      $Copy_Paste = " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;";

      if($this->CopyPasteHandler->isCurrentObjectPastAble()){

        if($this->CopyPasteHandler->isCurrentCutted()){
          $img = "images/cutpaste.png";
        }else{
          $img = "images/copypaste.png";
        }

        $Copy_Paste .= "<input type='image' name='editPaste' class='center' 
          src='".$img."' alt='"._("Paste")."' title='".$this->CopyPasteHandler->GetCurrentDn()."'>&nbsp;";
      }else{
        $Copy_Paste .= "<img class='center' src='images/cant_editpaste.png' alt='"._("Can't paste")."'>&nbsp;";
      }
    }else{
      $Copy_Paste ="";
    }

    // Managment
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input class='center' type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input class='center' type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input class='center' type='image' align='middle' src='images/list_new_app.png' alt='"._("new")."' title='"._("Create new application")."' name='appl_new'>&nbsp;".
      $Copy_Paste.
      " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Base")."&nbsp;<select name='depselect' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";



    if($this->enableCopyPaste){
      $actions = "<input class='center' type='image' 
        src='images/editcut.png' alt='"._("cut")."' name='cut_%KEY%' title='"._("Cut this entry")."'>&nbsp;";
      $actions.= "<input class='center' type='image' 
        src='images/editcopy.png' alt='"._("copy")."' name='copy_%KEY%' title='"._("Copy this entry")."'>&nbsp;";
      $actions.= "<input class='center' type='image' 
        src='images/edit.png' alt='"._("edit")."' name='appl_edit_%KEY%' title='"._("Edit this entry")."'>";
      $actions.= "<input class='center' type='image' 
        src='images/edittrash.png' alt='"._("delete")."' name='appl_del_%KEY%' title='"._("Delete this entry")."'>";
    }else{

      $actions = "<input class='center' type='image' 
        src='images/edit.png' alt='"._("edit")."' name='appl_edit_%KEY%' title='"._("Edit this entry")."'>";
      $actions.= "<input class='center' type='image' 
        src='images/edittrash.png' alt='"._("delete")."' name='appl_del_%KEY%' title='"._("Delete this entry")."'>";
    }


    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    // image Buttons
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img class='center' src='images/select_groups.png' alt='User'    title='%s'>";

    // Extension images
    $applimg  = "<img class='center' src='images/select_application.png' alt='A'  title='"._("Application")."'>";

    // Space
    $empty    = "<img class='center' src='images/empty.png' style='width:16px;height:16px;' alt=''>";


    $divlist = new divlist("applicationtabs");
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("Application name")." / "._("Department"), "attach" => "style=''"),
          array("string" => _("Actions"), "attach" => "style='width:80px;border-right:0px;text-align:right;'")
          ));


    $divlist->SetSummary(_("This table displays all groups, in the selected tree."));
    $divlist->SetEntriesPerPage(0);

    foreach($this->departments as $key=> $val){

      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      $non_empty="";
      $keys= str_replace("/","\/",$key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/".$keys."\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style=''");
      $field3 = array("string" => "&nbsp;", "attach" => "style='width:80px;border-right:0px;text-align:right;'");

      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    foreach($this->applications as $key => $val){
      $title = "title='dn : ".$val['dn']."'";

      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }
      $field1 = array("string" => sprintf($applimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,($val['cn']['0'].$desc)), "attach" => "style='' ".$title);
      $field3 = array("string" => preg_replace("/%KEY%/", $key, $actions), "attach" => "style='width:80px;border-right:0px;text-align:right;'");

      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    $smarty->assign("applicationshead", $listhead);
    $smarty->assign("applications", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("releaseimage"   , get_template_path('images/branch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    $smarty->assign("regex", $appfilter['regex']);

    if($this->enableReleaseManagement){
      $smarty->assign("releases",     $this->Releases );
      $smarty->assign("releaseKeys",   array_flip($this->Releases));
      $smarty->assign("select_release",$this->Release);
    }

    $smarty->assign("enableReleaseManagement",$this->enableReleaseManagement);

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function reload()
  {
    /* Get config */
    $appfilter= get_global('appfilter');

    /* Set base for all searches */
    $base= $this->Release;
    
    /* Regex filter? */
    if ($appfilter['regex'] != ""){
      $regex= $appfilter['regex'];
    } else {
      $regex= "*";
    }

    /* Generate application list */
    $res= get_list($this->ui->subtreeACL, "(&(cn=$regex)(objectClass=gosaApplication))", FALSE, $base, array("*"), FALSE);
    $this->applications= array();
    foreach ($res as $value){
      $this->applications[]= $value;
    }
    reset ($this->applications);



    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */
    $peopleOU = get_people_ou();

    $base2 = $appfilter['depselect'];

    $res3 =  get_list2($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
        TRUE, $base2, array("ou", "description"), TRUE);

    $this->departments= array();
    $tmp = array();
    foreach ($res3 as $value){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if(isset($value["description"][0])){
        $this->departments[$value['dn']]=convert_department_dn2($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]=convert_department_dn2($value['dn']);//$value["description"][0];
      }
    }


    /* END NEW LIST MANAGMENT
     */

    $tmp=array();
    foreach($this->applications as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->applications=array();
    foreach($tmp as $val){
      $this->applications[]=$val;
    }
    reset ($this->applications);


  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  }


  /* Check values */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

  function adapt_from_template($dn)
  {
  }

  function password_change_needed()
  {
  }

  function show_header($button_text, $text, $disabled= FALSE)
  {
  }

  function remove_lock()
  {
    if (isset($this->apptabs->dn)){
      del_lock ($this->apptabs->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
