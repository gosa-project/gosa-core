<?php

/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2007 Fabian Hickert

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


class Step_Schema extends setup_step
{
  var $checked = array();
  var $not_checked = array();
  var $schema_readable  = FALSE;
  var $attributes       = array("enable_schema_check","samba_version");
  var $enable_schema_check = TRUE;
  var $samba_version       = 3;

  function Step_Schema()
  {
    $this->update_strings();
  }

  
  function update_strings()
  {
    $this->s_title      = _("LDAP schema check");
    $this->s_title_long = _("LDAP schema check");
    $this->s_info       = _("Perform test on your current LDAP schema");
  }

  
  function execute()
  {
    $this->check_schema();
    $failed_checks = 0 ; 
    foreach($this->checked as $val) {
      if(!$val['STATUS']){
        $failed_checks ++;
      }
    }

    if($failed_checks == 0){
      $this->is_completed = TRUE;
    }else{
      $this->is_completed = TRUE;
    }

    $smarty = get_smarty();
    $smarty->assign("bool",array(FALSE => _("No"), TRUE => _("Yes")));
    $smarty->assign("schema_readable",$this->schema_readable);
    $smarty->assign("enable_schema_check",$this->enable_schema_check);
    $smarty->assign("checks",$this->checked);
    $smarty->assign("not_checked",$this->not_checked);
    $smarty->assign("failed_checks",$failed_checks);
    return($smarty -> fetch (get_template_path("../setup/setup_schema.tpl")));
  }

  function check_schema_version($class, $version)
  {
    return preg_match("/\(v$version\)/", $class['DESC']);
  }


  function save_object()
  {
    if(isset($_POST['step7_posted'])){

      /* Get attributes */
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = validate($_POST[$attr]);
        }
      }
    }
  }


  function check_schema()
  {
    $cfg = $this->parent->captured_values;
    $messages= array();

    /* Get objectclasses */
    $ldap = new LDAP($cfg['admin'],$cfg['password'],$cfg['connection'] ,FALSE,$cfg['tls']);
    $objectclasses = $ldap->get_objectclasses();
    if(count($objectclasses) == 0){
      print_red(_("Can't get schema information from server. No schema check possible!"));
    }

  
    /* Which samba version do we use? */
    if(isset($objectclasses['sambaSamAccount'])){
      $this->samba_version = 3;
    } elseif(isset($objectclasses['sambaAccount'])) {
      $this->samba_version = 2;
    }else{
      $this->samba_version = 0;
    }

    /* This is the default block used for each entry. 
     *  to avoid unset indexes. 
     */
    $def_check = array("REQUIRED_VERSION" => "0",
                       "SCHEMA_FILES"     => array(),
                       "CLASSES_REQUIRED" => array(),
                       "STATUS"           => FALSE,
                       "IS_MUST_HAVE"     => FALSE,
                       "MSG"              => "",
                       "INFO"             => "");#_("There is currently no information specified for this schema extension."));


    /* The gosa base schema */
    $checks['gosaObject'] = $def_check;
    $checks['gosaObject']['REQUIRED_VERSION'] = "2.4";
    $checks['gosaObject']['SCHEMA_FILES']     = array("gosa+samba3.schema","gosa.schema");
    $checks['gosaObject']['CLASSES_REQUIRED'] = array("gosaObject");
    $checks['gosaObject']['IS_MUST_HAVE']     = TRUE;

    /* GOsa Account class */
    $checks["gosaAccount"]["REQUIRED_VERSION"]= "2.4";
    $checks["gosaAccount"]["SCHEMA_FILES"]    = array("gosa+samba3.schema","gosa.schema");
    $checks["gosaAccount"]["CLASSES_REQUIRED"]= array("gosaAccount");
    $checks["gosaAccount"]["IS_MUST_HAVE"]    = TRUE;
    $checks["gosaAccount"]["INFO"]            = _("Used to store account specific informations.");

    /* GOsa lock entry, used to mark currently edited objects as 'in use' */
    $checks["gosaLockEntry"]["REQUIRED_VERSION"] = "2.4";
    $checks["gosaLockEntry"]["SCHEMA_FILES"]     = array("gosa+samba3.schema","gosa.schema");
    $checks["gosaLockEntry"]["CLASSES_REQUIRED"] = array("gosaLockEntry");
    $checks["gosaLockEntry"]["IS_MUST_HAVE"]     = TRUE;
    $checks["gosaLockEntry"]["INFO"]             = _("Used to lock currently edited entries to avoid multiple changes at the same time.");

    /* Some other checks */
    foreach(array(
          "gosaCacheEntry"        => array("version" => "2.4"),
          "gosaDepartment"        => array("version" => "2.4"),
          "goFaxAccount"          => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
          "goFaxSBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
          "goFaxRBlock"           => array("version" => "1.0.4", "class" => "gofaxAccount","file" => "gofax.schema"),
          "gosaUserTemplate"      => array("version" => "2.4", "class" => "posixAccount","file" => "nis.schema"),
          "gosaMailAccount"       => array("version" => "2.4", "class" => "mailAccount","file" => "gosa+samba3.schema"),
          "gosaProxyAccount"      => array("version" => "2.4", "class" => "proxyAccount","file" => "gosa+samba3.schema"),
          "gosaApplication"       => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),
          "gosaApplicationGroup"  => array("version" => "2.4", "class" => "appgroup","file" => "gosa.schema"),
          "GOhard"                => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
          "gotoTerminal"          => array("version" => "2.5", "class" => "terminals","file" => "goto.schema"),
          "goServer"              => array("version" => "2.4","class" => "server","file" => "goserver.schema"),
          "goTerminalServer"      => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
          "goShareServer"         => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
          "goNtpServer"           => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
          "goSyslogServer"        => array("version" => "2.4", "class" => "terminals","file" => "goto.schema"),
          "goLdapServer"          => array("version" => "2.4"),
          "goCupsServer"          => array("version" => "2.4", "class" => array("posixAccount", "terminals"),),
          "goImapServer"          => array("version" => "2.4", "class" => array("mailAccount", "mailgroup"),"file" => "gosa+samba3.schema"),
          "goKrbServer"           => array("version" => "2.4"),
          "goFaxServer"           => array("version" => "2.4", "class" => "gofaxAccount","file" => "gofax.schema"),
          ) as $name => $values){

      $checks[$name] = $def_check;
      if(isset($values['version'])){
        $checks[$name]["REQUIRED_VERSION"] = $values['version'];
      }
      if(isset($values['file'])){
        $checks[$name]["SCHEMA_FILES"] = array($values['file']);
      }
      $checks[$name]["CLASSES_REQUIRED"] = array($name);
    }

    foreach($checks as $name => $value){
      foreach($value['CLASSES_REQUIRED'] as $class){

        if(!isset($objectclasses[$name])){
          $checks[$name]['STATUS'] = FALSE;
          if($value['IS_MUST_HAVE']){
            $checks[$name]['MSG']    = sprintf(_("The required objectClass '%s' is not present in your schema setup"),$class);
          }else{
            $checks[$name]['MSG']    = sprintf(_("The optional objectClass '%s' is not present in your schema setup"),$class);
          }
        }elseif(!$this->check_schema_version($objectclasses[$name],$value['REQUIRED_VERSION'])){
          $checks[$name]['STATUS'] = FALSE;

          if($value['IS_MUST_HAVE']){
            $checks[$name]['MSG'] = sprintf(_("The required objectclass '%s' does not have version %s"), $class, $value['REQUIRED_VERSION']);
          }else{
            $checks[$name]['MSG'] = sprintf(_("The optional objectclass '%s' does not have version %s"), $class, $value['REQUIRED_VERSION']);
          }
        }else{
          $checks[$name]['STATUS'] = TRUE;
          $checks[$name]['MSG'] = sprintf(_("Class(es) available"));
        }
      }
    }
  
    $tmp = $objectclasses;

    /* Depending on selected rfc2307bis mode, we need different schema configurations */
    $rfc2307bis = $this->parent->captured_values['rfc2307bis'];


    /* The gosa base schema */
    $checks['posixGroup'] = $def_check;
    $checks['posixGroup']['REQUIRED_VERSION'] = "2.4";
    $checks['posixGroup']['SCHEMA_FILES']     = array("gosa+samba3.schema","gosa.schema");
    $checks['posixGroup']['CLASSES_REQUIRED'] = array("posixGroup");
    $checks['posixGroup']['STATUS']           = TRUE;
    $checks['posixGroup']['IS_MUST_HAVE']     = TRUE;
    $checks['posixGroup']['MSG']              = "";
    $checks['posixGroup']['INFO']             = "";

    if(isset($tmp['posixGroup'])){

      if($rfc2307bis && isset($tmp['posixGroup']['STRUCTURAL'])){
        $checks['posixGroup']['STATUS']           = FALSE;
        $checks['posixGroup']['MSG']              = _("You have enabled the rfc2307bis option on the 'ldap setup' step, but your schema configuration do not support this option.");
        $checks['posixGroup']['INFO']             = _("In order to use rfc2307bis conform groups the objectClass 'posixGroup' must be AUXILIARY");
      }
      if(!$rfc2307bis && !isset($tmp['posixGroup']['STRUCTURAL'])){
        $checks['posixGroup']['STATUS']           = FALSE;
        $checks['posixGroup']['MSG']              = _("You have disabled the rfc2307bis option on the 'ldap setup' step, but your schema configuration do not support this option.");
        $checks['posixGroup']['INFO']             = _("The objectClass 'posixGroup' must be STRUCTURAL");
      }
    }

    $this->checked = $checks;
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
