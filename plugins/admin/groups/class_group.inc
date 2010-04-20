<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class group extends plugin
{
  /* Group attributes */
  var $cn= "";
  var $description= "";
  var $gidNumber= "";
  var $memberUid= array();
  var $memberUid_used_by_some= array();

  /* Helpers */
  var $base= "";
  var $force_gid= FALSE;
  var $fon_group= FALSE;
  var $smbgroup= FALSE;
  var $groupType= FALSE;
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
  var $orig_base= "";
  var $has_mailAccount= FALSE;
  var $userSelect= FALSE;
  var $nagios_group =FALSE;
  var $sambaGroupType;
  var $dialog;
  var $rfc2307bis= FALSE;
  var $OnlyShowFirstEntries =200;
  var $dnMapping= array();
  var $view_logged = FALSE;
  var $allowGroupsWithSameNameInOtherSubtrees = true;
  var $baseSelector;

  /* attribute list for save action */
  var $attributes= array("cn", "description", "gidNumber","memberUid","sambaGroupType","sambaSID");
  var $objectclasses= array("top", "posixGroup");

  var $CopyPasteVars  = array("force_gid","fon_group","smbgroup","groupType","sambaSID","sambaDomainName","SID","nagios_group","sambaGroupType");

  var $multiple_support = TRUE;

  function group (&$config, $dn= NULL)
  {
     /* Set rfc2307bis flag */
     if ($config->get_cfg_value("rfc2307bis") == "true"){
       $this->rfc2307bis= TRUE;
       $this->attributes[]= "member";
       $this->objectclasses[]= "groupOfNames";
     }

    plugin::plugin ($config, $dn);

    $this->trustModeDialog = new trustModeDialog($this->config, $this->dn,NULL);
    $this->trustModeDialog->setAcl('groups/group');

    /* Load attributes depending on the samba version */
    $this->orig_dn= $dn;
    $this->orig_cn= $this->cn;

    /* Get member list */
    if (isset($this->attrs['memberUid'][0])){
      $tmp= array();
      for ($i= 0; $i<$this->attrs['memberUid']['count']; $i++){
        $tmp[$this->attrs['memberUid'][$i]]= $this->attrs['memberUid'][$i];
      }
      $this->memberUid= $tmp;
      ksort ($this->memberUid);
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
    if ($this->sambaSID != ""){
      $this->SID= preg_replace ("/-[^-]+$/", "", $this->sambaSID);
      $ldap= $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search ("(&(objectClass=sambaDomain)(sambaSID=$this->SID))",array("sambaAlgorithmicRidBase"));
      if ($ldap->count() != 0){
        $attrs= $ldap->fetch();
        if(isset($attrs['sambaAlgorithmicRidBase'])){  
          $this->ridBase= $attrs['sambaAlgorithmicRidBase'][0];  
        } else {  
          $this->ridBase= $this->config->get_cfg_value("sambaRidBase");
        } 

        /* Get domain name for SID */
        $this->sambaDomainName= "DEFAULT";
        foreach ($this->config->data['SERVERS']['SAMBA'] as $key => $val){
          if ($val['SID'] == $this->SID){
            $this->sambaDomainName= $key;
            break;
          }
        }
      } else {
        if ($this->config->get_cfg_value("sambaRidBase") != ""){
          $this->sambaDomainName= "DEFAULT";
          $this->ridBase= $this->config->get_cfg_value("sambaRidBase");
          $this->SID= $this->config->get_cfg_value("sid");
        } else {
          msg_dialog::display(_("Configuration error"), _("Cannot find group SID in your configuration!"), ERROR_DIALOG);
        }
      }

      /* Get group type */
      $this->groupType= (int)substr(strrchr($this->sambaSID, "-"), 1);
      if ($this->groupType < 500 || $this->groupType > 553){
        $this->groupType= 0;
      }
      $this->oldgroupType= $this->groupType;
    }

    if ($this->dn == "new"){
      if(session::is_set('CurrentMainBase')){
        $this->base = session::get('CurrentMainBase');
      }else{
        $ui= get_userinfo();
        $this->base= dn2base($ui->dn);
      }
    } else {

      /* Get object base */
      $this->base =preg_replace ("/^[^,]+,".preg_quote(get_groups_ou(), '/')."/i","",$this->dn);
    }
    $this->orig_base = $this->base;

    /* This is always an account */
    $this->is_account= TRUE;

    /* Instanciate base selector */
    $this->baseSelector= new baseSelector($this->get_allowed_bases(), $this->base);
    $this->baseSelector->setSubmitButton(false);
    $this->baseSelector->setHeight(300);
    $this->baseSelector->update(true);

    // Prepare lists
    $this->memberList = new sortableListing();
    $this->memberList->setDeleteable(true);
    $this->memberList->setInstantDelete(true);
    $this->memberList->setEditable(false);
    $this->memberList->setWidth("100%");
    $this->memberList->setHeight("300px");
    $this->memberList->setHeader(array('~',_("Given name"),_("Surename"),_("Uid")));
    $this->memberList->setColspecs(array('20px','*','*','*','20px'));
    $this->memberList->setDefaultSortColumn(1);

    $this->reload(TRUE);
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
      $display= "<img alt=\"\" src=\"images/small-error.png\" align=\"middle\">&nbsp;<b>".msgPool::noValidExtension()."</b>";
      return ($display);
    }

    /* Delete user from group */
    $this->memberList->save_object();
    $action = $this->memberList->getAction();
    if($action['action'] == 'delete' && preg_match("/w/",$this->getacl("memberUid"))){
      foreach ($action['targets'] as $id){
        $value = $this->memberList->getKey($id);
        unset ($this->members["$value"]);
        $this->removeUser($value);
      }
      $this->reload();
    }

    /* Add objects? */
    if (isset($_POST["edit_membership"]) && preg_match("/w/",$this->getacl("memberUid"))){
      $this->userSelect= new userSelect($this->config, get_userinfo());
    }

    /* Add objects finished? */
    if (isset($_POST["add_users_cancel"])){
      $this->userSelect= NULL;
    }

    /* Add user to group */
    if (isset($_POST['add_users_finish']) && $this->userSelect){
  
      $users = $this->userSelect->detectPostActions();
      if(isset($users['targets'])){
        $headpage = $this->userSelect->getHeadpage();
        foreach($users['targets'] as $dn){
          $attrs = $headpage->getEntry($dn);
          $value = $attrs['uid'][0];
          $this->addUser($value);
          $this->members["$value"]= $this->allusers[$value];
          $this->reload();
        }
      }
      $this->userSelect= NULL;
    }

    $smarty= get_smarty();
 
    // Handle trust mode dialog
    $this->dialog = FALSE;
    $trustModeDialog = $this->trustModeDialog->execute();
    if($this->trustModeDialog->trustSelect){
        $this->dialog = TRUE;
        return($trustModeDialog);
    }
    $smarty->assign("trustModeDialog" , $trustModeDialog);

    $smarty->assign("usePrototype", "true");

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
    if ($this->userSelect){
      $this->dialog = TRUE;
      return($this->userSelect->execute());
    }

    /* Create base acls */
    $smarty->assign("base", $this->baseSelector->render());

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
    

    /* Members and users */
    $this->memberList->setAcl($this->getacl("memberUid"));
    $data = $lData = array();

    foreach($this->members as $uid => $member){
        
        $data[$uid] = $member;
        $givenName = $sn = _("Unknown");
        if(isset($member['sn'][0])) $sn = $member['sn'][0];
        if(isset($member['givenName'][0])) $givenName = $member['givenName'][0];
        
        $image = image('images/false.png');
        if(isset($member['sn'])){
            $image = image('plugins/users/images/select_user.png');
        }

        $lData[$uid] = array('data' => array($image,$sn, $givenName, $uid));
    }

    $this->memberList->setListData($data,$lData);
    $this->memberList->update();

    $smarty->assign("memberList", $this->memberList->render());

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

    /* Multiple edit handling */
    $smarty->assign("multiple_support",$this->multiple_support_active);
    $smarty->assign("memberUid_All",$this->memberUid);
    $smarty->assign("memberUid_Some",$this->memberUid_used_by_some);

    foreach($this->attributes as $val){
      if(in_array($val,$this->multi_boxes)){
        $smarty->assign("use_".$val,TRUE);
      }else{
        $smarty->assign("use_".$val,FALSE);
      }
    }
    foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $val){
      if(in_array($val,$this->multi_boxes)){
        $smarty->assign("use_".$val,TRUE);
      }else{
        $smarty->assign("use_".$val,FALSE);
      }
    }

    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }

  function addUser($uid)
  {
    /* In mutliple edit we have to handle two arrays.
     *  memberUid               : Containing users used in all groups
     *  memberUid_used_by_some  : Those which are not used in all groups
     * So we have to remove the given $uid from the ..used_by_some array first.
     */
    if($this->multiple_support_active){
      if(isset($this->memberUid_used_by_some[$uid])){
        unset($this->memberUid_used_by_some[$uid]);
      }
    }  

    /* Ensure that the requested object is known to the group class 
     */
    if(!isset($this->dnMapping[$uid])){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gosaAccount)(uid=".$uid."))",array("dn", "uid","sn","givenName"));
      if($ldap->count() == 0 ){
        msg_dialog::display(_("Error"), 
            sprintf(_("Adding UID '%s' to group '%s' failed: cannot find user object!"), 
              $uid,$this->cn), 
            ERROR_DIALOG);
        return;
      }elseif($ldap->count() >= 2){
        msg_dialog::display(_("Error"), 
            sprintf(_("Add UID '%s' to group '%s' failed: UID is used more than once!"),
              $uid,$this->cn), 
            ERROR_DIALOG);
        return;
      }else{
        while($attrs = $ldap->fetch()){
          $this->dnMapping[$attrs['uid'][0]] = $attrs['dn'];
          $this->members[$attrs['uid'][0]] = $attrs;
          $this->allusers[$attrs['uid'][0]]= $atrts;
        }
      }
    }

    $this->memberUid[$uid]= $uid;
  }


  function removeUser($uid)
  {
    $temp= array();
    if(isset($this->memberUid[$uid])){
      unset($this->memberUid[$uid]);
    }

    /* We have two array contianing group members in multiple edit.
     *  this->memberUid             : Groups used by all currently edited groups 
     *  this->memberUid_used_by_some: Used by some 
     * So we have to remove the specified uid from both arrays.
     */
    if($this->multiple_support_active){
      if(isset($this->memberUid_used_by_some[$uid])){
        unset($this->memberUid_used_by_some[$uid]);
      }
    }
  }

  /* Reload data */
  function reload($silent = FALSE)
  {
    /* Prepare ldap link */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);


    /* Resolve still unresolved memberuids to fill the list with sn/giveName attributes 
        (Store gathered sn/givenName informations in $this->allusers too, 
         to be prepared when adding/deleting users)
     */    
    $filter = "";
    if ($this->config->get_cfg_value("ldapFilterNestingLimit") == "" ||
        count($this->memberUid) < $this->config->get_cfg_value("ldapFilterNestingLimit")){
      foreach ($this->memberUid as $value){
        if(!isset($this->members[$value])){
          $filter .= "(uid=".normalizeLdap($value).")";
        }
      }
    }

    if(!empty($filter)){    
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=gosaAccount)(|".$filter."))",array("dn", "uid","sn","givenName"));
      while($attrs = $ldap->fetch()){
        $this->dnMapping[$attrs['uid'][0]] = $attrs['dn'];
        $this->members[$attrs['uid'][0]] = $attrs;
        $this->allusers[$attrs['uid'][0]]= $attrs;
      } 
    }
  
    /* check if all uids are resolved */
    if ($this->config->get_cfg_value("ldapFilterNestingLimit") == "" ||
        count($this->memberUid) < $this->config->get_cfg_value("ldapFilterNestingLimit")){
      foreach ($this->memberUid as $value){
        if(!isset($this->members[$value])){
          $this->members[$value] = "";
        }
      }  
    }else{
      foreach ($this->memberUid as $value){
        $this->members[$value] = "";
      }  
    }
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
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_DEL, get_class()));
    }

    new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
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
      $acl->save();
    }

    /* Remove ACL dependencies, too */
    acl::remove_acl_for($this->dn);

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
      $this->trustModeDialog->save_object();
      $this->base = $base_tmp;

      /* Refresh base */
      if ($this->acl_is_moveable($this->base)){
        if (!$this->baseSelector->update()) {
          msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
        }
        if ($this->base != $this->baseSelector->getBase()) {
          $this->base= $this->baseSelector->getBase();
          $this->is_modified= TRUE;
        }
      }

      $this->force_gid= 0;

      /* Only reset sambagroup flag if we are able to write this flag */
      if($this->acl_is_writeable("sambaGroupType")){
        $this->smbgroup = 0;
      }

      foreach (array(
            "force_gid"  => "gidNumber", 
            "smbgroup"   => "sambaGroupType") as $val => $aclname) {
        if ($this->acl_is_writeable($aclname)  && isset($_POST["$val"])){
          $this->$val= $_POST["$val"];
        }
      }

      /* Save sambaDomain attribute */
      if ($this->acl_is_writeable("sambaDomainName") && isset ($_POST['sambaDomainName'])){
        $this->sambaDomainName= $_POST['sambaDomainName'];
        $this->groupType= $_POST['groupType'];
      }

      /* Save fon attribute */
      if ($this->acl_is_writeable("fonGroup")){
        if (isset ($_POST['fon_group'])){
          $this->fon_group= TRUE;
        } else {
          $this->fon_group= FALSE;
        }
      }
      if ($this->acl_is_writeable("nagiosGroup")){
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
        while (get_lock("gidnumber") != ""){
          sleep (1);

          /* timed out? */
          if ($wait-- == 0){
            break;
          }
        }
        add_lock ("gidnumber", "gosa");
        $this->gidNumber= get_next_id("gidNumber", $this->dn);
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
    if ($this->smbgroup){
  
      /* Fixed undefined index ... 
       */ 
      $this->SID = $this->ridBase = "";
      if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'])){
        $this->SID    = $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['SID'];
      }else{
        msg_dialog::display(_("Error"), sprintf(_("Cannot find any SID for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
      }
      if(isset($this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE'])){
        $this->ridBase= $this->config->data['SERVERS']['SAMBA'][$this->sambaDomainName]['RIDBASE']; 
      }else{
        msg_dialog::display(_("Error"), sprintf(_("Cannot find any RIDBASE for '%s'!"), $this->sambaDomainName), ERROR_DIALOG);
      }

      $this->attrs['objectClass'][]= 'sambaGroupMapping';
      $this->attrs['sambaGroupType']= "2";

      /* Check if we need to create a special entry */
      if ($this->groupType == 0){

        if ($this->sambaSID == "" || $this->oldgroupType != $this->groupType){
          $sid = $this->getSambaSID();
          $this->attrs['sambaSID']= $sid;
          $this->sambaSID= $sid;
        }

      } else {
        $this->attrs['sambaSID']=$this->SID."-".$this->groupType;
      }

      /* User wants me to fake the idMappings? This is useful for
         making winbind resolve the group names in a reasonable amount
         of time in combination with larger databases. */
      if ($this->config->get_cfg_value("sambaidmapping") == "true"){
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
      $this->attrs['memberUid']= array_values(array_unique($this->memberUid));
    }

    /* New accounts need proper 'dn', propagate it to remaining objects */
    if ($this->dn == 'new'){
      $this->dn= 'cn='.$this->cn.','.get_groups_ou().$this->base;
    }

    /* Add member dn's for RFC2307bis Support */
    if ($this->rfc2307bis){
      $this->attrs['member'] = array();
      if (count($this->memberUid)){
        foreach($this->attrs['memberUid'] as $uid) {

          if(isset($this->dnMapping[$uid])){
            $this->attrs['member'][]= $this->dnMapping[$uid];
          }
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
      if (!$this->smbgroup){
        $this->attrs['sambaGroupType']= array();
        $this->attrs['sambaSID']= array();
      }
      
      $mode= "modify";
    } else {
      $mode= "add";
      $ldap->cd($this->config->current['BASE']);
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
    }

    $this->trustModeDialog->dn = $this->dn;
    $this->trustModeDialog->save();

    /* Check generated gidNumber, it may be used by another group. 
     */
    if($this->gidNumber != ""){
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(!(cn=".$this->orig_cn."))(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))",array("cn"));
      if($ldap->count()){
        $cns = "";
        while($attrs = $ldap->fetch()){
          $cns .= $attrs['cn'][0].", ";
        }
        $cns = rtrim($cns,", ");
        msg_dialog::display(_("Warning"),sprintf(_("The gidNumber '%s' is already in use by %s!"),$this->gidNumber,$cns) , WARNING_DIALOG );
      }
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
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, 0, get_class()));
      $ret= 1;
    }

    /* Remove uid lock */
    del_lock ("gidnumber");

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
      $message[]= msgPool::required(_("Name"));
    }

    // Check if a wrong base was supplied
    if(!$this->baseSelector->checkLastBaseUpdate()){
      $message[]= msgPool::check_base();;
    }

    /* Check for valid input */
    if (!tests::is_uid($this->cn)){
      if (strict_uid_mode()){
        $message[]= msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/");
      } else {
        $message[]= msgPool::invalid(_("Name"), $this->cn, "/[a-z0-9_-]/i");
      }
    }

    // Check if a wrong base was supplied
    if(!$this->baseSelector->checkLastBaseUpdate()){
      $message[]= msgPool::check_base();;
    }

    if($this->allowGroupsWithSameNameInOtherSubtrees == true){

      /* Check for used 'cn' */
      $ldap= $this->config->get_ldap_link();
      if(($this->cn  != $this->orig_cn) || ($this->orig_dn == "new")){
        $ldap->cd(get_groups_ou().$this->base);
        $ldap->ls("(&(|(objectClass=gosaGroupOfNames)(objectClass=posixGroup))(cn=$this->cn))",get_groups_ou().$this->base,array("cn"));
        if ($ldap->count() != 0){
          $message[]= msgPool::duplicated(_("Name"));
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
          $message[]= msgPool::duplicated(_("Name"));
        }

        /* Moved? */
        elseif ($new_dn != $this->orig_dn){
          $ldap->fetch();
          if ($ldap->getDN() != $this->orig_dn){
            $message[]= msgPool::duplicated(_("Name"));
          }
        }
      }
    }
     
    /* Check ID */
    if ($this->force_gid == "1"){
      if (!tests::is_id($this->gidNumber)){
        $message[]= msgPool::invalid(_("GID"), $this->gidNumber, "/[0-9]/");
      } else {
        if ($this->gidNumber < $this->config->get_cfg_value("minId")){
          $message[]= msgPool::toosmall(_("GID"), $this->config->get_cfg_value("minId"));
        }

      }
    }
    
    /* Check if we are allowed to create or move this object 
     */
    if(!$this->orig_dn == "new" || 
        $this->orig_base != $this->base || 
        $this->cn != $this->orig_cn){

      if($this->orig_dn == "new" && !$this->acl_is_createable($this->base)){
        $message[] = msgPool::permCreate();
      }elseif($this->orig_dn != "new" && !$this->acl_is_moveable($this->base)){
        $message[] = msgPool::permMove();
      }
    }

    return ($message);
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
          "plSection"     => array("administration"),
          "plCategory"    => array("groups" => array("objectClass" => "posixGroup", "description" => _("Groups"))),

          "plProvidedAcls"    => array(
            "cn"                => _("Name"),
            "description"       => _("Description"),
            "base"              => _("Base"),

            "gidNumber"         => _("GID"),

            "sambaGroupType"    => _("Samba group type"),
            "sambaDomainName"   => _("Samba domain name"),
            "accessTo"        => _("System trust"),
            "fonGroup"          => _("Phone pickup group"),
            "nagiosGroup"       => _("Nagios group"),

            "memberUid"         => _("Group member"))
        ));
  }


  function multiple_save_object()
  {
    if(isset($_POST['group_mulitple_edit'])){

      /* Create a base backup and reset the
         base directly after calling plugin::save_object();
         Base will be set seperatly a few lines below */
      $base_tmp = $this->base;
      plugin::multiple_save_object();
      plugin::save_object();
      $this->trustModeDialog->multiple_save_object();
      $this->base = $base_tmp;

      foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $attr){
        if(isset($_POST['use_'.$attr])){
          $this->multi_boxes[] = $attr;
        }
      }

      /* Refresh base */
      if ($this->acl_is_moveable($this->base)){
        if (!$this->baseSelector->update()) {
          msg_dialog::display(_("Error"), msgPool::permMove(), ERROR_DIALOG);
        }
        if ($this->base != $this->baseSelector->getBase()) {
          $this->base= $this->baseSelector->getBase();
          $this->is_modified= TRUE;
        }
      }

      foreach (array( "smbgroup"   => "sambaGroupType" ,"nagios_group" => "nagios_group") as $val => $aclname) {
        if ($this->acl_is_writeable($aclname)){
          if(isset($_POST["$val"])){
            $this->$val=  TRUE;
          }else{
            $this->$val=  FALSE;
          }
        }
      }

      /* Save sambaDomain attribute */
      if ($this->acl_is_writeable("sambaDomainName") && isset ($_POST['sambaDomainName'])){
        $this->sambaDomainName= $_POST['sambaDomainName'];
        $this->groupType= $_POST['groupType'];
      }

      /* Save fon attribute */
      if ($this->acl_is_writeable("fonGroup")){
        if (isset ($_POST['fon_group'])){
          $this->fon_group= TRUE;
        } else {
          $this->fon_group= FALSE;
        }
      }
    }
  }


  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    $ret = array_merge($ret,$this->trustModeDialog->get_multi_edit_values());

    foreach(array("base","smbgroup","groupType","sambaDomainName","fon_group","nagios_group") as $attr){    
      if(in_array($attr,$this->multi_boxes)){
        $ret[$attr] = $this->$attr;
      }
    }

    $ret['memberUid'] = $this->memberUid;
    $ret['memberUid_used_by_some'] = $this->memberUid_used_by_some;
    return($ret);
  }

  function multiple_execute()
  {
    return($this->execute());
  }


  /* Initialize plugin with given atribute arrays
   */
  function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);
    $this->trustModeDialog->init_multiple_support($attrs,$all);

    $this->memberUid = array();
    $this->memberUid_used_by_some = array();
    if (isset($attrs['memberUid'])){
      for ($i= 0; $i<$attrs['memberUid']['count']; $i++){
        $this->memberUid[$attrs['memberUid'][$i]]= $attrs['memberUid'][$i];
      }
      ksort($this->memberUid);
    }

    if (isset($all['memberUid'])){
      for ($i= 0; $i<$all['memberUid']['count']; $i++){
        if(!in_array($all['memberUid'][$i],$this->memberUid)){
          $this->memberUid_used_by_some[$all['memberUid'][$i]]= $all['memberUid'][$i];
        }
      }
      ksort($this->memberUid_used_by_some);
    }
  }


  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    $this->trustModeDialog->PrepareForCopyPaste($source);

    if ($this->smbgroup) { 
      $this->sambaSID = $this->getSambaSID(); 
    } 

    $this->memberUid = array();
    if(isset($source['memberUid'])){
      for($i = 0 ; $i < $source['memberUid']['count']; $i ++){
        $this->memberUid[] = $source['memberUid'][$i];
      }
    }
  }


  function set_acl_base($base)
  {
    plugin::set_acl_base($base);
    $this->trustModeDialog->set_acl_base($base);
  }


  /*! \brief  Enables multiple support for this plugin
   */
  function enable_multiple_support()
  {
    plugin::enable_multiple_support();
    $this->trustModeDialog->enable_multiple_support();
  }


  function set_multi_edit_values($attrs)
  {
    $users = array();

    /* Update groupMembership, keep optinal group */
    foreach($attrs['memberUid_used_by_some'] as $uid){
      if(in_array($uid,$this->memberUid)){
        $users[$uid] = $uid;
      }
    }

    /* Update groupMembership, add forced groups */
    foreach($attrs['memberUid'] as $uid){
      $users[$uid] = $uid;
    }
    plugin::set_multi_edit_values($attrs);
    $this->trustModeDialog->set_multi_edit_values($attrs);
    $this->memberUid = $users;
  }


  /*! \brief Get a new SambaSID for a group */ 
  function getSambaSID() 
  { 
    $ldap = $this->config->get_ldap_link(); 
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
  
    return $sid; 
  } 

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
