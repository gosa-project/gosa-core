<?php
/*! 
  \brief   mail plugin
  \author  Fabian Hicker  <Fabian.Hickert@GONICUS.de>
  \version 2.6.2
  \date    03.12.2007

  This class provides the functionality to read and write all attributes
  relevant for gosaMailAccounts from/to the LDAP. 
  It does syntax checking and displays the formulars required.
  Special handling like sieve or imap actions will be implemented 
  by the mailMethods.


Functions :

 - mailAccount (&$config, $dn= NULL)
 - execute()
 - save_object()
 - get_vacation_templates()
 - addForwarder($address)
 - delForwarder($addresses)
 - addAlternate($address)
 - delAlternate($addresses)
 - prepare_vacation_template($contents)
 - remove_from_parent()
 - save()
 - check()
 - adapt_from_template($dn, $skip= array())
 - getCopyDialog()
 - saveCopyDialog()
 - PrepareForCopyPaste($source)
 - get_multi_edit_values()
 - multiple_check()
 - set_multi_edit_values($values)
 - init_multiple_support($attrs,$all)
 - get_multi_init_values()
 - multiple_execute()
 - multiple_save_object()
 - make_name($attrs)
 - plInfo()


 */

/*

    FLAG    POSTNAME                DESC
    ####################################################################################################
    L  (!)  only_local              Enables: "User is only allowed to send and receive local mails"

                                    <REVERSED> If checked in the ui, the flag is not present in the 
                                     gosaMailDeliveryMode attribute. If its unchecked in the ui, the 
                                     character 'L' is added to the delivery flags.

    R       use_mailsize_limit      Enables rule: "Reject mails bigger than [n] MB";
    S       use_spam_filter         Enables rule: "Move mails tagged with SPAM level greater than [n] 
                                     to folder [x]"
    V       use_vacation            Enables: "Vacation message"
    C       own_script              Enables: "Use custom sieve script (disables all Mail options!)"
    I       drop_own_mails          Enables: "No delivery to own mailbox."

*/

class mailAccount extends plugin
{
  /* Definitions */
  var $plHeadline     = "Mail";
  var $plDescription  = "Manage personal mail settings";
  var $view_logged    = FALSE;
  var $is_account     = FALSE;
  var $initially_was_account = FALSE;

  /* GOsa mail attributes */
  var $mail                               = "";
  var $gosaVacationStart                  = "";
  var $gosaVacationStop                   = "";
  var $gosaMailAlternateAddress           = array();
  var $gosaMailForwardingAddress          = array();
  var $gosaMailDeliveryMode               = "[L        ]";
  var $gosaMailServer                     = "";
  var $gosaMailQuota                      = "";
  var $gosaMailMaxSize                    = "";
  var $gosaVacationMessage                = "";
  var $gosaSpamSortLevel                  = "";
  var $gosaSpamMailbox                    = "";

  /* The methods defaults */
  var $quotaUsage     = -1; // Means unknown

  var $mailMethod      = NULL;
  var $MailDomain      = "";
  var $sieveManagementUsed = FALSE;
  var $vacationTemplates = array();
  var $sieve_management = NULL;
  var $mailAddressSelect = FALSE;
  var $initial_uid    = "";
  var $mailDomainPart = "";
  var $mailDomainParts = array();
  var $MailBoxes = array("INBOX");

  /* Used LDAP attributes && classes */
  var $attributes= array(
      "mail", "gosaMailServer","gosaMailQuota", "gosaMailMaxSize","gosaMailForwardingAddress",
      "gosaMailDeliveryMode", "gosaSpamSortLevel", "gosaSpamMailbox","gosaMailAlternateAddress",
      "gosaVacationStart","gosaVacationStop", "gosaVacationMessage", "gosaMailAlternateAddress", 
      "gosaMailForwardingAddress");
  var $objectclasses= array("gosaMailAccount");

  var $multiple_support = TRUE;

  var $uid = "";
  var $cn  = "";


  /*! \brief  Initialize the mailAccount 
   */
  function __construct (&$config, $dn= NULL)
  {
    plugin::__construct($config,$dn); 

    /* Get attributes from parent object 
     */
    foreach(array("uid","cn") as $attr){
      if(isset($this->parent->by_object['group']) && isset($this->parent->by_object['group']->$attr)){
        $this->$attr = &$this->parent->by_object['group']->$attr;
      }elseif(isset($this->attrs[$attr])){
        $this->$attr = $this->attrs[$attr][0];
      }
    }

    /* Intialize the used mailMethod
     */
    $tmp = new mailMethod($config,$this);
    $this->mailMethod       = $tmp->get_method();
    $this->mailMethod->fixAttributesOnLoad();
    $this->mailDomainParts  = $this->mailMethod->getMailDomains();
    $this->SpamLevels = $this->mailMethod->getSpamLevels();

    /* Remember account status 
     */
    $this->initially_was_account = $this->is_account;

    /* Initialize vacation settings, if enabled.
     */   
    if(empty($this->gosaVacationStart) && $this->mailMethod->vacationRangeEnabled()){
      $this->gosaVacationStart = time();
      $this->gosaVacationStop = time();
    }

    /* Read vacation templates 
     */
    $this->vacationTemplates = $this->get_vacation_templates();

    /* Initialize configured values 
     */ 
    if($this->is_account && !$this->is_template){

      if($this->mailMethod->connect() && $this->mailMethod->account_exists()){

        /* Read quota */
        $this->gosaMailQuota = $this->mailMethod->getQuota($this->gosaMailQuota);
        $this->quotaUsage    = $this->mailMethod->getQuotaUsage($this->quotaUsage);
        if($this->mailMethod->is_error()){
          msg_dialog::display(_("Mail error"), sprintf(_("Cannot read quota settings: %s"), 
                $this->mailMethod->get_error()), ERROR_DIALOG);
        }
        
        /* Read mailboxes */
        $this->MailBoxes = $this->mailMethod->getMailboxList($this->MailBoxes);
        if($this->mailMethod->is_error()){
          msg_dialog::display(_("Mail error"), sprintf(_("Cannot get list of mailboxes: %s"), 
                $this->mailMethod->get_error()), ERROR_DIALOG);
        }
          
      }elseif(!$this->mailMethod->is_connected()){
        msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"), 
              $this->mailMethod->get_error()), ERROR_DIALOG);
      }elseif(!$this->mailMethod->account_exists()){
        msg_dialog::display(_("Mail error"), sprintf(_("Mailbox '%s' doesn't exists on mail server: %s"), 
              $this->mailMethod->get_account_id(),$this->gosaMailServer), ERROR_DIALOG);
      }

      /* If the doamin part is selectable, we have to split the mail address
       */
      if(!(!$this->mailMethod->isModifyableMail() && $this->is_account)){
        if($this->mailMethod->domainSelectionEnabled()){
          $this->mailDomainPart = preg_replace("/^[^@]*+@/","",$this->mail);
          $this->mail = preg_replace("/@.*$/","\\1",$this->mail);
          if(!in_array_strict($this->mailDomainPart,$this->mailDomainParts)){
            $this->mailDomainParts[] = $this->mailDomainPart;
          }
        }
      }

      /* Load attributes containing arrays */
      foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){
        $this->$val= array();
        if (isset($this->attrs["$val"]["count"])){
          for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
            array_push($this->$val, $this->attrs["$val"][$i]);
          }
        }
      }
    }

    /* Intialize sieveManagement if necessary */
    if($this->mailMethod->allowSieveManagement()){
      $this->mailboxList = &$this->MailBoxes;
      $this->sieve_management = new sieveManagement($this->config,$this->dn,$this,$this->mailMethod->getUAttrib());
    }

    /* Disconnect mailMethod. Connect on demand later. 
     */
    $this->mailMethod->disconnect();

    /* Convert start/stop dates */
    #TODO: use date format
    $this->gosaVacationStart= date('d.m.Y', $this->gosaVacationStart);
    $this->gosaVacationStop= date('d.m.Y', $this->gosaVacationStop);
  }


  function execute()
  {

    /* Call parent execute */
    $display = plugin::execute();

    $theme = getThemeName();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }


    /****************
      Account status
     ****************/

    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable() && $this->mailMethod->accountRemoveAble()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable() && $this->mailMethod->accountCreateable()){
        $this->is_account= TRUE;
      }
    }
    if(!$this->multiple_support_active){
      if (!$this->is_account && $this->parent === NULL){
        switch ($theme) {
          case 'classic':
            $symbol = "<img alt='' src='images/small-error.png' align='middle'>";
            break;
          
          default:
            $symbol = "<i class='material-icons'>report</i>";
            break;
        }
        $display = "<div class='valign-wrapper'>".$symbol.msgPool::noValidExtension(_("Mail"))."</div>";
        $display.= back_to_main();
        return $display;
      }
      if ($this->parent !== NULL){
        if ($this->is_account){ 
          $reason = "";
          if(!$this->mailMethod->accountRemoveable($reason)){
            $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Mail")),$reason ,TRUE,TRUE);
          }else{
            $display= $this->show_disable_header(msgPool::removeFeaturesButton(_("Mail")),msgPool::featuresEnabled(_("Mail")));
          }
        } else {
          $reason = "";
          if(!$this->mailMethod->accountCreateable($reason)){
            $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Mail")),$reason ,TRUE,TRUE);
          }else{
            $display= $this->show_enable_header(msgPool::addFeaturesButton(_("Mail")),msgPool::featuresDisabled(_("Mail")));
          }
          return ($display);
        }
      }
    }

    /****************
      Sieve Management Dialog
     ****************/
    if($this->mailMethod->allowSieveManagement()){
      if(isset($_POST['sieveManagement'])
          && preg_match("/C/",$this->gosaMailDeliveryMode)
          && $this->acl_is_writeable("sieveManagement") 
          && $this->mailMethod->allowSieveManagement()) {
        $this->dialog = $this->sieve_management;
      }
      if(isset($_POST['sieve_cancel'])){
        $this->dialog = FALSE;
      }
      if(isset($_POST['sieve_finish'])){
        $this->sieve_management = $this->dialog;
        $this->dialog = FALSE;
      }
      if(is_object($this->dialog)){
        $this->dialog->save_object();
        return($this->dialog->execute());
      }
    }

    /****************
      Forward addresses 
     ****************/

    if (isset($_POST['add_local_forwarder'])){
      $this->mailAddressSelect=  new mailAddressSelect($this->config, get_userinfo());
      $this->dialog= TRUE;
    }
    if (isset($_POST['mailAddressSelect_cancel']) || isset($_POST['cancel-abort'])){
      $this->mailAddressSelect= FALSE;
      $this->dialog= FALSE;
    }

    if (isset($_POST['mailAddressSelect_save']) || isset($_POST['ok-save']) && $this->mailAddressSelect instanceOf mailAddressSelect){

      if($this->acl_is_writeable("gosaMailForwardingAddress")){
        $list = $this->mailAddressSelect->save();
        foreach ($list as $entry){
          $val = $entry['mail'][0];
          if (!in_array_strict($val, $this->gosaMailAlternateAddress) && $val != $this->mail){
            $this->addForwarder($val);
            $this->is_modified= TRUE;
          }
        }
        $this->mailAddressSelect= FALSE;
        $this->dialog= FALSE;
      } else {
        msg_dialog::display(_("Error"), _("Please select an entry!"), ERROR_DIALOG);
      }
    }

    if($this->mailAddressSelect instanceOf mailAddressSelect){
      $used  = array();
      $used['mail'] = array_values($this->gosaMailAlternateAddress);  
      $used['mail'] = array_merge($used['mail'], array_values($this->gosaMailForwardingAddress));  
      $used['mail'][] = $this->mail;

      // Build up blocklist
      session::set('filterBlacklist', $used);
      return($this->mailAddressSelect->execute());
    }

    if (isset($_POST['add_forwarder']) && isset($_POST['forward_address'])){
      if (!empty($_POST['forward_address'])){
        $address= get_post('forward_address');
        $valid= FALSE;
        if (!tests::is_email($address)){
          if (!tests::is_email($address, TRUE)){
            if ($this->is_template){
              $valid= TRUE;
            } else {
              msg_dialog::display(_("Error"), msgPool::invalid(_("Mail address"),
                    "","","your-address@your-domain.com"),ERROR_DIALOG);
            }
          }
        } elseif ($address == $this->mail
            || in_array_strict($address, $this->gosaMailAlternateAddress)) {
          msg_dialog::display(_("Error"),_("Cannot add primary address to the list of forwarders!") , ERROR_DIALOG);
        } else {
          $valid= TRUE;
        }
        if ($valid){
          if($this->acl_is_writeable("gosaMailForwardingAddress")){
            $this->addForwarder ($address);
            $this->is_modified= TRUE;
          }
        }
      }
    }
    if (isset($_POST['delete_forwarder']) && isset($_POST['forwarder_list'])){
      $this->delForwarder ($_POST['forwarder_list']);
    }
    if ($this->mailAddressSelect instanceOf mailAddressSelect){
    
      return($this->mailAddressSelect->execute());
    }


    /****************
      Alternate addresses 
     ****************/

    if (isset($_POST['add_alternate'])){
      $valid= FALSE;
      if (!tests::is_email($_POST['alternate_address'])){
        if ($this->is_template){
          if (!(tests::is_email($_POST['alternate_address'], TRUE))){
            msg_dialog::display(_("Error"),msgPool::invalid(_("Mail address"),"","","your-domain@your-domain.com"),ERROR_DIALOG);
          } else {
            $valid= TRUE;
          }
        } else {
          msg_dialog::display(_("Error"),msgPool::invalid(_("Mail address"),"","","your-domain@your-domain.com"),ERROR_DIALOG);
        }
      } else {
        $valid= TRUE;
      }
      if ($valid && ($user= $this->addAlternate (get_post('alternate_address'))) != ""){
        $ui= get_userinfo();
        $addon= "";
        if ($user[0] == "!") {
          $addon= sprintf(_("Address is already in use by group '%s'."), mb_substr($user, 1));
        } else {
          $addon= sprintf(_("Address is already in use by user '%s'."), $user);
        }
        msg_dialog::display(_("Error"), msgPool::duplicated(_("Mail address"))."<br><br><i>".
            "$addon</i>", ERROR_DIALOG);
      }
    }
    if (isset($_POST['delete_alternate']) && isset($_POST['alternates_list'])){
      $this->delAlternate ($_POST['alternates_list']);
    }

    /****************
      SMARTY- Assign smarty variables 
     ****************/
    $smarty = get_smarty();

    $smarty->assign("initially_was_account", $this->initially_was_account);
    $smarty->assign("isModifyableMail"  , $this->mailMethod->isModifyableMail());
    $smarty->assign("isModifyableServer", $this->mailMethod->isModifyableServer());
    $smarty->assign("mailEqualsCN", $this->mailMethod->mailEqualsCN());

    $tmp  = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $transl){
      $smarty->assign("$name"."ACL", $this->getacl($name));
    }
    foreach($this->attributes as $attr){
      $smarty->assign($attr,set_post($this->$attr));
    }
    $smarty->assign("quotaEnabled", $this->mailMethod->quotaEnabled());
    if($this->mailMethod->quotaEnabled()){
      $smarty->assign("quotaUsage",   mailMethod::quota_to_image($this->quotaUsage,$this->gosaMailQuota));
      $smarty->assign("gosaMailQuota",set_post($this->gosaMailQuota));
    }
    $smarty->assign("domainSelectionEnabled", $this->mailMethod->domainSelectionEnabled());
    $smarty->assign("MailDomains", set_post($this->mailDomainParts));
    $smarty->assign("MailDomain" , set_post($this->mailDomainPart));
    $smarty->assign("MailServers", set_post($this->mailMethod->getMailServers()));
    $smarty->assign("allowSieveManagement", $this->mailMethod->allowSieveManagement());
    $smarty->assign("own_script",  $this->sieveManagementUsed);

    /* _Multiple users vars_ */
    foreach($this->attributes as $attr){
      $u_attr = "use_".$attr;
      $smarty->assign($u_attr,in_array_strict($attr,$this->multi_boxes));
    }
    foreach(array("only_local","gosaMailForwardingAddress","use_mailsize_limit","drop_own_mails","use_vacation","use_spam_filter") as $attr){
      $u_attr = "use_".$attr;
      $smarty->assign($u_attr,in_array_strict($attr,$this->multi_boxes));
    }


    /****************
      SMARTY- Assign flags 
     ****************/

    $types = array(
        "V"=>"use_vacation",
        "S"=>"use_spam_filter",
        "R"=>"use_mailsize_limit",
        "I"=>"drop_own_mails",
        "C"=>"own_script");
    foreach($types as $option => $varname){
      if (preg_match("/".$option."/", $this->gosaMailDeliveryMode)) {
        $smarty->assign($varname, "checked");
      } else {
        $smarty->assign($varname, "");
      }
    }
    if (!preg_match("/L/", $this->gosaMailDeliveryMode)) {
      $smarty->assign("only_local", "checked");
    } else {
      $smarty->assign("only_local", "");
    }


    /****************
      Smarty- Vacation settings 
     ****************/
    $smarty->assign("rangeEnabled", FALSE);
    $smarty->assign("template", "");
    if (count($this->vacationTemplates)){
      $smarty->assign("show_templates", "true");
      $smarty->assign("vacationtemplates", set_post($this->vacationTemplates));
      if (isset($_POST['vacation_template'])){
        $smarty->assign("template", set_post(get_post('vacation_template')));
      }
    } else {
      $smarty->assign("show_templates", "false");
    }

    /* Vacation range assigments
     */
    if($this->mailMethod->vacationRangeEnabled()){
      $smarty->assign("rangeEnabled", TRUE);
    }

    /* fill filter settings 
     */
    $smarty->assign("spamlevel", $this->SpamLevels);
    $smarty->assign("spambox"  , $this->MailBoxes);
    $smarty->assign("is_template", $this->is_template);
    $smarty->assign("multiple_support",$this->multiple_support_active);  
    return($display.$smarty->fetch(get_template_path("generic.tpl",TRUE,dirname(__FILE__))));
  }



  /* Save data to object */
  function save_object()
  {
    if (isset($_POST['mailTab'])){

      /* Save ldap attributes */
      $mail   = $this->mail;
      $server = $this->gosaMailServer;
      plugin::save_object();

      if(!$this->mailMethod->isModifyableServer() && $this->initially_was_account && !$this->is_template){
        $this->gosaMailServer = $server;
      }

      if(!$this->mailMethod->isModifyableMail() && $this->initially_was_account && !$this->is_template){
        $this->mail = $mail;
      }else{

        /* Get posted mail domain part, if necessary  
         */
        if($this->mailMethod->domainSelectionEnabled() && isset($_POST['MailDomain'])){
          if(in_array_strict(get_post('MailDomain'), $this->mailDomainParts)){
            $this->mailDomainPart = get_post('MailDomain');
          }
        }
      }

      /* Import vacation message? 
       */
      if (isset($_POST["import_vacation"]) && isset($this->vacationTemplates[$_POST["vacation_template"]])){
        if($this->multiple_support_active){
          $contents = ltrim(preg_replace("/^DESC:.*$/m","",file_get_contents(get_post("vacation_template"))));
        }else{
          $contents = $this->prepare_vacation_template(file_get_contents(get_post("vacation_template")));
        }
        $this->gosaVacationMessage= htmlspecialchars($contents);
      }

      /* Handle flags 
       */
      if(isset($_POST['own_script'])){
        if(!preg_match("/C/",$this->gosaMailDeliveryMode)){
          $str= preg_replace("/[\[\]]/","",$this->gosaMailDeliveryMode);
          $this->gosaMailDeliveryMode = "[".$str."C]";
        }
      }else{

        /* Assemble mail delivery mode
           The mode field in ldap consists of values between braces, this must
           be called when 'mail' is set, because checkboxes may not be set when
           we're in some other dialog.

           Example for gosaMailDeliveryMode [LR        ]
           L -  Local delivery
           R -  Reject when exceeding mailsize limit
           S -  Use spam filter
           V -  Use vacation message
           C -  Use custm sieve script
           I -  Only insider delivery */

        $tmp= preg_replace("/[^a-z]/i","",$this->gosaMailDeliveryMode);


        /* Handle delivery flags */
        if($this->acl_is_writeable("gosaMailDeliveryModeL")){
          if(!preg_match("/L/",$tmp) && !isset($_POST['only_local'])){
            $tmp.="L";
          }elseif(preg_match("/L/",$tmp) && isset($_POST['only_local'])){
            $tmp = preg_replace("/L/","",$tmp);
          }
        }
        $opts = array(
            "R"   => "use_mailsize_limit",
            "S"   => "use_spam_filter",
            "V"   => "use_vacation",
            "C"   => "own_script",
            "I"   => "drop_own_mails");

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

        /* Get start/stop values for vacation scope of application
         */
        if($this->mailMethod->vacationRangeEnabled()){
          if($this->acl_is_writeable("gosaVacationMessage") && preg_match("/V/",$this->gosaMailDeliveryMode)){
            if(isset($_POST['gosaVacationStart'])){
              $this->gosaVacationStart = get_post('gosaVacationStart');
            }
            if(isset($_POST['gosaVacationStop'])){
              $this->gosaVacationStop = get_post('gosaVacationStop');
            }
          }
        }
      }
    }
  }


  /*! \brief  Parse vacation templates and build up an array
    containing 'filename' => 'description'. 
    Used to fill vacation dropdown box.
    @return Array   All useable vacation templates.
   */ 
  function get_vacation_templates()
  {
    $vct = array();
    if ($this->config->get_cfg_value("core","vacationTemplateDirectory") != ""){
      $dir= $this->config->get_cfg_value("core","vacationTemplateDirectory");
      if (is_dir($dir) && is_readable($dir)){
        $dh = opendir($dir);
        while($file = readdir($dh)){
          $description= "";
          if (is_file($dir."/".$file)){
            $fh = fopen($dir."/".$file, "r");
            $line= fgets($fh, 256);
            if (!preg_match('/^DESC:/', $line)){
              msg_dialog::display(_("Configuration error"), sprintf(_("No DESC tag in vacation template '%s'!"), $file), ERROR_DIALOG);
            }else{
              $description= trim(preg_replace('/^DESC:\s*/', '', $line));
            }
            fclose ($fh);
          }
          if ($description != ""){
            $vct["$dir/$file"]= $description;
          }
        }
        closedir($dh);
      }
    }
    return($vct); 
  }


  /*! \brief  Adds the given mail address to the list of mail forwarders 
   */ 
  function addForwarder($address)
  {
    if(empty($address)) return;
    if($this->acl_is_writeable("gosaMailForwardingAddress")){
      $this->gosaMailForwardingAddress[]= $address;
      $this->gosaMailForwardingAddress= array_unique ($this->gosaMailForwardingAddress);
      sort ($this->gosaMailForwardingAddress);
      reset ($this->gosaMailForwardingAddress);
      $this->is_modified= TRUE;
    }else{
      msg_dialog::display(_("Permission error"), _("You have no permission to modify these addresses!"), ERROR_DIALOG);
    }
  }


  /*! \brief  Removes the given mail address from the list of mail forwarders 
   */ 
  function delForwarder($addresses)
  {
    if($this->acl_is_writeable("gosaMailForwardingAddress")){
      $this->gosaMailForwardingAddress= array_remove_entries ($addresses, $this->gosaMailForwardingAddress);
      $this->is_modified= TRUE;
    }else{
      msg_dialog::display(_("Permission error"), _("You have no permission to modify these addresses!"), ERROR_DIALOG);
    }
  }


  /*! \brief  Add given mail address to the list of alternate adresses ,
    .          check if this mal address is used, skip adding in this case 
   */ 
  function addAlternate($address)
  {
    if(empty($address)) return;
    if($this->acl_is_writeable("gosaMailAlternateAddress")){
      $ldap= $this->config->get_ldap_link();
      $address= strtolower($address);

      /* Is this address already assigned in LDAP? */
      $ldap->cd ($this->config->current['BASE']);
      $ldap->search ("(&(!(objectClass=gosaUserTemplate))(objectClass=gosaMailAccount)(|(mail=$address)".
          "(alias=$address)(gosaMailAlternateAddress=$address)))", array("uid", "cn"));
      if ($ldap->count() > 0){
        $attrs= $ldap->fetch ();
        if (!isset($attrs["uid"])) {
          return ("!".$attrs["cn"][0]);
        }
        return ($attrs["uid"][0]);
      }
      if (!in_array_strict($address, $this->gosaMailAlternateAddress)){
        $this->gosaMailAlternateAddress[]= $address;
        $this->is_modified= TRUE;
      }
      sort ($this->gosaMailAlternateAddress);
      reset ($this->gosaMailAlternateAddress);
      return ("");
    }else{
      msg_dialog::display(_("Permission error"), _("You have no permission to modify these addresses!"), ERROR_DIALOG);
    }
  }


  /*! \brief  Removes the given mail address from the alternate addresses list 
   */ 
  function delAlternate($addresses)
  {
    if($this->acl_is_writeable("gosaMailAlternateAddress")){
      $this->gosaMailAlternateAddress= array_remove_entries ($addresses,$this->gosaMailAlternateAddress);
      $this->is_modified= TRUE;
    }else{
      msg_dialog::display(_("Permission error"), _("You have no permission to modify these addresses!"), ERROR_DIALOG);
    }
  }


  /*! \brief  Prepare importet vacation string. \
    .         Replace placeholder like %givenName a.s.o.
    @param  string  Vacation string
    @return string  Completed vacation string
   */
  private function prepare_vacation_template($contents)
  {
    /* Replace attributes */
    $attrs = array();
    $obj   = NULL;
    if(isset($this->parent->by_object['user'])){
      $attrs  = $this->parent->by_object['user']->attributes;
      $obj    = $this->parent->by_object['user'];
    }else{
      $obj    = new user($this->config,$this->dn);
      $attrs  = $obj->attributes;
    }
    if($obj){

      /* Replace vacation start and end time */
      if($this->mailMethod->vacationRangeEnabled()){
        if(preg_match("/%start/",$contents)){
          $contents = preg_replace("/%start/",$this->gosaVacationStart,$contents);
        }
        if(preg_match("/%end/",$contents)){
          $contents = preg_replace("/%end/",$this->gosaVacationStop,$contents);
        }
      }else{
        if(preg_match("/%start/",$contents)){
          $contents = preg_replace("/%start/", _("unknown"),$contents);
        }
        if(preg_match("/%end/",$contents)){
          $contents = preg_replace("/%end/", _("unknown"), $contents);
        }
      }

      foreach ($attrs as $val){

        // We can only replace strings here
        if(!is_string($obj->$val)) continue;

        if(preg_match("/dateOfBirth/",$val)){
          if($obj->use_dob){
            $contents= preg_replace("/%$val/",date("Y-d-m",$obj->dateOfBirth),$contents);
          }
        }else {
          $contents= preg_replace("/%$val/",
              $obj->$val, $contents);
        }

      }
    }
    $contents = ltrim(preg_replace("/^DESC:.*$/m","",$contents),"\n ");
    return($contents);
  }


  /*! \brief  Removes the mailAccount extension from ldap 
   */  
  function remove_from_parent()
  {
    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    /* If domain part was selectable, contruct mail address */
    if($this->mailMethod->domainSelectionEnabled()){
      $this->mail = $this->mail."@".$this->mailDomainPart;
    }

    /* Update sharedFolder dependencies. 
       Open each shared folder and remove this account. 
       Then Save the group to ensure that all necessary 
        actions will be taken (imap acls updated aso.).
     */
    $ldap = $this->config->get_ldap_link();    
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaMailAccount)(memberUid=".$this->uid."))",array("dn"));
    while($attrs = $ldap->fetch()){
      $grp = new grouptabs($this->config, $this->config->data['TABS']['GROUPTABS'], $attrs['dn']);
      if(isset($grp->by_object['mailgroup']) && isset($grp->by_object['group'])){
        $grp->by_object['group']->removeUser($this->uid);

        /* Do not save the complete group! This will quit the complete membership 
         */
        $grp->by_object['mailgroup']->save();
      } 
    }

    // Do NOT remove the mail attribute while it is used in the Fax Account.
    if(isset($this->parent->by_object['gofaxAccount'])){
        $fax = $this->parent->by_object['gofaxAccount'];

        // Fax delivery to the mail account is activated, keep the mail attribute.
        if($fax->goFaxDeliveryMode & 32){
           $this->attributes = array_remove_entries(array('mail'), $this->attributes)  ;
        }
    }

    /* Remove GOsa attributes */
    plugin::remove_from_parent();

    /* Zero arrays */
    $this->attrs['gosaMailAlternateAddress'] = array();
    $this->attrs['gosaMailForwardingAddress']= array();


    $this->mailMethod->fixAttributesOnRemove();
    $this->cleanup();

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify ($this->attrs);

    /* Add "view" to logging class */
    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }
    
    /* Let the mailMethod remove this mailbox, e.g. from imap and
       update shared folder membership, ACL may need to be updated. 
     */
    if (!$this->is_template){

      if(!$this->mailMethod->connect()){
        msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"), 
              $this->mailMethod->get_error()), ERROR_DIALOG);
      }else{
        if(!$this->mailMethod->deleteMailbox()){
          msg_dialog::display(_("Mail error"), sprintf(_("Cannot remove mailbox: %s"), 
                $this->mailMethod->get_error()), ERROR_DIALOG);
        }
      }
    }
    $this->mailMethod->disconnect();

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove",array("uid" => $this->uid));
  }


  /*! \brief  Save the mailAccount settings to the ldap database.
   */
  function save()
  {
    $ldap= $this->config->get_ldap_link();

    /* If domain part was selectable, contruct mail address */
    if(!(!$this->mailMethod->isModifyableMail() && $this->initially_was_account)){

      if($this->mailMethod->domainSelectionEnabled()){
        $this->mail = $this->mail."@".$this->mailDomainPart;
      }

      /* Enforce lowercase mail address and trim whitespaces
       */
      $this->mail = trim(strtolower($this->mail));
    }


    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save arrays */
    $this->attrs['gosaMailAlternateAddress'] = $this->gosaMailAlternateAddress;
    $this->attrs['gosaMailForwardingAddress']= $this->gosaMailForwardingAddress;

    if(!$this->mailMethod->vacationRangeEnabled()){
      $this->attrs['gosaVacationStart'] = $this->attrs['gosaVacationStop'] = array();
    }elseif (!preg_match('/V/', $this->gosaMailDeliveryMode)){
      unset($this->attrs['gosaVacationStart']);
      unset($this->attrs['gosaVacationStop']);
    } else {
      /* Adapt values to be timestamps */
      list($day, $month, $year)= explode('.', $this->gosaVacationStart);
      $this->attrs['gosaVacationStart']= mktime(0,0,0,$month, $day, $year);
      list($day, $month, $year)= explode('.', $this->gosaVacationStop);
      $this->attrs['gosaVacationStop']= mktime(0,0,0,$month, $day, $year);
    }

    /* Map method attributes */ 
    $this->mailMethod->fixAttributesOnStore();
    
    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs);

    if (!$ldap->success()){
      msg_dialog::display(_("LDAP error"), msgPool::ldaperror($ldap->get_error(), $this->dn, LDAP_MOD, get_class()));
    }

    /* Log last action */
    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

    /* Only do IMAP actions if we are not a template */
    if (!$this->is_template){
      $this->mailMethod->connect();
      if(!$this->mailMethod->is_connected()){
        msg_dialog::display(_("Mail error"), sprintf(_("Mail method cannot connect: %s"), 
              $this->mailMethod->get_error()), ERROR_DIALOG);
      }else{
        if(!$this->mailMethod->updateMailbox()){
          msg_dialog::display(_("Mail error"), sprintf(_("Cannot update mailbox: %s"), 
                $this->mailMethod->get_error()), ERROR_DIALOG);
        }
        if(!$this->mailMethod->setQuota($this->gosaMailQuota)){
          msg_dialog::display(_("Mail error"), sprintf(_("Cannot write quota settings: %s"), 
                $this->mailMethod->get_error()), ERROR_DIALOG);
        }

        if (!is_integer(strpos($this->gosaMailDeliveryMode, "C"))){

          /* Do not write sieve settings if this account is new and 
             doesn't seem to exist.
           */
          if(!$this->initially_was_account && !$this->mailMethod->account_exists()){
            @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,
                "Skipping sieve settings, the account doesn't seem to be created already.</b>","");
          }else{
            if(!$this->mailMethod->saveSieveSettings()){
              msg_dialog::display(_("Mail error saving sieve settings"), $this->mailMethod->get_error(), ERROR_DIALOG);
            }
          }
        }else{
          if ($this->sieve_management) {
            @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, 
                "User uses an own sieve script, skipping sieve update.".$str."</b>","");
            $this->sieve_management->save();
          }
        }
      }
    }
    $this->mailMethod->disconnect();

    /* Update sharedFolder dependencies.
       Open each shared folder and remove this account.
       Then Save the group to ensure that all necessary
       actions will be taken (imap acls updated aso.).
     */
    if(!$this->initially_was_account){
      $ldap = $this->config->get_ldap_link();
      $ldap->cd($this->config->current['BASE']);
      $ldap->search("(&(objectClass=posixGroup)(objectClass=gosaMailAccount)(memberUid=".$this->uid."))",array("dn"));
      while($attrs = $ldap->fetch()){
        $grp = new grouptabs($this->config, $this->config->data['TABS']['GROUPTABS'], $attrs['dn']);
        if(isset($grp->by_object['mailgroup'])){
          /* Do not save the complete group! This will quit the complete membership
           */
          $grp->by_object['mailgroup']->save();
        }
      }
    }

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify", array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add", array("uid" => $this->uid));
    }
  }


  /*! \brief  Check given values 
   */
  function check()
  {
    if(!$this->is_account){
      return(array());
    }

    $ldap= $this->config->get_ldap_link();

    /* Call common method to give check the hook */
    $message= plugin::check();

    if(empty($this->gosaMailServer)){
      $message[]= msgPool::noserver(_("Mail"));
    }

    /* Mail address checks */
    $mail = $this->mail;
    if(!(!$this->mailMethod->isModifyableMail() && $this->initially_was_account)){

      if($this->mailMethod->domainSelectionEnabled()){
        $mail.= "@".$this->mailDomainPart;
      }

      if (empty($mail)){
        $message[]= msgPool::required(_("Primary address"));
      }
      if ($this->is_template){
        if (!tests::is_email($mail, TRUE)){
          $message[]= msgPool::invalid(_("Mail address"),"","","{%givenName}.{%sn}@your-domain.com");
        }
      } else {
        if (!tests::is_email($mail)){
          $message[]= msgPool::invalid(_("Mail address"),"","","your-address@your-domain.com");
        }
      }

      /* Check if this mail address is already in use */
      $ldap->cd($this->config->current['BASE']);
      $filter = "(&(!(objectClass=gosaUserTemplate))(!(uid=".$this->uid."))".
        "(objectClass=gosaMailAccount)".
        "(|(mail=".$mail.")(alias=".$mail.")(gosaMailAlternateAddress=".$mail.")))";
      $ldap->search($filter,array("uid", "cn"));
      if ($ldap->count() != 0){
        $entry= $ldap->fetch();
        $addon= "";
        if (!isset($entry['uid'])) {
           $addon= sprintf(_("Address is already in use by group '%s'."), $entry['cn'][0]);
        } else {
           $addon= sprintf(_("Address is already in use by user '%s'."), $entry['uid'][0]);
        }
        $message[]= msgPool::duplicated(_("Mail address"))."<br><br><i>$addon</i>";
      }
    }


    /* Check quota */
    if ($this->gosaMailQuota != '' && $this->acl_is_writeable("gosaMailQuota")){
      if (!is_numeric($this->gosaMailQuota)) {
        $message[]= msgPool::invalid(_("Quota size"),$this->gosaMailQuota,"/^[0-9]*/");
      } else {
        $this->gosaMailQuota= (int) $this->gosaMailQuota;
      }
    }

    /* Check rejectsize for integer */
    if ($this->gosaMailMaxSize != '' && $this->acl_is_writeable("gosaMailMaxSize")){
      if (!is_numeric($this->gosaMailMaxSize)){
        $message[]= msgPool::invalid(_("Mail reject size"),$this->gosaMailMaxSize,"/^[0-9]*/");
      } else {
        $this->gosaMailMaxSize= (int) $this->gosaMailMaxSize;
      }
    }

    /* Need gosaMailMaxSize if use_mailsize_limit is checked */
    if (is_integer(strpos($this->gosaMailDeliveryMode, "R")) && $this->gosaMailMaxSize == ""){
      $message[]= msgPool::required(_("Mail reject size"));
    }

    if((preg_match("/S/", $this->gosaMailDeliveryMode))&&(empty($this->gosaSpamMailbox))) {
      $message[]= msgPool::required(_("Spam folder"));
    }

    if ($this->mailMethod->vacationRangeEnabled() && preg_match('/V/', $this->gosaMailDeliveryMode)){ 

      /* Check date strings */
      $state= true;
      if ($this->gosaVacationStart == "" || !tests::is_date($this->gosaVacationStart)) {
        $message[]= msgPool::invalid(_("from"),$this->gosaVacationStart);
        $state= false;
      }
      if ($this->gosaVacationStart == "" || !tests::is_date($this->gosaVacationStop)) {
        $message[]= msgPool::invalid(_("to"),$this->gosaVacationStop);
        $state= false;
      }

      #TODO: take care of date format
      if ($state) {
        list($day, $month, $year)= explode('.', $this->gosaVacationStart);
        $start= mktime(0,0,0,$month, $day, $year);
        list($day, $month, $year)= explode('.', $this->gosaVacationStop);
        $stop= mktime(0,0,0,$month, $day, $year);
        if($start > $stop){
          $message[]= msgPool::invalid(_("Vacation interval"));
        }
      }
    }
    return($message);
  }


  /*! \brief  Adapt from template, using 'dn' 
   */
  function adapt_from_template($dn, $skip= array())
  {
    plugin::adapt_from_template($dn, $skip);

    foreach (array("gosaMailAlternateAddress", "gosaMailForwardingAddress") as $val){

      if (in_array_strict($val, $skip)){
        continue;
      }

      $this->$val= array();
      if (isset($this->attrs["$val"]["count"])){
        for ($i= 0; $i<$this->attrs["$val"]["count"]; $i++){
          $value= $this->attrs["$val"][$i];
          foreach (array("sn", "givenName", "uid") as $repl){
            if (preg_match("/%$repl/i", $value)){
              $value= preg_replace ("/%$repl/i", $this->parent->$repl, $value);
            }
          }
            
          // Remove non ASCII charcters
          $value = iconv('UTF-8', 'US-ASCII//TRANSLIT', $value);
          $value = preg_replace('/[^(\x20-\x7F)]*/','',$value);

          // No spaces are allowed here
          $value = preg_replace("/[ ]/","", $value);
          array_push($this->$val, strtolower(rewrite($value)));
        }
      }
    }
    $this->mail= strtolower(rewrite($this->mail));

    // Remove non ASCII charcters
    $this->mail= iconv('UTF-8', 'US-ASCII//TRANSLIT', $this->mail);
    $this->mail= preg_replace('/[^(\x20-\x7F)]*/','',$this->mail);

    // No spaces are allowed here
    $this->mail = preg_replace("/[ ]/","", $this->mail);

    // Fix mail address when using templates
    if($this->is_account && $this->mailMethod->domainSelectionEnabled()){
      $this->mailDomainPart = preg_replace("/^[^@]*+@/","",$this->mail);
      $this->mail = preg_replace("/@.*$/","\\1",$this->mail);
      if(!in_array_strict($this->mailDomainPart,$this->mailDomainParts)){
        $this->mailDomainParts[] = $this->mailDomainPart;
      }
    }
  }


  /*! \brief  Creates the mail part for the copy & paste dialog 
   */ 
  function getCopyDialog()
  {
      if(!$this->is_account) return("");
      $smarty = get_smarty();
      $smarty->assign("mail",$this->mail);
      $smarty->assign("gosaMailAlternateAddress",$this->gosaMailAlternateAddress);
      $smarty->assign("gosaMailForwardingAddress",$this->gosaMailForwardingAddress);

      $smarty->assign("domainSelectionEnabled", $this->mailMethod->domainSelectionEnabled());
      $smarty->assign("MailDomains", $this->mailDomainParts);
      $smarty->assign("MailDomain" , $this->mailDomainPart);
      $smarty->assign("MailServers", $this->mailMethod->getMailServers());
      $smarty->assign("isModifyableMail"  , $this->mailMethod->isModifyableMail());
      $smarty->assign("initially_was_account", $this->initially_was_account);

    $str = $smarty->fetch(get_template_path("copypaste.tpl",TRUE, dirname(__FILE__)));

      $ret = array();
      $ret['status'] = "";
      $ret['string'] = $str;
      return($ret);
  }


  /*! \brief  save_object for copy&paste vars 
   */  
  function saveCopyDialog()
  {
      if(!$this->is_account) return;

      /* Execute to save mailAlternateAddress && gosaMailForwardingAddress */
      $this->execute();
      if(isset($_POST['mail'])){
          $this->mail = get_post('mail');
      }
  }


  /*! \brief  Prepare this account to be copied 
   */
  function PrepareForCopyPaste($source)
  {
      plugin::PrepareForCopyPaste($source);

      /* Reset alternate mail addresses */
      $this->gosaMailAlternateAddress = array();


      /* If the doamin part is selectable, we have to split the mail address
       */
      if($this->mailMethod->domainSelectionEnabled()){
          $this->mailDomainPart = preg_replace("/^[^@]*+@/","",$this->mail);
          $this->mail = preg_replace("/@.*$/","\\1",$this->mail);
          if(!in_array_strict($this->mailDomainPart,$this->mailDomainParts)){
              $this->mailDomainParts[] = $this->mailDomainPart;
          }
      }

      // Initialize Vacation start/stop times 
      // This value is set to 'dd.mm.YYYY' if no times are set in the source object. 
      // But if they are set, then this values contain timestamps.
      if(isset($source['gosaVacationStart'][0])){
          $this->gosaVacationStart= date('d.m.Y', $source['gosaVacationStart'][0]);
          $this->gosaVacationStop= date('d.m.Y', $source['gosaVacationStop'][0]);
      }
  }


  /*! \brief  Prepare this class the be useable when editing multiple users at once 
   */
  function get_multi_edit_values()
  {
    $ret = plugin::get_multi_edit_values();
    if(in_array_strict("gosaMailQuota",$this->multi_boxes)){
      $ret['gosaMailQuota'] = $this->gosaMailQuota;
    }
    $flag_add = $flag_remove = array();
    $tmp= preg_replace("/[^a-z]/i","",$this->gosaMailDeliveryMode);
    $opts = array(
        "R"   => "use_mailsize_limit",
        "S"   => "use_spam_filter",
        "L"   => "only_local",
        "V"   => "use_vacation",
        "C"   => "own_script",
        "I"   => "drop_own_mails");
    foreach($opts as $flag => $post){
      if(in_array_strict($post, $this->multi_boxes)){
        if(preg_match("/".$flag."/",$tmp)){
          $flag_add[] = $flag;
        }else{
          $flag_remove[] = $flag;
        }
      }
    }
    $ret['flag_add'] = $flag_add;
    $ret['flag_remove'] = $flag_remove;

    if($this->mailMethod->vacationRangeEnabled()){
      if(in_array_strict("V",$flag_add)){
        $ret['gosaVacationStart'] =  $this->gosaVacationStart = get_post('gosaVacationStart');
        $ret['gosaVacationStop'] =  $this->gosaVacationStop = get_post('gosaVacationStop');
      }
    }
    return($ret);
  }


  /*! \brief  Check given input for multiple user edit 
   */
  function multiple_check()
  {
    $message = plugin::multiple_check();

    if(empty($this->gosaMailServer) && in_array_strict("gosaMailServer",$this->multi_boxes)){
      $message[]= msgPool::noserver(_("Mail"));
    }

    /* Check quota */
    if ($this->gosaMailQuota != ''  && in_array_strict("gosaMailQuota",$this->multi_boxes)){
      if (!is_numeric($this->gosaMailQuota)) {
        $message[]= msgPool::invalid(_("Quota size"),$this->gosaMailQuota,"/^[0-9]*/");
      } else {
        $this->gosaMailQuota= (int) $this->gosaMailQuota;
      }
    }

    /* Check rejectsize for integer */
    if ($this->gosaMailMaxSize != '' && in_array_strict("gosaMailMaxSize",$this->multi_boxes)){
      if (!is_numeric($this->gosaMailMaxSize)){
        $message[]= msgPool::invalid(_("Mail reject size"),$this->gosaMailMaxSize,"/^[0-9]*/");
      } else {
        $this->gosaMailMaxSize= (int) $this->gosaMailMaxSize;
      }
    }

    if(empty($this->gosaSpamMailbox) && in_array_strict("gosaSpamMailbox",$this->multi_boxes)){
      $message[]= msgPool::required(_("Spam folder"));
    }

    if ($this->mailMethod->vacationRangeEnabled() && preg_match('/V/', $this->gosaMailDeliveryMode)){ 

      /* Check date strings */
      $state= true;
      if ($this->gosaVacationStart == "" || !tests::is_date($this->gosaVacationStart)) {
        $message[]= msgPool::invalid(_("from"),$this->gosaVacationStart);
        $state= false;
      }
      if ($this->gosaVacationStart == "" || !tests::is_date($this->gosaVacationStop)) {
        $message[]= msgPool::invalid(_("to"),$this->gosaVacationStop);
        $state= false;
      }

      #TODO: take care of date format
      if ($state) {
        list($day, $month, $year)= explode('.', $this->gosaVacationStart);
        $start= mktime(0,0,0,$month, $day, $year);
        list($day, $month, $year)= explode('.', $this->gosaVacationStop);
        $stop= mktime(0,0,0,$month, $day, $year);
        if($start > $stop){
          $message[]= msgPool::invalid(_("Vacation interval"));
        }
      }
    }
    return($message);
  }

  
  /*! \brief  ...
   */
  function set_multi_edit_values($values)
  {
    plugin::set_multi_edit_values($values);
    $tmp= preg_replace("/[^a-z]/i","",$this->gosaMailDeliveryMode);
    if(isset($values['flag_add'])){
      foreach($values['flag_add'] as $flag){
        if(!preg_match("/".$flag."/",$tmp)){
          $tmp .= $flag;
        }
      }
    }
    if(isset($values['flag_remove'])){
      foreach($values['flag_remove'] as $flag){
        if(preg_match("/".$flag."/",$tmp)){
          $tmp = preg_replace("/".$flag."/","",$tmp);
        }
      }
    }
    $this->gosaMailDeliveryMode = "[".$tmp."]";

    /* Set vacation message and replace placeholder like %givenName
     */
    if(isset($values['gosaVacationMessage'])){
      $this->gosaVacationMessage = $this->prepare_vacation_template($values['gosaVacationMessage']);
    }
  }


  /*! \brief  Initialize plugin to be used as multiple edit class. 
   */
  function init_multiple_support($attrs,$all)
  {
    plugin::init_multiple_support($attrs,$all);
    if(isset($this->multi_attrs['gosaMailQuota'])){
      $this->gosaMailQuota = $this->multi_attrs['gosaMailQuota'];
    }
  }

 
  /*! \brief
   */
  function get_multi_init_values()
  {
    $attrs = plugin::get_multi_init_values();
    $attrs['gosaMailQuota'] = $this->gosaMailQuota;
    return($attrs);
  }


  /*! \brief  Display multiple edit dialog 
   */
  function multiple_execute()
  {
    return($this->execute());
  }

  
  /*! \brief  Save posts from multiple edit dialog 
   */
  function multiple_save_object()
  {
    plugin::multiple_save_object();

    $this->save_object();
    foreach(array("only_local","gosaMailForwardingAddress","use_mailsize_limit","drop_own_mails","use_vacation","use_spam_filter") as $attr){
      if(isset($_POST["use_".$attr])){
        $this->multi_boxes[] = $attr;
      }
    }
  }


  /*! \brief  Creates the user names for the add_local_forward dialog
   */
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


  function allow_remove()
  {
    $resason = "";
    if(!$this->mailMethod->allow_remove($reason)){
      return($reason);
    }
    return("");
  }



  /*! \brief  ACL settings 
   */
  static function plInfo()
  {
      return (array(
                  "plShortName"     => _("Mail"),
                  "plDescription"   => _("Mail settings"),
                  "plSelfModify"    => TRUE,
                  "plDepends"       => array("user"),                     // This plugin depends on
                  "plPriority"      => 4,                                 // Position in tabs
                  "plSection"     => array("personal" => _("My account")),
                  "plCategory"    => array("users"),
                  "plOptions"       => array(),

                  "plRequirements"=> array(
                      'ldapSchema' => array('gosaMailAccount' => '>=2.7'),
                      'onFailureDisablePlugin' => array(get_class())
                      ),
    
                  "plProperties"  =>
                  array(
                      array(
                          "name"          => "cyrusDeleteMailbox",
                          "type"          => "bool",
                          "default"       => 'true',
                          "description"   => _("Remove mail boxes from the IMAP storage after they their user gets removed."),
                          "check"         => "gosaProperty::isBool",
                          "migrate"       => "",
                          "group"         => "mail",
                          "mandatory"     => FALSE
                          ),
                      array(
                          "name"          => "cyrusAutocreateFolders",
                          "type"          => "string",
                          "default"       => "",
                          "description"   => _("Comma separated list of folders to be automatically created on user creation."),
                          "check"         => "gosaProperty::isString",
                          "migrate"       => "",
                          "group"         => "mail",
                          "mandatory"     => FALSE
                          )
                          ),


                      "plProvidedAcls"  => array(
                              "mail"                      =>  _("Mail address"),
                              "gosaMailServer"            =>  _("Mail server"),
                              "gosaMailQuota"             =>  _("Quota size"),

                              "gosaMailDeliveryModeV"     =>  _("Add vacation information"),  // This is flag of gosaMailDeliveryMode
                              "gosaVacationMessage"       =>  _("Vacation message"),

                              "gosaMailDeliveryModeS"     =>  _("Use SPAM filter"),           // This is flag of gosaMailDeliveryMode
                              "gosaSpamSortLevel"         =>  _("SPAM level"),
                              "gosaSpamMailbox"           =>  _("SPAM mail box"),

                              "sieveManagement"           =>  _("Sieve management"),

                              "gosaMailDeliveryModeR"     =>  _("Reject due to mail size"),    // This is flag of gosaMailDeliveryMode
                              "gosaMailMaxSize"           =>  _("Mail max size"),

                              "gosaMailForwardingAddress" =>  _("Forwarding address"),
                              "gosaMailDeliveryModeL"     =>  _("Local delivery"),            // This is flag of gosaMailDeliveryMode
                              "gosaMailDeliveryModeI"     =>  _("No delivery to own mailbox "),     // This is flag of gosaMailDeliveryMode
                              "gosaMailAlternateAddress"  =>  _("Mail alternative addresses"),

                              "gosaMailForwardingAddress" =>  _("Forwarding address"),
                              "gosaMailDeliveryModeC"     =>  _("Use custom sieve script"))   // This is flag of gosaMailDeliveryMode
                                  ));
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
