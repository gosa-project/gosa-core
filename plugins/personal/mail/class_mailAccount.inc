<?php
/*! \brief   mail plugin
  \author  Cajus Pollmeier <pollmeier@gonicus.de>
  \version 2.00
  \date    24.07.2003

  This class provides the functionality to read and write all attributes
  relevant for gosaMailAccounts from/to the LDAP. It does syntax checking
  and displays the formulars required.
 */

/* Load sieve support */
require_once ("sieve/libsieve.inc");

/* Load mail methods */
global $BASE_DIR;
get_dir_list("$BASE_DIR/include");

class mailAccount extends plugin
{
  /* Definitions */
  var $plHeadline         = "Mail";
  var $plDescription      = "This does something";
  var $method             = "mailMethod";

  /* plugin specific values */
  var $mail                               = "";
  var $uid                                = "";
  var $gosaMailAlternateAddress           = array();
  var $gosaMailForwardingAddress          = array();
  var $gosaMailDeliveryMode               = "[L        ]";
  var $gosaMailServer                     = "";
  var $gosaMailQuota                      = "";
  var $gosaMailMaxSize                    = "";
  var $gosaVacationMessage                = "";
  var $gosaVacationStart                  = 0;
  var $gosaVacationStop                   = 0;
  var $gosaSpamSortLevel                  = "";
  var $gosaSpamMailbox                    = "";

  var $quotaUsage                         = 0;
  var $forward_dialog                     = FALSE;
  var $folder_prefix                      = "";
  var $mailboxList                        = array("INBOX");
  var $default_permissions                = "none";
  var $member_permissions                 = "post";
  var $members                            = array();
  var $admins                             = array();
  var $vacations                          = array();
  var $perms                              = array(  "lrs"       => "read", 
                                                    "lrsp"      => "post", 
                                                    "lrsip"     => "append",
                                                    "lrswipcd"  => "write", 
                                                    "lrswipcda" => "all" );

  /* attribute list for save action */
  var $attributes= array("mail", "gosaMailServer", "gosaMailQuota", "gosaMailMaxSize","gosaMailForwardingAddress",
      "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox","gosaMailAlternateAddress",
      "gosaVacationMessage", "gosaMailAlternateAddress", "gosaMailForwardingAddress", "gosaVacationStart", "gosaVacationStop");
  var $objectclasses= array("gosaMailAccount");

  var $sieve_management = NULL;

  /* constructor, if 'dn' is set, the node loads the given
     'dn' from LDAP */
  function mailAccount ($config, $dn= NULL, $parent= NULL)
  {
    /* Load bases attributes */
    plugin::plugin($config, $dn, $parent);

    if(isset($this->attrs['uid'])){
      $this->uid = $this->attrs['uid'][0];
    }
 
    if(is_array($this->gosaMailServer) && isset($this->gosaMailServer[0])){
      $this->gosaMailServer = $this->gosaMailServer[0];
    }

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;

    /*  Set mailMethod to the one defined in gosa.conf */
    if (isset($this->config->current['MAILMETHOD'])){
      $method= $this->config->current['MAILMETHOD'];
      if (class_exists("mailMethod$method")){
        $this->method= "mailMethod$method";
      } else {
        print_red(sprintf(_("There is no mail method '%s' specified in your gosa.conf available."), $method));
      }
    }

    /* Create the account prefix  user. user/ 
       Preset folder prefix. Will change it later to respect
       altnamespace. */
    if (isset($this->config->current['CYRUSUNIXSTYLE']) && $this->config->current['CYRUSUNIXSTYLE'] == "true"){
      $this->folder_prefix= "user/";
    }elseif (isset($this->config->data['MAIN']['CYRUSUNIXSTYLE']) && $this->config->data['MAIN']['CYRUSUNIXSTYLE'] == "true"){
      $this->folder_prefix= "user/";
    } else {
      $this->folder_prefix= "user.";
    }
   
    /* This is not a new account, parse additional attributes */
    if (($dn != NULL) && ($dn != "new") && $this->is_account){

      /* Load attributes containing arrays */
      foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
        $this->$val= array();
        if (isset($this->attrs["$val"]["count"])){
          for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }


      /* Only do IMAP actions if gosaMailServer attribute is set */
      if (isset ($this->attrs["gosaMailServer"][0])){

        $method = new $this->method($this->config);
        $id     = $method->uattrib;

        /* Adapt attributes if needed */
        $method->fixAttributesOnLoad($this);

        /* FixAttributesOnLoad possibly creates an array out of gosaMailServer.
            If the mail tab wasn't opened once before saving, the account can't be saved */
        if(is_array($this->gosaMailServer)){
          $this->gosaMailServer = $this->gosaMailServer[0];
        }

        if ($method->connect($this->attrs["gosaMailServer"][0])){

          /* Update quota values */
          $quota= $method->getQuota($this->folder_prefix.$this->$id);
         
          if($quota){
            if ($quota['gosaMailQuota'] == 2147483647){
              $this->quotaUsage     = "";
              $this->gosaMailQuota  = "";
            } else {
              $this->quotaUsage     = $quota['quotaUsage'];
              $this->gosaMailQuota  = $quota['gosaMailQuota'];
            }
          }else{
            $this->quotaUsage     = "";
            $this->gosaMailQuota  = "";
//            print_red(sprintf(_("Can't get quota information for '%s'."),$this->folder_prefix.$this->$id));
          }

          /* Get mailboxes / folder like INBOX ..*/
          $this->mailboxList= $method->getMailboxList($this->folder_prefix.$this->$id,$this->$id);
          
          $method->disconnect();
        }else{
          /* Could not connect to ldap.
           */
          if (isset($this->attrs['gosaMailQuota'][0])){
            $this->gosaMailQuota = $this->attrs['gosaMailQuota'][0];
          }
        }
      }
    }


    /* Get vacation message */

    /* Fill vacation array */
    $this->vacation= array();
    if (isset($this->config->current['VACATIONDIR'])){
      $dir= $this->config->current['VACATIONDIR'];
      if (is_dir($dir) && is_readable($dir)){

        /* Look for files and build the vacation array */
        $dh= opendir($dir);
        while ($file = readdir($dh)){
          $description= $this->parse_vacation("$dir/$file");
          if ($description != ""){
            $this->vacation["$dir/$file"]= $description;
          }
        }
        closedir($dh);
      }
    }

    /* Create sieve management class */
    $method = new $this->method($this->config);
    $id     = $method->uattrib;
    $this->sieve_management = new sieveManagement($this->config,$this->dn,$this,$id);

  /* Create filter */

    /* Get global filter config */
    if (!is_global("mailfilter")){
      $ui= get_userinfo();
      $base= get_base_from_people($ui->dn);
      $mailfilter= array( "depselect"       => $base,
          "muser"            => "",
          "regex"           => "*");
      register_global("mailfilter", $mailfilter);
    }
  }


  function parse_vacation($file)
  {
    $desc= "";

    if (is_file($file)){
      $fh = fopen($file, "r");
      $line= fgets($fh, 256);

      if (!preg_match('/^DESC:/', $line)){
        print_red (_("No DESC tag in vacation file:")." $file");
        return $desc;
      }
      fclose ($fh);

      $desc= trim(preg_replace('/^DESC:\s*/', '', $line));
    }

    return $desc;
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /* Initialise vars */

    /* Load templating engine */
    $smarty= get_smarty();
    $display= "";

    /* Get available mailserver */
    $mailserver= array();
    foreach ($this->config->data['SERVERS']['IMAP'] as $key => $val){
      $mailserver[]= $key;
    }


    /*
     * Sieve Management
     */
    if(isset($_POST['sieveManagement'])
        && preg_match("/C/",$this->gosaMailDeliveryMode)
        && chkacl($this->acl,"sieveManagement") == "") {
      $this->dialog = $this->sieve_management;
    }

    /* Cancel sieve edit */
    if(isset($_POST['sieve_cancel'])){
      $this->dialog = NULL;
    }

    /* Save sieve changes */
    if(isset($_POST['sieve_finish'])){
      $this->sieve_management = $this->dialog;
      $this->dialog = NULL;
    }

    if(is_object($this->dialog)){
      $this->dialog->save_object();
      return($this->dialog->execute());
    }


    /* Handle account state */

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){

      /* Onyl change account state if allowed */
      if($this->is_account && $this->acl == "#all#" && !$this->accountDelegationsConfigured()){
        $this->is_account= !$this->is_account;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
      }
    }

    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no mail extensions.")."</b>";

      $display.= back_to_main();
      return ($display);
    }

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        if($this->accountDelegationsConfigured()){
          $display= $this->show_header(_("Remove mail account"),
              _("This account can't be removed while there are delegations configured. Remove those delegations first."),TRUE,TRUE);
        }else{
          $display= $this->show_header(_("Remove mail account"),
              _("This account has mail features enabled. You can disable them by clicking below."));
        }
      } else {
        $display= $this->show_header(_("Create mail account"), _("This account has mail features disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Forwarder  subdialog */

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
            $this->is_modified= TRUE;
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
        $valid= FALSE;
        if (!is_email($address)){
          if (!is_email($address, TRUE)){
            if ($this->is_template){
              $valid= TRUE;
            } else {
              print_red (_("You're trying to add an invalid email address to the list of forwarders."));
            }
          }
        } elseif ($address == $this->mail
            || in_array($address, $this->gosaMailAlternateAddress)) {

          print_red (_("Adding your one of your own addresses to the forwarders makes no sense."));

        } else {
          $valid= TRUE;
        }

        if ($valid){
          /* Add it */
          if (chkacl ($this->acl, "gosaMailForwardingAddress") == ""){
            $this->addForwarder ($address);
            $this->is_modified= TRUE;
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

    
    /* Alternate address handling */

    /* Add alternate email addresses */
    if (isset($_POST['add_alternate'])){
      if ($_POST['alternate_address'] != "" &&
          chkacl ($this->acl, "gosaMailAlternateAddress") == ""){

        $valid= FALSE;
        if (!is_email($_POST['alternate_address'])){
          if ($this->is_template){
            if (!(is_email($_POST['alternate_address'], TRUE))){
              print_red (_("You're trying to add an invalid email address to the list of alternate addresses."));
            } else {
              $valid= TRUE;
            }
          } else {
            print_red (_("You're trying to add an invalid email address to the list of alternate addresses."));
          }

        } else {
          $valid= TRUE;
        }

        if ($valid && ($user= $this->addAlternate ($_POST['alternate_address'])) != ""){
          $ui= get_userinfo();
          if ($user != $ui->username && !$this->is_template){
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

  
    /* Vocation message */
  
    /* Import vacation message? */
    if (isset($_POST["import_vacation"]) && isset($this->vacation[$_POST["vacation_template"]])){
      $contents= "";
      $lines= file($_POST["vacation_template"]);
      foreach ($lines as $line){
        if (!preg_match('/^DESC:/', $line)){
          $contents.= $line;
        }
      }

      /* Replace attributes */
      $attrs = array();
      $obj = NULL;
      if(isset($this->parent->by_object['user'])){
        $attrs  = $this->parent->by_object['user']->attributes;
        $obj    = $this->parent->by_object['user'];
      }else{
        $obj    = new user($this->config,$this->dn);
        $attrs  = $obj->attributes;
      }

      if($obj){
        foreach ($attrs as $val){
          if(preg_match("/dateOfBirth/",$val)){
            if($obj->use_dob){
              $contents= preg_replace("/%$val/",date("Y-d-m",$obj->dateOfBirth),$contents);
            }
          }else {
            $contents= preg_replace("/%$val/",
                $obj->$val, $contents);
          }

          /* Replace vacation start and end time */
          if(preg_match("/%start/",$contents)){
            $contents = preg_replace("/%start/",date("d.m.Y",$this->gosaVacationStart),$contents);
          }
          if(preg_match("/%end/",$contents)){
            $contents = preg_replace("/%end/",date("d.m.Y",$this->gosaVacationStop),$contents);
          }
        }
      }

      /* Save message */
      $this->gosaVacationMessage= htmlspecialchars($contents);
    }

  
    /* Display forward dialog if requested above */

    /* Show forward add dialog */
    if ($this->forward_dialog){
      $ldap= $this->config->get_ldap_link();

      /* Save data */
      $mailfilter= get_global("mailfilter");
      foreach( array("depselect", "muser", "regex") as $type){
        if (isset($_POST[$type])){
          $mailfilter[$type]= $_POST[$type];
        }
      }
      if (isset($_GET['search'])){
        $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
        if ($s == "**"){
          $s= "*";
        }
        $mailfilter['regex']= $s;
      }
      register_global("mailfilter", $mailfilter);

      /* Get actual list */
      $mailusers= array ();
      if ($mailfilter['regex'] != '*' && $mailfilter['regex'] != ""){
        $regex= $mailfilter['regex'];
        $filter= "(|(mail=$regex)(gosaMailAlternateAddress=$regex))";
      } else {
        $filter= "";
      }
      if ($mailfilter['muser'] != ""){
        $user= $mailfilter['muser'];
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
      $res= get_list("(&(objectClass=gosaMailAccount)$filter)", $acl, $mailfilter['depselect'], 
                     array("sn", "mail", "givenName"), GL_SIZELIMIT | GL_SUBSEARCH);
      $ldap->cd($mailfilter['depselect']);
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
      if (isset($_POST['depselect'])){
        $smarty->assign("depselect", $_POST['depselect']);
      }
      $smarty->assign("deplist", $this->config->idepartments);
      $smarty->assign("apply", apply_filter());
      $smarty->assign("alphabet", generate_alphabet());
      $smarty->assign("hint", print_sizelimit_warning());
      foreach( array("depselect", "muser", "regex") as $type){
        $smarty->assign("$type", $mailfilter[$type]);
      }
      $smarty->assign("hint", print_sizelimit_warning());

      $display.= $smarty->fetch (get_template_path('mail_locals.tpl', TRUE, dirname(__FILE__)));
      return ($display);
    }

    /* Display mail account tab */
    if($this->gosaVacationStart ==0){
      $date= getdate(time());
      $this->gosaVacationStart = time();
    }else{
      $date= getdate($this->gosaVacationStart);
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
    $smarty->assign("start_day", $date["mday"]);
    $smarty->assign("days", $days);
    $smarty->assign("months", $months);
    $smarty->assign("start_month", $date["mon"]-1);
    $smarty->assign("years", $years);
    $smarty->assign("start_year", $date["year"]);

    if($this->gosaVacationStop ==0){
      $date= getdate(time());
      $date["mday"]++;
      $this->gosaVacationStop = time() + (60*60*24);
    }else{
      $date= getdate($this->gosaVacationStop);
    }
    $smarty->assign("end_day", $date["mday"]);
    $smarty->assign("end_month", $date["mon"]-1);
    $smarty->assign("end_year", $date["year"]);

    $smarty->assign("mailServers", $mailserver);
    foreach(array(
          "gosaMailServer", 
          "gosaMailQuota", 
          "perms", 
          "mail",
          "gosaMailAlternateAddress", 
          "gosaMailForwardingAddress",

          // gosaMailDeliveryMode Flags 
          "drop_own_mails",                       // No local delivery 
          "gosaMailMaxSize",                      // Enable - Drop mails > max size
          "gosaSpamSortLevel", "gosaSpamMailbox", // Enable - Spam sort options
          "gosaVacationMessage",                  // Enable - Vacation message      
          "gosaVacationStart",
          "gosaVacationStop",
          "custom_sieve",                         // Use custom sieve script
          "only_local"                            // Send/receive local mails 
                                        ) as $val){
      if(isset($this->$val)){
        $smarty->assign("$val", $this->$val);
      }
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
    }

    $smarty->assign("use_vacationACL",      chkacl($this->acl,"use_vacation"));
    $smarty->assign("gosaVacationTimerACL", chkacl($this->acl,"gosaVacationTimer"));
    $smarty->assign("sieveManagementACL",   chkacl($this->acl,"sieveManagement"));

    if (preg_match('/V/', $this->gosaMailDeliveryMode)){
      $smarty->assign('rangeEnabled', "");
    } else {
      $smarty->assign('rangeEnabled', "disabled");
    }

    if (is_numeric($this->gosaMailQuota) && $this->gosaMailQuota != 0){
      $smarty->assign("quotausage", progressbar(round(($this->quotaUsage * 100)/ $this->gosaMailQuota),100,15,true));
      $smarty->assign("quotadefined", "true");
    } else {
      $smarty->assign("quotadefined", "false");
    }

    /* Disable mail field if needed */
    $method= new $this->method($this->config);
    if ($method->uattrib == "mail" && $this->initially_was_account){
      $smarty->assign("mailACL", "disabled");
    }


    if (!preg_match("/L/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("drop_own_mails", "checked");
    } else {
      $smarty->assign("drop_own_mails", "");
    }

    $types = array(
          "V"=>"use_vacation",
          "S"=>"use_spam_filter",
          "R"=>"use_mailsize_limit",
          "I"=>"only_local",
          "C"=>"own_script");

    /* Fill checkboxes */
    foreach($types as $option => $varname){
      if (preg_match("/".$option."/", $this->gosaMailDeliveryMode)) {
        $smarty->assign($varname, "checked");
      } else {
        $smarty->assign($varname, "");
      }
    }
    
    if (preg_match("/V/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("use_vacation", "checked");
    } else {
      $smarty->assign("use_vacation", "");
    }
    if (preg_match("/S/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("use_spam_filter", "checked");
    } else {
      $smarty->assign("use_spam_filter", "");
    }
    if (preg_match("/R/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("use_mailsize_limit", "checked");
    } else {
      $smarty->assign("use_mailsize_limit", "");
    }
    if (preg_match("/I/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("only_local", "checked");
    } else {
      $smarty->assign("only_local", "");
    }
    if (preg_match("/C/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("own_script", "checked");
    } else {
      $smarty->assign("own_script", "");
    }

    /* Have vacation templates? */
    $smarty->assign("template", "");
    if (count($this->vacation)){
      $smarty->assign("show_templates", "true");
      $smarty->assign("vacationtemplates", $this->vacation);
      if (isset($_POST['vacation_template'])){
        $smarty->assign("template", $_POST['vacation_template']);
      }
    } else {
      $smarty->assign("show_templates", "false");
    }

    /* Fill spam selector */
    $spamlevel= array();
    for ($i= 0; $i<21; $i++){
      $spamlevel[]= $i;
    }
    $smarty->assign("spamlevel", $spamlevel);
    $smarty->assign("spambox", $this->mailboxList);
    $smarty->assign("custom_sieveACL", chkacl($this->acl, "custom_sieve"));	
    $smarty->assign("only_localACL", chkacl($this->acl, "only_local"));	

    $display.= $smarty->fetch (get_template_path('generic.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  /* check if we have some delegations configured, those delegations must be removed first */
  function accountDelegationsConfigured()
  { 
    /* We are in administrational edit mode.
        Check tab configurations directly */
    if(isset($this->attrs)){
      $checkArray  = array("kolabInvitationPolicy","unrestrictedMailSize", "calFBURL","kolabDelegate","kolabFreeBusyFuture");
      foreach($checkArray as $index){
        if(isset($this->attrs[$index])){
           return(true);
        }
      }
    }
    if(isset($this->parent->by_object['connectivity']->plugin['kolabAccount'])){
      if($this->parent->by_object['connectivity']->plugin['kolabAccount']->is_account){
        return(true);
      }
    }
    return(false); 
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

    /* Zero arrays */
    $this->attrs['gosaMailAlternateAddress']= array();
    $this->attrs['gosaMailForwardingAddress']= array();

    /* Adapt attributes if needed */
    $method= new $this->method($this->config);
    $method->fixAttributesOnRemove($this);

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing mail account failed"));

    /* Connect to IMAP server for account deletion */
    if ($this->gosaMailServer != ""){
      $method= new $this->method($this->config);
      $id= $method->uattrib;
      if ($method->connect($this->gosaMailServer)){

        /* Remove account from IMAP server */
        $method->deleteMailbox($this->folder_prefix.$this->$id);
        $method->disconnect();
      }
    }

    /* Update shared folder membership, ACL may need to be updated */
    $this->updateSharedFolder(); 

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove", array('uid'=> $this->uid));
  }


  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['mailTab'])){
      /* Save ldap attributes */
      plugin::save_object();

      /* Assemble mail delivery mode
         The mode field in ldap consists of values between braces, this must
         be called when 'mail' is set, because checkboxes may not be set when
         we're in some other dialog.

         Example for gosaMailDeliveryMode [LR        ]
         L: Local delivery
         R: Reject when exceeding mailsize limit
         S: Use spam filter
         V: Use vacation message
         C: Use custm sieve script
         I: Only insider delivery */

      $tmp= "";
      $Flags = array(
                     "R"  => array("ACL" => "gosaMailMaxSize",    "POST" => "use_mailsize_limit"),
                     "V"  => array("ACL" => "use_vacation",       "POST" => "use_vacation"),
                     "C"  => array("ACL" => "custom_sieve",       "POST" => "own_script"),
                     "I"  => array("ACL" => "only_local",         "POST" => "only_local"));

      /* Check acls and set new value if allowed. 
         If we do not have permission to change the value 
          check for the old value and use this */
      foreach($Flags as $flag => $val){
        $acl  = $val['ACL'];
        $post = $val['POST'];
        if(chkacl($this->acl,$acl) ==""){
          if (isset($_POST[$post])){
            $tmp.= $flag;
          }
        }else{
          if(preg_match("/".$flag."/",$this->gosaMailDeliveryMode)){
            $tmp.= $flag ;
          }
        }
      }

      /* ! Inverse flag handling for drop_own_mails */
      if(chkacl($this->acl,"drop_own_mails") == ""){
        if (!isset($_POST['drop_own_mails'])){
          $tmp.= "L";
        }
      }else{
        if(preg_match("/L/",$this->gosaMailDeliveryMode)){
          $tmp.= "L" ;
        }
      }

      /* If one of these acls are given, we are allowed to enable disable the the spam settings */
      if(chkacl($this->acl,"gosaSpamMailbox") == "" || chkacl($this->acl,"gosaSpamSortLevel") ==""){
        if (isset($_POST["use_spam_filter"])){
          $tmp.= "S";
        }
      }else{
        if(preg_match("/S/",$this->gosaMailDeliveryMode)){
          $tmp.= "S" ;
        }
      }
      
      /* Check if something has changed an assign new gosaMailDeliveryMode */
      $tmp= "[$tmp]";
      if ($this->gosaMailDeliveryMode != $tmp){
        $this->is_modified= TRUE;
      }
      $this->gosaMailDeliveryMode= $tmp;

      if(chkacl($this->acl,"gosaVacationTimer") == "" && preg_match("/V/",$this->gosaMailDeliveryMode)){
        if(isset($_POST['gosaVacationStart'])){
          $this->gosaVacationStart = $_POST['gosaVacationStart'];
        }
        if(isset($_POST['gosaVacationStop'])){
          $this->gosaVacationStop = $_POST['gosaVacationStop'];
        }
      }
    }
  }


  /* Save data to LDAP, depending on is_account we save or delete */
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save arrays */
    $this->attrs['gosaMailAlternateAddress']= $this->gosaMailAlternateAddress;
    $this->attrs['gosaMailForwardingAddress']= $this->gosaMailForwardingAddress;

    /* Adapt attributes if needed */
    $method= new $this->method($this->config);
    $id= $method->uattrib;

    $method->fixAttributesOnStore($this);

    /* Remove Mailquota if = "" or "0"  */
    if((isset($this->attrs['gosaMailQuota']))&&(!$this->attrs['gosaMailQuota'])) {
      $this->attrs['gosaMailQuota']=0;
    }

    if(empty($this->attrs['gosaSpamMailbox'])){
      unset($this->attrs['gosaSpamMailbox']);
    }

    $this->attrs['mail'] = strtolower($this->attrs['mail']); 

    /* Remove attributes - if not needed */
    if (!preg_match('/V/', $this->gosaMailDeliveryMode)){
      unset($this->attrs['gosaVacationStart']);
      unset($this->attrs['gosaVacationStop']);
    }

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving mail account failed"));

    /* Only do IMAP actions if we are not a template */
    if (!$this->is_template){

      if ($method->connect($this->gosaMailServer)){
        $method->updateMailbox($this->folder_prefix.$this->$id);
        
        $method->setQuota($this->folder_prefix.$this->$id, $this->gosaMailQuota);
        $method->disconnect();

        /* Only talk with sieve if the mail account already exists */
        if($this->initially_was_account){
         
          /* Write sieve information only if not in C mode */
          if (!is_integer(strpos($this->gosaMailDeliveryMode, "C"))){
            $method->configureFilter($this->$id,
                $this->gosaMailDeliveryMode,
                $this->mail,
                $this->gosaMailAlternateAddress,
                $this->gosaMailMaxSize,
                $this->gosaSpamMailbox,
                $this->gosaSpamSortLevel,
                $this->gosaVacationMessage);
            $this->is_modified = TRUE;
          }else{
            $this->sieve_management->save();
          }
        }
      }
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify", array('uid'=> $this->uid));
      }
    } else {
      $this->handle_post_events("add", array('uid'=>$this->uid));
    }

    $this->updateSharedFolder();
  }


  /* Check formular input */
  function check()
  {
    if(!$this->is_account) return(array());

    $ldap= $this->config->get_ldap_link();

    /* Call common method to give check the hook */
    $message= plugin::check();

    if(empty($this->gosaMailServer)){
      $message[]= _("There is no valid mailserver specified, please add one in the system setup.");
    }

    /* must: mail */
    if ($this->mail == ""){
      $message[]= _("The required field 'Primary address' is not set.");
    }
    if ($this->is_template){
      if (!is_email($this->mail, TRUE)){
        $message[]= _("Please enter a valid email address in 'Primary address' field.");
      }
    } else {
      if (!is_email($this->mail)){
        $message[]= _("Please enter a valid email address in 'Primary address' field.");
      }
    }
    $ldap->cd($this->config->current['BASE']);
    $ldap->search ("(&(!(objectClass=gosaUserTemplate))(objectClass=gosaMailAccount)(|(mail=".$this->mail.")(gosaMailAlternateAddress=".$this->mail."))(!(uid=".$this->uid."))(!(cn=".$this->uid.")))", array("uid"));
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
    if (is_integer(strpos($this->gosaMailDeliveryMode, "R")) && 
        $this->gosaMailMaxSize == ""){

      $message[]= _("You need to set the maximum mail size in order to reject anything.");
    }

    if((preg_match("/S/", $this->gosaMailDeliveryMode))&&(empty($this->gosaSpamMailbox))) {
      $message[]= _("You specified Spam settings, but there is no Folder specified.");
    }

    if (preg_match('/V/', $this->gosaMailDeliveryMode) && $this->gosaVacationStart > $this->gosaVacationStop){
      $message[]= _("Time interval to show vacation message is not valid.");
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
          array_push($this->$val, strtolower(rewrite($value)));
        }
      }
    }
    $this->mail= strtolower(rewrite($this->mail));
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
    $this->gosaMailForwardingAddress= array_remove_entries ($addresses, $this->gosaMailForwardingAddress);
    $this->is_modified= TRUE;
  }


  /* Add given mail address to the list of alternate adresses , 
      check if this mal address is used, skip adding in this case */
  function addAlternate($address)
  {
    $ldap= $this->config->get_ldap_link();
    $address= strtolower($address);
      
    /* Is this address already assigned in LDAP? */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(!(objectClass=gosaUserTemplate))(objectClass=gosaMailAccount)(|(mail=$address)"."(gosaMailAlternateAddress=$address)))", array("uid"));

    if ($ldap->count() > 0){
      $attrs= $ldap->fetch ();
      return ($attrs["uid"][0]);
    }

    /* Add to list of alternates */
    if (!in_array($address, $this->gosaMailAlternateAddress)){
      $this->gosaMailAlternateAddress[]= $address;
      $this->is_modified= TRUE;
    }

    sort ($this->gosaMailAlternateAddress);
    reset ($this->gosaMailAlternateAddress);
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

  
  /* Create the mail part for the copy & paste dialog */
  function getCopyDialog()
  {
    if(!$this->is_account) return("");
    $smarty = get_smarty();
    $smarty->assign("mail",$this->mail); 
    $smarty->assign("gosaMailAlternateAddress",$this->gosaMailAlternateAddress);
    $smarty->assign("gosaMailForwardingAddress",$this->gosaMailForwardingAddress);
    $str = $smarty->fetch(get_template_path("copypaste.tpl",TRUE, dirname(__FILE__)));

    $ret = array();
    $ret['status'] = "";
    $ret['string'] = $str;
    return($ret);
  }

  function saveCopyDialog()
  {
    if(!$this->is_account) return;  

    /* Execute to save mailAlternateAddress && gosaMailForwardingAddress */
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


  function allow_remove()
  {
    if (isset($this->config->current['MAILMETHOD'])){
      $method= $this->config->current['MAILMETHOD'];
      if(preg_match("/olab/i",$method)){
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->config->current['BASE']);
        $ldap->cat($this->dn);
        if($ldap->count()){
          $attrs = $ldap->fetch();
          if(isset($attrs['kolabDeleteFlag'])){ 
            return(_("Waiting for kolab to remove mail properties."));
          }elseif(in_array("gosaMailAccount",$attrs['objectClass'])){
            return(_("Please remove the mail account first, to allow kolab to call its remove methods."));
          }
        }
      }
    }
  }

  /* Upated shared folder ACLs 
   */
  function updateSharedFolder()
  {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaMailAccount)(memberUid=".$this->uid."))",array('dn','cn'));
    if(class_exists("grouptabs")){
      while($attrs = $ldap->fetch()){
        $tmp = new grouptabs($this->config, $this->config->data['TABS']['GROUPTABS'], $attrs['dn']);
        if(isset($tmp->by_object['mailgroup'])){
          $tmp->by_object['mailgroup']->members= $tmp->by_object['group']->memberUid;
          if(!$this->is_account){
            $tmp->by_object['mailgroup']->removeUserAcl($this->uid);
            $tmp->by_object['mailgroup']->removeUserAcl($this->mail);
          }
          $tmp->by_object['mailgroup']->save();
        }
      }
    } 
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
