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
require "tabs_group.inc";

class groupManagement extends plugin
{
  /* Definitions */
  var $plHeadline= "Groups";
  var $plDescription= "This does something";

  /* Dialog attributes */
  var $grouptab= NULL;
  var $grouplist= array();
  var $ui= NULL;
  var $acl= "";

  function groupManagement ($config, $ui)
  {
	/* Save configuration for internal use */
	$this->config= $config;
	$this->ui= $ui;

	/* Get global filter config */
	if (!is_global("groupfilter")){
		$base= get_base_from_people($ui->dn);
		$groupfilter= array("primarygroups" => "checked",
					"mailgroups" => "checked",
					"sambagroups" => "checked",
					"appgroups" => "checked",
					"functionalgroups" => "checked",
					"guser" => "*",
					"subsearch" => "",
					"depselect" => $base,
					"regex" => "*");
		register_global("groupfilter", $groupfilter);
	}
  }

  function execute()
  {
	/* Save data */
	$groupfilter= get_global("groupfilter");
	foreach( array("depselect", "guser", "regex") as $type){
		if (isset($_POST[$type])){
			$groupfilter[$type]= $_POST[$type];
		} 
	}
	if (isset($_POST['depselect'])){
		foreach( array("primarygroups", "sambagroups", "mailgroups", "appgroups",
				"functionalgroups", "subsearch") as $type){

			if (isset($_POST[$type])) {
				$groupfilter[$type]= "checked";
			} else {
				$groupfilter[$type]= "";
			}
		}
	}
        if (isset($_GET['search'])){
		$s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
                if ($s == "**"){
                        $s= "*";
                }
                $groupfilter['regex']= $s;
        }
	register_global("groupfilter", $groupfilter);

	$smarty= get_smarty();

	/* Prepare formular */
	if (!isset($this->grouptab) &&
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
	if (isset($_POST['new_group'])){

		/* By default we set 'dn' to 'new', all relevant plugins will
		   react on this. */
		$this->dn= "new";

		/* Create new usertab object */
		$this->grouptab= new grouptabs($this->config,
				$this->config->data['TABS']['GROUPTABS'], $this->dn);
		$this->grouptab->set_acl(array(':all'));
	}

	/* Cancel dialogs */
	if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel'])){
		del_lock ($this->grouptab->dn);
		unset ($this->grouptab);
		$this->grouptab= NULL;
		unset($_SESSION['objectinfo']);
	}

	/* Finish group edit is triggered by the tabulator dialog, so
	   the user wants to save edited data. Check and save at this
	   point. */
	if (isset($_POST['edit_finish'])){

		/* Check tabs, will feed message array */
		$message= $this->grouptab->check();

		/* Save, or display error message? */
		if (count($message) == 0){

			/* Save user data to ldap */
			$this->grouptab->save();
			gosa_log ("Group object'".$this->dn."' has been saved");

			/* Group has been saved successfully, remove lock from
			   LDAP. */
			if ($this->dn != "new"){
				del_lock ($this->dn);
			}

			/* There's no page reload so we have to read new users at
			   this point. */
			$this->reload ();
			unset ($this->grouptab);
			$this->grouptab= NULL;
			unset ($_SESSION['objectinfo']);

		} else {
			/* Ok. There seem to be errors regarding to the tab data,
			   show message and continue as usual. */
			show_errors($message);
		}
	}

	/* User wants to edit data? */
	if ((isset($_POST['select_group']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['grouplist']) && $_POST['grouplist'] != ""){

		/* Get 'dn' from posted 'uid', must be unique */
		$this->dn= trim($_POST['grouplist']);

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
		$this->grouptab= new grouptabs($this->config,
				$this->config->data['TABS']['GROUPTABS'], $this->dn);
		$this->grouptab->set_acl($acl);
		$_SESSION['objectinfo']= $this->dn;
	}

	/* Remove user was requested */
	if (isset($_POST['delete_group']) && isset($_POST['grouplist'])){

		/* Get 'dn' from posted 'uid' */
		$this->dn= trim($_POST['grouplist']);

		/* Load permissions for selected 'dn' and check if
		   we're allowed to remove this 'dn' */
		$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
		$this->acl= get_module_permission($acl, "group", $this->dn);
		if (chkacl($this->acl, "delete") == ""){

			/* Check locking, save current plugin in 'back_plugin', so
			   the dialog knows where to return. */
			if (($user= get_lock($this->dn)) != ""){
				return(gen_locked_message ($user, $this->dn));
			}

			/* Lock the current entry, so nobody will edit it during deletion */
			add_lock ($this->dn, $this->ui->dn);
			$smarty->assign("info", sprintf(_("You're about to delete the group '%s'."), $this->dn));
			return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
		} else {

			/* Obviously the user isn't allowed to delete. Show message and
			   clean session. */
			print_red (_("You are not allowed to delete this group!"));
		}
	}

	/* Confirmation for deletion has been passed. Group should be deleted. */
	if (isset($_POST['delete_group_confirm'])){

		/* Some nice guy may send this as POST, so we've to check
		   for the permissions again. */
		if (chkacl($this->acl, "delete") == ""){

			/* Delete request is permitted, perform LDAP action */
			$this->grouptab= new grouptabs($this->config,
					$this->config->data['TABS']['GROUPTABS'], $this->dn);
			$this->grouptab->set_acl(array($this->acl));
			$this->grouptab->delete ();
			gosa_log ("Group object'".$this->dn."' has been removed");
			unset ($this->grouptab);
			$this->grouptab= NULL;

			/* Group list has changed, reload it. */
			$this->reload ();
		} else {

			/* Normally this shouldn't be reached, send some extra
			   logs to notify the administrator */
			print_red (_("You are not allowed to delete this group!"));
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

        /* Show tab dialog if object is present */
        if ($this->grouptab){
                $display= $this->grouptab->execute();

                /* Don't show buttons if tab dialog requests this */
                if (!$this->grouptab->by_object[$this->grouptab->current]->dialog){
                        $display.= "<p style=\"text-align:right\">\n";
                        $display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
                        $display.= "&nbsp;\n";
                        $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
                        $display.= "</p>";
                }
                return ($display);
        }

	/* Show main page */
        $smarty->assign("grouplist", $this->grouplist);
	$smarty->assign("search_image", get_template_path('images/search.png'));
	$smarty->assign("tree_image", get_template_path('images/tree.png'));
        $smarty->assign("infoimage", get_template_path('images/info.png'));
        $smarty->assign("launchimage", get_template_path('images/launch.png'));
        $smarty->assign("deplist", $this->config->idepartments);
	foreach( array("depselect", "guser", "regex", "primarygroups", "mailgroups",
			"appgroups", "sambagroups", "functionalgroups", "subsearch") as $type){

	        $smarty->assign("$type", $groupfilter[$type]);
	}

	/* Extend if we are not using javascript */
	$smarty->assign("apply", apply_filter());
	$smarty->assign("alphabet", generate_alphabet());
	$smarty->assign("hint", print_sizelimit_warning());
	
	return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function reload()
  {
	/* Get config */
	$groupfilter= get_global('groupfilter');

	/* Set base for all searches */
	$base= $groupfilter['depselect'];

	/* Regex filter? */
	if ($groupfilter['regex'] != ""){
		$regex= $groupfilter['regex'];
	} else {
		$regex= "*";
	}

	/* User filter? */
	$filter= "";
	$error= "";
	$error2= "";
	$this->grouplist= array();

	/* What are primary groups? */
	$primaries= array();
	$ldap= $this->config->get_ldap_link(TRUE);
	$ldap->cd($base);
	$ldap->search("(&(uid=$regex)(!(uid=*$))(objectClass=posixAccount)(gidNumber=*))", array("gidNumber", "cn"));
	$error= $ldap->error;
	while ($attrs= $ldap->fetch()){
		$primaries[$attrs['gidNumber'][0]]= $attrs['cn'][0];
	}
	if ($groupfilter['primarygroups'] == "checked"){
		$filter.= "(&(objectClass=posixGroup)(|";
		foreach ($primaries as $gidNumber => $cn){
			$filter.= "(gidNumber=$gidNumber)";
		}
		$filter.= "))";
	}

	/* Set filter depending on selection */
	if ($groupfilter['appgroups'] == "checked"){
		$filter.= "(objectClass=gosaApplicationGroup)";
	}
	if ($this->config->current['SAMBAVERSION'] == 3){
		$sfilter= "(objectClass=sambaGroupMapping)";
		if ($groupfilter['sambagroups'] == "checked"){
			$filter.= "(objectClass=sambaGroupMapping)";
		}
	} else {
		$sfilter= "";
	}
	if ($groupfilter['mailgroups'] == "checked"){
		$filter.= "(objectClass=gosaMailAccount)";
	}
	if ($groupfilter['functionalgroups'] == "checked"){
		$ldap->cd($base);
		$ldap->set_size_limit($_SESSION['size_limit']);
		$ldap->search("(&(cn=$regex)(objectClass=posixGroup)(!(|(objectClass=gosaMailAccount)(objectClass=gosaApplicationGroup)$sfilter)))", array("cn", "gidNumber", "description"));
		$error2= $ldap->error;
		while ($attrs= $ldap->fetch()){
			if (!isset($primaries[$attrs['gidNumber'][0]])){
				if (isset($attrs["description"][0])){
					$this->grouplist[$attrs["dn"]]= $attrs["cn"][0]." [".
							$attrs["description"][0]."]";
				} else {
					$this->grouplist[$attrs["dn"]]= $attrs["cn"][0];
				}
			}
		}
	}

	/* Generate grouplist */
	if ($filter != ""){
		$filter= "(&(cn=$regex)(objectClass=posixGroup)(|$filter))";
		if ($groupfilter['guser'] != ""){
			$filter= "(&(|(memberUID=" . $groupfilter['guser'] . ")(cn=" . $groupfilter['guser'] . "))$filter)";
		}
	}
	if ($groupfilter['subsearch'] == "checked"){
		$res= get_list($this->ui->subtreeACL, "$filter", TRUE, $base, array("cn", "description", "gidNumber"), TRUE);
	} else {
		$base= get_groups_ou().$base;
		$res= get_list($this->ui->subtreeACL, "$filter", FALSE, $base, array("cn", "description", "gidNumber"), TRUE);
	}
	if (preg_match("/size limit/i", $error) || preg_match("/size limit/i", $error2)){
		$_SESSION['limit_exceeded']= TRUE;
	}

	foreach ($res as $value){
		/* Is primary group? Change description */
		if (isset($primaries[$value['gidNumber'][0]])){
			$dsc= _("Group of user")." ".$primaries[$value['gidNumber'][0]];
			$this->grouplist[$value["dn"]]= $value["cn"][0]." [".$dsc."]";
			continue;
		}

		if (isset($value["description"][0])){
			$this->grouplist[$value["dn"]]= $value["cn"][0]." [".
					$value["description"][0]."]";
		} else {
			$this->grouplist[$value["dn"]]= $value["cn"][0];
		}
	}
	natcasesort ($this->grouplist);
	reset ($this->grouplist);
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
	if (isset($this->grouptab->dn)){
		del_lock ($this->grouptab->dn);
	}
  }

}

?>
