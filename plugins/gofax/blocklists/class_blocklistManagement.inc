<?php
class blocklist extends plugin
{
  /* Definitions */
  var $plHeadline= "FAX Blocklists";
  var $plDescription= "This does something";

  /* CLI vars */
  var $cli_summary= "Handling of GOfax block lists";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* blocklist attributes */
  var $cn= "";
  var $description= "";
  var $type= "";
  var $goFaxBlocklist= array();
  var $base= "";

  /* Headpage attributes */
  var $blocklists= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description");
  var $objectclasses= array();

  function blocklist ($config, $ui)
  {
	$this->ui= $ui;
	$this->dn= "";
	$this->config= $config;

        /* Get global filter config */
        if (!is_global("blockfilter")){
                $ui= get_userinfo();
                $base= get_base_from_people($ui->dn);
                $blockfilter= array("sendlists" => "checked",
                                "receivelists" => "checked",
                                "entry" => "*",
                                "depselect" => $base,
                                "regex" => "*");
                register_global("blockfilter", $blockfilter);
        }

  }

  function execute()
  {
	/* Get global smarty instance */
	$smarty= get_smarty();

        /* Save data */
        $blockfilter= get_global("blockfilter");
        foreach( array("depselect", "entry", "regex") as $type){
                if (isset($_POST[$type])){
                        $blockfilter[$type]= $_POST[$type];
                }
        }
        if (isset($_POST['depselect'])){
                foreach( array("sendlists", "receivelists") as $type){

                        if (isset($_POST[$type])) {
                                $blockfilter[$type]= "checked";
                        } else {
                                $blockfilter[$type]= "";
                        }
                }
        }
        if (isset($_GET['search'])){
                $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
                if ($s == "**"){
                        $s= "*";
                }
                $blockfilter['regex']= $s;
        }
        register_global("blockfilter", $blockfilter);

	/* Create new blocklist? */
	if (isset($_POST['new_blocklist'])){
		$this->clear_fields();
		$this->acl= "#all#";
		$this->dn= "new";
		plugin::plugin ($this->config, $this->dn);
	}

	/* Edit selected blocklist? */
	if ((isset($_POST['select_blocklist']) || (isset($_POST['edit_helper']) && $_POST['edit_helper'] == "1")) && isset($_POST['blocklist']) != ""){
		$this->clear_fields();
		$this->dn= trim($_POST['blocklist']);
		$this->acl= get_permissions ($this->dn, $this->ui->subtreeACL);
		$this->acl= get_module_permission($this->acl, "blocklists", $this->dn);
		plugin::plugin ($this->config, $this->dn);

		/* Load missing lists */
		if (isset($this->attrs["goFaxSBlocklist"])){
			for ($i= 0; $i<$this->attrs["goFaxSBlocklist"]["count"]; $i++){
				$this->goFaxBlocklist[]= $this->attrs["goFaxSBlocklist"][$i];
			}
			$this->type= 0;
		} elseif (isset($this->attrs["goFaxRBlocklist"])){
			for ($i= 0; $i<$this->attrs["goFaxRBlocklist"]["count"]; $i++){
				$this->goFaxBlocklist[]= $this->attrs["goFaxRBlocklist"][$i];
			}
			$this->type= 1;
		}
		$_SESSION['objectinfo']= trim($this->dn);
	}

	/* Delete blocklist requested */
	if (isset($_POST['delete_blocklist']) && isset($_POST['blocklist'])){
		$this->dn= trim($_POST['blocklist']);

		/* Check locking */
		if (($user= get_lock($this->dn)) != ""){
			$_SESSION['dn']= $this->dn;
			return(gen_locked_message($user, $this->dn));
		} else {
			# Lock this dn for editing
			add_lock ($this->dn, $this->ui->dn);

			$smarty->assign("info", sprintf(_("You're about to delete the blocklist '%s'."), $this->dn));
			return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
		}
	}

	/* Finally delete blocklist */
	if (isset($_POST['delete_blocklist_confirm'])){
		$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
		$acl= get_module_permission($acl, "blocklists", $this->dn);
		if (chkacl($acl, "all") == ""){
			$this->remove_from_parent();
			gosa_log ("Blocklist object'".$this->dn."' has been removed");
			$this->reload ();
		} else {
			print_red (_("You have no permission to remove this blocklist."));
		}
	}

	/* Cancel dialog */
	if (isset($_POST['edit_cancel']) || isset($_POST['delete_blocklist_cancel']) ||
		isset($_POST['delete_blocklist_confirm']) || isset($_POST['delete_lock'])){

		del_lock ($this->dn);
		$this->clear_fields();
		unset($_SESSION['objectinfo']);
	}

	/* Handle interactions: add */
	if (isset($_POST['add_number']) && $_POST['number'] != ""){
		if (is_phone_nr($_POST['number'])){
			$this->addNumber ($_POST['number']);
		} else {
			print_red (_("Please specify a valid phone number."));
		}
	}

	/* Handle interactions: delete */
	if (isset($_POST['delete_number']) && count($_POST['numbers']) > 0){
		$this->delNumber ($_POST['numbers']);
	}

	/* What about finish? */
	if (isset($_POST['edit_finish'])){
		$message= $this->check();

		/* No errors, save object */
		if (count ($message) == 0){

			/* Perpare 'dn' in case of new entries */
			if ($this->dn == "new"){
				$this->dn= "cn=$this->cn,ou=gofax,ou=systems,".$this->base;
			}

			$this->save ();
			gosa_log ("Blocklist object'".$this->dn."' has been saved");
			$this->acl= "#none#";
			$this->dn= "";
			del_lock ($this->ui->dn);
			unset($_SESSION['objectinfo']);
		} else {
			/* Errors found, show message */
			show_errors ($message);
		}
	}
	
	/* Headpage or normal plugin screen? */
	if ($this->dn == ""){
		/* Check sorting variable */
		$this->reload();

	        /* Check for exeeded sizelimit */
	        if (($message= check_sizelimit()) != ""){
	                return($message);
	        }

		/* Show main page */
	        $smarty->assign("blocklists", $this->blocklists);
		$smarty->assign("search_image", get_template_path('images/search.png'));
		$smarty->assign("tree_image", get_template_path('images/tree.png'));
	        $smarty->assign("infoimage", get_template_path('images/info.png'));
		$smarty->assign("launchimage", get_template_path('images/launch.png'));
		$smarty->assign("apply", apply_filter());
		foreach( array("depselect", "entry", "regex", "sendlists", "receivelists") as $type){
                	$smarty->assign("$type", $blockfilter[$type]);
	        }
	        $smarty->assign("deplist", $this->config->idepartments);
	        $smarty->assign("alphabet", generate_alphabet());
		$smarty->assign("hint", print_sizelimit_warning());

		return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
	}

	/* Set base */
	if ($this->base == ""){
		if ($this->dn == "new"){
			$ui= get_userinfo();
			$this->base= dn2base($ui->dn);
		} else {
			$this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,/", "", $this->dn);
		}
	}

	/* Fill templating stuff */
	$smarty->assign("bases", $this->config->idepartments);
	$smarty->assign("base_select", $this->base);
	$smarty->assign("types", array(0 => _("send"), 1 => _("receive")));
        if ($this->dn == "new" || preg_match ("/,ou=incoming,/", $this->dn)){
                $smarty->assign("selectmode", "");
                $smarty->assign("mode", "");
        } else {
                $smarty->assign("selectmode", "disabled");
                $smarty->assign("mode", "readonly");
        }
	foreach(array("cn", "description", "type", "goFaxBlocklist") as $val){
		$smarty->assign("$val", $this->$val);
		$smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
	}

	/* Show main page */
	return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function clear_fields()
  {
	$this->dn= "";
	$this->description= "";
	$this->cn= "";
	$this->base= "";
	$this->goFaxBlocklist= array();
	$this->acl= "#none#";
  }

  function reload()
  {
	/* Get config */
        $blockfilter= get_global('blockfilter');

        /* Set base for all searches */
        $base= $blockfilter['depselect'];
	$filter= "";

        /* Regex filter? */
        if ($blockfilter['regex'] != ""){
                $regex= $blockfilter['regex'];
        } else {
                $regex= "*";
        }

        /* Entry filter? */
        /* Get list of blocklists to be shown */
        if ($blockfilter['sendlists'] == "checked"){
		$filter.= "(objectClass=goFaxSBlock)";
        }
        if ($blockfilter['receivelists'] == "checked"){
		$filter.= "(objectClass=goFaxRBlock)";
        }
	if ($filter != ""){
		$filter= "(|$filter)";
	}

        if ($blockfilter['entry'] != "" && $blockfilter['entry'] != "*"){
                $filter.= "(|(goFaxSBlocklist=".$blockfilter['entry'].")(goFaxRBlocklist=".$blockfilter['entry']."))";
        }

	/* Generate blocklists */
	$res= get_list($this->ui->subtreeACL, "(&(cn=$regex)$filter)", TRUE, $base, array("cn", "description"));

	$this->blocklists= array();
	foreach ($res as $value){

		/* Unwanted? */
		$this->blocklists[$value["dn"]]= $value["cn"][0];
		if (isset($value["description"][0])){
			$this->blocklists[$value["dn"]]= $value["cn"][0]." (".
						$value["description"][0].")";
		}
	}
	natcasesort ($this->blocklists);
	reset ($this->blocklists);
  }

  function remove_from_parent()
  {
	$ldap= $this->config->get_ldap_link();
	$ldap->rmDir($this->dn);
	$this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
	plugin::save_object();
	
	/* Save type, needed to detect objectClass */
	if (isset($_POST['type']) && chkacl($this->acl, "cn") == ""){
		$this->type= $_POST['type'];
	}

        /* Save base, since this is no LDAP attribute */
        if (isset($_POST['base']) && chkacl($this->acl, "create") == ""){
                $this->base= $_POST['base'];
        }
  }


  /* Check values */
  function check()
  {
	$message= array();

	/* Permissions for that base? */
	if ($this->base != ""){
		$new_dn= "cn=".$this->cn.",ou=gofax,ou=systems,".$this->base;
	} else {
		$new_dn= $this->dn;
	}

	$ui= get_userinfo();
	$acl= get_permissions ($new_dn, $ui->subtreeACL);
	$acl= get_module_permission($acl, "blocklist", $new_dn);
	if (chkacl($acl, "create") != ""){
		$message[]= _("You have no permissions to create a blocklist on this 'Base'.");
	}


	/* check syntax: must cn */
	if ($this->cn == ""){
		$message[]= _("Required field 'Name' is not set.");
	} else {
		if (!is_uid($this->cn)){
			$message[]= _("Required field 'Name' contains invalid characters");
		}
		if ($this->dn == 'new'){
			$ldap= $this->config->get_ldap_link();
			$ldap->cd ("ou=gofax,ou=systems,".$this->config->current["BASE"]);
			$ldap->search ("(&(|(objectClass=goFaxSBlock)(objectClass=goFaxRBlock))".
					"(cn=".$this->cn."))", array("cn"));
			if ($ldap->count() != 0){
				$message[]= _("Specified name is already used.");
			}
		}
	}

	/* Is user allowed to create in 'base'? */
	if (chkacl($this->acl, "create") != ""){
		$message[]= _("No permission to create a blocklist on this base.");
	}

	return $message;
  }


  /* Save to LDAP */
  function save()
  {
	plugin::save();

	/* Type selection */
	if ($this->type == 0){
		$type= "goFaxSBlock";
	} else {
		$type= "goFaxRBlock";
	}

	/* Add list */
	$this->attrs['objectClass']= $type;
	if (count($this->goFaxBlocklist)){
		$this->attrs["$type"."list"]= $this->goFaxBlocklist;
	}

	/* Write back to ldap */
	$ldap= $this->config->get_ldap_link();
	$ldap->cd($ldap->getParentDir($this->dn));
	$ldap->search("cn=$this->cn");
	$a= $ldap->fetch();
	if (count($a)){
		if (!isset($this->attrs["$type"."list"])){
			$this->attrs["$type"."list"]= array();
		}
		$ldap->cd($this->dn);
		$ldap->modify($this->attrs);
		$this->handle_post_events("modify");
	} else {
		$ldap->cd($this->config->current['BASE']);
		$ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
		$ldap->cd($this->dn);
		$ldap->add($this->attrs);
		$this->handle_post_events("add");
	}
	show_ldap_error($ldap->get_error());
  }


  function addNumber($number)
  {
	if (!in_array($number, $this->goFaxBlocklist)){
		$this->goFaxBlocklist[]= $number;
		sort($this->goFaxBlocklist);
	}
  }

  function delNumber($numbers)
  {
	$tmp= array();
	foreach ($this->goFaxBlocklist as $val){
		if (!in_array($val, $numbers)){
			$tmp[]= $val;
		}
	}
	$this->goFaxBlocklist= $tmp;
  }

  function remove_lock()
  {
	if (isset($this->dn)){
	        del_lock ($this->dn);
	}
  }

}

?>
