<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_account.inc 9975 2008-03-25 14:09:30Z hickert $$
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

class accountSoFS extends uniPlugin
{
  /* Definitions */
  var $plHeadline= "SoFS-Disk space";
  var $plDescription= "SoFS-Disk space plugin";


  function __construct(&$config,$dn)
  {
    plugin::plugin($config,$dn);

  }


  function execute()
  {
    $smarty = get_smarty();

    return($display.$smarty->fetch(get_template_path('accountQuota.tpl',TRUE,dirname(__FILE__))));
  }
 
  
  function save_object()
  {
    plugin::save_object();
  }


  function check()
  {
    $messages = plugin::check();
    return($messages);
  }


  function save()
  {
    plugin::save();
    $this->cleanup();
  }


  /* remove object from parent */
  function remove_from_parent()
  {
    $ui=get_userinfo();  

    /* Remove and write to LDAP */
    if(!$this->initially_was_account) return(false);

    plugin::remove_from_parent();

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__, $this->attributes, "Save");
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs);

    new log("remove","uniBonnAccount/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
    }else{
      $this->handle_post_events("remove",array("uid" => $uid, "userdn" => $ui->dn));
    }
  }

  function multiple_execute(){  return($this->execute()); }

  static function plInfo()
  {
    return (array(
          "plDescription"     => _("SoFS-Disk space"),
          "plSelfModify"      => TRUE,
          "plDepends"         => array("user"),
          "plPriority"        => 2,
          "plSection"         => array("personal" => _("My account")),
          "plCategory"        => array("uniBonnAccount"),
          "plOptions"         => array(),

          "plProvidedAcls"  => array(
             "test" => _("Test")) 
        ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
