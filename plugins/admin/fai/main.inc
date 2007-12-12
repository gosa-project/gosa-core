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
        if(isset($_SESSION['FAI'])){
                $FAI= $_SESSION['FAI'];
                $FAI->remove_lock();
                del_lock ($ui->dn);
                sess_del ('FAI');
        }
} else {
	/* Create usermanagement object on demand */
	if (!isset($_SESSION['FAI']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['FAI']= new faiManagement($config, $ui);
		$_SESSION['FAI']->set_acl_category("fai");
		$_SESSION['FAI']->set_acl_base($_SESSION['CurrentMainBase']);
	}
	$FAI= $_SESSION['FAI'];
	$FAI->save_object();
	$output= $FAI->execute();

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		sess_del ('FAI');
	}
	/* Page header*/
	if (isset($_SESSION['objectinfo'])){
		$display= print_header(get_template_path('images/fai.png'), _("Fully Automatic Installation"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix($_SESSION['objectinfo']));
	} else {
		$display= print_header(get_template_path('images/fai.png'), _("Fully Automatic Installation"));
	}
	$display.= $output;

	$_SESSION['FAI']= $FAI;
}

?>
