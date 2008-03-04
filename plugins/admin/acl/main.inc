<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

if ($remove_lock){
        if(session::is_set('macl')){
                $macl = session::get('macl');
                $macl->remove_lock();
                del_lock ($ui->dn);
                session::un_set ('macl');
        }
} else {
	$display= "No headpage yet for this module";

	/* Create usermanagement object on demand */
if (!session::is_set('macl') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		session::set('macl',new aclManagement($config, $ui));
	}
	$macl = session::get('macl');
	
	/* Show and save dialog */
	$macl->save_object();
	$output= $macl->execute();

	/* Page header*/
	if (session::is_set('objectinfo')){
		$display= print_header(get_template_path('images/acl.png'), _("ACL management"), "<img alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png')."\"> ".@LDAP::fix(session::get('objectinfo')));
	} else {
		$display= print_header(get_template_path('images/acl.png'), _("ACL management"));
	}

	/* Reset requested? */
	if (isset($_GET['reset']) && $_GET['reset'] == 1){
		del_lock ($ui->dn);
		session::un_set ('macl');
	}

	$display.= $output;
	session::set('macl',$macl);
}
?>
