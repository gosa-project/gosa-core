<?php

class phoneAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone";
  var $plDescription= "This does something";
  var $has_mailAccount= FALSE;

  /* Attributes */
  var $telephoneNumber        = array();
  var $goFonHardware          = "automatic";
  var $goFonFormat            = "wav";
  var $goFonPIN               = "";
  var $goFonVoicemailPIN      = "";
  var $goFonDeliveryMode      = "[]";
  var $phoneNumbers           = array();
  var $mail                   = "";
  var $hardware_list          = array();
  var $used_hardware          = array();
  var $goFonMacro             = "";
  var $macro                  = 0;              // Selected Macroi
  var $lastmacro              = "";
  var $macros                 = array();        // List of macros for smarty select box
  var $macroarray             = array();        // All needed macro informations
  var $macrostillavailable    = false;
  var $generate_error         = "";
  var $a_old_telenums         = array();
  var $goFonPINVoice          = "";
  var $goFonHomeServer        = "0";            // Contains the dn of the server that manage this account 
  var $init_HomeServer        = "0";            // Contains the dn of the server that manage this account 
  var $goFonHomeServers       = array();        // Contains all available server configurations 
    
  /* attribute list for save action */
  var $CopyPasteVars          = array("phoneNumbers","macroarray","macrostillavailable"/*"phoneNumbers" -Reset- */,
                                      "hardware_list","used_hardware");

  var $attributes             = array("goFonDeliveryMode", "goFonFormat","uid","cn","goFonHomeServer",
      "goFonHardware","goFonPIN","goFonVoicemailPIN","telephoneNumber", "goFonMacro","macro");
  var $objectclasses= array("goFonAccount");

  var $uid;

  function phoneAccount ($config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn, $parent);

    /* Assemble phone numbers */
    if (isset($this->attrs['telephoneNumber'])){
      for ($i= 0; $i<$this->attrs['telephoneNumber']['count']; $i++){
        $number= $this->attrs['telephoneNumber'][$i];
        $this->phoneNumbers[$number]= $number;
      }
    }

    /* Set up has_mailAccount */
    if (isset($this->attrs['objectClass'])){
      if (in_array("gosaMailAccount", $this->attrs['objectClass'])){
        $this->has_mailAccount= TRUE;
      }
    }

    /* Set uid */
    if((isset($this->parent->by_object['user']->uid))&&(!empty($this->parent->by_object['user']->uid))){
      $this->uid = $this->parent->by_object['user']->uid;
    }
    if((isset($this->parent->by_object['user']->cn))&&(!empty($this->parent->by_object['user']->cn))){
      $this->cn  = $this->parent->by_object['user']->cn;
    }

    /* Check server configurations 
     * Load all server configuration in $this->goFonHomeServers if available
     *  and first server as default if necessary.
     * Check if connection is successfull for the selected server $this->goFonHomeServer
     */

    /* Set available server */
    if(isset($_SESSION['config']->data['SERVERS']['FON'])){
      $this->goFonHomeServers = $_SESSION['config']->data['SERVERS']['FON'];
    }

    $a_SETUP= array();
    if($this->is_account &&
       array_key_exists('config',$_SESSION) &&
       array_key_exists('SERVERS',$_SESSION['config']->data) &&
       array_key_exists('FON',$_SESSION['config']->data['SERVERS']) &&
       is_callable("mysql_connect")
       ) {

      /* Servers defined? Watch here... */
      if (count($this->goFonHomeServers)){

        /* Set default server */
        if(empty($this->goFonHomeServer) || $this->goFonHomeServer == "0"){
          $this->goFonHomeServer= $this->goFonHomeServers[0]['DN'];
        }

        /* Remember inital home server, to be able to remove old entries */
        $this->init_HomeServer = $this->goFonHomeServer;

        /* Get config */
        if(!isset($this->goFonHomeServers[$this->goFonHomeServer])){
          print_red(sprintf(_("The specified goFonHomeServer '%s' is not available in GOsa server configuration. Saving this account will create a new entry on the server '%s'. Use cancel if you do not want to create a new entry ignoring old accounts."),$this->goFonHomeServer, $this->goFonHomeServers[0]['DN']));

          $this->goFonHomeServer = $this->goFonHomeServers[0]['DN'];
          $this->init_HomeServer = $this->goFonHomeServers[0]['DN'];
        }    
        $cur_cfg = $this->goFonHomeServers[$this->goFonHomeServer];

        $r_con =  @mysql_pconnect($cur_cfg['SERVER'],$cur_cfg['LOGIN'],$cur_cfg['PASSWORD']);
        if(!$r_con){
#          print_red( sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
#                $cur_cfg['SERVER'],$cur_cfg['LOGIN']));
          gosa_log(mysql_error());
        }
        $db  =  @mysql_select_db($cur_cfg['DB'],$r_con);
        if(!$db){
#         print_red(sprintf(_("Can't select database %s on %s."),$cur_cfg['DB'],$cur_cfg['SERVER']));
          gosa_log(mysql_error());
        }

        $first = false; 
        foreach($this->phoneNumbers as $key => $val){
          if(!$first){
            $first = $key;
          }
        }
      }
    }

    /* Get available phone hardware  
     * Search for all available phone hardware  
     */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(objectClass=goFonHardware)", array('cn', 'description'));
    while ($attrs= $ldap->fetch()){
      $cn= $attrs['cn'][0];
      if (isset($attrs['description'])){
        $description= " - ".$attrs['description'][0];
      } else {
        $description= "";
      }
      $this->hardware_list[$cn]= "$cn$description";
    }


    /* Get available Macros  
     * Search for all Marcos that are visible and create 
     *  an array with name and parameters 
     */
    $ldap->search("(&(objectClass=goFonMacro)(goFonMacroVisible=1))", array("*"));

    /* Add none for no macro*/
    $this->macros['none']=_("no macro");    
    $this->macro ="none";

    /* Fetch all Macros*/
    while ($attrs= $ldap->fetch()){

      /* unset Count, we don't need that here */
      unset($attrs['displayName']['count']);

      /* Parse macro data, unset count for parameterarrays  */
      if (isset($attrs['goFonMacroParameter']['count'])){
        unset($attrs['goFonMacroParameter']['count']);
      }

      /* fill Selectfield variable with Macros */
      if(isset($attrs['displayName'][0])){
        $this->macros[$attrs['dn']] = $attrs['displayName'][0]." (".$attrs['cn'][0].")";
      }else{
        $this->macros[$attrs['dn']] = _("undefined");
      }

      /* Go through available parameters and parse all attributes, like parametername, type, default ...*/
      if((isset($attrs['goFonMacroParameter']))&&(is_array($attrs['goFonMacroParameter']))){

        foreach($attrs['goFonMacroParameter'] as $pkey=>$pval){
          /* Split Data in readable values, by delimiter !  */
          $data = split("!",$attrs['goFonMacroParameter'][$pkey]);

          /* Set all attrs */
          $id = $data[0];
          $this->macroarray[$attrs['dn']][$id]['var']    ="var".$id;
          $this->macroarray[$attrs['dn']][$id]['choosen']=$data[3]; 
          $this->macroarray[$attrs['dn']][$id]['id']     =$id;
          $this->macroarray[$attrs['dn']][$id]['name']   =$data[1];
          $this->macroarray[$attrs['dn']][$id]['type']   =$data[2];
          $this->macroarray[$attrs['dn']][$id]['default']=$data[3];
          if($data[2] == "bool"){
            $this->macroarray[$attrs['dn']][$id]['choosen']=$data[3];
          }
        }//foreach
      }//is_array
    }//while


    /* Parse used Macro  
     * If we have a macro selected, parse it and set values 
     *  in $this->macroarray[$this->macro]. 
     */
    $tmp = split("!",$this->goFonMacro);
    if(is_array($tmp)){

      /* First value is the macroname */
      $this->macro = $tmp[0];

      /* Macroname saved, delete that index */
      unset($tmp[0]);

      /* Check if makro has been removed */
      if(!isset($this->macros[$this->macro])){
        $this->macrostillavailable = false;
      }else{
        $this->macrostillavailable = true;
      }

      /* for each parametervalues ( parameterID#value like 25#twentyfive) */
      foreach($tmp as $var){

        /* Split this, so we have $varar[0] = parameterID $varar[1] = SelectedValue */
        $varar = split("#",$var);

        /* Only insert if the parameter still exists */
        if(isset($this->macroarray[$this->macro][$varar[0]])){
          /* Assign value */
          $this->macroarray[$this->macro][$varar[0]]['choosen']=$varar[1];
        }
      }
    }


    /* Colorize phones 
     * Used phones will be colored in grey, 
     *  so we must detect which phones are currently in use.
     */
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(goFonHardware=*)",array('cn','dn','goFonHardware'));
    while($attrs = $ldap->fetch()){
        $cn = $attrs['goFonHardware'][0];
        if(isset($this->hardware_list[$cn])){
          $this->used_hardware[$cn]= $cn;
        }
    }
    $this->hardware_list["automatic"]= _("automatic");
    ksort($this->hardware_list);
    $this->a_old_telenums = $this->phoneNumbers;

    /* Get voicemail PIN from MySQL DB 
     * Because every user can change his PIN directly from the phone
     *  without any update to the ldap
     * This means, the PIN in the DB is up to date
     */
    // Connect to DB server
    if((is_callable("mysql_pconnect"))&&(isset($cur_cfg))&&(isset($cur_cfg['SERVER']))&&(isset($cur_cfg['LOGIN']))&&(isset($cur_cfg['PASSWORD']))){
      $r_con =  @mysql_pconnect($cur_cfg['SERVER'],$cur_cfg['LOGIN'],$cur_cfg['PASSWORD']);
      if($r_con){
        $r_db  =  @mysql_select_db($cur_cfg['DB'],$r_con);
        $vp = mysql_fetch_row(mysql_query("SELECT ".$cur_cfg['VOICE_TABLE'].".password FROM  ".$cur_cfg['VOICE_TABLE'].", ".$cur_cfg['SIP_TABLE']."  WHERE customer_id = sip_users.mailbox AND name='".$this->uid."'"));

        if((isset($vp[0]))&&(!empty($vp[0]))){
          $this->goFonPINVoice = $vp[0];
        }
      }
    }
    $this->lastmacro=$this->macro;

    if(is_callable("mysql_close")&&(isset($r_con))&&($r_con)){
      @mysql_close($r_con) ;
    }
  }


  /* This function generates the Database entries. 
   * The Parameter 'save' could be true or false.
   *  false - means only testing no database transactions.
   *  true  - write database entries.
   *
   * 'sip_users','voice_mail' and 'extensions' table entries will be created.
   * 
   * If the phone hardware is 'automatic' the table entries will only be removed
   *  and not added. 
   */
  function generate_mysql_entension_entries($save = false)
  {
    /* Check if there is at least one server available 
     * If not, return and tell the user that saving failed 
     */
    if(!count($this->goFonHomeServers)){
      if($save){
        print_red(_("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
      }
      return(true);
    }

    /* Check if Mysql extension is available */
    if(!is_callable("mysql_pconnect")){
      if($save)
      print_red(_("Can't save any changes to asterisk database, there is no mysql extension available."));
      return(true);
    }
 
    /********************** 
     * Attribute Initialisation
     **********************/

    $old_connection = false;

    // Get Configuration for Mysql database Server
    $s_parameter    = "";                                           // Contains paramter for selected Macro 
    $r_con          = false;                                        // DB connection
    $r_db           = false;                                        // Selected DB
    $r_res          = false;                                        // Result resource
    $a_ldap_attrs   = array();                                      //  

    $s_ip           = NULL;                   // Contains ip for Sip entry
    $s_host         = NULL;                   // Contains host for Sip entry
    $s_qualify      = "yes";                  // Qualify entry
    $s_pin          = NULL;                   // Entry for secret
    $s_type         = NULL;                   // Entry for phone type (friend , peer ..)

    $sip_data_array = array();                // Contains complete sip entry, to generate SQL syntax
    $i_old_key      = false;                  // Contains index for first old phonenumber, to delete old entries corectly
    $i_new_key      = false;                  // Contains index for first new phonenumber, to generate new  entries corectly

    $s_sip_values   = "";     // Contains string with all values for given attributes in SQL syntax
    $s_sip_keys     = "";     // Contains all needed attributes to generate sip entry in DB

    $s_sip_key      = "";     // Key for SIP entry index      
    $s_sip_val      = "";     // Value for SIP entry index      

    $b_first_deleted= false;  // Only delete first entry, 
    $s_telenums     = "";     // for each value variable

    $i_is_accounted = false;  // Ensure that extension entry, for name to number is only once in table

    /* Prepare some basic attributes */
    foreach($this->a_old_telenums as $tele){
      $oldnums[]= preg_replace("/[^0-9]/","",$tele);
    }
    foreach($this->phoneNumbers as $tele){
      $newnums[]= preg_replace("/[^0-9]/","",$tele);
    }

    /* If deletion starts from userslist, cn uid are not set */
    if((isset($this->parent->by_object['user']->uid))&&(!empty($this->parent->by_object['user']->uid))){
      $this->uid = $this->parent->by_object['user']->uid;
    }
    if((isset($this->parent->by_object['user']->cn))&&(!empty($this->parent->by_object['user']->cn))){
      $this->cn  = $this->parent->by_object['user']->cn;
    }
    /* Create voicemail entry 
     */
    if((!isset($this->cn))||(empty($this->cn))){
      $CNname= $this->uid;
    }else{
      $CNname= $this->cn;
    }

    if((isset($this->parent))&&(isset($this->parent->by_object['mailAccount']))&&($this->parent->by_object['mailAccount']->is_account==true)){
      $s_mail = $this->parent->by_object['mailAccount']->mail;
    }else{
      $s_mail = "";
    }
    /* Get phonehardware to setup sip entry  */
    $ldap         = $this->config->get_ldap_link();
    $r_res        = $ldap->search("(&(objectClass=goFonHardware)(cn=".$this->goFonHardware."))", array('*'));
    $a_ldap_attrs = $ldap->fetch();

    /* Check selected phone hardware, is a default IP set? */
    if(((isset($a_ldap_attrs['goFonDefaultIP'][0]))&&($a_ldap_attrs['goFonDefaultIP'][0] != "dynamic"))){
      $s_ip       = $a_ldap_attrs['goFonDefaultIP'][0];
      $s_host     = $s_ip;
    }else{
      $s_ip       = NULL;
      $s_host     = "dynamic";
    }

    // Attribute GoFonQualify set ?
    if(isset($a_ldap_attrs['goFonQualify'])){
      $s_qualify = $a_ldap_attrs['goFonQualify'][0];
    }

    // Attribute GoFonPIN set ?
    if(isset($this->goFonPIN)){
      $s_pin      = $this->goFonPIN;
    }

    // Attribute GoFonType set ?
    if(isset($a_ldap_attrs['goFonType'])){
      $s_type = $a_ldap_attrs['goFonType'][0];
    }

    if(isset($a_ldap_attrs['goFonDmtfMode'][0])){
      $sip_data_array['dtmfmode']     = $a_ldap_attrs['goFonDmtfMode'][0];
    }else{
      $sip_data_array['dtmfmode']     ="rfc2833";
    }

    /* Check if phone number is used */
    if($this->is_number_used()){
      $this->generate_error = $this->is_number_used(); 
      return false;
    }



    /********************** 
     * Check Server Connection Information
     **********************/
 
    /* Create Mysql handle for the current goFonHomeServer, if possible  
     * Get configuration to old asterisk home server 
     */ 
    $a_New = $this->goFonHomeServers[$this->goFonHomeServer];  // DB Configuration
    $new_connection =  @mysql_pconnect($a_New['SERVER'],$a_New['LOGIN'],$a_New['PASSWORD']);
    if(!$new_connection){
      $this->generate_error = sprintf(
          _("The MySQL Server '%s' isn't reachable as user '%s'. Abort saving entries to keep the database consistent, check GOsa log for mysql error."),
          $a_New['SERVER'],$a_New['LOGIN']);
      gosa_log(@mysql_error($new_connection));
      return false;
    }
    $new_database  =  @mysql_select_db($a_New['DB'],$new_connection);
    if(!$new_database){
      $this->generate_error = sprintf(
          _("Can't select database %s on %s. Abort saving entries to keep the database consistent, check GOsa log for mysql error."),
          $a_New['DB'],$a_New['SERVER']);
      gosa_log( @mysql_error($new_connection));
      return false;
    }

    /* If the home server has changed, we must remove entries from old 
     *  server and add new entries in new server.  
     */
    if($this->init_HomeServer != $this->goFonHomeServer){
    
      /* Get configuration to old asterisk home server */ 
      $a_Remove = $this->goFonHomeServers[$this->init_HomeServer];  // DB Configuration
 
      /* Create connection to the database that contains the old entry. 
       */
      $old_connection =  @mysql_pconnect($a_Remove['SERVER'],$a_Remove['LOGIN'],$a_Remove['PASSWORD']);
      if(!$old_connection){
        $this->generate_error = sprintf(
            _("The old MySQL home server '%s' isn't reachable as user '%s'. Abort saving entries to keep the database consistent, check GOsa log for mysql error."),
            $a_Remove['SERVER'],$a_Remove['LOGIN']);
        gosa_log(@mysql_error($old_connection));
        return false;
      }
      $old_database  =  @mysql_select_db($a_Remove['DB'],$old_connection);
      if(!$old_database){
        $this->generate_error = sprintf(
            _("Can't select database %s on %s. Abort saving entries to keep the database consistent, check GOsa log for mysql error."),
            $a_Remove['DB'],$a_Remove['SERVER']);
        gosa_log(@mysql_error($old_connection));
        return false;
      }
    }

    /* Save means that we must save changes, not only test  */
    if($save == true){
    
      /********************** 
       * Remove entries from old home server 
       **********************/

      /* Check if there is an old entry 
       * If there is an old entry, get callerid and remove voicemail and extensions 
       */
      if($old_connection){
        $query  = "SELECT id,name,callerid FROM ".$a_Remove['SIP_TABLE']." WHERE name='".$this->uid."';";
        $rid    = mysql_query($query,$old_connection);

        /* Old entry found, remove it */
        $query_a = array();
        if(mysql_affected_rows($old_connection)){
          $result = mysql_fetch_assoc($rid);
          $query_a[]= "DELETE FROM ".$a_Remove['SIP_TABLE']." WHERE name='".$this->uid."';";
          $query_a[]= "DELETE FROM ".$a_Remove['VOICE_TABLE']." WHERE customer_id='".$result['callerid']."';";
          $query_a[]= "DELETE FROM ".$a_Remove['EXT_TABLE']." WHERE exten='".$this->uid."';";
          foreach($oldnums as $s_telenums) {
            $query_a[]= "DELETE FROM ".$a_Remove['EXT_TABLE']." WHERE exten='".$s_telenums."';";
          }

          foreach($query_a as $qry){
            if(!mysql_query($qry,$old_connection)){
              echo mysql_error($old_connection);
            } 
          }
        }
      }

      /********************** 
       * Update / Insert sip_users entry  
       **********************/

      /* Set the first given phone number as callerid */
      reset($newnums);        
      $i_new_key = key($newnums);
      $sip_data_array['callerid']  =$newnums[$i_new_key];
      $sip_data_array['mailbox']   =$newnums[$i_new_key];

      /* Check if there is already an entry in sip_users for this uid */
      $SQL_query_array = array();
      $rid = mysql_query("SELECT * FROM ".$a_New['SIP_TABLE']." WHERE name='".$this->uid."';\n",$new_connection);
      if(mysql_affected_rows($new_connection)){

        /********************** 
         * Update sip_users entry 
         **********************/
        $result                     = mysql_fetch_assoc($rid);
        $sip_data_array['host']         = $s_host;
        $sip_data_array['qualify']      = $s_qualify;
        $sip_data_array['secret']       = $this->goFonPIN;
        $sip_data_array['type']         = $s_type ;
        $sip_data_array['username']     = $this->uid;
        $sip_data_array['ipaddr']       = $s_ip;

        /* Remove not changed attributes, to avoid updating table with same values */
        foreach($sip_data_array as $name => $value){
          if($result[$name] == $value){
            unset($sip_data_array[$name]);
          }
        }
        /* Only update entry if there is something to uopdate */
        if(count($sip_data_array)){
          $query = "UPDATE ".$a_New['SIP_TABLE']." SET ";
          foreach($sip_data_array as $key => $val){
            $query.= "".$key."='".$val."',"; 
          } 
          $query = preg_replace("/,$/","",$query);
          $query.= " WHERE name='".$this->uid."';";
          $SQL_query_array[] = $query;
        }
      } else {
 
        /********************** 
         * Insert sip_users entry 
         **********************/
        //generate SIP entry
        $sip_data_array['id']           = "";
        $sip_data_array['name']         = $this->uid;
        $sip_data_array['accountcode']  = NULL;          
        $sip_data_array['amaflags']     = NULL;
        $sip_data_array['callgroup']    = NULL;
        $sip_data_array['canreinvite']  = "no";
        $sip_data_array['context']      = "default";
        $sip_data_array['defaultip']    = NULL;
        $sip_data_array['fromuser']     = NULL;
        $sip_data_array['fromdomain']   = NULL;
        $sip_data_array['host']         = $s_host;
        $sip_data_array['insecure']     = NULL;
        $sip_data_array['language']     = NULL;
        $sip_data_array['mailbox']      = $newnums[$i_new_key];
        $sip_data_array['md5secret']    = NULL;
        $sip_data_array['nat']          = "no";
        $sip_data_array['permit']       = NULL;
        $sip_data_array['deny']         = NULL;
        $sip_data_array['mask']         = NULL;
        $sip_data_array['pickupgroup']  = NULL;
        $sip_data_array['port']         = NULL;
        $sip_data_array['qualify']      = $s_qualify;
        $sip_data_array['restrictcid']  = "n";
        $sip_data_array['rtptimeout']   = NULL;
        $sip_data_array['rtpholdtimeout']=NULL;
        $sip_data_array['secret']       = $this->goFonPIN;
        $sip_data_array['type']         = $s_type ;
        $sip_data_array['username']     = $this->uid;
        $sip_data_array['disallow']     = NULL;
        $sip_data_array['allow']        = NULL;
        $sip_data_array['musiconhold']  = NULL;
        $sip_data_array['regseconds']   = NULL;
        $sip_data_array['ipaddr']       = $s_ip;
        $sip_data_array['regexten']     = NULL;
        $sip_data_array['cancallforward']=NULL;

        /* There is currently no entry for this user in the sip_users table. 
         * We should create one i
         */
        foreach($sip_data_array as $s_sip_key=>$s_sip_val){
          if($s_sip_val == NULL) continue;
          $s_sip_values.="'".$s_sip_val."',";
          $s_sip_keys  .="`".$s_sip_key."`,";
        }
        $s_sip_values =  preg_replace("/,$/","",$s_sip_values);
        $s_sip_keys   =  preg_replace("/,$/","",$s_sip_keys);

        /* Add sip entries to mysql queries */
        $SQL_query_array[] ="INSERT INTO ".$a_New['SIP_TABLE']." (".$s_sip_keys.") VALUES (".$s_sip_values.");";
      }


      /********************** 
       * Update / Insert Voice mail entry  
       **********************/

      $customer_id = $newnums[$i_new_key];

      $voice_data_array = array(
          "customer_id" => $customer_id,
          "mailbox"     => $customer_id,
          "password"    => $this->goFonVoicemailPIN,
          "fullname"    => $CNname,
          "email"       => $s_mail);

      /* Set pager number if available */
      if(isset($this->parent->by_object['user']->pager)){
        $voice_data_array['pager']   = $this->parent->by_object['user']->pager;
      }

      /* Check if there is already an entry in sip_users for this uid */
      $rid = mysql_query("SELECT * FROM ".$a_New['VOICE_TABLE']." WHERE customer_id='".$customer_id."';\n",$new_connection);
      if(mysql_affected_rows($new_connection)){

        /********************** 
         * Update Voice mail entry  
         **********************/
        $result = mysql_fetch_assoc($rid)  ;

        foreach($voice_data_array as $name => $value){
          if($result[$name] == $value){
            unset($voice_data_array[$name]);
          }
        }

        /* Only update entry if there is something to uopdate */
        if(count($voice_data_array)){
          $query = "UPDATE ".$a_New['VOICE_TABLE']." SET ";
          foreach($voice_data_array as $key => $val){
            $query.= "".$key."='".$val."',"; 
          } 
          $query = preg_replace("/,$/","",$query);
          $query.= " WHERE customer_id='".$customer_id."';";
          $SQL_query_array[] = $query;
        }
      }else{

        /********************** 
         * Insert Voice mail entry  
         **********************/
        $voice_data_array['context'] = "default";
  
        /* There is currently no voice mail entry for this user. 
         * We should create one 
         */
        $s_voi_values = $s_voi_keys = "";
        foreach($voice_data_array as $s_voi_key=>$s_voi_val){
          if($s_voi_val == NULL) continue;
          $s_voi_values.="'".$s_voi_val."',";
          $s_voi_keys  .="`".$s_voi_key."`,";
        }
        $s_voi_values =  preg_replace("/,$/","",$s_voi_values);
        $s_voi_keys   =  preg_replace("/,$/","",$s_voi_keys);

        /* Add sip entries to mysql queries */
        $SQL_query_array[] ="INSERT INTO ".$a_New['VOICE_TABLE']." (".$s_voi_keys.") VALUES (".$s_voi_values.");";
      }

     
      /********************** 
       * Remove/Insert extension entries
       **********************/
      
      /* Remove old entries */
      $query = array();
      $query[]= "DELETE FROM ".$a_New['EXT_TABLE']." WHERE exten=\"".$this->uid."\";";
      foreach($oldnums as $s_telenums){
        $query[]= "DELETE FROM ".$a_New['EXT_TABLE']." WHERE exten=\"".$s_telenums."\";";
      }
      foreach($newnums as $s_telenums){
        $query[]= "DELETE FROM ".$a_New['EXT_TABLE']." WHERE exten=\"".$s_telenums."\";";
      }
      foreach($query as $qry){
        if(!mysql_query($qry,$new_connection)){
          echo mysql_error($new_connection);
        } 
      }
 
      /********************** 
       * Insert extension entries
       **********************/
 
      // Get selected Macro Parameter and create parameter entry 
      if(isset($this->macroarray[$this->macro])){
        foreach($this->macroarray[$this->macro] as $key => $val ){
          $s_parameter .= $val['choosen']."|";
        }
        $s_parameter = preg_replace("/\|$/","",$s_parameter);
      }
     
      $i = 0; 
      $EXT = array();
    
      if(!is_numeric($this->uid)){
        $EXT[$i]['context'] = 'GOsa';
        $EXT[$i]['exten']   = $this->uid;
        $EXT[$i]['priority']= 1;
        $EXT[$i]['app']     = "Goto";
        $EXT[$i]['appdata'] = $newnums[$i_new_key]."|1";
        $i ++;
      }

      // Entension entries  Hint / Dial / Goto
      foreach($newnums as $s_telenums){

        /* Hint Entry */
        $EXT[$i]['context'] = 'GOsa';
        $EXT[$i]['exten']   = $s_telenums;
        $EXT[$i]['priority']= "Hint";
        $EXT[$i]['app']     = 'SIP/'.$this->uid;
        $i ++;  
        /* SetCID */
        //$EXT[$i]['context'] = 'GOsa';
        //$EXT[$i]['exten']   = $s_telenums;
        //$EXT[$i]['priority']= 1;
        //$EXT[$i]['app']     = "SetCIDName";
        //$EXT[$i]['appdata'] = $CNname;
        //$i ++;  

        // If no macro is selected use Dial
        if($this->macro!="none"){ 
          $macroname = preg_replace("/,.*$/","",$this->macro);        
          $macroname = preg_replace("/^.*=/","",$macroname);        
          $s_app = "Macro";$macroname;
          $s_par = $macroname."|".$s_parameter; 
        }else{
          $s_app = "Dial";
          $s_par = 'SIP/'.$this->uid."|20|r";
        }

        $EXT[$i]['context'] = 'GOsa';
        $EXT[$i]['exten']   = $s_telenums;
        $EXT[$i]['priority']= 1;
        $EXT[$i]['app']     = $s_app;
        $EXT[$i]['appdata'] = $s_par;
        $i ++;
      }

      // Append all these Entries 
      foreach($EXT as $entr){
        $SQL_syn = "INSERT INTO ".$a_New['EXT_TABLE']." (";
        foreach($entr as $key2 => $val2){
          $SQL_syn.= "`".$key2."`,";
        }
        $SQL_syn = preg_replace("/,$/","",$SQL_syn);
        $SQL_syn .= ") VALUES ("; 
        foreach($entr as $key2 => $val2){
          $SQL_syn .= "'".$val2."',";
        }
        $SQL_syn = preg_replace("/,$/","",$SQL_syn);
        $SQL_syn .=");\n";

        $SQL_query_array[] =$SQL_syn;
        $SQL_syn ="";
      }

      // Perform queries ...
      if($this->goFonHardware != "automatic"){
        foreach($SQL_query_array as $query){
          if(!@mysql_query($query,$new_connection)){
            print_red(_("Error while performing query:")." ".mysql_error());
            return false;
          }
        }
      }
    }
    @mysql_close($new_connection);
    return true;
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $display = "";

    if(empty($this->macro)&&(!empty($this->goFonMacro))){

      /* Go through already saved values, for a parameter */
      $tmp = split("!",$this->goFonMacro);

      /* it is possible that nothing has been saved yet */
      if(is_array($tmp)){

        /* First value is the macroname */
        $this->macro = $tmp[0];

        /* Macroname saved, delete that index */
        unset($tmp[0]);

        /* Check if macro has been removed */
        if(!isset($this->macroarray[$this->macro])){
          $this->macrostillavailable = false;
        }else{
          $this->macrostillavailable = true;
        }

        /* for each parametervalues ( parameterID#value like 25#twentyfive) */
        foreach($tmp as $var){

          /* Split this, so we have $varar[0] = parameterID $varar[1] = SelectedValue */
          $varar = split("#",$var);

          /* Only insert if the parameter still exists */
          if(isset($this->macroarray[$this->macro][$varar[0]])){
            /* Assign value */
            $this->macroarray[$this->macro][$varar[0]]['choosen']=$varar[1];
          }
        }
      }
    }
    
    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no phone extensions.")."</b>";
      $display.= back_to_main();
      return ($display);
    }

    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){

      /* Onyl change account state if allowed */
      if($this->is_account && $this->acl == "#all#"){
        $this->is_account= !$this->is_account;
      }elseif(!$this->is_account && chkacl($this->acl,"create") == ""){
        $this->is_account= !$this->is_account;
      }
    }

    /* Select no macro if, state is empty, this is the case, if the selected macro is no longer available */
    if(empty($this->macro)){
      $this->macro ="none";
    }

    /* Prepare templating */
    $smarty= get_smarty();

    /* tell user that the selected plugin is no longer available */
    if((!$this->macrostillavailable)&&($this->macro!="none")){
      print_red(_("The macro you selected, is no longer available for you, please choose another one."));
    }

    /* Assing macroselectbox values  */
    $smarty->assign("macros",$this->macros);   
    $smarty->assign("macro", $this->macro);   

    /* check if there is a FON server created */
    if(!count($this->goFonHomeServer)){
      print_red(_("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
    }

    /* Create html parameter table for selected macro parameters 
     *  skip if no parameters given 
     */
    if(!isset($this->macroarray[$this->macro])){
      $macrotab="";
    }else{

      $macrotab ="<table summary=\""._("Parameter")."\">";
      /* for every single parameter-> display textfile,combo, or true false switch*/

      foreach($this->phoneNumbers as $phonenum){
        $tmp[] = $phonenum;
      }
    
      if($this->macro != $this->lastmacro){
        /* Go through all params */
        foreach($this->macroarray[$this->macro] as $key => $paras){

          $string = $paras['default'];

          $string=preg_replace("/%uid/i",$this->uid,$string);

          if(isset($this->cn)){
            $string=preg_replace("/%cn/i",$this->cn,$string);
          }

          for($i = 0 ; $i < 10; $i++){
            if(isset($tmp[$i])){
              $string = preg_replace("/%telephoneNumber_".($i+1)."/i",$tmp[$i],$string);
            }
          }
          if(isset($tmp[0])){
            $string = preg_replace("/%telephoneNumber/i",$tmp[0],$string);
          }
          $this->macroarray[$this->macro][$key]['choosen']=$string;
        }
      }

      foreach($this->macroarray[$this->macro] as $paras){

        /* get al vars */
        $var        = $paras['var'];           
        $name       = $paras['name'];           
        $default    = $paras['default'];
        $type       = $paras['type'];
        $choosen    = $paras['choosen'] ; 
        $str        = $default;

        /* in case of a combo box display a combobox with selected attr */
        $macrotab.= "<tr>";
        switch ($type){

          case "combo":
            $str= "<select name='".$var."' ".chkacl($this->acl, "goFonMacro")." >";
          foreach(split(":",$default) as $choice){
            if($choosen==$choice){
              $str.= "\n<option value='".$choice."' selected>".$choice."&nbsp;</option>";
            }else{
              $str.= "\n<option value='".$choice."'>".$choice."&nbsp;</option>";
            }
          }
          $str.="</select>";
          $macrotab.= "<td>".base64_decode($name)."</td><td>$str";
          break;

          case "bool":
            if(!$choosen){
              $str="\n<input type='checkbox' name='".$var."' value='1' ".chkacl($this->acl, "goFonMacro")." >";
            }else{
              $str="\n<input type='checkbox' name='".$var."' value='1' checked  ".chkacl($this->acl, "goFonMacro").">";
            }
          $macrotab.= "<td colspan='2'>$str&nbsp;".base64_decode($name)."";
          break;

          case "string":
            $str="<input name='".$var."' value='".$choosen."' ".chkacl($this->acl, "goFonMacro")." style='width:340px;'>";
          $macrotab.= "<td>".base64_decode($name)."</td><td>$str";
          break;

        }
        $macrotab.= "</td></tr>";

      }
      $macrotab.="</table><input name='post_success' type='hidden' value='1'>";
    }//is_array()

    /* Give smarty the table */
    $smarty->assign("macrotab",$macrotab);


    /* Do we represent a valid account? */
    if (!$this->is_account && $this->parent == NULL){
      $display= "<img alt=\"\" src=\"images/stop.png\" align=\"middle\">&nbsp;<b>".
        _("This account has no phone extensions.")."</b>";
      $display.= back_to_main();
      return($display);
    }

    $display= "";

    /* Show tab dialog headers */
    if ($this->parent != NULL){
      if ($this->is_account){
        $display= $this->show_header(_("Remove phone account"),
            _("This account has phone features enabled. You can disable them by clicking below."));
      } else {
        if(empty($this->uid)){
          $display= $this->show_header(_("Create phone account"),
              _("This account has phone features disabled. You can't enable them while no uid is set."),TRUE,TRUE);
        }else{
          $display= $this->show_header(_("Create phone account"),
              _("This account has phone features disabled. You can enable them by clicking below."));
        }
        return ($display);
      }
    }

    /* Add phone number */
    if (isset($_POST["add_phonenumber"]) && $_POST['phonenumber']){
      if (is_phone_nr($_POST['phonenumber'])){
        $number= $_POST["phonenumber"];
        $this->phoneNumbers[$number]= $number;
        $this->is_modified= TRUE;
      } else {
        print_red(_("Please enter a valid phone number!"));
      }
    }

    /* Remove phone number */
    if (isset($_POST["delete_phonenumber"]) && isset($_POST["phonenumber_list"])){
      foreach ($_POST['phonenumber_list'] as $number){
        unset($this->phoneNumbers[$number]);
        $this->is_modified= TRUE;
      }
    }

    /* Transfer ACL's */
    foreach($this->attributes as $val){
      $smarty->assign($val."ACL", chkacl($this->acl,$val));
      if(isset($this->$val)){
        $smarty->assign($val,$this->$val);
      }else{
        $smarty->assign($val,"");
      }
    }

    /* Create home server array */
    $tmp = array();
    foreach($this->goFonHomeServers as $dn => $attrs){
      if(!is_numeric($dn)){
        $tmp[$dn] = $attrs['SERVER'];
      }
    }
    $smarty->assign("goFonHomeServers",$tmp);

    /* Fill arrays */
    $smarty->assign ("goFonHardware", $this->goFonHardware);
    if (!count($this->phoneNumbers)){
      $smarty->assign ("phoneNumbers", array());
    } else {
      $smarty->assign ("phoneNumbers", $this->phoneNumbers);
    }
    $hl= "<select size=\"1\" name=\"goFonHardware\" title=\"".
      _("Choose your private phone")."\" ".chkacl($this->acl, "goFonHardware").">\n";
    foreach ($this->hardware_list as $cn => $description){
      if ($cn == $this->goFonHardware){
        $selected= "selected";
      } else {
        $selected= "";
      }
      if (isset($this->used_hardware[$cn])){
        $color= "style=\"color:#A0A0A0\"";
      } else {
        $color= "";
      }
      $hl.= "  <option $color label=\"$cn\" value=\"$cn\" $selected>$description&nbsp;</option>\n";
    }
    $hl.= "</select>\n";
    $smarty->assign ("hardware_list", $hl);

    /* Show main page */
    $this->lastmacro = $this->macro;
    $display.= $smarty->fetch(get_template_path('generic.tpl', TRUE, dirname(__FILE__)));
    return($display);
  }


  function save_object()
  {
    if (isset($_POST["phoneTab"])){
    
      plugin::save_object();

      /* Save checkbox */
      if (isset($_POST['fon_to_mail'])){
        $tmp= "[M]";
      } else {
        $tmp= "[]";
      }
      if (chkacl ($this->acl, "goFonDeliveryMode") == ""){
        if ($this->goFonDeliveryMode != $tmp){
          $this->is_modified= TRUE;
        }
        $this->goFonDeliveryMode= $tmp;
      }

      /* Every macro in the select box are available */
      if((isset($_POST['macro']))){
        $this->macrostillavailable=true;
      }

      if(isset($_POST['macro']) && $_POST['macro'] != $this->macro){
        $this->is_modified =true;
      }

      if(is_array($this->phoneNumbers)){
        foreach($this->phoneNumbers as $telenumms) {
          $nummsinorder[]=$telenumms; 
        }
      }else{
        $nummsinorder=array("");
      }

      /* get all Postvars */
      if(isset($this->macroarray[$this->macro])){
 
        foreach($this->macroarray[$this->macro] as $key => $paras){

          $backup = $this->macroarray[$this->macro][$key];  

          if(isset($_POST[$paras['var']])){
            $this->macroarray[$this->macro][$key]['choosen'] = $_POST[$paras['var']];
          }

          /* Checkboxes are special, they are not Posted if they are not selected, so the won't be changed with the above code 
             We need this code below to read and save checkboxes correct
           */

          if(isset($_POST['post_success'])){
            if($this->macroarray[$this->macro][$key]['type']=="bool"){
              if(isset($_POST[$this->macroarray[$this->macro][$key]['var']])) {
                $this->macroarray[$this->macro][$key]['choosen']=$_POST[$paras['var']];
              }else{
                $this->macroarray[$this->macro][$key]['choosen']=false;
              }
            }
          }
          if(count(array_diff($this->macroarray[$this->macro][$key],$backup))){
            $this->modified = TRUE;
          }
        }
      }
    }
  }

  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if(!count($this->goFonHomeServers)){
      $message[] = _("There must be at least one server with an asterisk database to create a phone account.");
    }

    if(empty($this->goFonHomeServer)){
      $message[] = _("Please select a valid goFonHomeServer.");
    }

    if((strlen($this->goFonVoicemailPIN)==0)||(strlen($this->goFonVoicemailPIN)>4)){
      $message[]=(_("Voicemail PIN must be between 1-4 characters."));
    }else{
      if(preg_match("/[^0-9]/",$this->goFonVoicemailPIN)){
        $message[]=(_("The specified Voicemail PIN contains invalid characters, only numeric values are allowed here."));
      }
    }

    if(preg_match("/[^0-9a-z]/i",$this->goFonPIN)){
      $message[]=(_("The specified phone PIN contains invalid characters, only aphanumeric values are allowed here."));
    }

    if ($this->initially_was_account != $this->is_account || $this->is_modified){
      if(!$this->generate_mysql_entension_entries()){
        $message[] = $this->generate_error;
      }
    }

    /* We need at least one phone number */
    if (count($this->phoneNumbers) == 0){
      $message[]= sprintf(_("You need to specify at least one phone number!"));
    }

    /* check for ! in any parameter setting*/
    if(isset($this->macroarray[$this->macro])){
      foreach($this->macroarray[$this->macro] as $val){
        if((strstr($val['choosen'],"!"))||(strstr($val['choosen'],"#"))){
          $message[] = sprintf(_("The parameter %s contains invalid char. '!,#' is used as delimiter"),$val['name']);
        }
      }
    }
    return ($message);
  }



  function save()
  {
    plugin::save();

    /* Force saving macro again 
     * This ensures that 
     *  - the macro is available on the destiantion server.
     *  - the macro saved is up to date on the destination server.
     */
    if(!empty($this->macro) && $this->macro != "none")  {
      $macro_tab = new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->macro);
      $macro_tab -> save();
    }

    /* Save arrays */
    $tmp_numbers = array();
    foreach ($this->phoneNumbers as $number){
      $tmp_numbers[] = $number;
    }

    /* Save settings, or remove goFonMacro attribute*/
    if($this->macro!="none"){    
      $this->attrs['goFonMacro']=$this->macro;
      if(isset($this->macroarray[$this->macro])){
        foreach($this->macroarray[$this->macro] as $paras)  {
          $this->attrs['goFonMacro'].="!".$paras['id']."#".$paras['choosen'];
        }
      }
    }else{
      $this->attrs['goFonMacro']=array();
    }
    unset($this->attrs['macro'])  ;

    $this->attrs['goFonForwarding']=array();

    if ($this->initially_was_account != $this->is_account || $this->is_modified){
      $str = $this->generate_mysql_entension_entries(true);
      if(empty($str)){
        print_red($str);
      }
    }

    if($this->attrs['goFonMacro']==""){
      $this->attrs['goFonMacro']=array();
    }

    unset($this->attrs['cn']);

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    
    /* Force saving numbers, else it will be overwriten by user account. */
    $this->attrs['telephoneNumber'] =$tmp_numbers;
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Saving phone account failed"));

    /* Optionally execute a command after we're done */

    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify",array("uid" => $this->uid));
      }
    } else {
      $this->handle_post_events("add",array("uid" => $this->uid));
    }

  }


  function adapt_from_template($dn)
  {
    plugin::adapt_from_template($dn);

    /* Assemble phone numbers */
    if (isset($this->attrs['telephoneNumber'])){
      for ($i= 0; $i<$this->attrs['telephoneNumber']['count']; $i++){
        $number= $this->attrs['telephoneNumber'][$i];
        $this->phoneNumbers[$number]= $number;
      }
    }
  }


  function remove_from_parent()
  {
    if(!$this->initially_was_account) return;

    foreach($this->attributes as $key=>$val){
      if(in_array($val,array("uid","cn"))){
        unset($this->attributes[$key]);
        unset($this->$val);
      }
    }
    if(count($this->goFonHomeServers) && !empty($this->init_HomeServer) && is_callable("mysql_pconnect")){

      // Get Configuration for initial Mysql database Server
      $a_SETUP = $this->goFonHomeServers[$this->init_HomeServer];
      $s_parameter  ="";

      // Connect to DB server
      $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

      // Check if we are  connected correctly
      if(!$r_con){
        print_red(sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
                    $a_SETUP['SERVER'],$a_SETUP['LOGIN']));
        gosa_log(@mysql_error());
        return false;
      }

      // Select database for Extensions
      $db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

      // Test if we have the database selected correctly
      if(!$db){
        print_red(sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']));
        gosa_log(@mysql_error());
        return false;
      }

      $SQL="";

      /* If deletion starts from userslist, cn uid are not set */
      if((isset($this->parent->by_object['user']->uid))&&(!empty($this->parent->by_object['user']->uid))){
        $this->uid = $this->parent->by_object['user']->uid;
      }

      if((isset($this->parent->by_object['user']->cn))&&(!empty($this->parent->by_object['user']->cn))){
        $this->cn  = $this->parent->by_object['user']->cn;
      }

      $first_num = false;
      // Delete old entries
      foreach($this->a_old_telenums as $s_telenums){
        if(!$first_num){
          $first_num = $s_telenums;
        }
        $SQL[] = "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$s_telenums."';\n";
      }

      $SQL[] = "DELETE FROM ".$a_SETUP['VOICE_TABLE']." WHERE customer_id='".$first_num."';";
      $SQL[] = "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$this->uid."';\n";
      $SQL[] = "DELETE FROM ".$a_SETUP['SIP_TABLE']." WHERE name='".$this->uid."';\n";

      foreach($SQL as $query){
        if(!@mysql_query($query,$r_con)){
          print_red(_("Stop".mysql_error()));
          return false;
        }
      }
    }else{
      print_red(_("Can't remove phone account, the mysql extension is not present in php configuration."));
      return false;
    }

    /* unset macro attr, it will cause an error */
    $tmp = array_flip($this->attributes);
    unset($tmp['macro']);
    $this->attributes=array_flip($tmp);

    /* Cancel if there's nothing to do here */
    if (!$this->initially_was_account){
      return;
    }

    plugin::remove_from_parent();

    /* Just keep one phone number */
    if (count($this->telephoneNumber) && $this->telephoneNumber != ""){
      $this->attrs['telephoneNumber']= $this->telephoneNumber;
    } else {
      $this->attrs['telephoneNumber']= array();
    }


    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->search("(&(objectClass=goFonQueue)(member=*))", array("member"));
    while($attr = $ldap->fetch()){
      if(in_array($this->dn,$attr['member'])){
        $new =new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'],$attr['dn']);
        unset($new->by_object['ogroup']->memberList[$this->dn]);
        unset($new->by_object['ogroup']->member[$this->dn]);
        $new->save();
        print_red(sprintf(_("Removed user '%s' from phone queue '%s'."),$this->uid,$new->by_object['ogroup']->cn));
      }
    }
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    show_ldap_error($ldap->get_error(), _("Removing phone account failed"));

    /* Optionally execute a command after we're done */
    @mysql_close($r_con);
    $this->handle_post_events('remove',array("uid"=> $this->uid));
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

    foreach($this->phoneNumbers as $num){
      if(!isset($this->cn)) $this->cn = "";

      if((isset($numbers[$num]))&&(($numbers[$num]['uid'][0]!=$this->uid))){
        if(isset($numbers[$num]['uid'][0])){
          return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['uid'][0]);
        }else{
          return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['cn'][0]);
        }
      }
    }
  }


  /* Create phoneAccount part of copy & paste dialog */
  function getCopyDialog()
  { 
    if(!$this->is_account) return("");
    $smarty = get_smarty();
    if (!count($this->phoneNumbers)){
      $smarty->assign ("phoneNumbers", array(""));
    } else {
      $smarty->assign ("phoneNumbers", $this->phoneNumbers);
    }

    $smarty->assign("goFonVoicemailPIN",$this->goFonVoicemailPIN);
    $smarty->assign("goFonPIN",$this->goFonPIN);

    $display= $smarty->fetch(get_template_path('paste_generic.tpl', TRUE, dirname(__FILE__)));
    $ret =array();
    $ret['string'] = $display;
    $ret['status'] = "";
    return($ret);
  }

  /* Save posts from copy & paste dialog dialog  */
  function saveCopyDialog()
  {
    if(!$this->is_account) return;
    $this->execute();
    if(isset($_POST['goFonVoicemailPIN'])) {
      $this->goFonVoicemailPIN = $_POST['goFonVoicemailPIN'];
    }
    if(isset($_POST['goFonPIN'])){
      $this->goFonPIN = $_POST['goFonPIN'];
    }
  }


  function allow_remove()
  {
    /* Check if previously selected server is still available */
    if($this->initially_was_account && !isset($this->goFonHomeServers[$this->goFonHomeServer])){
      return sprintf(_("The previously selected asterisk home server (%s) is no longer available. Remove aborted."),preg_replace("/,/",", ",$this->goFonHomeServer));
    }
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
