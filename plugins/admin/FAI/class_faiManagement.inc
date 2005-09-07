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
require "tabsPartition.inc";
require "tabsScript.inc";

class faiManagement extends plugin
{
	/* Definitions */
	var $plHeadline			= "FAI management";
	var $plDescription		= "Fully Automatic Installation - management";

	/* CLI vars */
	var $cli_summary		= "Handling of FAI entries";
	var $cli_description	= "This plugin represents a management tool\n
		which allows us to manage all needed attributes for fully automatic installations (FAI)";
	var $cli_parameters		= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

	/* Headpage attributes */
	var $departments= array();
	var $deptabs= NULL;

	/* attribute list for save action */
	var $attributes			= array(); 	// 	Attributes Managed by this plugin, none, 
	//	because this is only an overview over all objects already defined in FAI.
	var $objectclasses		= array();	// 	ObjectClasses which the attributes are related to
	var $dialog				= array();	// 	This object contains every dialog we have currently opened

	var $objects			= array();	// 	This array contains all available objects shown in divlist
	var $is_dialog			= false;

	/* construction/reconstruction 
	 * The Filter ($faifilter stored in $_SESSION['faifilter']) defines the last 
	 *  selected department and filter options
	 * If it is not defined already we create a new fresh faifilter.
	 */
	function faiManagement ($config, $ui)
	{
		$this->ui			= $ui;	

		/* Set current dn to "", */
		$this->dn			= "";

		/* Assign config */
		$this->config		= $config;

		/* Get global filter config */
		if (!is_global("faifilter")){
			$base	= get_base_from_people($ui->dn);
			$faifilter= array("base" => "ou=fai,ou=configs,ou=systems,".$base,
					"regex" => "*");
			$faifilter['ShowProfiles'] 	= true;
			$faifilter['ShowTemplates'] = true;
			$faifilter['ShowScripts'] 	= true;
			$faifilter['ShowHooks'] 	= true;
			$faifilter['ShowVariables']	= true;
			$faifilter['ShowPackages'] 	= true;
			$faifilter['ShowPartitions']=true;
			register_global("faifilter", $faifilter);
		}
	}

	function execute()
	{
		$display 	= "";
		$smarty 	= get_smarty();
		$s_action	= "";
		$s_entry 	= "";
		/* filter management 
		 * Filter will be changed if POST['regex'] or $_GET['search'] isset
		 * New Filter will be stored in session and is used to generate list contents
		 */	
		$faifilter = $_SESSION['faifilter'];

		/* Filter is posted by apply button 
		 * Check every single chkbox 
		 */
		if(isset($_POST['regex'])){
			$faifilter['regex'] = $_POST['regex']."*";
			foreach(array("ShowPartitions","ShowProfiles","ShowTemplates","ShowScripts","ShowHooks","ShowVariables","ShowPackages") as $attrs){
				if(isset($_POST[$attrs])){
					$faifilter[$attrs] = true;
				}else{
					$faifilter[$attrs] = false;
				}
			}
		}

		/* Check ImageButton posts
		 * Create new tab ich new_xx is posted
		 */
		foreach($_POST as $key => $val){
			if(preg_match("/create_partition/i",$key)){
				$s_action = "new_partition";
			}elseif(preg_match("/UseNewClass/",$key)){
				$s_action = "select_class_name_finished";
			}elseif(preg_match("/UseSelectedClass/",$key)){
				$s_action = "select_class_name_finished";
			}elseif(preg_match("/^entry_edit_.*/",$key)){
				$s_entry = preg_replace("/^entry_edit_/","",$key);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
				$s_action = "edit";
			}elseif(preg_match("/^entry_delete_.*/",$key)){
				$s_entry = preg_replace("/^entry_delete_/","",$key);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
				$s_action = "delete";
			}

		}

		/* Confirm dialog 
		 * Delte object 
         */
		if ($s_action=="delete"){

			/* Get 'dn' from posted termlinst */
			$this->dn= $this->objects[$s_entry]['dn'];

			/* Load permissions for selected 'dn' and check if
			   we're allowed to remove this 'dn' */
			$acl= 		get_permissions ($this->dn, $this->ui->subtreeACL);
			$this->acl= get_module_permission($acl, "FAIclass", $this->dn);
			if (chkacl($this->acl, "delete") == ""){

				/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
				if (($user= get_lock($this->dn)) != ""){
					return(gen_locked_message ($user, $this->dn));
				}

				/* Lock the current entry, so nobody will edit it during deletion */
				add_lock ($this->dn, $this->ui->dn);
				$smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), $this->dn));
				return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
			} else {

				/* Obviously the user isn't allowed to delete. Show message and
				   clean session. */
				print_red (_("You are not allowed to delete this component!"));
			}
		}

		/* Deltetion was confirmed, so delete this entry
		 */
		if (isset($_POST['delete_terminal_confirm'])){

			/* Some nice guy may send this as POST, so we've to check
			   for the permissions again. */
			if (chkacl($this->acl, "delete") == ""){

				/* Find out more about the object type */
				$ldap	= $this->config->get_ldap_link();
				$ldap->cat($this->dn);
				$attrs	= $ldap->fetch();

				$type	= $this->get_type($attrs);			

				$this->dialog= new $type[0]($this->config,	$this->config->data['TABS'][$type[2]], $this->dn);
				$this->dialog->set_acl(array($this->acl));

				$this->dialog->by_object[$type[1]]->remove_from_parent ();
				unset ($this->dialog);
				gosa_log ("System object'".$this->dn."' has been removed");
				$this->dialog= NULL;

				/* Terminal list has changed, reload it. */
				$this->reload ();
			} else {

				/* Normally this shouldn't be reached, send some extra
				   logs to notify the administrator */
				print_red (_("You are not allowed to delete this component!"));
				gosa_log ("Warning: '".$this->ui->uid."' tried to trick system ".
						"deletion.");
			}

			/* Remove lock file after successfull deletion */
			del_lock ($this->dn);
		}

		/* Edit Entry */
		if($s_action == "edit"){
			$entry  = $this->objects[$s_entry];

			$a_setup = ( $this->get_type($entry));

			$this->dn = $entry['dn'];
			$this->dialog= new $a_setup[0]($this->config,
					$this->config->data['TABS'][$a_setup[2]],$this->dn);
			$this->is_dialog = true;
		}


		/* Dialog handling */
		if($s_action == "new_partition"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIpartitionTable");
		}

		if($s_action == "select_class_name_finished"){
			$this->dialog->save_object();
			if(count($this->dialog->check())!=0){
				foreach($this->dialog->check() as $msg){
					print_red($msg);
				}		
			}else{
				$this->dn = "new" ;
				$name = $this->dialog->save();
				$this->dialog = new tabsPartition($this->config,
						$this->config->data['TABS']['FAIPARTITIONTABS'],$this->dn);
				$this->dialog->by_object['faiPartitionTable']->cn = $name;
				$this->is_dialog = true;
			}		
		}	


		/* Search is set */
		if(isset($_GET['search'])){
			$faifilter['regex']=$_GET['search']."*";
		}

		/* remove double asterisks and save new filter */
		$faifilter['regex'] = preg_replace("/\*\*/","*",$faifilter['regex']);
		$_SESSION['faifilter']= $faifilter;

		/* Edit dialog was canceled 
		 * Remove dialog an show management dialog
		 */
		if(isset($_POST['edit_cancel'])){
			unset($this->dialog);
			$this->dialog=NULL;
			$this->is_dialog = false;
		}


		/* This check if the given tab could be saved 
		 * If it was possible to save it, remove dialog object. 
		 * If it wasn't possible, show errors and keep dialog.
		 */
		if(isset($_POST['edit_finish'])){
			$this->dialog->save_object();
			if(count($this->dialog->check())!=0){
				foreach($this->dialog->check() as $msg){
					print_red($msg);
				}
			}else{
				$this->dialog->save();
				unset($this->dialog);
				$this->dialog=NULL;
				$this->is_dialog=false;
			}
		}


		/* If dialog is set, but $this->is_dialog==false, then 
		 *  only the "abort" button is shown, this are dialogs that must not be saved.  
		 * If is_dialog == true, we are currently editing tab objects.
		 *  Here we need both, save and cancel
		 */ 

		if($this->dialog != NULL){
			$display .= $this->dialog->execute();
			/* Don't show buttons if tab dialog requests this */
			if (((isset($this->dialog->current))&&($this->dialog->by_object[$this->dialog->current]->is_dialog))){
				
			}elseif(((isset($this->dialog->current))&&(!$this->dialog->by_object[$this->dialog->current]->is_dialog))){
				$display.= "<p style=\"text-align:right\">\n";
				$display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
				$display.= "&nbsp;\n";
				$display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
				$display.= "</p>";
			}elseif(!isset($this->dialog->current)){
				$display.= "<p style=\"text-align:right\">\n";
				$display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
				$display.= "</p>";
			}
			return($display);
		}

		/* Show Management template
		 * The following code will only be reached if no dialog (tabs or dialogs)
		 *  are currently opened.
		 * Assign all reguired vars to template engine
		 */

		/* Create listhead, it will be shown on top of the divlist. 
		 * It provides general navigation and object creation
		 */
		$faihead = 
			"<div style='background:#F0F0F9;padding:5px;'>".
			" <input type='image' align='middle' src='images/list_back.png' title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
			" <input type='image' src='images/list_root.png' align='middle' title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".

			" <input type='image' src='images/select_new_server.png' align='middle' 
			title='"._("New partition table")."' name='Create_partition' alt='"._("Partition")."'>&nbsp;".

			" <input type='image' src='images/zip.png' align='middle' 
			title='"._("New packages")."' name='' alt='"._("Partition")."'>&nbsp;".

			"</div>";

		$this->reload();

		/* Create list with objects */
		$divlist = new divlist("faiManagement"); 
		$divlist->SetEntriesPerPage(20);
		$divlist->SetSummary(_("This table displays all systems, in the selected tree."));

		$divlist->SetHeader(array(
					array("string" => "&nbsp;"),
					array("string" => _("FAI objects / ObjectClasses")),
					array("string" => _("Object type")),
					array("string" => _("Obtions"), "attach" => "style='border:none'" )));


		$action  = "<input type='image' src='images/edit.png' 	alt='"._("edit")."'   name='entry_edit_%KEY%' title='"._("Edit object")."'>";
		$action .= "<input type='image' src='images/editdelete.png' alt='"._("delete")."' name='entry_delete_%KEY%' title='"._("Delete object")."'>";

		/* Attach objects */
		foreach($this->objects as $key => $value){
				$info= "";
				$img = "";
				$type = $value['type'];
				$abort=false;

				switch($type) {
					case "FAIpartitionTable" 	: 
						if(!$faifilter['ShowPartitions']){ 
							$abort = true;
						}
					$img="<img src='images/server.png' title='Partition table' alt='PT'>";
					$info = _("Partition table");
					break;
					case "FAIpackageList"		: 
						if(!$faifilter['ShowPackages']){ 
							$abort = true;
						}
					$img="<img src='images/zip.png' title='Package list' alt='PL'>";
					$info = _("Package list");
					break;
					case "FAIscript"			: 
						if(!$faifilter['ShowScripts']){ 
							$abort = true;
						}
					$img="<img src='images/edit.png' title='Set of scripts' alt='S'>";
					$info = _("Script set");
					break;
					case "FAIvariable"			: 
						if(!$faifilter['ShowVariables']){ 
							$abort = true;
						}
					$img="<img src='images/log_critical.png' title='Variable set' alt='V'>";
					$info = _("Varialbe set");
					break;
					case "FAIhook"				:
						if(!$faifilter['ShowHooks']){ 
							$abort = true;
						}
					$img="<img src='images/rightarrow.png' title='Hooks' alt='H'>";
					$info = _("Hook set");
					break;
					case "FAIprofile"			: 
						if(!$faifilter['ShowProfiles']){ 
							$abort = true;
						}
					$img="<img src='images/tree.png' title='Profile' alt='P'>";
					$info = _("Profile")." (".$value['FAIclass']." )";
					break;
					case "FAItemplate"			: 
						if(!$faifilter['ShowTemplates']){ 
							$abort = true;
						}
					$img="<img src='images/folder.png' title='Template' alt='T'>";
					$info = _("Template");
					break;
					default 					: 
					$img="<img src='images/empty.png' >";$info = "";break;

				}
				if(!$abort)	{
					if((isset($value['description']))&&(!empty($value['description']))){
						$desc= " [".$value['description']."]";
					}else{
						$desc= "";
					}

					$field1 = array("string" => $img , "attach" => "style='height:26px;width=20px;'");
					$field2 = array("string" => $value['cn'].$desc , "attach" => "");
					$field3 = array("string" => $info, "attach" => "");
					$field4 = array("string" => preg_replace("/%KEY%/",$key,$action) , "attach" => "style='border-right:none;'");
					$divlist->AddEntry(array($field1,$field2,$field3,$field4));

			}
		}

		foreach($faifilter as $attrs => $value){
			if($value){
				$smarty->assign($attrs."CHK", " checked ");
			}else{
				$smarty->assign($attrs."CHK", " ");
			}
		}

		/* Assign all reguired vars to template engine */
		$smarty->assign("faihead"		, $faihead);	
		$smarty->assign("failist"		, $divlist->DrawList());
		$smarty->assign("regex"			, $faifilter['regex']);	
		$smarty->assign("infoimage"		, get_template_path('images/info.png'));
		$smarty->assign("launchimage"	, get_template_path('images/launch.png'));
		$smarty->assign("alphabet"		, generate_alphabet());
		$smarty->assign("apply"			, apply_filter());
		$smarty->assign("search_image"	, get_template_path('images/search.png'));

		$display 	= $smarty->fetch(get_template_path('headpage.tpl', TRUE, dirname(__FILE__)));
		return ($display);
	}


	function reload()
	{
		/* Create a new list of FAI object 
		 * Object list depends on faifilter['regex']
		 * Generate List of Partitions,Hooks,Scripts,Templates,Profiles ... 
		 */
		$faifilter= get_global('faifilter');
		// Added for dirlist function...

		/* Set base for all searches */
		$base= $faifilter['base'];
		$regex = $faifilter['regex'];	

		/* Array to save objects */
		$objects = array();
	
		$this->objects=array();

		$res= get_list($this->ui->subtreeACL, "(objectClass=organizationalUnit)", 
				FALSE, $base, array("cn","description","objectClass"),TRUE);

		foreach($res as $objecttypes){
			$res2 = get_list($this->ui->subtreeACL, "(&(objectClass=*)(cn=".$regex."))",
					FALSE, $objecttypes['dn'], array("cn","description","objectClass","FAIclass"),TRUE);
			foreach($res2 as $object){
				$type= "";	
				unset($object['objectClass']['count']);
				if(!isset($object['description'][0])){
					$object['description'][0]="";
				}
				
				/* Clean up object informations */
				$obj['cn'] 			= $object['cn'][0];
				$obj['dn'] 			= $object['dn'];
				$obj['description'] = $object['description'][0];
				$obj['objectClass'] = $object['objectClass'];

				if(in_array("FAIpartitionTable",$obj['objectClass'])){
					$type = "FAIpartitionTable";
				}elseif(in_array("FAIpackageList",$obj['objectClass'])){
					$type = "FAIpackageList";
				}elseif(in_array("FAIscript",$obj['objectClass'])){
					$type = "FAIscript";
				}elseif(in_array("FAIvariable",$obj['objectClass'])){
					$type = "FAIvariable";
				}elseif(in_array("FAIhook",$obj['objectClass'])){
					$type = "FAIhook";
				}elseif(in_array("FAIprofile",$obj['objectClass'])){
					$obj['FAIclass'] 	= $object['FAIclass'][0];
					$type = "FAIprofile";
				}elseif(in_array("FAItemplate",$obj['objectClass'])){
					$type = "FAItemplate";
				}
				$this->objects[$obj['cn'].$type] = $obj;
				$this->objects[$obj['cn'].$type]['type']=$type;
 
			}
		}
		reset ($this->objects);
	}

	function remove_lock()
	{
		if (isset($this->dn)){
			del_lock ($this->dn);
		}
	}

	function get_type($array){
		if(in_array("FAIpartitionTable",$array['objectClass'])){
			return(array("tabsPartition","faiPartitionTable","FAIPARTITIONTABS"));
		}
		if(in_array("FAIscript",$array['objectClass'])){
			return(array("tabsScript","faiScript","FAISCRIPTTABS"));
		}
	}

}

?>
