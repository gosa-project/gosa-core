<?php
if ($remove_lock){
    if(session::is_set('newConfigManagement')){
        $macl = session::get('newConfigManagement');
        $macl->remove_lock();
    }
}

/* Remove this plugin from session
 */
if ( $cleanup ){
    session::un_set('newConfigManagement');
}else{

    /* Create newConfigManagement object on demand */
    if (!session::is_set('newConfigManagement')){
        $newConfigManagement= new newConfigManagement ($config, $ui);
        session::set('newConfigManagement',$newConfigManagement);
    }
    $newConfigManagement = session::get('newConfigManagement');
    $display= $newConfigManagement->execute();

    /* Reset requested? */
    if (isset($_GET['reset']) && $_GET['reset'] == 1){
        session::un_set ('newConfigManagement');
    }

    /* Show and save dialog */
    session::set('newConfigManagement',$newConfigManagement);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
