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

		/* Get global filter config */
		if (!is_global("depfilter")){
			$depfilter= array("regex" => "*");
			register_global("depfilter", $depfilter);
		}
		$base= get_base_from_people($ui->dn);
		if(!isset($_SESSION['CurrentMainBase'])){
			$_SESSION['CurrentMainBase'] = $this->config->current['BASE'];
		}
		$_SESSION['CurrentMainBase'] = $_SESSION['CurrentMainBase'];
	}

	function execute()
	{
		/* Call parent execute */
		plugin::execute();

		$_SESSION['LOCK_VARS_TO_USE'] = array("/^act$/","/^id$/","/^dep_edit_.*/","/^dep_del_.*/");

		/* Reload departments */
		$this->config->get_departments();
		$this->config->make_idepartments();
		$_SESSION['config']= $this->config;

		$smarty						= get_smarty();
		$display 					= "";
		$depfilter					= get_global("depfilter");
		$s_action 					= "";  // Will contain an action, like del or edit
		$s_entry  					= "";  // The entry name for edit delete -...

		if (isset($_POST['regex'])){
			$depfilter['regex']= $_POST['regex'];
		}

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
			}elseif(preg_match("/dep_back.*/i",$key)){
				$s_action="back";
			}elseif(preg_match("/dep_home.*/i",$key)){
				$s_action="home";
			}elseif(preg_match("/dep_root.*/i",$key)){
				$s_action="root";
			}

		}

		if(isset($_POST['CurrentMainBase']) && $_POST['CurrentMainBase']){
			$_SESSION['CurrentMainBase'] = $_POST['CurrentMainBase'];
		}

		if($s_action=="root"){
			$_SESSION['CurrentMainBase'] = ($this->config->current['BASE']);
		}

		if($s_action=="home"){
			$_SESSION['CurrentMainBase'] =(preg_replace("/^[^,]+,/","",$this->ui->dn));
			$_SESSION['CurrentMainBase'] =(preg_replace("/^[^,]+,/","",  $_SESSION['CurrentMainBase'] ));
		}

		if($s_action=="back"){
			$base_back 			= preg_replace("/^[^,]+,/","",  $_SESSION['CurrentMainBase']);
			$base_back          = convert_department_dn($base_back);

			if(isset($this->config->departments[trim($base_back)])){
				$_SESSION['CurrentMainBase'] = $this->config->departments[trim($base_back)];
			}else{
				$_SESSION['CurrentMainBase'] = $this->config->departments["/"];
			}
		}

		if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
			$s_action="open";
			$s_entry = base64_decode($_GET['dep_id']);
			$_SESSION['CurrentMainBase'] = $this->config->departments[trim($s_entry)];
		}

		if (isset($_GET['search'])){
			$s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
			if ($s == "**"){
				$s= "*";
			}
			$depfilter['regex']= $s;
		}
		register_global("depfilter", $depfilter);

		/* New Entry if Posted action (s_action) == new
		 */
		if ($s_action=="new"){
			$this->dn= "new";
			$this->deptabs= new deptabs($this->config,
					$this->config->data['TABS']['DEPTABS'], $this->dn);
		}

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
				$smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), LDAP::fix($this->dn)));
				$display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
				return ($display);
			}
		}

		/* If department deletion is accepted ...
		 * Finally delete department 
		 */
		if (isset($_POST['delete_department_confirm'])){
			$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
			$acl= get_module_permission($acl, "department", $this->dn);
			if (chkacl($acl, "delete") == ""){
				$this->remove_from_parent();
				gosa_log ("Department object'".$this->dn."' has been removed");
				$this->reload ();
			} else {
				print_red (_("You have no permission to remove this department."));
			}
		}

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

		/* This department must be tagged (Is called from iframe, generates output)*/
    if(isset($_GET['TagDepartment'])){
			$this->deptabs->by_object['department']->tag_objects();
      exit();  
		}

    /* initiate recursive remove  (Is called from iframe, generates output)*/
		if(isset($_GET['PerformRecMove'])){
			$this->deptabs->by_object['department']->recursive_move("","",true);
		  exit();
    }

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

		/* If there is an unsaved object and all operations are done
        remove locks & save object tab & unset current object */
		if($this->ObjectInSaveMode && (!$this->RecursiveRemoveRequested) && (!$this->ObjectTaggingRequested)){
      $this->deptabs->save();
      $this->ObjectInSaveMode = false;
      if ($this->dn != "new"){
        del_lock ($this->dn);
      }
  		gosa_log ("Department object'".$this->dn."' has been saved");
			$this->reload ();
			unset ($this->deptabs);
			$this->deptabs= NULL;
			unset ($_SESSION['objectinfo']);
		}

		/* User cancelt edit oder delete
		 * Cancel dialog 
		 */
		if (isset($_POST['edit_cancel']) || isset($_POST['delete_cancel']) ||
				isset($_POST['delete_department_confirm'])){

			del_lock ($this->dn);
			unset($this->depdabs);
			$this->deptabs= NULL;
			unset ($_SESSION['objectinfo']);
		}

		/* Headpage or normal plugin screen? */
		if ($this->deptabs == NULL){
			/* Check sorting variable */
			$this->reload();

			/* Check for exeeded sizelimit */
			if (($message= check_sizelimit()) != ""){
				return($message);
			}

			/* Prepare departments */
			$options= "";
			foreach ($this->config->idepartments as $key => $value){
				if ($_SESSION['CurrentMainBase'] == $key){
					$options.= "<option selected value='$key'>$value</option>";
				} else {
					$options.= "<option value='$key'>$value</option>";
				}
			}

			/* Generate list head */
			$listhead = "<div style='background:#F0F0F9;padding:5px;'>".
				" <input type='image' class='center' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
				" <input type='image' class='center' src='images/list_up.png' align='middle' title='"._("Go up one department")."' name='dep_back' alt='"._("Up")."'>&nbsp;".
				" <input type='image' class='center' src='images/list_home.png' align='middle' title='"._("Go to users home department")."' name='dep_home' alt='"._("Home")."'>&nbsp;".
				" <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
				" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
				" <input class='center' type='image' src='images/list_new_department.png' align='middle' alt='"._("Create new department")."' name='dep_new'>&nbsp;".
				" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
				_("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
				" <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
				"</div>";

			/* Show main page */
			$divlist = new divlist("departmenttabs");
			$divlist->SetSummary(_("This table displays all departments, in the selected tree."));
			$divlist->SetEntriesPerPage(0);
			$actions= "<input class='center' type='image' src='images/edit.png' 		 alt='"._("edit")."' 	name='dep_edit_%KEY%' title='"._("Edit this entry")."'>";
			$actions.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' 	name='dep_del_%KEY%' title='"._("Delete this entry")."'>";

			$linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

			$divlist->SetHeader(array(
						array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
						array("string"=>_("Department name"), "attach" => "style=''"),
						array("string" =>_("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'")));

			foreach($this->departments as $key => $val) {
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

				$title = $this->config->departments[$key];
				$field0 = array("string" => "<img src='images/".$non_empty."folder.png' title='"._("department")."' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
				$field1 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style='' title='".$title."'");
				$field2 = array("string" => preg_replace("/%KEY%/", base64_encode($key), $actions), "attach" => "style='width:60px;border-right:0px;text-align:right;'");

				$divlist->AddEntry( array($field0,$field1,$field2));

			}
			$smarty->assign("departments",$divlist->DrawList() );
			$smarty->assign("departmentshead",$listhead);
			$smarty->assign("search_image", get_template_path('images/search.png'));
			$smarty->assign("tree_image", get_template_path('images/tree.png'));
			$smarty->assign("infoimage", get_template_path('images/info.png'));
			$smarty->assign("launchimage", get_template_path('images/launch.png'));
			$smarty->assign("deplist", $this->config->idepartments);

			foreach( array("regex") as $type){
				$smarty->assign("$type", $depfilter[$type]);
			}

			$smarty->assign("CurrentMainBase", $_SESSION['CurrentMainBase']);

			/* Extend if we are not using javascript */
			$smarty->assign("apply", apply_filter());
			$smarty->assign("alphabet", generate_alphabet());
			$smarty->assign("hint", print_sizelimit_warning());

			$display= $smarty->fetch(get_template_path('headpage.tpl', TRUE));
			return($display);
		}

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
	}


	function reload()
	{
		/* Get config */
		$depfilter= get_global('depfilter');
		// Added for dirlist function...

		if(isset($_POST['deplist'])){
			$depfilter= get_global("depfilter");
			$_SESSION['CurrentMainBase']= $this->config->departments[trim($_POST['deplist'])];
			register_global("depfilter", $depfilter);
		}

		/* Set base for all searches */
		$base= $_SESSION['CurrentMainBase'];

		/* Regex filter? */
		if ($depfilter['regex'] != ""){
			$regex= $depfilter['regex'];
		} else {
			$regex= "*";
		}

		// Configure Back Address
		$base_back = preg_replace("/^[^,]+,/","",$base);

		// Create Array to Test if we have a valid back button
		$tmp = array_flip($_SESSION['config']->departments);

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

		$res= get_list("(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))",
				$this->ui->subtreeACL, $base, array("ou", "description"), GL_SIZELIMIT | GL_CONVERT);

		$this->departments= array();
		foreach ($res as $value){
			if(isset($value["description"][0])){
				$this->departments[$value['dn']]= get_sub_department($value['dn'])." - [".$value["description"][0]."]";
			}else{
				$this->departments[$value['dn']]= get_sub_department($value['dn']);//$value["description"][0];
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

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
