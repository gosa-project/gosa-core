<?php

class servkolab extends plugin {
  /* CLI vars */
  var $cli_summary = "Manage server basic objects";
  var $cli_description = "Some longer text\nfor help";
  var $cli_parameters = array("eins"=>"Eins ist toll", "zwei"=>"Zwei ist noch besser");


  var $postfix_mydomain                 = "";
  var $postfix_mydestination            = "";
  var $postfix_mynetworks               = "";
  var $postfix_enable_virus_scan        = "";
  var $cyrus_autocreatequota            = "";
  var $cyrus_quotawarn                  = "";//ok
  var $cyrus_admins                     = "";
  var $cyrus_imap                       = "";//ok
  var $cyrus_pop3                       = "";//ok
  var $cyrus_imaps                      = "";//ok
  var $cyrus_pop3s                      = "";//ok
  var $cyrus_sieve                      = "";//ok
  var $apache_allow_unauthenticated_fb  = "";//doing now
  var $proftpd_ftp                      = "";
  var $apache_http                      = "";
  var $kolabHost                        = array("");
  var $postfix_relayhost                = "";
  var $attributes =  array("postfix_mydomain", "postfix_mydestination",
        "postfix_mynetworks", "postfix_enable_virus_scan","postfix_relayhost",
        "cyrus_autocreatequota", "cyrus_admins", "cyrus_imap",
        "cyrus_pop3", "cyrus_imaps", "cyrus_pop3s", "cyrus_sieve",
        "apache_allow_unauthenticated_fb", "proftpd_ftp",
        "apache_http", "cyrus_quotawarn", "kolabHost");

  var $objectclasses = array("top", "kolab");

  function servkolab($config, $dn = NULL) 
  {
    echo "DN korrigieren<br>";
    $this->dn = "k=kolab,".$config->current['BASE'];
    
    plugin::plugin($config, $this->dn);
    $this->dn = $dn;

    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }
  }




  /* If the Button remove Kolab extension is clicked, this will be called */
  function RemoveAccount() 
  {
  
  gosa_log("Remove function");
    
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->dn);

    if ($ldap->dn_exists($this->dn)) {
      $ldap->rmdir($this->dn);
      show_ldap_error($ldap->get_error());
    }
  }





  /* this fucntion generates all needed attributes to start a new kolab extension */
  function AddAccount() 
  {

    /* get the Ldap link, to test if there is an obeject with the same dn */
    $ldap = $this->config->get_ldap_link();
    
    gosa_log("Add function");

    /* test */
    if (!$ldap->dn_exists($this->dn)) {
      /* Object Classes */
      $str['objectClass'][] = "top";
      $str['objectClass'][] = "kolab";

      /* specific kolab attribute */
      $str['k'][] = "kolab";

      /* Postfix default values */
      $str['postfix-mydomain'][] = "gonicus.de";
      $str['postfix-mydestination'][] = "\$mydomain";
      $str['postfix-mynetworks'][] = "127.0.0.0/8";
      $str['postfix-enable-virus-scan'][] = "TRUE";

      /* Cyrus Attributes */
      $str['cyrus-autocreatequota'][] = "100000";
      $str['cyrus-quotawarn'][] = "80";
      $str['cyrus-admins'][] = "manager";
      $str['cyrus-imap'][] = "TRUE";
      $str['cyrus-pop3'][] = "FALSE";
      $str['cyrus-imaps'][] = "FALSE";
      $str['cyrus-pop3s'][] = "FALSE";
      $str['cyrus-sieve'][] = "FALSE";

      /* */
      $str['apache-allow-unauthenticated-fb'][] = "FALSE";
      $str['proftpd-ftp'][] = "FALSE";
      $str['uid'][] = "freebusy";
      $str['userPassword'][] = "ZnJlZWJ1c3k";
      $str['apache-http'][] = "TRUE";


      /* Set Attributes */
      $ldap->cd($this->dn);
      $this->attrs = $str;

      /* Copy needed attributes */
      foreach($this->attributes as $val) {
        $name = preg_replace('/_/', '-', $val);
        if (isset($this->attrs["$name"][0])) {
          $this->$val = $this->attrs["$name"][0];
        }
      }

    }


  }


  function execute() 
  {
    /* Fill templating stuff */
    $smarty = get_smarty();
    $display = "";

    /* Tell the script the dn, we are using */
    $this->dn = "k=kolab,".$this->config->current['BASE'];

    $ldap = $this->config->get_ldap_link();
    
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])) {
      $this->is_account = !$this->is_account;
      if($this->is_account) 
        $this->AddAccount();
      else  
        $this->RemoveAccount();   
    
    
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

    /* There a new Host submitted */
    if (isset($_POST['NewHost'])) {
      
      $abort= false;
      if(empty($_POST['NewkolabHost']))
        {
          //print_red(_("The given Hostname ist empty."))
          $abort = true;
        }
      else
        {
        foreach($this->attrs['kolabHost'] as $host)
          {
          if(strtolower($host) ==strtolower($_POST['NewkolabHost']))
            {
            $abort = true;
            //print_red(_("The given Host already exists"));
            }
          }
        }
      if(!$abort)
        {
        $this->attrs['kolabHost'][] = $_POST['NewkolabHost'];
        }
    }

    /* We need to delete a Host */
    if (isset($_POST['DelHost'])) {
      foreach($this->attrs['kolabHost'] as $key => $host) {
        if(isset($_POST['kolabHost'])){
          if ($host == $_POST['kolabHost'])
          unset($this->attrs['kolabHost'][$key]);
        }
      }
    }



    /* Initialize all attributes, that were submitted */
    foreach($this->attributes as $val) {
      /* If theres a button pressed, we need to check all attributes */
      if ((isset($_POST['NewHost'])) || (isset($_POST['DelHost']))) {
        /* Set var to value or to zero */
        if (isset($_POST[$val])) {

          $this->$val = $_POST[$val];
        } else {
          $this->$val = 0;
        }
      }
      /* Tell smarty which variables we are useing */
      $smarty->assign($val, $this->$val);
      if (($this->$val != "FALSE") && (!empty($this->$val)))
        $smarty->assign($val."Check", "checked");
      else
        $smarty->assign($val."Check", "");

    }

    /*  If there are some Host submitted, tell it smarty */
    $this->kolabHost = array();
    if (isset($this->attrs['kolabHost'])) {
      $this->kolabHost = $this->attrs['kolabHost'];
      unset($this->kolabHost['count']);
    }

    $smarty->assign("kolabHost", $this->kolabHost);

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
    $message = array();


    if(!is_int((int)($this->cyrus_quotawarn))) {
      $message[] = "The given Quota settings value must be a number";
    }elseif(!(($this->cyrus_quotawarn<=100)&&($this->cyrus_quotawarn>=1))){
      $message[] = "Please choose a value between 1 and 100 for Quota settings";
    }
    

    /* You will need a least one entry to save this settings */
    if (count($this->kolabHost) == 0) {
      $message[] =
        "You will need at least one Entry in Hosts. If You want to 'Remove' Kolab extension please use the \"Remove Kolab extension\"  Button to remove Kolab  ";
    }
    return ($message);
  }


  /* Save to LDAP */
  function save() 
  {
    /* Set ldap connection */
    $ldap = $this->config->get_ldap_link();
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
    if ($newattrs['apache-allow-unauthenticated-fb'] == 0)
      $newattrs['apache-allow-unauthenticated-fb'] = "FALSE";
    else
      $newattrs['apache-allow-unauthenticated-fb'] = "TRUE";


    if(empty($newattrs['postfix-relayhost'])) unset( $newattrs['postfix-relayhost']);

    /* Reorder Host array, to get constant increasing index */
    $newattrs['kolabHost'] = array();
    foreach($this->attrs['kolabHost'] as $key=>$val) {
      if (is_int($key)) {
        $newattrs['kolabHost'][] = $val;
      }
    }

    /* Save as following object ! */
    $this->dn = "k=kolab,".$this->config->current['BASE'];

    /* If we already have an object like this one,
       we only need to modify the entry
     */



    if ($ldap->dn_exists($this->dn)) {
      gosa_log("Modifying");
      
      /* unset all attributes we won't change*/
      unset($newattrs['cyrus-admins']);
      unset($newattrs['postfix-mydomain']);
      unset($newattrs['postfix-mydestination']);
      //print_a($newattrs); 
    


      $ldap->cd($this->dn);
      $ldap->modify($newattrs);
    } else {
      /* We must create a new Entry */
      $newattrs['k'] = "kolab";
      $newattrs['uid'] = "freebusy";
      $newattrs['postfix-mydomain'] = "gonicus.de";
      $newattrs['postfix-mydestination'] = "\$mydomain";
      $newattrs['userPassword'] = "tester";
      $newattrs['objectClass'] = $this->objectclasses;

      gosa_log("Adding");
      $ldap->cd($this->dn);
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
