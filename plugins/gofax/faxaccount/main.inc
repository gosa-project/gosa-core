<?php
if (!$remove_lock){

	/* Reset requested? */
	if (isset($_POST['edit_cancel']) ||
		(isset($_GET['reset']) && $_GET['reset'] == 1)){

		del_lock ($ui->dn);
		sess_del ('edit');
		sess_del ('gofaxAccount');
	}

	/* Create gofaxAccount object on demand */
	if (!isset($_SESSION['gofaxAccount']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['gofaxAccount']= new gofaxAccount ($config, $ui->dn);
	}
	$gofaxAccount= $_SESSION['gofaxAccount'];

	/* save changes back to object */
	if (isset ($_SESSION['edit'])){
		$gofaxAccount->save_object ();
	}

	/* Enter edit mode? */
	if (isset($_POST['edit'])){

		/* Check locking */
		if (($username= get_lock($ui->dn)) != ""){
			$_SESSION['back_plugin']= $plug;
			gen_locked_message ($username, $ui->dn);
			exit ();
		}

		/* Lock the current entry */
		add_lock ($ui->dn, $ui->dn);
		$_SESSION['dn']= $ui->dn;
		$_SESSION['edit']= TRUE;
	}

	/* Adjust acl's to mode */
	if (isset($_SESSION['edit'])){
		$acl= get_permissions ($ui->dn, $ui->subtreeACL);
		$gofaxAccount->acl= get_module_permission($acl, "gofax", $ui->dn);
	} else {
		$acl= get_permissions ($ui->dn, $ui->subtreeACL);
		$editacl= get_module_permission($acl, "gofax", $ui->dn);
		$gofaxAccount->acl= "#none#";
	}

	/* save changes to LDAP and disable edit mode */
	if (isset($_POST['edit_finish'])){

                /* Perform checks */
                $message= $gofaxAccount->check ();

                /* No errors, save object */
                if (count ($message) == 0){
                        $gofaxAccount->save ();
			gosa_log ("User/fax object'".$ui->dn."' has been saved");
                        $gofaxAccount->acl= "#none#";
                        del_lock ($ui->dn);
                        sess_del ('edit');

                } else {
                        /* Errors found, show message */
                        show_errors ($message);
                }
        }

	/* Execute formular */
	$display= $gofaxAccount->execute ();
	$info= "";

	/* Store changes  in session */
	if (isset ($_SESSION['edit'])){
		$_SESSION['gofaxAccount']= $gofaxAccount;
	}

	/* Show page footer depending on the mode */
	if (!$gofaxAccount->locals_dialog &&
		!$gofaxAccount->out_blocklist_dialog &&
		!$gofaxAccount->in_blocklist_dialog &&
		$gofaxAccount->is_account){

		$display.= "<p class=\"plugbottom\">\n";

		/* Are we in edit mode? */
		if (isset($_SESSION['edit'])){
			$display.= "<input type=submit name=\"edit_finish\" value=\""._("Finish")."\">\n";
			$display.= "&nbsp;\n";
			$display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
			$info= "<img align=\"center\" src=\"".get_template_path('images/closedlock.png')."\"> ".$ui->dn."&nbsp;";
		} else {
			$info= "<img align=\"center\" src=\"".get_template_path('images/openlock.png')."\"> ".$ui->dn."&nbsp;";
			if (isset($editacl) && $editacl != "#none#"){
				$info.= "<img align=\"center\" src=\"".get_template_path('images/lamp.png')."\">"._("Click the 'Edit' button below to change informations in this dialog");
				$display.= "<input type=submit name=\"edit\" value=\""._("Edit")."\">";
			}
			$display.= "<input type=\"hidden\" name=\"ignore\">\n";
		}
		$display.= "</p>";
	}
	
	/* Page header*/
	$display= print_header(get_template_path('images/fax.png'), _("FAX settings"), $info).$display;

}
?>
