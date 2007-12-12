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
		if(isset($_SESSION['DeviceManagement'])){
				$DeviceManagement= $_SESSION['DeviceManagement'];
				$DeviceManagement->remove_lock();
				del_lock ($ui->dn);
				sess_del ('DeviceManagement');
		}
} else {

		/* Create DeviceManagement object on demand */
		if (!isset($_SESSION['DeviceManagement']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
				$_SESSION['DeviceManagement']= new deviceManagement ($config);
		}

		/* Get object */
		$DeviceManagement= $_SESSION['DeviceManagement'];
		$DeviceManagement->save_object();
		$output= $DeviceManagement->execute();

		/* Page header*/
		if (isset($_SESSION['objectinfo'])){
				$display= print_header(get_template_path('images/devices.png'), _("Device management"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix($_SESSION['objectinfo']));
		} else {
				$display= print_header(get_template_path('images/devices.png'), _("Device management"));
		}

		/* Reset requested? */
		if (isset($_GET['reset']) && $_GET['reset'] == 1){
				del_lock ($ui->dn);
				sess_del ('DeviceManagement');
		}

		/* Show and save dialog */
		$display.= $output;
		$_SESSION['DeviceManagement']= $DeviceManagement;
}
?>
