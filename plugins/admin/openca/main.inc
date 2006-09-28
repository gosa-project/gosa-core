<?php
   #################################################################### 
 #                                                                      #
#  CA Management v0.1                                                    #
#  ssl certificates management tool  for GOsa framework                  #
#  Copyright (c) 2006 by Guido Serra (zeph@purotesto.it)                 #
#                                                                        #
#  Lastest version?                                                      # 
#  https://dev.purotesto.it/support/gosa                                 #
#                                                                        #
#  This program is free software. You can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation; either version 2 of the License.        #
#                                                                        #
 #                                                                      #
   ####################################################################

if (!$remove_lock){
	/* Create logview object on demand */
	if (!isset($_SESSION['openca']) || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		$_SESSION['openca']= new openca ($config);
	}
	$openca= $_SESSION['openca'];

	/* Execute formular */
	$display= $openca->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Page header*/
	$display= print_header(get_template_path('images/default.png'), _("Certificates")).$display;

	/* Store changes  in session */
	$_SESSION['openca']= $openca;
}
?>
