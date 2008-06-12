<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class departmentManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Departments";
  var $plDescription= "Manage Departments";

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
  var $dns = array();

  var $acl_module = array("department");

  function departmentManagement (&$config, &$ui)
  {
    $this->ui= &$ui;
    $this->dn= "";
    $this->config= &$config;
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

    session::set('LOCK_VARS_TO_USE',array("/^act$/","/^id$/","/^dep_edit_.*/","/^dep_del_.*/","/^item_selected/","/^remove_multiple_departments/","/^menu_action/"));

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
      }elseif(preg_match("/^remove_multiple_departments/",$key)){
        $s_action="del_multiple";
      }
    }

    /* Create options */
    if(isset($_POST['menu_action']) && preg_match("/^dep_new_/",$_POST['menu_action'])){
      $s_action = "new";
      $s_entry  = preg_replace("/^dep_new_([a-z]*)/","\\1",$_POST['menu_action']);
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }


    /***************
      Create a new department
     ***************/

    /* New Entry if Posted action (s_action) == new
     */
    if ($s_action=="new"){
      $this->dn= "new";

      $objects['c'] ['ACL'] = "country";
      $objects['c'] ['TAB'] = "COUNTRY_TABS";
      $objects['ou']['ACL'] = "department";
      $objects['ou']['TAB'] = "DEPTABS";
      $objects['l'] ['ACL'] = "locatity";
      $objects['l'] ['TAB'] = "LOCALITY_TABS";
      $objects['o'] ['ACL'] = "organization";
      $objects['o'] ['TAB'] = "ORGANIZATION_TABS";
    
      if(isset($objects[$s_entry])){
        $tab = $objects[$s_entry]['TAB'];
        $acl = $objects[$s_entry]['ACL'];
        $this->deptabs= new deptabs($this->config,$this->config->data['TABS'][$tab], $this->dn,$acl);
      }else{
        trigger_error("Invalid / Not implemented countainer type.");
      }
      $this->deptabs->set_acl_base($this->DivListDepartment->selectedBase);
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

      /* Register deptabs to trigger edit dialog */
      $this->deptabs= new deptabs($this->config,$this->config->data['TABS']['DEPTABS'], $this->dn,"department");
      $this->deptabs->set_acl_base($this->dn);

      session::set('objectinfo',$this->dn);
    }


    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();


      if(count($ids)){
        $this->dns = array();
        foreach($ids as $id){
          $id = base64_decode($id);
          $this->dns[$id] = $dn = $this->config->departments[$id];
        }

        /* Check locks */
        if ($user= get_multiple_locks($this->dns)){
          return(gen_locked_message($user,$this->dns));
        }

        $dns_names = array();
        foreach($this->dns as $dn){
          $dns_names[] = @LDAP::fix($dn);
        }
        add_lock ($this->dns, $this->ui->dn);

        /* Lock the current entry, so nobody will edit it during deletion */
        $smarty->assign("info", msgPool::deleteInfo($dns_names));
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }
   

    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_department_confirm'])){

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){
        $acl = $this->ui->get_permissions($dn,"department/department");
        if (preg_match('/d/', $acl)){

          /* Delete request is permitted, perform LDAP action */
          $this->deptabs= new deptabs($this->config,$this->config->data['TABS']['DEPTABS'], $dn,"department");
          $this->deptabs->set_acl_base();
          $this->deptabs->delete ();
          $this->deptabs = NULL;
        } else {
          msg_dialog::display(_("Permission error"), msgPool::permDelete(), WARNING_DIALOG);
        }
      }

      /* Remove lock file after successfull deletion */
      $this->remove_lock();
      $this->dns = array();
    }

 
    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
    if(isset($_POST['delete_multiple_department_cancel'])){

      /* Remove lock file after successfull deletion */
      $this->remove_lock();
      $this->dns = array();
    }


    /***************
      Delete entry 
     ***************/

    /* Delete Entry if Posted action (s_action) == del 
     * The entry which will be deleted is defined in $s_entry
     */
    if ($s_action =="del"){
      $this->dn= $this->config->departments[trim($s_entry)];

      /* check acls */
      $acl = $this->ui->get_permissions($this->dn,"department/department");
      if(preg_match("/d/",$acl)){

        /* Check locking */
        if (($user= get_lock($this->dn)) != ""){
          session::set('dn',$this->dn);
          return(gen_locked_message($user, $this->dn));
        } else {
          add_lock ($this->dn, $this->ui->dn);
          $smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), @LDAP::fix($this->dn)));
          $smarty->assign("multiple", false);
          $display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
          return ($display);
        }
      }else{
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), WARNING_DIALOG);
      }
    }


    /***************
      Delete department confirmed  
     ***************/

    /* If department deletion is accepted ...
     * Finally delete department 
     */
    if (isset($_POST['delete_department_confirm'])){

      /* check acls */
      $acl = $this->ui->get_permissions($this->dn,"department/department");
      if(preg_match("/d/",$acl)){
        $this->remove_from_parent();
      } else {
        msg_dialog::display(_("Permission error"), msgPool::permDelete(), WARNING_DIALOG);
      }
    }


    /***************
      Handle tagging/recursive move (Return output for an iframe)
     ***************/

    /* initiate recursive remove  (Is called from iframe, generates output)*/
    if(isset($_GET['PerformRecMove'])){
      $this->deptabs->move_me();
      $this->DivListDepartment->selectedBase = $this->deptabs->by_object['department']->dn;  
      exit();
    }

    /* This department must be tagged (Is called from iframe, generates output)*/
    if(isset($_GET['TagDepartment'])){
      $this->deptabs->by_object['department']->tag_objects();
      exit();  
    }


    /***************
      Edit department finished 
     ***************/

    if (is_object($this->deptabs) &&          // Ensure we have a valid deptab here 
        (isset($_POST['edit_finish']) ||      // If 'Save' button is pressed in the edit dialog.
         isset($_POST['dep_move_confirm']) || // The move(rename) confirmation was given
         $this->deptabs->move_done())){       // The move(rename) is done, we have to save the rest now.

      /* Check tabs, will feed message array.
         This call will also initiate a sav_object() call.
          So don't move it below the moved check !.
       */
      $message= $this->deptabs->check();

      /*************
        MOVED ? 
         Check if this department has to be moved 
       *************/  
      if(!isset($_POST['dep_move_confirm']) && $this->deptabs->am_i_moved()){
        return($smarty->fetch(get_template_path("dep_move_confirm.tpl",TRUE)));
      }elseif(isset($_POST['dep_move_confirm']) && $this->deptabs->am_i_moved()){
        $smarty = get_smarty();
        $smarty->assign("src","?plug=".$_GET['plug']."&amp;PerformRecMove&no_output_compression");
        $smarty->assign("message",_("As soon as the move operation has finished, you can scroll down to end of the page and press the 'Continue' button to continue with the department management dialog."));
        return($smarty->fetch(get_template_path("dep_iframe.tpl",TRUE)));
      }

      /* Save, or display error message? */
      if (count($message) == 0){
        global $config;

        $this->deptabs->save();
        $config->get_departments();
        $config->make_idepartments();
        $this->config = $config;

        /* This var indicated that there is an object which isn't saved right now. */
        $this->ObjectInSaveMode = true;

        /* This object must be tagged, so set ObjectTaggingRequested to true */
        if($this->deptabs->by_object[$this->deptabs->base_name]->must_be_tagged()){
          $smarty = get_smarty();
          $smarty->assign("src","?plug=".$_GET['plug']."&TagDepartment&no_output_compression");
          $smarty->assign("message",_("As soon as the tag operation has finished, you can scroll down to end of the page and press the 'Continue' button to continue with the department management dialog."));
          return($smarty->fetch(get_template_path("dep_iframe.tpl",TRUE)));
        }		

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        msg_dialog::displayChecks($message);
      }
    }


   /***************
     In case of tagging/moving the object wasn't deleted, do it know
    ***************/

   /* If there is an unsaved object and all operations are done
      remove locks & save object tab & unset current object */
   if($this->ObjectInSaveMode){
     $this->config->get_departments();
     $this->ObjectInSaveMode = false;
     if ($this->dn != "new"){
       $this->remove_lock();
     }
     unset ($this->deptabs);
     $this->deptabs= NULL;
     session::un_set('objectinfo');
   }


   /***************
      Dialog canceled  
     ***************/

    /* User canceled edit oder delete
     * Cancel dialog 
     */
    if (isset($_POST['edit_cancel']) || isset($_POST['delete_cancel']) || isset($_POST['delete_department_confirm'])){
      $this->remove_lock();
      $this->deptabs= NULL;
      session::un_set('objectinfo');
    }

    /* Headpage or normal plugin screen? */
    if ($this->deptabs !== NULL){

      /* Show main page (tabs) */
      $display= $this->deptabs->execute();
      if (!$this->deptabs->by_object[$this->deptabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=submit name=\"edit_finish\" value=\"".msgPool::okButton()."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=submit name=\"edit_cancel\" value=\"".msgPool::cancelButton()."\">\n";
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
    $config = session::get('config');
    $tmp = $config->idepartments;

    // In case of a valid back button create entry
    if(isset($tmp[$base_back])){
      $tmp2    ['dn']          = convert_department_dn($base_back);

      // If empty always go to top
      if(empty($tmp2['dn'])){
        $tmp2['dn']="/";
      }
      $tmp2    ['description'][0] = ".. ".msgPool::backButton();
      $result[$tmp[$base_back]]=$tmp2;
    }

    

    if($this->DivListDepartment->SubSearch){
      $res= get_list("(&(|(ou=$Regex)(description=$Regex))(objectClass=gosaDepartment))",
          "department", $base, array("ou", "description"), GL_SIZELIMIT | GL_SUBSEARCH);
    }else{
      $res= get_list("(&(|(ou=$Regex)(description=$Regex))(objectClass=gosaDepartment))",
          "department", $base, array("ou", "description"), GL_SIZELIMIT );
    }

    
    /**/
    $non_ou = get_list("(&(|(c=$Regex)(description=$Regex))(objectClass=country))",
          "department", $base, array("c", "description"), GL_SIZELIMIT );
    $non_ou = array_merge($non_ou,get_list("(&(|(l=$Regex)(description=$Regex))(objectClass=locality))",
          "department", $base, array("l", "description"), GL_SIZELIMIT ));
    $non_ou = array_merge($non_ou,get_list("(&(|(o=$Regex)(description=$Regex))(objectClass=organization))",
          "department", $base, array("o", "description"), GL_SIZELIMIT ));
  
    $this->departments= array();
    foreach($non_ou as $obj){
      $this->departments[] = $obj['dn'];
    }


    /* Add current base to the list of available departments, but only if its naming attribute is 'ou' */
    if(preg_match("/^ou=/",$base)){
      $this->departments [ convert_department_dn($base) ] = ".";
    }

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
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[a-z0-9\\/\=]*$/i",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
    if(isset($this->dn) && !empty($this->dn) && $this->dn != "new"){
      del_lock($this->dn);
    }
    if(isset($this->dns) && is_array($this->dns) && count($this->dns)){
      del_lock($this->dns);
    }
  }

  function save_object()
  {
    /* reload department */
    $this->config->get_departments();

    $this->config->make_idepartments();
    $this->DivListDepartment->config= $this->config;
    $this->DivListDepartment->save_object();
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
