<?php
/*
  This code is an addon for GOsa (https://gosa.gonicus.de)
  Copyright (C) 2015 Mike Gabriel

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/


/* Remove locks created by this plugin
*/
if ($remove_lock){
  if(session::is_set('pwreset')){
    // Nothing to unlock here
  }
}

/* Remove this plugin from session
*/
if ( $cleanup ){
  session::un_set('pwreset');
}else{

	/* Create phonelist object on demand */
	if (!session::is_set('pwreset')){
		$pwreset= new pwreset ($config);
		$pwreset->set_acl_category("pwreset");
		session::set('pwreset',$pwreset);
	}
	$pwreset = session::get('pwreset');

	/* Execute formular */
	$display= $pwreset->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Store changes in session */
	session::set('pwreset',$pwreset);
}
?>
