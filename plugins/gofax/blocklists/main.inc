<?php
if ($remove_lock){
        if(isset($_SESSION['blocklist'])){
                $blocklist= $_SESSION['blocklist'];
                $blocklist->remove_lock();
                del_lock ($ui->dn);
                sess_del ('blocklist');
        }
} else {
	/* Create blocklist object on demand */
	if (!isset($_SESSION['blocklist']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['blocklist']= new blocklist($config, $ui);
	}
	$blocklist= $_SESSION['blocklist'];
	$blocklist->save_object();
	$output= $blocklist->execute();

	/* Page header*/
	if (isset($_SESSION['objectinfo'])){
	        $display= print_header(get_template_path('images/blocklists.png'), _("Blocklist management"), "<img align=\"center\" src=\"".get_template_path('images/closedlock.png')."\"> ".$_SESSION['objectinfo']);
	} else {
	        $display= print_header(get_template_path('images/blocklists.png'), _("Blocklist management"));
	}

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		sess_del ('blocklist');
	}

	/* Show and save dialog */
	$display.= $output;
	$_SESSION['blocklist']= $blocklist;
}
?>
