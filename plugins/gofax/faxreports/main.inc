<?php

if (!$remove_lock){
	/* Create faxreport object on demand */
	if (!isset($_SESSION['faxreport']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['faxreport']= new faxreport ($config, $ui);
	}
	$faxreport= $_SESSION['faxreport'];

	/* Execute formular */
	$faxreport->save_object();
	$display= $faxreport->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";
	$display= print_header(get_template_path('images/reports.png'), _("FAX reports")).$display;

	/* Store changes  in session */
	$_SESSION['faxreport']= $faxreport;
}
?>
