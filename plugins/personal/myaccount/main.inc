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
if ($remove_lock || isset($_POST['edit_cancel'])){
  if(session::is_set('MyAccountTabs') && session::is_set('edit')){
    session::un_set('MyAccountTabs');
    del_lock($ui->dn);
  }
}

/* Reset requested? */
if (isset($_POST['edit_cancel']) || $cleanup){
  session::un_set ('edit');
  session::un_set ('MyAccountTabs');
}

/* Remove this plugin from session
 */
if (! $cleanup ){

  /* Create MyAccountTabs object on demand */
  if (!session::is_set('MyAccountTabs') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
    $MyAccountTabs= new MyAccountTabs($config,$config->data['TABS']['MYACCOUNTTABS'], $ui->dn, "users", true, true); 
    $MyAccountTabs->setReadOnly(TRUE);
    session::set('MyAccountTabs',$MyAccountTabs);
  }
  $MyAccountTabs = session::get('MyAccountTabs');
  
  /* Enter edit mode? */
  if ((isset($_POST['edit'])) && (!session::is_set('edit'))){
    $MyAccountTabs->setReadOnly(FALSE);
    
    /* Check locking */
    if (($username= get_lock($ui->dn)) != ""){
      session::set('back_plugin',$plug);
      session::set('LOCK_VARS_TO_USE',array("/^edit$/","/^plug$/"));
      $lock_msg =  gen_locked_message ($username, $ui->dn);

    }else{

      /* Lock the current entry */
      add_lock ($ui->dn, $ui->dn);
      session::set('edit',TRUE);
    }
  }

  /* save changes to LDAP and disable edit mode */
  $info= "";
  if (isset($_POST['edit_finish'])){

    /* Perform checks */
    $message= $MyAccountTabs->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $MyAccountTabs->save ();
      $MyAccountTabs->setReadOnly(TRUE);
      del_lock ($ui->dn);
      session::un_set ('edit');

      /* Remove from session */
      session::un_set ('MyAccountTabs');
    } else {
      /* Errors found, show message */
      msg_dialog::displayChecks($message);
    }
  }

  /* Execute formular */
  if($lock_msg){
    $display = $lock_msg;
  }else{
    $display.= $MyAccountTabs->execute ();
  }

  /* Store changes  in session */
  if (session::is_set('edit')){
    session::set('MyAccountTabs',$MyAccountTabs);
  }

  /* Show page footer depending on the mode */
  if (!$MyAccountTabs->is_modal_dialog() && empty($lock_msg)){

    $display.= "<div class='plugin-actions'>";

    /* Are we in edit mode? */
    if (session::is_set('edit')){
      $display.= "<button class='button' type='submit' name='edit_finish'>".msgPool::okButton()."</button>\n";
      $display.= "<button class='button' type='submit' name='edit_cancel'>".msgPool::cancelButton()."</button>\n";
    } else {
      if(preg_match("/r/",$ui->get_permissions($ui->dn,"users/MyAccountTabs"))){
        $display.= "<button class='button' type='submit' name='edit'>".msgPool::editButton()."</button>\n";
      }
      $display.= "<input type='hidden' name='ignore'>\n";
    }
    $display.= "</div>\n";
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
