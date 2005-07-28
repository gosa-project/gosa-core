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

  function ogroupManagement ($config, $dn= NULL)
  {
    /* Include config object */
    $this->config= $config;
    $this->ui= get_userinfo();

    /* Fill translation array */
    $this->obtypes= array(	"posixAccount" => _("UNIX accounts"),
        "posixGroup" => _("Groups"),
        "gosaDepartment" => _("Departments"),
        "gosaApplication" => _("Applications"),
        "goServer" => _("Servers"),
        "gotoTerminal" => _("Thin Clients"),
        "gotoWorkstation" => _("Workstations"),
        "goFonHardware" => _("Phone"),
        "gotoPrinter" => _("Printer"));
    asort($this->obtypes);

    /* Get global filter config */
    if (!is_global("ogroupfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $ogroupfilter= array(	"usergroups" => "checked",
          "groupgroups" => "checked",
          "appgroups" => "checked",
          "depgroups" => "checked",
          "servergroups" => "checked",
          "wsgroups" => "checked",
          "prtgroups" => "checked",
          "tcgroups" => "checked",
          "fongroups" => "checked",
          "depselect" => $base,
          "regex" => "*");
      register_global("ogroupfilter", $ogroupfilter);
    }
  }

  function execute()
  {
    /* Prepare template */
    $smarty= get_smarty();

    $ogroupfilter= get_global("ogroupfilter");
    $s_action   = "";
    $s_entry    = "";

    foreach( array("depselect", "regex") as $type){
      if (isset($_POST[$type])){
        $ogroupfilter[$type]= $_POST[$type];
      }
    }
    if (isset($_POST['regex'])){
      foreach( array("usergroups", "groupgroups", "appgroups", "depgroups",
            "servergroups", "wsgroups", "prtgroups", "tcgroups", "fongroups") as $type){

        if (isset($_POST[$type])) {
          $ogroupfilter[$type]= "checked";
        } else {
          $ogroupfilter[$type]= "";
        }
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $ogroupfilter['regex']= $s;
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

    /* Department changed? */
    if(isset($_POST['depselect']) && $_POST['depselect']){
      $ogroupfilter['depselect']= $_POST['depselect'];
      $this->reload();
    }

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
      $s_action="open";
      $s_entry = base64_decode($_GET['dep_id']);
      $ogroupfilter['depselect']= "".$this->config->departments[trim($s_entry)];
      $this->reload();
    }

    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","",$ogroupfilter['depselect']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $ogroupfilter['depselect']= $this->config->departments[trim($base_back)];
      }else{
        $ogroupfilter['depselect']= $this->config->departments["/"];
      }
      register_global("ogroupfilter", $ogroupfilter);
      $this->reload();
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $ogroupfilter['depselect']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $ogroupfilter['depselect']=(preg_replace("/^[^,]+,/","",$ogroupfilter['depselect']));
      $this->reload();
    }

    /* root posted */ 
    if($s_action=="root"){
      $ogroupfilter['depselect']=($this->config->current['BASE']);
      $this->reload();
    }

    // Edit if
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    register_global("ogroupfilter", $ogroupfilter);

    /* Prepare formular */
    if (!isset($this->ogroup) &&
        !isset($_POST['new_group']) &&
        !isset($_POST['delete_group']) &&
        !isset($_POST['select_group'])){
      $this->reload();
    }

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
      $this->ogroup= new ogrouptabs($this->config,
          $this->config->data['TABS']['OGROUPTABS'], $this->dn);
      $this->ogroup->set_acl(array(':all'));
    }

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
        $smarty->assign("info", sprintf(_("You're about to delete the object group '%s'."), $this->dn));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this object group!"));
      }
    }

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

        /* Group list has changed, reload it. */
        $this->reload ();
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


    /* Delete group canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    if ($s_action=="edit"){

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

    /* Finish button has been pressed */
    if (isset($_POST['edit_finish'])){

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

        /* There's no page reload so we have to read new users at
           this point. */
        $this->reload ();
        unset ($this->ogroup);
        $this->ogroup= NULL;
        unset ($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel'])){
      del_lock ($this->ogroup->dn);
      unset ($this->ogroup);
      $this->ogroup= NULL;
      unset($_SESSION['objectinfo']);
    }

    /* Show dialog if object is present */
    if ($this->ogroup){
      $display= $this->ogroup->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->ogroup->by_object[$this->ogroup->current]->dialog){
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
            if ($ogroupfilter['depselect'] == $key){
                    $options.= "<option selected value='$key'>$value</option>";
            } else {
                    $options.= "<option value='$key'>$value</option>";
            }
    }

    // Managment
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
      " <input type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
      " <input type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."'
name='dep_root' alt='"._("Root")."'>&nbsp;".
      " <input type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."'
 alt='"._("Home")."' name='dep_home'>&nbsp;".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      " <input type='image' align='middle' src='images/list_new_ogroup.png' title='"._("Create new object group")."' alt='"._("new")."' name='group_new'>".
      " <img src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
      _("Current base")."&nbsp;<select name='depselect' onChange='mainform.submit()'>$options</select>".
      " <input type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
      "</div>";


    $actions = "<input type='image' src='images/edit.png' alt='"._("edit")."' name='group_edit_%KEY%' title='"._("Edit this entry")."'>";
    $actions.= "<input type='image' src='images/edittrash.png' alt='"._("delete")."' name='group_del_%KEY%' title='"._("Delete this entry")."'>";

    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    // image Buttons
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
    $userimg  = "<img src='images/select_groups.png' alt='User'    title='%s'>";

    // Extension images
    $mailimg  = "<img src='images/mailto.png'             alt='M'  title='"._("Mail")   ."'>";

    // Space
    $empty    = "<img src='images/empty.png' style='width:16px;height:16px;' alt=''>";

    // List Setup
    $divlist = new divlist();
    $divlist->SetHeader(array(
          array("string" => "&nbsp;","attach"=>"style='width:20px;'"),
          array("string" => _("Name of object groups")." / "._("Departments")),
          array("string" => _("Properties") ,"attach"=>"style='width:50px;'"),
          array("string" => _("Actions")    ,"attach"=>"style='width:50px;border:none;'")
          ));


    $divlist->SetSummary(_("This table displays all groups, in the selected tree."));
    $divlist->SetEntriesPerPage(20);

    foreach($this->departments as $key=> $val){

      $field1 = array("string" => "<img src='images/folder.png' alt='"._("Department")."'>");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val));
      $field3 = array("string" => "&nbsp;");
      $field4 = array("string" => "&nbsp;","attach"=>"style='text-align:right;border:none'");

      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }

    // Assigning ogroups 
    foreach($this->ogrouplist as $key => $val){

      if(isset($val['mail'])){
        $mail = $mailimg;
      }else{
        $mail = $empty;
      }

      $title = "title='gid : ".$key." - dn : ".$val['dn']."'";

      if(!isset($val['description'][0])){
        $desc = "";
      }else{
        $desc = " - [ ".$val['description'][0]." ]";
      }
      $field1 = array("string" => "<img src='images/list_ogroup.png' alt='"._("Object group")."'>");
      $field2 = array("string" => sprintf($editlink,$key,($val['cn']['0'].$desc)),"attach"=>$title);
      $field3 = array("string" => $this->convert_list($val)."&nbsp;".$mail);
      $field4 = array("string" => preg_replace("/%KEY%/", $key, $actions),"attach"=>"style='text-align:right;border:none'");

      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }






    /* Show main page */
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);

    $smarty->assign("groupshead", $listhead);
    $smarty->assign("groups", $divlist->DrawList());

    foreach( array("usergroups", "groupgroups", "appgroups", "depgroups",
          "servergroups", "wsgroups", "prtgroups", "tcgroups", "fongroups", "regex", "depselect") as $type){
      $smarty->assign("$type", $ogroupfilter[$type]);
    }

    $smarty->assign("hint", print_sizelimit_warning());
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    return ($smarty->fetch (get_template_path('headpage.tpl', TRUE)));
  }


  function convert_list($input)
  {
    $temp= "";


    $conv= array(   "U" => "select_user.png",
        "G" => "select_groups.png",
        "A" => "select_application.png",
        "D" => "select_department.png",
        "S" => "select_server.png",
        "F" => "select_phone.png",
        "W" => "select_workstation.png",
        "T" => "select_terminal.png",
        "P" => "select_printer.png");

    /* Assemble picture */
    $type= $input['gosaGroupObjects'][0];
    $type= preg_replace("/[^A-Z]/i","",$type);
    if (isset($type[0])){
      $p1['pic']= $conv[$type[0]];
      $p1['alt']= $type[0];
    } else {
      $p1['pic']= "empty.png";
      $p1['alt']= "";
    }
    if (isset($type[1])){
      $p2['pic']= $conv[$type[1]];
      $p2['alt']= $type[1];
    } else {
      $p2['pic']= "empty.png";
      $p2['alt']= "";
    }
    $temp = "<img src='images/".$p1['pic']."' alt='".$p1['alt']."'>";
    $temp.= "<img src='images/".$p2['pic']."' alt='".$p2['alt']."'>";
    return ($temp);
  }


  function reload()
  {
    /* Get config */
    $ogroupfilter= get_global('ogroupfilter');

    /* Set base for all searches */
    $base= $ogroupfilter['depselect'];

    /* Regex filter? */
    if ($ogroupfilter['regex'] != ""){
      $regex= $ogroupfilter['regex'];
    } else {
      $regex= "*";
    }

    /* User filter? */
    $this->oogrouplist= array();

    /* Generate oogrouplist filter */
    $filter= "";

    if ($ogroupfilter['usergroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*U*)";
    }
    if ($ogroupfilter['groupgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*G*)";
    }
    if ($ogroupfilter['appgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*A*)";
    }
    if ($ogroupfilter['depgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*D*)";
    }
    if ($ogroupfilter['servergroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*S*)";
    }
    if ($ogroupfilter['wsgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*W*)";
    }
    if ($ogroupfilter['prtgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*P*)";
    }
    if ($ogroupfilter['tcgroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*T*)";
    }
    if ($ogroupfilter['fongroups'] == "checked"){
      $filter.= "(gosaGroupObjects=*F*)";
    }

    if ($filter != ""){
      $filter= "(&(cn=$regex)(objectClass=gosaGroupOfNames)(|$filter))";
    } else {
      $filter= "(&(cn=$regex)(objectClass=gosaGroupOfNames))";
    }
    $res= get_list($this->ui->subtreeACL, "$filter", TRUE, $base, array("*"), TRUE);
    $this->ogrouplist= $res;
    ksort ($this->ogrouplist);
    reset ($this->ogrouplist);


    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */
    $base2 = preg_replace("/ou=people,/i","",$base);

    $res3 =  get_list2($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
                              TRUE, $base2, array("ou", "description"), TRUE);

    $this->departments= array();
    foreach ($res3 as $value){
      if($value["description"][0]!=".."){
        $this->departments[$value['dn']]= convert_department_dn2($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]=$value["description"][0];
      }
    }

    /* END NEW LIST MANAGMENT
     */




  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
