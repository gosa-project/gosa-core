<?php
   if ($remove_lock) {
     if (isset($_SESSION['dfsManagment'])) {
       $dfsManagment = $_SESSION['dfsManagment'];
       $dfsManagment->remove_lock();
       del_lock($ui->dn);
       sess_del('dfsManagment');
     }
   } else {
     # create dfsManagment object
     if (!isset($_SESSION['dfsManagment']) || 
        (isset($_GET['reset']) && $_GET['reset'] == 1)) {
       $_SESSION['dfsManagment'] = new dfsManagment($config, $_SESSION['ui']);
     }
     $dfsManagment = $_SESSION['dfsManagment'];
     $output = $dfsManagment->execute();

     # do we have to reset
     if (isset($_GET['reset']) && $_GET['reset'] == 1) {
       del_lock ($ui->dn);
       sess_del('dfsManagment');
     }
 
     # create page header
     if (session::is_set('objectinfo')) {
       $display = print_header(get_template_path('images/dfs.png'), 
                                _("Distributed File System Administration"),
                                "<img alt=\"\" class=\"center\" src=\"".
                                get_template_path('images/closedlock.png').
                                "\"> ".$_SESSION['objectinfo']);
     } else {
       $display = print_header(get_template_path('images/dfs.png'),
                                _("Distributed File System Administration"));
     }

     $display.= $output;
    
     # show page
     session::set('dfsManagment',$dfsManagment);
  }

?>
