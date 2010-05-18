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


/* Remove locks created by this plugin
*/
if ($remove_lock){
    if(session::is_set('propertyEditor')){
        $config->configRegistry->reload($force=TRUE);
    }
}


/* Remove this plugin from session
 */
if ( $cleanup ){
    if (session::is_set('propertyEditor')){
    }
    session::un_set('propertyEditor');
}else{


    /* Create logview object on demand */
    if (!session::is_set('propertyEditor')){
        session::set('propertyEditor',new propertyEditor($config, get_userinfo()));
    }
    $propertyEditor = session::get('propertyEditor');

    /* Execute formular */
    $display= $propertyEditor->execute ();

    /* Store changes  in session */
    session::set('propertyEditor',$propertyEditor);
}
?>
