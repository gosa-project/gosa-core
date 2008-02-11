<?php
/*!
  \brief   posixAccount plugin
  \author  Cajus Pollmeier <pollmeier@gonicus.de>
  \version 2.00
  \date    24.07.2003

  This class provides the functionality to read and write all attributes
  relevant for posixAccounts and shadowAccounts from/to the LDAP. It
  does syntax checking and displays the formulars required.
 */

class posixAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "UNIX";
  var $plDescription= "This does something";

  /* Plugin specific values */
  var $homeDirectory= "";
  var $loginShell= "/bin/bash";
  var $uidNumber= "";
  var $gidNumber= "";
  var $gecos= "";
  var $shadowMin= "0";
  var $shadowMax= "0";
  var $shadowWarning= "0";
  var $shadowLastChange= "0";
  var $shadowInactive= "0";
  var $shadowExpire= "0";
  var $gosaDefaultPrinter= "";
  var $accessTo= array();
  var $trustModel= "";

  var $glist=array();
  var $status= "";
  var $loginShellList= array();
  var $groupMembership= array();
  var $savedGroupMembership= array();
  var $savedUidNumber= "";
  var $savedGidNumber= "";
  var $use_shadowMin= "0";
  var $use_shadowMax= "0";
  var $use_shadowWarning= "0";
  var $use_shadowInactive= "0";
  var $use_shadowExpire= "0";
  var $must_change_password= "0";
  var $force_ids= 0;
  var $group_dialog= FALSE;
  var $show_ws_dialog= FALSE;
  var $secondaryGroups= array();
  var $primaryGroup= 0;
  var $was_trust_account= FALSE;

  var $grouplist  = array();
  var $ui         = array();

  var $GroupRegex       = "*";
  var $GroupUserRegex   = "*";
  var $SubSearch        = false;
  var $memberGroup      = array();

  /* attribute list for save action */
  var $CopyPasteVars  = array("grouplist","groupMembership","use_shadowMin","use_shadowMax","use_shadowWarning","use_shadowInactive","use_shadowExpire","must_change_password","grouplist","savedGidNumber","savedUidNumber");
  var $attributes     = array("homeDirectory", "loginShell", "uidNumber", "gidNumber", "gecos",
      "shadowMin", "shadowMax", "shadowWarning", "shadowInactive", "shadowLastChange",
      "shadowExpire", "gosaDefaultPrinter", "uid","accessTo","trustModel");
  var $objectclasses= array("posixAccount", "shadowAccount");

  var $uid ="";

  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function posixAccount ($config, $dn= NULL, $parent= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    /* Load bases attributes */
    plugin::plugin($config, $dn, $parent);

    /* set user id */    
    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }

    $ldap= $this->config->get_ldap_link();

    if ($dn != NULL){

      /* Correct is_account. shadowAccount is not required. */
      if (isset($this->attrs['objectClass']) &&
          in_array ('posixAccount', $this->attrs['objectClass'])){

        $this->is_account= TRUE;
      }

      /* Is this account a trustAccount? */
      if ($this->is_account && isset($this->attrs['trustModel'])){
        $this->trustModel= $this->attrs['trustModel'][0];
        $this->was_trust_account= TRUE;
      } else {
        $this->was_trust_account= FALSE;
        $this->trustModel= "";
      }
	 
	  $this->accessTo = array(); 
      if ($this->is_account && isset($this->attrs['accessTo'])){
        for ($i= 0; $i<$this->attrs['accessTo']['count']; $i++){
          $tmp= $this->attrs['accessTo'][$i];
          $this->accessTo[$tmp]= $tmp;
        }
      }
      $this->initially_was_account= $this->is_account;

      /* Fill group */
      $this->primaryGroup= $this->gidNumber;

      /* Generate status text */
      $current= date("U");

      $current= floor($current / 60 /60 / 24);

      if (($current >= $this->shadowExpire) && $this->shadowExpire){
        $this->status= _("expired");
        if (($current - $this->shadowExpire) < $this->shadowInactive){
          $this->status.= ", "._("grace time active");
        }
      } elseif (($this->shadowLastChange + $this->shadowMin) >= $current){
        $this->status= _("active, password not changable");
      } elseif (($this->shadowLastChange + $this->shadowMax) >= $current){
        $this->status= _("active, password expired");
      } else {
        $this->status= _("active");
      }

      /* Get group membership */
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=posixGroup)(memberUid=".$this->uid."))", array("cn", "description"));

      while ($this->attrs= $ldap->fetch()){
        if (!isset($this->attrs["description"][0])){
          $entry= $this->attrs["cn"][0];
        } else {
          $dsc= preg_replace ('/^Group of user/', _("Group of user"), $this->attrs["description"][0]);
          $entry= $this->attrs["cn"][0]." [$dsc]";
        }
        $this->groupMembership[$ldap->getDN()]= $entry;
      }
      asort($this->groupMembership);
      reset($this->groupMembership);
      $this->savedGroupMembership= $this->groupMembership;
      $this->savedUidNumber= $this->uidNumber;
      $this->savedGidNumber= $this->gidNumber;
    }

    /* Adjust shadow checkboxes */
    foreach (array("shadowMin", "shadowMax", "shadowWarning", "shadowInactive",
          "shadowExpire") as $val){

      if ($this->$val != 0){
        $oval= "use_".$val;
        $this->$oval= "1";
      }
    }

    /* Convert to seconds */
    $this->shadowExpire= $this->convertToSeconds($this->shadowExpire);

    /* Generate shell list from CONFIG_DIR./shells */
    if (file_exists(CONFIG_DIR.'/shells')){
      $shells = file (CONFIG_DIR.'/shells');
      foreach ($shells as $line){
        if (!preg_match ("/^#/", $line)){
          $this->loginShellList[]= trim($line);
        }
      }
    } else {
      if ($this->loginShell == ""){
        $this->loginShellList[]= _("unconfigured");
      }
    }

    /* Insert possibly missing loginShell */
    if ($this->loginShell != "" && !in_array($this->loginShell, $this->loginShellList)){
      $this->loginShellList[]= $this->loginShell;
    }

    /* Set tag attribute if we've tagging activated */
    $filter= "(objectClass=posixGroup)";
    $ui= get_userinfo();
    if ($ui->gosaUnitTag != "" && isset($config->current['STRICT_UNITS']) &&
        preg_match('/TRUE/i', $config->current['STRICT_UNITS'])){
      $filter= "(&(objectClass=posixGroup)(gosaUnitTag=".$ui->gosaUnitTag."))";
    }

    /* Generate group list */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("$filter", array("cn", "gidNumber"));
    $this->secondaryGroups[]= "- "._("automatic")." -";
    while ($attrs= $ldap->fetch()){
      $this->secondaryGroups[$attrs['gidNumber'][0]]= $attrs['cn'][0];
    }
    asort ($this->secondaryGroups);

    /* Get global filter config */
    if (!is_global("sysfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $sysfilter= array( "depselect"       => $base,
          "regex"           => "*");
      register_global("sysfilter", $sysfilter);
    }
    $this->ui = get_userinfo();
  }


  /* execute generates the html output for this node */
  function execute($isCopyPaste = false)
  {
	/* Call parent execute */
	plugin::execute();
  $display= "";

  /* Department has changed? */
  if(isset($_POST['depselect'])){
    $_SESSION['CurrentMainBase']= validate($_POST['depselect']);
  }

  if(!$isCopyPaste){

    $required = (isset($this->parent->by_object['sambaAccount']) && $this->parent->by_object['sambaAccount']->is_account) || 
                (isset($this->parent->by_object['environment'] ) && $this->parent->by_object['environment'] ->is_account);

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      if($this->is_account && $this->acl == "#all#" && !$required ){
        $this->is_account= !$this->is_account;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
      }
    }

    /* Do we represent a valid posixAccount? */
    if (!$this->is_account && $this->parent == NULL ){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no unix extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }


    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        if (isset($this->parent->by_object['sambaAccount'])){
          $obj= $this->parent->by_object['sambaAccount'];
        }
        if (isset($obj) && $obj->is_account == TRUE &&
            ((isset($this->parent->by_object['sambaAccount']))&&($this->parent->by_object['sambaAccount']->is_account))
            ||(isset($this->parent->by_object['environment'] ))&&($this->parent->by_object['environment'] ->is_account)){

          /* Samba3 dependency on posix accounts are enabled
             in the moment, because I need to rely on unique
             uidNumbers. There'll be a better solution later
             on. */
          $display= $this->show_header(_("Remove posix account"),
              _("This account has unix features enabled. To disable them, you'll need to remove the samba / environment account first."), TRUE);
        } else {
          $display= $this->show_header(_("Remove posix account"),
              _("This account has posix features enabled. You can disable them by clicking below."));
        }
      } else {
        $display= $this->show_header(_("Create posix account"),
            _("This account has posix features disabled. You can enable them by clicking below."));
        return($display);
      }
    }
  }
  /* Trigger group edit? */
  if (isset($_POST['edit_groupmembership'])){
    $this->group_dialog= TRUE;
    $this->dialog= TRUE;
  }

  /* Cancel group edit? */
  if (isset($_POST['add_groups_cancel']) ||
      isset($_POST['add_groups_finish'])){
    $this->group_dialog= FALSE;
    $this->dialog= FALSE;
  }

  /* Add selected groups */
  if (isset($_POST['add_groups_finish']) && isset($_POST['groups']) &&
      count($_POST['groups'])){

      if (chkacl ($this->acl, "memberUid") == ""){
        $this->addGroup ($_POST['groups']);
        $this->is_modified= TRUE;
      }
    }

    /* Delete selected groups */
    if (isset($_POST['delete_groupmembership']) && 
        isset($_POST['group_list']) && count($_POST['group_list'])){

      if (chkacl ($this->acl, "memberUid") == ""){
        $this->delGroup ($_POST['group_list']);
        $this->is_modified= TRUE;
      }
    }

    /* Add user workstation? */
    if (isset($_POST["add_ws"]) && chkacl($this->acl,"allowedHosts") == ""){
      $this->show_ws_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Add user workstation? */
    if (isset($_POST["add_ws_finish"]) && isset($_POST['wslist'])){
      foreach($_POST['wslist'] as $ws){
        $this->accessTo[$ws]= $ws;
      }
      ksort($this->accessTo);
      $this->is_modified= TRUE;
    }

    /* Remove user workstations? */
    if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list']) && chkacl($this->acl,"allowedHosts") == ""){
      foreach($_POST['workstation_list'] as $name){
        unset ($this->accessTo[$name]);
      }
      $this->is_modified= TRUE;
    }

    /* Add user workstation finished? */
    if (isset($_POST["add_ws_finish"]) || isset($_POST["add_ws_cancel"])){
      $this->show_ws_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Templates now! */
    $smarty= get_smarty();

    /* Show ws dialog */
    if ($this->show_ws_dialog){
      /* Save data */
      $sysfilter= get_global("sysfilter");
      foreach( array("depselect", "regex") as $type){
        if (isset($_POST[$type])){
          $sysfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $sysfilter['regex']= $s;
      }
      register_global("sysfilter", $sysfilter);

      /* Get workstation list */
      $exclude= "";
      foreach($this->accessTo as $ws){
        $exclude.= "(cn=$ws)";
      }
      if ($exclude != ""){
        $exclude= "(!(|$exclude))";
      }
      $acl= array($this->config->current['BASE'] => ":all");
      $regex= $sysfilter['regex'];
      $filter= "(&(|(objectClass=goServer)(objectClass=gotoWorkstation)(objectClass=gotoTerminal))$exclude(cn=*)(cn=$regex))";
      $res= get_list($filter, $acl, $sysfilter['depselect'], array("cn"), GL_SUBSEARCH | GL_SIZELIMIT);
      $wslist= array();
      foreach ($res as $attrs){
        $wslist[]= preg_replace('/\$/', '', $attrs['cn'][0]);
      }
      asort($wslist);
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("depselect", "regex") as $type){
        $smarty->assign("$type", $sysfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $smarty->assign("wslist", $wslist);
      $smarty->assign("apply", apply_filter());
      $display= $smarty->fetch (get_template_path('trust_machines.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Manage group add dialog */
    if ($this->group_dialog){

      /* Get global filter config */
      $this->reload();

      /* remove already assigned groups */
      $glist= array();
      foreach ($this->grouplist as $key => $value){
        if (!isset($this->groupMembership[$key])){
          $glist[$key]= $value;
        }
      }

      if($this->SubSearch){
        $smarty->assign("SubSearchCHK"," checked ");
      }else{
        $smarty->assign("SubSearchCHK","");
      }

      $smarty->assign("regex",$this->GroupRegex);
      $smarty->assign("guser",$this->GroupUserRegex);
      $smarty->assign("groups", $glist);
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      $smarty->assign("depselect",$_SESSION['CurrentMainBase']);
      $smarty->assign("hint", print_sizelimit_warning());

      $smarty->assign("apply", apply_filter());
      $display.= $smarty->fetch (get_template_path('posix_groups.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Show main page */
    $smarty= get_smarty();

    /* Depending on pwmode, currently hardcoded because there are no other methods */
    if ( 1 == 1 ){
      $smarty->assign("pwmode", dirname(__FILE__)."/posix_shadow");
      $shadowMinACL= chkacl($this->acl, "shadowMin");
      $smarty->assign("shadowmins", sprintf(_("Password can't be changed up to %s days after last change"), "<input name=\"shadowMin\" size=3 maxlength=4 $shadowMinACL value=\"".$this->shadowMin."\">"));
      $shadowMaxACL= chkacl($this->acl, "shadowMax");
      $smarty->assign("shadowmaxs", sprintf(_("Password must be changed after %s days"), "<input name=\"shadowMax\" size=3 maxlength=4 $shadowMaxACL value=\"".$this->shadowMax."\">"));
      $shadowInactiveACL= chkacl($this->acl, "shadowInactive");
      $smarty->assign("shadowinactives", sprintf(_("Disable account after %s days of inactivity after password expiery"), "<input name=\"shadowInactive\" size=3 maxlength=4 $shadowInactiveACL value=\"".$this->shadowInactive."\">"));
      $shadowWarningACL= chkacl($this->acl, "shadowWarning");
      $smarty->assign("shadowwarnings", sprintf(_("Warn user %s days before password expiery"), "<input name=\"shadowWarning\" size=3 maxlength=4 $shadowWarningACL value=\"".$this->shadowWarning."\">"));
      foreach( array("must_change_password", "use_shadowMin", "use_shadowMax",
            "use_shadowExpire", "use_shadowInactive",
            "use_shadowWarning") as $val){
        if ($this->$val == 1){
          $smarty->assign("$val", "checked");
        } else {
          $smarty->assign("$val", "");
        }
        $smarty->assign("$val"."ACL", chkacl($this->acl, $val));
      }
    }

    /* Fill calendar */
   
    /* If this $this->shadowExpire is empty 
        use current date as base for calculating selectbox values.
       (This attribute is empty if this is a new user )*/ 
    if(empty($this->shadowExpire)){
      $date= getdate(time());
    }else{
      $date= getdate($this->shadowExpire);
    }

    $days= array();
    for($d= 1; $d<32; $d++){
      $days[$d]= $d;
    }
    $years= array();
    for($y= $date['year']-10; $y<$date['year']+10; $y++){
      $years[]= $y;
    }
    $months= array(_("January"), _("February"), _("March"), _("April"),
        _("May"), _("June"), _("July"), _("August"), _("September"),
        _("October"), _("November"), _("December"));
    $smarty->assign("day", $date["mday"]);
    $smarty->assign("days", $days);
    $smarty->assign("months", $months);
    $smarty->assign("month", $date["mon"]-1);
    $smarty->assign("years", $years);
    $smarty->assign("year", $date["year"]);

    /* Fill arrays */
    $smarty->assign("shells", $this->loginShellList);
    $smarty->assign("secondaryGroups", $this->secondaryGroups);
    $smarty->assign("primaryGroup", $this->primaryGroup);
   if (!count($this->groupMembership)){
      $smarty->assign("groupMembership", array("&nbsp;"));
    } else {
      $smarty->assign("groupMembership", $this->groupMembership);
    }
    if (count($this->groupMembership) > 16){
      $smarty->assign("groups", "too_many_for_nfs");
    } else {
      $smarty->assign("groups", "");
    }

	/* Avoid "Undefined index: forceMode" */
    $smarty->assign("forceMode", "");

    /* Checkboxes */
    if ($this->force_ids == 1){
      $smarty->assign("force_ids", "checked");
      if ($_SESSION['js']){
        $smarty->assign("forceMode", "");
      }
    } else {
      if ($_SESSION['js']){
		if($this->acl != "#none#")
        $smarty->assign("forceMode", "disabled");
      }
      $smarty->assign("force_ids", "");
    }
    $smarty->assign("force_idsACL", chkacl($this->acl, "force_ids"));

    /* Load attributes and acl's */
    foreach($this->attributes as $val){
      if((chkacl($this->acl,$val)=="")&&(($_SESSION["js"])&&(($val=="uidNumber")||($val=="gidNumber"))))
        {
          $smarty->assign("$val"."ACL","");
          $smarty->assign("$val", $this->$val);
          continue;
        }
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl,$val));
    }
    $smarty->assign("groupMembershipACL", chkacl($this->acl, "groupMembership"));
    $smarty->assign("status", $this->status);

    /* Work on trust modes */
    $smarty->assign("trustmodeACL", chkacl($this->acl, "trustmode"));
    if ($this->trustModel == "fullaccess"){
      $trustmode= 1;
      // pervent double disable tag in html code, this will disturb our clean w3c html
    
    if(chkacl($this->acl, "trustmode")==""){
          $smarty->assign("trusthide", "disabled");
      }else{
          $smarty->assign("trusthide", "");
      }

    } elseif ($this->trustModel == "byhost"){
      $trustmode= 2;
      $smarty->assign("trusthide", "");
    } else {
      // pervent double disable tag in html code, this will disturb our clean w3c html
      if(chkacl($this->acl, "trustmode")==""){
          $smarty->assign("trusthide", "disabled");
      }else{
          $smarty->assign("trusthide", "");
      }
      $trustmode= 0;
    }
    $smarty->assign("trustmode", $trustmode);
    $smarty->assign("trustmodes", array( 0 => _("disabled"), 1 => _("full access"),
          2 => _("allow access to these hosts")));



    if((count($this->accessTo))==0)
      $smarty->assign("emptyArrAccess",true);
    else
      $smarty->assign("emptyArrAccess",false);
    


    $smarty->assign("workstations", $this->accessTo);

    $smarty->assign("apply", apply_filter());
    $display.= $smarty->fetch (get_template_path('generic.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }


  /* remove object from parent */
  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    /* include global link_info */
    $ldap= $this->config->get_ldap_link();
    
	/* Remove and write to LDAP */
    plugin::remove_from_parent();

    /* Zero out array */
    $this->attrs['gosaHostACL']= array();

    /* Keep uid, because we need it for authentification! */
    unset($this->attrs['uid']);
    unset($this->attrs['trustModel']);

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing UNIX account failed"));

    /* Delete group only if cn is uid and there are no other
       members inside */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))", array("cn", "memberUid"));
    if ($ldap->count() != 0){
      $attrs= $ldap->fetch();
      if ($attrs['cn'][0] == $this->uid &&
          !isset($this->attrs['memberUid'])){

        $ldap->rmDir($ldap->getDN());
      }
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove", array("uid" => $this->uid));
  }


  function save_object()
  {
    if (isset($_POST['posixTab'])){
      /* Save values to object */
      plugin::save_object();

      /* Save force GID attribute */
      if (chkacl ($this->acl, "force_ids") == ""){
        if (isset ($_POST['force_ids'])){
          $data= 1;
        } else {
          $data= 0;
        }
        if ($this->force_ids != $data){
          $this->is_modified= TRUE;
        }
        $this->force_ids= $data;

        $data= $_POST['primaryGroup'];
        if ($this->primaryGroup != $data){
          $this->is_modified= TRUE;
        }
        $this->primaryGroup= $_POST['primaryGroup'];
        if ($this->primaryGroup != 0){
          $this->gidNumber= $this->primaryGroup;
        }
      }

      /* Save pwmode dependent attributes, curently hardcoded because there're
         no alternatives */
      if (1 == 1){
        foreach( array("must_change_password", "use_shadowMin", "use_shadowMax",
              "use_shadowExpire", "use_shadowInactive",
              "use_shadowWarning") as $val){
          if (chkacl($this->acl, "$val") == ""){
            if (isset ($_POST[$val])){
              $data= 1;
            } else {
              $data= 0;
            }
            if ($data != $this->$val){
              $this->is_modified= TRUE;
            }
            $this->$val= $data;
          }
        }
      }

      /* Trust mode - special handling */
      if (isset($_POST['trustmode']) && chkacl($this->acl,"allowedHosts") == ""){
        $saved= $this->trustModel;
        if ($_POST['trustmode'] == "1"){
          $this->trustModel= "fullaccess";
        } elseif ($_POST['trustmode'] == "2"){
          $this->trustModel= "byhost";
        } else {
          $this->trustModel= "";
        }
        if ($this->trustModel != $saved){
          $this->is_modified= TRUE;
        }
      }
    }

    /* Get regex from alphabet */
    if(isset($_GET['search'])){
      $this->GroupRegex = $_GET['search']."*";
    }

    /* Check checkboxes and regexes */
    if(isset($_POST["PosixGroupDialogPosted"])){
      if(isset($_POST['SubSearch']) && ($_POST['SubSearch'])){
        $this->SubSearch = true;
      }else{
        $this->SubSearch = false;
      }
    
      if(isset($_POST['guser'])){
        $this->GroupUserRegex = $_POST['guser'];
      }
      if(isset($_POST['regex'])){
        $this->GroupRegex = $_POST['regex'];
      }
    }
    $this->GroupRegex = preg_replace("/\*\**/","*",$this->GroupRegex);
    $this->GroupUserRegex = preg_replace("/\*\**/","*",$this->GroupUserRegex);
  }


  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
	
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Adapt shadow values */
    if (!$this->use_shadowExpire){
      $this->shadowExpire= "0";
    } else {
      /* Transform seconds to days here */
      $this->shadowExpire= (int)($this->shadowExpire / (60 * 60 * 24)) ;
    }
    if (!$this->use_shadowMax){
      $this->shadowMax= "0";
    }
    if ($this->must_change_password){
      $this->shadowLastChange= (int)(date("U") / 86400) - $this->shadowMax - 1;
    } else {
      $this->shadowLastChange= (int)(date("U") / 86400);
    }
    if (!$this->use_shadowWarning){
      $this->shadowWarning= "0";
    }

    /* Check what to do with ID's */
    if ($this->force_ids == 0){

      /* Use id's that are already set */
      if ($this->savedUidNumber != ""){
        $this->uidNumber= $this->savedUidNumber;
        $this->gidNumber= $this->savedGidNumber;
      } else {

        /* Calculate new id's. We need to place a lock before calling get_next_id
           to get real unique values. */
        $wait= 10;
        while (get_lock("uidnumber") != ""){
          sleep (1);

          /* Oups - timed out */
          if ($wait-- == 0){
            print_red (_("Failed: overriding lock"));
            break;
          }
        }

        add_lock ("uidnumber", "gosa");
        $this->uidNumber= $this->get_next_id("uidNumber", $this->dn);
        if ($this->savedGidNumber != ""){
          $this->gidNumber= $this->savedGidNumber;
        } else {
          $this->gidNumber= $this->get_next_id("gidNumber", $this->dn);
        }
      }

      if ($this->primaryGroup != 0){
        $this->gidNumber= $this->primaryGroup;
      }
    }

    if ($this->use_shadowMin != "1" ) {
      $this->shadowMin = "";
    }

    if (($this->use_shadowMax != "1") && ($this->must_change_password != "1")) {
      $this->shadowMax = "";
    }

    if ($this->use_shadowWarning != "1" ) {
      $this->shadowWarning = "";
    }

    if ($this->use_shadowInactive != "1" ) {
      $this->shadowInactive = "";
    }

    if ($this->use_shadowExpire != "1" ) {
      $this->shadowExpire = "";
    }

    /* Fill gecos */
    if (isset($this->parent) && $this->parent != NULL){
      $this->gecos= rewrite($this->parent->by_object['user']->cn);
      if (!preg_match('/^[a-z0-9 -]+$/i', $this->gecos)){
        $this->gecos= "";
      }
    }

	foreach(array("shadowMin","shadowMax","shadowWarning","shadowInactive","shadowExpire") as $attr){
		$this->$attr = (int) $this->$attr;
	}
    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Trust accounts */
    $objectclasses= array();
    foreach ($this->attrs['objectClass'] as $key => $class){
      if (preg_match('/trustAccount/i', $class)){
        continue;
      }
      $objectclasses[]= $this->attrs['objectClass'][$key];
    }
    $this->attrs['objectClass']= $objectclasses;
    if ($this->trustModel != ""){
      $this->attrs['objectClass'][]= "trustAccount";
      $this->attrs['trustModel']= $this->trustModel;
      $this->attrs['accessTo']= array();
      if ($this->trustModel == "byhost"){
        foreach ($this->accessTo as $host){
          $this->attrs['accessTo'][]= $host;
        }
      }
    } else {
      if ($this->was_trust_account){
        $this->attrs['accessTo']= array();
        $this->attrs['trustModel']= array();
      }
    }

    if(empty($this->attrs['gosaDefaultPrinter'])){
      $thid->attrs['gosaDefaultPrinter']=array();
    }


    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    unset($this->attrs['uid']);
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving UNIX account failed"));

    /* Remove lock needed for unique id generation */
    del_lock ("uidnumber");


    /* Posix accounts have group interrelationship, 
        take care about these here if this is a new user without forced gidNumber. */
    if ($this->force_ids == 0 && $this->primaryGroup == 0 && !$this->initially_was_account){
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))", array("cn"));

      /* Create group if it doesn't exist */
      if ($ldap->count() == 0){
        $groupdn= preg_replace ('/^'.$this->config->current['DNMODE'].'=[^,]+,'.get_people_ou().'/i', 'cn='.$this->uid.','.get_groups_ou(), $this->dn);
        $g= new group($this->config, $groupdn);
        $g->cn= $this->uid;
        $g->force_gid= 1;
        $g->gidNumber= $this->gidNumber;
        $g->description= _("Group of user")." ".$this->givenName." ".$this->sn;
        $g->save ();
      }
    }
 
    /* Take care about groupMembership values: add to groups */
    foreach ($this->groupMembership as $key => $value){
      if (!isset($this->savedGroupMembership[$key])){
        $g= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $key, false);
        $g->by_object['group']->addUser($this->uid);
        $g->save();
      }
    }
    
    /* Remove from groups not listed in groupMembership */
    foreach ($this->savedGroupMembership as $key => $value){
      if (!isset($this->groupMembership[$key])){
        $g= new grouptabs($this->config,$this->config->data['TABS']['GROUPTABS'], $key);
        $g->by_object['group']->removeUser ($this->uid);
        $g->save();
      }
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" , $this->uid));
    }
  }

  /* Check formular input */
  function check()
  {
    /* Include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Append groups as memberGroup: to check hook 
     */
    $tmp_attributes  = $this->attributes;    
    $this->attributes[] = "memberGroup";
    $this->memberGroup = array();
    foreach($this->groupMembership as $dn => $name){
      $this->memberGroup[] = $name;
    }

    /* Call common method to give check the hook */
    $message= plugin::check();
    $this->attributes = $tmp_attributes;

    /* must: homeDirectory */
    if ($this->homeDirectory == ""){
      $message[]= _("The required field 'Home directory' is not set.");
    }
    if (!is_path($this->homeDirectory)){
      $message[]= _("Please enter a valid path in 'Home directory' field.");
    }

    /* Check ID's if they are forced by user */
    if ($this->force_ids == "1"){

      /* Valid uid/gid? */
      if (!is_id($this->uidNumber)){
        $message[]= _("Value specified as 'UID' is not valid.");
      } else {
        if ($this->uidNumber < $this->config->current['MINID']){
          $message[]= _("Value specified as 'UID' is too small.");
        }
      }
      if (!is_id($this->gidNumber)){
        $message[]= _("Value specified as 'GID' is not valid.");
      } else {
        if ($this->gidNumber < $this->config->current['MINID']){
          $message[]= _("Value specified as 'GID' is too small.");
        }
      }
    }

    /* Check shadow settings, well I like spaghetties... */
    if ($this->use_shadowMin){
      if (!is_id($this->shadowMin)){
        $message[]= _("Value specified as 'shadowMin' is not valid.");
      }
    }
    if ($this->use_shadowMax){
      if (!is_id($this->shadowMax)){
        $message[]= _("Value specified as 'shadowMax' is not valid.");
      }
    }
    if ($this->use_shadowWarning){
      if (!is_id($this->shadowWarning)){
        $message[]= _("Value specified as 'shadowWarning' is not valid.");
      }
      if (!$this->use_shadowMax){
        $message[]= _("'shadowWarning' without 'shadowMax' makes no sense.");
      }
      if ($this->shadowWarning > $this->shadowMax){
        $message[]= _("Value specified as 'shadowWarning' should be smaller than 'shadowMax'.");
      }
      if ($this->use_shadowMin && $this->shadowWarning < $this->shadowMin){
        $message[]= _("Value specified as 'shadowWarning' should be greater than 'shadowMin'.");
      }
    }
    if ($this->use_shadowInactive){
      if (!is_id($this->shadowInactive)){
        $message[]= _("Value specified as 'shadowInactive' is not valid.");
      }
      if (!$this->use_shadowMax){
        $message[]= _("'shadowInactive' without 'shadowMax' makes no sense.");
      }
    }
    if ($this->use_shadowMin && $this->use_shadowMax){
      if ($this->shadowMin > $this->shadowMax){
        $message[]= _("Value specified as 'shadowMin' should be smaller than 'shadowMax'.");
      }
    }

  //  if(empty($this->gosaDefaultPrinter)){
  //    $message[]= _("You need to specify a valid default printer.");
  //  }

    return ($message);
  }

  function addGroup ($groups)
  {
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Walk through groups and add the descriptive entry if not exists */
    foreach ($groups as $value){
      if (!array_key_exists($value, $this->groupMembership)){
        $ldap->cat($value, array('cn', 'description', 'dn'));
        $attrs= $ldap->fetch();
        error_reporting (0);
        if (!isset($attrs['description'][0])){
          $entry= $attrs["cn"][0];
        } else {
          $dsc= preg_replace ('/^Group of user/', _("Group of user"), $attrs["description"][0]);
          $entry= $attrs["cn"][0]." [$dsc]";
        }
        error_reporting (E_ALL);
        $this->groupMembership[$attrs['dn']]= $entry;
      }
    }

    /* Sort groups */
    asort ($this->groupMembership);
    reset ($this->groupMembership);
  }


  /* Del posix user from some groups */
  function delGroup ($groups)
  {
    $dest= array();

    foreach ($this->groupMembership as $key => $value){
      if (!in_array($key, $groups)){
        $dest[$key]= $value;
      }
    }
    $this->groupMembership= $dest;
  }

  /* Adapt from template, using 'dn' */
  function adapt_from_template($dn)
  {
    /* Include global link_info */
    $ldap= $this->config->get_ldap_link();

    plugin::adapt_from_template($dn);
    $template= $this->attrs['uid'][0];

    /* Adapt group membership */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=posixGroup)(memberUid=".$this->attrs["uid"][0]."))", array("description", "cn"));

    while ($this->attrs= $ldap->fetch()){
      if (!isset($this->attrs["description"][0])){
        $entry= $this->attrs["cn"][0];
      } else {
        $entry= $this->attrs["cn"][0]." [".$this->attrs["description"][0]."]";
      }
      $this->groupMembership[$ldap->getDN()]= $entry;
    }

    /* Fix primary group settings */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=posixGroup)(cn=$template)(gidNumber=".$this->gidNumber."))", array("cn"));
    if ($ldap->count() != 1){
      $this->primaryGroup= $this->gidNumber;
    }

    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=gosaUserTemplate)(uid=".$template.")(accessTo=*))", array("cn","accessTo"));

    while($attr = $ldap->fetch()){
      $tmp = $attr['accessTo'];
      unset ($tmp['count']);
      $this->accessTo = $tmp;	
    }

    /* Adjust shadow checkboxes */
    foreach (array("shadowMin", "shadowMax", "shadowWarning", "shadowInactive") as $val){
      if ($this->$val != 0){
        $oval= "use_".$val;
        $this->$oval= "1";
      }
    }

    /* 
     * If shadowExpire is not enabled in the template, it's a UNIX timestamp - so don't convert it to seconds.
     * The check is a hack - if difference between timestamp generated above and here is max 1 day.
     */
    if(abs($this->shadowExpire - time())>86400) {
      $this->shadowExpire= $this->convertToSeconds($this->shadowExpire);
    }
    
    /* Only enable checkbox, if shadowExpire is in the future */
    if($this->shadowExpire > time()) {
      $this->use_shadowExpire= "1";
    }
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

    /* Add the nobody id */
    $ids[]= 65534;

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

    /* Check if current id reaches the maximum of 32 bit */
    if ($id >= pow(2,32)){
      echo _("Too many users, can't allocate a free ID!");
      exit;
    }
  }

 function reload()
  {
    /* Set base for all searches */
    $base     = $_SESSION['CurrentMainBase'];
    $ldap     = $this->config->get_ldap_link();    
    $attrs    =  array("cn", "description", "gidNumber");
    $Flags    = GL_SIZELIMIT;

    /* Get groups */
    if ($this->GroupUserRegex == '*'){
      $filter = "(&(objectClass=posixGroup)(cn=".$this->GroupRegex."))";
    } else {
      $filter= "(&(objectClass=posixGroup)(cn=".$this->GroupRegex.")(memberUid=".$this->GroupUserRegex."))";
    }
    if($this->SubSearch){
      $Flags |= GL_SUBSEARCH;
    }else{
      $base = get_groups_ou().$base;
    }


    $res= get_list($filter, $this->ui->subtreeACL, $base,$attrs, $Flags);

    /* check sizelimit */
    if (preg_match("/size limit/i", $ldap->error)){
      $_SESSION['limit_exceeded']= TRUE;
    }

    /* Create a list of users */
    $this->grouplist = array();
    foreach ($res as $value){
        $this->grouplist[$value['gidNumber'][0]]= $value;
    }

    $tmp=array();
    foreach($this->grouplist as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }

    /* Sort index */
    ksort($tmp);

    /* Recreate index array[dn]=cn[description]*/
    $this->grouplist=array();
    foreach($tmp as $val){
      if(isset($val['description'])){
        $this->grouplist[$val['dn']]=$val['cn'][0]."&nbsp;[".$val['description'][0]."]";
      }else{
        $this->grouplist[$val['dn']]=$val['cn'][0];
      }
    }
    reset ($this->grouplist);
  }

 
  /* Get posts from copy & paste dialog */ 
  function saveCopyDialog()
  {
    if(isset($_POST['homeDirectory'])){
      $this->homeDirectory = $_POST['homeDirectory'];
      if (isset ($_POST['force_ids'])){
        $data= 1;
        $this->gidNumber = $_POST['gidNumber'];
        $this->uidNumber = $_POST['uidNumber'];
      } else {
        $data= 0;
      }
      if ($this->force_ids != $data){
        $this->is_modified= TRUE;
      }
      $this->force_ids= $data;
    }
  }


  /* Create the posix dialog part for copy & paste */
  function getCopyDialog()
  {
    /* Skip dialog creation if this is not a valid account */
    if(!$this->is_account) return("");
    if ($this->force_ids == 1){
      $force_ids = "checked";
      if ($_SESSION['js']){
        $forceMode = "";
      }
    } else {
      if ($_SESSION['js']){
        if($this->acl != "#none#")
          $forceMode ="disabled";
      }
      $force_ids = "";
    }
   
    $sta = "";
 
    /* Open group add dialog */
    if(isset($_POST['edit_groupmembership'])){
      $this->group_dialog = TRUE;
      $sta = "SubDialog";
    }

    /* If the group-add dialog is closed, call execute 
        to ensure that the membership is updatd */
    if(isset($_POST['add_groups_finish']) || isset($_POST['add_groups_cancel'])){
      $this->execute();
      $this->group_dialog =FALSE;
    }

    if($this->group_dialog){
      $str = $this->execute(true);
      $ret = array();
      $ret['string'] = $str;
      $ret['status'] = $sta;
      return($ret);
    }

    /* If a group member should be deleted, simply call execute */
    if(isset($_POST['delete_groupmembership'])){
      $this->execute();
    }

    /* Assigned informations to smarty */
    $smarty = get_smarty();
    $smarty->assign("homeDirectory",$this->homeDirectory);
    $smarty->assign("uidNumber",$this->uidNumber);
    $smarty->assign("gidNumber",$this->gidNumber);
    $smarty->assign("forceMode",$forceMode);
    $smarty->assign("force_ids",$force_ids);
    if (!count($this->groupMembership)){
      $smarty->assign("groupMembership", array("&nbsp;"));
    } else {
      $smarty->assign("groupMembership", $this->groupMembership);
    }

    /* Display wars message if there are more than 16 group members */
    if (count($this->groupMembership) > 16){
      $smarty->assign("groups", "too_many_for_nfs");
    } else {
      $smarty->assign("groups", "");
    }
    $str = $smarty->fetch(get_template_path("paste_generic.tpl",TRUE,dirname(__FILE__)));

    $ret = array();
    $ret['string'] = $str;
    $ret['status'] = $sta;
    return($ret);
  }

  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);
    
    /* Avoid using the same gid/uid number as source user */
    $this->savedUidNumber = $this->get_next_id("uidNumber", $this->dn);
    $this->savedGidNumber = $this->get_next_id("gidNumber", $this->dn);
  }

  function convertToSeconds($val)
  {
    if ($val != 0){
      $val*= 60 * 60 * 24;
    } else {
      $date= getdate();
      $val= floor($date[0] / (60*60*24)) * 60 * 60 * 24;
    }
    return($val);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
