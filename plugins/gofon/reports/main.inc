<?php

if (!$remove_lock){
	/* Page header*/
        $display= print_header(get_template_path('images/phonereport.png'), _("Phone reports"));

        /* Create fonreport object on demand */
        if (!isset($_SESSION['fonreport']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
                $_SESSION['fonreport']= new fonreport ($config, $ui);
        }
        $fonreport= $_SESSION['fonreport'];

        /* Execute formular */
	$display.= $fonreport->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Store changes  in session */
	$_SESSION['fonreport']= $fonreport;
}
?>
