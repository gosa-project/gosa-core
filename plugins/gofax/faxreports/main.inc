<?php

/* Remove classes and variables from session
 */
if ( $cleanup ){
  session::un_set('faxreport');
}else{

	/* Create faxreport object on demand */
	if (!session::is_set('faxreport')){
		session::set('faxreport',new faxreport ($config, $ui));
	}
	$faxreport = session::get('faxreport');

	/* Execute formular */
	$faxreport->save_object();
	$display= $faxreport->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Store changes  in session */
	session::set('faxreport',$faxreport);
}
?>
