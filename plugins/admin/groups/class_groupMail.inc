<?php

class mailgroup extends plugin
{
  /* CLI vars */
  var $cli_summary        = "Manage mail groups/shared folders";
  var $cli_description    = "Some longer text\nfor help";
  var $cli_parameters     = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

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
      '%members%' => 'lrswp',  // %members% are all group-members
      ''          => 'p');    // Every user added gets this right

  var $kolabFolderType_SubType = "";
  var $kolabFolderType_Type = "";

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

  var $view_logged = FALSE;

  /* attribute list for save action */
  var $attributes= array( "mail",   "gosaMailServer", "gosaMailQuota", "gosaMailMaxSize",
      "gosaMailAlternateAddress", "gosaMailForwardingAddress",
      "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox",
      "acl","gosaSharedFolderTarget", "gosaVacationMessage");

  var $objectclasses= array("gosaMailAccount");
  var $CopyPasteVars          = array("quotaUsage","imapacl");

  function mailgroup (&$config, $dn= NULL, $ui= NULL)
  {
    /* Initialise all available attributes ... if possible
     */
    plugin::plugin($config, $dn);
    $this->orig_cn = $this->cn;

    /* Set mailMethod to the one defined in gosa.conf 
     */
    if (isset($this->config->current['MAILMETHOD'])){
      $this->mmethod= $this->config->current['MAILMETHOD'];
    }

    /* Check if selected mail method exists 
     */
    $cls = get_correct_class_name("mailMethod$this->mmethod");
    if ($cls && class_exists($cls)){
      $this->method= $cls;
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

    /* If this isn't a new mailgroup, read all required data from ldap
     */
    if (($dn != "new")&&($dn !== NULL)){

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

          /* If we do NOT use kolab or equal methods, 
             read imap the acls from the mail method class.
             They will be merged later with the ldap specified acls.
           */
          if(!preg_match("/olab/i",$this->mmethod)){
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
          }

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
    $ldap = $this->config->get_ldap_link();

    /* Read acls  from ldap and merge them with
     *  acls read from mail method.
     */
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

    /**
     * Detect group members which use the same acl
     *  as used for %members% and remove them.
     **/

    /* In this section we detect which acl is the most used.
       This will be used as %members% acl.
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
    if(!isset($this->imapacl['%members%'])){
      $this->imapacl['%members%'] = "lrspw";
    }


    /* Open ldap connection 
     */
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);

    /* Remove those users, that use %members% acl && are member of this group. */
    foreach($this->imapacl as $mail => $permission){
      $ldap->search("(&(objectClass=person)(|(mail=".$mail.")(uid=".$mail.")))",array("uid"));
      $atr = $ldap->fetch();
      if((isset($this->attrs['memberUid'])) && (is_array($this->attrs['memberUid']))){
        if((isset($atr['uid'][0]))&&(in_array($atr['uid'][0],$this->attrs['memberUid']))&&($permission == $this->imapacl['%members%'])){
          unset($this->imapacl[$mail]);
        }
      }
    }

    /**
     * ENDE: Detect group member with same acl and replace them with %members%
     **/
    $this->indexed_user = array("%members%","anyone");

    /* Append an empty entry, for special acl handling */
    if(count($this->imapacl)==2){
      $this->imapacl[''] ="lrsw";
      $this->indexed_user[] = '';
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

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","groups/".get_class($this),$this->dn);
    }

    /* Load templating engine */
    $smarty= get_smarty();

    /* Assign acls */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translation) {
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    if (session::get('js')==FALSE){
      $smarty->assign("javascript", "false");
    } else {
      $smarty->assign("javascript", "true");
    }

    /* Do we need to flip is_account state? */
    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    $display = "";

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent === NULL){

      $display.= "<img alt=\"\" src=\"images/stop.png\" align=middle>&nbsp;<b>".
        _("This 'dn' has no valid mail extensions.")."</b>";
      return ($display);
    }

    /* Show tab dialog headers */
    $display= "";
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display.= $this->show_disable_header(_("Remove mail account"),
            _("This account has mail features enabled. You can disable them by clicking below."));
      } else {
        $display.= $this->show_enable_header(_("Create mail account"),
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
    if($this->acl_is_writeable("acl")){
      foreach ($this->indexed_user as $nr => $user){
        if (isset($_POST["add_$nr"])){
          $this->imapacl[""]= "l";
        }
        if (isset($_POST["del_$nr"])){
          unset ($this->imapacl[$user]);
        }
      }
    }

    /* Trigger forward add dialog? */
    if($this->acl_is_writeable("gosaMailForwardingAddress")){
      if (isset($_POST['add_local_forwarder'])){
        $this->forward_dialog= TRUE;
        $this->dialog= TRUE;
      }
    }

    /* Cancel forward add dialog? */
    if($this->acl_is_writeable("gosaMailForwardingAddress")){
      if (isset($_POST['add_locals_cancel'])){
        $this->forward_dialog= FALSE;
        $this->dialog= FALSE;
      }
    }

    /* Finished adding of locals? */
    if ((isset($_POST['add_locals_finish'])) && ($this->acl_is_writeable("gosaMailForwardingAddress"))) {
      if (count ($_POST['local_list']) && $this->acl_is_writeable("gosaMailForwardingAddress")){

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
    if ((isset($_POST['add_forwarder'])) && ($this->acl_is_writeable("gosaMailForwardingAddress"))){
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
          if ($this->acl_is_writeable("gosaMailForwardingAddress")){
            $this->addForwarder ($address);
          }

        }
      }
    }

    /* Delete forward email addresses */
    if (isset($_POST['delete_forwarder']) && ($this->acl_is_writeable("gosaMailForwardingAddress"))){
      if (count($_POST['forwarder_list'])&& $this->acl_is_writeable("gosaMailForwardingAddress")){

        $this->delForwarder ($_POST['forwarder_list']);
      }
    }

    /* Add alternate email addresses */
    if (isset($_POST['add_alternate'])){
      if ($_POST['alternate_address'] != "" && $this->acl_is_writeable("gosaMailAlternateAddress")){

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
    if($this->acl_is_writeable("gosaMailAlternateAddress")){
      if (isset($_POST['delete_alternate']) && isset ($_POST['alternates_list'])){
        if (count($_POST['alternates_list']) && $this->acl_is_writeable("gosaMailAlternateAddress")){
          $this->delAlternate ($_POST['alternates_list']);
        }
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

      $res= get_list("(&(objectClass=gosaMailAccount)$filter)", "users", $gmailfilter['depselect'],
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
      error_reporting (E_ALL | E_STRICT);
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
    if (isset($this->imapacl['anyone'])){
      $smarty->assign("default_permissions", $this->imapacl['anyone']);
    }
    $smarty->assign("member_permissions", "lrsp");
    if (isset($this->imapacl['%members%'])){
      $smarty->assign("member_permissions", $this->imapacl['%members%']);
    }

    /* Assemble extra attributes */
    $perm= $this->getacl( "permissions");
    $tmp= "";
    $nr= 0;
    $count= count($this->imapacl);
    $this->indexed_user= array();
    $this->indexed_acl= array();
    foreach($this->imapacl as $user => $acl){

      /* Add additional acl settings */
      if ($user != "anyone" && $user != "%members%"){

        $Dis  = "";
        if(!preg_match("/w/",$perm)){
          $Dis = " disabled ";
        }
  
        /* Reset given Acls to ensure that nobody can read username and acls if not allwoed */
        if(!preg_match("/r/",$perm)){
          $user = "";
          $nr   = "none";
          $key  = "none";  
        }

        $tmp.= "<tr>  
                 <td>
                  <input name=\"user_$nr\" size=20 maxlength=60 value=\"$user\" ".$Dis.">
                 </td>
                 <td>
                 <select size=\"1\" name=\"perm_$nr\" ".$Dis.">";

        /* Add acl options for this additional acl setting */
        if(preg_match("/r/",$perm)){
          foreach ($this->perms as $key => $value){
            if ($acl == $key){
              $tmp.= "<option value=\"$key\" selected>$value</option>";
            } else {
              $tmp.= "<option value=\"$key\">$value</option>";
            }
          }
        }
        $tmp.= "</select>&nbsp;";


        
        if ($nr == $count - 1){
          if($this->acl_is_writeable("acl")){
            $tmp.= "<input type=submit value=\""._("Add")."\" ".
              "name=\"add_$nr\" >";
          }
        }
        if ($count > 3){
          if($this->acl_is_writeable("acl")){
            $tmp.= "<input type=submit value=\""._("Remove")."\" ".
              "name=\"del_$nr\" ></td></tr>";        
          }
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
    }
    if (is_numeric($this->gosaMailQuota) && $this->gosaMailQuota != 0){
      if($this->acl_is_readable("gosaMailQuota")){
        $smarty->assign("quotausage", progressbar(round(($this->quotaUsage * 100)/ $this->gosaMailQuota),100,15,true));
        $smarty->assign("quotadefined", "true");
      }else{
        $smarty->assign("quotadefined", "true");
        $smarty->assign("quotausage", "-");
      }
    } else {
      $smarty->assign("quotadefined", "false");
    }

    if(isset($this->config->current['MAILMETHOD'])&&preg_match("/olab/i",$this->config->current['MAILMETHOD'])){

      $smarty->assign("kolab", TRUE);
      $smarty->assign("JS",session::get('js'));
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
    if(!$this->initially_was_account){
      return;
    }
  
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
    show_ldap_error($ldap->get_error(), sprintf(_("Removing of groups/mail with dn '%s' failed."),$this->dn));


    new log("remove","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

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

      plugin::save_object();

      $tmp= preg_replace("/[^a-z]/i","",$this->gosaMailDeliveryMode);

      /* Handle delivery flags */
      if($this->acl_is_writeable("gosaMailDeliveryModeL")){
        if(!preg_match("/L/",$tmp) && !isset($_POST['drop_own_mails'])){
          $tmp.="L";
        }elseif(preg_match("/L/",$tmp) && isset($_POST['drop_own_mails'])){
          $tmp = preg_replace("/L/","",$tmp);
        }
      }

      $opts = array(
          "R"   => "use_mailsize_limit",
          "S"   => "use_spam_filter",
          "V"   => "use_vacation",
          "C"   => "own_script",
          "I"   => "only_local");

      foreach($opts as $flag => $post){
        if($this->acl_is_writeable("gosaMailDeliveryMode".$flag)){
          if(!preg_match("/".$flag."/",$tmp) && isset($_POST[$post])){
            $tmp.= $flag;
          }elseif(preg_match("/".$flag."/",$tmp) && !isset($_POST[$post])){
            $tmp = preg_replace("/".$flag."/","",$tmp);
          }
        }
      }

      $tmp= "[$tmp]";
      if ($this->gosaMailDeliveryMode != $tmp){
        $this->is_modified= TRUE;
      }
      $this->gosaMailDeliveryMode= $tmp;

      /* Collect data and re-assign it to the imapacl array */
      if ($this->acl_is_writeable("acl")){
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
    $this->attrs['acl']= array();

    /* Prepare Mail server attribute */
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
      if(!empty($this->kolabFolderType_Type)){
        $this->attrs['kolabFolderType'] = $this->kolabFolderType_Type.".".$this->kolabFolderType_SubType;
      }else{
        $this->attrs['kolabFolderType'] = array();
      }
    }

    /* Get naming attribute for mail accounts */
    $tmp = new $this->method($this->config);
    $uattrib = $tmp->uattrib;

    /* Create ACL array
       What is done here.

       1.  Do not write acl entries for empty entries.
       2.  Check if given user is a valid GOsa user.
       - If he is one, check if he has a valid mail extension
       -If this is true, add the user to the ACL entry.
       -If this is NOT true, skip ACL entries for this user.
       - He is not a GOsa Account, so write the ACL. (Manually entered ACL)
       3. In case of "olab" mail method, remove the entry from the $this->imapacl array
       because the kolab deamon will set the acls for us.

     */
    foreach ($this->imapacl as $user => $acl){

      /* Skip empty entries */
      if (empty($user) || $user == ""){
        unset($this->imapacl[$user]);
      }

      /* Skip placeholder */
      if (empty($user) || $user == "" || preg_match("/%members%/",$user)){
        continue;
      }

      /* Check if your is a real GOsa user
       * If user is a real GOsa user but do not have an email address - SKIP adding acls
       * If user is a real GOsa user with an email address - add acls
       */
      $ldap->search("(&(objectClass=person)(|(uid=".$user.")(mail=".$user.")))",array("mail","uid"));
      if($ldap->count()){

        /* Has the user a valid mail account? */
        $attrs = $ldap->fetch();
        if(isset($attrs['mail'][0])){

          $name = $attrs[$uattrib][0];
          $this->attrs['acl'][]= $name." ".$acl;

          /* Do not write imap acl directly i nkolab mode, let the kolab deamon do this. */
          unset($this->imapacl[$user]);
          if(!preg_match("/olab/i",$this->mmethod)){
            $this->imapacl[$name] = $acl;
          }

        }else{

          /* User is a valid GOsa account, but he has no mail extension. Skip ACLs */
          unset($this->imapacl[$user]);
        }
      }else{

        /* Seems to be a manually a added acl
         * Write this acl.
         */
        $this->attrs['acl'][]= $user." ".$acl;

        /* In case of kolab methods, let the deamon add the imap acls */
        if(preg_match("/olab/i",$this->mmethod)){
          unset($this->imapacl[$user]);
        }
      }
    }

    /* Handle mail method actions, set acls, quota ...*/
    if ((!$this->is_template)&&(!empty($this->gosaMailServer))){
      $method= new $this->method($this->config);
      $method->fixAttributesOnStore($this);
      if (($method->connect($this->gosaMailServer))){
        $method->updateMailbox($this->uid);
        $method->setQuota($this->uid, $this->gosaMailQuota);

        /* Only write imap acls directly if we are not
         *  using a kolab like mail method.
         */
        if(!preg_match("/olab/i",$this->mmethod)){
          $method->setSharedFolderPermissions($this->uid, $this->imapacl);
        }
        $method->disconnect();
      }
    }

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 
    show_ldap_error($ldap->get_error(), sprintf(_("Saving of groups/mail with dn '%s' failed."),$this->dn));
    
    if($this->initially_was_account){
      new log("modify","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","groups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());  
    }
    

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
    if ($this->gosaMailQuota != '' && $this->acl_is_writeable("gosaMailQuota")){
      if (!is_numeric($this->gosaMailQuota)) {
        $message[]= _("Value in 'Quota size' is not valid.");
      } else {
        $this->gosaMailQuota= (int) $this->gosaMailQuota;
      }
    }

    /* Check rejectsize for integer */
    if ($this->gosaMailMaxSize != '' && $this->acl_is_writeable("gosaMailQuota")){
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


  /* Return plugin informations for acl handling  */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Mail"),
          "plDescription" => _("Group mail"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 0,
          "plSection"     => array("administration"),
          "plCategory"    => array("groups"), 
          "plProvidedAcls"=> array(
            "mail"                      => _("Mail address"),
            "gosaMailAlternateAddress"  => _("Alternate addresses"),
            "gosaMailForwardingAddress" => _("Forwarding addresses"),
            "gosaMailQuota"             => _("Quota size"),
            "gosaMailServer"            => _("Mail server"),
            "acl"                       => _("Permissions"))
          ));
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
