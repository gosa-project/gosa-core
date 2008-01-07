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
        if(session::is_set('applicationManagement')){
                $applicationManagement = session::get('applicationManagement');
                $applicationManagement->remove_lock();
                del_lock ($ui->dn);
                session::un_set ('applicationManagement');
        }
} else {
	/* Create applicationManagement object on demand */
	if (!session::is_set('applicationManagement') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		session::set('applicationManagement',new applicationManagement ($config, $ui));
	}
	$applicationManagement = session::get('applicationManagement');
	$applicationManagement->save_object();
	$output= $applicationManagement->execute();

	/* Page header*/
	if (session::is_set('objectinfo')){
		$display= print_header(get_template_path('images/application.png'), _("Application management"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix(session::get('objectinfo')));
	} else {
		$display= print_header(get_template_path('images/application.png'), _("Application management"));
	}

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		session::un_set ('applicationManagement');
	}

	/* Show and save dialog */
	$display.= $output;
	session::set('applicationManagement',$applicationManagement);
}

?>
