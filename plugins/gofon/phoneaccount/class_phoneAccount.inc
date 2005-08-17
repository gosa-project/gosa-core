<?php

class phoneAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Phone";
  var $plDescription= "This does something";
  var $has_mailAccount= FALSE;

  /* Attributes */
  var $telephoneNumber        = "";
  var $goFonHardware          = "";
  var $goFonFormat            = "";
  var $goFonPIN               = "";
  var $goFonDeliveryMode      = "";
  var $phoneNumbers           = array();
  var $mail                   = "";
  var $hardware_list          = array();
  var $used_hardware          = array();
  var $goFonMacro             = "";
  var $macro                  = 0;              // Selected Macor
  var $macros                 = array();        // List of macros for smarty select box
  var $macroarray             = array();        // All needed macro informations
  var $macrostillavailable    = false;
  var $generate_error         = "";
  var $a_old_telenums           = array();
  var $uid;
  var $cn;

  /* CLI vars */
  var $cli_summary            = "Manage users phone account";
  var $cli_description        = "Some longer text\nfor help";
  var $cli_parameters         = array("eins" => "Eins ist toll", "zwei" => "Zwei ist noch besser");

  /* attribute list for save action */
  var $attributes             = array("goFonDeliveryMode", "goFonFormat",
      "goFonHardware", "goFonPIN", "telephoneNumber", "goFonMacro","macro");
  var $objectclasses= array("goFonAccount");

  function phoneAccount ($config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);

    /* Set phone hardware */
    if (!isset($this->attrs['goFonHardware'])){
      $this->goFonHardware= "automatic";
    }

    /* Preset voice format */
    if (!isset($this->attrs['goFonFormat'])){
      $this->goFonFormat= "wav";
    }

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


    /* Load hardware list */
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

    /* Prepare templating */
    $smarty= get_smarty();


    /* Perform search, to get Macro Parameters,Name,Dn,Displayname etc*/
    $ldap->search("(objectClass=goFonMacro)", array("*"));

    /* Add none for no macro*/
    $this->macros['none']=_("no macro");    
    $this->macro ="none";


    /* Fetch all Macros*/
    while ($attrs= $ldap->fetch()){

      /* Only visisble */
      if((isset($attrs['goFonMacroVisible'][0]))&&($attrs['goFonMacroVisible'][0] ==1)){

        /* unset Count, we don't need that here */
        unset($attrs['displayName']['count']);

        /* fill Selectfield variable with Macros */
        if(isset($attrs['displayName'][0])){
          $this->macros[$attrs['dn']] = $attrs['displayName'][0]." (".$attrs['cn'][0].")";
        }else{
          $this->macros[$attrs['dn']] = _("undefined");
        }

        /* Parse macro data, unset count for parameterarrays  */
        unset($attrs['goFonMacroParameter']['count']);

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
      }//visible = 1
    }//while

    /* Go through already saved values, for a parameter */
    $tmp = split("!",$this->goFonMacro);

    /* it is possible that nothing has been saved yet */
    if(is_array($tmp)){

      /* First value is the macroname */
      $this->macro = $tmp[0];

      /* Macroname saved, delete that index */
      unset($tmp[0]);

      /* Check if makro has been removed */
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


    /* Eventually colorize phones */
    $ldap->cd($this->config->current['BASE']);
    foreach ($this->hardware_list as $cn => $desc){
      $ldap->search("(goFonHardware=$cn)", array('cn'));
      if ($ldap->count() > 0){
        $ldap->fetch();
        if ($ldap->getDN() != $this->dn){
          $this->used_hardware[$cn]= $ldap->getDN();
        }
      }
    }
    $this->hardware_list["automatic"]= _("automatic");
    ksort($this->hardware_list);
    $this->a_old_telenums = $this->phoneNumbers;
  }




  // Generate MySQL Syntax
  function generate_mysql_entension_entries($save = false){

    // Get Configuration for Mysql database Server
    $a_SETUP        = $_SESSION['config']->data['SERVERS']['FON'];  // DB Configuration
    $s_parameter    = "";                                           // Contains paramter for selected Macro 
    $r_con          = false;                                        // DB connection
    $r_db           = false;                                        // Selected DB
    $r_res          = false;                                        // Result resource
    $a_ldap_attrs   = array();                                      //  

    $s_ip           = NULL;                   // Contains ip for Sip entry
    $s_host         = NULL;                   // Contains host for Sip entry
    $s_qualify      = NULL;                   // Qualify entry
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

    $i_is_accounted =false;   // Ensure that extension entry, for name to number is only once in table


    // Connect to DB server
    $r_con =  @mysql_connect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

    // Check if we are  connected correctly
    if(!$r_con){
      $this->generate_error = sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
          $a_SETUP['SERVER'],$a_SETUP['LOGIN']);
      gosa_log(mysql_error());
      return false;
    }

    // Select database for Extensions
    $r_db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

    // Test if we have the database selected correctly
    if(!$r_db){
      $this->generate_error = sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']);
      gosa_log(mysql_error());
      return false;
    }

    // Get phonehardware to setup sip entry
    $ldap         = $this->config->get_ldap_link();
    $r_res        = $ldap->search("(&(objectClass=goFonHardware)(cn=".$this->goFonHardware."))", array('*'));
    $a_ldap_attrs = $ldap->fetch();

    if($this->is_number_used()){
      $this->generate_error = $this->is_number_used(); 
      return false;
    }



    /* If Save == true, we should save something.
     * Generate SQL, for drop of old entries
     * Generate SQL, for insert of new entries
     */ 
    if($save == true){
      // Attribute GoFonDefaultIP set ?
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

      // generate SIP entry
      $sip_data_array['id']           = "";
      $sip_data_array['name']         = $this->uid;
      $sip_data_array['accountcode']  = NULL;          
      $sip_data_array['amaflags']     = NULL;
      $sip_data_array['callgroup']    = NULL;
      $sip_data_array['callerid']     = "";
      $sip_data_array['canreinvite']  = "yes";
      $sip_data_array['context']      = "default";
      $sip_data_array['defaultip']    = NULL;
      $sip_data_array['fromuser']     = NULL;
      $sip_data_array['fromdomain']   = NULL;
      $sip_data_array['host']         = $s_host;
      $sip_data_array['insecure']     = NULL;
      $sip_data_array['language']     = NULL;
      $sip_data_array['mailbox']      = "asterisk";
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
      $sip_data_array['secret']       = $s_pin;
      $sip_data_array['type']         = $s_type ;
      $sip_data_array['username']     = $this->uid;
      $sip_data_array['disallow']     = NULL;
      $sip_data_array['allow']        = NULL;
      $sip_data_array['musiconhold']  = NULL;
      $sip_data_array['regseconds']   = NULL;
      $sip_data_array['ipaddr']       = $s_ip;
      $sip_data_array['regexten']     = NULL;
      $sip_data_array['cancallforward']=NULL;

      // Get selected Macro Parameter and create parameter entry 
      if(isset($this->macroarray[$this->macro])){
        foreach($this->macroarray[$this->macro] as $key => $val ){
          $s_parameter .= $val['choosen']."|";
        }
        $s_parameter = preg_replace("/\|$/","",$s_parameter);
      }

      if($this->is_number_used()){
        $this->generate_error = $this->is_number_used(); 
        return false;
      }

      // Create new SIP entry ...
      $sip_entry = $sip_data_array;
      reset($this->phoneNumbers);
      $i_new_key = key($this->phoneNumbers);
      $sip_entry['callerid']  =$this->phoneNumbers[$i_new_key];
      $sip_entry['mailbox']   =$this->phoneNumbers[$i_new_key];

      if((isset($this->parent->by_object['mailAccount']->mail))&&($this->parent->by_object['mailAccount']->is_account==true)){
        $s_mail = $this->parent->by_object['mailAccount']->mail;
      }else{
        $s_mail = "";
      }

      // $SQL contains all queries
      $SQL   = array();
      $SQL[] = "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$this->uid."';\n";
      $SQL[] = "DELETE FROM ".$a_SETUP['SIP_TABLE']." WHERE name='".$this->uid."';\n";
      $SQL[] = "DELETE FROM ".$a_SETUP['VOICE_TABLE']." WHERE customer_id=".$this->phoneNumbers[$i_new_key].";"; 

      // Generate Strings with keys and values 
      foreach($sip_entry as $s_sip_key=>$s_sip_val){
        if($s_sip_val == NULL) continue;
        $s_sip_values.="'".$s_sip_val."',";
        $s_sip_keys  .="`".$s_sip_key."`,";
      }
      // Remove last ,
      $s_sip_values =  preg_replace("/,$/","",$s_sip_values);
      $s_sip_keys   =  preg_replace("/,$/","",$s_sip_keys);

      // Append SIP Entry 
      $SQL[] ="INSERT INTO ".$a_SETUP['SIP_TABLE']." (".$s_sip_keys.") VALUES (".$s_sip_values.");";

      // Delete old entries
      $b_first_deleted  =false;
      foreach($this->a_old_telenums as $s_telenums){
        $SQL[] = "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE exten='".$s_telenums."';\n";
        if(!$b_first_deleted){
          $b_first_deleted=true;
          $SQL[] = "DELETE FROM ".$a_SETUP['VOICE_TABLE']." WHERE customer_id=".$s_telenums.";";
        }
      }

      $SQL[]= "INSERT INTO ".$a_SETUP['VOICE_TABLE']."
        (`customer_id`,`context`,`mailbox`,`password`,`fullname`,`email`,`pager`)
        VALUES
        ('".$this->phoneNumbers[$i_new_key]."','default','".$this->phoneNumbers[$i_new_key]."','".$this->goFonPIN."','".$this->sn."','".$s_mail."','');";

      $i_is_accounted=false;

      // Entension entries  Hint / Dial / Goto
      foreach($this->phoneNumbers as $s_telenums){
        // Entry  to call by name
        $s_entry_name['context']  = 'GOsa';
        $s_entry_name['exten']    = $this->uid;
        $s_entry_name['priority'] = 1;
        $s_entry_name['app']      = 'Goto';
        $s_entry_name['appdata']  = $s_telenums."|1";

        // hint
        $s_entry_hint['context']  = 'GOsa';
        $s_entry_hint['exten']    = $s_telenums;
        $s_entry_hint['priority']      = 'hint';
        $s_entry_hint['app']  = 'SIP/'.$this->uid;

        // If no macro is selected use Dial
        if($this->macro!="none"){ 
          $macroname = preg_replace("/,.*$/","",$this->macro);        
          $macroname = preg_replace("/^.*=/","",$macroname);        
          $s_app = "Macro";$macroname;
          $s_par = $macroname."|".$s_parameter; 
        }else{
          $s_app = "Dial";
          $s_par = 'SIP/'.$this->uid;
        }

        // Entry  to call by number
        $s_entry_phone['context']  = 'GOsa';
        $s_entry_phone['exten']    = $s_telenums;
        $s_entry_phone['priority'] = 1;
        $s_entry_phone['app']      = $s_app;
        $s_entry_phone['appdata']  = $s_par;

        // append name entry only once
        if(!$i_is_accounted){ 
          $i_is_accounted = true;
          $entries[]=array("hint"=>$s_entry_hint,"phone"=>$s_entry_phone,"name"=>$s_entry_name); 
        }else{
          $entries[]=array("hint"=>$s_entry_hint,"phone"=>$s_entry_phone);
        }
      }

      // Append all these Entries 
      foreach($entries as $num => $val){
        foreach($val as $entr){
          $SQL_syn = "INSERT INTO ".$a_SETUP['EXT_TABLE']." (";
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
          $SQL[] =$SQL_syn;
          $SQL_syn ="";
        }
      }

      // Perform queries ...
      foreach($SQL as $query){
        if(!mysql_query($query,$r_con)){
          print_red(_("Error while performing query ".mysql_error()));
          return false;
        }
      }
    }
    return true;
  }














  function execute()
  {

    /* force postmodify event, to restart phones */

    $this->parent->by_object['user']->is_modified=TRUE;
    $this->is_modified=TRUE; 

    $this->uid = $this->parent->by_object['user']->uid;
    $this->cn  = $this->parent->by_object['user']->cn;


    /* Do we need to flip is_account state? */
    if (isset($_POST['modify_state'])){
      $this->is_account= !$this->is_account;
    }

    /* Select no macro if, state is empty, this is the case, if the selected macro is no longer available */
    if(empty($this->macro)){
      $this->macro ="none";
    }

    /* tell user that the pluging selected is no longer available*/
    if((!$this->macrostillavailable)&&($this->macro!="none")){
      print_red(_("The macro you selected in the past, is no longer available for you, please choose another one."));
    }

    /* Prepare templating */
    $smarty= get_smarty();

    /* Assing macroselectbox values  */
    $smarty->assign("macros",$this->macros);   
    $smarty->assign("macro", $this->macro);   

    /* Create parameter table, skip if no parameters given */
    if(!isset($this->macroarray[$this->macro])){
      $macrotab="";
    }else{

      $macrotab ="<table summary=\""._("Parameter")."\">";
      /* for every single parameter-> display textfile,combo, or true false switch*/


      /* Automatic fill out */
      if(isset($_POST['fillout'])){

        foreach($this->phoneNumbers as $phonenum){
          $tmp[] = $phonenum;
        }

        /* Go through all params */
        foreach($this->macroarray[$this->macro] as $key => $paras){

          $string = $paras['default'];

          $string=preg_replace("/%uid/i",$this->uid,$string);
          $string=preg_replace("/%cn/i",$this->cn,$string);

          for($i = 0 ; $i < 10; $i++){
            if(isset($tmp[$i])){
              $string = preg_replace("/%telephoneNumber_".($i+1)."/i",$tmp[$i],$string);
            }
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
            $str= "<select name='".$var."' ".chkacl($this->acl, "goFonMacro")."  ".chkacl($this->acl, "goFonMacro").">";
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
      $smarty->assign($val."ACL", chkacl($this->acl, "$val"));
      $smarty->assign($val,$this->$val);
    }

    /* Fill arrays */
    $smarty->assign ("goFonHardware", $this->goFonHardware);
    if (!count($this->phoneNumbers)){
      $smarty->assign ("phoneNumbers", array(""));
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
          if(isset($_POST[$paras['var']])){
            //            $par = $this->macroarray[$this->macro][$key];
            //            $string = "";
            //            if(preg_match("/.*%telephoneNumber_.*/",$par['default'])){
            //              $string = $par['default'];
            //              foreach($nummsinorder as $nummsinorderkey=> $nummsinorderval){
            //                $string = (str_replace("%telephoneNumber_".($nummsinorderkey+1),$nummsinorderval,$string));
            //              }
            //            }

            //            if(preg_match("/.*%uid.*/",$par['default'])){
            //              if(empty($string)) $string = $par['default'];
            //              $string = str_replace("%uid",$this->uid,$string);
            //            }    

            //            if(!empty($string)){  
            //              $this->macroarray[$this->macro][$key]['choosen'] = $string; 
            //            }else{
            $this->macroarray[$this->macro][$key]['choosen'] = $_POST[$paras['var']];
            //            }
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
        }
      }
    }
  }

  function check()
  {
    /* Reset message array */
    $message= array();

    if(!$this->generate_mysql_entension_entries()){
      $message[] = $this->generate_error;
    }

    /* We need at least one phone number */
    if (count($this->phoneNumbers) == 0){
      $message[]= sprintf(_("You need to specify at least one phone number!"));
    }

    if(($this->goFonPIN)==""){
      $message[]= sprintf(_("You need to specify a Phone PIN."));
    }else{
      if(strcmp ((int)($this->goFonPIN),($this->goFonPIN))){
        $message[] = sprintf(_("The given PIN is not valid, only numbers are allowed for this type."));
      }elseif(strlen($this->goFonPIN) < 4){
        $message[] = sprintf(_("The given PIN is too short"));
      }

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

    /* Save arrays */
    $this->attrs['telephoneNumber']= array();
    foreach ($this->phoneNumbers as $number){
      $this->attrs['telephoneNumber'][]= $number;
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

    $this->generate_mysql_entension_entries(true);

    if($this->attrs['goFonMacro']==""){
      $this->attrs['goFonMacro']=array();
    }

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */

    if ($this->initially_was_account == $this->is_account){
      if ($this->is_modified){
        $this->handle_post_events("modify");
      }
    } else {
      $this->handle_post_events("add");
    }

  }


  function insert_after($entry, $nr, $list)
  {
    /* Is the entry free? No? Make it free... */
    if (isset($list[$nr])) {
      $dest= array();
      $newidx= 0;

      foreach ($list as $idx => $contents){
        $dest[$newidx++]= $contents;
        if ($idx == $nr){
          $dest[$newidx++]= $entry;
        }
      }
    } else {
      $dest= $list;
      $dest[$nr]= $entry;
    }

    return ($dest);
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
    // Get Configuration for Mysql database Server
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];
    $s_parameter  ="";

    // Connect to DB server
    $r_con =  @mysql_connect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

    // Check if we are  connected correctly
    if(!$r_con){
      $this->generate_error = sprintf(_("The MySQL Server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
          $a_SETUP['SERVER'],$a_SETUP['LOGIN']);
      gosa_log(mysql_error());
      return false;
    }

    // Select database for Extensions
    $db  =  @mysql_select_db($a_SETUP['DB'],$r_con);

    // Test if we have the database selected correctly
    if(!$db){
      $this->generate_error = sprintf(_("Can't select database %s on %s."),$a_SETUP['DB'],$a_SETUP['SERVER']);
      gosa_log(mysql_error());
      return false;
    }

    $SQL="";

    /* If deletion starts from userslist, cn uid are not set */
    $this->uid = $this->parent->by_object['user']->uid;
    $this->cn  = $this->parent->by_object['user']->cn;

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
      if(!mysql_query($query,$r_con)){
        print_red(_("Stop".mysql_error()));
        return false;
      }
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
    $ldap->search("(objectClass=goFonQueue)", array("member"));
    while($attr = $ldap->fetch()){
      if(in_array($this->dn,$attr['member'])){
        $new =new ogrouptabs($this->config, $this->config->data['TABS']['OGROUPTABS'],$attr['dn']);
        unset($new->by_object['ogroup']->memberList[$this->dn]);
        unset($new->by_object['ogroup']->member[$this->dn]);
        $new->save();
        print_red(sprintf(_("Removed user '%s' from phone queue '%s'."),$this->uid,$new->by_object['ogroup']->attrs['cn']));
      }
    }
    $ldap->cd($this->dn);
    $ldap->modify($this->attrs);
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
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


}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
