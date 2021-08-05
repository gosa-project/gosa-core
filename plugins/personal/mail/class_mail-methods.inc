<?php


class mailMethod{

  /* Allow modification of account_ids for existing mail accounts */
  protected $modifyableMail   = TRUE;

  /* Allow modification of the mail server attribute existing mail accounts */
  protected $modifyableServer = TRUE;

  /* Enforces same value for 'mail' as used for 'cn' */
  protected $mailEqualsCN   = FALSE; 

  /* the attribute used to create accounts */ 
  protected $uattrib        = "mail";  // Naming attribute for accounts, e.g. imap.

  /* The account prefixes, keep the '.' here! See FAQ cyrusUseSlashes */
  protected $user_prefix    = "user.";  
  protected $share_prefix   = "share.";

  /* Account ID creation 

      !!Depends on the attributes 'user_prefix'/'share_prefix' and the option 'cyrusUseSlashes'

      Examples - based on defaults : 
        %PREFIX%    => "user." or "user/" (Depending on cyrusUseSlashes=FALSE/TRUE) 
        %CN%        => "technik"          (The groups cn)
        %UID%       => "herbert"          (The users uid)
        %MAIL%      => "herbert@domain.de"(The mail address)
        %DOMAIN%    => "domain.de"        (The domain part of the specified mail)
        %MAILPART%  => "herbert"          (The mail address without domain)
        %UATTRIB%   => "herbert"/"herbert@domains.de"
                                            (Configured in gosa.conf mailAttribute="mail"/"uid")
   */
  protected $user_id    = "%PREFIX%%UATTRIB%";  
  protected $share_id   = "%PREFIX%%UATTRIB%";

  /* Create accounts in cyrus style with '/' instead of '.' */
  protected $cyrusUseSlashes= FALSE;

  /* gosaSharedFolderTarget settings, 
   *  E.g. 
   *    For an accountID like: 'share/herberts.folder@gonicus.de' the value 'dummy+' 
   *    will result in gosaSharedFolderTarget: dummy+share/herberts.folder@gonicus.de
   */
  protected $gosaSharedPrefix = '';  

  /* The atribute mapping for this class  Source --> Destination */
  protected $attributes     = array();
  protected $userObjectClasses = array();
  protected $shareObjectClasses = array();

  /* Enabled mail domain selection. If enabled getMailDomains() have to return an array 
   *  with the domain parts.
   */ 
  protected $enableDomainSelection= FALSE;
  protected $enableQuota          = TRUE;
  protected $enableSieveManager   = FALSE;
  protected $enableVacationRange  = TRUE;
  protected $enableFolderTypes    = FALSE;

  /* Default values */
  protected $quotaValue   = 0;  
  protected $quotaUsage   = 0;  

  /* Method internal */
  protected $type               = "user"; 
  protected $account_id         = "";
  protected $initial_account_id = "";
  protected $connected          = FALSE;
  protected $error              = "";
  protected $parent             = NULL;   
  protected $MailServer         = "";

  protected $default_acls = array("__anyone__" => "p", "__member__" => "lrswp");

  protected $acl_map = array(
      "lrsw"     => "read",
      "lrswp"    => "post",
      "p"        => "external post",
      "lrswip"   => "append",
      "lrswipd"  => "delete",
      "lrswipcd" => "write",
      "lrswipcda"=> "admin",
      " "         => "none");

  protected $acl_mapping = array();


  /*! \brief  Constructs the mail class 
      @param  Object  Config  The GOsa configuration object
      @param  Object  Plugin  The initator
      @param  String          Open "user" or "group" account.
   */
  function __construct(&$config, &$parent, $type = "user")
  {
    $this->parent = $parent;
    $this->config = $config;

    /* Create a refernce to the mail selected server 
     */
    if(isset($this->parent->gosaMailServer)){
      $this->MailServer = &$this->parent->gosaMailServer;
    }else{
      trigger_error("mailMethod with invalid parent object initialized.");
    }

    if(!in_array_strict($this->type,array("user","group"))){
      trigger_error("Unknown mail class type used '".$type."'.");
    }else{
      $this->type = $type;
    }

  }

  
  /*! \brief  Intialize attributes and config settings.
   */
  protected function init()
  {
    /* Get config value for cyrusUseSlashes
     */
    if ($this->config->get_cfg_value("core","cyrusUseSlashes") == "true"){
      $this->cyrusUseSlashes = TRUE;
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "","<b>MAIL:</b> cyrusUseSlashes: <b>Enabled</b>");
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "","<b>MAIL:</b> cyrusUseSlashes: <b>Disabled</b>");
    }

    /* Check if the mail account identification attribute
       is overridden in the configuration file
     */
    if($this->config->get_cfg_value("core","mailAttribute") != ""){
      $new_uattrib= strtolower($this->config->get_cfg_value("core","mailAttribute"));
      if(in_array_strict($new_uattrib,array("mail","uid"))){
        $this->uattrib = $new_uattrib;
      }else{
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<b>".$new_uattrib."</b>",
          "<b>MAIL:</b> Unsupported 'mailAttribute' in gosa configuration specified");
        msg_dialog::display(_("Configuration error"), 
            sprintf(_("The configured mail attribute '%s' is unsupported!"), $new_uattrib), ERROR_DIALOG);
      }
    }

    /* Create ACL map */
    foreach($this->acl_map as $acl => $name){
      $this->acl_mapping[$acl] = _($name);
    }

    /* Check if we have an individual user/folder creation syntax 
     */
    $tmp = $this->config->get_cfg_value("core","mailUserCreation");
    if(!empty($tmp)){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<i>".$tmp."</i>",
          "<b>MAIL:</b> User creation set to");
      $this->user_id  = $tmp;
    }
    $tmp = $this->config->get_cfg_value("core","mailFolderCreation");
    if(!empty($tmp)){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<i>".$tmp."</i>",
          "<b>MAIL:</b> Shared folder creation set to");
      $this->share_id = $tmp;
    }
    $tmp = $this->config->get_cfg_value("core","gosaSharedPrefix");
    if(!empty($tmp)){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<i>".$tmp."</i>",
          "<b>MAIL:</b> Shared folder prefix set to");
      $this->gosaSharedPrefix = $tmp;
    }

    $this->build_account_id();
    $this->initial_account_id = $this->account_id;
  }

  
  public function fixAttributesOnLoad()
  {
    foreach($this->attributes as $source => $dest){
      if(isset($this->parent->attrs[$source])){
        $this->parent->attrs[$dest] = $this->parent->attrs[$source];
      }
      if(isset($this->parent->$source)){
        $this->parent->$dest = $this->parent->$source;
      }
      if(isset($this->parent->attrs[$source][0])){
        $this->parent->saved_attributes[$source] = $this->parent->attrs[$source][0];
      }
    }
  }


  public function mailEqualsCN()
  {
    return($this->mailEqualsCN);
  }
  

  public function fixAttributesOnRemove()
  {
    /* Remove objectClasses  
     */ 
    if($this->type == "user"){
      $this->parent->attrs['objectClass'] = 
        array_remove_entries_ics($this->userObjectClasses, $this->parent->attrs['objectClass']);
    }else{
      $this->parent->attrs['objectClass'] = 
        array_remove_entries_ics($this->shareObjectClasses, $this->parent->attrs['objectClass']);
      $this->parent->attrs['gosaSharedFolderTarget'] =array();
    }
    foreach($this->attributes as $source => $dest){
      $this->attrs[$dest]   = array();
      $this->attrs[$source] = array();
    }
  }

  public function fixAttributesOnStore()
  {
    foreach($this->attributes as $source => $dest){
      if(isset($this->parent->attrs[$dest])){
        $this->parent->attrs[$source] = $this->parent->attrs[$dest ];
      }
      if(isset($this->parent->$dest)){
        $this->parent->$source = $this->parent->$dest;
      }
    }

    if($this->type == "user"){
      $ocs = $this->userObjectClasses;
    }else{
      $ocs = $this->shareObjectClasses;
    }
    foreach($ocs as $oc){
      if(!in_array_strict($oc, $this->parent->attrs['objectClass'])){
        $this->parent->attrs['objectClass'][] = $oc;
      }
    }
  
    // Add gosaSharedFolderTarget for groups.
    $this->build_account_id();
    if($this->type == "group"){ 
      $this->parent->attrs['gosaSharedFolderTarget'] = 
        $this->gosaSharedPrefix.$this->account_id;
    }
  }


  /*! \brief  Connect services like imap.
              Not necessary for the base class.
      @return Boolean True if this method is connected else false.
   */
  public function connect()
  {
    $this->reset_error();
    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"","<b>MAIL: Connect method</b>: ".get_class($this));
    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"","<b>MAIL: Current server</b>: ".$this->MailServer);
 
    $this->connected = TRUE;
    return(TRUE);
  }


  /*! \brief  Returns the connection status of this method.
      @return Boolean True if this method is connected else false.
   */
  public function is_connected()
  {
    return($this->connected);
  }


  /*! \brief  Disconnect this method. Close services like imap connection.
              Not necessary for the base class.
   */
  public function disconnect()
  {
    $this->reset_error();
    if($this->is_connected()){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"","<b>MAIL: Disconnect method</b>: ".get_class($this));
      $this->connected =FALSE; 
    }
  }

  
  /*! \brief  Returns true the current object represents a valid account
              (Some methods may check imap accounts here.)
      @return Boolean TRUE if this is a valid account else FALSE
  */
  public function account_exists()
  {
    $this->reset_error();
    return(TRUE);
  }
  

  /*! \brief  Returns the last error occurred 
      @return String  The last error message.
   */
  public function get_error(){
    return($this->error);
  }


  public function isModifyableMail()
  {
    return($this->modifyableMail);
  }


  public function isModifyableServer()
  {
    return($this->modifyableServer);
  }


  /*! \brief  Returns TRUE if the action caused an error.
      @return Boolean TRUE on error else FALSE
   */
  public function is_error(){
    return($this->error != "");
  }


  /*! \brief  Resets the error message.
   */
  public function reset_error(){
    $this->error = "";
  }


  public function get_account_id()
  {
    $this->build_account_id();
    return($this->account_id);
  }

  /*! \brief  Create a new account id, like 'user/name@domain.com'.
   */
  protected function build_account_id()
  {
    /* Build account identicator */
    if($this->type == "user"){
      $prefix = $this->user_prefix;
      $acc_string = $this->user_id;
    }else{
      $prefix = $this->share_prefix;
      $acc_string = $this->share_id;
    }

    /* Create account prefix and respect "cyrusUseSlashes" 
       Do not replace escaped dots for cyrusUseSlashes.
     */
    $uattrib = $this->uattrib;
    if($this->cyrusUseSlashes){
      $prefix     = preg_replace('/([^\\\\])\./',"\\1/",$prefix);
      $acc_string = preg_replace('/([^\\\\])\./',"\\1/",$acc_string); 
    }
    $prefix = preg_replace("/\\\\([\.\/])/","\\1",$prefix);
    $acc_string = preg_replace("/\\\\([\.\/])/","\\1",$acc_string); 
  
    $domain = $mailpart = "";
    $mail = $this->parent->mail;
    if(preg_match("/\@/",$mail)){
      list($mailpart,$domain) = explode("@",$mail);
    }

    /* Create account_id 
     */ 
    $from = array("/%cn%/i","/%uid%/i","/%prefix%/i","/%uattrib%/i","/%domain%/i","/%mailpart%/i","/%mail%/i");
    $to   = array($this->parent->cn,$this->parent->uid,$prefix,$this->parent->$uattrib, $domain, $mailpart,$mail);
    $acc_id = trim(strtolower(preg_replace($from,$to,$acc_string)));

    /* Check for not replaced pattern.
     */
    if(preg_match("/%/",$acc_id)){
      $notr = preg_replace("/^[^%]*/","",$acc_id);
      if(!empty($notr)){
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"<b>Warning</b>", 
            sprintf("<b>MAIL: WARNING unknown pattern in account creation string '%s' near '%s'</b>", $acc_id, $notr));

        /* Remove incomprehensible patterns */
        $acc_id = preg_replace("/%[^%]+%/","",$acc_id);
      }
    }


    if($this->account_id != $acc_id){
      $this->account_id = $acc_id;
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"", "<b>MAIL:</b> AccountID generated: <b>".$acc_id."</b>");
    }
  }


  /*! \brief  Creates a valid folder id for a given folder name.
               e.g. $folder_id = "INBOX/test"  &&  $this->account_id = "share/mailbox@domain.de"
               will result in "share/mailbox/test@domain.de"
              This function is mainly used to read and write folder permissions.
      @return String A valid folder id
   */
  public function create_folder_id($folder, $type = "")
  {

    if(!empty($folder)){
      $folder = trim(preg_replace("/^INBOX[\.\/]*/i","",$folder));
    }
    if(!empty($folder)){
      $folder = "/".$folder;
    }

    /* Build account identicator */
    if($this->type == "user"){
      $prefix = $this->user_prefix;
      $acc_string = $this->user_id;
    }else{
      $prefix = $this->share_prefix;
      $acc_string = $this->share_id;
    }

    /* Create account prefix and respect "cyrusUseSlashes" 
       Do not replace escaped dots for cyrusUseSlashes.
     */
    $uattrib = $this->uattrib;
    if($this->cyrusUseSlashes){
      $prefix     = preg_replace('/([^\\\\])\./',"\\1/",$prefix);
      $acc_string = preg_replace('/([^\\\\])\./',"\\1/",$acc_string); 
    }
    $prefix = preg_replace("/\\\\([\.\/])/","\\1",$prefix);
    $acc_string = preg_replace("/\\\\([\.\/])/","\\1",$acc_string); 
  
    $domain = $mailpart = "";
    $mail = $this->parent->mail;
    if(preg_match("/\@/",$mail)){
      list($mailpart,$domain) = explode("@",$mail);
    }

    /* Create account_id
     */
    $from = array("/%cn%/i","/%uid%/i","/%prefix%/i","/%uattrib%/i","/%domain%/i","/%mailpart%/i","/%mail%/i");
    $to   = array($this->parent->cn,$this->parent->uid,$prefix,$this->parent->$uattrib, $domain, $mailpart,$mail);
    $acc_id = trim(strtolower(preg_replace($from,$to,$acc_string)));

    /* Check for not replaced pattern.
     */
    if(preg_match("/%/",$acc_id)){
      $notr = preg_replace("/^[^%]*/","",$acc_id);
      if(!empty($notr)){
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"<b>Warning</b>", 
            sprintf("<b>MAIL: WARNING unknown pattern in account creation string '%s' near '%s'</b>", $acc_id, $notr));

        /* Remove incomprehensible patterns */
        $acc_id = preg_replace("/%[^%]+%/","",$acc_id);
      }
    }

    if(preg_match("/\@/",$acc_id)){
      list($mail,$domain) = explode("@",$acc_id);
      $str = trim($mail . $folder . "@" . $domain);
    }else{
      $str = trim($acc_id . $folder);
    }
    return($str) ;
  }


  /*! \brief  Returns the configured mail method for the given parent object, 
                initialized and read for use.
      @return mailMethod  The configured mailMethod.
   */
  public function get_method()
  {
    $methods = mailMethod::get_methods();
    if ($this->config->get_cfg_value("core","mailMethod") != ""){
      $method= $this->config->get_cfg_value("core","mailMethod");
      $cls = get_correct_class_name("mailMethod$method");
      if(isset($methods[$cls])){
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"", "<b>MAIL:</b> Selected mailMethod: <b>".$cls."</b>");
        $tmp = new $cls($this->config,$this->parent,$this->type);
        $tmp->init();
        return($tmp);
      }else{
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "","<b>MAIL: Invalid mailMethod defined: ".$cls.
            " falling back to ".get_class($this)."</b>");

        /* Print out configuration errors directly, we can't catch them everywhere. 
         */
        msg_dialog::display(_("Configuration error"), 
            sprintf(_("Mail method '%s' is unknown!"), $method), ERROR_DIALOG);
      }
    }

    /* If no valued mailMethod could be detected, return the base class.
     */
    $this->init();
    return($this);
  }


  /*! \brief Saves sieve settings 
   */
  public function saveSieveSettings()
  {
    $this->reset_error();
    return(TRUE);
  }

  
  /*! \brief  Creates or Updates the mailAccount represented by this class. 
   */
  public function updateMailbox()
  {
    $this->reset_error();
    return(TRUE);
  }


  /*! \brief  Update shared folder dependencies 
   */
  public function updateSharedFolder()
  {
    $this->reset_error();
    return(TRUE);
  }


  /*! \brief  Removes the mailbox represented by this class,
               and update shared folder ACLs .
   */
  public function deleteMailbox()
  {
    $this->reset_error();

    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, "<b>".$this->account_id."</b>" ,
        "<b>MAIL: Remove account</b> from server :".$this->MailServer);

    return(TRUE);
  }

  
  /*! \brief  Returns the used mail attribute (mail,uid)
      @param  String  One out of 'mail','uid'
   */
  public function getUAttrib()
  {
    return($this->uattrib);
  }


  /*! \brief  Returns the used mail attribute (mail,uid)
      @param  String  One out of 'mail','uid'
   */
  public function getUAttribValue()
  {
    $uattrib = $this->getUAttrib();
    return($this->parent->$uattrib);
  }

  
  /*! \brief  Returns whether the quota settings are enabled or not 
      @return Boolean TRUE if enabled else FALSE
   */
  public function quotaEnabled()
  {
    return($this->enableQuota);
  }

  
  /*! \brief  Returns the used quota 
      @return Integer Quota used.
   */
  public function getQuotaUsage()
  {
    return(-1);
  }


  /*! \brief  Returns the quota restrictions.
      @return Integer Quota restrictions.
   */
  public function getQuota($quotaValue)
  {
    return($quotaValue);
  }

  
  /*! \brief  Sets the mail quota
   */
  public function setQuota($number)
  {
    if(!is_numeric($number)){
      $number = (int) $number;
      if(!$number){
        $number = 0;
      }
    }
    $this->quotaValue = $number; 
    return(TRUE);
  }


  /*! \brief  Returns true whether the domain is selectable or not 
   */
  public function domainSelectionEnabled()
  {
    return($this->enableDomainSelection);
  } 


  /*! \brief Returns a list of configured mail domains 
      @return Array A list of mail domains
   */
  public function getMailDomains()
  {
    return(array("Unconfigured"));
  } 

  
  /*! \brief  Returns the used Spamlevels for this mailmethod 
   */
  public function getSpamLevels()
  {
    $spamlevel= array();
    for ($i= 0; $i<21; $i++){
      $spamlevel[]= $i;
    }
    return($spamlevel);
  }

  
  /*! \brief  Returns the list of configured mailbox folders
      @return Array The mailbox folders.
   */
  public function getMailboxList()
  {
    return(array("INBOX"));
  }


  /*! \brief  Returns whether the vacation range is selectable or not
      @return Boolean TRUE, FALSE
   */
  public function vacationRangeEnabled()
  {
    return($this->enableVacationRange);
  }

  
  /*! \brief  Returns true if the sieveManagement is allowed
      @return Boolean TRUE, FALSE
   */
  public function allowSieveManagement()
  {
    return($this->enableSieveManager);
  } 


  /*! \brief  Checks dependencies to other GOsa plugins.
   */
  public function accountCreateable(&$reason = ""){
    return(TRUE);
  }


  /*! \brief  Checks whether this account is removeable or not.
              There may be some dependencies left, eg. kolab.
   */
  public function accountRemoveable(&$reason = ""){
    return(TRUE);
  }

  
  /*! \brief  Returns all mail servers configured in GOsa 
               that are useable with this mailMethod.
      @return Array  All useable mail servers.
  */
  public function getMailServers()
  {
    $mailserver = array();
    $ui = get_userinfo();
    foreach ($this->config->data['SERVERS']['IMAP'] as $key => $val){
      if( $this->MailServer == $key ||
          preg_match("/r/",$ui->get_permissions($val['server_dn'],"server/goImapServer",""))){
        $mailserver[]= $key;
      }
    }
    return($mailserver);
  }

  
  /*! \brief  Returns the available mailMethods
      @return Array   A list of all avaialable mailMethods_
   */ 
  static function get_methods()
  {
    global $class_mapping;
    $available = array();
    foreach($class_mapping as $class => $path){
      if($class == "mailMethod") continue;
      if(preg_match("/^mailMethod/",$class)){
        $available[$class] = $class;
      }
    }
    return($available);
  }

  
  /* \brief   Some method require special folder types, "kolab" for example.
      !! Those values are dummy values, the base class doesn't use folder types;
     @return  Array Return folder types.
   */
  public function getAvailableFolderTypes()
  {
    $ret = array();
    $ret['CAT'][''] = _("None"); 
    $ret['SUB_CAT'][''][''] = _("None");
    return($ret);
  }


  /* \brief  Returns the selected folder type. 
      !! Those values are dummy values, the base class doesn't use folder types;
     @return  Array  The folde type.
   */
  public function getFolderType($default)
  {
    return($default);
  }

 
  /* \brief  Returns the selected folder type. 
      !! Those values are dummy values, the base class doesn't use folder types;
     @return  Array  The folde type.
   */
  public function setFolderType($type)
  {
    return(TRUE) ;   
  }


  /*! \brief  Returns configured acls 
   */
  public function  getFolderACLs($folder_acls)
  {
    /* Merge given ACL with acl mapping 
       This ensures that no ACL will accidentally overwritten by gosa.
     */
    foreach($folder_acls as $user => $acl){
      if(!isset($this->acl_mapping[$acl])){
        $this->acl_mapping[$acl] = $acl;
      }
    }

    return($folder_acls);
  }


  /*! \brief  Write ACLs back to imap or what ever 
   */
  public function  setFolderACLs($array)
  {
    return(TRUE);
  }


  /*! \brief  Returns a list of all possible acls.
      @return Array   ACLs.
  */
  public function getAclTypes()
  {
    return( $this->acl_mapping);
  }

  public function folderTypesEnabled()
  {
    return($this->enableFolderTypes);
  }

  public function allow_remove(&$reason)
  {
    return(TRUE);
  }


  /*! \brief  Returns the configured mailMethod
      @return String  the configured mail method or ""
   */
  static public function get_current_method($config)
  {
    global $class_mapping;
    $method= $config->get_cfg_value("core","mailMethod");
    $cls = get_correct_class_name("mailMethod$method");
    foreach($class_mapping as $class => $path){
      if($class == $cls){
        return($class);
      }
    }
    return("");
  }


  static function quota_to_image($use,$quota)
  {
    if($use == -1){
      return("&nbsp;&nbsp;"._("Unknown"));
    }elseif(empty($quota)){
      return("&nbsp;&nbsp;"._("Unlimited"));
    }else{
      $usage =round(($use/$quota) * 100);
      return(progressbar($usage,100,15,true));
    }
  }

 
  /*! \brief  Returns the default sharedFolder ACLs for this method.
      @return Array Returns an array containg acls for __member__ and __anyone__
   */ 
  public function getDefaultACLs()
  {
    $tmp = $this->default_acls;
    if(!isset($tmp['__member__'])) $tmp['__member__'] = " ";
    if(!isset($tmp['__anyone__'])) $tmp['__anyone__'] = " ";
    return($tmp);
  }
}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
