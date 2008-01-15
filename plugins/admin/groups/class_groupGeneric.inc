<?php

class group extends plugin
{
  /* Group attributes */
  var $cn= "";
  var $description= "";
  var $gidNumber= "";
  var $memberUid= array();

  /* Helpers */
  var $base= "";
  var $force_gid= FALSE;
  var $fon_group= FALSE;
  var $smbgroup= FALSE;
  var $groupType= FALSE;
  var $samba3= FALSE;
  var $sambaSID= "";
  var $sambaDomainName= "DEFAULT";
  var $SID= "";
  var $ridBase= 0;
  var $members= array();
  var $users= array();
  var $member= array();
  var $allusers= array();
  var $department= "";
  var $saved_gidNumber= "";
  var $savedSID= "";
  var $oldgroupType= "";
  var $orig_dn= "";
  var $orig_cn= "";
  var $has_mailAccount= FALSE;
  var $group_dialog= FALSE;
  var $nagios_group =FALSE;
  var $sambaGroupType;
  var $dialog;
  var $rfc2307bis= FALSE;
  var $OnlyShowFirstEntries =200;
  var $dnMapping= array();

  var $allowGroupsWithSameNameInOtherSubtrees = true;

  var $CopyPasteVars          = array("force_gid","fon_group","smbgroup","groupType","sambaSID","sambaDomainName","SID","nagios_group","sambaGroupType");

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gidNumber","memberUid","sambaGroupType","sambaSID");
  var $objectclasses= array("top", "posixGroup");

  function group ($config, $dn= NULL, $parent= NULL)
  {
    /* Set rfc2307bis flag */
    if (isset($config->current['RFC2307BIS']) && ($config->current['RFC2307BIS']== "true")){
      $this->rfc2307bis= TRUE;
      $this->attributes[]= "member";
      $this->objectclasses[]= "groupOfNames";
    }

    plugin::plugin ($config, $dn, $parent);

    /* Load attributes depending on the samba version */
    $this->samba3= ($config->current['SAMBAVERSION'] == 3);
    $this->orig_dn= $dn;
    $this->orig_cn= $this->cn;

    /* Get member list */
    if (isset($this->attrs['memberUid'][0])){
      $tmp= array();
      for ($i= 0; $i<$this->attrs['memberUid']['count']; $i++){
        $tmp[]= $this->attrs['memberUid'][$i];
      }
      $this->memberUid= $tmp;
      sort ($this->memberUid);
    }

    /* Save gidNumber for later use */
    if (isset($this->attrs['gidNumber'])){
      $this->saved_gidNumber= $this->attrs['gidNumber'][0];
    }

    /* Is a samba group? */
    if (isset($this->attrs['objectClass'])){
      if (array_search ('sambaGroupMapping', $this->attrs['objectClass']) == NULL ){
        $this->smbgroup= FALSE;
      } else {
        $this->smbgroup= TRUE;
        if (isset($this->attrs['sambaSID'])){
          $this->sambaSID= $this->attrs['sambaSID'][0];
        }
      }
      if (array_search ('goFonPickupGroup', $this->attrs['objectClass']) == NULL ){
        $this->fon_group= FALSE;
      } else {
        $this->fon_group= TRUE;
      }
      if (array_search ('nagiosContactGroup', $this->attrs['objectClass']) == NULL ){
        $this->nagios_group= FALSE;
      } else {
        $this->nagios_group= TRUE;
      }
    }

    /* Set mail flag */
    if (isset($this->attrs['objectClass']) && in_array('gosaMailAccount', $this->attrs['objectClass'])){
      $this->has_mailAccount= TRUE;
    }

    /* Get samba Domain in case of samba 3 */
    if ($this->samba3 && $this->sambaSID != ""){
      $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
      $this->savedSID= $this->SID;
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase"));
      if ($ldap->count() != 0){
        $attrs= $ldap->fetch();
        $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];

        /* Get domain name for SID */
        $this->sambaDomainName= "DEFAULT";
        foreach ($this->config->data['SERVERS']['SAMBA'] as $key => $val){
          if ($val['SID'] == $this->SID){
            $this->sambaDomainName= $key;
            break;
          }
        }
      } else {
        if (isset($this->config->current['RIDBASE'])){
          $this->sambaDomainName= "DEFAULT";
          $this->ridBase= $this->config->current['RIDBASE'];
          $this->SID= $this->config->current['SID'];
        } else {
          print_red(_("Can't find this groups SID in LDAP or in your configuration file!"));
        }
      }

      /* Get group type */
      $this->groupType= (int)substr(strrchr($this->sambaSID, "-"), 1);
      if ($this->groupType < 500 || $this->groupType > 553){
        $this->groupType= 0;
      }
      $this->oldgroupType= $this->groupType;
    }

    /* Get global filter config */
    if (!is_global("gufilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $gufilter= array( "dselect"       => $base,
          "regex"           => "*");
      register_global("gufilter", $gufilter);
    }
    $gufilter= get_global('gufilter');

    $gufilter['SubSearchGroup'] = false;
    $gufilter['dselect'] = $_SESSION['gufilter']['dselect'];  

    register_global('gufilter',$gufilter);
  
    if ($this->dn == "new"){
      if(isset($_SESSION['CurrentMainBase'])){
        $this->base= $_SESSION['CurrentMainBase'];
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {

      /* Get object base */
      $this->base =preg_replace ("/^[^,]+,".normalizePreg(get_groups_ou())."/","",$this->dn);
    }

    /* This is always an account */
    $this->is_account= TRUE;
    $this->reload();
  }

  function execute()
  {
	/* Call parent execute */
	plugin::execute();

  $ui= get_userinfo();
  $acla= get_permissions ($ui->dn, $ui->subtreeACL);
  $this->acl= get_module_permission($acla, "group", $ui->dn);
  /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no group.")."</b>";
      return ($display);
    }

    /* Delete user from group */
    if (isset($_POST['del_users']) && isset($_POST['members']) && chkacl($this->acl,"memberUid") ==""){
      foreach ($_POST['members'] as $value){
        unset ($this->members["$value"]);
        $this->removeUser($value);
      }
      $this->reload();
    }

    /* Add objects? */
    if (isset($_POST["edit_membership"]) && chkacl($this->acl,"memberUid") ==""){
      $this->group_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Add objects finished? */
    if (isset($_POST["add_users_finish"]) || isset($_POST["add_users_cancel"])){
      $this->group_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Add user to group */
    if (isset($_POST['add_users_finish']) && isset($_POST['users'])){
      foreach ($_POST['users'] as $value){
        $this->members["$value"]= $this->allusers[$value];
        asort($this->members);
        $this->addUser($value);
      }
      $this->reload();
    }

    /* Base select dialog */
    $once = true;
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
        $this->base = $this->dialog->isSelected();
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

   /* Assign templating stuff */
    $smarty= get_smarty();
    if ($this->samba3){
      $smarty->assign("samba3", "true");
    } else {
      $smarty->assign("samba3", "");
    }

    if(search_config($this->config->data['MENU'], "nagiosaccount", "CLASS")){
      $smarty->assign("nagios",true);
    }else{
      $smarty->assign("nagios",false);
    }
    
    if(search_config($this->config->data['MENU'], "phoneAccount", "CLASS")){
      $smarty->assign("pickupGroup",true);
    }else{
      $smarty->assign("pickupGroup",false);
    }

    /* Manage object add dialog */
    if ($this->group_dialog){

      /* Save data */
      $gufilter= get_global("gufilter");
      foreach( array("dselect", "regex") as $type){
        if (isset($_POST[$type])){
          $gufilter[$type]= $_POST[$type];
        }
      }
      if(isset($_POST['regex'])){
        if(isset($_POST['SubSearchGroup'])){
          $gufilter['SubSearchGroup'] = true;
        }else{
          $gufilter['SubSearchGroup'] = false;
        }
      }

      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $gufilter['regex']= $s;
      }
      register_global("gufilter", $gufilter);
      $this->reload();

      /* Show dialog */
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("dselect", "regex","SubSearchGroup") as $type){
        $smarty->assign("$type", $gufilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("users", $this->displayUsers);
      $smarty->assign("apply", apply_filter());
      $display= $smarty->fetch (get_template_path('group_objects.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Bases / Departments */
    if (isset($_POST['base'])){
      $this->base= $_POST['base'];
    }

    $smarty->assign("bases", $this->config->idepartments);
    $smarty->assign("base_select", $this->base);
    $smarty->assign("department", $this->department);

    if ($this->samba3){
      $domains= array();
      foreach($this->config->data['SERVERS']['SAMBA'] as $name => $content){
        $domains[$name]= $name;
      }
      $smarty->assign("sambaDomains", $domains);
      $smarty->assign("sambaDomainName", $this->sambaDomainName);
      $groupTypes= array(0 => _("Samba group"), 512 => _("Domain admins"), 513 => _("Domain users"),
          514 => _("Domain guests"));

      /* Don't loose special groups! If not key'ed above, add it to
         the combo box... */	
      if ($this->groupType >= 500 && $this->groupType <= 553 && !isset($groupTypes[$this->groupType])){
        $groupTypes[$this->groupType]= sprintf(_("Special group (%d)"), $this->groupType);
      }

      $smarty->assign("groupTypes", $groupTypes);
      $smarty->assign("groupType", $this->groupType);
    }

    /* Members and users */
    $smarty->assign("members", $this->members);

    /* Checkboxes */
    foreach (array("force_gid", "smbgroup") as $val){
      if ($this->$val == "1"){
        $smarty->assign("$val", "checked");
      } else {
        $smarty->assign("$val", "");
      }
    }
    if ($this->force_gid != "1"){
      $smarty->assign("forceMode", "disabled");
    }else{
      $smarty->assign("forceMode", "");
    }
    $smarty->assign("force_gidACL", chkacl($this->acl, "gidNumber"));
    $smarty->assign("sambaDomainNameACL", chkacl($this->acl, "sambaDomainName"));
    if ($this->fon_group){
      $smarty->assign("fon_group", "checked");
    } else {
      $smarty->assign("fon_group", "");
    }
    $smarty->assign("fon_groupACL", chkacl($this->acl, "fon_group"));

    if ($this->nagios_group){
      $smarty->assign("nagios_group", "checked");
    } else {
      $smarty->assign("nagios_group", "");
    }
    $smarty->assign("nagios_groupACL", chkacl($this->acl, "nagios_group"));

    /* Fields */
    foreach (array("cn", "description", "gidNumber") as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    /* Missing ACL's */
    foreach (array("base", "smbgroup", "members") as $val){
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    /* Show main page */
    $smarty->assign("alphabet", generate_alphabet(10));
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("deplist", $this->config->idepartments);
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function addUser($uid)
  {
    $this->memberUid[]= $uid;
    $this->memberUid= array_unique($this->memberUid);
  }

  function removeUser($uid)
  {
    $temp= array();
    foreach ($this->memberUid as $value){
      if ($value != $uid){
        $temp[]= $value;
      }
    }
    $this->memberUid= $temp;
  }


  /* Reload data */
  function reload()
  {
    /* Fix regex string */
    $gufilter = get_global("gufilter");
    $regex    = normalizeLdap($gufilter['regex']);
    $MaxUser  = $this->OnlyShowFirstEntries;

    /* Prepare ldap link */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($gufilter['dselect']);


    /* Resolve still unresolved memberuids to fill the list with sn/giveName attributes 
        (Store gathered sn/givenName informations in $this->allusers too, 
         to be prepared when adding/deleting users)
     */    
    $filter = "";
    foreach ($this->memberUid as $value){
      if(!isset($this->members[$value])){
        $filter .= "(uid=".normalizeLdap($value).")";
      }
    }
    if(!empty($filter)){    
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(|".$filter."))",array("dn", "uid","sn","givenName"));
      while($attrs = $ldap->fetch()){
        $this->dnMapping[$attrs['uid'][0]] = $attrs['dn'];
        $this->members[$attrs['uid'][0]] = $this->createResultName($attrs);
        $this->allusers[$attrs['uid'][0]]= $this->createResultName($attrs);
      } 
    }
  
    /* check if all uids are resolved */
    foreach ($this->memberUid as $value){
      if(!isset($this->members[$value])){
        $this->members[$value] =  _("! unknown id")." [".$value."]"; 
      }
    }  

    /* Create display list of users matching regex & filter 
     */
    $this->displayUsers = array();

    /* Set tag attribute if we've tagging activated */
    $tag= "";
    $ui= get_userinfo();
    if ($ui->gosaUnitTag != "" && isset($this->config->current['STRICT_UNITS']) &&
        preg_match('/TRUE/i', $this->config->current['STRICT_UNITS'])){
      $tag= "(gosaUnitTag=".$ui->gosaUnitTag.")";
    }

    $filter = "(&(objectClass=gosaAccount)$tag(!(objectClass=gosaUserTemplate))(!(uid=*$))(|(uid=".$regex.")(sn=".$regex.")(givenName=".$regex.")))";

    /* Search in current tree or within subtrees depending on the checkbox from filter section */
    if($gufilter['SubSearchGroup']){
      $ldap->search($filter, array("dn", "uid", "sn","givenName"));
    }else{
      $ldap->ls ($filter, get_people_ou().$gufilter['dselect'],array("dn", "uid", "sn", "givenName"));
    }
    $i = 0;
    
    /* Fetch all users and skip already used users */
    while($attrs = $ldap->fetch()){
      if(in_array($attrs['uid'][0], $this->memberUid)) {
        continue;
      }
      $i ++;
      if($i > $MaxUser) {
        break;
      }
      $this->dnMapping[$attrs['uid'][0]]= $attrs["dn"];
      $this->allusers[$attrs['uid'][0]]     = $this->createResultName($attrs);
      $this->displayUsers[$attrs['uid'][0]] = $this->createResultName($attrs);
    }
  
    /* If more than max users are found, display a message to warn the user */
    if(($i == $MaxUser)){
      print_red(sprintf(_("Your search method returned more than '%s' users, only '%s' users are shown.") , $MaxUser,$MaxUser));
    }
    
    /* Sort lists */
    natcasesort($this->members);
    reset($this->members);
    natcasesort ($this->displayUsers);
    reset ($this->displayUsers);
  }


  /* Create display name, this was used so often that it is excluded into a seperate function */
  function createResultName($attrs)
  {
    if (isset($attrs["givenName"][0]) && isset($attrs["sn"][0])){
      $ret =  $attrs["sn"][0].", ".$attrs["givenName"][0]." [".$attrs["uid"][0]."]";
    } else {
      $ret= $attrs['uid'][0];
    }
    return($ret);
  }


  function remove_from_parent()
  {
    plugin::remove_from_parent();

    $ldap= $this->config->get_ldap_link();
    $ldap->rmdir($this->dn);
    show_ldap_error($ldap->get_error(), _("Removing group failed"));

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

    /* Send signal to the world that we've done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    /* Save additional values for possible next step */
    if (isset($_POST['groupedit'])){

      plugin::save_object();

      $this->force_gid= 0;
      $this->smbgroup= 0;
      foreach (array("force_gid", "department", "base", "smbgroup") as $val) {
        if (chkacl ($this->acl, "$val") == "" && isset($_POST["$val"])){
          $this->$val= $_POST["$val"];
        }
      }

      /* Save sambaDomain attribute */
      if (chkacl ($this->acl, "sambaDomainName") == "" && $this->samba3 &&
          isset ($_POST['sambaDomainName'])){

        $this->sambaDomainName= $_POST['sambaDomainName'];
        $this->groupType= $_POST['groupType'];
      }

      /* Save fon attribute */
      if (chkacl ($this->acl, "fon_group") == ""){
        if (isset ($_POST['fon_group'])){
          $this->fon_group= TRUE;
        } else {
          $this->fon_group= FALSE;
        }
      }
	 if (chkacl ($this->acl, "nagios_group") == ""){
        if (isset ($_POST['nagios_group'])){
          $this->nagios_group= TRUE;
        } else {
          $this->nagios_group= FALSE;
        }
      }
    }
  }


  /* Save to LDAP */
  function save()
  {

    /* ID handling */
    if ($this->force_gid == 0){
      if ($this->saved_gidNumber != ""){
        $this->gidNumber= $this->saved_gidNumber;
      } else {
        /* Calculate new, lock uids */
        $wait= 10;
        while (get_lock("uidnumber") != ""){
          sleep (1);

          /* timed out? */
          if ($wait-- == 0){
            break;
          }
        }
        add_lock ("uidnumber", "gosa");
        $this->gidNumber= $this->get_next_id("gidNumber", $this->dn);
      }
    }
  
    plugin::save(); 

    /* Remove objectClass for samba/phone support */
    $tmp= array();
    for ($i= 0; $i<count($this->attrs["objectClass"]); $i++){
      if ($this->attrs['objectClass'][$i] != 'sambaGroupMapping' &&
          $this->attrs['objectClass'][$i] != 'sambaIdmapEntry' &&
          $this->attrs['objectClass'][$i] != 'goFonPickupGroup' &&
	 $this->attrs['objectClass'][$i] != 'nagiosContactGroup'){
        $tmp[]= $this->attrs['objectClass'][$i];
      }
    }
    $this->attrs['objectClass']= $tmp;
    $ldap= $this->config->get_ldap_link();

    /* Add samba group functionality */
    if ($this->samba3 && $this->smbgroup){
  
      /* Fixed undefined index ... 
       */ 
      $this->SID = $this->ridBase = "";
      if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])){
        $this->SID    = $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
      }else{
        print_red(sprintf(_("No configured SID found for '%s'."),$this->sambaDomainName));
      }
      if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'])){
        $this->ridBase= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE']; 
      }else{
        print_red(sprintf(_("No configured RIDBASE found for '%s'."),$this->sambaDomainName));
      }

      $this->attrs['objectClass'][]= 'sambaGroupMapping';
      $this->attrs['sambaGroupType']= "2";

      /* Check if we need to create a special entry */
      if ($this->groupType == 0){

        if ($this->sambaSID == "" || $this->SID != $this->savedSID || $this->oldgroupType != $this->groupType ){
          $gidNumber= $this->gidNumber;
          while(TRUE){
            $sid= $this->SID."-".($gidNumber*2 + $this->ridBase+1);
            $ldap->cd($this->config->current['BASE']);
            $ldap->search("(sambaSID=$sid)",array("sambaSID"));
            if ($ldap->count() == 0){
              break;
            }
            $gidNumber++;
          }
          $this->attrs['sambaSID']= $sid;
          $this->sambaSID= $sid;
        }

      } else {
        $this->attrs['sambaSID']=$this->SID."-".$this->groupType;
      }

      /* User wants me to fake the idMappings? This is useful for
         making winbind resolve the group names in a reasonable amount
         of time in combination with larger databases. */
      if (isset($this->config->current['SAMBAIDMAPPING']) &&
          preg_match('/true/i', $this->config->current['SAMBAIDMAPPING'])){
        $this->attrs['objectClass'][]= "sambaIdmapEntry";
      }

    }

    /* Add phone functionality */
    if ($this->fon_group){
      $this->attrs['objectClass'][]= "goFonPickupGroup";
    }

    /* Add nagios functionality */
    if ($this->nagios_group){
      $this->attrs['objectClass'][]= "nagiosContactGroup";
    }

    /* Take members array */
    if (count ($this->memberUid)){
      $this->attrs['memberUid']= array_unique($this->memberUid);
    }

    /* New accounts need proper 'dn', propagate it to remaining objects */
    if ($this->dn == 'new'){
      $this->dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
    }

    /* Add member dn's for RFC2307bis Support */
    if ($this->rfc2307bis){
      if (count($this->memberUid)){
        $this->attrs['member'] = array();
        foreach($this->attrs['memberUid'] as $uid) {
          $this->attrs['member'][]= $this->dnMapping[$uid];
        }
      } else {
        $this->attrs['member'][]= $this->dn;
      }
    }

    /* Save data. Using 'modify' implies that the entry is already present, use 'add' for
       new entries. So do a check first... */
    $ldap->cat ($this->dn, array('dn'));
    if ($ldap->fetch()){
      /* Modify needs array() to remove values :-( */
      if (!count ($this->memberUid)){
        $this->attrs['memberUid']= array();
      }
      if ($this->samba3){
        if (!$this->smbgroup){
          $this->attrs['sambaGroupType']= array();
          $this->attrs['sambaSID']= array();
        }
      }
      $mode= "modify";
    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    /* Write back to ldap */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->$mode($this->attrs);

    $ret= 0;
    if (show_ldap_error($ldap->get_error(), _("Saving group failed"))){
      $ret= 1;
    }

    /* Remove uid lock */
    del_lock ("uidnumber");

    /* Post that we've done*/
    $this->handle_post_events($mode);

    return ($ret);
  }

  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    /* Permissions for that base? */
    if ($this->base != ""){
      $new_dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
    } else {
      $new_dn= $this->dn;
    }

    /* Check permissions */
    $ui= get_userinfo();
    $acl= get_permissions ($new_dn, $ui->subtreeACL);
    $acl= get_module_permission($acl, "group", $new_dn);
    if ($this->dn == "new" && chkacl($acl, "create") != ""){
      $message[]= _("You have no permissions to create a group on this 'Base'.");
    } elseif ($this->dn != $new_dn && $this->dn != "new"){
      $acl= get_permissions ($new_dn, $ui->subtreeACL);
      $acl= get_module_permission($acl, "group", $new_dn);
      if (chkacl($acl, "create") != ""){
        $message[]= _("You have no permissions to move a group from the original 'Base'.");
      }
    }

    /* must: cn */
    if ($this->cn == "" && chkacl ($this->acl, "cn") == ""){
      $message[]= "The required field 'Name' is not set.";
    }

    /* Check for valid input */
    if (!is_uid($this->cn)){
      $message[]= _("The field 'Name' contains invalid characters. Lowercase, numbers and dashes are allowed.");
    }

    if($this->allowGroupsWithSameNameInOtherSubtrees == true){

      /* Check for used 'cn' */
      $ldap= $this->config->get_ldap_link();
      if(($this->cn  != $this->orig_cn) || ($this->orig_dn == "new")){
        $ldap->cd("ou=groups,".$this->base);
        $ldap->ls("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))",get_groups_ou().$this->base,array("cn"));
        if ($ldap->count() != 0){
          $message[]= _("Value specified as 'Name' is already used.");
        }
      }

    }else{

      /* Check for used 'cn' */
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))",array("cn"));
      if ($ldap->count() != 0){

        /* New entry? */
        if ($this->dn == 'new'){
          $message[]= _("Value specified as 'Name' is already used.");
        }

        /* Moved? */
        elseif ($new_dn != $this->orig_dn){
          $ldap->fetch();
          if ($ldap->getDN() != $this->orig_dn){
            $message[]= _("Value specified as 'Name' is already used.");
          }
        }
      }
    }
     
    /* Check ID */
    if ($this->force_gid == "1"){
      if (!is_id($this->gidNumber)){
        $message[]= _("Value specified as 'GID' is not valid.");
      } else {
        if ($this->gidNumber < $this->config->current['MINID']){
          $message[]= _("Value specified as 'GID' is too small.");
        }

      }
    }

    return ($message);
  }

  function get_next_id($attrib, $dn)
  {
    $ids= array();
    $ldap= $this->config->get_ldap_link();

    $ldap->cd ($this->config->current['BASE']);
    if (preg_match('/gidNumber/i', $attrib)){
      $oc= "posixGroup";
    } else {
      $oc= "posixAccount";
    }
    $ldap->search ("(&(objectClass=$oc)($attrib=*))", array("$attrib"));

    /* Get list of ids */
    while ($attrs= $ldap->fetch()){
      $ids[]= (int)$attrs["$attrib"][0];
    }

    /* Find out next free id near to UID_BASE */
    if (!isset($this->config->current['BASE_HOOK'])){
      $base= $this->config->current['UIDBASE'];
    } else {
      /* Call base hook */
      $base= get_base_from_hook($dn, $attrib);
    }
    for ($id= $base; $id++; $id < pow(2,32)){
      if (!in_array($id, $ids)){
        return ($id);
      }
    }

    /* check if id reached maximum of 32 bit*/
    if ($id >= pow(2,32)){
      echo _("Too many groups, can't allocate a free ID!");
      exit;
    }
  }

  function getCopyDialog()
  {
    $vars = array("cn");
  
    if($this ->force_gid){
      $used = " checked ";
      $dis  = "";
    }else{
      $used = "";
      $dis  = " disabled ";
    }

    $smarty = get_smarty();
    $smarty->assign("used",$used);
    $smarty->assign("dis" ,$dis);
    $smarty->assign("cn" ,$this->cn);
    $smarty->assign("gidNumber",$this->gidNumber);
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE));
    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = "";
    return($ret);
  }

  function saveCopyDialog()
  {
    if(isset($_POST['cn'])){
      $this->cn = $_POST['cn'];
    }
    if(isset($_POST['force_gid'])){
      $this->force_gid  = 1;
      $this->gidNumber= $_POST['gidNumber'];
    }else{
      $this->force_gid  = 0;
      $this->gidNumber  = false;
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
