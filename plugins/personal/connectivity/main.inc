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
  /* Reset requested? */
  if (isset($_POST['edit_cancel']) ||
      (isset($_GET['reset']) && $_GET['reset'] == 1)){

    del_lock ($ui->dn);
    sess_del ('edit');
    sess_del ('connectivity');
  }

  /* Create connectivity object on demand */
  if (!isset($_SESSION['connectivity']) || (isset($_GET['reset']) &&
      $_GET['reset'] == 1)){
    $_SESSION['connectivity']= new connectivity ($config, $ui->dn);
  }
  $connectivity= $_SESSION['connectivity'];

  /* save changes back to object */
  if (isset ($_SESSION['edit'])){
    $connectivity->save_object ();
  }

  /* Enter edit mode? */
  if ((isset($_POST['edit'])) && (!isset( $_SESSION['edit'])){

    /* Check locking */
    if (($username= get_lock($ui->dn)) != ""){
      $_SESSION['back_plugin']= $plug;
      gen_locked_message ($username, $ui->dn);
      exit ();
    }

    /* Lock the current entry */
    add_lock ($ui->dn, $ui->dn);
    $_SESSION['dn']= $ui->dn;
    $_SESSION['edit']= TRUE;
  }

  /* save changes to LDAP and disable edit mode */
  if (isset($_POST['edit_finish'])){

    /* Perform checks */
    $message= $connectivity->check ();

    /* No errors, save object */
    if (count ($message) == 0){
      $connectivity->save ();
      gosa_log ("User/connectivity object'".$ui->dn."' has been saved");
      $connectivity->acl= "#none#";
      del_lock ($ui->dn);
      sess_del ('edit');

      /* Write back to session */
      $_SESSION['connectivity']= $connectivity;
    } else {
      /* Errors found, show message */
      show_errors ($message);
    }
  }

  /* Adjust acl's to mode */
  if (isset($_SESSION['edit'])){
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $connectivity->acl= get_module_permission($acl, "connectivity", $ui->dn);
    foreach ($connectivity->plugin_name as $name){
      $connectivity->plugin[$name]->acl= get_module_permission($acl, "$name", $ui->dn);
    }
  } else {
    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $editacl= get_module_permission($acl, "connectivity", $ui->dn);
    $connectivity->acl= "#none#";
    foreach ($connectivity->plugin_name as $name){
      $connectivity->plugin[$name]->acl= "#none#";
    }
  }

  /* Execute formular */
  $display= $connectivity->execute ();

  /* Store changes  in session */
  if (isset ($_SESSION['edit'])){
    $_SESSION['connectivity']= $connectivity;
  }

  $info= "";

  /* Are we in edit mode? */
  $in_edit_mode= false;
  foreach ($connectivity->plugin_name as $name){
    $acls = get_module_permission($acl, "$name", $ui->dn);
    if($acls != "#none#"){
      $in_edit_mode|= true;
    }
  }
  if ($in_edit_mode){
    $display.="<div align='right'>";
    if (isset($_SESSION['edit'])){
      $display.= "<input type=submit name=\"edit_finish\" value=\""._("Save")."\">\n";
      $display.= "&nbsp;";
      $display.= "<input type=submit name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
      $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/closedlock.png').
             "\"> ".$ui->dn."&nbsp;";
    } else {
      $info= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/openlock.png').
             "\"> ".$ui->dn."&nbsp;";
      if ($editacl != "#none#"){
        $info.= "<img class=\"center\" alt=\"\" align=\"middle\" src=\"".get_template_path('images/lamp.png').
                "\">"._("Click the 'Edit' button below to change informations in this dialog");
        $display.= "<input type=submit name=\"edit\" value=\""._("Edit")."\">\n";
      }
      $display.= "<input type=\"hidden\" name=\"ignore\">\n";
    }
    $display.="</div>";
  }
//  $display.= "</p>\n";

  /* Page header*/
  $display= print_header(get_template_path('images/proxy.png'),
                         _("Connectivity"), $info).$display;

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
