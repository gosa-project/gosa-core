<?php

class group extends plugin
{
  /* CLI vars */
  var $cli_summary= "Handling of GOsa's base group object";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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
  var $saved_gidNumber= "";
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
  var $view_logged = FALSE;
  var $allowGroupsWithSameNameInOtherSubtrees = true;

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gidNumber","memberUid","sambaGroupType","sambaSID");
  var $objectclasses= array("top", "posixGroup");

  var $CopyPasteVars  = array("force_gid","fon_group","smbgroup","groupType","sambaSID","sambaDomainName","SID","nagios_group","sambaGroupType");

  function group (&$config, $dn= NULL)
  {
     /* Set rfc2307bis flag */
     if (isset($config->current['RFC2307BIS']) && ($config->current['RFC2307BIS']== "true")){
       $this->rfc2307bis= TRUE;
       $this->attributes[]= "member";
       $this->objectclasses[]= "groupOfNames";
     }

    plugin::plugin ($config, $dn);

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
      if (array_search ('sambaGroupMapping', $this->attrs['objectClass']) == FALSE ){
        $this->smbgroup= FALSE;
      } else {
        $this->smbgroup= TRUE;
        if (isset($this->attrs['sambaSID'])){
          $this->sambaSID= $this->attrs['sambaSID'][0];
        }
      }
      if (array_search ('goFonPickupGroup', $this->attrs['objectClass']) == FALSE ){
        $this->fon_group= FALSE;
      } else {
        $this->fon_group= TRUE;
      }
      if (array_search ('nagiosContactGroup', $this->attrs['objectClass']) == FALSE ){
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

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","groups/".get_class($this),$this->dn);
    }

    /* Do we represent a valid group? */
    if (!$this->is_account && $this->parent === NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This 'dn' is no group.")."</b>";
      return ($display);
    }

    /* Delete user from group */
    if (isset($_POST['del_users']) && isset($_POST['members'])){
      foreach ($_POST['members'] as $value){
        unset ($this->members["$value"]);
        $this->removeUser($value);
      }
      $this->reload();
    }

    /* Add objects? */
    if (isset($_POST["edit_membership"])){
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
      if((preg_match("/^chooseBase/",$name) && $once) && ($this->acl_is_moveable())){
          
        $once = false;
        $this->dialog = new baseSelectDialog($this->config,$this,$this->get_allowed_bases());
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

        /* Check if selected base is valid */
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$this->dialog->isSelected()])){
          $this->base = $this->dialog->isSelected();
        }
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

    if($this->config->search("nagiosaccount", "CLASS",array('menu'))){
      $smarty->assign("nagios",true);
    }else{
      $smarty->assign("nagios",false);
    }
    
    if($this->config->search("phoneAccount", "CLASS",array('menu'))){
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

      $smarty->assign("deplist", $this->get_allowed_bases("users/user"));
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

    $smarty->assign("bases", $this->get_allowed_bases());
    $smarty->assign("base_select", $this->base);

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
    if ($this->fon_group){
      $smarty->assign("fon_group", "checked");
    } else {
      $smarty->assign("fon_group", "");
    }

    if ($this->nagios_group){
      $smarty->assign("nagios_group", "checked");
    } else {
      $smarty->assign("nagios_group", "");
    }

    /* Fields */
    foreach (array("cn", "description", "gidNumber") as $val){
      $smarty->assign("$val", $this->$val);
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation){
      $smarty->assign($name."ACL",$this->getacl($name));
    }
    
    if($this->acl_is_writeable("base")){
      $smarty->assign("baseSelect",true);
    }else{
      $smarty->assign("baseSelect",false);
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
    $filter = "(&(objectClass=gosaAccount)(!(objectClass=gosaUserTemplate))(!(uid=*$))(|(uid=".$regex.")(sn=".$regex.")(givenName=".$regex.")))";

    /* Search in current tree or within subtrees depending on the checkbox from filter section */
    if($gufilter['SubSearchGroup']){
      $flag = GL_SIZELIMIT | GL_SUBSEARCH;
      $base = $gufilter['dselect'];
    }else{
      $flag = GL_SIZELIMIT ;
      $base = get_people_ou().$gufilter['dselect'];
    }
    $i = 0;
  

    $res = get_list($filter,"users",$base,array("dn", "uid", "sn", "givenName"),$flag);

    /* Fetch all users and skip already used users */
    foreach($res as $attrs){
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
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of groups/generic with dn '%s' failed."),$this->dn));

    new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".$this->dn."))", array("cn"));
    while ($ldap->fetch()){
      $og= new ogroup($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

    /* Remove ACL dependencies too,
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaAcl)(gosaAclEntry=*".base64_encode($this->dn)."*))",array("gosaAclEntry","dn"));
    while($attrs = $ldap->fetch()){
      $acl = new acl($this->config,$this->parent,$attrs['dn']);
      foreach($acl->gosaAclEntry as $id => $entry){
        foreach($entry['members'] as $m_id => $member){
          if($m_id == "G:".$this->dn || $m_id == "U:".$this->dn){
            unset($acl->gosaAclEntry[$id]['members'][$m_id]);
            gosa_log("modify","groups/acl",$attrs['dn'],array(),sprintf("Removed acl for %s on object %s.",$this->dn,$attrs['dn']));
          }
        }
      }
      $acl -> save();
    }

    /* Remove ACL dependencies too,
     */
    $tmp = new acl($this->config,$this->parent,$this->dn);
    $tmp->remove_acl();

    /* Send signal to the world that we've done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    /* Save additional values for possible next step */
    if (isset($_POST['groupedit'])){

      /* Create a base backup and reset the 
          base directly after calling plugin::save_object();  
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::save_object();
      $this->base = $base_tmp;

      $this->force_gid= 0;

      /* Only reset sambagroup flag if we are able to write this flag */
      if($this->acl_is_writeable("sambaGroupType")){
        $this->smbgroup = 0;
      }

      /* Get base selection */
      if(isset($_POST['base'])){
        $tmp = $this->get_allowed_bases();
        if(isset($tmp[$_POST['base']])){
          $this->base = $_POST['base'];
        }
      }

      foreach (array(
            "force_gid"  => "gidNumber", 
            "smbgroup"   => "sambaGroupType") as $val => $aclname) {
        if ($this->acl_is_writeable($aclname)  && isset($_POST["$val"])){
          $this->$val= $_POST["$val"];
        }
      }

      /* Save sambaDomain attribute */
      if ($this->acl_is_writeable("sambaDomainName") && $this->samba3 && isset ($_POST['sambaDomainName'])){
        $this->sambaDomainName= $_POST['sambaDomainName'];
        $this->groupType= $_POST['groupType'];
      }

      /* Save fon attribute */
      if ($this->acl_is_writeable("fon_group")){
        if (isset ($_POST['fon_group'])){
          $this->fon_group= TRUE;
        } else {
          $this->fon_group= FALSE;
        }
      }
      if ($this->acl_is_writeable("nagios_group")){
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

        if ($this->sambaSID == "" || $this->oldgroupType != $this->groupType){
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

    /* Remove ACL dependencies too,
     */
    if($this->dn != $this->orig_dn && $this->orig_dn != "new"){
      $tmp = new acl($this->config,$this->parent,$this->dn);
      $tmp->update_acl_membership($this->orig_dn,$this->dn);
    }

    if($this->initially_was_account){
      new log("modify","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    $ret= 0;
    if ( show_ldap_error($ldap->get_error(), sprintf(_("Removing of groups/generic with dn '%s' failed."),$this->dn))){
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

    /* must: cn */
    if ($this->cn == "" && $this->acl_is_writeable("cn")){
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

    /* Check if id reached maximum */
    if ($id >= pow(2,32)){
      print_red(_("Too many users, can't allocate a free ID!"));
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

  
  /* Return plugin informations for acl handling  */ 
  static function plInfo()
  {
    return (array(  
          "plShortName" => _("Generic"),
          "plDescription" => _("Generic group settings"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("admin"),
          "plCategory"    => array("groups" => array("objectClass" => "posixGroup", "description" => _("Groups"))),

          "plProvidedAcls"    => array(
            "cn"                => _("Name"),
            "base"              => _("Base"),
            "description"       => _("Description"),

            "fonGroup"          => _("Phone pickup group"),
            "nagiosGroup"       => _("Nagios group"),

            "gidNumber"         => _("GID"),
            "memberUid"         => _("Group member"),
            "sambaGroupType"    => _("Samba group type"),
            "sambaDomainName"   => _("Samba domain name"),
            "sambaSID"          => _("Samba SID"))
        ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
