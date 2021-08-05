<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: main.inc 21054 2011-11-09 07:53:05Z hickert $$
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

/* Remove locks created by this plugin
 */
if ($remove_lock){
    if(session::is_set('systemManagement')){
        $macl = session::get('systemManagement');
        $macl->remove_lock();
    }
}

/* Remove this plugin from session
 */
if ( $cleanup ){
    session::un_set('systemManagement');
}else{

    /* Create systemManagement object on demand */
    if (!session::is_set('systemManagement')){
        $systemManagement= new systemManagement ($config, $ui);
        session::set('systemManagement',$systemManagement);
    }
    $systemManagement = session::get('systemManagement');
    $display= $systemManagement->execute();

    /* Show and save dialog */
    session::set('systemManagement',$systemManagement);
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
