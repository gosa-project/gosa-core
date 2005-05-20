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

  /* CLI vars */
  var $cli_summary= "Manage users posix account";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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
  var $gosaDefaultLanguage= "";
  var $accessTo= array();
  var $trustModel= "";

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
  var $printerList= array();
  var $group_dialog= FALSE;
  var $show_ws_dialog= FALSE;
  var $secondaryGroups= array();
  var $primaryGroup= 0;
  var $was_trust_account= FALSE;


  /* attribute list for save action */
  var $attributes= array("homeDirectory", "loginShell", "uidNumber", "gidNumber", "gecos",
      "shadowMin", "shadowMax", "shadowWarning", "shadowInactive", "shadowLastChange",
      "shadowExpire", "gosaDefaultPrinter", "gosaDefaultLanguage", "uid");
  var $objectclasses= array("posixAccount", "shadowAccount");


  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function posixAccount ($config, $dn= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    /* Load bases attributes */
    plugin::plugin($config, $dn);

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
      if (($current >= $this->shadowExpire) && $this->shadowExpire){
        $this->status= "expired";
        if (($this->shadowExpire - $current) < $this->shadowInactive){
          $this->status.= ", grace time active";
        }
      } elseif (($this->shadowLastChange + $this->shadowMin) >= $current){
        $this->status= "active, password not changable";
      } elseif (($this->shadowLastChange + $this->shadowMax) >= $current){
        $this->status= "active, password expired";
      } else {
        $this->status= "active";
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
    if ($this->shadowExpire != 0){
      $this->shadowExpire*= 60 * 60 * 24;
    } else {
      $date= getdate();
      $this->shadowExpire= floor($date[0] / (60*60*24)) * 60 * 60 * 24;
    }

    /* Generate shell list from /etc/gosa/shells */
    if (file_exists('/etc/gosa/shells')){
      $shells = file ('/etc/gosa/shells');
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

    /* Generate printer list */
    if (isset($this->config->data['SERVERS']['CUPS'])){
      $this->printerList= get_printer_list ($this->config->data['SERVERS']['CUPS']);
      asort($this->printerList);
    }

    /* Generate group list */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=posixGroup)", array("cn", "gidNumber"));
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
  }


  /* execute generates the html output for this node */
  function execute()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid posixAccount? */
    if (!$this->is_account && $this->parent == NULL ){
      $display= "<img src=\"images/stop.png\" align=center>&nbsp;<b>".
        _("This account has no unix extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }

    $display= "";

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        if (isset($this->parent->by_object['sambaAccount'])){
          $obj= $this->parent->by_object['sambaAccount'];
        }
        if (isset($obj) && $obj->is_account == TRUE &&
            isset($this->parent->by_object['sambaAccount'])){

          /* Samba3 dependency on posix accounts are enabled
             in the moment, because I need to rely on unique
             uidNumbers. There'll be a better solution later
             on. */
          $display= $this->show_header(_("Remove posix account"),
              _("This account has unix features enabled. To disable them, you'll need to remove the samba account first."), TRUE);
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
    if (isset($_POST["add_ws"])){
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
    if (isset($_POST["delete_ws"]) && isset($_POST['workstation_list'])){
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
      $res= get_list($acl, "$filter", TRUE, $sysfilter['depselect'], array("cn"), TRUE);
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
      $gd= new groupManagement($this->config, get_userinfo());

      /* Save data */
      $groupfilter= get_global("groupfilter");
      foreach( array("depselect", "guser", "regex") as $type){
        if (isset($_POST[$type])){
          $groupfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_POST['depselect'])){
        foreach( array("primarygroups", "sambagroups", "mailgroups", "appgroups",
              "functionalgroups") as $type){

          if (isset($_POST[$type])) {
            $groupfilter[$type]= "checked";
          } else {
            $groupfilter[$type]= "";
          }
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $groupfilter['regex']= $s;
      }
      register_global("groupfilter", $groupfilter);

      /* Calculate actual groups */
      $gd->reload();
      $glist= array();
      foreach ($gd->grouplist as $key => $value){
        if (!isset($this->groupMembership[$key])){
          $glist[$key]= $value;
        }
      }

      /* Show dialog */
      $smarty->assign("groups", $glist);
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("alphabet", generate_alphabet());
      foreach( array("depselect", "guser", "regex", "primarygroups", "mailgroups",
            "appgroups", "sambagroups", "functionalgroups") as $type){
        $smarty->assign("$type", $groupfilter[$type]);
      }
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
    $date= getdate($this->shadowExpire);

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
    $smarty->assign("groupMembership", $this->groupMembership);
    if (count($this->groupMembership) > 16){
      $smarty->assign("groups", "too_many_for_nfs");
    } else {
      $smarty->assign("groups", "");
    }
    $smarty->assign("printerList", $this->printerList);
    $smarty->assign("languages", $this->config->data['MAIN']['LANGUAGES']);

    /* Checkboxes */
    if ($this->force_ids == 1){
      $smarty->assign("force_ids", "checked");
    } else {
      if ($_SESSION["js"]){
        $smarty->assign("forceMode", "disabled");
      }
      $smarty->assign("force_ids", "");
    }
    $smarty->assign("force_idsACL", chkacl($this->acl, "force_ids"));

    /* Load attributes and acl's */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl,$val));
    }
    $smarty->assign("groupMembershipACL", chkacl($this->acl, "groupMembership"));
    $smarty->assign("status", $this->status);

    /* Work on trust modes */
    $smarty->assign("trustmodeACL", chkacl($this->acl, "trustmode"));
    if ($this->trustModel == "fullaccess"){
      $trustmode= 1;
      $smarty->assign("trusthide", "disabled");
    } elseif ($this->trustModel == "byhost"){
      $trustmode= 2;
      $smarty->assign("trusthide", "");
    } else {
      $smarty->assign("trusthide", "disabled");
      $trustmode= 0;
    }
    $smarty->assign("trustmode", $trustmode);
    $smarty->assign("trustmodes", array( 0 => _("disabled"), 1 => _("full access"),
          2 => _("allow access to these hosts")));
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

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

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
    $this->handle_post_events("remove");
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
      if (isset($_POST['trustmode'])){
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
      $this->shadowExpire= (int)($this->shadowExpire / (60 * 60 * 24)) + 1;
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
        $this->uidNumber= $this->get_next_id("uidNumber");
        if ($this->savedGidNumber != ""){
          $this->gidNumber= $this->savedGidNumber;
        } else {
          $this->gidNumber= $this->get_next_id("gidNumber");
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

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Fill gecos */
    if (isset($this->parent) && $this->parent != NULL){
      $this->gecos= $this->parent->by_object['user']->cn;
    }

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

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Remove lock needed for unique id generation */
    del_lock ("uidnumber");


    /* Posix accounts have group interrelationship, take care about these here. */
    if ($this->force_ids == 0 && $this->primaryGroup == 0){
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=posixGroup)(gidNumber=".$this->gidNumber."))", array("cn"));

      /* Create group if it doesn't exist */
      if ($ldap->count() == 0){
        $groupdn= preg_replace ('/^'.$this->config->current['DNMODE'].'=[^,]+,'.get_people_ou().'/i', 'cn='.$this->uid.','.get_groups_ou(), $this->dn);

        $g= new group($this->config, $groupdn);
        $g->cn= $this->uid;
        $g->force_gid= 1;
        $g->gidNumber= $this->gidNumber;
        $g->description= "Group of user ".$this->givenName." ".$this->sn;
        $g->save ();
      }
    }

    /* Take care about groupMembership values: add to groups */
    foreach ($this->groupMembership as $key => $value){
      $g= new group($this->config, $key);
      $g->addUser ($this->uid);
      $g->save();

      /* May need to save the mail part, too */
      if ($g->has_mailAccount){
        $m= new mailgroup($this->config, $key);
        $m->save();
      }
    }

    /* Remove from groups not listed in groupMembership */
    foreach ($this->savedGroupMembership as $key => $value){
      if (!array_key_exists ($key, $this->groupMembership)){
        $g= new group($this->config, $key);
        $g->removeUser ($this->uid);
        $g->save();

        /* May need to save the mail part, too */
        if ($g->has_mailAccount){
          $m= new mailgroup($this->config, $key);
          $m->save();
        }
      }
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }

  /* Check formular input */
  function check()
  {
    /* Include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Reset message array */
    $message= array();

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

    return ($message);
  }


  /* Add posix user to some groups */
  function addGroup ($groups)
  {
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Walk through groups and add the descriptive entry if not exists */
    foreach ($groups as $value){
      if (!array_key_exists($value, $this->groupMembership)){
        $ldap->cat($value);
        $attrs= $ldap->fetch();
        error_reporting (0);
        if (!isset($attrs['description'][0])){
          $entry= $attrs["cn"][0];
        } else {
          $dsc= preg_replace ('/^Group of user/', _("Group of user"), $attrs["description"][0]);
          $entry= $attrs["cn"][0]." [$dsc]";
        }
        error_reporting (E_ALL);
        $this->groupMembership[$ldap->getDN()]= $entry;
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
  }

  function get_next_id($attrib)
  {
    $ids= array();
    $ldap= $this->config->get_ldap_link();

    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("($attrib=*)", array("$attrib"));

    /* Get list of ids */
    while ($attrs= $ldap->fetch()){
      $ids[]= (int)$attrs["$attrib"][0];
    }

    /* Find out next free id near to UID_BASE */
    for ($id= $this->config->current['UIDBASE']; $id++; $id<65000){
      if (!in_array($id, $ids)){
        return ($id);
      }
    }

    /* Should not happen */
    if ($id == 65000){
      print_red(_("Too many users, can't allocate a free ID!"));
      exit;
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
