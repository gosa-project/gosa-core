<?php

if (!$remove_lock){
	/* Create fonreport object on demand */
	if (!isset($_SESSION['fonreport']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['fonreport']= new fonreport ($config, $ui);
	}
	$fonreport= $_SESSION['fonreport'];

	/* set permissions */
	$acl= get_permissions ($ui->dn, $ui->subtreeACL);
	$fonreport->acl= get_module_permission($acl, "fonreport", $ui->dn);


	/* Execute formular */
	$fonreport->save_object();
	$display= $fonreport->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Page header*/
	$display= print_header(get_template_path('images/phonereport.png'), _("Phone reports")).$display;

	/* Store changes  in session */
	$_SESSION['fonreport']= $fonreport;
}
?>
