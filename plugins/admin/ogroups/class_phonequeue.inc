<?php

class phonequeue extends plugin
{
  /* plugin specific values */
  var $mail= "";
  var $cn= "";
  var $old_cn ="";

  var $goFonTimeOut             ="20"; 
  var $goFonMaxLen              ="20"; // 
  var $goFonAnnounceFrequency   ="60"; // Annouce Frequency in seconds
  var $goFonDialOptiont         ="";
  var $goFonDialOptionT         ="";
  var $goFonDialOptionh         ="";
  var $goFonDialOptionr         ="";
  var $goFonDialOptionH         ="";
  var $goFonQueueAnnounce       ="gonicus-berlin-welcome";
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

  var $goFonHomeServer  = "0";
  var $init_HomeServer  = "0";
  var $goFonHomeServers = array();

  var $view_logged = FALSE;

  /* attribute list for save action */
  var $attributes= array( "goFonTimeOut","goFonMaxLen","goFonAnnounceFrequency","goFonDialOptiont","goFonDialOptionT",
      "goFonDialOptionh","goFonDialOptionr","cn","goFonHomeServer",
      "goFonDialOptionH","goFonMusiconHold","goFonWelcomeMusic","goFonQueueReportHold","goFonQueueYouAreNext",
      "goFonQueueThereAre","goFonQueueCallsWaiting","goFonQueueThankYou","goFonQueueMinutes","goFonQueueSeconds","goFonQueueLessThan",
      "telephoneNumber","goFonQueueLanguage","goFonQueueStrategy","goFonQueueAnnounceHoldtime","goFonQueueAnnounce","goFonDialOption","goFonQueueRetry");

  /* ObjectClass */
  var $objectclasses= array("goFonQueue");



  /* Pluigin initialization 
   * - Check currently selected and available home server.
   * - Set default home server if necessary
   * - Parse phone options flags
   * - Get phone numbers 
   */
  function phonequeue (&$config, $dn= NULL)
  {
    plugin::plugin($config, $dn);

    /* Check server configurations
     * Load all server configuration in $this->goFonHomeServers if available
     *  and use first server as default if necessary.
     */
    if(isset($config->data['SERVERS']['FON']) && 
       count($config->data['SERVERS']['FON']) &&
       is_callable("mysql_connect")){

      /* Set available server */
      $this->goFonHomeServers = $config->data['SERVERS']['FON'];

      /* Set default server */
      if($this->dn == "new"){
        $this->goFonHomeServer = $this->goFonHomeServers[0]['DN'];
      }

      /* Remember inital home server, to be able to remove old entries */
      $this->init_HomeServer = $this->goFonHomeServer;

      /* get config */
      if(!isset($this->goFonHomeServers[$this->goFonHomeServer])){
        print_red(sprintf(_("The specified home server '%s' is not available in GOsa server configuration. Saving this account will create a new entry on the server '%s'. Use cancel if you do not want to create a new entry while ignoring old accounts."), preg_replace("/,/",", ",$this->goFonHomeServer), preg_replace("/,/",", ",$this->goFonHomeServers[0]['DN'])));
        $this->goFonHomeServer = $this->goFonHomeServers[0]['DN'];
        $this->init_HomeServer = $this->goFonHomeServers[0]['DN'];
      }
      $cur_cfg = $this->goFonHomeServers[$this->goFonHomeServer];
    }


    /* Variable init  
     *  Load phone nubmers and parse dial options 
     */
    if($this->is_account){
      if(isset($this->attrs['telephoneNumber'])){
        $this->telephoneNumber=$this->attrs['telephoneNumber'];
        unset($this->telephoneNumber['count']); 
      }
      for($i = 0; $i < strlen($this->goFonDialOption); $i++){
        $name = "goFonDialOption".$this->goFonDialOption[$i];
        $this->$name=$this->goFonDialOption[$i];
      }
    }

    /* Set Queue announce hold time to true/false */
    if(preg_match("/no/i",$this->goFonQueueAnnounceHoldtime)){
      $this->goFonQueueAnnounceHoldtime=false;
    }else{
      $this->goFonQueueAnnounceHoldtime=true;
    }

    /* Define all available ringdown types */
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
    $this->old_cn = $this->cn;
    $this->old_phone_numbers = $this->telephoneNumber;
  }


  /* This function ensures that the selected home server 
   * and the initially selected home server are reachable and accessible  
   */
  function check_database_accessibility()
  {
    /* Check if mysql extension is available */
    if(!is_callable("mysql_pconnect")){
      return(_("Can't save any changes to asterisk database, there is currently no mysql extension available in your php setup."));
    }

    /********************
     * Check currently selected home server
     ********************/

    $cfg_Current  = $this->goFonHomeServers[$this->goFonHomeServer];
    $r_current    =  @mysql_pconnect($cfg_Current['SERVER'],$cfg_Current['LOGIN'],$cfg_Current['PASSWORD']);
    if(!$r_current){
      new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($r_current));
      return(sprintf(_("The MySQL home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
            $cfg_Current['SERVER'],$cfg_Current['LOGIN']));
    }
    $db_current  =  @mysql_select_db($cfg_Current['DB'],$r_current);
    if(!$db_current){
      new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($r_current));
      mysql_close($r_current);
      return( sprintf(_("Can't select database '%s' on home server '%s'."),$cfg_Current['DB'],$cfg_Current['SERVER']));
    }

    /********************
     * Check init home server
     ********************/

    if($this->initially_was_account){
      $cfg_Init  = $this->goFonHomeServers[$this->init_HomeServer] ;
      $r_init    =  @mysql_pconnect($cfg_Init['SERVER'],$cfg_Init['LOGIN'],$cfg_Init['PASSWORD']);
      if(!$r_init){
        new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($r_init));
        return(sprintf(_("The MySQL initial home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
              $cfg_Init['SERVER'],$cfg_Init['LOGIN']));
      }
      $db_init  =  @mysql_select_db($cfg_Init['DB'],$r_init);
      if(!$db_init){
        new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($r_init));
        mysql_close($r_init);
        return( sprintf(_("Can't select database '%s' on initial home server '%s'."),$cfg_Init['DB'],$cfg_Init['SERVER']));
      }
    }
  }

  
  /* Display plugin ui */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","ogroups/".get_class($this),$this->dn);
    }

    if(isset($_POST['modify_state'])){
      if($this->is_account && $this->acl_is_removeable()){
        $this->is_account= FALSE;
      }elseif(!$this->is_account && $this->acl_is_createable()){
        $this->is_account= TRUE;
      }
    }

    /* Show tab dialog headers */
    if ($this->parent !== NULL){
      if ($this->is_account){
        $display= $this->show_disable_header(_("Remove the phone queue from this Account"),
            _("Phone queue is enabled for this group. You can disable it by clicking below."));
      } else {
        $display= $this->show_enable_header(_("Create phone queue"),
            _("For this group the phone queues are disabled. You can enable them by clicking below."));
        return ($display);
      }
    }

    if($this->acl_is_writeable("telephoneNumber")){

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
        $this->telephoneNumber[$_POST['goFonQueueNumber_List']]  = $up;
      }
    }

    $tmp = array();
    foreach($this->telephoneNumber as $val){
      if(!empty($val)){
        $tmp[]= $val;
      }
    }  
    $this->telephoneNumber=$tmp;

    /* Assign samrty variables */
    $smarty= get_smarty();
    $smarty->assign("goFonQueueLanguageOptions",get_languages(FALSE,TRUE));
    $smarty->assign("goFonQueueStrategyOptions", $this->goFonQueueStrategyOptions);

    /* Set acls */
    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $name => $translated){
      $smarty->assign($name."ACL",$this->getacl($name));
    }

    foreach($this->attributes as $key => $val){
      $smarty->assign($val,$this->$val);  
      if($this->$val == false){
        $smarty->assign($val."CHK","");
      }else{
        $smarty->assign($val."CHK"," checked ");
      }
    }
    
    /* Create array with goFonHomeServer */
    $tmp = array();
    foreach($this->goFonHomeServers as $dn => $val){
      if(!is_numeric($dn)){
        $tmp[$dn]  = $val['SERVER'];
      }
    }
    $smarty->assign("goFonHomeServers",$tmp);

    return ($display.$smarty->fetch (get_template_path('phonequeue.tpl', TRUE)));
  }


  /* Check formular input */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    if(!count($this->goFonHomeServers)){
      $message[] = _("There must be at least one server with an asterisk database to create a phone queue.");
    }
    if(empty($this->goFonHomeServer)){
      $message[] = _("Please select a valid goFonHomeServer.");
    }
    if($this->is_number_used()){
      $message[] = $this->is_number_used();
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

    /* check if add to database could be successfull  */
    $str = $this->add_to_database();
    if(!empty($str)){
      $message[] = $str;
    }
    return $message;
  }


  /* This function removes the old database entries. 
   * If this entry should be removed or the home server has changed
   *  this function is called to ensure that all old entries will be deleted.
   */
  function remove_from_database($save = false)
  {
    /* Check if we must remove old entries */
    if($this->initially_was_account){

      /* Check if there is at least on server configuration */
      if(!count($this->goFonHomeServers)){
        return( _("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
      }

      /********************
       * Get configuration and check it
       ********************/

      /* Check if databases are reachable, returns an error string if anything fails  */
      $error_str = $this->check_database_accessibility();
      if($error_str){
        return($error_str);
      }

      /* Connect to current database to be able to add new entries */
      $cfg_Current  = $this->goFonHomeServers[$this->init_HomeServer] ;
      $res_cur      =  @mysql_pconnect($cfg_Current['SERVER'],$cfg_Current['LOGIN'],$cfg_Current['PASSWORD']);
      $db_cur       =  @mysql_select_db($cfg_Current['DB'],$res_cur);

      /* Create sql entries */
      $delete[]=    "DELETE FROM ".$cfg_Current['EXT_TABLE']."    WHERE exten='".$this->old_cn."';\n";
      $delete[]=    "DELETE FROM ".$cfg_Current['QUEUE_TABLE']."  WHERE name='".$this->old_cn."'; \n";
      $delete[]=    "DELETE FROM ".$cfg_Current['QUEUE_MEMBER_TABLE']." WHERE queue_name='".$this->old_cn."';\n";
      foreach($this->old_phone_numbers as $number){
        $delete[]=    "DELETE FROM ".$cfg_Current['EXT_TABLE']."    WHERE exten='".$number."';\n";
      }

      /* Execute the queries */  
      if($save){
        foreach($delete as $sql){
          $res = @mysql_query($sql,$res_cur);
          if(!$res){
            new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($r_current));
            return(_("Error while removing old queue entries from database.").
                "&nbsp;"._("Please have a look a the gosa logfiles."));
          }
        }
      }
    }
  }


  /* This function handles the database entries for this 
   *  queue. 
   * Existing entries will be updated if possible.
   */
  function add_to_database($save = false)
  {
    /* Check if there is at least on server configuration */
    if(!count($this->goFonHomeServers)){
      return( _("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
    }

    /********************
     * Get configuration and check it
     ********************/

    /* Check if databases are reachable, returns an error string if anything fails  */
    $error_str = $this->check_database_accessibility();
    if($error_str){
      return($error_str);
    }

    /* Connect to current database to be able to add new entries */
    $cfg_Current  = $this->goFonHomeServers[$this->goFonHomeServer] ;
    $res_cur      =  @mysql_pconnect($cfg_Current['SERVER'],$cfg_Current['LOGIN'],$cfg_Current['PASSWORD']);
    $db_cur       =  @mysql_select_db($cfg_Current['DB'],$res_cur);

    /* Connect to old home server and remove old entries if necessary */
    if(($this->initially_was_account) && ($this->init_HomeServer != $this->goFonHomeServer)){
      $str = $this->remove_from_database($save); 
      if(!empty($str)){
        return($str);;
      }
    }

    /* Ensure that we have the new cn in $this->cn and the old cn in $this->old_cn */
    $this->cn = $this->parent->by_object['ogroup']->cn;

    if($save){
  
      /*****************
       * Create queue table entry 
       *****************/
 
      /* Check if QUEUE_TABLE entry exists.
       * If this entry is missing - create it 
       *  else update the entry with new values.
       */
      $query = "SELECT * FROM ".$cfg_Current['QUEUE_TABLE']."  WHERE name='".$this->old_cn."';";
      $res   = mysql_query($query,$res_cur);
      if(!$res){
        new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($res_cur));
        return(_("Could not detect old queue entry, query failed.")."&nbsp;"._("Please have a look a the gosa logfiles."));
      }
      $cnt = mysql_affected_rows($res_cur);


      /* Create queue table entry 
       * Leave unused options empty. 
       */
      $queue["announce"]              = "";
      $queue["monitor_join"]          = "";
      $queue["monitor_format"]        = "";
      $queue["announce_round_seconds"]= "";   
      $queue["wrapuptime"]            = "";
      $queue["servicelevel"]          = "";
      $queue["eventmemberstatus"]     = "";
      $queue["eventwhencalled"]       = "";
      $queue["memberdelay"]           = "";
      $queue["weight"]                = "";
      $queue["timeoutrestart"]        = "";

      $queue["queue_holdtime"]        = $this->goFonQueueAnnounce;
      $queue["queue_lessthan"]        = $this->goFonQueueLessThan;   
      $queue["retry"]                 = $this->goFonQueueRetry;
      $queue["reportholdtime"]        = "1";
      $queue["joinempty"]             = "no";
      $queue["leavewhenempty"]        = "yes";   

      $queue["context"]               = "default";
      $queue["name"]                  = $this->cn;  
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


      /* Check if we must create a new queue entry 
       *  or if we can update an existing entry 
       *  $cnt contains the number of entries matching this cn
       */

      /* Create new queue table entry 
       */
      if($cnt == 0){

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
        $SQL[]="INSERT INTO ".$cfg_Current['QUEUE_TABLE']." (".$entries.") VALUES (".$values.");";
      }elseif($cnt == 1){

        /* Update queue table entry 
         */
        $queue_old = @mysql_fetch_assoc($res);
        foreach($queue_old as $name => $value){
          if(isset($queue[$name]) && $queue[$name] == $value){
            unset($queue[$name]);
          }
        }

        /* Parse and Add Queue */
        if(count($queue)){
          $query = "UPDATE ".$cfg_Current['QUEUE_TABLE']." SET ";
          foreach($queue as $key => $val){
            $query.= "".$key."='".$val."',";
          }
          $query = preg_replace("/,$/","",$query);
          $query.= " WHERE name='".$this->old_cn."';";
          $SQL[] = $query;
        }
      }else{
        return(sprintf(_("More than one entry in queue table found, that uses the name ('%s'). Please fix this issue manually first."),$this->cn));
      }


      /*****************
       * Create queue member entries
       *****************/
      
      /* Add the queue member entries 
       * First we must remove all old user entries. 
       * to be able to add a clean set of members.
       */ 
      $SQL[]= "DELETE FROM ".$cfg_Current['QUEUE_MEMBER_TABLE']." WHERE queue_name='".$this->cn."';";
      $SQL[]= "DELETE FROM ".$cfg_Current['QUEUE_MEMBER_TABLE']." WHERE queue_name='".$this->old_cn."';";
      
      /* Append new Member for this queue */ 
      $queueuser =array();
      $i = 0;
      $parent = $this->parent->by_object['ogroup'];

      
      $ldap = $this->config->get_ldap_link();
      foreach($parent->memberList as $member => $mem_data){
        $ldap->cat($member,array("goFonHomeServer","objectClass","dn","uid"));
        if($ldap->count()){

          $obj = $ldap->fetch();

          /* Calculate server and account dependencies */
          $is_acc = in_array("goFonAccount",$obj['objectClass']);
          $is_home= isset($obj['goFonHomeServer'][0]) && $obj['goFonHomeServer'][0] == $this->goFonHomeServer; 

          /* Append user to list of queue member,
           *  only if user has phoneAccount extension && is on same home server */
          if($is_acc && $is_home){
            $i ++ ;
            $queueuser[$i]['queue_name']  = $this->cn; 
            $queueuser[$i]['interface']   = "SIP/".$obj['uid'][0]; 
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

          $SQL[]="INSERT INTO ".$cfg_Current['QUEUE_MEMBER_TABLE']." (".$entries.") VALUES (".$values.")"; 
        }
      }
      

      /*****************
       * Create extension entries
       *****************/
      
      /* Add the extension entries 
       * First we must remove all old entensions. 
       */ 
      $SQL[]=  "DELETE FROM ".$cfg_Current['EXT_TABLE']."  WHERE exten='".$this->cn."';\n";
      $SQL[]=  "DELETE FROM ".$cfg_Current['EXT_TABLE']."  WHERE exten='".$this->old_cn."';\n";

      /* Delete old enxtension entries for the old telephone nubmer  */
      if(is_array($this->old_phone_numbers)){
        foreach($this->old_phone_numbers as $phone){
          $SQL[]= "DELETE FROM ".$cfg_Current['EXT_TABLE']." WHERE exten='".$phone."';\n";
        }
      }
      
      /* Delete enxtension entries for telephone numbers  */
      if(is_array($this->telephoneNumber)){
        foreach($this->telephoneNumber as $phone){
          $SQL[]= "DELETE FROM ".$cfg_Current['EXT_TABLE']." WHERE exten='".$phone."';\n";
        }
      }

      /* Create a extension entry fpr each telephoneNumber */
      $i_insert_only_once = false;
      $prio = 11;   // This represents the priority for each telephoneNumber 
      foreach($this->telephoneNumber as $num){

        /* The naming refrences */
        if($i_insert_only_once == false){
          $i_insert_only_once = true;
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $this->cn;
          $a_ext[$i]['priority'] = 1;
          $a_ext[$i]['app']      = "Goto";
          $a_ext[$i]['appdata']  = $num."|1";
          $i ++ ; 
        }

        /* If there is currently no user for this queue 
         * Play no service sound file and return to default context.  
         */
        if(count($queueuser)==0){
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

          /* Dcrement priority to avoid using same priority twice */
          $prio --;

          /* Wait for 2 seconds */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 1;
          $a_ext[$i]['app']      = "Wait";
          $a_ext[$i]['appdata']  = "2";
          $i ++ ; 
    
          /* Set language to queue language */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 2;
          $a_ext[$i]['app']      = "SetLanguage";
          $a_ext[$i]['appdata']  = $this->goFonQueueLanguage;
          $i ++ ; 
  
          /* Play welcome sound file */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 3;
          $a_ext[$i]['app']      = "Playback";
          $a_ext[$i]['appdata']  = $this->goFonWelcomeMusic;
          $i ++ ; 

          /* Set CID name */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 4;
          $a_ext[$i]['app']      = "SetCIDName";
          if(!empty($this->parent->by_object['ogroup']->description)){
            $a_ext[$i]['appdata']  = $this->parent->by_object['ogroup']->description;
          }else{
            $a_ext[$i]['appdata']  = $this->cn." - ".$num;
          }
          $i ++ ; 

          /* Set queue priority */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 5;
          $a_ext[$i]['app']      = "SetVar";
          $a_ext[$i]['appdata']  = "QUEUE_PRIO=".$prio;
          $i ++ ; 

          /* Open queue */
          $a_ext[$i]['context']  = 'GOsa';
          $a_ext[$i]['exten']    = $num;
          $a_ext[$i]['priority'] = 6;
          $a_ext[$i]['app']      = "Queue";
          $a_ext[$i]['appdata']  =  $this->cn;
            "|".
            $this->goFonDialOptiont.
            $this->goFonDialOptionT.
            $this->goFonDialOptionh.
            $this->goFonDialOptionH.
            $this->goFonDialOptionr;
        }

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
        $SQL[]="INSERT INTO ".$cfg_Current['EXT_TABLE']." (".$entries.") VALUES (".$values.")";
      }

      /* Do all collected mysql queries 
       */
      foreach($SQL as $query)
      $res   = mysql_query($query,$res_cur);
      if(!$res){
        new log("debug","ogroups/".get_class($this),"",array(),@mysql_error($res_cur));
        return(_("Mysql query failed.")."&nbsp;"._("Please have a look a the gosa logfiles."));
      }
    }
    @mysql_close($r_con);
    return(false);
  }


  /* This function checks if the given phonenumbers 
   *  are available or already in use
   */
  function is_number_used()
  {
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(& (!(uid=".$this->cn."))
                        (!(cn=".$this->cn.")))
                     (| (objectClass=goFonAccount)
                        (objectClass=goFonQueue)
                        (objectClass=goFonConference)))", array("telephoneNumber","cn","uid"));
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


  /* Get posted attributes */
  function save_object()
  {
    plugin::save_object();  
    if(isset($_POST['phonenumber'])){
      foreach(array("goFonDialOptiont","goFonDialOptionT","goFonDialOptionh","goFonDialOptionr","goFonDialOptionH","goFonMusiconHold") as $val){
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
    $ldap= $this->config->get_ldap_link();

    plugin::save();

    /* Create dial option attribute */
    $this->attrs['goFonDialOption'] = "";
    foreach(array("goFonDialOptiont","goFonDialOptionT","goFonDialOptionr","goFonDialOptionh","goFonDialOptionH","cn") as $val){
      $this->attrs['goFonDialOption'].=$this->$val; 
      unset($this->attrs[$val]); 
    }
    if(empty($this->attrs['goFonDialOption'])) {
      $this->attrs['goFonDialOption']=array();
    }

    /* Set announce hold time to yes no .. */
    if($this->goFonQueueAnnounceHoldtime != "no" ){
      $this->attrs['goFonQueueAnnounceHoldtime'] = "yes";
    }else{
      $this->attrs['goFonQueueAnnounceHoldtime'] = "no";
    }

    /* Set strategy */
    $this->attrs['goFonQueueStrategy'] = $this->goFonQueueStrategyOptionsR[$this->goFonQueueStrategy];

    /* Add database entry, display error and abort if this fails */
    $str = $this->add_to_database(true);
    if(!empty($str)){
      print_red($str);
    }

    /* Save data to LDAP */
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("modify","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

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
    /* Cancel if nothing is to do here */
    if (!$this->initially_was_account){
      return;
    }

    /* Remove database entries, 
     *  if fails display errors and abort
     */
    $str = $this->remove_from_database(true);
    if(!empty($str)){
      print_red($str);
      return false;
    }
  
    /* Remove all temporary attributes */
    $tmp = array_flip($this->attributes);
    foreach(array("goFonDialOptiont","goFonDialOptionT","goFonDialOptionr","goFonDialOptionh","goFonDialOptionH","cn") as $val){
      unset($this->$val);
      unset($this->attrs[$val]);
      unset($tmp[$val]);
    }   
    foreach(array_flip($tmp) as $key => $val){
      $tmp2[]=$val;
    } 
    $this->attributes = $tmp2;

    /* include global link_info */
    $ldap= $this->config->get_ldap_link();

    /* Remove and write to LDAP */
    plugin::remove_from_parent();

    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,$this->attributes, "Save");
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 
    new log("remove","ogroups/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
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


  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Phone"),
          "plDescription" => _("Phone group"),
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 3,
          "plSection"     => array("administration"),
          "plCategory"    => array("ogroups"),
          "plProvidedAcls"=> array(

            "goFonTimeOut"              => _("Timeout"),
            "goFonMaxLen"               => _("Max queue length"),
            "goFonHomeServer"           => _("Home server"),
            "goFonAnnounceFrequency"    => _("Announce frequency"),
            "goFonDialOptiont"          => _("Allow the called user to transfer his call"),
            "goFonDialOptionT"          => _("Allows calling user to transfer call"),
            "goFonDialOptionh"          => _("Allow the called to hangup by pressing *"),
            "goFonDialOptionr"          => _("Ring instead of playing background music"),
            "goFonDialOptionH"          => _("Allows calling to hangup by pressing *"),

            "goFonMusiconHold"          => _("Music on hold"),
            "goFonWelcomeMusic"         => _("Welcome music"),
            "goFonQueueReportHold"      => _("Report hold time"),
            "goFonQueueYouAreNext"      => _("'You are next' sound"),
            "goFonQueueThereAre"        => _("'There are' sound"),
            "goFonQueueCallsWaiting"    => _("'Call waiting' sound"),
            "goFonQueueThankYou"        => _("'Thank you' sound"),
            "goFonQueueMinutes"         => _("'Minutes' sound"),
            "goFonQueueSeconds"         => _("'Seconds' sound"),
            "goFonQueueLessThan"        => _("'Less than' sound"),
            "telephoneNumber"           => _("Queue phone number"),
            "goFonQueueLanguage"        => _("Language"),
            "goFonQueueStrategy"        => _("Method"),
            "goFonQueueAnnounceHoldtime"=> _("Announce holdtime"),
            "goFonQueueAnnounce"        => _("Announce"),
            "goFonQueueRetry"           => _("Retry"))
              ));
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
