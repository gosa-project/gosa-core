<?php

class servkolab extends plugin {
  /* CLI vars */
  var $cli_summary = "Manage server basic objects";
  var $cli_description = "Some longer text\nfor help";
  var $cli_parameters = array("eins"=>"Eins ist toll", "zwei"=>"Zwei ist noch besser");

  var $postfix_mydomain                 = "";
  var $postfix_mydestination            = "";
  var $postfix_mynetworks               = "127.0.0.1/8";
  var $postfix_enable_virus_scan        = "TRUE";
  var $postfix_relayhost                = "";
  var $postfix_mxrelayenabled           =  true;
  var $postfix_allow_unauthenticated     = "FALSE";
  var $cyrus_quotawarn                  = "80";
  var $kolabFreeBusyFuture              = "1";
  var $k                                = "kolab";
  var $cyrus_admins                     = "TRUE";
  var $cyrus_imap                       = "TRUE";
  var $cyrus_pop3                       = "TRUE";
  var $cyrus_imaps                      = "TRUE";
  var $cyrus_pop3s                      = "TRUE";
  var $cyrus_sieve                      = "TRUE";
  var $apache_allow_unauthenticated_fb  = "TRUE";
  var $proftpd_ftp                      = "TRUE";
  var $apache_http                      = "TRUE";
  var $kolabHost                        = array();
  var $is_in_there                      = false;
  var $exactName                        = false;
  var $attributes =  array("postfix_mydomain", "postfix_mydestination", "proftpd_ftp", "k",
      "postfix_mynetworks", "postfix_enable_virus_scan", "postfix_relayhost", "apache_http",
      "postfix_allow_unauthenticated", "cyrus_admins", "cyrus_imap","kolabFreeBusyFuture",
      "cyrus_pop3", "cyrus_imaps", "cyrus_pop3s", "cyrus_sieve", "apache_allow_unauthenticated_fb",
      "cyrus_quotawarn");
  var $objectclasses = array("top", "kolab");

  function servkolab($config, $dn = NULL) 
  {
    /* Setting the hostname and tell this Plugin that we are the kolab extension*/
    $this->hostname = preg_replace('/^cn=([^,]+),.*$/', '\1', $dn);
    $this->dn       = "k=kolab,".$config->current['BASE'];

    /* Load variables, if given*/
    plugin::plugin($config, $this->dn);

    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }

    /* Toggle relayhost */
    $this->postfix_mxrelayenabled = preg_match('/^\[/', $this->postfix_relayhost);
    $this->postfix_relayhost      = preg_replace("/[\[\]]/","",$this->postfix_relayhost);

    /* Is this Server a member of the Kolab extension or not ?*/ 
    if(isset($this->attrs['kolabHost'])) {
      $this->kolabHost= $this->attrs['kolabHost'];
      unset($this->kolabHost['count']);
    }
    $this->is_in_there           = false;
    $this->is_account            = false;
    $this->initially_was_account = false;
    foreach($this->kolabHost as $host){
      if(preg_match("/".$this->hostname.".*/i",$host)){
        $this->is_account            = true;
        $this->is_in_there           = true;
        $this->initially_was_account = true;
      }
    } 


    /* Mhh */ 
    if(in_array($this->hostname, $this->kolabHost)) {
      $this->exactName=true;
    } else {   
      $this->exactName=false;    
    } 

    /* Parse mynetworks */
    if(isset($this->attrs['postfix-mynetworks'])){
      if(is_array($this->attrs['postfix-mynetworks'])){  
        unset($this->attrs['postfix-mynetworks']['count']);
        $tmp="";
        foreach($this->attrs['postfix-mynetworks'] as $tm){
          $tmp.=$tm.";";
        }
        $this->postfix_mynetworks = $tmp;
      }
    }else{
      $this->postfix_mynetworks="";
    }
  }


  function execute() 
  {
    /* Call parent execute */
    plugin::execute();

    /* Fill templating stuff */
    $smarty = get_smarty();
    $display = "";

    /* The Ldap link is needed to ask ldap some questions */
    $ldap = $this->config->get_ldap_link();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])) {
      $this->is_account = !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->is_account) {
      /* call Add Acoount to add account */
      $display = $this->show_header(_("Remove Kolab extension"), _("This server has kolab features enabled. You can disable them by clicking below."));
    } else {
      /* call remove Account */
      $display = $this->show_header(_("Add Kolab service"), _("This server has kolab features disabled. You can enable them by clicking below."));
      return ($display);
    }

    /* Set relayhost and if we have MX lookup enabled*/
    if($this->postfix_mxrelayenabled) {
      $smarty->assign("RelayMxSupportCheck"," checked ");
    } else {
      $smarty->assign("RelayMxSupportCheck","");
    }

    /* Initialize all attributes, that were submitted */
    foreach($this->attributes as $val) 
    {
      $smarty->assign($val."ACL",chkacl($this->acl,str_replace("_","-",$val)));

      /* Tell smarty which variables we are useing */
      $smarty->assign($val, $this->$val);
      if (($this->$val != "FALSE") && !empty($this->$val)){
        $smarty->assign($val."Check", "checked");
      } else {
        $smarty->assign($val."Check", "");
      }
    }

    /* Assemble free/busy string */
    $edit= sprintf('<input name="kolabFreeBusyFuture" value="%s" %s type="text" maxlength="3" size="4">',
        $this->kolabFreeBusyFuture, chkacl($this->acl, 'kolabFreeBusyFuture'));
    $fbfuture= sprintf(_("Include data from %s days in the past when creating free/busy lists"), $edit);
    $smarty->assign("fbfuture", $fbfuture);

    /* Assemble quota string */
    $edit= sprintf('<input name="cyrus_quotawarn" value="%s" type="text" maxlength="3" size="4" %s>',
        $this->cyrus_quotawarn, chkacl($this->acl, 'cyrus_quotawarn'));
    $quotastr= sprintf(_("Warn users when using more than %s%% of their mail quota"), $edit);
    $smarty->assign("quotastr", $quotastr);

    /* Load Template */
    $display.=$smarty->fetch(get_template_path('servkolab.tpl', TRUE));
    return ($display);
  }

  function remove_from_parent() 
  {
    /* Only walk through following code, if this host 
       was a member of the kolab hosts, else skip this */
    if(!$this->initially_was_account){
      return;
    }

    $ldap     = $this->config->get_ldap_link();
    $this->dn = "k=kolab,".$this->config->current['BASE'];


    /* We can't simply remove the whole entry, it is possible that there are 
       some other hosts assigned to this object. 
       So, first of all check if we are the last host entry within the host 
       : Remove k=kolab entry 
       if we aren't alone, only remove host name from hosts entry and save */
    /* Are we alone? Remove complete entry... */

    /* Check if we are definitly in kolabHosts */
    if(!in_array_ics($this->cn,$this->kolabHost)) {
      return;
    }

    /* Integration check, not translatet because they can't pop up at all, only for debug */ 
    if(count($this->kolabHost) == 0){
      print_red("Server - Kolab tab : This is not possible, we can't remove an account which doesn't exists.");
      return;
    }
    if(!isset($this->cn) || (empty($this->cn))){
      print_red("The required attribute cn seams to empty.");
    }

    /* Our hostname is in kolabHosts and there is a only one entry 
       = we are the last host entry, delete k=kolab entry  */
    if (count($this->kolabHost) == 1){

      /* Remove complete entry */
      $ldap->rmdir ($this->dn);
      show_ldap_error($ldap->get_error(), _("Removing kolab host entry failed"));
      gosa_log("k=kolab entry removed, ".$this->cn." was the last kolabHost entry.");

    } else {

      /* Only modify kolabHost */
      $hosts= array();
      foreach ($this->kolabHost as $host){
        if($host != $this->cn){
          $hosts[]= $host;
        }
      }
      $attrs= array('kolabHost' => $hosts);
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($attrs); 
      show_ldap_error($ldap->get_error(), _("Removing server from kolab object failed"));
      gosa_log("Removing ".$this->cn." from list of kolabHosts");
    }

    /* Optionally execute a command after we're done */
    $this->handle_post_events("remove");
  }


  function save_object()    
  {
    if (isset($_POST['kolabtab'])){
      plugin::save_object();

      /* Save checkboxes */
      foreach (array( "postfix_enable_virus_scan", "postfix_allow_unauthenticated",
            "cyrus_admins", "cyrus_imap", "cyrus_pop3", "cyrus_imaps",
            "cyrus_pop3s", "cyrus_sieve", "apache_allow_unauthenticated_fb",
            "proftpd_ftp", "apache_http") as $cb){
        if (isset($_POST[$cb])){
          $this->$cb= "TRUE";
        } else {
          $this->$cb= "FALSE";
        }
      }

      /* Toggle relay check */
      $this->postfix_mxrelayenabled= isset($_POST['RelayMxSupport']);
    }

  }


  function check() 
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(($this->kolabFreeBusyFuture==""))    {
      $message[] = _("Future days in Free/Busy settings must be set.");
    }elseif(!is_uid($this->kolabFreeBusyFuture) || $this->kolabFreeBusyFuture < 0){
      $message[] = _("Future days in Free/Busy settings must be a positive value.");
    }

    if(!is_int((int)($this->cyrus_quotawarn))) {
      $message[] = _("The given Quota settings value must be a number.");
    }elseif(!(($this->cyrus_quotawarn<=100)&&($this->cyrus_quotawarn>=0))){
      $message[] = _("Please choose a value between 1 and 100 for Quota settings.");
    }elseif(strcasecmp($this->cyrus_quotawarn,(int)$this->cyrus_quotawarn)){
      $message[] = _("Future days must be a value.");
    }

    if(empty($this->postfix_mynetworks)) { 
      $message[] = _("No SMTP privileged networks set.");
    }

    /*if(empty($this->postfix_relayhost)) {
      $message[] = _("No SMTP smarthost/relayhost set.");
      }
     */

    return ($message);
  }


  /* Save to LDAP */
  function save() 
  {
    /* Set ldap connection */
    $ldap = $this->config->get_ldap_link();

    /* Open current dn*/
    $this->dn = "k=kolab,".$this->config->current['BASE'];

    /* Adapt relayhost */
    $this->postfix_relayhost= preg_replace('/[\[\]]/', '', $this->postfix_relayhost);
    if (!$this->postfix_mxrelayenabled && $this->postfix_relayhost != ""){
      $this->postfix_relayhost= "[".$this->postfix_relayhost."]";
    }

    /* Add ourselves to the list of kolabHost's if needed */
    if ((!$this->is_in_there)){
      $this->kolabHost[]= $this->cn;
    }



    $tmp = split(";",$this->postfix_mynetworks);
    $this->postfix_mynetworks = array();
    foreach($tmp as $tm){
      trim($tm);
      if(!empty($tm)){
        $this->postfix_mynetworks[]=$tm;
      }
    }

    $this->attrs['postfix_mynetworks']=$this->postfix_mynetworks;

    /* Call parents save to prepare $this->attrs */
    plugin::save();

    /* Save or modify? */
    $ldap->cat($this->dn, array('dn'));
    if (!$ldap->fetch()){
      $mode= "add"; 
    } else {
      $mode= "modify";
    }

    /* Do attribute conversion */
    foreach ($this->attrs as $key => $value){
      if (preg_match('/_/', $key)){
        $old_key= $key;
        $key= preg_replace('/_/', '-', $key);
        $this->attrs[$key]= $value;
        unset($this->attrs[$old_key]);
      }
    }

    /* Add kolab hosts */
    $this->attrs['kolabHost']= $this->kolabHost;

    if(($this->is_in_there)&&(!$this->exactName)){
      unset($this->attrs['kolabHost']);
    }

    /* Perform LDAP action */
    $ldap->cd($this->dn);
    $this->cleanup();;
    $ldap->$mode($this->attrs);
    show_ldap_error($ldap->get_error(), _("Saving server to kolab object failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account) {
      if ($this->is_modified) {
        $this->handle_post_events("mofify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
