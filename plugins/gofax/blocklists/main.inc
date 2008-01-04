<?php
if ($remove_lock){
        if(session::is_set('blocklist')){
                $blocklist = session::get('blocklist');
                del_lock ($ui->dn);
                sess_del ('blocklist');
        }
} else {
	/* Create blocklist object on demand */
	if (!session::is_set('blocklist') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['blocklist']= new blocklist($config, $ui);
	 	$_SESSION['blocklist']->set_acl_category("gofaxlist");
	}
	$blocklist = session::get('blocklist');
	$blocklist->save_object();
	$output= $blocklist->execute();

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		sess_del ('blocklist');
	}

	/* Page header*/
	if (session::is_set('objectinfo')){
	        $display= print_header(get_template_path('images/blocklists.png'), _("Blocklist management"), "<img alt=\"\"align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix($_SESSION['objectinfo']));
	} else {
	        $display= print_header(get_template_path('images/blocklists.png'), _("Blocklist management"));
	}
	$display.= $output;

	session::set('blocklist',$blocklist);
}
?>
