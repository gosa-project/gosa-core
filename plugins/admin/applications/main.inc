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

if ($remove_lock){
        if(isset($_SESSION['applicationManagement'])){
                $applicationManagement= $_SESSION['applicationManagement'];
                $applicationManagement->remove_lock();
                del_lock ($ui->dn);
                sess_del ('applicationManagement');
        }
} else {
	/* Create applicationManagement object on demand */
	if (!isset($_SESSION['applicationManagement']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['applicationManagement']= new applicationManagement ($config, $_SESSION['ui']);
	}
	$applicationManagement= $_SESSION['applicationManagement'];
	$applicationManagement->save_object();
	$output= $applicationManagement->execute();

	/* Page header*/
	if (isset($_SESSION['objectinfo'])){
		$display= print_header(get_template_path('images/application.png'), _("Application management"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix($_SESSION['objectinfo']));
	} else {
		$display= print_header(get_template_path('images/application.png'), _("Application management"));
	}

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		sess_del ('applicationManagement');
	}

	/* Show and save dialog */
	$display.= $output;
	$_SESSION['applicationManagement']= $applicationManagement;
}

?>
