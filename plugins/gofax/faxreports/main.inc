<?php

if (!$remove_lock){
	/* Create faxreport object on demand */
	if (!session::is_set('faxreport') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		session::set('faxreport',new faxreport ($config, $ui));
	}
	$faxreport = session::get('faxreport');

	/* Execute formular */
	$faxreport->save_object();
	$display= $faxreport->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";
	$display= print_header(get_template_path('images/reports.png'), _("FAX reports")).$display;

	/* Store changes  in session */
	session::set('faxreport',$faxreport);
}
?>
