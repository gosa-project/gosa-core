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
require "tabsVariable.inc";
require "tabsHook.inc";
require "tabsTemplate.inc";
require "tabsScript.inc";
require "tabsProfile.inc";
require "tabsPackage.inc";

class faiManagement extends plugin
{
	/* Definitions */
	var $plHeadline			= "FAI";
	var $plDescription		= "Fully Automatic Installation - management";

	/* CLI vars */
	var $cli_summary		= "Handling of FAI entries";
	var $cli_description	= "This plugin represents a management tool\n
		which allows us to manage all needed attributes for fully automatic installations (FAI)";
	var $cli_parameters		= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

	/* Headpage attributes */
	var $departments= array();
	var $deptabs= NULL;
  var $lock_type    = "";    // should be branch/freeze
  var $lock_name    = "";
  var $lock_dn      = "";  

	/* attribute list for save action */
	var $attributes			= array("lock_type","lock_name","lock_dn"); 	// 	Attributes Managed by this plugin, none, 
	//	because this is only an overview over all objects already defined in FAI.
	var $objectclasses		= array();	// 	ObjectClasses which the attributes are related to
	var $dialog				= array();	// 	This object contains every dialog we have currently opened

	var $objects			= array();	// 	This array contains all available objects shown in divlist
	var $is_dialog		= false;

  var $dispNewBranch= false;
  var $dispNewFreeze= false;

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
			$faifilter= array("base" => $base,
					"regex" => "*","branch" => "main");
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

	/* Call parent execute */
	plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^edit_entry$/","/^id$/","/^entry_edit_/","/^entry_delete_/");

		$display 	= "";
		$smarty 	= get_smarty();
		$smarty->assign("BranchName","");
    $s_action	= "";
		$s_entry 	= "";
		/* filter management 
		 * Filter will be changed if POST['regex'] or $_GET['search'] isset
		 * New Filter will be stored in session and is used to generate list contents
		 */	
		$faifilter = $_SESSION['faifilter'];
    
    if((isset($_POST['select_branch']))&&(!empty($_POST['select_branch']))){
      $faifilter['branch'] = $_POST['select_branch'];
    }

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
      if(preg_match("/remove_branch/",$key)){
        $s_action = "remove_branch";
      }elseif(preg_match("/branch_branch/",$key)){
        $s_action = "branch_branch";
      }elseif(preg_match("/freeze_branch/",$key)){
        $s_action = "freeze_branch";
			}elseif(preg_match("/create_partition/i",$key)){
				$s_action = "new_partition";
			}elseif(preg_match("/create_script/i",$key)){
				$s_action = "new_script";
			}elseif(preg_match("/create_hook/i",$key)){
				$s_action = "new_hook";
			}elseif(preg_match("/create_variable/i",$key)){
				$s_action = "new_variable";
			}elseif(preg_match("/create_template/i",$key)){
				$s_action = "new_template";
			}elseif(preg_match("/create_package/i",$key)){
				$s_action = "new_package";
			}elseif(preg_match("/create_profile/i",$key)){
				$s_action = "new_profile";
			}elseif(preg_match("/edit_continue/",$key)){
				$s_action = "select_class_name_finished";
			}elseif(preg_match("/^entry_edit_.*/",$key)){
				$s_entry = preg_replace("/^entry_edit_/","",$key);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
				$s_action = "edit";
			}elseif(preg_match("/^entry_delete_.*/",$key)){
				$s_entry = preg_replace("/^entry_delete_/","",$key);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
        $s_action = "delete";
      }elseif(preg_match("/dep_back.*/i",$key)){
        $faifilter['branch'] = "main";
        $s_action="back";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $faifilter['branch'] = "main";
        $s_action="home";
      }elseif(preg_match("/dep_root.*/i",$key)){
        $faifilter['branch'] = "main";
        $s_action="root";
      }

    }

		if($s_action=="root"){
			$faifilter['base']=($this->config->current['BASE']);
		}

		if($s_action=="home"){
			$faifilter['base']= get_base_from_people($this->ui->dn);
			//$faifilter['base']=(preg_replace("/^[^,]+,/","",$faifilter['base']));
		}

		if($s_action=="back"){
			$base_back          = preg_replace("/^[^,]+,/","",$faifilter['base']);
			$base_back          = convert_department_dn($base_back);

			if(isset($this->config->departments[trim($base_back)])){
				$faifilter['base']= $this->config->departments[trim($base_back)];
			}else{
				$faifilter['base']= $this->config->departments["/"];
			}
		}

		if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
            $s_action="open";
            $s_entry = base64_decode($_GET['dep_id']);
            $faifilter['base']= $this->config->departments[trim($s_entry)];
        }

		if(isset($_GET['edit_entry'])){
			$s_entry = $_GET['edit_entry'];
			$s_action = "edit";
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
				$smarty->assign("warning", sprintf(_("You're about to delete all information about the FAI class at '%s'."), LDAP::fix($this->dn)));
				return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
			} else {

				/* Obviously the user isn't allowed to delete. Show message and
				   clean session. */
				print_red (_("You are not allowed to delete this component!"));
			}
		}

		/* Delete canceled? */
		if (isset($_POST['delete_cancel'])){
			del_lock ($this->dn);
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
				gosa_log ("FAI class '".$this->dn."' has been removed");
				$this->dialog= NULL;

				/* Terminal list has changed, reload it. */
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
		//if($s_action == "edit"){
		if(($s_action == "edit") && (!isset($this->dialog->config))){
			$entry  = $this->objects[$s_entry];

			$a_setup = ($this->get_type($entry));
		
			$this->dn = $entry['dn'];
			/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
			if (($user= get_lock($this->dn)) != ""){
				return(gen_locked_message ($user, $this->dn));
			}
			add_lock ($this->dn, $this->ui->dn);

			$this->dialog= new $a_setup[0]($this->config,
					$this->config->data['TABS'][$a_setup[2]],$this->dn);
			$this->is_dialog = true;
			$_SESSION['objectinfo'] = $this->dn;
		}


    /*  Branch handling 
        09.01.2006
    */

    /* Create new branch */
    if((isset($_POST['UseBranchName']))&&($this->dispNewBranch)){
     
      /* Check branch name */
      $name = $_POST['BranchName'];
      $is_ok = true;

      $smarty->assign("BranchName",$name);
		  $base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];
      
      /* Check used characters */
      if(preg_match("/[^0-9a-z \ö\ä\ü\.-_:,]/i",$name)){
        print_red(_("Specified branch name is invalid."));
        $is_ok = false;
      }
   
      /* Check if this name is already in use */
      if(!$this->CheckNewBranchName($_POST['BranchName'],$faifilter['branch'])){
        print_red(_("This name is already in use."));
        $is_ok = false;
      }
     
      if($is_ok){
        /* Create it know */
        $ldap = $this->config->get_ldap_link();

        $baseToUse = $base;
        if($_SESSION['faifilter']['branch']!="main"){
          $baseToUse = $_SESSION['faifilter']['branch'];
        }

        $ldap->cd($this->config->current['BASE']);
        $ldap->create_missing_trees($baseToUse);
        $ldap->cd ("ou=".$name.",".$baseToUse);
        $ldap->recursive_remove();
        
        $ldap->cd ($this->config->current['BASE']);
        $ldap->copy_FAI_resource_recursive($baseToUse,"ou=".$name.",".$baseToUse,"branch",true);

        /* Postcreate */ 

        /* Assign possible attributes */
        $this->lock_type  = "branch"; 
        $this->lock_name  = $name; 
        $this->lock_dn    = $baseToUse; 
        $this->postcreate();
        $this->dispNewBranch = false; 
      }
    }

    /* Abort creating new branch */
    if(isset($_POST['CancelBranchName'])){
      $this->dispNewBranch = false;
      $this->dispNewFreeze = false;
    }

    /* Open dialog to insert new branch name */
    if(($s_action == "branch_branch")||($this->dispNewBranch)){
      $this->dispNewBranch=true;
		  $base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];
		  $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
      return($display);
    }

    /* Remove branch */
    if($s_action == "remove_branch"){
      $base= $faifilter['branch'];
      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($faifilter['base'], $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "FAIclass", $base);
      if (chkacl($this->acl, "delete") == ""){
        $smarty->assign("info", sprintf(_("You're about to delete a fai branch / freeze  '%s'."), $faifilter['branch']));
        return($smarty->fetch(get_template_path('remove_branch.tpl',TRUE)));
      } else {
        print_red (_("You are not allowed to delete this release!"));
      }
    }

    /* Delete this entry */
    if(isset($_POST['delete_branch_confirm'])){
      $bb =  $faifilter['branch'];
      if(!isset($ldap)){
        $ldap = $this->config->get_ldap_link();
      }

      $br = $this->getBranches();

      if(isset($br[$bb])){

        $name = $br[$bb];

        $ldap->cd($bb);
        $ldap->recursive_remove();
        $faifilter['branch'] = "main";

        /* Post remove */
                
//        $this->lock_type is set from this->reload
        $this->lock_name   = $name;
        $this->lock_dn     = $bb;

        $this->postremove();
      }
    }


    if((isset($_POST['UseBranchName']))&&($this->dispNewFreeze)){
      /* Check branch name */
      $name = $_POST['BranchName'];
      $is_ok = true;

      $smarty->assign("BranchName",$name);

		  $base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];
      
      /* Check used characters */
      if(preg_match("/[^0-9a-z \ö\ä\ü\.-_:,]/i",$name)){
        print_red(_("Specified branch name is invalid."));
        $is_ok = false;
      }
   
      /* Check if this name is already in use */
      if(!$this->CheckNewBranchName($_POST['BranchName'],$faifilter['branch'])){
        print_red(_("This name is already in use."));
        $is_ok = false;
      }
     
      if($is_ok){
        /* Create it know */
        $ldap = $this->config->get_ldap_link();

        $baseToUse = $base;
        if($_SESSION['faifilter']['branch']!="main"){
          $baseToUse = $_SESSION['faifilter']['branch'];
        }
        
        $ldap->cd ("ou=".$name.",".$baseToUse);
        $ldap->recursive_remove();
        
        $ldap->cd ($this->config->current['BASE']);
        $ldap->copy_FAI_resource_recursive($baseToUse,"ou=".$name.",".$baseToUse,"freeze",true);
        
        $this->dispNewFreeze = false; 
        
        /* Postcreate */ 

        /* Assign possible attributes */
        $this->lock_type  = "branch"; 
        $this->lock_name  = $name; 
        $this->lock_dn    = $baseToUse;
        $this->postcreate();
      }
    }
    
    if(($s_action == "freeze_branch")||($this->dispNewFreeze)){
      $this->dispNewFreeze = true;
		  $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
      return($display);
    }

    /* ENDE Branch handling 
     */

    /* Dialog handling */
		if($s_action == "new_partition"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIpartitionTable");
		}

		/* Dialog handling */
		if($s_action == "new_script"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIscript");
		}

		/* Dialog handling */
		if($s_action == "new_hook"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIhook");
		}

		/* Dialog handling */
		if($s_action == "new_variable"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIvariable");
		}

		/* Dialog handling */
		if($s_action == "new_template"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAItemplate");
		}

		/* Dialog handling */
		if($s_action == "new_package"){
			$this->dialog = new askClassName($this->config,$this->dn,$this->ui,"FAIpackageList");
		}

		/* New Profile */
		if($s_action == "new_profile"){
			$this->dn = "new" ;
			$a_setup= $this->get_type(array("objectClass"=>array("FAIprofile")));

			$this->dialog = new $a_setup[0]($this->config,
					$this->config->data['TABS'][$a_setup[2]],$this->dn);
			$this->is_dialog = true;

		}

		if($s_action == "select_class_name_finished"){
			$this->dialog->save_object();
			if(count($this->dialog->check())!=0){
				foreach($this->dialog->check() as $msg){
					print_red($msg);
				}		
			}else{
				$this->dn = "new" ;
				$a_setup= $this->get_type(array("objectClass"=>array($this->dialog->objectClass)));
				$name = $this->dialog->save();

				$this->dialog = new $a_setup[0]($this->config,
						$this->config->data['TABS'][$a_setup[2]],$this->dn,$name);
				$this->dialog->by_object[$a_setup[1]]->cn = $name;
				$this->is_dialog = true;
			}		
		}	

		/* Search is set */
		if(isset($_GET['search'])){
			$faifilter['regex']=$_GET['search']."*";
		}

		/* Department select */
		if((isset($_POST['base']))&&($s_action=="")){
			$faifilter['base']=$_POST['base'];
		}

    $base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];

    $br = $this->getBranches($base);
    if(!isset($br[$faifilter['branch']])){
      $faifilter['branch'] = "main";
    }
   
    $smarty->assign("select_branch",$faifilter['branch']);

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
			unset($_SESSION['objectinfo']);
			del_lock ($this->dn);
		}

		/* This check if the given tab could be saved 
		 * If it was possible to save it, remove dialog object. 
		 * If it wasn't possible, show errors and keep dialog.
		 */
		if((isset($_POST['edit_finish'])) && (isset($this->dialog->config))){
			$this->dialog->save_object();
			if(count($this->dialog->check())!=0){
				foreach($this->dialog->check() as $msg){
					print_red($msg);
				}
			}else{
				del_lock ($this->dn);
				$this->dialog->save();
				unset($this->dialog);
				$this->dialog=NULL;
				$this->is_dialog=false;
				unset($_SESSION['objectinfo']);
			}
		}

		/* If dialog is set, but $this->is_dialog==false, then 
		 *  only the "abort" button is shown, this are dialogs that must not be saved.  
		 * If is_dialog == true, we are currently editing tab objects.
		 *  Here we need both, save and cancel
		 */ 

		if(($this->dialog != NULL) && (isset($this->dialog->config))){
			$display .= $this->dialog->execute();
			/* Don't show buttons if tab dialog requests this */
			if (((isset($this->dialog->current))&&($this->dialog->by_object[$this->dialog->current]->is_dialog))){
				
			}elseif(((isset($this->dialog->current))&&(!$this->dialog->by_object[$this->dialog->current]->is_dialog))){
				$display.= "<p style=\"text-align:right\">\n";
				$display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Finish")."\">\n";
				$display.= "&nbsp;\n";
				$display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
				$display.= "</p>";
			}elseif(!isset($this->dialog->current)){
				$display.= "<p style=\"text-align:right\">\n";
				$display.= "<input type=\"submit\" name=\"edit_continue\" value=\""._("Continue")."\">&nbsp;";
				$display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
				$display.= "</p>";
			}
			return($display);
		}

		/* Show Management template
		 * The following code will only be reached if no dialog (tabs or dialogs)
		 *  are currently opened.
		 * Assign all reguired vars to template engine
		 */

		/* Prepare departments */
		$options= "";
		foreach ($this->config->idepartments as $key => $value){
			if ($faifilter['base'] == $key){
				$options.= "<option selected='selected' value='$key'>$value</option>";
			} else {
				$options.= "<option value='$key'>$value</option>";
			}
		}

		/* Create listhead, it will be shown on top of the divlist. 
		 * It provides general navigation and object creation
		 */
		$faihead = 
			"<div style='background:#F0F0F9;padding:5px;'>&nbsp;".

			"<input class='center' type='image' src='images/list_up.png' align='middle' 
			title='"._("Go up one department")."' name='dep_back' alt='"._("Up"). "'>&nbsp;".

			"<input class='center' type='image' src='images/list_root.png' align='middle' 
			title='"._("Go to root department")."' name='dep_root' alt='".      _("Root")."'>&nbsp;".

			"<input class='center' type='image' src='images/list_home.png' align='middle' 
			title='"._("Go to users home department")."' name='dep_home' alt='"._("Home")."'>&nbsp;".

			"<img class='center' src=\"images/list_seperator.png\" alt=\"\" align=\"middle\" height=\"16\" width=\"1\">&nbsp;".
			
			" <input class='center' type='image' src='images/fai_new_profile.png' align='middle' 
			title='"._("New profile")."' name='Create_profile' alt='"._("P")."'>&nbsp;".
			
			"<img class='center' src=\"images/list_seperator.png\" alt=\"\" align=\"middle\" height=\"16\" width=\"1\">&nbsp;".
				
			" <input class='center' type='image' src='images/fai_new_partitionTable.png' align='middle' 
			title='"._("New partition table")."' name='Create_partition' alt='"._("PT")."'>&nbsp;".

			" <input class='center' type='image' src='images/fai_new_script.png' align='middle' 
			title='"._("New scripts")."' name='Create_script' alt='"._("S")."'>&nbsp;".

			" <input class='center' type='image' src='images/fai_new_hook.png' align='middle' 
			title='"._("New hooks")."' name='Create_hook' alt='"._("H")."'>&nbsp;".

			" <input class='center' type='image' src='images/fai_new_variable.png' align='middle' 
			title='"._("New variables")."' name='Create_variable' alt='"._("V")."'>&nbsp;".
			
			" <input class='center' type='image' src='images/fai_new_template.png' align='middle' 
			title='"._("New templates")."' name='Create_template' alt='"._("T")."'>&nbsp;".

			" <input class='center' type='image' src='images/fai_new_packages.png' align='middle' 
			title='"._("New package list")."' name='Create_package' alt='"._("PK")."'>&nbsp;".

			" <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".

			_("Base")."&nbsp;<select name='base' onChange='mainform.submit()' class='center'>$options</select>".
			" <input class='center' type='image' src='images/list_submit.png' align='middle' title='"._("Submit department")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
			
			"</div>";

		$this->reload();

		/* Create list with objects */
		$divlist = new divlist("faiManagement"); 
		$divlist->SetEntriesPerPage(0);
		$divlist->SetSummary(_("This table displays all systems, in the selected tree."));

		$divlist->SetHeader(array(
					array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
					array("string" => _("Name of FAI class"), "attach" => "style=''"),
					array("string" => _("Class type"), "attach" => "style='width:200px;'"),
					array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'")));


		$action  = "<input class='center' type='image' src='images/edit.png' 	alt='"._("edit")."'   name='entry_edit_%KEY%' title='"._("Edit class")."'>";
		$action .= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."' name='entry_delete_%KEY%' title='"._("Delete class")."'>";

		$editlink ="<a href='?plug=".$_GET['plug']."&amp;edit_entry=%KEY%'>%NAME%</a>"; 		
		$linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

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
			$field2 = array("string" => "&nbsp;", "attach" => "style='width:200px;'");
			$field3 = array("string" => "&nbsp;", "attach" => "style='width:60px;border-right:0px;text-align:right;'");

			$divlist->AddEntry( array($field0,$field1,$field2,$field3));

		}
    
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
					$img="<img class='center' src='images/fai_partitionTable.png' title='"._("Partition table")."' alt='PT'>";
					$info = _("Partition table");
					break;
					case "FAIpackageList"		: 
						if(!$faifilter['ShowPackages']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_packages.png' title='"._("Package list")."' alt='PL'>";
					$info = _("Package list");
					break;
					case "FAIscript"			: 
						if(!$faifilter['ShowScripts']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_script.png' title='"._("Scripts")."' alt='S'>";
					$info = _("Scripts");
					break;
					case "FAIvariable"			: 
						if(!$faifilter['ShowVariables']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_variable.png' title='"._("Variables")."' alt='V'>";
					$info = _("Variables");
					break;
					case "FAIhook"				:
						if(!$faifilter['ShowHooks']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_hook.png' title='"._("Hooks")."' alt='H'>";
					$info = _("Hooks");
					break;
					case "FAIprofile"			: 
						if(!$faifilter['ShowProfiles']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_profile.png' title='"._("Profile")."' alt='P'>";
					$info = _("Profile");
					break;
					case "FAItemplate"			: 
						if(!$faifilter['ShowTemplates']){ 
							$abort = true;
						}
					$img="<img class='center' src='images/fai_template.png' title='"._("Templates")."' alt='T'>";
					$info = _("Templates");
					break;
					default 					: 
					$img="<img class='center' src='images/empty.png' alt=''>";$info = "";break;

				}
				
				if(!$abort)	{
					if((isset($value['description']))&&(!empty($value['description']))){
						$desc= " [".$value['description']."]";
					}else{
						$desc= "";
					}

          if($value['FAIstate'] == "freeze"){
            $acti = "";
            $edi  = "%NAME%";
          }else{
            $acti = $action;
            $edi = $editlink;
          }

					$field1 = array("string" => $img , "attach" => "style='text-align:center;width:20px;'");
					$field2 = array("string" => preg_replace(array("/%KEY%/","/%NAME%/"),array($key,$value['cn'].$desc),$edi) , "attach" => "style=''");
					$field3 = array("string" => $info, "attach" => "style='width:200px;'");
					$field4 = array("string" => preg_replace("/%KEY%/",$key,$acti) , "attach" => "style='width:60px;border-right:0px;text-align:right;'");
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

    $branches = $this->getBranches();
    asort($branches);
    $smarty->assign("branchKeys",array_flip($branches));
    $smarty->assign("branches",$branches);
    
		$smarty->assign("faihead"		, $faihead);	
		$smarty->assign("failist"		, $divlist->DrawList());
		$smarty->assign("regex"			, $faifilter['regex']);	
		$smarty->assign("infoimage"		, get_template_path('images/info.png'));
		$smarty->assign("branchimage"		, get_template_path('images/branch.png'));
		$smarty->assign("launchimage"	, get_template_path('images/launch.png'));
		$smarty->assign("alphabet"		, generate_alphabet());
		$smarty->assign("apply"			, apply_filter());
		$smarty->assign("search_image"	, get_template_path('images/search.png'));
		$smarty->assign("hint", print_sizelimit_warning());

		$display 	= $smarty->fetch(get_template_path('headpage.tpl', TRUE, dirname(__FILE__)));
		return ($display);
	}

  function getBranches($base = false,$prefix = "")
  {
    $ret = array("main"=>"/");
    $ldap = $this->config->get_ldap_link();
  
    $ldap->cd($this->config->current['BASE']);

    $faifilter = $_SESSION['faifilter'];


    if(!$base){
  		$base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];
    }
    $ldap->cd($base);
    $ldap->ls("(objectClass=FAIbranch)",$base);

    while($attrs = $ldap->fetch()){
      
      if((isset($attrs['ou']))&&($attrs['dn']!=$base)){
        $ret[$attrs['dn']]= $prefix.$attrs['ou'][0];
        $ret = array_merge($ret,$this->getBranches($attrs['dn'],"".$prefix.$attrs['ou'][0]."/"));
      }
    }
    return($ret);
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
		$base= "ou=fai,ou=configs,ou=systems,".$faifilter['base'];

    if($faifilter['branch'] !="main"){
      $base = $faifilter['branch'];
    }
   
		$regex = $faifilter['regex'];	

		/* Array to save objects */
		$objects = array();
	
		$this->objects=array();

		/* NEW LIST MANAGMENT
		 * We also need to search for the departments
		 * So we are able to navigate like in konquerer
		 */
		$base2 = $faifilter['base'];

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

	
		$res= get_list($this->ui->subtreeACL, "(objectClass=organizationalUnit)", 
				FALSE, $base, array("cn","description","objectClass"),TRUE);

    $str = "";

		foreach($res as $objecttypes){
			$res2 = get_list($this->ui->subtreeACL, "(&(objectClass=*)(cn=".$regex."))",
					FALSE, $objecttypes['dn'], array("cn","description","objectClass","FAIclass","FAIstate"),TRUE);
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

        if(isset($object['FAIstate'])){
          $obj['FAIstate'] = $object['FAIstate'][0];

          /* Append type to this string, to be able to check if the selected 
           * entry is of type 'freeze' or 'branch'
           */
          $str.="|".$obj['FAIstate'];
        }else{
          $obj['FAIstate'] ="";
        }

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
				$this->objects[strtolower($obj['cn']).$obj['cn'].$type] = $obj;
				$this->objects[strtolower($obj['cn']).$obj['cn'].$type]['type']=$type;
 
			}
		}

    /* Tell class what we have currently opened 
        'main' has type '' 
     */
    if(preg_match("/freeze/",$str)){
      $this->lock_type = "freeze";
    }elseif(preg_match("/branch/",$str)){
      $this->lock_type = "branch";
    }else{
      $this->lock_type = "";
    }

		ksort($this->objects);
		reset ($this->objects);
	
		/* use numeric index, thats a bit more secure */	
		$tmp0 = array();
		foreach($this->objects as $obj){
			$tmp0[]= $obj;
		}
		$this->objects = array();
		$this->objects = $tmp0;

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
		if(in_array("FAItemplate",$array['objectClass'])){
			return(array("tabsTemplate","faiTemplate","FAITEMPLATETABS"));
		}
		if(in_array("FAIhook",$array['objectClass'])){
			return(array("tabsHook","faiHook","FAIHOOKTABS"));
		}
		if(in_array("FAIvariable",$array['objectClass'])){
			return(array("tabsVariable","faiVariable","FAIVARIABLETABS"));
		}
		if(in_array("FAIprofile",$array['objectClass'])){
			return(array("tabsProfile","faiProfile","FAIPROFILETABS"));
		}
		
		if(in_array("FAIpackageList",$array['objectClass'])){
			return(array("tabsPackage","faiPackage","FAIPACKAGETABS"));
		}
	}

  function CheckNewBranchName($name,$base){
    $f = $_SESSION['faifilter']['branch'];

    if($f == "main"){
      $f = $_SESSION['faifilter']['base'];
    } 
    
    if(in_array($name,$this->getBranches($f))) {
      return(false);
    }

    if(empty($name)){
      return(false);
    }
    
    if(in_array($name,array("fai","disk","packages","scripts","templates","hooks","variables","profiles"))){
      return(false);
    }
  
    return(true);
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
