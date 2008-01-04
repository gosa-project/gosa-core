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

if (!$remove_lock){

	/* Create gotomasses object on demand */
	if (!session::is_set('gotomasses') || (isset($_GET['reset']) && $_GET['reset'] == 1) || isset($_POST['reload_gotomass_data'])){
		$gotomasses= new gotomasses ($config);
		$gotomasses->set_acl_category("gotomasses");

        /* Check root dn and user dn for acl informations */
        $gotomasses->set_acl_base($config->current['BASE']);
        if($gotomasses->getacl("") == ""){
            $gotomasses->set_acl_base($ui->dn);
        }
		
		/* Check if we have acl on our own base */
        if($gotomasses->getacl("") == ""){
            $gotomasses->set_acl_base(dn2base($ui->dn));
        }
		session::set("gotomasses",$gotomasses);
	}
	$gotomasses = session::get('gotomasses');

	/* Execute formular */
	$display= $gotomasses->save_object();
	if(isset($_POST['save_gotomass_changes'])){
		$gotomasses->save();
	}
	$display= $gotomasses->execute ();

	/* Page header*/
	$display= print_header(get_template_path('images/system.png'), _("System deployment")).$display;

	/* Store changes  in session */
	session::set('gotomasses',$gotomasses);
}
