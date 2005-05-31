<?php

class servkolab extends plugin {
  /* CLI vars */
  var $cli_summary = "Manage server basic objects";
  var $cli_description = "Some longer text\nfor help";
  var $cli_parameters = array("eins"=>"Eins ist toll", "zwei"=>"Zwei ist noch besser");

  var $postfix_mydomain                 = "\$domain";
  var $postfix_mydestination            = "\$domain";
  var $postfix_mynetworks               = "127.0.0.1/8";
  var $postfix_enable_virus_scan        = "TRUE";
  var $postfix_relayhost                = "";
  var $postfix_mxrelayenabled           =  true;
  var $postfix_allow_unauthenticated     = "false";
  var $cyrus_quotawarn                  = "80";
  var $kolabFreeBusyFuture              = "1";
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
  var $added                            = false; 
  var $remove                           = false;
  var $changed                          = false;
  var $attributes =  array("postfix_mydomain", "postfix_mydestination",
        "postfix_mynetworks", "postfix_enable_virus_scan","postfix_relayhost","postfix_allow_unauthenticated",
         "cyrus_admins", "cyrus_imap","kolabFreeBusyFuture",
        "cyrus_pop3", "cyrus_imaps", "cyrus_pop3s", "cyrus_sieve",
        "apache_allow_unauthenticated_fb", "proftpd_ftp",
        "apache_http", "cyrus_quotawarn", "kolabHost");

  var $objectclasses = array("top", "kolab");

  function servkolab($config, $dn = NULL) 
  {


    /* Setting the hostname and tell this Plugin that we are the kolab extension*/
    $this->hostname= preg_replace('/^cn=([^,]+),.*$/', '\1', $dn);
    $this->dn = "k=kolab,".$config->current['BASE'];
    
    /* Load variables, if given*/
    plugin::plugin($config, $this->dn);

    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }
  
    /* Is this Server a member of the Kolab extension or not ?*/ 
    if(isset($this->attrs['kolabHost']) && in_array($this->hostname, $this->attrs['kolabHost'])) {
      $this->is_account=true;
    }    else    {   
      $this->is_account=false;    
    } 
  }


  function execute() 
  {
    /* Fill templating stuff */
    $smarty = get_smarty();
    $display = "";

    $this->changed =true;

    /* Lets get all Host that are member of the Kolab extension, but don't catch count */
    if(is_array($this->attrs['kolabHost']))
      {
      unset($this->attrs['kolabHost']['count']);
      $this->kolabHost = $this->attrs['kolabHost'];
      }
    else
      {
      $this->attrs['kolabHost']=array();
      $this->kolabHost = $this->attrs['kolabHost'];
      }    
    
    /* Tell the script the dn, we are using */
    $this->kolabdn = "k=kolab,".$this->config->current['BASE'];

    /* The Ldap link is needed to ask ldap some questions */
    $ldap = $this->config->get_ldap_link();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])) {
      $this->is_account = !$this->is_account;
      

      /*if we flip to true, we'll have to create a new account, so lets do that*/
      if($this->is_account==true)
      {
        /* Only add the given Hostname to the existing hosts*/
        $this->kolabHost[]=$this->hostname;
        $this->attrs['kolabHost'][]=$this->hostname;
      }      
      else
      {
        /* First we have to find the Host we want to delete, take the index and delete em*/
        foreach($this->kolabHost as $key=>$host) 
        {
          /* Delete only if the Host is the in the array*/
          if($host == $this->hostname)
          {
            unset($this->kolabHost[$key]);
            unset($this->attrs['kolabHost'][$key]);
          }
        }
      /* We deletet an Entry so we must reorder the index of the array */ 
      $tmp = $this->kolabHost;
      $this->kolabHost=array();
      $this->attrs['kolabHost']=array();
      
      /* reorder  reorder ...*/
      foreach($tmp as $host){
        $this->kolabHost[]=$host;
        $this->attrs['kolabHost'][]=$host;
      }
      /* Tell Save that we want to delete someone*/
      $this->remove = true;
      }

    }
    /* Check Relayhost and if we have MX lookup enabled*/
    if(isset($this->postfix_relayhost[0]) && $this->postfix_relayhost[0]=="[")
      {
      $this->postfix_mxrelayenabled=false;
      $this->postfix_relayhost = str_replace("[","",$this->postfix_relayhost);
      $this->postfix_relayhost = str_replace("]","",$this->postfix_relayhost);
      }
    if($this->postfix_mxrelayenabled)    {
      $smarty->assign("RelayMxSupportCheck"," checked ");
      }      else      {
      $smarty->assign("RelayMxSupportCheck","");
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

    /* Initialize all attributes, that were submitted */
    foreach($this->attributes as $val) 
    {
      $smarty->assign($val."ACL",chkacl($this->acl,str_replace("_","-",$val)));
      /* Tell smarty which variables we are useing */
      $smarty->assign($val, $this->$val);
      if (($this->$val != "FALSE") && (!empty($this->$val)))
        $smarty->assign($val."Check", "checked");
      else
        $smarty->assign($val."Check", "");
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
  }


  function save_object()    
  {
    plugin::save_object();
  }


  function check() 
  {
    error_reporting(E_ALL);
    $message = array();
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

    if(empty($this->postfix_mynetworks))
    { 
      $message[] = _("No SMTP privileged networks set.");
    }

    if(empty($this->postfix_relayhost))
    {
      $message[] = _("No SMTP smarthost/relayhost set.");
    }


    return ($message);
  }


  /* Save to LDAP */
  function save() 
  {
    if(!$this->changed) return;

    /* Set ldap connection */
    $ldap = $this->config->get_ldap_link();
    
    /* Open current dn*/
    $ldap->cd($this->dn);

  
    /* Setup Attributes to save */
    $newattrs = array();

    /* Set vars with correct - _ - */
    foreach($this->attributes as $key) {
      $key2 = str_replace("_", "-", $key);
      $newattrs[$key2] = $this->$key;
    }
   


    /* Set bool vars */
    if ($newattrs['postfix-enable-virus-scan'] == 0)
      $newattrs['postfix-enable-virus-scan'] = "FALSE";
    else
      $newattrs['postfix-enable-virus-scan'] = "TRUE";
    if ($newattrs['cyrus-imap'] == 0)
      $newattrs['cyrus-imap'] = "FALSE";
    else
      $newattrs['cyrus-imap'] = "TRUE";
    if ($newattrs['cyrus-imaps'] == 0)
      $newattrs['cyrus-imaps'] = "FALSE";
    else
      $newattrs['cyrus-imaps'] = "TRUE";
    if ($newattrs['cyrus-pop3'] == 0)
      $newattrs['cyrus-pop3'] = "FALSE";
    else
      $newattrs['cyrus-pop3'] = "TRUE";
    if ($newattrs['cyrus-pop3s'] == 0)
      $newattrs['cyrus-pop3s'] = "FALSE";
    else
      $newattrs['cyrus-pop3s'] = "TRUE";
    if ($newattrs['cyrus-sieve'] == 0)
      $newattrs['cyrus-sieve'] = "FALSE";
    else
      $newattrs['cyrus-sieve'] = "TRUE";
    if ($newattrs['proftpd-ftp'] == 0)
      $newattrs['proftpd-ftp'] = "FALSE";
    else
      $newattrs['proftpd-ftp'] = "TRUE";
    if ($newattrs['apache-http'] == 0)
      $newattrs['apache-http'] = "FALSE";
    else
      $newattrs['apache-http'] = "TRUE";
    if ($newattrs['postfix-allow-unauthenticated'] == 0)
      $newattrs['postfix-allow-unauthenticated'] = "FALSE";
    else
      $newattrs['postfix-allow-unauthenticated'] = "TRUE";
    if ($newattrs['apache-allow-unauthenticated-fb'] == 0)
      $newattrs['apache-allow-unauthenticated-fb'] = "FALSE";
    else
      $newattrs['apache-allow-unauthenticated-fb'] = "TRUE";


    /* Reorder Host array, to get constant increasing index */
    $newattrs['kolabHost'] = array();

    /* Check Relayhost and if we have MX lookup enabled*/
    if(isset($_POST['RelayMxSupport']))
      $this->postfix_mxrelayenabled = true ;
    else
      $this->postfix_mxrelayenabled = false;
    if(!$this->postfix_mxrelayenabled)
      {
      $newattrs['postfix-relayhost']="[".$this->postfix_relayhost."]";
      }

    /* If we want to add someone or do only changes on the settings ...*/
    if(!$this->remove)
    {
      /* Get all Host known, that are member of the Kolab extension */
      $this->kolabHost = $this->attrs['kolabHost'];
      
      /* So, the Host is already member of the extension, so get all and do nothing */
      if(in_array($this->hostname,$this->kolabHost))
        {
        $newattrs['kolabHost']=$this->kolabHost;
        }
      /* So this is the first entry */
      elseif(empty($this->kolabHost))
        {
        /* Create an array an add the Host */  
        $newattrs['kolabHost']=array();
        $newattrs['kolabHost'][]=$this->hostname;
        }
      /* Theres already an entry, but only in a string, */
      elseif(is_string($this->kolabHost))
        {
        $tmp = $this->kolabHost;
        $newattrs['kolabHost']=array();
        $newattrs['kolabHost'][]=$tmp;
        $newattrs['kolabHost'][]=$this->hostname;
        }
      /* Here we have already some entries */
      elseif(is_array($newattrs['kolabHost']))
        {
        /* Insert the new one*/
        foreach($this->kolabHost as $key=>$val) {
          if (is_int($key)) {
            $newattrs['kolabHost'][] = $val;
            }
          }
        $newattrs['kolabHost'][]= $this->hostname;      
        }
      }
      else
      {
      /* Here we want to delete an entry*/
      $newattrs['kolabHost']= $this->attrs['kolabHost'];
      }
    /* Save as following object ! */
    $this->kolabdn = "k=kolab,".$this->config->current['BASE'];

    /* If we already have an object like this one,
       we only need to modify the entry
     */

    /* is this an empty extension, no host defined for it, than delet it */
    if(count($newattrs['kolabHost'])==0)
    {
      /* Delete the entry*/
      $ldap->cd ("k=kolab,".$this->config->current['BASE']);
      $ldap->rmdir("k=kolab,".$this->config->current['BASE']);


    }elseif ($ldap->dn_exists($this->kolabdn)) {
    /* Ok there is already an extension, so we only need to add the changes and the new Host, (if it was a new host)*/
      gosa_log("Modifying");
      
      /* unset all attributes we won't change*/
      unset($newattrs['cyrus-admins']);
      unset($newattrs['postfix-mydomain']);
      unset($newattrs['postfix-mydestination']);
    
      /* Update changes */
      $ldap->cd($this->kolabdn);
      $ldap->modify($newattrs);
    } else {

      /* We must create a new Entry */
      $newattrs['k'] = "kolab";
      $newattrs['uid'] = "freebusy";
      $newattrs['postfix-mydomain'] = "gonicus.de";
      $newattrs['postfix-mydestination'] = "\$mydomain";
      $newattrs['userPassword'] = "tester";
      $newattrs['objectClass'] = $this->objectclasses;

      /* For better reading / Objectclass was added at last */ 
      $newattrs = array_reverse ($newattrs);  
      
      /* And add the entry*/
      $ldap->cd($this->kolabdn);
      $ldap->add($newattrs);
    }
    /* show any errors */
    show_ldap_error($ldap->get_error());
    
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
