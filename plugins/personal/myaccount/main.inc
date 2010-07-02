<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: main.inc 15315 2010-01-26 13:00:52Z hickert $$
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
$lock_msg="";
if ($remove_lock || 
        isset($_POST['edit_cancel']) || 
        isset($_POST['password_cancel'])){
    del_lock($ui->dn);
    if(session::is_set('MyAccountTabs') && session::is_set('edit')){
        session::un_set('MyAccountTabs');
    }
}

/* Reset requested? */
if (isset($_POST['edit_cancel']) || $cleanup || isset($_POST['password_changed'])){ 
    session::un_set ('edit');
    session::un_set ('MyAccountTabs');
}

/* Remove this plugin from session
 */
if (! $cleanup ){

    /* Create MyAccountTabs object on demand */
    if (!session::is_set('MyAccountTabs') || (isset($_GET['reset']) && $_GET['reset'] == 1)){

        // Check if the base plugin is available - it is mostly responsible for object creation and removal.
        $first  = $config->data['TABS']['MYACCOUNTTABS'][0];
        if(!class_available($first['CLASS'])){
            msg_dialog::display(_("Internal error"),
                    sprintf(_("Cannot instantiate tabbed-plugin, the base plugin (%s) is not available!"), $first['CLASS']),
                    ERROR_DIALOG);
        }else{

            $MyAccountTabs= new MyAccountTabs($config,$config->data['TABS']['MYACCOUNTTABS'], $ui->dn, "users", true, true); 
            $MyAccountTabs->setReadOnly(TRUE);
            session::set('MyAccountTabs',$MyAccountTabs);
        }
    }

    if(session::is_set('MyAccountTabs')){
        $MyAccountTabs = session::get('MyAccountTabs');

        /* Enter edit mode? */
        if ((isset($_POST['edit'])) && (!session::is_set('edit'))){

            /* Check locking */
            if (($username= get_lock($ui->dn)) != ""){
                session::set('back_plugin',$plug);
                session::set('LOCK_VARS_TO_USE',array("/^edit$/","/^plug$/"));
                $lock_msg =  gen_locked_message ($username, array($ui->dn));

            }else{

                /* Lock the current entry */
                add_lock ($ui->dn, $ui->dn);
                session::set('edit',TRUE);
                $MyAccountTabs->setReadOnly(FALSE);
            }
        }

        $info= "";
        if (isset($_POST['edit_finish'])){
            $message= $MyAccountTabs->check ();
            if (count ($message) == 0){
                $MyAccountTabs->save ();
                del_lock ($ui->dn);
                session::un_set ('edit');
                $MyAccountTabs->setReadOnly(TRUE);
                if(isset($MyAccountTabs->by_object['user']) && $MyAccountTabs->by_object['user']->password_change_needed()){
                    $MyAccountTabs->password_change_needed = TRUE;
                }else{
                    session::un_set ('MyAccountTabs');
                }
            } else {
                msg_dialog::displayChecks($message);
            }
        }

        // Build up password class to make password-hash changes 
        //  effective, by setting a new password.
        if($MyAccountTabs->password_change_needed){
            if(!isset($MyAccountTabs->passwordClass)){
                $user = $MyAccountTabs->by_object['user'];
                $MyAccountTabs->passwordClass= new password($config, $ui->dn);
                $MyAccountTabs->passwordClass->forceHash($user->pw_storage);
            }
            $display.= $MyAccountTabs->passwordClass->execute();
        }

        /* Execute formular */
        if(!$MyAccountTabs->password_change_needed){
            pathNavigator::registerPlugin(_("My account"));
            if($lock_msg){
                $display = $lock_msg;
            }else{
                $display.= $MyAccountTabs->execute ();
            }
        }

        /* Store changes  in session */
        if (session::is_set('edit')){
            session::set('MyAccountTabs',$MyAccountTabs);
        }

        /* Show page footer depending on the mode */
        if (!$MyAccountTabs->is_modal_dialog() && empty($lock_msg) && !$MyAccountTabs->password_change_needed){

            $display.= "<div class='plugin-actions'>";

            /* Are we in edit mode? */
            if (session::is_set('edit')){
                $display.= "<button class='button' type='submit' name='edit_finish'>".msgPool::okButton()."</button>\n";
                $display.= "<button class='button' type='submit' name='edit_cancel'>".msgPool::cancelButton()."</button>\n";
            } else {
                if(preg_match("/r/",$ui->get_category_permissions($ui->dn,"users"))){
                    $display.= "<button class='button' type='submit' name='edit'>".msgPool::editButton()."</button>\n";
                }
                $display.= "<input type='hidden' name='ignore'>\n";
            }
            $display.= "</div>\n";
        }
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
