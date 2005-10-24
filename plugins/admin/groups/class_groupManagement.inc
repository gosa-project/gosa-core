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
require "tabs_group.inc";

class groupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Groups";
  var $plDescription= "This does something";
  var $departments = array();

  /* Dialog attributes */
  var $grouptab= NULL;
  var $grouplist= array();
  var $ui= NULL;
  var $acl= "";

  function groupManagement ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("groupfilter")){
      $base= get_base_from_people($ui->dn);
      $groupfilter= array("primarygroups" => "checked",
          "mailgroups" => "checked",
          "sambagroups" => "checked",
          "appgroups" => "checked",
          "functionalgroups" => "checked",
          "guser" => "*",
          "subsearch" => "",
          "depselect" => $base,
          "regex" => "*");
      register_global("groupfilter", $groupfilter);
    }
  }

  function execute()
  {
    /* Save data */
    $groupfilter= get_global("groupfilter");
    $s_action   = "";
    $s_entry    = "";


    if (!isset($this->grouptab)){
      foreach( array("depselect", "guser", "regex") as $type){
        if (isset($_POST[$type])){
          $groupfilter[$type]= $_POST[$type];
        } 
      }
      if (isset($_POST['regex'])){
        foreach( array("primarygroups", "sambagroups", "mailgroups", "appgroups",
              "functionalgroups", "subsearch") as $type){

          if (isset($_POST[$type])) {
            $groupfilter[$type]= "checked";
          } else {
            $groupfilter[$type]= "";
          }
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $groupfilter['regex']= $s;
      }
      register_global("groupfilter", $groupfilter);
    }


    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/group_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/group_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/group_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/group_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/group_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/group_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/group_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/group_chgpw_/i","",$key);
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      }
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry); 

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
      $s_action="open";
      $s_entry = base64_decode($_GET['dep_id']);
      $groupfilter['depselect']= "".$this->config->departments[trim($s_entry)];
    }

    // Edit if
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    /* Department changed? */
    if(isset($_POST['depselect']) && $_POST['depselect']){
      $groupfilter['depselect']= $_POST['depselect'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $groupfilter['depselect']= (preg_replace("/^[^,]+,/","",$this->ui->dn));
      $groupfilter['depselect']= (preg_replace("/^[^,]+,/","",$groupfilter['depselect']));
    }

    if($s_action=="root"){
      $groupfilter['depselect']=($this->config->current['BASE']);
    }

    /* If Backbutton is Posted */
    if($s_action == "back"){
      $base_back= preg_replace("/^[^,]+,/","",$groupfilter['depselect']);
      $base_back= convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $groupfilter['depselect']= $this->config->departments[trim($base_back)];
      }else{
        $groupfilter['depselect']= $this->config->departments["/"];
      }
    }
    register_global("groupfilter", $groupfilter);

    $smarty= get_smarty();

    /* Prepare formular */
    $this->reload();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* New group? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->grouptab= new grouptabs($this->config,
          $this->config->data['TABS']['GROUPTABS'], $this->dn);
      $this->grouptab->set_acl(array(':all'));
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->grouptab->dn);
      unset ($this->grouptab);
      $this->grouptab= NULL;
      unset($_SESSION['objectinfo']);
    }

    /* Finish group edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if (isset($_POST['edit_finish'])){

      /* Check tabs, will feed message array */
      $message= $this->grouptab->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save user data to ldap */
        $this->grouptab->save();
        gosa_log ("Group object'".$this->dn."' has been saved");

        /* Group has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new users at
           this point. */
        $this->reload ();
        unset ($this->grouptab);
        $this->grouptab= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* User wants to edit data? */
    if ($s_action=="edit"){

      /* Get 'dn' from posted 'uid', must be unique */
      $this->dn= $this->grouplist[trim($s_entry)]['dn'];

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
      $this->grouptab= new grouptabs($this->config,
          $this->config->data['TABS']['GROUPTABS'], $this->dn);
      $this->grouptab->set_acl($acl);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Remove user was requested */
    if ($s_action=="del"){
      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->grouplist[trim($s_entry)]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "group", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the group '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this group!"));
      }
    }

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_group_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Delete request is permitted, perform LDAP action */
        $this->grouptab= new grouptabs($this->config,
            $this->config->data['TABS']['GROUPTABS'], $this->dn);
        $this->grouptab->set_acl(array($this->acl));
        $this->grouptab->delete ();
        gosa_log ("Group object'".$this->dn."' has been removed");
        unset ($this->grouptab);
        $this->grouptab= NULL;

        /* Group list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this group!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if ($this->grouptab){
      $display= $this->grouptab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->grouptab->by_object[$this->grouptab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }

    /* Prepare departments */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
            if ($groupfilter['depselect'] == $key){
                    $options.= "<option selected value='$key'>$value</option>";
            } else {
                    $options.= "<option value='$key'>$value</option>";
            }
    }

    // Managment
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."'
 alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input type='image' align='middle' src='images/list_new_group.png' title='"._("Create new group")."'
 alt='"._("New")."' name='group_new'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Current base")."&nbsp;<select name='depselect' onChange='mainform.submit()'>$options</select>".
       " <input type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."'   name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";

    
    $actions = "<input type='image' src='images/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";
    $actions.= "<input type='image' src='images/edittrash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";

    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    // image Buttons 
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img src='images/select_groups.png' alt='User'    title='%s'>";

    // Extension images 
    $posiximg = "<img src='images/select_groups.png'        alt='P'  title='"._("Posix")  ."'>";
    $mailimg  = "<img src='images/mailto.png'             alt='M'  title='"._("Mail")   ."'>";
    $sambaimg = "<img src='images/select_winstation.png'  alt='S'  title='"._("Samba")  ."'>";
    $applimg  = "<img src='images/select_application.png' alt='A'  title='"._("Application")."'>"; 
    $phoneimg = "<img src='images/select_phone.png'      alt='Ph' title='"._("Phone")  ."'>"; 
    $envimg   = "<img src='images/network.png'      alt='E' title='"._("Environment")  ."'>"; 
    // Space
    $empty    = "<img src='images/empty.png' style='width:16px;height:16px;' alt=''>";


    $divlist = new divlist("groupstab");
    $divlist->SetHeader(array(
          array("string" => "&nbsp;"),
          array("string" => _("Groupname")." / "._("Department")),
          array("string" => _("Properties"), "attach" => "style='width:116px;'"),
          array("string" => _("Actions")    ,"attach" => "style='border:none'")
          ));


    $divlist->SetSummary(_("This table displays all groups, in the selected tree."));
    $divlist->SetEntriesPerPage(20);

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


      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val));
      $field3 = array("string" => "&nbsp;");
      $field4 = array("string" => "&nbsp;","attach"=>"style='text-align:right;border:none'");

      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }

    foreach($this->grouplist as $key => $val){

      if(isset($val['objectClass'])){
        if(in_array("posixGroup",           $val['objectClass']))   $posix = $posiximg;   else $posix   = $empty;
        if(in_array("gosaMailAccount",      $val['objectClass']))   $mail  = $mailimg;    else $mail    = $empty;
        if(in_array("sambaGroupMapping",    $val['objectClass']))   $samba = $sambaimg;   else $samba   = $empty;
        if(in_array("gosaApplicationGroup", $val['objectClass']))   $appl  = $applimg;    else $appl    = $empty;
        if(in_array("goFonPickupGroup",     $val['objectClass']))   $phone = $phoneimg;   else $phone   = $empty;
        if(in_array("gotoEnvironment",      $val['objectClass']))   $enviro= $envimg;     else $enviro  = $empty;
      }else{
        $posix=$mail=$samba=$appl=$phone=$empty;
      }

      $title = "title='gid : ".$key." - dn : ".$val['dn']."'";

      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }      
      $field1 = array("string" => sprintf($userimg,$val['dn']),"attach"=>"style='width:20px;align:middle;'");
      $field2 = array("string" => sprintf($editlink,$key,($val['cn']['0'].$desc)),"attach"=>$title);
      $field3 = array("string" => $posix."&nbsp;".$enviro."&nbsp;".$mail."&nbsp;".$samba."&nbsp;".$appl."&nbsp;".$phone);  
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions),"attach"=>"style='text-align:right;border:none;width:32px;'");
      
      $divlist->AddEntry(array($field1,$field2,$field3,$field4));    
    }

    /* Show main page */
    $smarty->assign("grouplisthead", $listhead);
    $smarty->assign("grouplist", $divlist->DrawList());
    $smarty->assign("searchg_image", get_template_path('images/search_group.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    foreach( array("depselect", "guser", "regex", "primarygroups", "mailgroups","appgroups", "sambagroups", "functionalgroups", "subsearch") as $type){
      $smarty->assign("$type", $groupfilter[$type]);
    }

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }

  function reload()
  {
    /* Get config */
    $groupfilter= get_global('groupfilter');

    /* Set base for all searches */
    $base= $groupfilter['depselect'];

    /* Regex filter? */
    if ($groupfilter['regex'] != ""){
      $regex= $groupfilter['regex'];
    } else {
      $regex= "*";
    }

    /* User filter? */
    $filter= "";
    $error= "";
    $error2= "";
    $this->grouplist= array();

    /* What are primary groups? */
    $primaries= array();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($base);
    $ldap->search("(&(uid=$regex)(!(uid=*$))(objectClass=posixAccount)(gidNumber=*))", array("gidNumber", "cn"));
    show_ldap_error($ldap->get_error());
    while ($attrs= $ldap->fetch()){
      $primaries[$attrs['gidNumber'][0]]= $attrs['cn'][0];
    }

    if ($groupfilter['primarygroups'] == "checked"){
      $filter.= "(&(objectClass=posixGroup)(|";
      foreach ($primaries as $gidNumber => $cn){
        $filter.= "(gidNumber=$gidNumber)";
      }
      $filter.= "))";
    }
  
    /* Set filter depending on selection */
    $sfilter= "";
    if ($groupfilter['appgroups'] == "checked"){
      $filter.= "(objectClass=gosaApplicationGroup)";
    }
    if ($this->config->current['SAMBAVERSION'] == 3){
      if ($groupfilter['primarygroups'] != "checked"){
        $sfilter= "(objectClass=sambaGroupMapping)";
      } elseif ($groupfilter['sambagroups'] == "checked"){
        $filter.= "(objectClass=sambaGroupMapping)";
      }
    }

    if ($groupfilter['mailgroups'] == "checked"){
      $filter.= "(objectClass=gosaMailAccount)";
    }
    if ($groupfilter['functionalgroups'] == "checked"){
      $ldap->cd($base);
      $ldap->set_size_limit($_SESSION['size_limit']);
      $ldap->ls("(&(cn=$regex)(objectClass=posixGroup)(!(|(objectClass=gosaMailAccount)(objectClass=gosaApplicationGroup)$sfilter)))", array("cn", "gidNumber", "description"));
      $error2= $ldap->error;
      while ($attrs= $ldap->fetch()){
        if (!isset($primaries[$attrs['gidNumber'][0]])){
          $this->grouplist[$attrs['gidNumber'][0]]= $attrs;
        }
      }
    }

    /*
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */
    $base2 = preg_replace("/ou=people,/i","",$base);

    $res3 =  get_list2($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
                              FALSE, $base2, array("ou", "description"), TRUE);

    $this->departments= array();
    $tmp = array();
    foreach ($res3 as $value){
      $tmp[strtolower($value['dn']).$value['dn']]=$value;
    }
    ksort($tmp);
    foreach($tmp as $value){
      if($value["description"][0]!=".."){
        $this->departments[$value['dn']]=convert_department_dn2($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]=$value["description"][0];
      }
    }

    /* Generate grouplist */
    if ($filter != ""){
      $filter= "(&(cn=$regex)(objectClass=posixGroup)(|$filter))";
      if ($groupfilter['guser'] != ""){
        $filter= "(&(|(memberUID=" . $groupfilter['guser'] . ")(cn=" . $groupfilter['guser'] . "))$filter)";
      }
    }
    if ($groupfilter['subsearch'] == "checked"){
      $res= get_list($this->ui->subtreeACL, "$filter", FALSE, $base, array("cn", "description", "gidNumber"), TRUE);
    } else {
     $base= get_groups_ou().$base;
      $res= get_list($this->ui->subtreeACL, "$filter", FALSE, $base, array("cn", "description", "gidNumber"), TRUE);
    }
    if (preg_match("/size limit/i", $error) || preg_match("/size limit/i", $error2)){
      $_SESSION['limit_exceeded']= TRUE;
    }

    foreach ($res as $value){
      $this->grouplist[$value['gidNumber'][0]]= $value;
    }



      $tmp=array();
    foreach($this->grouplist as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->grouplist=array();
    foreach($tmp as $val){
      $this->grouplist[]=$val;
    }
    reset ($this->grouplist);
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
    if (isset($this->grouptab->dn)){
      del_lock ($this->grouptab->dn);
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
