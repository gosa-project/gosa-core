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
require "tabs_macros.inc";

class gofonMacro extends plugin
{
  /* Definitions */
  var $plHeadline		= "Phone macros";
  var $plDescription	= "This does something";

  /* Dialog attributes */
  var $macrotabs		= NULL;
  var $macros		= array();
  var $ui				= NULL;
  var $acl				= "";

  function gofonMacro($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("macrofilter")){
      $base= get_base_from_people($ui->dn);
      $macrofilter= array("regex" => "*");
      register_global("macrofilter", $macrofilter);
    }

    if(!isset($_SESSION['CurrentMainBase'])){
      $_SESSION['CurrentMainBase'] = $base= get_base_from_people($ui->dn);
    }

    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goFonMacro", $ui->dn);
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Save data */
    $macrofilter= get_global("macrofilter");
    foreach( array("regex") as $type){
      if (isset($_POST[$type])){
        $macrofilter[$type]= $_POST[$type];
      }
    }
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $macrofilter['regex']= $s;
    }

    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton

    /* Start for New List Managment */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
            $s_action="open";
            $s_entry = base64_decode($_GET['dep_id']);
             $_SESSION['CurrentMainBase'] = $this->config->departments[trim($s_entry)];

    }

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/user_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/user_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/user_chgpw.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_chgpw_/i","",$key);
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      }
    }
  if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Department changed? */
    if(isset($_POST['CurrentMainBase']) && $_POST['CurrentMainBase']){
      $_SESSION['CurrentMainBase']= $_POST['CurrentMainBase'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $_SESSION['CurrentMainBase'] = preg_replace("/^[^,]+,/","",$this->ui->dn);
      $_SESSION['CurrentMainBase'] = preg_replace("/^[^,]+,/","", $_SESSION['CurrentMainBase']);
    }

    if($s_action=="root"){
      $_SESSION['CurrentMainBase'] = $this->config->current['BASE'] ;
    }


    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","", $_SESSION['CurrentMainBase']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $_SESSION['CurrentMainBase'] = $this->config->departments[trim($base_back)];
      }else{
        $_SESSION['CurrentMainBase'] = $this->config->departments["/"];
      }
    }

    register_global("macrofilter", $macrofilter);
    $this->reload();
    
    $smarty= get_smarty();

    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* New macro? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn);
    }

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->macrotabs->dn);
      unset ($this->macrotabs);
      $this->macrotabs= NULL;
      unset ($_SESSION['objectinfo']);
    }

    /* Finish mac edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->macrotabs->config))){

      /* Check tabs, will feed message array */
      $this->macrotabs->last= $this->macrotabs->current;
      $this->macrotabs->save_object();
      $message= $this->macrotabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->macrotabs->save();
        gosa_log ("Macro object'".$this->dn."' has been saved");

        /* macro has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new mac at
           this point. */
        $this->reload ();
        unset ($this->macrotabs);
        $this->macrotabs= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }

    /* User wants to edit data? */
    if (($s_action=="edit") && (!isset($this->macrotabs->config))){

      /* Get 'dn' from posted 'macrolists', must be unique */
      $this->dn= $this->macros[$s_entry]['dn']; 

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the
         above dialog */
      add_lock ($this->dn, $this->ui->dn);
      /* Register macrotabs to trigger edit dialog */
      $this->macrotabs= new macrotabs($this->config,
      $this->config->data['TABS']['MACROTABS'], $this->dn);
      $_SESSION['objectinfo']= $this->dn;
    }

    /* Remove user was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $this->dn= $this->macros[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "goFonMacro", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the macro '%s'."), $this->dn));
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this macro!"));
      }
    }

    /* Confirmation for deletion has been passed. Group should be deleted. */
    if (isset($_POST['delete_macro_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){
      
        /* Delete request is permitted, perform LDAP action */
        $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn);
        $this->macrotabs->set_acl(array($this->acl));
      
        $this->macrotabs->delete ();
        gosa_log ("macro object'".$this->dn."' has been removed");
        unset ($this->macrotabs);
        $this->macrotabs= NULL;

        /* Group list has changed, reload it. */
        $this->reload ();
      } else {
        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this macro!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick group deletion.");
      }
      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /* Delete macro canceled? */
    if (isset($_POST['delete_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }

    /* Show tab dialog if object is present */
    if (($this->macrotabs) && (isset($this->macrotabs->config))){
      $display= $this->macrotabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->macrotabs->by_object[$this->macrotabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Finish")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }




    /* Prepare departments */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
            if ($_SESSION['CurrentMainBase'] == $key){
                    $options.= "<option selected='selected' value='$key'>$value</option>";
            } else {
                    $options.= "<option value='$key'>$value</option>";
            }
    }


   /* NEW LIST MANAGMENT */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
            " <input class='center' type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root").        "'>&nbsp;".
            " <input class='center' type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
            " <input class='center' type='image' align='middle' src='images/list_home.png' title='"._("Go to users department")."' alt='"._("Home")."'                     name='dep_home'>&nbsp;".
            " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
            " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
            " <input class='center' type='image' align='middle' src='images/list_new_macro.png' title='"._("Create new phone macro")."' alt='"._("New")."' name='user_new'>&nbsp;".
            " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
            _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
            " <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
            "</div>";


    /* Options */
    $action= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit user")."'>";
    $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete user")."'>";

    $divlist = new divlist("macrotabs");
    $divlist->SetHeader(array(
                              array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
                              array("string" => _("Username")." / "._("Department"), "attach" => "style=''"),
                              array("string" => _("Visible"), "attach" => "style='width:50px;'"),
                              array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));


    $divlist->SetSummary(_("This table displays all phone macros, in the selected tree."));
    $divlist->SetEntriesPerPage(0);

    // Defining Links
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

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
      $field3 = array("string" => "&nbsp;", "attach" => "style='width:50px;'");
      $field4 = array("string" => "&nbsp;", "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }

    // Pictures for Extensions
    $empty    = "<img class='center' src='images/nothing.png' style='width:16px;height:16px;' alt=''>";

    // User and Template  Images
    $macroimg   = "<img class='center' src='images/list_macro.png' alt='"._("Macro")."' title='%s'>";
    $visible    = "<img class='center' src='images/true.png' alt='"._("yes")."' title='"._("visible")."'>";
    $invisible  = "<img class='center' src='images/false.png' alt='"._("no")."'title='"._("invisible")."'>";

    // Link for edit
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";
  
    // Test Every Entry and generate divlist Array
    foreach($this->macros as $key => $val){
      // Generate Array to Add
      $display= $val["displayName"][0];
    
      
      if(isset($val['goFonMacroVisible']['0'])&&($val['goFonMacroVisible']['0'] == "1")){
        $pic1 = $visible;
      }else{
        $pic1 = $invisible;
      }

      $field1 = array("string" => sprintf($macroimg,$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".$val['dn']."'");
      $field3 = array("string" => $pic1, "attach" => "style='width:50px;'");
      $field4 = array("string" => preg_replace("/%KEY%/", "$key", $action), "attach" => "style='width:60px;border-right:0px;text-align:right;'");

      $divlist->AddEntry(array($field1,$field2,$field3,$field4));
    }

    /* Show main page */
    $smarty->assign("macroshead", $listhead);
    $smarty->assign("macros", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    foreach( array("regex") as $type){
      $smarty->assign("$type", $macrofilter[$type]);
    }

    $smarty->assign("CurrentMainBase" , $_SESSION['CurrentMainBase']);

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function reload()
  {
    /* Get config */
    $macrofilter= get_global('macrofilter');

    /* Set base for all searches */
    $base= $_SESSION['CurrentMainBase'];

    /* Regex filter? */
    if ($macrofilter['regex'] != ""){
      $regex= $macrofilter['regex'];
    } else {
      $regex= "*";
    }

    /* Generate macro list */
    $res= get_list($this->ui->subtreeACL, "(&(cn=$regex)(objectClass=goFonMacro))", FALSE, "ou=macros,ou=asterisk,ou=configs,ou=systems,".$base, array("*"), TRUE);
    $this->macros= $res;

    /* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */
    $peopleOU = get_people_ou();

    if(empty($peopleOU)){
      $base2 = $base;
    }else{
      $base2 = preg_replace("/".$peopleOU."/i","",$base);
    }

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
    foreach($this->macros as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->macros=array();
    foreach($tmp as $val){
      $this->macros[]=$val;
    }

    reset ($this->macros);
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
    if (isset($this->macrotabs->dn)){
      del_lock ($this->macrotabs->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
