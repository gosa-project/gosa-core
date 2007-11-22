<?php

class mailgroup extends plugin
{
  var $uid                        = "";       // User id 
  var $cn                         = "";       // cn
  var $orig_cn                    = "";       // cn

  var $method                     = "mailMethod"; // Used Mail method 
  var $mmethod                    = "";           // Contains the gosa.conf MAILMETHOD
  var $mail                       = "";           // Default mail address 

  var $gosaMailAlternateAddress   = array();  // Set default Alternate Mail Adresses to empty array
  var $gosaMailForwardingAddress  = array();  // Forwarding also empty

  var $gosaMailServer             = "";       // Selected mailserver 
  var $gosaMailQuota              = "";       // Defined Quota 
  var $quotaUsage                 = 0;        // Currently used quota

  var $gosaVacationMessage        = "";       // Vocation message 

  var $imapacl                    = array('anyone'    => 'p',     // Set acls for everyone
      '%members%' => 'lrspw',  // %members% are all group-members
      ''          => 'p');    // Every user added gets this right


  var $gosaSpamSortLevel          = "";     
  var $gosaSpamMailbox            = "";
  var $gosaSharedFolderTarget     ;

  var $forward_dialog             = FALSE;    

  var $members                    = array();  // Group members

  var $mailusers                  = array();
  var $perms                      = array();
  var $gosaMailDeliveryMode       = "[L        ]";   // 
  var $gosaMailMaxSize            = "";       // 
  
  var $remove_folder_from_imap    = true;

  /* Helper */
  var $indexed_acl= array();
  var $indexed_user= array();

  /* Copy & paste */
  var $CopyPasteVars          = array("quotaUsage","imapacl");

  /* attribute list for save action */
  var $attributes= array( "mail",   "gosaMailServer", "gosaMailQuota", "gosaMailMaxSize",
      "gosaMailAlternateAddress", "gosaMailForwardingAddress",
      "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox",
      "acl","gosaSharedFolderTarget", "gosaVacationMessage");

  var $objectclasses= array("gosaMailAccount");

  var $kolabFolderType_SubType = "";
  var $kolabFolderType_Type = "";

  function mailgroup ($config, $dn= NULL, $parent= NULL)
  {
    /* Initialise all available attributes ... if possible
     */
    plugin::plugin($config, $dn, $parent);

    /* Save initial cn */
    $this->orig_cn = $this->cn;

    /* Set mailMethod to the one defined in gosa.conf 
     */
    if (isset($this->config->current['MAILMETHOD'])){
      $this->mmethod= $this->config->current['MAILMETHOD'];
    }

    /* Check if selected mail method exists 
     */
    if (class_exists("mailMethod$this->mmethod")){
      $this->method= "mailMethod$this->mmethod";
    } else {
      print_red(sprintf(_("There is no mail method '%s' specified in your gosa.conf available."), $this->mmethod));
    }

    /* Load Mailserver  
     */
    if(isset($this->attrs['gosaMailServer'][0])){
      $this->gosaMailServer =  $this->attrs['gosaMailServer'][0];
    }

    /* Convert cn to uid in case of existing entry
     */
    if (isset($this->attrs['cn'][0])){
      $this->uid= $this->attrs['cn'][0];
    }

    /* Get folder type */
    if(isset($this->config->current['MAILMETHOD'])&&preg_match("/olab/i",$this->config->current['MAILMETHOD'])){
      if(isset($this->attrs['kolabFolderType'])){
        $tmp = split("\.",$this->attrs['kolabFolderType'][0]);
        $this->kolabFolderType_Type = $tmp[0];
        $this->kolabFolderType_SubType = $tmp[1];
      }
    }

    /* If this ins't new mailgroup, read all required data from ldap
     */
    if (($dn != "new")&&($dn != NULL)){

      /* Load attributes which represent multiple entries  
       */
      foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
        $this->$val = array();
        if (isset($this->attrs["$val"]["count"])){
          for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }

      /* Only do IMAP actions if gosaMailServer attribute is set 
       */
      if (isset ($this->attrs["gosaMailServer"][0])){

        /* Create new instance of our defined mailclass
         */
        $method= new $this->method($this->config);

        if ($method->connect($this->attrs["gosaMailServer"][0])){
        

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
            if(!empty($leader)){
              $this->imapacl['%members%']= $leader;
            }
            foreach ($this->imapacl as $user => $acl){
              if ($this->acl == $leader && in_array($user, $this->attrs['memberUid'])){
                unset($this->imapacl[$user]);
              }
            }

          } // ENDE ! isset ($this->attrs['acl'])
          
          /* Adapt attributes if needed */
          $method->fixAttributesOnLoad($this);
          
          /*  get Quota */
          $quota= $method->getQuota($this->uid);

          /* Update quota values */
          if(is_array($quota)){
            if ($quota['gosaMailQuota'] == 2147483647){
              $this->quotaUsage= "";
              $this->gosaMailQuota= "";
            } else {
              $this->quotaUsage= $quota['quotaUsage'];
              $this->gosaMailQuota= $quota['gosaMailQuota'];
            }
          }else{
            $this->quotaUsage     = "";
            $this->gosaMailQuota  = "";
//            print_red(sprintf(_("Can't get quota information for '%s'."),$this->uid));
          }
          $method->disconnect();
        }   // ENDE $method->connect($this->attrs["gosaMailServer"][0])){

      }   // ENDE gosaMailServer

    }   // ENDE dn != "new"


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
    if(preg_match("/olab/i",$this->mmethod)){
      $ldap = $this->config->get_ldap_link();

      if (isset($this->attrs['acl'])){

        for ($i= 0; $i<$this->attrs['acl']['count']; $i++){
          list($user, $permission)= split(' ', $this->attrs['acl'][$i]);

          /* Add to list */
          $this->imapacl[$user]= $permission;

          /* Get all user permissions sorted by acl, to detect the most used acl
             This acl is then used for %members%
           */
          if ($user != "anyone" && $user != "%members%"){
            $tmp[$permission][] = $user;
          }

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
      /* Most used at last 
       */
      ksort($tmp2);      
  
      /* Assign last (most used acl) to %members% acl 
       */
      $str = array_pop($tmp2);
      if(!empty($str)) {
        $this->imapacl['%members%']=$str;
      }

      /* Open ldap connection 
       */
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);

      /* Remove those users, that use %members% acl && are member of this group. */
      foreach($this->imapacl as $mail => $permission){
        $ldap->search("(&(objectClass=person)(mail=".$mail."))",array("uid"));
        $atr = $ldap->fetch();
        if((isset($this->attrs['memberUid'])) && (is_array($this->attrs['memberUid']))){
          if((isset($atr['uid'][0]))&&(in_array($atr['uid'][0],$this->attrs['memberUid']))&&($permission == $this->imapacl['%members%'])){
            unset($this->imapacl[$mail]);
          }
        }
      }
      /* Append an empty entry, for special acl handling */
      if(count($this->imapacl)==2){
        $this->imapacl[''] ="";
      }
  
    }else{ // Not kolab 
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

    /* Load Mailserver  
     */
    if(isset($this->attrs['gosaMailServer'][0])){
      $this->gosaMailServer =  $this->attrs['gosaMailServer'][0];
    }
    /* Fill translations */
    $this->perms["lrsw"]= _("read");
    $this->perms["lrswp"]= _("post");
    $this->perms["p"]= _("external post");
    $this->perms["lrswip"]= _("append");
    $this->perms["lrswipcd"]= _("write");
    $this->perms["lrswipcda"]= _("admin");
    $this->perms[""]= _("none");
  }

  function execute()
  {
    /* Call parent execute */
    //plugin::execute();

    /* Load templating engine */
    $smarty= get_smarty();
    $display = "";
    if ($_SESSION['js']==FALSE){
      $smarty->assign("javascript", "false");
    } else {
      $smarty->assign("javascript", "true");
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){

      /* Onyl change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
      }
    }
    
    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){

      $display.= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
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
        $display.= $this->show_header(_("Create mail account"),
            _("This account has mail features disabled. You can enable them by clicking below."));

        /* Show checkbox that allows us to remove imap entry too*/
        if($this->initially_was_account){
        
          $c = "";
          if($this->remove_folder_from_imap){
            $c= " checked ";
          }
  
          $display .= "<h2>Shared folder delete options</h2>
                       <input class='center' type='checkbox' name='remove_folder_from_imap' value='1' ".$c."  
                          title='"._("Remove shared folder from mail server database when entry gets removed in LDAP")."'>";
          $display .= _("Remove the shared folder and all its contents after saving this account"); 
        }

        return ($display);
      }
    }

    /* Add ACL? */
    if (isset($_POST["add_acl"])){
      $this->imapacl[""]= "l";
    }
    foreach ($this->indexed_user as $nr => $user){
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
      $res= get_list("(&(objectClass=gosaMailAccount)$filter)", $acl, $gmailfilter['depselect'],
                     array("sn", "mail", "givenName"), GL_SUBSEARCH | GL_SIZELIMIT);
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
    $smarty->assign("member_permissions", "lrsp");
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
            $tmp.= "<option value=\"$key\" selected>$value</option>";
          } else {
            $tmp.= "<option value=\"$key\">$value</option>";
          }
        }
        $tmp.= "</select>&nbsp;";
        if ($nr == $count - 1){
        }
        if ($count > 3){
          $tmp.= "<input type=submit value=\""._("Remove")."\" ".
            "name=\"del_$nr\" $perm></td></tr>";
        }
      }
      $this->indexed_user[$nr]= $user;
      $this->indexed_acl[$nr++]= $acl;
    }
    $tmp.= "<tr><td></td><td></td><td><input type=submit value='"._("Add")."' name='add_acl' $perm>";
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
      $smarty->assign("quotausage", progressbar(round(($this->quotaUsage * 100)/ $this->gosaMailQuota),100,15,true));
      $smarty->assign("quotadefined", "true");
    } else {
      $smarty->assign("quotadefined", "false");
    }

    if(isset($this->config->current['MAILMETHOD'])&&preg_match("/olab/i",$this->config->current['MAILMETHOD'])){

      $smarty->assign("kolab", TRUE);
      $smarty->assign("JS",$_SESSION['js']);
      $smarty->assign("kolabFolderType_Types",    array (   ''      => _('Unspecified'),  'mail' => _('Mails'),
                                                            'task'  => _('Tasks') ,       'journal' => _('Journals'),
                                                            'calendar' => _('Calendar'),       'contact' => _('Contacts'), 
                                                            'note'  => _('Notes')));
      if($this->kolabFolderType_Type == "mail"){
        $smarty->assign("kolabFolderType_SubTypes", array(    
              ''          => _('Unspecified'),  'inbox'     => _("Inbox")   , 
              'drafts'    => _("Drafts"),       'sentitems' => _("Sent items"),
              'junkemail' => _("Junk mail")));
      }else{
        $smarty->assign("kolabFolderType_SubTypes", array(  'default' => _("Default")));
      }
      $smarty->assign("kolabFolderType_Type",     $this->kolabFolderType_Type);
      $smarty->assign("kolabFolderType_SubType",  $this->kolabFolderType_SubType);
    }else{
      $smarty->assign("kolab", FALSE);
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
    if(isset($this->config->current['MAILMETHOD'])&&preg_match("/olab/i",$this->config->current['MAILMETHOD'])){
      $this->attributes[]="acl";
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

    /* Connect to IMAP server for account deletion */
    if ($this->initially_was_account){
 
      $method= new $this->method($this->config);
      $method->fixAttributesOnRemove($this);
      if ($method->connect($this->gosaMailServer) && $this->remove_folder_from_imap){

        /* Remove account from IMAP server */
        $method->deleteMailbox($this->uid);
        $method->disconnect();
      }
    }
    /* Keep uid */
    unset ($this->attrs['uid']);

    $ldap->cd($this->dn);
    $ldap->modify ($this->attrs); 
    show_ldap_error($ldap->get_error(), _("Removing group mail settings failed"));

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  /* Save data to object */
  function save_object()
  {

    /* Add special kolab attributes */    
    if(isset($this->config->current['MAILMETHOD'])&&preg_match("/olab/i",$this->config->current['MAILMETHOD'])){
      if(isset($_POST['kolabFolderType_Type'])){
        $this->kolabFolderType_Type = get_post("kolabFolderType_Type");
        $this->kolabFolderType_SubType = get_post("kolabFolderType_SubType");
      }
    }
  
    /* Check if user wants to remove the shared folder from imap too */
    if($this->initially_was_account && !$this->is_account){
      if(isset($_POST['remove_folder_from_imap'])){
        $this->remove_folder_from_imap = true;
      }else{
        $this->remove_folder_from_imap = false;
      }
    }

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



  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save arrays */
    $this->attrs['gosaMailAlternateAddress']  = $this->gosaMailAlternateAddress;
    $this->attrs['gosaMailForwardingAddress'] = $this->gosaMailForwardingAddress;
    $this->attrs['gosaSharedFolderTarget']    = "share+".$this->uid;

    /* Only do IMAP actions if we are not a template */
    if(preg_match("/olab/i",$this->mmethod)){
      if (empty($this->gosaMailServer)||is_array($this->gosaMailServer)){
        if(isset($this->attrs['gosaMailServer'][0])){
          $this->gosaMailServer = $this->attrs['gosaMailServer'][0];
        }
      }
    }  

    /* Exchange '%member%' pseudo entry */
    $memberacl= $this->imapacl['%members%'];
    foreach ($this->members as $user){
      if (!isset($this->imapacl[$user])){
        $this->imapacl[$user]= $memberacl;
      }
    }

    /* Prepare kolab attributes to be written  */
    if(preg_match("/olab/i",$this->mmethod)){
      $this->attrs['acl']= array();
      if(!empty($this->kolabFolderType_Type)){ 
        $this->attrs['kolabFolderType'] = $this->kolabFolderType_Type.".".$this->kolabFolderType_SubType;
      }else{
        $this->attrs['kolabFolderType'] = array();
      }
    }else{
      $this->attrs['acl']= array();;
      unset($this->attrs['acl']);
    }

    /* Get naming attribute for mail accounts */  
    $tmp = new $this->method($this->config);
    $uattrib = $tmp->uattrib;

    /* Create ACL array 
     *  In case of kolab also create attrs['acl'] to save acls in ldap.
     */
    foreach ($this->imapacl as $user => $acl){

      if (empty($user) || $user == ""){
        unset($this->imapacl[$user]);
      }
  
      /* Skip placeholder */
      if (empty($user) || $user == "" || preg_match("/%members%/",$user)){
        continue;
      }

      /* Check if your is a real GOsa user  
       * If user is a real GOsa user but do not have an email address - SKIP adding acls
       * If user is a real GOsa user with an emal address - add acls  
       */
      $ldap->search("(&(objectClass=person)(|(uid=".$user.")(mail=".$user.")))",array("mail","uid"));
      if($ldap->count()){
        $attrs = $ldap->fetch();

        /* Has the user a valid mail account? */
        if(isset($attrs['mail'][0])){

          $name = $attrs[$uattrib][0];

          /* In case of kolab methods add acl attribute too */
          if(preg_match("/olab/i",$this->mmethod)){
            $this->attrs['acl'][]= $name." ".$acl;
          }
          $this->imapacl[$name] = $acl;
        }else{

          /* User has no mail extension */
          unset($this->imapacl[$user]);
        }
      }else{
        /* Seems to be a manually added acl */

        /* In case of kolab methods add acl attribute too */
        if(preg_match("/olab/i",$this->mmethod)){
          $this->attrs['acl'][]= $user." ".$acl;
        }
        $this->imapacl[$user] = $acl;
      }
    }
  
    if ((!$this->is_template)&&(!empty($this->gosaMailServer))){
      $method= new $this->method($this->config);
      $method->fixAttributesOnStore($this);
      if (($method->connect($this->gosaMailServer))){
        $method->updateMailbox($this->uid);
        $method->setQuota($this->uid, $this->gosaMailQuota);
        $method->setSharedFolderPermissions($this->uid, $this->imapacl);
        $method->disconnect();
      }
    }

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 
    show_ldap_error($ldap->get_error(), _("Saving group mail settings failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }

  /* Check formular input */
  function check()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!$this->is_account) return array();
    
    //$message[] = $str;      

    /* must: mail */
    if ($this->mail == ""){
      $message[]= _("The required field 'Primary address' is not set.");
    }
    if (!is_email($this->mail)){
      $message[]= _("Please enter a valid email addres in 'Primary address' field.");
    }
    $ldap->cd($this->config->current['BASE']);
    $ldap->search ("(&(!(objectClass=gosaUserTemplate))(objectClass=gosaMailAccount)(|(mail=".$this->mail.")(gosaMailAlternateAddress=".
        $this->mail."))(!(uid=".$this->orig_cn."))(!(cn=".$this->orig_cn.")))");
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

  function getCopyDialog()
  {
    if(!$this->is_account) return("");

    $smarty = get_smarty();
    $smarty->assign("gosaMailAlternateAddress",$this->gosaMailAlternateAddress);
    $smarty->assign("gosaMailForwardingAddress",$this->gosaMailForwardingAddress);
    $smarty->assign("mail",$this->mail);
    $display= $smarty->fetch (get_template_path('paste_mail.tpl', TRUE));
    $ret = array();
    $ret['string'] = $display;
    $ret['status'] = "";
    return($ret);
  }

  function saveCopyDialog()
  {
    if(!$this->is_account) return;

    /* Perform ADD / REMOVE ... for mail alternate / mail forwarding addresses 
    */
    $this->execute();
    if(isset($_POST['mail'])){
      $this->mail = $_POST['mail'];
    }
  }

  function PrepareForCopyPaste($source)
  {
    plugin::PrepareForCopyPaste($source);

    /* Reset alternate mail addresses */
    $this->gosaMailAlternateAddress = array();
  }

  
  /* Remove given ACL for given member (uid,mail) ..
   */
  function removeUserAcl($index )
  {
    if(isset($this->imapacl[$index])){
      unset($this->imapacl[$index]);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
