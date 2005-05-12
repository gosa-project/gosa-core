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
	/* Page header*/
	$display= print_header(get_template_path('images/logview.png'), _("System log view"));

	/* Create logview object on demand */
	if (!isset($_SESSION['logview']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['logview']= new logview ($config);
	}
	$logview= $_SESSION['logview'];

	/* Execute formular */
	$display.= $logview->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Store changes  in session */
	$_SESSION['logview']= $logview;
}
?>
