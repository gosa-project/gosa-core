<?php

class mailgroup extends plugin
{
  /* CLI vars */
  var $cli_summary= "Manage mail groups/shared folders";
  var $cli_description= "Some longer text\nfor help";
  var $cli_parameters= array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");
  var $method= "mailMethod";

  /* plugin specific values */
  var $mail= "";
  var $uid= "";
  var $cn= "";
  var $gosaMailAlternateAddress= array();
  var $gosaMailForwardingAddress= array();
  var $gosaMailDeliveryMode= "[L        ]";
  var $gosaMailServer= "";
  var $gosaMailQuota= "";
  var $gosaMailMaxSize= "";
  var $gosaVacationMessage= "";
  var $gosaSpamSortLevel= "";
  var $gosaSpamMailbox= "";

  var $quotaUsage= 0;
  var $forward_dialog= FALSE;
  var $members= array();
  var $mailusers= array();
  var $perms= array();
  var $imapacl= array('anyone' => 'p', '%members%' => 'lrsp', '' => 'p');

  /* Helper */
  var $indexed_acl= array();
  var $indexed_user= array();

  /* attribute list for save action */
  var $attributes= array("mail", "gosaMailServer", "gosaMailQuota", "gosaMailMaxSize",
      "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox",
      "gosaVacationMessage");
  var $objectclasses= array("gosaMailAccount");


  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function mailgroup ($config, $dn= NULL, $ui= NULL)
  {
    /* Configuration is fine, allways */
    $this->config= $config;

    /* Load bases attributes */
    plugin::plugin($config, $dn);

    /* Set mailMethod to the one defined in gosa.conf */
    if (isset($this->config->current['MAILMETHOD'])){
      $method= $this->config->current['MAILMETHOD'];
      if (class_exists("mailMethod$method")){
        $this->method= "mailMethod$method";
      } else {
        print_red(sprintf(_("There is no mail method '%s' specified in your gosa.conf available."), $method));
      }
    }

    /* Load Mailserver string, only in case of kolab ???? */
    if(preg_match("/kolab/i",$this->config->current['MAILMETHOD'])){
      if(isset($this->attrs['gosaMailServer'][0])){
        $this->gosaMailServer =  $this->attrs['gosaMailServer'][0];
      }
    } 

    /* Convert cn to uid in case of existing entries */
    if (isset($this->attrs['cn'][0])){
      $this->uid= $this->attrs['cn'][0];
    }

    if ($dn != NULL){

      /* Load attributes containing arrays */
      foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
        if (isset($this->attrs["$val"]["count"])){
          for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }

      /* Only do IMAP actions if gosaMailServer attribute is set */
      if (isset ($this->attrs["gosaMailServer"][0])){
        $method= new $this->method($this->config);
        if ($method->connect($this->attrs["gosaMailServer"][0])){
          $quota= $method->getQuota($this->uid);

          /* Maybe the entry is not saved in new style, get
             permissions from IMAP and convert them to acl attributes */
          if (!isset($this->attrs['acl'])){
            $this->imapacl=  $method->getSharedFolderPermissions($this->uid);

            /* Need to filter what a member acl could be... */
            $vote= array();
            $peak= 0;
            $leader= "";
            foreach ($this->imapacl as $user => $acl){

              if ($user != "anyone" ){
                if (!isset($vote[$acl])){
                  $vote[$acl]= 1;
                } else {
                  $vote[$acl]++;
                }
                if ($vote[$acl] > $peak){
                  $leader= $acl;
                  $peek= $vote[$acl];
                }
              }

            }

            /* Highest count wins as %members%, remove all members
               with the same acl */
            $this->imapacl['%members%']= $leader;
            foreach ($this->imapacl as $user => $acl){
              if ($this->acl == $leader && in_array($user, $this->attrs['memberUid'])){
                unset($this->imapacl[$user]);
              }
            }

          }

          /* Update quota values */
          if ($quota['gosaMailQuota'] == 2147483647){
            $this->quotaUsage= "";
            $this->gosaMailQuota= "";
          } else {
            $this->quotaUsage= $quota['quotaUsage'];
            $this->gosaMailQuota= $quota['gosaMailQuota'];
          }
          $method->disconnect();
        }

        /* Adapt attributes if needed */
        $method->fixAttributesOnLoad($this);
      }
    }

    /* Get global filter config */
    if (!is_global("gmailfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $gmailfilter= array( "depselect"       => $base,
          "muser"            => "",
          "regex"           => "*");
      register_global("gmailfilter", $gmailfilter);
    }

    /* Load permissions */
    $tmp = array();
    if(preg_match("/kolab/i",$this->config->current['MAILMETHOD'])){
      $ldap = $this->config->get_ldap_link();
      if (isset($this->attrs['acl'])){
        for ($i= 0; $i<$this->attrs['acl']['count']; $i++){
          list($user, $permission)= split(' ', $this->attrs['acl'][$i]);

          /* Add to list */
          $this->imapacl[$user]= $permission;

          /* Get all user permissions sorted by acl, to detect the most used acl
             This acl is then used for %members%
          */
          $tmp[$permission][] = $user;

          /* There is an entry in $this->imapacl like this this ... 
                $this->attrs['imapacl']['anyone'] = "p";
                $this->attrs['imapacl']['%members%'] = "lprs";
                $this->attrs['imapacl'][''] = ""; <------ This is used to diplay an empty 
                                                          Field for special acls in our template.
                                                          If there is at least one special acl in out imapacl,
                                                           we don't need this entry anymore, because it is already displayed. 
          */
          if ($user != "anyone" && $user != "%members%"){
            unset($this->imapacl['']);
          }
        }
      }

      /* In this section we dectect which acl is tho most used 
         This will be used as %members% acl  
      */
      $tmp2 = array(); 
      foreach($tmp as $acl => $user){
        $tmp2[count($tmp[$acl])]=$acl;
      }
      /* Most used at last */
      ksort($tmp2);      
    
      /* Assign last (most used acl) to %members% acl */ 
      $this->imapacl['%members%']=array_pop($tmp2);

      /* Open ldap connection */
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);

      /* Remove those users, that use %members% acl && are member of this group. */
      foreach($this->imapacl as $mail => $permission){
        $ldap->search("(&(objectClass=person)(mail=".$mail."))",array("uid"));
        $atr = $ldap->fetch();
        if((isset($atr['uid'][0]))&&(in_array($atr['uid'][0],$this->attrs['memberUid']))&&($permission == $this->imapacl['%members%'])){
          unset($this->imapacl[$mail]);
        }
      }

      /* Append an empty entry, for special acl handling */
      if(count($this->imapacl)==2){
        $this->imapacl[''] ="";
      }
    }else{
      /* Load permissions */ 
      if (isset($this->attrs['acl'])){
        for ($i= 0; $i<$this->attrs['acl']['count']; $i++){
          list($user, $permission)= split(' ', $this->attrs['acl'][$i]);
          $this->imapacl[$user]= $permission;
          if ($user != "anyone" && $user != "%members%"){
            unset($this->imapacl['']);
          }
        }
      }
    }
    /* Fill translations */
    $this->perms["lrs"]= _("read");
    $this->perms["lrsp"]= _("post");
    $this->perms["p"]= _("external post");
    $this->perms["lrsip"]= _("append");
    $this->perms["lrswipcd"]= _("write");
  }


  function execute()
  {
    /* Call parent execute */
    //plugin::execute();

    /* Load templating engine */
    $smarty= get_smarty();
    if ($_SESSION['js']==FALSE){
      $smarty->assign("javascript", "false");
    } else {
      $smarty->assign("javascript", "true");
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no valid mail extensions.")."</b>";
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove mail account"),
            _("This account has mail features enabled. You can disable them by clicking below."));
      } else {
        $display= $this->show_header(_("Create mail account"),
            _("This account has mail features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Add ACL? */
    foreach ($this->indexed_user as $nr => $user){
      if (isset($_POST["add_$nr"])){
        $this->imapacl[""]= "l";
      }
      if (isset($_POST["del_$nr"])){
        unset ($this->imapacl[$user]);
      }
    }

    /* Trigger forward add dialog? */
    if (isset($_POST['add_local_forwarder'])){
      $this->forward_dialog= TRUE;
      $this->dialog= TRUE;
    }

    /* Cancel forward add dialog? */
    if (isset($_POST['add_locals_cancel'])){
      $this->forward_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Finished adding of locals? */
    if (isset($_POST['add_locals_finish'])){
      if (count ($_POST['local_list']) &&
          chkacl ($this->acl, "gosaMailForwardingAddress") == ""){

        /* Walk through list of forwarders, ignore own addresses */
        foreach ($_POST['local_list'] as $val){
          if (!in_array ($val, $this->gosaMailAlternateAddress) &&
              $val != $this->mail){

            $this->addForwarder($val);
          }
        }
      }
      $this->forward_dialog= FALSE;
      $this->dialog= FALSE;
    }

    /* Add forward email addresses */
    if (isset($_POST['add_forwarder'])){
      if ($_POST['forward_address'] != ""){

        /* Valid email address specified? */
        $address= $_POST['forward_address'];
        if (!is_email($address)){

          print_red (_("You're trying to add an invalid email address ".
                "to the list of forwarders."));

        } elseif ($address == $this->mail
            || in_array($address, $this->gosaMailAlternateAddress)) {

          print_red (_("Adding your one of your own addresses to the forwarders makes no sense."));

        } else {

          /* Add it */
          if (chkacl ($this->acl, "gosaMailForwardingAddress") == ""){
            $this->addForwarder ($address);
          }

        }
      }
    }

    /* Delete forward email addresses */
    if (isset($_POST['delete_forwarder'])){
      if (count($_POST['forwarder_list'])
          && chkacl ($this->acl, "gosaMailForwardingAddress") == ""){

        $this->delForwarder ($_POST['forwarder_list']);
      }
    }

    /* Add alternate email addresses */
    if (isset($_POST['add_alternate'])){
      if ($_POST['alternate_address'] != "" &&
          chkacl ($this->acl, "gosaMailAlternateAddress") == ""){

        if (!is_email($_POST['alternate_address'])){
          print_red (_("You're trying to add an invalid email address to the list of alternate addresses."));

        } elseif (($user= $this->addAlternate ($_POST['alternate_address'])) != ""){
          $ui= get_userinfo();
          if ($user != $ui->username){
            print_red (_("The address you're trying to add is already used by user")." '$user'.");
          }
        }
      }
    }

    /* Delete alternate email addresses */
    if (isset($_POST['delete_alternate']) && isset ($_POST['alternates_list'])){
      if (count($_POST['alternates_list']) &&
          chkacl ($this->acl, "gosaMailAlternateAddress") == ""){

        $this->delAlternate ($_POST['alternates_list']);
      }
    }

    /* Show forward add dialog */
    if ($this->forward_dialog){
      $ldap= $this->config->get_ldap_link();

      /* Save data */
      $gmailfilter= get_global("gmailfilter");
      foreach( array("depselect", "muser", "regex") as $type){
        if (isset($_POST[$type])){
          $gmailfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $gmailfilter['regex']= $s;
      }
      register_global("gmailfilter", $gmailfilter);

      /* Get actual list */
      $mailusers= array ();
      if ($gmailfilter['regex'] != '*' && $gmailfilter['regex'] != ""){
        $regex= $gmailfilter['regex'];
        $filter= "(|(mail=$regex)(gosaMailAlternateAddress=$regex))";
      } else {
        $filter= "";
      }
      if ($gmailfilter['muser'] != ""){
        $user= $gmailfilter['muser'];
        $filter= "$filter(|(uid=$user)(cn=$user)(givenName=$user)(sn=$user))";
      }

      /* Add already present people to the filter */
      $exclude= "";
      foreach ($this->gosaMailForwardingAddress as $mail){
        $exclude.= "(mail=$mail)";
      }
      if ($exclude != ""){
        $filter.= "(!(|$exclude))";
      }

      $acl= array($this->config->current['BASE'] => ":all");
      $res= get_list($acl, "(&(objectClass=gosaMailAccount)$filter)", TRUE, $gmailfilter['depselect'], array("sn", "mail", "givenName"), TRUE);
      $ldap->cd($gmailfilter['depselect']);
      $ldap->search ("(&(objectClass=gosaMailAccount)$filter)", array("sn", "mail", "givenName"));
      error_reporting (0);
      while ($attrs= $ldap->fetch()){
        if(preg_match('/%/', $attrs['mail'][0])){
          continue;
        }
        $name= $this->make_name($attrs);
        $mailusers[$attrs['mail'][0]]= $name."&lt;".
          $attrs['mail'][0]."&gt;";
      }
      error_reporting (E_ALL);
      natcasesort ($mailusers);
      reset ($mailusers);

      /* Show dialog */
      $smarty->assign("search_image", get_template_path('images/search.png'));
      $smarty->assign("usearch_image", get_template_path('images/search_user.png'));
      $smarty->assign("tree_image", get_template_path('images/tree.png'));
      $smarty->assign("infoimage", get_template_path('images/info.png'));
      $smarty->assign("launchimage", get_template_path('images/small_filter.png'));
      $smarty->assign("mailusers", $mailusers);
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("apply", apply_filter());
      $smarty->assign("alphabet", generate_alphabet());
      $smarty->assign("hint", print_sizelimit_warning());
      foreach( array("depselect", "muser", "regex") as $type){
        $smarty->assign("$type", $gmailfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());
      $display.= $smarty->fetch (get_template_path('mail_locals.tpl', TRUE));
      return ($display);
    }

    /* Assemble normal permissions */
    $smarty->assign("permissionsACL", chkacl($this->acl, "permissions"));
    if (isset($this->imapacl['anyone'])){
      $smarty->assign("default_permissions", $this->imapacl['anyone']);
    }
    if (isset($this->imapacl['%members%'])){
      $smarty->assign("member_permissions", $this->imapacl['%members%']);
    }

    /* Assemble extra attributes */
    $perm= chkacl($this->acl, "permissions");
    $tmp= "";
    $nr= 0;
    $count= count($this->imapacl);
    $this->indexed_user= array();
    $this->indexed_acl= array();
    foreach($this->imapacl as $user => $acl){
      if ($user != "anyone" && $user != "%members%"){
        $tmp.= "<tr><td><input name=\"user_$nr\" size=20 maxlength=60 ".
               "value=\"$user\" $perm></td><td><select size=\"1\" name=\"perm_$nr\" $perm>";
        foreach ($this->perms as $key => $value){
          if ($acl == $key){
            $tmp.= "<option value=$key selected>$value</option>";
          } else {
            $tmp.= "<option value=$key>$value</option>";
          }
        }
        $tmp.= "</select>&nbsp;";
        if ($nr == $count - 1){
          $tmp.= "<input type=submit value=\""._("Add")."\" ".
                 "name=\"add_$nr\" $perm>";
        }
        if ($count > 3){
          $tmp.= "<input type=submit value=\""._("Remove")."\" ".
                 "name=\"del_$nr\" $perm></td></tr>";
        }
      }
      $this->indexed_user[$nr]= $user;
      $this->indexed_acl[$nr++]= $acl;
    }
    $smarty->assign("plusattributes", $tmp);

    /* Show main page */
    $mailserver= array();
    foreach ($this->config->data['SERVERS']['IMAP'] as $key => $val){
      $mailserver[]= $key;
    }
    $smarty->assign("mailServers", $mailserver);
    foreach(array("gosaMailServer", "gosaMailQuota", "perms", "mail",
          "gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }
    if (is_numeric($this->gosaMailQuota) && $this->gosaMailQuota != 0){
      $smarty->assign("quotausage", progressbar(round(($this->quotaUsage * 100)/ $this->gosaMailQuota,100,15,true)));
      $smarty->assign("quotadefined", "true");
    } else {
      $smarty->assign("quotadefined", "false");
    }

    $display.= $smarty->fetch (get_template_path('mail.tpl', TRUE));
    return ($display);
  }


  /* remove object from parent */
  function remove_from_parent()
  {
    /* Added these ObjectClass and Attributes, because they were not 
       removed correctly, only in case of kolab ... 
     */
    if(preg_match("/kolab/i",$this->config->current['MAILMETHOD'])){
      $this->attributes[]="acl";
      $this->attributes[]="kolabHomeServer";
      $this->objectclasses[] = "kolabSharedFolder";
    }
    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();

    /* Zero arrays */
    $this->attrs['gosaMailAlternateAddress']= array();
    $this->attrs['gosaMailForwardingAddress']= array();
    $this->attrs['gosaSharedFolderTarget']= array();

    /* Keep uid */
    unset ($this->attrs['uid']);
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Connect to IMAP server for account deletion */
    if ($this->is_account){
      $method= new $this->method($this->config);
      if ($method->connect($this->attrs["gosaMailServer"][0])){
        /* Remove account from IMAP server */
        $method->deleteMailbox($this->uid);
        $method->disconnect();
      }
      $method->fixAttributesOnRemove($this);
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {
    /* Assemble mail delivery mode
       The mode field in ldap consists of values between braces, this must
       be called when 'mail' is set, because checkboxes may not be set when
       we're in some other dialog.

       Example for gosaMailDeliveryMode [LR        ]
       L: Local delivery
       R: Reject when exceeding mailsize limit
       S: Use spam filter
       V: Use vacation message
       C: Use custom sieve script
       I: Only insider delivery */
    if (isset($_POST['mailedit'])){

      /* Save ldap attributes */
      plugin::save_object();

      $tmp= "";
      if (!isset($_POST["drop_own_mails"])){
        $tmp.= "L";
      }
      if (isset($_POST["use_mailsize_limit"])){
        $tmp.= "R";
      }
      if (isset($_POST["use_spam_filter"])){
        $tmp.= "S";
      }
      if (isset($_POST["use_vacation"])){
        $tmp.= "V";
      }
      if (isset($_POST["own_script"])){
        $tmp.= "C";
      }
      if (isset($_POST["only_local"])){
        $tmp.= "I";
      }
      $tmp= "[$tmp]";

      if (chkacl ($this->acl, "gosaMailDeliveryMode") == ""){
        $this->gosaMailDeliveryMode= $tmp;
      }

      /* Collect data and re-assign it to the imapacl array */
      if (chkacl($this->acl, "permissions") == ""){
        $this->imapacl= array();
        $this->imapacl['%members%']= $_POST['member_permissions'];
        $this->imapacl['anyone']= $_POST['default_permissions'];
        foreach ($this->indexed_user as $nr => $user){
          if (!isset($_POST["user_$nr"])){
            continue;
          }
          if ($_POST["user_$nr"] != $user ||
              $_POST["perm_$nr"] != $this->indexed_acl[$nr]){
            $this->is_modified= TRUE;
          }
          $this->imapacl[$_POST["user_$nr"]]= $_POST["perm_$nr"];
        }
      }
    }

  }


  /*  
      Backup for function save 
      Änderungen :  fixAttributesOnStore($this) wurde erst nach dem ldap->modify
                    Ausgeführt, deshalb wurden die gemappten Attribute auch nicht 
                    gespeichert.

      Von        : Fabian Hickert
      Datum      : 15.12.2005

      Alter Quellcode :

  /* Save data to LDAP, depending on is_account we save or delete * /
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call parents save to prepare $this->attrs * /
    plugin::save();

    /* Save arrays * /
    $this->attrs['gosaMailAlternateAddress']= $this->gosaMailAlternateAddress;
    $this->attrs['gosaMailForwardingAddress']= $this->gosaMailForwardingAddress;

    /* Save shared folder target * /
    $this->attrs['gosaSharedFolderTarget']= "share+".$this->uid;

    /* Save acl's * /
    $this->attrs['acl']= array();
    foreach ($this->imapacl as $user => $acl){
      if ($user == ""){
        continue;
      }
      $this->attrs['acl'][]= "$user $acl";
    }

    /* Save data to LDAP * /
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Only do IMAP actions if we are not a template * /
    if (!$this->is_template){
      $method= new $this->method($this->config);
      $method->fixAttributesOnStore($this);
      if ($method->connect($this->gosaMailServer)){
        $method->updateMailbox($this->uid);
        $method->setQuota($this->uid, $this->gosaMailQuota);

        /* Exchange '%member%' pseudo entry * /
        $memberacl= $this->imapacl['%members%'];
        unset ($this->imapacl['%members%']);
        foreach ($this->members as $user){
          if (!isset($this->imapacl[$user])){
            $this->imapacl[$user]= $memberacl;
          }
        }

        $method->setSharedFolderPermissions($this->uid, $this->imapacl);
        $method->disconnect();
      }
    }

    /* Optionally execute a command after we're done * /
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }
      
  ENDE Alter Quellcode
  */








  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save arrays */
    $this->attrs['gosaMailAlternateAddress']= $this->gosaMailAlternateAddress;
    $this->attrs['gosaMailForwardingAddress']= $this->gosaMailForwardingAddress;

    /* Save shared folder target */
    $this->attrs['gosaSharedFolderTarget']= "share+".$this->uid;

    if(preg_match("/kolab/i",$this->config->current['MAILMETHOD'])){
      /* Save acl's */
      $this->attrs['acl']= array();
      foreach ($this->imapacl as $user => $acl){
        if ($user == ""){
          continue;
        }
        $ldap->search("(&(objectClass=person)(uid=".$user."))",array("mail"));
        $mail = $ldap->fetch();
        if($mail){
          if(isset($mail['mail'][0])){
            $this->attrs['acl'][]= $mail['mail'][0]." $acl";
          }
        }else{
          $this->attrs['acl'][]= "$user $acl";
        }
      }
    }else{
      /* Save acl's */
      $this->attrs['acl']= array();
      foreach ($this->imapacl as $user => $acl){
        if ($user == ""){
          continue;
        }
        $this->attrs['acl'][]= "$user $acl";
      }
    }

    /* Only do IMAP actions if we are not a template */
    if(preg_match("/kolab/i",$this->config->current['MAILMETHOD'])){
      if (empty($this->gosaMailServer)||is_array($this->gosaMailServer)){
        if(isset($this->attrs['gosaMailServer'][0])){
          $this->gosaMailServer = $this->attrs['gosaMailServer'][0];
        }
      }
    }  
 
    if ((!$this->is_template)&&(!empty($this->gosaMailServer))){
      $method= new $this->method($this->config);
      $method->fixAttributesOnStore($this);
      if ($method->connect($this->gosaMailServer)){
        $method->updateMailbox($this->uid);
        $method->setQuota($this->uid, $this->gosaMailQuota);

        /* Exchange '%member%' pseudo entry */
        $memberacl= $this->imapacl['%members%'];
        unset ($this->imapacl['%members%']);
        foreach ($this->members as $user){
          if (!isset($this->imapacl[$user])){
            $this->imapacl[$user]= $memberacl;
          }
        }

        $method->setSharedFolderPermissions($this->uid, $this->imapacl);
        $method->disconnect();
      }
    }

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

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
    $ldap= $this->config->get_ldap_link();

    $message= array();

    /* must: mail */
    if ($this->mail == ""){
      $message[]= _("The required field 'Primary address' is not set.");
    }
    if (!is_email($this->mail)){
      $message[]= _("Please enter a valid email addres in 'Primary address' field.");
    }
    $ldap->cd($this->config->current['BASE']);
    $ldap->search ("(&(!(objectClass=gosaUserTemplate))(objectClass=gosaMailAccount)(|(mail=".$this->mail.")(gosaMailAlternateAddress=".
        $this->mail."))(!(uid=".$this->cn."))(!(cn=".$this->cn.")))");
    if ($ldap->count() != 0){
      $message[]= _("The primary address you've entered is already in use.");
    }

    /* Check quota */
    if ($this->gosaMailQuota != '' && chkacl ($this->acl, "gosaMailQuota") == ""){
      if (!is_numeric($this->gosaMailQuota)) {
        $message[]= _("Value in 'Quota size' is not valid.");
      } else {
        $this->gosaMailQuota= (int) $this->gosaMailQuota;
      }
    }

    /* Check rejectsize for integer */
    if ($this->gosaMailMaxSize != '' && chkacl ($this->acl, "gosaMailQuota") == ""){
      if (!is_numeric($this->gosaMailMaxSize)){
        $message[]= _("Please specify a vaild mail size for mails to be rejected.");
      } else {
        $this->gosaMailMaxSize= (int) $this->gosaMailMaxSize;
      }
    }

    /* Need gosaMailMaxSize if use_mailsize_limit is checked */
    if (is_integer(strpos($this->gosaMailDeliveryMode, "reject")) && $this->gosaMailMaxSize == ""){
      $message[]= _("You need to set the maximum mail size in order to reject anything.");
    }

    if(ord($this->imapacl['anyone'][0])==194){
      $message[] = _("Please choose valid permission settings. Default permission can't be emtpy.");
    }

    if(empty($this->gosaMailServer)){
      $message[] = _("Please select a valid mail server.");
    }

    return ($message);
  }

  /* Adapt from template, using 'dn' */
  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);

    foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
      $this->$val= array();
      if (isset($this->attrs["$val"]["count"])){
        for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
          $value= $this->attrs["$val"][$i];
          foreach (array("sn", "givenName", "uid") as $repl){
            if (preg_match("/%$repl/i", $value)){
              $value= preg_replace ("/%$repl/i", $this->parent->$repl, $value);
            }
          }
          array_push($this->$val, $value);
        }
      }
    }
  }

  /* Add entry to forwarder list */
  function addForwarder($address)
  {
    $this->gosaMailForwardingAddress[]= $address;
    $this->gosaMailForwardingAddress= array_unique ($this->gosaMailForwardingAddress);

    sort ($this->gosaMailForwardingAddress);
    reset ($this->gosaMailForwardingAddress);
    $this->is_modified= TRUE;
  }

  /* Remove list of addresses from forwarder list */
  function delForwarder($addresses)
  {
    $this->gosaMailForwardingAddress= array_remove_entries ($addresses,
                                      $this->gosaMailForwardingAddress);
    $this->is_modified= TRUE;
  }



  function addAlternate($address)
  {
    $ldap= $this->config->get_ldap_link();

    $address= strtolower($address);

    /* Is this address already assigned in LDAP? */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaMailAccount)(|(mail=$address)".
        "(gosaMailAlternateAddress=$address)))");

    if ($ldap->count() > 0){
      $attrs= $ldap->fetch ();
      return ($attrs["uid"][0]);
    }

    /* Add to list of alternates */
    if (!in_array($address, $this->gosaMailAlternateAddress)){
      $this->gosaMailAlternateAddress[]= $address;
    }

    sort ($this->gosaMailAlternateAddress);
    reset ($this->gosaMailAlternateAddress);
    $this->is_modified= TRUE;

    return ("");
  }


  function delAlternate($addresses)
  {
    $this->gosaMailAlternateAddress= array_remove_entries ($addresses,
                                     $this->gosaMailAlternateAddress);
    $this->is_modified= TRUE;
  }


  function make_name($attrs)
  {
    $name= "";
    if (isset($attrs['sn'][0])){
      $name= $attrs['sn'][0];
    }
    if (isset($attrs['givenName'][0])){
      if ($name != ""){
        $name.= ", ".$attrs['givenName'][0];
      } else {
        $name.= $attrs['givenName'][0];
      }
    }
    if ($name != ""){
      $name.= " ";
    }

    return ($name);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
