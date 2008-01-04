<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2005  Cajus Pollmeier

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
        /* Create msgplug object on demand */
        if (!session::is_set('msgplug') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
                $ui = get_userinfo();
                $_SESSION['msgplug']= new msgplug ($config);
                $_SESSION['msgplug']->set_acl_category("msgplug");

                /* Check root dn and user dn for acl informations */
                $_SESSION['msgplug']->set_acl_base($config->current['BASE']);
                if($_SESSION['msgplug']->getacl("") == ""){
                        $_SESSION['msgplug']->set_acl_base($ui->dn);
                }
        }
	$msgplug= $_SESSION['msgplug'];

	/* save changes back to object */
	if (isset ($_POST['target'])){
	  $msgplug->save_object ();
	}

	/* Execute formular */
	$display= $msgplug->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Page header*/
	$display= print_header(get_template_path('images/notifications.png'), _("Notifications")).$display;

	/* Store changes  in session */
	session::set('msgplug',$msgplug);
}
?>
