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



/****************
 * FUNCTIONS 

Step_Migrate                - Constructor.
update_strings              - Used to update the displayed step informations.
initialize_checks           - Initialize migration steps.
check_ldap_permissions      - Check if the used admin account has full access to the ldap database.
check_gosaAccounts          - Check if there are users without the required objectClasses.
migrate_gosaAccounts        - Migrate selected users to GOsa user accounts.
check_organizationalUnits   - Check if there are departments, that are not visible for GOsa
migrate_organizationalUnits - Migrate selected departments 
check_administrativeAccount - Check if there is at least one acl entry available                  
checkBase                   - Check if there is a root object available 

get_user_list               - Get list of available users
get_group_list              - Get list of groups
  
create_admin                
create_admin_user           

execute                     - Generate html output of this plugin
save_object                 - Save posts 
array_to_ldif               - Create ldif output of an ldap result array 
 
 ****************/



class Step_Migrate extends setup_step
{
  var $languages      = array();
  var $attributes     = array();
  var $header_image   = "images/monitoring.png";
  var $checks         = array();

  /* Department migration attributes */
  var $dep_migration_dialog = FALSE;
  var $deps_to_migrate      = array();
  var $show_details         = FALSE;

  /* Department migration attributes */
  var $users_migration_dialog= FALSE;
  var $users_to_migrate      = array();

  /* Create Acl attributes */
  var $acl_create_dialog  = FALSE;
  var $acl_create_selected= ""; // Currently selected element, that should receive admin rights 
  var $acl_create_changes = ""; // Contains ldif information about changes 
  var $acl_create_confirmed= FALSE;

  /* Checks initialised ? */
  var $checks_initialised = FALSE;

  /* Users outside to people ou */
  var $outside_users        = array();
  var $outside_users_dialog = FALSE;

  /* Users outside to groups ou */
  var $outside_groups        = array();
  var $outside_groups_dialog = FALSE;

  /* Win-Workstations outside to reserved ou */
  var $outside_winstations        = array();
  var $outside_winstations_dialog = FALSE;

  /* check for multiple use of same uidNumber */
  var $check_uidNumbers        = array();
  var $check_uidNumbers_dialog = FALSE;

  /* check for multiple use of same gidNumber */
  var $check_gidNumbers        = array();
  var $check_gidNumbers_dialog = FALSE;


  function Step_Migrate()
  {
    $this->update_strings(); 
  }

  function update_strings()
  {
    $this->s_title      = _("LDAP inspection");
    $this->s_title_long = _("LDAP inspection");
    $this->s_info       = _("Analyze your current LDAP for GOsa compatibility");
  }

  function initialize_checks()
  {
    $this->checks = array();
    $this->checks['root']['TITLE']     = _("Checking for root object");
    $this->checks['root']['STATUS']    = FALSE;
    $this->checks['root']['STATUS_MSG']= "";
    $this->checks['root']['ERROR_MSG'] = "";
    $this->checkBase();

    $this->checks['permissions']['TITLE']     = _("Checking permissions on LDAP database");
    $this->checks['permissions']['STATUS']    = FALSE;
    $this->checks['permissions']['STATUS_MSG']= "";
    $this->checks['permissions']['ERROR_MSG'] = "";
    $this->check_ldap_permissions();

    $this->checks['deps_visible']['TITLE']     = _("Checking for invisible departments");
    $this->checks['deps_visible']['STATUS']    = FALSE;
    $this->checks['deps_visible']['STATUS_MSG']= "";
    $this->checks['deps_visible']['ERROR_MSG'] = "";
    $this->check_organizationalUnits();

    $this->checks['users_visible']['TITLE']     = _("Checking for invisible users");
    $this->checks['users_visible']['STATUS']    = FALSE;
    $this->checks['users_visible']['STATUS_MSG']= "";
    $this->checks['users_visible']['ERROR_MSG'] = "";
    $this->check_gosaAccounts();

    $this->checks['acls']['TITLE']     = _("Checking for super administrator");
    $this->checks['acls']['STATUS']    = FALSE;
    $this->checks['acls']['STATUS_MSG']= "";
    $this->checks['acls']['ERROR_MSG'] = "";
    $this->check_administrativeAccount();

    $this->checks['outside_users']['TITLE']     = _("Checking for users outside the people tree");
    $this->checks['outside_users']['STATUS']    = FALSE;
    $this->checks['outside_users']['STATUS_MSG']= "";
    $this->checks['outside_users']['ERROR_MSG'] = "";
    $this->search_outside_users();
    
    $this->checks['outside_groups']['TITLE']     = _("Checking for groups outside the groups tree");
    $this->checks['outside_groups']['STATUS']    = FALSE;
    $this->checks['outside_groups']['STATUS_MSG']= "";
    $this->checks['outside_groups']['ERROR_MSG'] = "";
    $this->search_outside_groups();

    $this->checks['outside_winstations']['TITLE']     = _("Checking for windows workstations outside the winstation tree");
    $this->checks['outside_winstations']['STATUS']    = FALSE;
    $this->checks['outside_winstations']['STATUS_MSG']= "";
    $this->checks['outside_winstations']['ERROR_MSG'] = "";
    $this->search_outside_winstations();

    $this->checks['uidNumber_usage']['TITLE']     = _("Checking for duplicated UID numbers");
    $this->checks['uidNumber_usage']['STATUS']    = FALSE;
    $this->checks['uidNumber_usage']['STATUS_MSG']= "";
    $this->checks['uidNumber_usage']['ERROR_MSG'] = "";
    $this->check_uidNumber();
    
    $this->checks['gidNumber_usage']['TITLE']     = _("Checking for duplicate GID numbers");
    $this->checks['gidNumber_usage']['STATUS']    = FALSE;
    $this->checks['gidNumber_usage']['STATUS_MSG']= "";
    $this->checks['gidNumber_usage']['ERROR_MSG'] = "";
    $this->check_gidNumber();
  }


  /* Check if there are uidNumbers which are used more than once. 
   */
  function check_uidNumber()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    $ldap->cd($cv['base']);
    $res = $ldap->search("(&(objectClass=posixAccount)(uidNumber=*))",array("dn","uidNumber"));
    if(!$res){
      $this->checks['uidNumber_usage']['STATUS']    = FALSE;
      $this->checks['uidNumber_usage']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['uidNumber_usage']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }

    $this->check_uidNumbers= array(); 
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[$attrs['uidNumber'][0]][] = $attrs;
    }

    foreach($tmp as $id => $entries){
      if(count($entries) > 1){
        foreach($entries as $entry){
          $this->check_uidNumbers[base64_encode($entry['dn'])] = $entry;
        }
      }
    }

    if($this->check_uidNumbers){
      $this->checks['uidNumber_usage']['STATUS']    = FALSE;
      $this->checks['uidNumber_usage']['STATUS_MSG']= "<font style='color:#F0A500'>"._("Warning")."</font>";
      $this->checks['uidNumber_usage']['ERROR_MSG'] =
        sprintf(_("Found %s duplicate values for attribute 'uidNumber'."),count($this->check_uidNumbers));
      return(false);
    }else{
      $this->checks['uidNumber_usage']['STATUS']    = TRUE;
      $this->checks['uidNumber_usage']['STATUS_MSG']= _("Ok");
      $this->checks['uidNumber_usage']['ERROR_MSG'] = "";
      return(TRUE);
    }
  }

  
  /* Check if there are duplicated gidNumbers present in ldap
   */
  function check_gidNumber()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    $ldap->cd($cv['base']);
    $res = $ldap->search("(&(objectClass=posixGroup)(gidNumber=*))",array("dn","gidNumber"));
    if(!$res){
      $this->checks['gidNumber_usage']['STATUS']    = FALSE;
      $this->checks['gidNumber_usage']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['gidNumber_usage']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }

    $this->check_gidNumbers= array(); 
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[$attrs['gidNumber'][0]][] = $attrs;
    }

    foreach($tmp as $id => $entries){
      if(count($entries) > 1){
        foreach($entries as $entry){
          $this->check_gidNumbers[base64_encode($entry['dn'])] = $entry;
        }
      }
    }

    if($this->check_gidNumbers){
      $this->checks['gidNumber_usage']['STATUS']    = FALSE;
      $this->checks['gidNumber_usage']['STATUS_MSG']= "<font style='color:#F0A500'>"._("Warning")."</font>";
      $this->checks['gidNumber_usage']['ERROR_MSG'] =
        sprintf(_("Found %s duplicate values for attribute 'gidNumber'."),count($this->check_gidNumbers));
      return(false);
    }else{
      $this->checks['gidNumber_usage']['STATUS']    = TRUE;
      $this->checks['gidNumber_usage']['STATUS_MSG']= _("Ok");
      $this->checks['gidNumber_usage']['ERROR_MSG'] = "";
      return(TRUE);
    }
  }


  /* Search for winstations outside the winstation ou 
   */
  function search_outside_winstations()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Get winstation ou */
    if($cv['generic_settings']['wws_ou_active']) {
      $winstation_ou = $cv['generic_settings']['ws_ou'];
    }else{
      $winstation_ou = "ou=winstations";
    }

    if($cv['samba_version'] == 3){
      $oc = "sambaSamAccount";
    }else{
      $oc = "sambaAccount";
    }
 
    $ldap->cd($cv['base']);
    $res = $ldap->search("(&(objectClass=".$oc.")(uid=*$))",array("dn","sambaSID"));
    if(!$res){
      $this->checks['outside_winstations']['STATUS']    = FALSE;
      $this->checks['outside_winstations']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['outside_winstations']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }

    $this->outside_winstations = array();
    while($attrs = $ldap->fetch()){
      if((!preg_match("/^[^,]+,".normalizePreg($winstation_ou)."/",$attrs['dn'])) && !preg_match("/,dc=addressbook,/",$attrs['dn'])){
        $attrs['selected'] = FALSE;
        $attrs['ldif']     = "";
        $this->outside_winstations[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    if(count($this->outside_winstations)){
      $this->checks['outside_winstations']['STATUS']    = FALSE;
      $this->checks['outside_winstations']['STATUS_MSG']= _("Failed");
      $this->checks['outside_winstations']['ERROR_MSG'] = 
        sprintf(_("Found %s winstations outside the predefined winstation department ou '%s'."),count($this->outside_winstations),$winstation_ou);
      $this->checks['outside_winstations']['ERROR_MSG'].= "<input type='submit' name='outside_winstations_dialog' value='"._("Migrate")."...'>";
      return(false);
    }else{
      $this->checks['outside_winstations']['STATUS']    = TRUE;
      $this->checks['outside_winstations']['STATUS_MSG']= _("Ok");
      $this->checks['outside_winstations']['ERROR_MSG'] = "";
      return(TRUE);
    }
  }


  /* Search for groups outside the group ou 
   */
  function search_outside_groups()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    $group_ou = $cv['groupou'];
    $ldap->cd($cv['base']);

    /***********
     * Get all gosaDepartments to be able to
     *  validate correct ldap tree position of every single user
     ***********/
    $valid_deps = array();
    $valid_deps['/'] = $cv['base'];
    $ldap->search("(&(objectClass=gosaDepartment)(ou=*))",array("dn","ou"));
    while($attrs = $ldap->fetch()){
      $valid_deps[] = $attrs['dn'];
    }

    /***********
     * Get all groups
     ***********/
    $res = $ldap->search("(objectClass=posixGroup)",array("dn"));
    if(!$res){
      $this->checks['outside_groups']['STATUS']    = FALSE;
      $this->checks['outside_groups']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['outside_groups']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }

    $this->outside_groups = array();
    while($attrs = $ldap->fetch()){
      $group_db_base = preg_replace("/^[^,]+,".normalizePreg($group_ou)."+,/i","",$attrs['dn']);

      /* Check if entry is not an addressbook only user
       *  and verify that he is in a valid department
       */
      if( !preg_match("/".normalizePreg("dc=addressbook,")."/",$group_db_base) &&
          !in_array($group_db_base,$valid_deps)
        ){
        $attrs['selected'] = FALSE;
        $attrs['ldif']     = "";
        $this->outside_groups[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    if(count($this->outside_groups)){
      $this->checks['outside_groups']['STATUS']    = FALSE;
      $this->checks['outside_groups']['STATUS_MSG']= "<font style='color:#F0A500'>"._("Warning")."</font>";
      $this->checks['outside_groups']['ERROR_MSG'] =
        sprintf(_("Found %s groups outside the configured tree '%s'."),count($this->outside_groups),$group_ou);
      $this->checks['outside_groups']['ERROR_MSG'].= "&nbsp;<input type='submit' name='outside_groups_dialog' value='"._("Move")."...'>";
      return(false);
    }else{
      $this->checks['outside_groups']['STATUS']    = TRUE;
      $this->checks['outside_groups']['STATUS_MSG']= _("Ok");
      $this->checks['outside_groups']['ERROR_MSG'] = "";
      return(TRUE);
    }
  }

 /* Search for users outside the people ou
   */
  function search_outside_users()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($cv['base']);


    /***********
     * Get all gosaDepartments to be able to
     *  validate correct ldap tree position of every single user
     ***********/
    $valid_deps = array();
    $valid_deps['/'] = $cv['base'];
    $ldap->search("(&(objectClass=gosaDepartment)(ou=*))",array("dn","ou"));
    while($attrs = $ldap->fetch()){
      $valid_deps[] = $attrs['dn'];
    }

    /***********
     * Search for all users
     ***********/
    $res = $ldap->search("(&(objectClass=gosaAccount)(!(uid=*$)))",array("dn"));
    if(!$res){
      $this->checks['outside_users']['STATUS']    = FALSE;
      $this->checks['outside_users']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['outside_users']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }

    /***********
     * Check if returned users are within a valid GOsa deparmtment. (peopleou,gosaDepartment,base)
     ***********/
    $this->outside_users = array();
    $people_ou = trim($cv['peopleou']);
    if(!empty($people_ou)){
      $people_ou = $people_ou.",";
    }

    while($attrs = $ldap->fetch()){
      $people_db_base = preg_replace("/^[^,]+,".normalizePreg($people_ou)."/i","",$attrs['dn']);

      /* Check if entry is not an addressbook only user
       *  and verify that he is in a valid department
       */
      if( !preg_match("/".normalizePreg("dc=addressbook,")."/",$people_db_base) &&
          !in_array($people_db_base,$valid_deps)
         ){
        $attrs['selected'] = FALSE;
        $attrs['ldif']     = "";
        $this->outside_users[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    if(count($this->outside_users)){
      $this->checks['outside_users']['STATUS']    = FALSE;
      $this->checks['outside_users']['STATUS_MSG']= "<font style='color:#F0A500'>"._("Warning")."</font>";
      $this->checks['outside_users']['ERROR_MSG'] =
        sprintf(_("Found %s user(s) outside the configured tree '%s'."),count($this->outside_users),$people_ou);
      $this->checks['outside_users']['ERROR_MSG'].= "<input type='submit' name='outside_users_dialog' value='"._("Move")."...'>";
      return(false);
    }else{
      $this->checks['outside_users']['STATUS']    = TRUE;
      $this->checks['outside_users']['STATUS_MSG']= _("Ok");
      $this->checks['outside_users']['ERROR_MSG'] = "";
      return(TRUE);
    }
  }


  /* Check ldap accessibility 
   * Create and remove a dummy object, 
   *  to ensure that we have the necessary permissions
   */
  function check_ldap_permissions()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Create dummy entry 
     */
    $name     = "GOsa_setup_text_entry_".session_id().rand(0,999999);
    $dn       = "ou=".$name.",".$cv['base'];
    $testEntry= array();
    $testEntry['objectClass'][]= "top";
    $testEntry['objectClass'][]= "organizationalUnit";
    $testEntry['objectClass'][]= "gosaDepartment";
    $testEntry['description']= "Created by GOsa setup, this object can be removed.";
    $testEntry['ou']  = $name;

    /* check if simple ldap cat will be successful 
     */
    $res = $ldap->cat($cv['base']);  
    if(!$res){
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['permissions']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
      return(false);
    }
  
    /* Try to create dummy object 
     */ 
    $ldap->cd ($dn);
    $ldap->create_missing_trees($dn);
    $res = $ldap->add($testEntry);
    $ldap->cat($dn);
    if(!$ldap->count()){
      new log("view","setup/".get_class($this),$dn,array(),$ldap->get_error());

      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      return(false);
    }

    /* Try to remove created entry 
     */
    $res = $ldap->rmDir($dn);
    $ldap->cat($dn);
    if($ldap->count()){
      new log("view","setup/".get_class($this),$dn,array(),$ldap->get_error());
      $this->checks['permissions']['STATUS']    = FALSE;
      $this->checks['permissions']['STATUS_MSG']= _("Failed");
      $this->checks['permissions']['ERROR_MSG'] = 
        sprintf(_("The specified user '%s' does not have full access to your ldap database."),$cv['admin']);
      return(false);
    }

    /* Create & remove of dummy object was successful */
    $this->checks['permissions']['STATUS']    = TRUE;
    $this->checks['permissions']['STATUS_MSG']= _("Ok");
    $this->checks['permissions']['ERROR_MSG'] = "";
    return(true);
  } 


  /* Check if there are users which will 
   *  be invisible for GOsa 
   */
  function check_gosaAccounts()
  {
    /* Remember old list of ivisible users, to be able to set 
     *  the 'html checked' status for the checkboxes again 
     */
    $cnt_ok = 0;
    $old    = $this->users_to_migrate;
    $this->users_to_migrate = array();

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Get all invisible users 
     */
    $ldap->cd($cv['base']); 
    $res =$ldap->search("(&(|(objectClass=posixAccount)(&(objectClass=inetOrgPerson)(objectClass=organizationalPerson)))(!(objectClass=gosaAccount))(uid=*))",array("sn","givenName","cn","uid"));
    while($attrs = $ldap->fetch()){
      if(!preg_match("/,dc=addressbook,/",$attrs['dn'])){
        $attrs['checked'] = FALSE;
        $attrs['before']  = "";
        $attrs['after']   = "";

        /* Set objects to selected, that were selected before reload */
        if(isset($old[base64_encode($attrs['dn'])])){
          $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
        }
        $this->users_to_migrate[base64_encode($attrs['dn'])] = $attrs;
      }
    }

    /* No invisible */
    if(!$res){
      $this->checks['users_visible']['STATUS']    = FALSE;
      $this->checks['users_visible']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['users_visible']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }elseif(count($this->users_to_migrate) == 0){
      $this->checks['users_visible']['STATUS']    = TRUE;
      $this->checks['users_visible']['STATUS_MSG']= _("Ok");
      $this->checks['users_visible']['ERROR_MSG'] = "";
    }else{
      $this->checks['users_visible']['STATUS']    = FALSE;
      $this->checks['users_visible']['STATUS_MSG']= "<font style='color:#F0A500'>"._("Warning")."</font>";
      $this->checks['users_visible']['ERROR_MSG'] = sprintf(_("Found %s user(s) that will not be visible in GOsa."), 
          count($this->users_to_migrate));
      $this->checks['users_visible']['ERROR_MSG'] .= "<input type='submit' name='users_visible_migrate' value='"._("Migrate")."...'>";
    }
  }


  /* Start user account migration 
   */  
  function migrate_gosaAccounts($only_ldif = FALSE)
  {
    $this->show_details= $only_ldif;

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Add gosaAccount objectClass to the selected users  
     */
    foreach($this->users_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get old objectClasses */
        $ldap->cat($dep['dn'],array("objectClass"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass array */
        $new_attrs  = array();
        $new_attrs['objectClass']= array("gosaAccount","inetOrgPerson","organizationalPerson");
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          if(!in_array_ics($attrs['objectClass'][$i], $new_attrs['objectClass'])){
            $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
          }
        }

        /* Set info attributes for current object, 
         *  or write changes to the ldap database 
         */
        if($only_ldif){
          $this->users_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->users_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            msg_dialog::display(_("Migration error"), sprintf(_("Cannot migrate department '%s':")."<br><br><i>%s</i>",LDAP::fix($attrs['dn']),$ldap->get_error()), ERROR_DIALOG);
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }


  /* Check if there are invisible organizational Units 
   */
  function check_organizationalUnits()
  {
    $cnt_ok = 0;
    $old = $this->deps_to_migrate;
    $this->deps_to_migrate = array();

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Skip GOsa internal departments */
    $skip_dns = array("/".$cv['peopleou']."/","/".$cv['groupou']."/","/^ou=people,/","/^ou=groups,/",
        "/(,|)ou=configs,/","/(,|)ou=systems,/",
        "/(,|)ou=apps,/","/(,|)ou=mime,/","/(,|)ou=devices/","/^ou=aclroles,/","/^ou=incoming,/",
        "/ou=snapshots,/","/(,|)dc=addressbook,/","/^(,|)ou=machineaccounts,/",
        "/(,|)ou=winstations,/");

    /* Get all invisible departments */
    $ldap->cd($cv['base']); 
    $res = $ldap->search("(&(objectClass=organizationalUnit)(!(objectClass=gosaDepartment)))",array("ou","description","dn"));
    while($attrs = $ldap->fetch()){
      $attrs['checked'] = FALSE;
      $attrs['before']  = "";
      $attrs['after']   = "";

      /* Set objects to selected, that were selected before reload */
      if(isset($old[base64_encode($attrs['dn'])])){
        $attrs['checked'] = $old[base64_encode($attrs['dn'])]['checked'];
      }
      $this->deps_to_migrate[base64_encode($attrs['dn'])] = $attrs;
    }

    /* Filter returned list of departments and ensure that 
     *  GOsa internal departments will not be listed 
     */
    foreach($this->deps_to_migrate as $key => $attrs){
      $dn = $attrs['dn'];
      $skip = false;;
      foreach($skip_dns as $skip_dn){
        if(preg_match($skip_dn,$dn)){
          $skip = true;
        }
      }
      if($skip){
        unset($this->deps_to_migrate[$key]);
      }
    }

    /* If we have no invisible departments found  
     *  tell the user that everything is ok 
     */
    if(!$res){
      $this->checks['deps_visible']['STATUS']    = FALSE;
      $this->checks['deps_visible']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['deps_visible']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }elseif(count($this->deps_to_migrate) == 0 ){
      $this->checks['deps_visible']['STATUS']    = TRUE;
      $this->checks['deps_visible']['STATUS_MSG']= _("Ok");
      $this->checks['deps_visible']['ERROR_MSG'] = "";
    }else{
      $this->checks['deps_visible']['STATUS']    = TRUE;
      $this->checks['deps_visible']['STATUS_MSG']= '<font style="color:#FFA500">'._("Warning").'</font>';
      $this->checks['deps_visible']['ERROR_MSG'] = sprintf(_("Found %s department(s) that will not be visible in GOsa."),count($this->deps_to_migrate));
      $this->checks['deps_visible']['ERROR_MSG'] .= "&nbsp;<input type='submit' name='deps_visible_migrate' value='"._("Migrate")."...'>";
    }
  }



  /* Start deparmtment migration */  
  function migrate_organizationalUnits($only_ldif = FALSE)
  {
    $this->show_details= $only_ldif;

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Add gosaDepartment objectClass to each selected entry 
     */
    foreach($this->deps_to_migrate as $key => $dep){
      if($dep['checked']){

        /* Get current objectClasses */
        $ldap->cat($dep['dn'],array("objectClass","description"));
        $attrs      = $ldap->fetch();

        /* Create new objectClass attribute including gosaDepartment*/
        $new_attrs  = array();
        for($i = 0 ; $i < $attrs['objectClass']['count']; $i ++ ){
          $new_attrs['objectClass'][]   = $attrs['objectClass'][$i];
        }
        $new_attrs['objectClass'][] = "gosaDepartment";

        /* Append description it is missing */
        if(!isset($attrs['description'])){
          $new_attrs['description'][] = "GOsa department";
        }

        /* Depending on the parameter >only_diff< we save the changes as ldif
         *  or we write our changes directly to the ldap database
         */
        if($only_ldif){
          $this->deps_to_migrate[$key]['before'] = $this->array_to_ldif($attrs);
          $this->deps_to_migrate[$key]['after']  = $this->array_to_ldif($new_attrs);
        }else{
          $ldap->cd($attrs['dn']);
          if(!$ldap->modify($new_attrs)){
            msg_dialog::display(_("Migration error"), sprintf(_("Cannot migrate department '%s':")."<br><br><i>%s</i>",LDAP::fix($attrs['dn']), $ldap->get_error()), ERROR_DIALOG);
            return(false);
          }
        }
      }
    }
    return(TRUE);
  }


  /* Check Acls if there is at least one object with acls defined 
   */
  function check_administrativeAccount()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($cv['base']);
    $res = $ldap->cat($cv['base']);
    
    if(!$res){
      $this->checks['acls']['STATUS']    = FALSE;
      $this->checks['acls']['STATUS_MSG']= _("LDAP query failed");
      $this->checks['acls']['ERROR_MSG'] = _("Possibly the 'root object' is missing.");
    }else{
      $found = false;
      $username = "";
      $attrs = $ldap->fetch();
      if(isset($attrs['gosaAclEntry'])){
        $acls = $attrs['gosaAclEntry'];
        for($i = 0 ; $i < $acls['count'] ; $i++){
          $acl = $acls[$i];
          $tmp = split(":",$acl);
          if($tmp[1] == "psub"){
            $members = split(",",$tmp[2]);
            foreach($members as $member){
              $member = base64_decode($member);

              /* Check if acl owner is a valid GOsa user account */
              $ldap->cat($member,array("objectClass","uid","cn"));
              $ret = $ldap->fetch();

              if(isset($ret['objectClass']) && in_array("posixGroup",$ret['objectClass'])){
                $found = TRUE;
                $username .= "ACL-Group:&nbsp;".$ret['cn'][0]."<br>";
              }elseif(isset($ret['objectClass']) && in_array("gosaAccount",$ret['objectClass']) &&
                  in_array("organizationalPerson",$ret['objectClass']) &&
                  in_array("inetOrgPerson",$ret['objectClass'])){
                $found = TRUE;
                $username .= "ACL:&nbsp;".$ret['uid'][0]."<br>";
              }
            }
          }elseif($tmp[1] == "role"){

            /* Check if acl owner is a valid GOsa user account */
            $ldap->cat(base64_decode($tmp[2]),array("gosaAclTemplate"));
            $ret = $ldap->fetch();

            if(isset($ret['gosaAclTemplate'])){
              $cnt = $ret['gosaAclTemplate']['count'];
              for($e = 0 ; $e < $cnt ; $e++){

                $a_str = $ret['gosaAclTemplate'][$e];
                if(preg_match("/^[0-9]*:psub:/",$a_str) && preg_match("/:all;cmdrw$/",$a_str)){

                  $members = split(",",$tmp[3]);
                  foreach($members as $member){
                    $member = base64_decode($member);

                    /* Check if acl owner is a valid GOsa user account */
                    $ldap->cat($member,array("objectClass","uid"));
                    $ret = $ldap->fetch();
  
                    if(isset($ret['objectClass']) && in_array("gosaAccount",$ret['objectClass']) &&
                        in_array("organizationalPerson",$ret['objectClass']) &&
                        in_array("inetOrgPerson",$ret['objectClass'])){
                      $found = TRUE;
                      $username .= "ACL Role:&nbsp;".$ret['uid'][0]."<br>";
                    }
                  }
                }
              }
            }
          }
        }
      }

      # For debugging
      #echo $username;

      if($found){
        $this->checks['acls']['STATUS']    = TRUE;
        $this->checks['acls']['STATUS_MSG']= _("Ok");
        $this->checks['acls']['ERROR_MSG'] = "";
      }else{
        $this->checks['acls']['STATUS']    = FALSE;
        $this->checks['acls']['STATUS_MSG']= _("Failed");
        $this->checks['acls']['ERROR_MSG']= _("There is no GOsa administrator account inside your LDAP.")."&nbsp;";
        $this->checks['acls']['ERROR_MSG'].= "<input type='submit' name='create_acls' value='"._("Create")."'>";
      }
    }
    return($ldap->count()>=1);
  }



  function create_admin($only_ldif = FALSE)
  {
    /* Reset '' */
    $this->acl_create_changes="";

    /* Object that should receive admin acls */
    $dn = $this->acl_create_selected;

    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;

    /* On first call check for rid/sid base */
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Get current base attributes */
    $ldap->cd($cv['base']);
    $ldap->cat($cv['base'],array("dn","objectClass","gosaAclEntry"));
    $attrs = $ldap->fetch();

    /* Add acls for the selcted user to the base */
    $attrs_new = array();
    $attrs_new['objectClass'] = array("gosaACL");

    for($i = 0; $i < $attrs['objectClass']['count']; $i ++){
      if(!in_array_ics($attrs['objectClass'][$i],$attrs_new['objectClass'])){
        $attrs_new['objectClass'][] = $attrs['objectClass'][$i];
      }
    }

    $acl = "0:psub:".base64_encode($dn).":all;cmdrw";    
    $attrs_new['gosaAclEntry'][] = $acl;
    if(isset($attrs['gosaAclEntry'])){
      for($i = 0 ; $i < $attrs['gosaAclEntry']['count']; $i ++){
          
        $prio = preg_replace("/[:].*$/","",$attrs['gosaAclEntry'][$i]);
        $rest = preg_replace("/^[^:]/","",$attrs['gosaAclEntry'][$i]);
 
        $data = ($prio+1).$rest;
        $attrs_new['gosaAclEntry'][] = $data;
      }
    }

    if($only_ldif){
      $this->acl_create_changes ="\n".($ldap->fix($cv['base']))."\n";
      $this->acl_create_changes.=$this->array_to_ldif($attrs)."\n";
      $this->acl_create_changes.="\n".($ldap->fix($cv['base']))."\n";
      $this->acl_create_changes.=$this->array_to_ldif($attrs_new);
    }else{
   
      $ldap->cd($cv['base']);
      if(!$ldap->modify($attrs_new)){
        msg_dialog::display(_("Migration error"), sprintf(_("Cannot add ACL for user '%s':")."<br><br><i>%s</i>", LDAP::fix($dn), $ldap->get_error()), ERROR_DIALOG);
        return(FALSE);
      }else{
        return(TRUE);
      }
    }
  }
 
  
  function create_admin_user()
  {
    $pw1 = $pw2 = "";
    $uid = "";
  
    if(isset($_POST['new_user_uid'])){
      $uid = $_POST['new_user_uid'];
    }
    if(isset($_POST['new_user_password'])){
      $pw1 = $_POST['new_user_password'];
    }
    if(isset($_POST['new_user_password2'])){
      $pw2 = $_POST['new_user_password2'];
    }
  
    if(empty($pw1) || empty($pw2) | ($pw1 != $pw2)){
      msg_dialog::display(_("Password error"), _("Provided passwords do not match!"), ERROR_DIALOG);
      return false;
    }
 
    if(!tests::is_uid($uid) || empty($uid)){
      msg_dialog::display(_("Input error"), _("Specify a valid user ID!"), ERROR_DIALOG);
      return false;
    }
    
    /* On first call check for rid/sid base */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Get current base attributes */
    $ldap->cd($cv['base']);
  
    $people_ou = trim($cv['peopleou']);
    if(!empty($people_ou)){
      $people_ou = trim($people_ou).",";
    }

    if($cv['peopledn'] == "cn"){
      $dn = "cn=System Administrator-".$uid.",".$people_ou.$cv['base'];
    }else{
      $dn = "uid=".$uid.",".$people_ou.$cv['base'];
    }

    $hash = passwordMethod::make_hash($pw2, $cv['encryption']);

    $new_user=array();
    $new_user['objectClass']= array("top","person","gosaAccount","organizationalPerson","inetOrgPerson");
    $new_user['givenName']  = "System";
    $new_user['sn']  = "Administrator";
    $new_user['cn']  = "System Administrator-".$uid;
    $new_user['uid'] = $uid;
    $new_user['userPassword'] = $hash;
   
    $ldap->cd($cv['base']);
  
    $ldap->cat($dn,array("dn"));
    if($ldap->count()){
      msg_dialog::display(_("Error"), sprintf(_("Adding an administrative user failed: object '%s' already exists!"), LDAP::fix($dn)), ERROR_DIALOG);
      return(FALSE);  
    }

    $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$dn));
    $ldap->cd($dn);  
    $res = $ldap->add($new_user);
    $this->acl_create_selected = $dn;
    $this->create_admin();
    
    if(!$res){
      msg_dialog::display(_("LDAP error"), $ldap->get_error(), ERROR_DIALOG);
      return(FALSE);
    }
  
    $this->acl_create_dialog=FALSE;        
    $this->check_administrativeAccount();
    return(TRUE);
  }
 

  function migrate_outside_winstations($perform = FALSE)
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    $ldap->cd($cv['base']);

    /* Check if there was a destination department posted */
    if(isset($_POST['move_winstation_to'])){
      $destination_dep = $_POST['move_winstation_to'];
    }else{
      msg_dialog::display(_("LDAP error"), _("Cannot move users to the requested department!"), ERROR_DIALOG);
      return(false);
    }
 
    foreach($this->outside_winstations as $b_dn => $data){
      $this->outside_winstations[$b_dn]['ldif'] ="";
      if($data['selected']){
        $dn = base64_decode($b_dn);
        $d_dn = preg_replace("/,.*$/",",".base64_decode($destination_dep),$dn);
        if(!$perform){
          $this->outside_winstations[$b_dn]['ldif'] = _("Winstation will be moved from").":<br>\t".($ldap->fix($dn))."<br>"._("to").":<br>\t".($ldap->fix($d_dn));


          /* Check if there are references to this object */
          $ldap->search("(&(member=".LDAP::prepare4filter($dn).")(|(objectClass=gosaGroupOfNames)(objectClass=groupOfNames)))",array('dn'));
          $refs = "";
          while($attrs = $ldap->fetch()){
            $ref_dn = $attrs['dn'];
            $refs .= "<br />\t".$ref_dn;
          } 
          if(!empty($refs)){ 
            $this->outside_winstations[$b_dn]['ldif'] .= "<br /><br /><i>"._("Updating following references too").":</i>".$refs;
          }

        }else{
          $this->move($dn,$d_dn);
        }
      }
    }
  }
  

  function migrate_outside_groups($perform = FALSE)
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($cv['base']);

    /* Check if there was a destination department posted */
    if(isset($_POST['move_group_to'])){
      $destination_dep = $_POST['move_group_to'];
    }else{
      msg_dialog::display(_("LDAP error"), _("Cannot move users to the requested department!"), ERROR_DIALOG);
      return(false);
    }
 
    foreach($this->outside_groups as $b_dn => $data){
      $this->outside_groups[$b_dn]['ldif'] ="";
      if($data['selected']){
        $dn = base64_decode($b_dn);
        $d_dn = preg_replace("/,.*$/",",".base64_decode($destination_dep),$dn);
        if(!$perform){

          $this->outside_groups[$b_dn]['ldif'] = _("Group will be moved from").":<br>\t".($ldap->fix($dn))."<br>"._("to").":<br>\t".($ldap->fix($d_dn));

          /* Check if there are references to this object */
          $ldap->search("(&(member=".LDAP::prepare4filter($dn).")(|(objectClass=gosaGroupOfNames)(objectClass=groupOfNames)))",array('dn'));
          $refs = "";
          while($attrs = $ldap->fetch()){
            $ref_dn = $attrs['dn'];
            $refs .= "<br />\t".$ref_dn;
          } 
          if(!empty($refs)){ 
            $this->outside_groups[$b_dn]['ldif'] .= "<br /><br /><i>"._("Updating following references too").":</i>".$refs;
          }

        }else{
          $this->move($dn,$d_dn);
        }
      }
    }
  }
  

  function migrate_outside_users($perform = FALSE)
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($cv['base']);

    /* Check if there was a destination department posted */
    if(isset($_POST['move_user_to'])){
      $destination_dep = $_POST['move_user_to'];
    }else{
      msg_dialog::display(_("LDAP error"), _("Cannot move users to the requested department!"), ERROR_DIALOG);
      return(false);
    }
      
    foreach($this->outside_users as $b_dn => $data){
      $this->outside_users[$b_dn]['ldif'] ="";
      if($data['selected']){
        $dn = base64_decode($b_dn);
        $d_dn = preg_replace("/,.*$/",",".base64_decode($destination_dep),$dn);
        if(!$perform){
          $this->outside_users[$b_dn]['ldif'] = _("User will be moved from").":<br>\t".($ldap->fix($dn))."<br>"._("to").":<br>\t".($ldap->fix($d_dn));

          /* Check if there are references to this object */
          $ldap->search("(&(member=".LDAP::prepare4filter($dn).")(|(objectClass=gosaGroupOfNames)(objectClass=groupOfNames)))",array('dn'));
          $refs = "";
          while($attrs = $ldap->fetch()){
            $ref_dn = $attrs['dn'];
            $refs .= "<br />\t".$ref_dn;
          } 
          if(!empty($refs)){ 
            $this->outside_users[$b_dn]['ldif'] .= "<br /><br /><i>"._("The following references will be updated").":</i>".$refs;
          }

        }else{
          $this->move($dn,$d_dn);
        }
      }
    }
  }
  

  function execute()
  {
    /* Initialise checks if this is the first call */
    if(!$this->checks_initialised || isset($_POST['reload'])){
      $this->initialize_checks();
      $this->checks_initialised = TRUE;
    }

    /*************
     * Winstations outside the group ou 
     *************/
    
    if(isset($_POST['outside_winstations_dialog_cancel'])){
      $this->outside_winstations_dialog = FALSE;
      $this->dialog = FALSE;
      $this->show_details = FALSE;
    }
   
    if(isset($_POST['outside_winstations_dialog_whats_done'])){
      $this->migrate_outside_winstations(FALSE);
    }
 
    if(isset($_POST['outside_winstations_dialog_perform'])){
      $this->migrate_outside_winstations(TRUE);
      $this->search_outside_winstations();
      $this->dialog = FALSE;
      $this->show_details = FALSE;
      $this->outside_winstations_dialog = FALSE;
    }

    if(isset($_POST['outside_winstations_dialog'])){
      $this->outside_winstations_dialog = TRUE;
      $this->dialog = TRUE;
    }
    
    if($this->outside_winstations_dialog){

      /* Fix displayed dn syntax */ 
      $tmp = $this->outside_winstations;
      foreach($tmp as $key => $data){
        $tmp[$key]['dn'] = @LDAP::fix($data['dn']);
      }

      $smarty = get_smarty();
      $smarty->assign("ous",$this->get_all_winstation_ous());
      $smarty->assign("method","outside_winstations");
      $smarty->assign("outside_winstations",$tmp);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }
    /*************
     * Groups outside the group ou 
     *************/
    
    if(isset($_POST['outside_groups_dialog_cancel'])){
      $this->outside_groups_dialog = FALSE;
      $this->show_details = FALSE;
      $this->dialog = FALSE;
    }
   
    if(isset($_POST['outside_groups_dialog_whats_done'])){
      $this->show_details= TRUE;
      $this->migrate_outside_groups(FALSE);
    }
 
    if(isset($_POST['outside_groups_dialog_refresh'])){
      $this->show_details= FALSE;
    }

    if(isset($_POST['outside_groups_dialog_perform'])){
      $this->migrate_outside_groups(TRUE);
      $this->dialog = FALSE;
      $this->show_details = FALSE;
      $this->outside_groups_dialog = FALSE;
      $this->initialize_checks();
    }

    if(isset($_POST['outside_groups_dialog'])){
      $this->outside_groups_dialog = TRUE;
      $this->dialog = TRUE;
    }
    
    if($this->outside_groups_dialog){

      /* Fix displayed dn syntax */ 
      $tmp = $this->outside_groups;
      foreach($tmp as $key => $data){
        $tmp[$key]['dn'] = @LDAP::fix($data['dn']);
      }

      $smarty = get_smarty();
      $smarty->assign("ous",$this->get_all_group_ous());
      $smarty->assign("method","outside_groups");
      $smarty->assign("outside_groups",$tmp);
      $smarty->assign("group_details", $this->show_details);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }
 
    /*************
     * User outside the people ou 
     *************/
    
    if(isset($_POST['outside_users_dialog_cancel'])){
      $this->outside_users_dialog = FALSE;
      $this->dialog = FALSE;
      $this->show_details = FALSE;
    }
   
    if(isset($_POST['outside_users_dialog_whats_done'])){
      $this->show_details= TRUE;
      $this->migrate_outside_users(FALSE);
    }
 
    if(isset($_POST['outside_users_dialog_perform'])){
      $this->migrate_outside_users(TRUE);
      $this->initialize_checks();
      $this->dialog = FALSE;
      $this->show_details = FALSE;
      $this->outside_users_dialog = FALSE;
    }

    if (isset($_POST['outside_users_dialog_refresh'])){
      $this->show_details= FALSE;
    }

    if(isset($_POST['outside_users_dialog'])){
      $this->outside_users_dialog = TRUE;
      $this->dialog = TRUE;
    }
    
    if($this->outside_users_dialog){

      /* Fix displayed dn syntax */ 
      $tmp = $this->outside_users;
      foreach($tmp as $key => $data){
        $tmp[$key]['dn'] = @LDAP::fix($data['dn']);
      }

      $smarty = get_smarty();
      $smarty->assign("ous",$this->get_all_people_ous());
      $smarty->assign("method","outside_users");
      $smarty->assign("outside_users",$tmp);
      $smarty->assign("user_details", $this->show_details);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }
 
    /*************
     * Root object check  
     *************/
  
    if(isset($_POST['retry_root_create'])){

      $state = $this->checks['root']['STATUS'];
      $this->checkBase(FALSE);
      if($state != $this->checks['root']['STATUS']){
        $this->initialize_checks();
      }
    }
 
    /*************
     * User Migration handling 
     *************/

    if(isset($_POST['retry_acls'])){
      $this->check_administrativeAccount();
    }

    if(isset($_POST['create_acls'])){
      $this->acl_create_dialog = TRUE;
      $this->dialog = TRUE;
    }
  
    if(isset($_POST['create_acls_cancel'])){
      $this->acl_create_dialog = FALSE;
      $this->dialog = FALSE;
      $this->show_details = FALSE;
    }

#    if(isset($_POST['create_acls_create_confirmed'])){
#      if($this->create_admin()){
#        $this->acl_create_dialog = FALSE;
#        $this->dialog = FALSE;
#      $this->show_details = FALSE;
#        $this->initialize_checks();
#      }
#    }

    if(isset($_POST['create_acls_create'])){
      $this->create_admin(TRUE);
    }

    if(isset($_POST['create_admin_user'])){
      if($this->create_admin_user()){
        $this->dialog = FALSE;
      $this->show_details = FALSE;
      }
    }

    if($this->acl_create_dialog){
      $smarty = get_smarty();
  
      $uid = "admin";
      if(isset($_POST['new_user_uid'])){
        $uid = $_POST['new_user_uid'];
      }

      $smarty->assign("new_user_uid",$uid);
      $smarty->assign("new_user_password",@$_POST['new_user_password']);
      $smarty->assign("new_user_password2",@$_POST['new_user_password2']);
      $smarty->assign("method","create_acls");
      $smarty->assign("acl_create_selected",$this->acl_create_selected);
      $smarty->assign("what_will_be_done_now",$this->acl_create_changes);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    /*************
     * User Migration handling 
     *************/

    /* Refresh list of deparments */
    if(isset($_POST['users_visible_migrate_refresh'])){
      $this->check_gosaAccounts();
    }

    /* Open migration dialog */
    if(isset($_POST['users_visible_migrate'])){
      $this->show_details= FALSE;
      $this->users_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['users_visible_migrate_close'])){
      $this->users_migration_dialog = FALSE;
      $this->dialog =FALSE;
      $this->show_details = FALSE;
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_migrate'])){
      if($this->migrate_gosaAccounts()){
        $this->initialize_checks();
        $this->dialog = FALSE;
        $this->show_details = FALSE;
        $this->users_migration_dialog = FALSE;
      }
    }

    /* Start migration */
    if(isset($_POST['users_visible_migrate_whatsdone'])){
      $this->migrate_gosaAccounts(TRUE);
    }

    /* Display migration dialog */
    if($this->users_migration_dialog){

      /* Fix displayed dn syntax */ 
      $tmp = $this->users_to_migrate;
      foreach($tmp as $key => $data){
        $tmp[$key]['dn'] = @LDAP::fix($data['dn']);
      }

      $smarty = get_smarty();
      $smarty->assign("users_to_migrate",$tmp);
      $smarty->assign("method","migrate_users");
      $smarty->assign("user_details", $this->show_details);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }


    /*************
     * Department Migration handling 
     *************/

    /* Refresh list of deparments */
    if(isset($_POST['deps_visible_migrate_refresh'])){
      $this->check_organizationalUnits();
      $this->show_details= FALSE;
    }

    /* Open migration dialog */
    if(isset($_POST['deps_visible_migrate'])){
      $this->dep_migration_dialog = TRUE;
      $this->dialog =TRUE;
    }

    /* Close migration dialog */
    if(isset($_POST['deps_visible_migrate_close'])){
      $this->dep_migration_dialog = FALSE;
      $this->dialog =FALSE;
      $this->show_details = FALSE;
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_migrate'])){
      if($this->migrate_organizationalUnits()){
        $this->show_details= FALSE;
        $this->check_organizationalUnits();
        $this->dialog = FALSE;
        $this->dep_migration_dialog = FALSE;
      }
    }

    /* Start migration */
    if(isset($_POST['deps_visible_migrate_whatsdone'])){
      $this->migrate_organizationalUnits(TRUE);
    }

    /* Display migration dialog */
    if($this->dep_migration_dialog){
      $smarty = get_smarty();
   
      /* Fix displayed dn syntax */ 
      $tmp = $this->deps_to_migrate;
      foreach($tmp as $key => $data){
        $tmp[$key]['dn'] = @LDAP::fix($data['dn']);
      }

      $smarty->assign("deps_to_migrate",$tmp);
      $smarty->assign("method","migrate_deps");
      $smarty->assign("deps_details", $this->show_details);
      return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
    }

    $smarty = get_smarty();
    $smarty->assign("checks",$this->checks);
    $smarty->assign("method","default");
    return($smarty->fetch(get_template_path("setup_migrate.tpl",TRUE,dirname(__FILE__))));
  }


  function save_object()
  {
    $this->is_completed= TRUE;

    /* Capture all selected winstations from outside_winstations_dialog */
    if($this->outside_winstations_dialog){
      foreach($this->outside_winstations as $dn => $data){
        if(isset($_POST['select_winstation_'.$dn])){
          $this->outside_winstations[$dn]['selected'] = TRUE;
        }else{
          $this->outside_winstations[$dn]['selected'] = FALSE;
        }
      }
    }

    /* Capture all selected groups from outside_groups_dialog */
    if($this->outside_groups_dialog){
      foreach($this->outside_groups as $dn => $data){
        if(isset($_POST['select_group_'.$dn])){
          $this->outside_groups[$dn]['selected'] = TRUE;
        }else{
          $this->outside_groups[$dn]['selected'] = FALSE;
        }
      }
    }

    /* Capture all selected users from outside_users_dialog */
    if($this->outside_users_dialog){
      foreach($this->outside_users as $dn => $data){
        if(isset($_POST['select_user_'.$dn])){
          $this->outside_users[$dn]['selected'] = TRUE;
        }else{
          $this->outside_users[$dn]['selected'] = FALSE;
        }
      }
    }

    /* Get "create acl" dialog posts */
    if($this->acl_create_dialog){

      if(isset($_POST['create_acls_create_abort'])){
        $this->acl_create_selected = "";
      }
    }

    /* Get selected departments */
    if($this->dep_migration_dialog){
      foreach($this->deps_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->deps_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->deps_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }

    /* Get selected users */
    if($this->users_migration_dialog){
      foreach($this->users_to_migrate as $id => $data){
        if(isset($_POST['migrate_'.$id])){
          $this->users_to_migrate[$id]['checked'] = TRUE;
        }else{
          $this->users_to_migrate[$id]['checked'] = FALSE;
        }
      }
    }
  }


  /* Check if the root object exists.
   * If the parameter just_check is true, then just check if the 
   *  root object is missing and update the info messages.
   * If the Parameter is false, try to create a new root object.
   */
  function checkBase($just_check = TRUE)
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Check if root object exists */
    $ldap->cd($cv['base']);
    $ldap->set_size_limit(1);
    $res = $ldap->search("(objectClass=*)");
    $ldap->set_size_limit(0);
    $err = ldap_errno($ldap->cid); 

    if( !$res || 
        $err == 0x20 ||  # LDAP_NO_SUCH_OBJECT
        $err == 0x40) {  # LDAP_NAMING_VIOLATION

      /* Root object doesn't exists 
       */
      if($just_check){
        $this->checks['root']['STATUS']    = FALSE;
        $this->checks['root']['STATUS_MSG']= _("Failed");
        $this->checks['root']['ERROR_MSG'] =  _("The LDAP root object is missing. It is required to use your LDAP service.").'&nbsp;';
        $this->checks['root']['ERROR_MSG'].=  "<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
        return(FALSE);
      }else{

        /* Add root object */ 
        $ldap->cd($cv['base']);
        $res = $ldap->create_missing_trees($cv['base']);

        /* If adding failed, tell the user */
        if(!$res){
          $this->checks['root']['STATUS']    = FALSE;
          $this->checks['root']['STATUS_MSG']= _("Failed");
          $this->checks['root']['ERROR_MSG'] = _("Root object couldn't be created, you should try it on your own.");
          $this->checks['root']['ERROR_MSG'].= "&nbsp;<input type='submit' name='retry_root_create' value='"._("Try to create root object")."'>";
          return($res);;
        }
      }
    }

    /* Create & remove of dummy object was successful */
    $this->checks['root']['STATUS']    = TRUE;
    $this->checks['root']['STATUS_MSG']= _("Ok");
  }


  /* Return ldif information for a 
   * given attribute array 
   */
  function array_to_ldif($atts)
  {
    $ret = "";
    unset($atts['count']);
    unset($atts['dn']);
    foreach($atts as $name => $value){
      if(is_numeric($name)) {
        continue;
      }
      if(is_array($value)){
        unset($value['count']);
        foreach($value as $a_val){
          $ret .= $name.": ". $a_val."\n";
        }
      }else{
        $ret .= $name.": ". $value."\n";
      }
    }
    return(preg_replace("/\n$/","",$ret));
  }


  function get_user_list()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    $ldap->cd($cv['base']);
    $ldap->search("(objectClass=gosaAccount)",array("dn"));
  
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[base64_encode($attrs['dn'])] = @LDAP::fix($attrs['dn']);
    }
    return($tmp);
  }


 function get_all_people_ous()
  {
    /* Get collected configuration settings */
    $cv = $this->parent->captured_values;
    $people_ou = trim($cv['peopleou']);

    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /*****************
     * If people ou is NOT empty
     * search for for all objects matching the given container
     *****************/
    if(!empty($people_ou)){
      $ldap->search("(".$people_ou.")",array("dn"));

      /* Create people ou if there is currently none */
      if($ldap->count() == 0 ){
        $add_dn = $cv['peopleou'].",".$cv['base'];
        $naming_attr = preg_replace("/=.*$/","",$add_dn);
        $naming_value = preg_replace("/^[^=]*+=([^,]*).*$/","\\1",$add_dn);
        $add = array();
        $add['objectClass'] = array("organizationalUnit");
        $add[$naming_attr] = $naming_value;
        $ldap->cd($cv['base']);
        $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$add_dn));
        $ldap->cd($add_dn);
        $ldap->add($add);
      }

      /* Create result */
      $ldap->search("(".$cv['peopleou'].")",array("dn"));
      $tmp = array();
      while($attrs= $ldap->fetch()){
        if(!preg_match("/ou=snapshots,/",$attrs['dn'])){
          $tmp[base64_encode($attrs['dn'])] = $ldap->fix($attrs['dn']);
        }
      }
    } else{

      /************
       * If people ou is empty
       * Get all valid gosaDepartments
       ************/
      $ldap->cd($cv['base']);
      $tmp = array();
      $ldap->search("(&(objectClass=gosaDepartment)(ou=*))",array("dn"));
      $tmp[base64_encode($cv['base'])] = $ldap->fix($cv['base']);
      while($attrs = $ldap->fetch()){
        $tmp[base64_encode($attrs['dn'])] = $ldap->fix($attrs['dn']);;
      }
    }
    return($tmp);
  }


  function get_all_winstation_ous()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    /* Get winstation ou */
    if($cv['generic_settings']['wws_ou_active']) {
      $winstation_ou = $cv['generic_settings']['ws_ou'];
    }else{
      $winstation_ou = "ou=winstations";
    }

    $ldap->cd($cv['base']);
    $ldap->search("(".$winstation_ou.")",array("dn"));
  
    if($ldap->count() == 0 ){
      $add_dn = $winstation_ou.",ou=systems,".$cv['base'];
      $naming_attr = preg_replace("/=.*$/","",$add_dn);
      $naming_value = preg_replace("/^[^=]*+=([^,]*).*$/","\\1",$add_dn);
      $add = array();
      $add['objectClass'] = array("organizationalUnit");
      $add[$naming_attr] = $naming_value;

      $ldap->cd($cv['base']);
      $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$add_dn));
      $ldap->cd($add_dn);
      $ldap->add($add);
    }

    $ldap->search("(".$winstation_ou.")",array("dn"));
    $tmp = array();
    while($attrs= $ldap->fetch()){
      if(!preg_match("/ou=snapshots,/",$attrs['dn'])){
        $tmp[base64_encode($attrs['dn'])] = $ldap->fix($attrs['dn']);
      }
    }
    return($tmp); 
  }


 function get_all_group_ous()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

    $group_ou = trim($cv['groupou']);
    if(!empty($group_ou)){
      $group_ou = trim($group_ou);
    }

    /************
     * If group ou is NOT empty
     * Get all valid group ous, create one if necessary
     ************/
    $ldap->cd($cv['base']);
    if(!empty($group_ou)){
      $ldap->search("(".$group_ou.")",array("dn"));
      if($ldap->count() == 0 ){
        $add_dn = $group_ou.$cv['base'];
        $naming_attr = preg_replace("/=.*$/","",$add_dn);
        $naming_value = preg_replace("/^[^=]*+=([^,]*).*$/","\\1",$add_dn);
        $add = array();
        $add['objectClass'] = array("organizationalUnit");
        $add[$naming_attr] = $naming_value;

        $ldap->cd($cv['base']);
        $ldap->create_missing_trees(preg_replace("/^[^,]+,/","",$add_dn));
        $ldap->cd($add_dn);
        $ldap->add($add);
      }
      $ldap->search("(".$group_ou.")",array("dn"));
      $tmp = array();
      while($attrs= $ldap->fetch()){
        if(!preg_match("/ou=snapshots,/",$attrs['dn'])){
          $tmp[base64_encode($attrs['dn'])] = $ldap->fix($attrs['dn']);
        }
      }
    }else{
      /************
       * If group ou is empty
       * Get all valid gosaDepartments
       ************/
      $ldap->cd($cv['base']);
      $tmp = array();
      $ldap->search("(&(objectClass=gosaDepartment)(ou=*))",array("dn"));
      $tmp[base64_encode($cv['base'])] = $ldap->fix($cv['base']);
      while($attrs = $ldap->fetch()){
        $tmp[base64_encode($attrs['dn'])] = $ldap->fix($attrs['dn']);;
      }
    }
    return($tmp);
  }


  function get_group_list()
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);
    
    $ldap->cd($cv['base']);
    $ldap->search("(objectClass=posixGroup)",array("dn"));
  
    $tmp = array();
    while($attrs = $ldap->fetch()){
      $tmp[base64_encode($attrs['dn'])] = @LDAP::fix($attrs['dn']);
    }
    return($tmp);
  }


  function move($source,$destination)
  {
    /* Establish ldap connection */
    $cv = $this->parent->captured_values;
    $ldap_l = new LDAP($cv['admin'],
        $cv['password'],
        $cv['connection'],
        FALSE,
        $cv['tls']);

    $ldap = new ldapMultiplexer($ldap_l);

     /* Update object references in gosaGroupOfNames */
    $ogs_to_fix = array();
    $ldap->cd($cv['base']);
    $ldap->search('(&(objectClass=gosaGroupOfNames)(member='.@LDAP::prepare4filter($source).'))', array('cn','member'));
    while ($attrs= $ldap->fetch()){
      $dn = $attrs['dn'];
      $attrs = $this->cleanup_array($attrs);
      $member_new = array($destination);
      foreach($attrs['member'] as $member){
        if($member != $source){
          $member_new[] = $member;
        }
      }
      $attrs['member'] = $member_new;
      $ogs_to_fix[$dn] = $attrs;
    }

    /* Copy source to destination dn */
    $ldap->cat($source);
    $new_data = $this->cleanup_array($ldap->fetch());
    $ldap->cd($destination);
    $res = $ldap->add($new_data);

    /* Display warning if copy failed */
    if(!$res){
      msg_dialog::display(_("LDAP error"), sprintf(_("Copy '%s' to '%s' failed:")."<br><br><i>%s</i>", LDAP::fix($source), LDAP::fix($destination), $ldap->get_error()), ERROR_DIALOG);
    }else{
      $res = $ldap->rmDir($source);
      if (!$ldap->success()){
        msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $source, LDAP_DEL, get_class()));
      }

      /* Object is copied, so update its references */
      foreach($ogs_to_fix as $dn => $data){
        $ldap->cd($dn);
        $ldap->modify($data);
      }
    }
  }

  
  /* Cleanup ldap result to be able to write it be to ldap */
  function cleanup_array($attrs)
  {
    foreach($attrs as $key => $value) {
      if(is_numeric($key) || in_array($key,array("count","dn"))){
        unset($attrs[$key]);
      }
      if(is_array($value) && isset($value['count'])){
        unset($attrs[$key]['count']);
      }
    }
    return($attrs);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
