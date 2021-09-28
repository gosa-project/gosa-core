<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2005 Alejandro Escanero Blanco

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

class oxchangeAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Open-Xchange";
  var $plDescription= "Manage Open-Xchange user settings";

  /* GW attributes */
  var $OXAppointmentDays="500";
  var $OXTaskDays="5";
  var $OXTimeZone="GMT";
  var $mailEnabled="OK";
  var $dnModeValue = "";
  var $view_logged = FALSE;

  /* attribute list for save action */
  var $attributes= array("OXAppointmentDays", "OXTaskDays", "OXTimeZone","mailEnabled");
  var $objectclasses= array("OXUserObject");
  var $ReadOnly =false;
  var $oxconf = array();
  var $timezones=array();
  var $dnmode= "";
  var $uid ="";
  var $parent = NULL;

  function __construct(&$config, $dn= NULL,&$parent)
  {
    plugin::__construct ($config, $dn,$parent);
    $this->parent = &$parent;
    for ($i=0;$i<count($this->config->data['TABS']['CONNECTIVITY']);$i++){
      if($this->config->data['TABS']['CONNECTIVITY'][$i]['CLASS']=='oxchangeAccount') {
        $this->oxconf=$this->config->data['TABS']['CONNECTIVITY'][$i];
        break;
      }
    }

    $zones = timezone::_get_tz_zones();
    $this->timezones= array_keys($zones['TIMEZONES']);

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

    $this->dnmode= $this->config->get_cfg_value("core","accountPrimaryAttribute");
    if(isset($this->attrs[$this->dnmode][0])){
      $this->dnModeValue = $this->attrs[$this->dnmode][0];
    }
  }

  function execute()
  {
    /* Call parent execute */
    //plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    $display="";

    /* Show main page */
    $smarty= get_smarty();

    if (function_exists("pg_connect")){
      $smarty->assign("pg", true);
    }else{
      $smarty->assign("pg", false);
    }

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", set_post($this->$val));
    }
    if ($this->is_account){
      $smarty->assign("oxchangeState", "checked");
      $smarty->assign("oxState", "");
    } else {
      $smarty->assign("oxchangeState", "");
      $smarty->assign("oxState", "disabled");
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $key => $desc){
      $smarty->assign($key."ACL",$this->getacl($key,$this->ReadOnly));
      $smarty->assign($key."_W",$this->acl_is_writeable($key));
    }

    if((!$this->ReadOnly) && (($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable())) ){
      $smarty->assign('oxchangeAccountACL', "");
    }else{
      $smarty->assign('oxchangeAccountACL', " disabled ");
    }

    $smarty->assign("timezones", $this->timezones);

    if ($this->parent !== NULL){
      $smarty->assign("tabbed", 1);
    }else{
      $smarty->assign("tabbed", 0);
	}

	/* Trying to open a Postgresql Database Server */
	if (function_exists("pg_connect")){
		if(isset($this->oxconf["PGUSER"]) and isset($this->oxconf["PGHOST"]) and isset($this->oxconf["PGDBNAME"]) and isset($this->oxconf["PGPASSWD"])){
			$pgcon =  @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
			if (! $pgcon){
				$smarty->assign("pg", false);
			}
		}else{
			$smarty->assign("pg", false);
		}
	}else{
		$smarty->assign("pg", false);
	}

    $display.= $smarty->fetch (get_template_path('oxchange.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if(!$this->acl_is_removeable() || !$this->initially_was_account)  return;
    $this->attrs[$this->dnmode][0] = $this->dnModeValue;
    $sqldeluser=array(
        "delete from prg_notes where (user_right like '".$this->attrs[$this->dnmode][0]."') and (group_right like 's')",
        "delete from prg_documents_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_documents_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_docufolders_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_docufolders_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_date_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_date_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_dates_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_folder_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_folder_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_pin_board_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_pin_board_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_folder_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_folder_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_projects_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_projects_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete	from prg_projects_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from oxfolder_permissions where (entity like '".$this->attrs[$this->dnmode][0]."') AND ((role = 256) OR (role = 1024))",
        "delete from oxfolder_standardfolders where owner like '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_read where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_mod where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_abo where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_seen where username = '".$this->attrs[$this->dnmode][0]."'",
        "delete from sys_holiday where (userid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from usr_holiday where (userid like '".$this->attrs[$this->dnmode][0]."')");

    /* Trying to open a Postgresql Database Server */
    if (function_exists("pg_connect")){
      if(isset($this->oxconf["PGUSER"]) and 
		 isset($this->oxconf["PGHOST"]) and 
	     isset($this->oxconf["PGDBNAME"]) and 
         isset($this->oxconf["PGPASSWD"])){
        $pgcon = @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
        if (! $pgcon){
          msg_dialog::display(_("Error"), msgPool::dbconnect(_("Open-Xchange"),@pg_last_error($pgcon)), ERROR_DIALOG);
          return;
        }
      }else{
        msg_dialog::display(_("Error"), msgPool::noserver(_("Open-Xchange database")), ERROR_DIALOG);
        return;
      }
    }else{
      msg_dialog::display(_("Configuration error"),msgPool::missingext("postgresql"), ERROR_DIALOG);
      return;
    }

    plugin::remove_from_parent();
    $ldap= $this->config->get_ldap_link();

    if($ldap->dn_exists("ou=addr,".$this->dn)){
    	$ldap->rmdir_recursive("ou=addr,".$this->dn);
      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
      }
    }

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');

    /* Finally remove data from postgresql server */
    foreach ($sqldeluser as $sqls){
       @pg_exec($pgcon,$sqls);
    }
    pg_close();
  }

  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['oxchange'])){
        if (!$this->is_account && $_POST['oxchange'] == "B") {
          if($this->acl_is_createable()){
            $this->is_account= TRUE;
          }
        }
      } else {
        if($this->acl_is_removeable()){
          $this->is_account= FALSE;
        }
      }
    }

    /* Get objects */
    foreach(array("OXTimeZone","OXTaskDays","OXAppointmentDays") as $name) {
      if($this->acl_is_writeable($name) && isset($_POST[$name])){
        $this->$name = get_post($name);
      } 
    }
  
    if (isset($_POST["oxchangeStatus"])){
      $this->oxchangeStatus = "disabled";
    } else {
      $this->oxchangeStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {
    $needupdate=TRUE;
    $istemplate=FALSE;

    /*First at all, we must check if this is new or is updated */
    /*Also check is we have a template, if is a template, is a new user */
    if (isset($this->attrs['objectClass'])){
      foreach ($this->attrs['objectClass'] as $object){
        if($object=="OXUserObject") $needupdate=FALSE;
	if($object=="gosaUserTemplate") $istemplate=TRUE;
      }
    }
  
    /* Detect accounts uid 
     */
    if(!isset($this->attrs[$this->dnmode][0])){
      $uid = $this->parent->by_object['user']->uid;
    }else{
      $uid=$this->attrs[$this->dnmode][0];
    }

    if (trim($uid) == "") {
      msg_dialog::display(_("Error"), msgPool::invalid(_("Open-Xchange account name")), ERROR_DIALOG);
      return;
    }
	
    if ($istemplate) $needupdate=TRUE;

    if($needupdate){
      /* Trying to open a Postgresql Database Server */
      if (function_exists("pg_connect")){
        if(isset($this->oxconf["PGUSER"]) and isset($this->oxconf["PGHOST"]) and isset($this->oxconf["PGDBNAME"]) and isset($this->oxconf["PGPASSWD"])){
          $pgcon =  @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
          if (! $pgcon){
            msg_dialog::display(_("Error"), msgPool::dbconnect(_("Open-Xchange"),@pg_last_error($pgcon)), ERROR_DIALOG);
            return;
          }
        }else{
          msg_dialog::display(_("Error"), msgPool::noserver(_("Open-Xchange database")), ERROR_DIALOG);
          return;
        }
      }else{
        msg_dialog::display(_("Configuration error"),msgPool::missingext("postgresql"), ERROR_DIALOG);
        return;
      }
    }

    plugin::save();

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
    }

    // Optionally execute a command after we're done
    $this->handle_post_events('add');

    if($needupdate){
      $ldap->create_missing_trees("ou=addr,".$this->dn);
      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
      }

      /* Finally save data to postgresql server */
      pg_set_client_encoding ("UNICODE");
      $nv = "SELECT nextval ('serial_id')";
      $ot = "insert into oxfolder_tree (fuid, parent, fname, module, type, owner, creator, creating_date, created_from, changing_date, changed_from) VALUES (%d, 1, '%s', '%s', 'private', '%s', '%s', 'now', 'System', 'now', 'System')";
      $op = "INSERT INTO oxfolder_permissions (puid, pid, role, entity, sealed, fp, orp, owp, odp) VALUES (%d, %d, 32768,  '%s', 0, 128, 128, 128, 128)";
      $os = "insert into oxfolder_standardfolders (owner,module_calendar,module_contact,module_task) VALUES ('%s',%d,%d,%d)";
      $ugr= "INSERT INTO usr_general_rights SELECT creating_date, created_from, changing_date, changed_from,text('%s'),  addr_u, addr_r, addr_d, cont_u, cont_r, cont_d, data_u, data_r, data_d, serie_u, serie_r, serie_d, task_u, task_r, task_d,  refer, proj_u, proj_r, proj_d, dfolder_u, dfolder_r, dfolder_d, doc_u, doc_r, doc_d, knowl_u, knowl_r, knowl_d, bfolder_u,  bfolder_r, bfolder_d, bookm_u, bookm_r, bookm_d, pin_u, pin_r, pin_d, forum_n, fentrie_n, setup, pin_public, internal,  int_groups, kfolder_u, kfolder_r, kfolder_d, webmail FROM sys_gen_rights_template WHERE login LIKE 'default_template'";

      $error = FALSE;
      $res=@pg_exec($pgcon,$nv); 

      if(!$res){
        $error = TRUE;
      }else{
        $calendarid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$calendarid[0],'My Appointments','calendar',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 

      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$calendarid[0],$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $contactsid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$contactsid[0],'My Contacts','contact',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$contactsid[0],$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $tasksid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$tasksid[0],'My Tasks','task',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$tasksid[0],$uid);
        @pg_exec($pgcon,$q);
      
        $q=sprintf($os,$uid,$calendarid[0],$contactsid[0],$tasksid[0]);
        @pg_exec($pgcon,$q);
      
        $q=sprintf($ugr,$uid);
        @pg_exec($pgcon,$q);
      }
    
      @pg_close($pgcon);

      if($error){
        msg_dialog::display(_("Error"), _("Cannot save Open-Xchange account!"), ERROR_DIALOG);
      }
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Open-Xchange"),
          "plDescription"   => _("Open-Xchange account settings")."&nbsp;("._("Connectivity add-on").")",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 27,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),
          "plRequirements"=> array(
              'ldapSchema' => array('OXUserObject' => ''),
              'onFailureDisablePlugin' => array(get_class())
              ),

          "plProvidedAcls"  => array(
            "OXAppointmentDays" => _("OXAppointmentDays"),
            "OXTaskDays"   	    => _("OXTaskDays"),
            "OXTimeZone"   	    => _("OXTimeZone"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
