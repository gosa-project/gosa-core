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
  var $cyrus_quotawarn                  = "999";//ok
  var $cyrus_admins                     = "TRUE";
  var $cyrus_imap                       = "TRUE";//ok
  var $cyrus_pop3                       = "TRUE";//ok
  var $cyrus_imaps                      = "TRUE";//ok
  var $cyrus_pop3s                      = "TRUE";//ok
  var $cyrus_sieve                      = "TRUE";//ok
  var $apache_allow_unauthenticated_fb  = "TRUE";//doing now
  var $proftpd_ftp                      = "TRUE";
  var $apache_http                      = "TRUE";
  var $kolabHost                        = array();
  var $postfix_relayhost                = "";
  var $added                            = false; 
  var $remove                           = false;
  var $attributes =  array("postfix_mydomain", "postfix_mydestination",
        "postfix_mynetworks", "postfix_enable_virus_scan","postfix_relayhost",
         "cyrus_admins", "cyrus_imap",
        "cyrus_pop3", "cyrus_imaps", "cyrus_pop3s", "cyrus_sieve",
        "apache_allow_unauthenticated_fb", "proftpd_ftp",
        "apache_http", "cyrus_quotawarn", "kolabHost");

  var $objectclasses = array("top", "kolab");

  function servkolab($config, $dn = NULL) 
  {
    $this->hostname= preg_replace('/^cn=([^,]+),.*$/', '\1', $dn);
    $this->dn = "k=kolab,".$config->current['BASE'];
    
    plugin::plugin($config, $this->dn);

    /* Copy needed attributes */
    foreach($this->attributes as $val) {
      $name = preg_replace('/_/', '-', $val);
      if (isset($this->attrs["$name"][0])) {
        $this->$val = $this->attrs["$name"][0];
      }
    }
    
    if(in_array($this->hostname,$this->attrs['kolabHost']))
    {
      $this->is_account=true;
    }
    else
    {   
      $this->is_account=false;
    } 
  }


  function execute() 
  {
    /* Fill templating stuff */
    $smarty = get_smarty();
    $display = "";


    unset($this->attrs['kolabHost']['count']);
    $this->kolabHost = $this->attrs['kolabHost'];


    /* Tell the script the dn, we are using */
    $this->kolabdn = "k=kolab,".$this->config->current['BASE'];

    $ldap = $this->config->get_ldap_link();
    
    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])) {
      $this->is_account = !$this->is_account;
      
      if($this->is_account==true)
      {
        $this->kolabHost[]=$this->hostname;
        $this->attrs['kolabHost'][]=$this->hostname;
      }      
      else
      {
        foreach($this->kolabHost as $key=>$host) 
        {
          if($host == $this->hostname)
          {
            unset($this->kolabHost[$key]);
            unset($this->attrs['kolabHost'][$key]);
          }
        }
      $tmp = $this->kolabHost;
      $this->kolabHost=array();
      $this->attrs['kolabHost']=array();
      foreach($tmp as $host){
        $this->kolabHost[]=$host;
        $this->attrs['kolabHost'][]=$host;
      }
      $this->remove = true;
      }

    }


    print_a($this);
    
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
      /* If theres a button pressed, we need to check all attributes */
      if ((isset($_POST['NewHost'])) || (isset($_POST['DelHost']))) 
      {
        /* Set var to value or to zero */
        if (isset($_POST[$val])) 
        {

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

    if(!$this->remove)
    {
      $this->kolabHost = $this->attrs['kolabHost'];
      
      if(in_array($this->hostname,$this->kolabHost))
        {
        $newattrs['kolabHost']=$this->kolabHost;
        }
      elseif(empty($this->kolabHost))
        {
        print "empty";
        $newattrs['kolabHost']=array();
        $newattrs['kolabHost'][]=$this->hostname;
        }
      elseif(is_string($this->kolabHost))
        {
        print "string";
        $tmp = $this->kolabHost;
        $newattrs['kolabHost']=array();
        $newattrs['kolabHost'][]=$tmp;
        $newattrs['kolabHost'][]=$this->hostname;
        }
      elseif(is_array($newattrs['kolabHost']))
        {
        print "array";
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
      $newattrs['kolabHost']= $this->attrs['kolabHost'];
      }
    /* Save as following object ! */
    $this->kolabdn = "k=kolab,".$this->config->current['BASE'];

    /* If we already have an object like this one,
       we only need to modify the entry
     */

    if(count($newattrs['kolabHost'])==0)
    {
    $ldap->cd ("k=kolab,".$this->config->current['BASE']);
    print "Deleting";
    $ldap->rmdir("k=kolab,".$this->config->current['BASE']);

    }elseif ($ldap->dn_exists($this->kolabdn)) {
      gosa_log("Modifying");
      
      /* unset all attributes we won't change*/
      unset($newattrs['cyrus-admins']);
      unset($newattrs['postfix-mydomain']);
      unset($newattrs['postfix-mydestination']);
    

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

      gosa_log("Adding");
      
      $newattrs = array_reverse ($newattrs);  
    
      $ldap->cd($this->kolabdn);
      $ldap->add($newattrs);
    }
    
    print_a($newattrs);
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
