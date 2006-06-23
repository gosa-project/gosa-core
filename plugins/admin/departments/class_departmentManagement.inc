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
require "tabs_department.inc";

class departmentManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Departments";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Handling of LDAP subtrees";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* Headpage attributes */
  var $last_dep_sorting= "invalid";
  var $departments= array();
  var $deptabs= NULL;

  /* attribute list for save action */
  var $attributes= array();
  var $objectclasses= array();

  /* Vars to handle operations after saving the department 
     Recursive move && tagging   */
  var $ObjectInSaveMode         = false;    // Is true, if current object wasn't saved right now
  var $ObjectTaggingRequested   = false;    // Object must be tagged, an iframe will be shown. 
  var $RecursiveRemoveRequested = false;    // Is true, if this object must be moved, an iframe will be displayed in this case

  function departmentManagement ($config, $ui)
  {
    $this->ui= $ui;
    $this->dn= "";
    $this->config= $config;
    $this->DivListDepartment = new divListDepartment($this->config,$this);
  }

  function execute()
  {
    global $config;
  
    /* Call parent execute */
    plugin::execute();

    /***************
      Var init 
     ***************/

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^dep_edit_.*/","/^dep_del_.*/");

    /* Reload departments */
    $smarty						= get_smarty();
    $display 					= "";
    $s_action 					= "";  // Will contain an action, like del or edit
    $s_entry  					= "";  // The entry name for edit delete -...


    /***************
      Check posts  
     ***************/

    // Check Post action
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/dep_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/dep_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
        $s_entry  = base64_decode($s_entry);
        // Post for edit
      }elseif(preg_match("/dep_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/dep_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
        $s_entry  = base64_decode($s_entry);
        // Post for new
      }elseif(preg_match("/dep_new.*/",$key)){
        $s_action="new";
      }

    }


    /***************
      Create a new department
     ***************/

    /* New Entry if Posted action (s_action) == new
     */
    if ($s_action=="new"){
      $this->dn= "new";
      $this->deptabs= new deptabs($this->config,
          $this->config->data['TABS']['DEPTABS'], $this->dn);
    }


    /***************
      Edit entry
     ***************/

    /* Edit Entry if Posted action (s_action) == edit 
     * The entry which will be edited is defined in $s_entry
     */
    if (( $s_action=="edit") && (!isset($this->deptabs->config))){
      $this->dn= $this->config->departments[trim($s_entry)];

      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Lock the current entry, so everyone will get the  above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Set up the users ACL's for this 'dn' */
      $this->acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Register deptabs to trigger edit dialog */
      $this->deptabs= new deptabs($this->config,$this->config->data['TABS']['DEPTABS'], $this->dn);
      $this->deptabs->set_acl($this->acl);
      $_SESSION['objectinfo']= $this->dn;
    }


    /***************
      Delete entry 
     ***************/

    /* Delete Entry if Posted action (s_action) == del 
     * The entry which will be deleted is defined in $s_entry
     */
    if ($s_action =="del"){
      $this->dn= $this->config->departments[trim($s_entry)];

      /* Check locking */
      if (($user= get_lock($this->dn)) != ""){
        $_SESSION['dn']= $this->dn;
        return(gen_locked_message($user, $this->dn));
      } else {
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), @LDAP::fix($this->dn)));
        $display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
        return ($display);
      }
    }


    /***************
      Delete department confirmed  
     ***************/

    /* If department deletion is accepted ...
     * Finally delete department 
     */
    if (isset($_POST['delete_department_confirm'])){
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "department", $this->dn);
      if (chkacl($acl, "delete") == ""){
        $this->remove_from_parent();
        gosa_log ("Department object'".$this->dn."' has been removed");
      } else {
        print_red (_("You have no permission to remove this department."));
      }
    }


    /***************
      Edit department finished 
     ***************/

    if ((isset($_POST['edit_finish'])) && (isset($this->deptabs->config))){

      /* Check tabs, will feed message array */
      $message= $this->deptabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        $this->deptabs->save(true);
        $this->deptabs->by_object['department']->save();     

        /* This object must be tagged, so set ObjectTaggingRequested to true */
        if($this->deptabs->by_object['department']->must_be_tagged()){
          $this->ObjectTaggingRequested   = true; 
        }		

        /* Get recursive move is required, set RecursiveRemoveRequested to true */
        if($this->deptabs->by_object['department']->am_i_moved()){	
          $this->RecursiveRemoveRequested = true;
        }		

        /* This var indicated that there is an object which isn't saved right now. */
        $this->ObjectInSaveMode = true;

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /***************
      Handle Tagging (Return output for an iframe)
     ***************/

    /* This department must be tagged (Is called from iframe, generates output)*/
    if(isset($_GET['TagDepartment'])){
      $this->deptabs->by_object['department']->tag_objects();
      exit();  
    }


    /***************
      Handle recursive move (Return output for an iframe)
     ***************/

    /* initiate recursive remove  (Is called from iframe, generates output)*/
    if(isset($_GET['PerformRecMove'])){
      $this->deptabs->by_object['department']->recursive_move("","",true);
      exit();
    }


    /***************
      Return iframes, which call tagging / recusrsive move 
     ***************/

    /* While one of these vars below isset, we must return an iframe, 
       to perform requested operation */
    if($this->ObjectTaggingRequested){
      $this->ObjectTaggingRequested = false;
      return($this->deptabs->by_object['department']->ShowTagFrame());
    }
    if($this->RecursiveRemoveRequested){
      $this->RecursiveRemoveRequested = false;
      return($this->deptabs->by_object['department']->ShowMoveFrame());
    }


    /***************
      In case of tagging/moving the object wasn't deleted, do it know
     ***************/

    /* If there is an unsaved object and all operations are done
       remove locks & save object tab & unset current object */
    if($this->ObjectInSaveMode && (!$this->RecursiveRemoveRequested) && (!$this->ObjectTaggingRequested)){
      $this->deptabs->save();
      $this->ObjectInSaveMode = false;
      if ($this->dn != "new"){
        del_lock ($this->dn);
      }
      gosa_log ("Department object'".$this->dn."' has been saved");
      unset ($this->deptabs);
      $this->deptabs= NULL;
      unset ($_SESSION['objectinfo']);
    }


    /***************
      Dialog canceled  
     ***************/

    /* User canceled edit oder delete
     * Cancel dialog 
     */
    if (isset($_POST['edit_cancel']) || isset($_POST['delete_cancel']) || isset($_POST['delete_department_confirm'])){
      del_lock ($this->dn);
      unset($this->depdabs);
      $this->deptabs= NULL;
      unset ($_SESSION['objectinfo']);
    }

    /* Headpage or normal plugin screen? */
    if ($this->deptabs != NULL){

      /* Show main page (tabs) */
      $display= $this->deptabs->execute();
      if (!$this->deptabs->by_object[$this->deptabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }else{
      /* Display dialog with group list */
      $this->DivListDepartment->parent = $this;
      $this->DivListDepartment->execute();
      $this->reload();
      $this->DivListDepartment->DepartmentsAdded = true;
      $this->DivListDepartment->setEntries($this->departments);
      return($this->DivListDepartment->Draw());
    }
  }


  function reload()
  {
    /* Vairaible init */
    $base         = $this->DivListDepartment->selectedBase;
    $base_back    = preg_replace("/^[^,]+,/","",$base);
    $Regex        = $this->DivListDepartment->Regex;
  
    // Create Array to Test if we have a valid back button
    $tmp = $_SESSION['config']->idepartments;

    // In case of a valid back button create entry
    if(isset($tmp[$base_back])){
      $tmp2    ['dn']          = convert_department_dn($base_back);

      // If empty always go to top
      if(empty($tmp2['dn'])){
        $tmp2['dn']="/";
      }
      $tmp2    ['description'][0] = _("..");
      $result[$tmp[$base_back]]=$tmp2;
    }

    if($this->DivListDepartment->SubSearch){
      $res= get_list("(&(|(ou=$Regex)(description=$Regex))(objectClass=gosaDepartment))",
          $this->ui->subtreeACL, $base, array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);
    }else{
      $res= get_list("(&(|(ou=$Regex)(description=$Regex))(objectClass=gosaDepartment))",
          $this->ui->subtreeACL, $base, array("ou", "description"), GL_SIZELIMIT );
    }

    $this->departments= array();
    foreach ($res as $key => $value){

      /* Don't display base as entry on subsearch */
      if(($value['dn'] == $base) && ($this->DivListDepartment->SubSearch)){
        continue;
      }

      $cdn= convert_department_dn($value['dn']);

      /* Append to dep list */
      if(isset($value["description"][0])){
        $this->departments[$cdn]= get_sub_department($cdn)." - [".$value["description"][0]."]";
      }else{
        $this->departments[$cdn]= get_sub_department($cdn);//$value["description"][0];
      }
    }
    natcasesort ($this->departments);
    reset ($this->departments);
  }

  function remove_from_parent()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->recursive_remove();

    /* Optionally execute a command after we're done */
    $this->postremove();

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }

  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
  }

  function save_object()
  {
    /* reload department */
    $this->config->get_departments();
    $config = $this->config;

    $this->config->make_idepartments();
    $this->DivListDepartment->save_object();
  }
  
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
