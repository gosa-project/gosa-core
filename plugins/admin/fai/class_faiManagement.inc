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

	/* Headpage attributes */
  var $lock_type    = "";    // should be branch/freeze
  var $lock_name    = "";
  var $lock_dn      = "";  

	/* attribute list for save action */
	var $attributes		= array("lock_type","lock_name","lock_dn"); 	// 	Attributes Managed by this plugin 
	var $objectclasses= array();	// 	ObjectClasses which the attributes are related to
	var $dialog				= array();	// 	This object contains every dialog we have currently opened

	var $objects			= array();	// 	This array contains all available objects shown in divlist
	var $is_dialog		= false;

  var $dispNewBranch= false;
  var $dispNewFreeze= false;
  var $DivListFai;
  var $CopyPasteHandler     = NULL;

	/* construction/reconstruction 
	 */
	function faiManagement ($config, $ui)
	{
		/* Set defaults */
		$this->dn			= "";
		$this->config	= $config;
		$this->ui			= $ui;	
    
    /* Creat dialog object */
    $this->DivListFai = new divListFai($this->config,$this);
    $this->DivListFai->AvailableBranches = $this->getBranches();

    /* Copy & Paste handler */
    if ($this->config->boolValueIsTrue("main", "enableCopyPaste")){
      $this->CopyPasteHandler= new CopyPasteHandler($this->config);
    }
	}

	function execute()
	{
    /* Call parent execute */
    plugin::execute();
  
    /* Get new list of available branches */
    $this->DivListFai->AvailableBranches = $this->getBranches();

    /* Initialise vars and smarty */
		$smarty 	= get_smarty();
		$smarty->assign("BranchName","");
    
		$display 	= "";
    $s_action	= "";
		$s_entry 	= "";
    
    /* If an entry was locked, these vars will be stored in a session to allow direct edit */
    $_SESSION['LOCK_VARS_TO_USE'] = array("/^edit_entry$/","/^id$/","/^entry_edit_/","/^entry_delete_/");


    /****************
      Handle posts 
     ****************/

		/* Check ImageButton posts
		 * Create new tab ich new_xx is posted
		 */
    $posts = array( "/remove_branch/"=>"remove_branch",    "/branch_branch/"=>"branch_branch",
                    "/freeze_branch/"=>"freeze_branch",    "/create_partition/i"=>"new_partition",
                    "/create_script/i"=>"new_script",      "/create_hook/i"=>"new_hook",
                    "/create_variable/i"=>"new_variable",  "/create_template/i"=>"new_template",
                    "/create_package/i"=>"new_package",    "/create_profile/i"=>"new_profile",
                    "/edit_continue/"=>"select_class_name_finished");

		foreach($_POST as $name => $value){
      foreach($posts as $reg => $act ){
        if(preg_match($reg,$name)){
          $s_action = $act;
        }
      }
			if(preg_match("/^entry_edit_.*/",$name)){
				$s_entry = preg_replace("/^entry_edit_/","",$name);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
				$s_action = "edit";
			}elseif(preg_match("/^entry_delete_.*/",$name)){
				$s_entry = preg_replace("/^entry_delete_/","",$name);
				$s_entry = preg_replace("/_.*$/","",$s_entry);
        $s_action = "delete";
      }

      /* Check for Copy & Paste actions */
      foreach(array(  "editPaste" => "editPaste",   
            "copy"      => "copy",
            "cut"       => "cut") as $act => $action){
        if (preg_match("/".$act.".*/", $name)){
          $s_action= $action;
          $s_entry= preg_replace("/".$act."_/i", "", $name);
				  $s_entry = preg_replace("/_.*$/","",$s_entry);
          break;
        }
      }
    }

		if(isset($_GET['edit_entry'])){
			$s_entry = $_GET['edit_entry'];
			$s_action = "edit";
		}
    
    if((isset($_POST['CancelBranchName'])) || (isset($_POST['CloseIFrame']))){
      $this->dispNewBranch = false;
      $this->dispNewFreeze = false;
    }

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling($s_action,$s_entry);
    if($ret){
      return($ret);
    }

    /****************
      Delete confirme dialog 
     ****************/

		if ($s_action=="delete"){

			/* Get 'dn' from posted termlinst */
			$this->dn= $this->objects[$s_entry]['dn'];

			/* Load permissions for selected 'dn' and check if
			   we're allowed to remove this 'dn' */
			$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
			$this->acl= get_module_permission($acl, "FAIclass", $this->dn);
			if (chkacl($this->acl, "delete") == ""){

				/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
				if (($user= get_lock($this->dn)) != ""){
					return(gen_locked_message ($user, $this->dn));
				}

				/* Lock the current entry, so nobody will edit it during deletion */
				add_lock ($this->dn, $this->ui->dn);
				$smarty->assign("warning", sprintf(_("You're about to delete all information about the FAI class at '%s'."), @LDAP::fix($this->dn)));
				return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
			} else {

				/* Obviously the user isn't allowed to delete. Show message and
				   clean session. */
				print_red (_("You are not allowed to delete this component!"));
			}
		}


    /****************
      Delete aborted  
     ****************/

		/* Delete canceled? */
		if (isset($_POST['delete_cancel'])){
			del_lock ($this->dn);
		}


    /****************
      Delete confirmed 
     ****************/

		/* Deltetion was confirmed, so delete this entry
		 */
		if (isset($_POST['delete_terminal_confirm'])){

			/* Some nice guy may send this as POST, so we've to check
			   for the permissions again. */
			$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
			$this->acl= get_module_permission($acl, "FAIclass", $this->dn);
			if (chkacl($this->acl, "delete") == ""){

				/* Find out more about the object type */
				$ldap	  = $this->config->get_ldap_link();
				$ldap->cat($this->dn, array('objectClass'));
				$attrs	= $ldap->fetch();
				$type	  = $this->get_type($attrs);			
				$this->dialog = new $type[0]($this->config,	$this->config->data['TABS'][$type[2]], $this->dn);
				$this->dialog->set_acl(array($this->acl));

				$this->dialog->by_object[$type[1]]->remove_from_parent ();
				unset ($this->dialog);
				gosa_log ("FAI class '".$this->dn."' has been removed");
				$this->dialog= NULL;
        $this->DivListFai->AvailableBranches = $this->getBranches();
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


    /****************
      Edit entry 
     ****************/

		if(($s_action == "edit") && (!isset($this->dialog->config))){
			$entry    = $this->objects[$s_entry];
			$a_setup  = $this->get_type($entry);
			$this->dn = $entry['dn'];

			/* Check locking, save current plugin in 'back_plugin', so the dialog knows where to return. */
			if (($user= get_lock($this->dn)) != ""){
				return(gen_locked_message ($user, $this->dn));
			}
			add_lock ($this->dn, $this->ui->dn);

			$this->dialog     = new $a_setup[0]($this->config,$this->config->data['TABS'][$a_setup[2]],$this->dn);
			$this->is_dialog  = true;

      if($entry['FAIstate'] == "freeze"){
        $this->dialog->set_acl(array("#none#"))  ;
      }
			$_SESSION['objectinfo'] = $this->dn;
		}


    /*  Branch handling 
        09.01.2006
    */

    /****************
      Remove branch
     ****************/

    /* Remove branch 
     */
    if($s_action == "remove_branch"){

      /* Check if we have a post remove method configured
       *  else skip this operation. (Skip:Button in the ui should be disabled in this case)
       */
      if("" != search_config($this->config->data,"faiManagement","POSTREMOVE")){

          $base= $this->DivListFai->selectedBranch;

          /* Load permissions for selected 'dn' and check if
             we're allowed to remove this 'dn' */
          $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
          $this->acl= get_module_permission($acl, "FAIclass", $base);
          if (chkacl($this->acl, "delete") == ""){
          $smarty->assign("info", sprintf(_("You're about to delete a fai branch / freeze  '%s'."), $this->DivListFai->selectedBranch));
          return($smarty->fetch(get_template_path('remove_branch.tpl',TRUE)));
        } else {
          print_red (_("You are not allowed to delete this release!"));
        }
      }
    }

    
    /****************
      Remove branch confirmed
     ****************/

    if(isset($_POST['delete_branch_confirm'])){

      /* Check if we have a post remove method configured
       *  else skip this operation. (Skip:Button in the ui should be disabled in this case)
       */
      if("" != search_config($this->config->data,"faiManagement","POSTREMOVE")){

        $bb =  $this->DivListFai->selectedBranch;
        if(!isset($ldap)){
          $ldap = $this->config->get_ldap_link();
        }

        $br = $this->DivListFai->AvailableBranches;

        if(isset($br[$bb])){
          $name = $br[$bb];
          $ldap->cd($bb);
          $ldap->recursive_remove();
          $ldap->cd(preg_replace('/,ou=fai,ou=configs,ou=systems,/', ',ou=apps,', $bb));
          $ldap->recursive_remove();
          $this->DivListFai->selectedBranch = "main";
          $this->DivListFai->AvailableBranches = $this->getBranches();

          /* Post remove */
          $this->lock_name   = $name;
          $this->lock_dn     = $bb;
          $this->postremove();
        }
      }
    }


    /****************
      Create a new branch "insert Name"
     ****************/

    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
    if((isset($_POST['UseBranchName']))&&(($this->dispNewBranch)||($this->dispNewFreeze)) && chkacl($this->acl,"FAIclass") == ""){
      $_SESSION['LASTPOST'] = $_POST;

      if($this->dispNewBranch){
        $type = "branch";
      }else{
        $type = "freeze";
      }

      /* Check branch name */
      $name = $_POST['BranchName'];
      $is_ok = true;
      $smarty->assign("BranchName",$name);
      $base= "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;

      /* Check used characters */
      if(preg_match("/[^0-9a-z \ö\ä\ü\.-_:,]/i",$name)){
        if($type == "branch"){
          print_red(_("Specified branch name is invalid."));
        }else{
          print_red(_("Specified freeze name is invalid."));
        }
        $is_ok = false;
      }

      /* Check if this name is already in use */
      if(!$this->CheckNewBranchName($_POST['BranchName'],$this->DivListFai->selectedBranch)){
        print_red(_("This name is already in use."));
        $is_ok = false;
      }

      if($is_ok){
        $_SESSION['LASTPOST']['base'] = $base;
        $_SESSION['LASTPOST']['type'] = $type;
        $smarty->assign("iframe", true);
        $smarty->assign("plugID", $_GET['plug']);
        $display 	= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
        return($display);
      }
    }


    /****************
      Create a new branch 
     ****************/

    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
    if(isset($_GET['PerformBranch']) && chkacl($this->acl,"FAIclass") == ""){
      /* Create it know */
      $this->dispNewBranch = false;
      $this->dispNewFreeze = false;
      $base = $_SESSION['LASTPOST']['base'];
      $_POST = $_SESSION['LASTPOST'];      
      $name = $_POST['BranchName'];

      $type = $_SESSION['LASTPOST']['type'];
      $ldap = $this->config->get_ldap_link();

      $baseToUse = $base;
      if($this->DivListFai->selectedBranch != "main" ){
        $baseToUse = $this->DivListFai->selectedBranch;
      }
  
      /* Create new Release name to be able to set faidebianRelease for FAIpackageList */
      
      $CurrentReleases  = $this->DivListFai->AvailableBranches;
      $NewReleaseName   = $name;
      if(isset($CurrentReleases[$this->DivListFai->selectedBranch])) {
        if($this->DivListFai->selectedBranch != "main"){
          $NewReleaseName = $CurrentReleases[$this->DivListFai->selectedBranch]."/".$name;
          $NewReleaseName = preg_replace("#\/#","/",$NewReleaseName); 
        }else{
          $NewReleaseName   = $name;
        }
      }

      $appsrc = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=apps,",$baseToUse); 
      $appdst = preg_replace("/ou=fai,ou=configs,ou=systems,/","ou=apps,","ou=".$name.",".$baseToUse) ; 

      /* Check if source depeartments exist */
      foreach(array($baseToUse,$appsrc) as $dep){
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($dep);
        if(!$ldap->count()){
          $ldap->create_missing_trees($dep);
        }
      }
    


      /* Print out html introduction */
      echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
        <html>
        <head>
        <title></title>
        <style type="text/css">@import url("themes/default/style.css");</style>
        <script language="javascript" src="include/focus.js" type="text/javascript"></script>
        </head>
        <body style="background: none; margin:4px;" id="body" >
        ';

      /* Duplicate applications 
       */
      $ldap->cd ($appdst);
      $ldap->recursive_remove();
      $ldap->cd ($this->config->current['BASE']);
      $ldap->copy_FAI_resource_recursive($appsrc,$appdst,$NewReleaseName,$type,true);

      /* Duplicate fai objects 
       */
      $ldap->cd ("ou=".$name.",".$baseToUse);
      $ldap->recursive_remove();
      $ldap->cd ($this->config->current['BASE']);
      $ldap->copy_FAI_resource_recursive($baseToUse,"ou=".$name.",".$baseToUse,$NewReleaseName,$type,true);

      echo "<div style='width:100%;text-align:right;'><form name='form' method='post' action='?plug=".$_GET['plug']."' target='_parent'>
              <br><input type='submit' name='CloseIFrame' value='"._("Continue")."'>
            </form></div>";

      echo "<script language=\"javascript\" type=\"text/javascript\">scrollDown2();</script>" ;

      /* Print footer to have valid html */
      echo "</body></html>";

      $this->dispNewFreeze = false; 

      /* Postcreate */ 

      /* Assign possible attributes */
      $this->lock_type  = $type; 
      $this->lock_name  = $name; 
      $this->lock_dn    = $baseToUse;
      $this->postcreate();
      $this->DivListFai->AvailableBranches = $this->getBranches();
      exit();
    }


    /****************
      Display dialog to enter new Branch name
     ****************/
  
    /* Check if we have a post create method configured 
     *  else skip this operation. (Skip:Button in the ui should be disabled in this case)
     */
    if("" != search_config($this->config->data,"faiManagement","POSTCREATE")){
      $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
      if((($s_action == "branch_branch")||($this->dispNewBranch)) && chkacl($this->acl,"FAIclass") == ""){
        $this->dispNewBranch=true;
        $smarty->assign("iframe",false);
        $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
        return($display);
      }
    }

    /****************
      Display dialog to enter new Freeze name
     ****************/

    /* Check if we have a post create method configured 
     *  else skip this operation. (Skip:Button in the ui should be disabled in this case)
     */
    if("" != search_config($this->config->data,"faiManagement","POSTCREATE")){
      $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
      if((($s_action == "freeze_branch")||($this->dispNewFreeze)) && chkacl($this->acl,"FAIclass") == ""){
        $this->dispNewFreeze = true;
        $smarty->assign("iframe",false);
        $display 	.= $smarty->fetch(get_template_path('faiNewBranch.tpl', TRUE, dirname(__FILE__)));
        return($display);
      }
    }


    /****************
      Create a new object 
     ****************/

    $types = array( "new_partition"     =>  "FAIpartitionTable",
                    "new_script"        =>  "FAIscript",
                    "new_hook"          =>  "FAIhook",
                    "new_variable"      =>  "FAIvariable",
                    "new_template"      =>  "FAItemplate",
                    "new_package"       =>  "FAIpackageList");

    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
    if(isset($types[$s_action]) && chkacl($this->acl,"FAIclass") == ""){
      $this->dialog = new askClassName($this->config,$this->dn,$this->ui,$types[$s_action]);
    }

    /* New Profile */
    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
    if($s_action == "new_profile" && chkacl($this->acl,"FAIclass") == ""){
      $this->dn = "new" ;
      $a_setup= $this->get_type(array("objectClass"=>array("FAIprofile")));

      $this->dialog = new $a_setup[0]($this->config,
          $this->config->data['TABS'][$a_setup[2]],$this->dn);
      $this->is_dialog = true;
    }


    /****************
      Get from ask class name dialog 
     ****************/

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


    /****************
     Cancel dialogs 
     ****************/

		if(isset($_POST['edit_cancel'])){
			unset($this->dialog);
			$this->dialog=NULL;
			$this->is_dialog = false;
			unset($_SESSION['objectinfo']);
			del_lock ($this->dn);
		}


    /****************
      Save sub dialogs 
     ****************/

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


    /****************
      Display currently open dialog 
     ****************/

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
				$display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Save")."\">\n";
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
		

    /****************
      Dialog display
     ****************/

    /* Display dialog with system list */
    $this->DivListFai->parent = $this;
    $this->DivListFai->execute();
    $this->DivListFai->AddDepartments($this->DivListFai->selectedBase,4);
    $this->reload();
    $this->DivListFai->setEntries($this->objects);
    return($this->DivListFai->Draw());
	}


  /* Get available branches for current base */
  function getBranches($base = false,$prefix = "")
  {
    $ret = array("main"=>"/");
    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $this->DivListFai->selectedBase);
    if(chkacl($this->acl,"FAIclass") != ""){
      return($ret);
    }
    
    $ldap = $this->config->get_ldap_link();
    if(!$base){
      $base= "ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
    }
    $ldap->ls("(objectClass=FAIbranch)",$base);
    $objects = array();
    while($attrs = $ldap->fetch()){
      $objects[] = $attrs;
    }
    foreach($objects as $attrs){

      if((isset($attrs['ou']))&&($attrs['dn']!=$base)){
        $ret[$attrs['dn']]= $prefix.$attrs['ou'][0];
        $ret = array_merge($ret,$this->getBranches($attrs['dn'],"".$prefix.$attrs['ou'][0]."/"));
      }
    }

    return ($ret);
  }

 
   /* Perform copy & paste requests
      If copy&paste is in progress this returns a dialog to fix required attributes
   */
  function copyPasteHandling($s_action,$s_entry)
  {
  
    /* Only perform copy/paste if it is enabled */
    if($this->CopyPasteHandler){

      /* Prepare current object to be pasted */
      if( $s_action == "editPaste" || $this->CopyPasteHandler->stillOpen()){

        $this->CopyPasteHandler->save_object();
        $this->CopyPasteHandler->SetVar("base",             $this->DivListFai->selectedBase);

        if($this->CopyPasteHandler->copyCurrent){
          $this->CopyPasteHandler->SetVar("copy_paste_mode",  TRUE);
        }
        if($this->CopyPasteHandler->cutCurrent){
          $this->CopyPasteHandler->SetVar("cut_paste_mode",  TRUE);
        }
    
        $release = "ou=fai,ou=configs,ou=systems,".$_SESSION['CurrentMainBase'];
        if($_SESSION['faifilter']['branch']!="main"){
                $release = $_SESSION['faifilter']['branch'];
        }
        $this->CopyPasteHandler->SetVar("release",  $release);

        /* Execute copy & paste dialog and display returned data, normaly a dialog which allows
           us to solve all attribute mismatches for this object.
            If nothing is returned, copy & paste was succesfully or aborted */
        if(($ret= $this->CopyPasteHandler->execute())){
          return ($ret);
        }

        /* Use the last dn to search for it's ID in the newly generated list. */
        $dn= $this->CopyPasteHandler->lastdn;

        /* Get new list */
        $this->reload();
        foreach($this->objects as $id => $entry){
          if($entry['dn'] == $dn){
            $s_entry= $id;
            break;
          }
        }
      }

      /* Copy selected object
          Create a new empty object and the current selected object.
          Send both to copy&paste class*/
      if($s_action == "copy"){
        $this->CopyPasteHandler->Clear();

			  $dn= $this->objects[$s_entry]['dn'];
        $acl= get_permissions ($dn, $this->ui->subtreeACL);

        $ldap       = $this->config->get_ldap_link();
        $ldap->cat($dn);
        $attr       = $ldap->fetch();
        $tmp        = $this->get_type($attr);

        if(!count($tmp) == 3 ) return;

        $tabs  = $tmp[0];
        $class = $tmp[1];
        $data  = $tmp[2];

        $obj    = new $tabs($this->config, $this->config->data['TABS'][$data], $dn);
        $objNew = new $tabs($this->config, $this->config->data['TABS'][$data], "new");
        $obj->    set_acl($acl);
        $objNew-> set_acl($acl);
        $this->CopyPasteHandler->Copy($obj,$objNew);
      }

      /* Cut selected object.
          Open user object and send it to the copy & paste handler */
      if($s_action == "cut"){
        $this->CopyPasteHandler->Clear();
			  $dn= $this->objects[$s_entry]['dn'];

        $ldap       = $this->config->get_ldap_link();
        $ldap->cat($dn);
        $attr       = $ldap->fetch();
        $tmp        = $this->get_type($attr);

        if(!count($tmp) == 3 ) return;

        $tabs  = $tmp[0];
        $class = $tmp[1];
        $data  = $tmp[2];
        $obj= new $tabs($this->config, $this->config->data['TABS'][$data], $dn);
        $this->CopyPasteHandler->Cut($obj);
      }
    }
  }

    
 
  /* reload list of objects */
	function reload()
	{
    /* Create a new list of FAI object 
     * Generate List of Partitions,Hooks,Scripts,Templates,Profiles ... 
     */
    $ObjectTypes = array(
        "FAIpartitionTable"  => array("OU"=>"ou=disk,"       , "CHKBOX"=>"ShowPartitions"),
        "FAIpackageList"     => array("OU"=>"ou=packages,"    , "CHKBOX"=>"ShowPackages"),
        "FAIscript"          => array("OU"=>"ou=scripts,"     , "CHKBOX"=>"ShowScripts"),
        "FAIvariable"        => array("OU"=>"ou=variables,"   , "CHKBOX"=>"ShowVariables"),
        "FAIhook"            => array("OU"=>"ou=hooks,"       , "CHKBOX"=>"ShowHooks"),
        "FAIprofile"         => array("OU"=>"ou=profiles,"    , "CHKBOX"=>"ShowProfiles"),
        "FAItemplate"        => array("OU"=>"ou=templates,"   , "CHKBOX"=>"ShowTemplates"));

    /* Set base for all searches */
    $base           = "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;
    if($this->DivListFai->selectedBranch != "main"){
      $br = $this->DivListFai->AvailableBranches;
      if(isset($br[$this->DivListFai->selectedBranch])){
        $base = $this->DivListFai->selectedBranch;
      }else{
        $base = "ou=fai,ou=configs,ou=systems,".$this->DivListFai->selectedBase;
      }
    }

    $Regex          = $this->DivListFai->Regex;
    $str            = "";
    $objects        = array();
    $this->objects  = array();

    $acl= get_permissions ($this->DivListFai->selectedBase, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "FAIclass", $base);
    if(chkacl($this->acl,"FAIclass") != ""){
      return;
    }

    /* Walk through possible sub departments and fetch all FAI objects */
		foreach($ObjectTypes as $type => $obj){

      /* Skip all unchecked types */
      if(! $this->DivListFai->$obj['CHKBOX']){
        continue;
      }

      /* Fetch objects from fai sub department */
      $res= get_list("(&(objectClass=".$type.")(cn=$Regex))", $this->ui->subtreeACL, $obj['OU'].$base,
            array("cn","description","objectClass","FAIclass","FAIstate"), GL_SIZELIMIT);

      /* Walk through objects */
			foreach($res as $object){

        /* Prepare object */
				unset($object['objectClass']['count']);
				if(!isset($object['description'][0])){
					$object['description'][0]="";
				}
				
				/* Clean up object informations */
				$obj['cn'] 			      = $object['cn'][0];
				$obj['dn'] 			      = $object['dn'];
				$obj['description']   = $object['description'][0];
				$obj['objectClass']   = $object['objectClass'];

        /* Append type to this string, to be able to check if the selected 
         * entry is of type 'freeze' or 'branch'
         */
        if(isset($object['FAIstate'])){
          $obj['FAIstate'] = $object['FAIstate'][0];
          $str.="|".$obj['FAIstate'];
        }else{
          $obj['FAIstate'] ="";
        }
				$this->objects[strtolower($obj['cn']).$obj['cn'].$type] = $obj;
				$this->objects[strtolower($obj['cn']).$obj['cn'].$type]['type']=$type;
			}
		}

    /* Tell class what we have currently opened 
        'main' has type '' */
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

  function CheckNewBranchName($name,$base)
  {
    $f = $this->DivListFai->selectedBranch;
    if(empty($name)){
      return(false);
    }elseif(in_array($name,$this->getBranches($f))) {
      return(false);
    }elseif(empty($name)){
      return(false);
    }elseif(is_department_name_reserved($name,$base)){
      return(false);
    }
    return(true);
  }

  function save_object()
  {
    $this->DivListFai->save_object();
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
