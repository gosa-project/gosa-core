<?php
/*
 * Created on 24-ott-2006 by "Guido Serra aka Zeph" <zeph@purotesto.it>
 * 
 * PuroTesto (freesoftware developers, Milano, IT)
 * http://purotesto.it
 * 
 * This file is part of OpenCA module for GOsa^2.
 *
 * The OpenCA module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * OpenCA module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

if ($remove_lock){
        if(isset($_SESSION['openca'])){
                $openca= $_SESSION['openca'];
                $openca->remove_lock();
                del_lock ($ui->dn);
                sess_del ('openca');
        }
} else {
	/* Create logview object on demand */
	if (!isset($_SESSION['openca']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['openca']= new openca ($config);
	}
	$openca= $_SESSION['openca'];
	$openca->save_object();
	$output= $openca->execute();

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		sess_del ('openca');
	}

	/* Page header*/
	if (isset($_SESSION['objectinfo'])){
		$display= print_header(get_template_path('images/default.png'), _("Certificates"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix($_SESSION['objectinfo']));
	} else {
		$display= print_header(get_template_path('images/default.png'), _("Certificates"));
	}

	/* Show and save dialog */
	$display.= $output;
	$_SESSION['openca']= $openca;
}
?>
