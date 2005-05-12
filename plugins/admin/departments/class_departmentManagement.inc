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

  function departmentManagement ($config, $ui)
  {
	$this->ui= $ui;
	$this->dn= "";
	$this->config= $config;

        /* Get global filter config */
        if (!is_global("depfilter")){
                $base= get_base_from_people($ui->dn);
                $depfilter= array("depselect" => $base,
                                  "regex" => "*");
                register_global("depfilter", $depfilter);
        }
  }

  function execute()
  {
	/* Reload departments */
	$this->config->departments= get_departments();
	$this->config->make_idepartments();
	$smarty= get_smarty();

        /* Save data */
        $depfilter= get_global("depfilter");
        foreach( array("depselect", "regex") as $type){
                if (isset($_POST[$type])){
                        $depfilter[$type]= $_POST[$type];
                }
        }
        if (isset($_GET['search'])){
                $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
                if ($s == "**"){
                        $s= "*";
                }
                $depfilter['regex']= $s;
        }
        register_global("depfilter", $depfilter);


	/* Create new department? */
	if (isset($_POST['new_department'])){
		$this->acl= array(":all");
		$this->dn= "new";

                /* Register deptabs to trigger edit dialog */
		$this->deptabs= new deptabs($this->config,
				$this->config->data['TABS']['DEPTABS'], $this->dn);
		$this->deptabs->set_acl($this->acl);
	}

	/* Edit existing department */
	if ((isset($_POST['select_department']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['deplist']) != ""){
		$this->dn= $this->config->departments[trim($_POST['deplist'])];

		/* Check locking, save current plugin in 'back_plugin', so
		   the dialog knows where to return. */
		if (($user= get_lock($this->dn)) != ""){
			return(gen_locked_message ($user, $this->dn));
		}

		/* Lock the current entry, so everyone will get the
		   above dialog */
		add_lock ($this->dn, $this->ui->dn);

		/* Set up the users ACL's for this 'dn' */
		$this->acl= get_permissions ($this->dn, $this->ui->subtreeACL);

                /* Register deptabs to trigger edit dialog */
		$this->deptabs= new deptabs($this->config,
				$this->config->data['TABS']['DEPTABS'], $this->dn);
		$this->deptabs->set_acl($this->acl);
		$_SESSION['objectinfo']= $this->dn;
	}


	/* Delete requested department */
	$display= "";
	if (isset($_POST['delete_department']) && isset($_POST['deplist'])){
		$this->dn= $this->config->departments[trim($_POST['deplist'])];

		/* Check locking */
		if (($user= get_lock($this->dn)) != ""){
			$_SESSION['dn']= $this->dn;
			return(gen_locked_message($user, $this->dn));
		} else {
			# Lock this dn for editing
			add_lock ($this->dn, $this->ui->dn);

			$smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), $this->dn));
			$display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
			return ($display);
		}
	}

	/* Finally delete department */
	if (isset($_POST['delete_department_confirm'])){
		$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
		$acl= get_module_permission($acl, "department", $this->dn);
		if (chkacl($acl, "all") == ""){
			$this->remove_from_parent();
			gosa_log ("Department object'".$this->dn."' has been removed");
			$this->reload ();
		} else {
			print_red (_("You have no permission to remove this department."));
		}
	}

	/* Finish request */
       if (isset($_POST['edit_finish'])){

                /* Check tabs, will feed message array */
                $message= $this->deptabs->check();

                /* Save, or display error message? */
                if (count($message) == 0){

                        /* Save user data to ldap */
                        $this->deptabs->save();
                        gosa_log ("Department object'".$this->dn."' has been saved");

                        /* Group has been saved successfully, remove lock from
                           LDAP. */
                        if ($this->dn != "new"){
                                del_lock ($this->dn);
                        }

                        /* There's no page reload so we have to read new users at
                           this point. */
                        $this->reload ();
                        unset ($this->deptabs);
                        $this->deptabs= NULL;
			unset ($_SESSION['objectinfo']);

                } else {
                        /* Ok. There seem to be errors regarding to the tab data,
                           show message and continue as usual. */
                        show_errors($message);
                }
        }


	/* Cancel dialog */
	if (isset($_POST['edit_cancel']) || isset($_POST['delete_cancel']) ||
		isset($_POST['delete_department_confirm']) || isset($_POST['delete_lock'])){

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

		/* Show main page */
		$smarty->assign("departments", $this->departments);
		$smarty->assign("search_image", get_template_path('images/search.png'));
		$smarty->assign("tree_image", get_template_path('images/tree.png'));
		$smarty->assign("infoimage", get_template_path('images/info.png'));
		$smarty->assign("launchimage", get_template_path('images/launch.png'));
		$smarty->assign("deplist", $this->config->idepartments);
		foreach( array("depselect", "regex") as $type){
                	$smarty->assign("$type", $depfilter[$type]);
		}

		/* Extend if we are not using javascript */
	        $smarty->assign("apply", apply_filter());
	        $smarty->assign("alphabet", generate_alphabet());
		$smarty->assign("hint", print_sizelimit_warning());

		$display= $smarty->fetch(get_template_path('headpage.tpl', TRUE));
		return($display);
	}

	/* Show main page (tabs) */
	$display= $this->deptabs->execute();

	$display.= "<p style=\"text-align:right\">\n";
	$display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
	$display.= "&nbsp;\n";
	$display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
	$display.= "</p>";

	return ($display);
  }


  function reload()
  {
	/* Get config */
        $depfilter= get_global('depfilter');

        /* Set base for all searches */
        $base= $depfilter['depselect'];

        /* Regex filter? */
        if ($depfilter['regex'] != ""){
                $regex= $depfilter['regex'];
        } else {
                $regex= "*";
        }

	/*Get all gosaDepartments */
	$res= get_list($this->ui->subtreeACL, "(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))", TRUE, $base, array("ou", "description"), TRUE);

	$this->departments= array();
	foreach ($res as $value){
		$title= convert_department_dn($value["dn"]);
		$this->departments[$title]= "$title [".$value["description"][0]."]";
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

?>
