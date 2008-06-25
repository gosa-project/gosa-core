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
        // Post for edit
      }elseif(preg_match("/dep_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/dep_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
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

      $types = $this->get_support_departments();
      $type = "";
      foreach($types as $key => $data){
        if($data['ATTR'] == $s_entry){
          $type = $key;
          break;
        }
      }

      if(isset($types[$type])){
        $tab = $types[$type]['TAB'];
        $acl = $types[$type]['ACL'];
        $this->deptabs= new deptabs($this->config,$this->config->data['TABS'][$tab], $this->dn,$acl);
        $this->deptabs->set_acl_base($this->DivListDepartment->selectedBase);
      }else{
        trigger_error("Invalid / Not implemented countainer type.");
      }
    }


    /***************
      Edit entry
     ***************/

    /* Edit Entry if Posted action (s_action) == edit 
     * The entry which will be edited is defined in $s_entry
     */
    if (( $s_action=="edit") && (!isset($this->deptabs->config))){

      $types = $this->get_support_departments();

      $this->dn= $this->departments[trim($s_entry)]['dn'];
      $entry   = $this->departments[trim($s_entry)];

      /* Detect department type 
       */ 
      $data = array();
      foreach($types as $oc => $type){
        if(in_array($oc,$entry['objectClass'])){
          $data = $type;
          break;
        }
      }

      /* Check if the department type was editable 
       */
      if(!count($data)){
        trigger_error("Unknown department type skipped '".$this->dn."'.");
      }elseif(empty($data['TAB'])){
        // Do nothing, this object is currently not editable
      }else{

        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so everyone will get the  above dialog */
        add_lock ($this->dn, $this->ui->dn);

        /* Register deptabs to trigger edit dialog */
        $this->deptabs= new deptabs($this->config,$this->config->data['TABS'][$data['TAB']], $this->dn,"department");
        $this->deptabs->set_acl_base($this->dn);

        session::set('objectinfo',$this->dn);
      }
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
      $base_name = $this->deptabs->base_name;
      $this->deptabs->by_object[$base_name]->tag_objects();
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

    $types = $this->get_support_departments();

    /* Create search filter for each department type
     */
    $res = array();
    if($this->DivListDepartment->SubSearch){
      foreach($types as $oc => $data){
        $oc_f = "(&(objectClass=".$data['OC'].")(objectClass=gosaDepartment))";
        $res= array_merge($res,get_list("(&(|(".$data['ATTR']."=$Regex)(description=$Regex))".$oc_f.")",
            "department", $base, array($data['ATTR'],"objectClass", "description"), GL_SIZELIMIT | GL_SUBSEARCH));
      }
    }else{
      foreach($types as $oc => $data){
        $oc_f = "(&(objectClass=".$data['OC'].")(objectClass=gosaDepartment))";
        $res= array_merge($res,get_list("(&(|(".$data['ATTR']."=$Regex)(description=$Regex))".$oc_f.")",
            "department", $base, array($data['ATTR'], "objectClass","description"), GL_SIZELIMIT ));
      }
    }

    /* Prepare list (sortable index) 
     */
    $deps = array();
    $this->departments = array();
    foreach($res as $attr){

      /* Don't display base as entry on subsearch */
      if(($attr['dn'] == $base) && ($this->DivListDepartment->SubSearch)){
        continue;
      }

      /* Detect type and create sortable index 
       */
      $found =FALSE;
      foreach($types as $oc => $data){
        if(in_array($data['OC'],$attr['objectClass']) && isset($attr[$data['ATTR']][0])){
          $name = $attr[$data['ATTR']][0]."-".$attr['dn'];
          $deps[$name] = $attr;
          $found =TRUE;
          break;
        }
      }
    }
    uksort($deps, 'strnatcasecmp');

    /* In case of a valid back button create entry 
     */
    if(isset($tmp[$base])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->cat($base );
      $attr = $ldap->fetch();
      foreach($types as $oc => $type){
        if(in_array($oc,$attr['objectClass'])){
          $attr[$type['ATTR']][0] = ".";
        }
      }
      if(isset($attr['description'])){
         unset($attr['description']);
      }
      $deps= array_merge(array($attr),$deps);
    }

    $this->departments = array_values($deps);
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


  /*! \brief  Returns information about all container types that GOsa con handle.
    @return Array   Informations about departments supported by GOsa.
   */
  public static function get_support_departments()
  {
    /* Domain component 
     */
    $types = array();
    $types['dcObject']['ACL']     = "dcObject";
    $types['dcObject']['CLASS']   = "dcObject";
    $types['dcObject']['ATTR']    = "dc";
    $types['dcObject']['TAB']     = "DCOBJECT_TABS";
    $types['dcObject']['OC']      = "dcObject";
    $types['dcObject']['IMG']     = "plugins/departments/images/dc.png";
    $types['dcObject']['IMG_FULL']= "plugins/departments/images/dc.png";
    $types['dcObject']['TITLE']   = _("Domain Component");
    $types['dcObject']['TPL']     = "dcObject.tpl";

    /* Country object
     */
    $types['country']['ACL']     = "country";
    $types['country']['CLASS']   = "country";
    $types['country']['TAB']     = "COUNTRY_TABS";
    $types['country']['ATTR']    = "c";
    $types['country']['OC']      = "country";
    $types['country']['IMG']     = "plugins/departments/images/country.png";
    $types['country']['IMG_FULL']= "plugins/departments/images/country.png";
    $types['country']['TITLE']   = _("Country");
    $types['country']['TPL']     = "country.tpl";

    /* Locality object
     */
    $types['locality']['ACL']     = "locality";
    $types['locality']['CLASS']   = "locality";
    $types['locality']['TAB']     = "LOCALITY_TABS";
    $types['locality']['ATTR']    = "l";
    $types['locality']['OC']      = "locality";
    $types['locality']['IMG']     = "plugins/departments/images/locality.png";
    $types['locality']['IMG_FULL']= "plugins/departments/images/locality.png";
    $types['locality']['TITLE']   = _("Locality");
    $types['locality']['TPL']     = "locality.tpl";

    /* Organization
     */
    $types['organization']['ACL']     = "organization";
    $types['organization']['CLASS']   = "organization";
    $types['organization']['TAB']     = "ORGANIZATION_TABS";
    $types['organization']['ATTR']    = "o";
    $types['organization']['OC']      = "organization";
    $types['organization']['IMG']     = "plugins/departments/images/organization.png";
    $types['organization']['IMG_FULL']= "plugins/departments/images/organization.png";
    $types['organization']['TITLE']   = _("Organization");
    $types['organization']['TPL']     = "organization.tpl";

    /* Organization
     */
    $types['organizationalUnit']['ACL']     = "department";
    $types['organizationalUnit']['CLASS']   = "department";
    $types['organizationalUnit']['TAB']     = "DEPTABS";
    $types['organizationalUnit']['ATTR']    = "ou";
    $types['organizationalUnit']['OC']      = "organizationalUnit";
    $types['organizationalUnit']['IMG']     = "images/lists/folder.png";//plugins/departments/images/department.png";
    $types['organizationalUnit']['IMG_FULL']= "images/lists/folder-full.png";//:wplugins/departments/images/department.png";
    $types['organizationalUnit']['TITLE']   = _("Department");
    $types['organizationalUnit']['TPL']     = "generic.tpl";


#  /* Alias
#   */
#  $types['alias']['ACL']     = "alias";
#  $types['alias']['CLASS']   = "alias";
#  $types['alias']['TAB']     = "ALIASTABS";
#  $types['alias']['ATTR']    = "???";
#  $types['alias']['OC']      = "???";
#  $types['alias']['IMG']     = "plugins/departments/images/department_alias.png";
#  $types['alias']['IMG_FULL']= "plugins/departments/images/department_alias.png";
#  $types['alias']['TITLE']   = _("Alias");
#  $types['alias']['TPL']     = "alias.tpl";
#
#  /* Referal
#   */
#  $types['referal']['ACL']     = "referal";
#  $types['referal']['CLASS']   = "referal";
#  $types['referal']['TAB']     = "REFERALTABS";
#  $types['referal']['ATTR']    = "???";
#  $types['referal']['OC']      = "???";
#  $types['referal']['IMG']     = "plugins/departments/images/department_referal.png";
#  $types['referal']['IMG_FULL']= "plugins/departments/images/department_referal.png";
#  $types['referal']['TITLE']   = _("Referal");
#  $types['referal']['TPL']     = "referal.tpl";

    return($types);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
