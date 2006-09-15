<?php

class phonequeue extends plugin
{
  /* plugin specific values */
  var $mail= "";
  var $cn= "";


  var $goFonTimeOut             ="20"; 
  var $goFonMaxLen              ="20"; // 
  var $goFonAnnounceFrequency   ="60"; // Annouce Frequency in seconds
  var $goFonDialOption_t        ="";
  var $goFonDialOption_T        ="";
  var $goFonDialOption_h        ="";
  var $goFonDialOption_r        ="";
  var $goFonQueueAnnounce       ="gonicus-berlin-welcome";
  var $goFonDialOption_H        ="";
  var $goFonMusiconHold         ="default";
  var $goFonWelcomeMusic        ="gonicus-berlin-welcome";
  var $goFonQueueReportHold     ="yes";
  var $goFonQueueYouAreNext     ="queue-youarenext";
  var $goFonQueueThereAre       ="queue-thereare";
  var $goFonQueueCallsWaiting   ="queue-callswaiting";
  var $goFonQueueThankYou       ="queue-thankyou";
  var $goFonQueueMinutes        ="queue-minutes"; 
  var $goFonQueueSeconds        ="queue-seconds";
  var $goFonQueueLessThan       ="queue-lessthan";
  var $goFonQueueLanguage       ="queue-holdtime";
  var $goFonQueueStrategy       ="ringall";
  var $goFonQueueAnnounceHoldtime="yes";
  var $telephoneNumber          =array();
  var $goFonQueueMember         =array(); 
  var $goFonDialOption          ="";
  var $goFonQueueRetry          =5;

  var $goFonQueueStrategyOptions=array();
  var $goFonQueueStrategyOptionsR=array();
  

  var $old_phone_numbers        =array();

  /* attribute list for save action */
  var $attributes= array( "goFonTimeOut","goFonMaxLen","goFonAnnounceFrequency","goFonDialOption_t","goFonDialOption_T",
      "goFonDialOption_h","goFonDialOption_r","cn",
      "goFonDialOption_H","goFonMusiconHold","goFonWelcomeMusic","goFonQueueReportHold","goFonQueueYouAreNext",
      "goFonQueueThereAre","goFonQueueCallsWaiting","goFonQueueThankYou","goFonQueueMinutes","goFonQueueSeconds","goFonQueueLessThan",
      "telephoneNumber","goFonQueueLanguage","goFonQueueStrategy","goFonQueueAnnounceHoldtime","goFonQueueAnnounce","goFonDialOption","goFonQueueRetry");
  /* ObjectClass */
  var $objectclasses= array("goFonQueue");

  function phonequeue ($config, $dn= NULL)
  {
    plugin::plugin($config, $dn);

    /* Include config object */
    $this->config= $config;

    /* Save initial account state */
    $this->initially_was_account= $this->is_account;

    if($this->is_account){
      if(isset($this->attrs['telephoneNumber'])){
        $this->telephoneNumber=$this->attrs['telephoneNumber'];
        unset($this->telephoneNumber['count']); 
      }

      for($i = 0; $i < strlen($this->goFonDialOption); $i++){
        $name = "goFonDialOption_".$this->goFonDialOption[$i];
        $this->$name=$this->goFonDialOption[$i];
      }
    }

    if($this->goFonQueueAnnounceHoldtime == "no"){
      $this->goFonQueueAnnounceHoldtime=false;
    }
    $this->old_phone_numbers = $this->telephoneNumber;

   $types= array('ringall'    =>_("ring all"),
          'roundrobin' =>_("round robin"),
          'leastrecent'=>_("least recently called"),
          'fewestcalls'=>_("fewest completed calls"),
          'random'     =>_("random"),
          'rrmemory'   =>_("round robin with memory"));

   $i = 0;
   foreach($types as $type => $name){
     $i++;
     $this->goFonQueueStrategyOptions[$i]    =$name;
     $this->goFonQueueStrategyOptionsR[$i]   =$type;
     $tmp[$type] = $i; 
   }
  
  $this->goFonQueueStrategy= $tmp[$this->goFonQueueStrategy];
  

  }


  function execute()
  {
	/* Call parent execute */
	plugin::execute();

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove the phone queue from this Account"),
            _("Phone queue is enabled for this group. You can disable it by clicking below."));
      } else {
        $display= $this->show_header(_("Create phone queue"), 
                  _("For this group the phone queues are disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    /* Add queue number */ 
    if(isset($_POST['add_phonenumber'])&&(isset($_POST['phonenumber']))&&(!empty($_POST['phonenumber']))){
      if((!in_array($_POST['phonenumber'],$this->telephoneNumber))&&(is_numeric($_POST['phonenumber']))){
        $this->telephoneNumber[]=$_POST['phonenumber'];
      }
    }

    /* Delete queue number */ 
    if(isset($_POST['delete_phonenumber'])&&(isset($_POST['goFonQueueNumber_List']))){
      unset($this->telephoneNumber[$_POST['goFonQueueNumber_List']]);
    }
  
    $tmp = array();
    foreach($this->telephoneNumber as $val){
      if(!empty($val)){
        $tmp[]= $val;
      }
    }  
    $this->telephoneNumber=$tmp;

    /* queue number up */ 
    if(isset($_POST['up_phonenumber'])&&(isset($_POST['goFonQueueNumber_List']))){
      if($_POST['goFonQueueNumber_List']>0){
        $up   = $this->telephoneNumber[$_POST['goFonQueueNumber_List']];
        $down = $this->telephoneNumber[$_POST['goFonQueueNumber_List']-1];
        $this->telephoneNumber[$_POST['goFonQueueNumber_List']]    = $down; 
        $this->telephoneNumber[$_POST['goFonQueueNumber_List']-1]  = $up;
      }
    }

    /* Queuenumber down */ 
    if(isset($_POST['down_phonenumber'])&&(isset($_POST['goFonQueueNumber_List']))){
      if(isset($this->telephoneNumber[($_POST['goFonQueueNumber_List']+1)])){
        $up   = $this->telephoneNumber[$_POST['goFonQueueNumber_List']+1];
        $down = $this->telephoneNumber[$_POST['goFonQueueNumber_List']];
        $this->telephoneNumber[$_POST['goFonQueueNumber_List']+1]    = $down; 
        $this->telephoneNumber[$_POST['goFonQueueNumber_List']]  = $up;
      }
    }

    $smarty= get_smarty();

    $smarty->assign("goFonQueueLanguageOptions",array('de'=>_('German')));
    $smarty->assign("goFonQueueStrategyOptions", $this->goFonQueueStrategyOptions);

    foreach($this->attributes as $key => $val){
      $smarty->assign($val,$this->$val);  

      if($this->$val == false){
        $smarty->assign($val."CHK","");
      }else{
        $smarty->assign($val."CHK"," checked ");
      }

      if(chkacl($this->acl,$key)==""){
        $smarty->assign($val."ACL","");
      }else{
        $smarty->assign($val."ACL"," disabled ");
      }
    }
    return ($display.$smarty->fetch (get_template_path('phonequeue.tpl', TRUE)));
  }


  /* Check formular input */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if($this->is_number_used()){
      $message[] = $this->is_number_used();
    }

    if($this->generate_mysql_table_entries()){
      $message[] = $this->generate_mysql_table_entries();
    }

    if(!((is_numeric($this->goFonTimeOut))||(empty($this->goFonTimeOut)))){
      $message[] = _("Timeout must be numeric");
    }
    if(!((is_numeric($this->goFonQueueRetry))||(empty($this->goFonQueueRetry)))){
      $message[] = _("Retry must be numeric");
    }
    if(!((is_numeric($this->goFonMaxLen))||(empty($this->goFonMaxLen)))){
      $message[] = _("Max queue length must be numeric");
    }
    if(!((is_numeric($this->goFonAnnounceFrequency))||(empty($this->goFonAnnounceFrequency)))){
      $message[] = _("Announce frequency must be numeric");
    }
    if(count($this->telephoneNumber)==0){
      $message[] = _("There must be least one queue number defined.");
    }

    return $message;
  }



  function generate_mysql_table_entries($save = false)
  {

    $SQL = array();

    if(!isset($_SESSION['config']->data['SERVERS']['FON'])){
      return(_("There is currently no asterisk server defined. Your settings can't be saved."));
    }

    // Get Configuration for Mysql database Server
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];
    $s_parameter  ="";

    // Connect to DB server
    $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

    // Check if we are  connected correctly
    if(!$r_con){
      gosa_log(mysql_error());
      return (sprintf(_("The MySQL server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
          $a_SETUP['SERVER'],$a_SETUP['LOGIN']));
    }

    // Select database for Extensions
    $db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

    // Test if we have the database selected correctly
    if(!$db){
      gosa_log(mysql_error());
      return( sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
    }

    if($save){
      $i = 0;
      $prio = 11; 

      $delete = array();
      if(!empty($this->parent->by_object['ogroup'])){
        $new_cn = $this->parent->by_object['ogroup']->cn;
      }
      $old_cn = $this->cn;

      $delete[]=    "DELETE FROM ".$a_SETUP['EXT_TABLE']."    WHERE exten='".$new_cn."';\n";
      $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_TABLE']."  WHERE name='".$new_cn."'; \n";
      $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_MEMBER_TABLE']." WHERE queue_name='".$new_cn."';\n";
      $delete[]=    "DELETE FROM ".$a_SETUP['EXT_TABLE']."    WHERE exten='".$old_cn."';\n";
      $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_TABLE']."  WHERE name='".$old_cn."'; \n";
      $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_MEMBER_TABLE']." WHERE queue_name='".$old_cn."';\n";

      // Delete old Entries 
      if(is_array($this->old_phone_numbers)){
        foreach($this->old_phone_numbers as $phone){
          $delete[]= "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$phone."';\n";
        }
      }

      /* Perform queries to delte old entries */
      foreach($delete as $query){
        if(!mysql_query($query)){
          gosa_log(mysql_error());
          return(mysql_error(). sprintf(_("Can't delete in Database %s, on Server %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
        }
      }

      $this->attrs['cn'][0] = $new_cn;

      /* Append new Member for this queue */ 
      $i = 0;
      $queueuser =array();
      foreach($this->parent->by_object['ogroup']->memberList as $member){
        if(isset($member['objectClass'])){
          if(in_array("goFonAccount",$member['objectClass'])){
            $i ++ ;
            $queueuser[$i]['queue_name']  = $this->attrs['cn'][0]; 
            $queueuser[$i]['interface']   = "SIP/".$member['uid'][0]; 
            $queueuser[$i]['penalty']     = 1; 
          }
        }
      }

      /* Parse and Add members to query Array */
      if(is_array($queueuser)){
        foreach($queueuser as $user){
          $entries = "";
          $values  = "";
          foreach($user as $attr => $val){
            $entries.= "`".$attr."`,"; 
            $values .= "'".$val."',";
          }
          $values  = preg_replace("/,$/","",$values);
          $entries = preg_replace("/,$/","",$entries );

          $SQL[]="INSERT INTO ".$a_SETUP['QUEUE_MEMBER_TABLE']." (".$entries.") VALUES (".$values.")"; 
        }
      }
      
      /* generate Extension entries, with priority  */

      $queueusers=0;
      foreach($this->parent->by_object['ogroup']->memberList as $member){
        if(isset($member['objectClass'])){
          if(in_array("goFonAccount",$member['objectClass'])){
            $queueusers++;
          }
        }
      }


      $i = 0;

      $i_insert_only_once = false;
      
      foreach($this->telephoneNumber as $num){
       
        // If there are no member in a Queue
        // Play sound an quit

        // A Queue is not deleted directly, it is stored until the o group is deleted
        
        if($i_insert_only_once == false){
          $i_insert_only_once = true;
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $this->attrs['cn'][0];
          $a_ext[$i]['priority'] = 1;
          $a_ext[$i]['app']      = "Goto";
          $a_ext[$i]['appdata']  = $num."|1";
          $i ++ ; 
        }
    
        if($queueusers == 0){
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 1;
          $a_ext[$i]['app']      = "SetLanguage";
          $a_ext[$i]['appdata']  = "de";
          $i ++ ; 
          
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 2;
          $a_ext[$i]['app']      = "Playback";
          $a_ext[$i]['appdata']  = "ss-noservice";
          $i ++ ; 
          
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 3;
          $a_ext[$i]['app']      = "Goto";
          $a_ext[$i]['appdata']  = "default";
          $i ++ ; 
        }else{
          $prio --;
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 1;
          $a_ext[$i]['app']      = "Wait";
          $a_ext[$i]['appdata']  = "2";
          $i ++ ; 
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 2;
          $a_ext[$i]['app']      = "SetLanguage";
          $a_ext[$i]['appdata']  = $this->goFonQueueLanguage;
          $i ++ ; 
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 3;
          $a_ext[$i]['app']      = "Playback";
          $a_ext[$i]['appdata']  = $this->goFonWelcomeMusic;
          $i ++ ; 
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 4;
          $a_ext[$i]['app']      = "SetCIDName";
          if(!empty($this->parent->by_object['ogroup']->description)){
            $a_ext[$i]['appdata']  = $this->parent->by_object['ogroup']->description;
          }else{
            $a_ext[$i]['appdata']  = $this->attrs['cn'][0]." - ".$num;
          }
          $i ++ ; 
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 5;
          $a_ext[$i]['app']      = "SetVar";
          $a_ext[$i]['appdata']  = "QUEUE_PRIO=".$prio;
          $i ++ ; 
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 6;
          $a_ext[$i]['app']      = "Queue";
          $a_ext[$i]['appdata']  =  $this->attrs['cn'][0].
            "|".
            $this->goFonDialOption_t.
            $this->goFonDialOption_T.
            $this->goFonDialOption_h.
            $this->goFonDialOption_H.
            $this->goFonDialOption_r;
        }

        if($this->goFonQueueAnnounceHoldtime != false) {
          $this->goFonQueueAnnounceHoldtime = "yes";
        }else{
          $this->goFonQueueAnnounceHoldtime = "no";
        }


        /* Generate Priority Entry */
        $queue["announce"]              = "";
        $queue["monitor_join"]          = "";
        $queue["monitor_format"]        = "";
        $queue["queue_holdtime"]        = $this->goFonQueueAnnounce;
        $queue["queue_lessthan"]        = $this->goFonQueueLessThan;   
        $queue["announce_round_seconds"]= "";   
        $queue["retry"]                 = $this->goFonQueueRetry;
        $queue["wrapuptime"]            = "";
        $queue["servicelevel"]          = "";
        $queue["joinempty"]             = "no";
        $queue["leavewhenempty"]        = "yes";   
        $queue["eventmemberstatus"]     = "";
        $queue["eventwhencalled"]       = "";
        $queue["reportholdtime"]        = "yes";
        $queue["memberdelay"]           = "";
        $queue["weight"]                = "";
        $queue["timeoutrestart"]        = "";

        $queue["context"]               = "default";
        $queue["name"]                  = $this->attrs['cn'][0];  
        $queue["timeout"]               = $this->goFonTimeOut; 
        $queue["maxlen"]                = $this->goFonMaxLen;
        $queue["strategy" ]             = $this->goFonQueueStrategyOptionsR[$this->goFonQueueStrategy];
        $queue["queue_thankyou"]        = $this->goFonQueueThankYou;   
        $queue["queue_reporthold"]      = $this->goFonQueueReportHold; 
        $queue["announce_frequency"]    = $this->goFonAnnounceFrequency;
        $queue["queue_youarenext"]      = $this->goFonQueueYouAreNext;   
        $queue["queue_thereare"]        = $this->goFonQueueThereAre;   
        $queue["queue_callswaiting"]    = $this->goFonQueueCallsWaiting;
        $queue["queue_minutes"]         = $this->goFonQueueMinutes;
        $queue["queue_seconds"]         = $this->goFonQueueSeconds;   
        $queue["announce_holdtime"]     = $this->goFonQueueAnnounceHoldtime;   
        $queue["musiconhold"]           = $this->goFonMusiconHold;

        $i++;
      }

      /* Parse and Add Extension entries */
      foreach($a_ext as $ext){
        $entries = "";
        $values  = "";
        foreach($ext as $attr => $val){
          $entries.= "`".$attr."`,";
          $values .= "'".$val."',";
        }
        $values  = preg_replace("/,$/","",$values);
        $entries = preg_replace("/,$/","",$entries );
        $SQL[]="INSERT INTO ".$a_SETUP['EXT_TABLE']." (".$entries.") VALUES (".$values.")";
      }


      /* Parse and Add Queue */
      $entries = "";
      $values  = "";
      foreach($queue as $attr=>$val){
        if($val == "") continue;
        $entries.= "`".$attr."`,";
        $values .= "'".$val."',";
      }
      $values  = preg_replace("/,$/","",$values);
      $entries = preg_replace("/,$/","",$entries );
      $SQL[]="INSERT INTO ".$a_SETUP['QUEUE_TABLE']." (".$entries.") VALUES (".$values.")";

      foreach($SQL as $query){
         if(!mysql_query($query)){
          gosa_log(mysql_error());
          print_red(mysql_error());
          return(mysql_error(). sprintf(_("Can't delete in Database %s, on Server %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
        }
      }

    }
    @mysql_close($r_con);
    return(false);
  }



 /* This function checks if the given phonenumbers are available or already in use*/

  function is_number_used()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(|(objectClass=goFonAccount)(objectClass=goFonQueue)(objectClass=goFonConference))", array("telephoneNumber","cn","uid"));
    while($attrs = $ldap->fetch()) {
      unset($attrs['telephoneNumber']['count']);
      foreach($attrs['telephoneNumber'] as $tele){
        if(!isset($attrs['cn'][0])) $attrs['cn'][0]=$attrs['dn'];
        if(!isset($attrs['uid'][0])) $attrs['uid'][0]=$attrs['dn'];
        $numbers[$tele]=$attrs;
      }
    }

    foreach($this->telephoneNumber as $num){
      if((isset($numbers[$num]))&&(($numbers[$num]['cn'][0]!= $this->attrs['cn'][0]))){
        if(isset($numbers[$num]['uid'][0])){
          return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['uid'][0]);
        }else{
          return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['cn'][0]);
        }
      }
    }
  }





  function save_object()
  {
    plugin::save_object();  
    if(isset($_POST['phonenumber'])){
      foreach(array("goFonDialOption_t","goFonDialOption_T","goFonDialOption_h","goFonDialOption_r","goFonDialOption_H","goFonMusiconHold") as $val){
        if(isset($_POST[$val])){
          $this->$val = $_POST[$val];
        }else{
          $this->$val = false;
        }
      }
      if(isset($_POST['goFonQueueAnnounceHoldtime'])){
        $this->goFonQueueAnnounceHoldtime = "yes";
      }else{
        $this->goFonQueueAnnounceHoldtime = false;
      }
      
    }

  }

  function save()
  {
  #fixme workaround : Tab is not initialised correct
	if(!$this->is_account) return;
    $ldap= $this->config->get_ldap_link();

    $this->generate_mysql_table_entries(true);

    plugin::save();
    $this->attrs['goFonDialOption'] = "";
    foreach(array("goFonDialOption_t","goFonDialOption_T","goFonDialOption_r","goFonDialOption_h","goFonDialOption_H","cn") as $val){
      $this->attrs['goFonDialOption'].=$this->$val; 
      unset($this->attrs[$val]); 
    }
    if($this->attrs['goFonDialOption']=="") $this->attrs['goFonDialOption']=array();
  
    if($this->goFonQueueAnnounceHoldtime != "no" ){
      $this->attrs['goFonQueueAnnounceHoldtime'] = "yes";
    }else{
      $this->attrs['goFonQueueAnnounceHoldtime'] = "no";
    }

    $this->attrs['goFonQueueStrategy'] = $this->goFonQueueStrategyOptionsR[$this->goFonQueueStrategy];

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving phone queue failed"));

    /* Optionally execute a command after we're done */
    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }
  }


  /* remove object from parent */
  function remove_from_parent()
  {
    $SQL = array();

    // Get Configuration for Mysql database Server
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];
    $s_parameter  ="";

    // Connect to DB server
    $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

    // Check if we are  connected correctly
    if(!$r_con){
      gosa_log(mysql_error());
      return (sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
            $a_SETUP['SERVER'],$a_SETUP['LOGIN']));
    }

    // Select database for Extensions
    $db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

    // Test if we have the database selected correctly
    if(!$db){
      gosa_log(mysql_error());
      return( sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
    }

    /* Remove all temporary attributes */
    $tmp = array_flip($this->attributes);
    foreach(array("goFonDialOption_t","goFonDialOption_T","goFonDialOption_r","goFonDialOption_h","goFonDialOption_H","cn") as $val){
      unset($this->$val);
      unset($this->attrs[$val]);
      unset($tmp[$val]);
    }   
    foreach(array_flip($tmp) as $key => $val){
      $tmp2[]=$val;
    } 
    $this->attributes = $tmp2;

    $i = 0;
    $prio = 11;

    if(empty($this->cn)){
      $this->cn = $this->parent->by_object['ogroup']->cn;
      $this->attrs['cn'][0] = $this->parent->by_object['ogroup']->cn;
    }

    // Delete old Entries
    $delete = array();
    foreach($this->old_phone_numbers as $phone){
      $delete[]= "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$phone."';\n";
    }
    $delete[]=    "DELETE FROM ".$a_SETUP['EXT_TABLE']."    WHERE exten='".$this->attrs['cn'][0]."';\n";
    $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_TABLE']." WHERE name=\"".$this->attrs['cn'][0]."\"; \n";
    $delete[]=    "DELETE FROM ".$a_SETUP['QUEUE_MEMBER_TABLE']." WHERE queue_name=\"".$this->attrs['cn'][0]."\";\n";

    /* Perform queries to delte old entries */
    foreach($delete as $query){
      if(!mysql_query($query)){
        gosa_log(mysql_error());
        return(mysql_error(). sprintf(_("Can't delete in Database %s, on Server %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
      }
    }



    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing phone queue failed"));
  }

  
  function getCopyDialog()
  {
    $str  = "";
    $str .= _("Phone number");
    $str .= "&nbsp;<input type='text' name='telephoneNumber' value='".$this->telephoneNumber."'>";
    return($str);
  }


  function saveCopyDialog()
  {
    if(isset($_POST['telephoneNumber'])){
      $this->telephoneNumber = $_POST['telephoneNumber'];
    }
  }


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
