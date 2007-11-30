<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class conference extends plugin
{
  /* department attributes */
  var $cn						  = "";
  var $description		= "";
  var $base 				  = "";
  var $ou 						= "";
  var $goFonPIN				= "";
  var $old_cn         = "";

  var $goFonConferenceOption   	= "";
  var $goFonConferenceOption_P 	= "";   // Set PIN 
  var $goFonConferenceOption_r 	= "";   // record Conference
  var $goFonConferenceOption_M 	= "";   // Play Music opn hold
  var $goFonConferenceOption_s 	= "";   // activate menu
  var $goFonConferenceOption_i 	= "";   // announce new and leaving user
  var $goFonConferenceOption_c 	= "";   // Count User
  var $goFonConferenceOption_D 	= "";   // Conference Type, no PIN/PIN

  var $goFonConferenceOwner     = "";
  var $goFonHomeServer          = "0";      // Home server of the conference
  var $init_HomeServer          = "0";      // Initial home server of the conference
  var $goFonHomeServers         = array();  // All available home servers

  var $goFonConferenceOptionFormat 		= "";
  var $goFonConferenceOptionLifetime 	= "";
  var $telephoneNumber 		= "";

  var $old_tele_number	        = false;
  var $old_dn;

  /* Headpage attributes */
  var $last_dep_sorting= "invalid";
  var $departments= array();

  var $dialog ;

  /* attribute list for save action */
  var $attributes= array("cn","base", "description", "goFonPIN","goFonConferenceOption_P","goFonConferenceOption_r",
      "goFonConferenceOption_M","goFonConferenceOption_s","goFonConferenceOption_i","goFonConferenceOption_c","goFonHomeServer",
      "goFonConferenceOption_D","goFonConferenceOptionFormat","goFonConferenceOptionLifetime","telephoneNumber","goFonConferenceOwner");

  var $language  = "de";
  var $languages = array();

  var $objectclasses= array("top", "goFonConference");

  function conference ($config, $dn, $plugin= NULL)
  {
    plugin::plugin($config, $dn, $plugin);
    $this->is_account	= TRUE;
    $this->ui			    = get_userinfo();
    $this->orig_dn		= $dn;

    $this->languages= get_languages(TRUE,TRUE);


    /* Check server configurations
     * Load all server configuration in $this->goFonHomeServers if available
     *  and use first server as default if necessary.
     */
    $a_SETUP= array();
    if(array_key_exists('config',$_SESSION) &&
       array_key_exists('SERVERS',$_SESSION['config']->data) &&
       array_key_exists('FON',$_SESSION['config']->data['SERVERS']) &&
       count($_SESSION['config']->data['SERVERS']['FON']) &&
       is_callable("mysql_connect")
       ) {

      /* Set available server */
      $this->goFonHomeServers = $_SESSION['config']->data['SERVERS']['FON'];

      /* Set default server */
      if($this->dn == "new"){
        $this->goFonHomeServer = $this->goFonHomeServers[0]['DN'];
      }

      /* Remember inital home server, to be able to remove old entries */
      $this->init_HomeServer = $this->goFonHomeServer;

      /* get config */
      if(!isset($this->goFonHomeServers[$this->goFonHomeServer])){
        print_red(sprintf(_("The specified home server '%s' is not available in GOsa server configuration. Saving this account will create a new entry on the server '%s'. Use cancel if you do not want to create a new entry while ignoring old accounts."),$this->goFonHomeServer, $this->goFonHomeServers[0]['DN']));

        $this->goFonHomeServer = $this->goFonHomeServers[0]['DN'];
        $this->init_HomeServer = $this->goFonHomeServers[0]['DN'];
      }
      $cur_cfg = $this->goFonHomeServers[$this->goFonHomeServer]; 
    }

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      if(isset($_SESSION['CurrentMainBase'])){
        $this->base = $_SESSION['CurrentMainBase'];
      }else{
        $this->base= dn2base($ui->dn);
      }
    } else {

      /* The base is something like this 
          "cn=Confis,ou=conferences,ou=asterisk,ou=configs,ou=systems," */
      $this->base= preg_replace ("/^[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,/", "", $this->dn);
    }

    $ui= get_userinfo();
    $acl= get_permissions ($ui->dn, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "conference", $ui->dn);
    $this->goFonConferenceOwner=$this->ui->dn;

    /* Parse Options ... 
     * Parameter|Lifetime|number
     */
    if($this->dn!="new"){
      $tmp1= split("\|",$this->attrs['goFonConferenceOption'][0]);

      for($i = 0 ; $i < strlen($tmp1[0]);$i++){
        $varname = "goFonConferenceOption_".$tmp1[0][$i];
        if($tmp1[0][$i]=="d"){
          $this->goFonConferenceOption_D = $tmp1[0][$i];    
        }else{
          $this->$varname = $tmp1[0][$i];    
        }
      }

      $this->goFonConferenceOptionLifetime  = $tmp1[1];
      if(isset($tmp1[2]) && isset($this->languages[$tmp1[2]])){
        $this->language = $tmp1[2];
      }
  
      $this->old_tele_number                = $this->telephoneNumber;
    }
    $this->old_dn = $this->dn;
    $this->old_cn = $this->cn;
  }


  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $smarty= get_smarty();

    $smarty->assign("bases" ,$this->config->idepartments);
    $smarty->assign("base"  ,$this->base);

    $once = true; 
    foreach($_POST as $name => $value){
      if(preg_match("/^chooseBase/",$name) && $once){
        $once = false;
        $this->dialog = new baseSelectDialog($this->config);
        $this->dialog->setCurrentBase($this->base);
      }
    }

    /* Dialog handling */
    if(is_object($this->dialog)){
      /* Must be called before save_object */
      $this->dialog->save_object();

      if($this->dialog->isClosed()){
        $this->dialog = false;
      }elseif($this->dialog->isSelected()){
        $this->base = $this->dialog->isSelected();
        $this->dialog= false;
      }else{
        return($this->dialog->execute());
      }
    }

    /* Create array with goFonHomeServer */
    $tmp = array();
    foreach($this->goFonHomeServers as $dn => $val){
      if(!is_numeric($dn)){
        $tmp[$dn]  = $val['SERVER'];
      }
    }
    $smarty->assign("language",$this->language);
    $smarty->assign("languages",$this->languages);
    $smarty->assign("goFonHomeServers",$tmp);
    $smarty->assign("goFonConferenceOptions",		    array("D"=>"Conference ","d"=>"Conference without PIN"));
    $smarty->assign("goFonConferenceOptionFormats",	array("WAV"=>"Wave","GSM"=>"GSM","WAV49"=>"Wave49"));
    $smarty->assign("goFonConferenceOption",        $this->goFonConferenceOption_D);

    foreach ($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
      $smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
      if(!$this->$val){
        $smarty->assign($val."CHK", "");
      }else{
        $smarty->assign($val."CHK", " checked ");
      }
    }

    if($_SESSION['js']==1){
      if($this->goFonConferenceOption_P != "P"){
        $smarty->assign("goFonPINACL"," disabled ");
        $smarty->assign("goFonPIN","");
      }
      if($this->goFonConferenceOption_r != "r"){
        $smarty->assign("goFonConferenceOptionFormatACL"," disabled ");
      }
    }
    return($smarty->fetch (get_template_path('generic.tpl', TRUE)));
  }


  function remove_from_parent()
  {
    /* Check if 'old' home server is available in gosa FON server configuration 
     * Try to remove this entry from database and display errors.  
     */
    if(isset($this->goFonHomeServers[$this->goFonHomeServer])){
      $str = $this->SQL_remove_me(true); 
      if($str){
        print_red($str);
        return false;
      }
    }else{
      print_red(_("Could not remove the conference entry from database on home server (%s). Please check your asterisk database configuration."));
      return false;
    }

    /* Remove ldap entry */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->recursive_remove();

    /* Optionally execute a command after we're done */
    $this->handle_post_events('remove');
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
    if(isset($_POST['cn'])){

      /* Get selected language */
      if(isset($_POST['language']) && isset($this->languages[get_post('language')])){
        $this->language = get_post('language');
      }

      foreach(array("goFonConferenceOption_P","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_D","goFonPIN") as $attrs){
        if(isset($_POST[$attrs])){
          $this->$attrs = $_POST[$attrs];
        }else{
          $this->$attrs = false;
        }
      }
    }
  }


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
      gosa_log(@mysql_error($r_current));
      return(sprintf(_("The MySQL home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
            $cfg_Current['SERVER'],$cfg_Current['LOGIN']));
    }
    $db_current  =  @mysql_select_db($cfg_Current['DB'],$r_current);
    if(!$db_current){
      gosa_log(@mysql_error($r_current));
      mysql_close($r_current);
      return( sprintf(_("Can't select database '%s' on home server '%s'."),$cfg_Current['DB'],$cfg_Current['SERVER']));
    }

    /********************
     * Check init home server 
     ********************/

    if($this->goFonHomeServers != $this->init_HomeServer){
      $cfg_Init  = $this->goFonHomeServers[$this->init_HomeServer] ;
      $r_init    =  @mysql_pconnect($cfg_Init['SERVER'],$cfg_Init['LOGIN'],$cfg_Init['PASSWORD']);
      if(!$r_init){
        gosa_log(@mysql_error($r_init));
        return(sprintf(_("The MySQL initial home server '%s' isn't reachable as user '%s', check GOsa log for mysql error."),
              $cfg_Init['SERVER'],$cfg_Init['LOGIN']));
      }
      $db_init  =  @mysql_select_db($cfg_Init['DB'],$r_init);
      if(!$db_init){
        gosa_log(@mysql_error($r_init));
        mysql_close($r_init);
        return( sprintf(_("Can't select database '%s' on initial home server '%s'."),$cfg_Init['DB'],$cfg_Init['SERVER']));
      }
    }
  }

  /* Check values */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();

    if($this->is_number_used()){
      $message[] =  $this->is_number_used();
    }

    /* Check if previously selected server is still available */
    if($this->initially_was_account && !isset($this->goFonHomeServers[$this->goFonHomeServer])){
      $message[]= sprintf(_("The previously selected asterisk home server (%s) is no longer available."),preg_replace("/,/",", ",$this->goFonHomeServer));
      return($message);
    }

    if((!empty($this->goFonPIN)||($this->goFonConferenceOption_P=="P"))&&($this->goFonConferenceOption_D=="d")){
      $message[] =_("You have specified a conference 'without PIN' ... please leave the PIN fields empty.");
    }

    if((empty($this->goFonPIN))&&($this->goFonConferenceOption_P=="P")&&($this->goFonConferenceOption_D=="D")){
      $message[]= _("Please enter a PIN.");
    }

    if(empty($this->cn)){
      $message[] =_("Please enter a name for the conference.");
    }

    if(!is_numeric($this->telephoneNumber)){
      $message[] =_("Only numeric chars are allowed in Number field.");
    }

    if(!((is_numeric($this->goFonConferenceOptionLifetime))||(empty($this->goFonConferenceOptionLifetime)))){
      $message[] =_("Only numbers are allowed in Lifetime.");
    }

    /* Check if add could be successful */    
    $str = $this->SQL_add_me(false);
    if(!empty($str)){
      $message[] = $str;
    }

    return $message;
  }


  function SQL_add_me($save)
  {
    /* Check if there is at least on server configuration */
    if(!count($this->goFonHomeServers)){
      return( _("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). Your settings can't be saved to asterisk database."));
    }
  
    /********************
     * Get configuration and try to connect 
     ********************/

    /* Check if databases are reachable, returns an error string if anything fails  */
    $error_str = $this->check_database_accessibility();
    if($error_str){
      return($error_str);
    }

    /* Remove old entries, returns an error string if anything fails  */
    $error_str = $this->SQL_remove_me($save);
    if($error_str){
      return($error_str);
    }

    /* Connect to current database to be able to add new entries */
    $cfg_Current  = $this->goFonHomeServers[$this->goFonHomeServer] ;
    $res_cur      =  @mysql_pconnect($cfg_Current['SERVER'],$cfg_Current['LOGIN'],$cfg_Current['PASSWORD']);
    $db_cur       =  @mysql_select_db($cfg_Current['DB'],$res_cur);

    /********************
     * Remove entries that could cause trouble  
     ********************/

    /* If the current home server is different to the initial home server,
     *  there may be already some entries with the given telephoneNumber and/or cn.
     * We must remove those entries to avoid duplicate use of the same extension name.
     */
    if($this->goFonHomeServer != $this->init_HomeServer){
      $query = "SELECT id FROM ".$cfg_Current['EXT_TABLE']." WHERE exten='".$this->telephoneNumber."' OR '".$this->cn."';";
      $res = @mysql_query($query,$res_cur);
      if(!$res){
        gosa_log(@mysql_error($res_cur));
        return(_("Can not check if there are already some entries with given telephone number and/or cn in the destination home server.").
            "&nbsp;"._("Please have a look a the gosa logfiles."));
      }
      if($save && mysql_affected_rows($res_cur)) {
        $SQL = "DELETE FROM ".$cfg_Current['EXT_TABLE']." 
          WHERE   (exten='".$this->telephoneNumber."') 
          OR    (exten='".$this->cn."')";

        /* Query and ensure that everything went fine */
        $res =  @mysql_query($SQL,$res_cur);
        if(!$res){
          gosa_log(@mysql_error($res_cur));
          return(_("Can not remove entries with some telephone number and/or cn from destination home server.").
              "&nbsp;"._("Please have a look a the gosa logfiles."));
        }
      }
    }

    /********************
     * Add new conference entry  
     ********************/
    if((!empty($this->telephoneNumber))&&($save==true)){

      /* Create string out of conference Flags */
      $parameter  ="";
      foreach(array("goFonConferenceOption_P","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_D") as $attrs){
        $parameter .= $this->$attrs;
      }

      $i=1;
      $EXT=array();
      $context="GOsa";
      // Set Language to German
      $EXT[$i]['exten']   =$this->telephoneNumber;
      $EXT[$i]['context'] = $context;
      $EXT[$i]['priority']= $i;
      $EXT[$i]['app']     ="SetLanguage";
      $EXT[$i]['appdata'] =$this->language;
      $i++;      

      if($this->goFonConferenceOption_r == "r"){

        // Recordingformat for conference
        $EXT[$i]['exten']   =$this->telephoneNumber;
        $EXT[$i]['context'] =$context;
        $EXT[$i]['priority']= $i;
        $EXT[$i]['app']     ="Setvar";
        $EXT[$i]['appdata'] ="MEETME_RECORDINGFORMAT=".$this->goFonConferenceOptionFormat;
        $i++;      

      }

      // Answer Call
      $EXT[$i]['exten']   =$this->telephoneNumber;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']=$i;
      $EXT[$i]['app']     ="answer";
      $EXT[$i]['appdata'] ="";
      $i++;      

      // Start Conference 
      $EXT[$i]['exten']   =$this->telephoneNumber;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']=$i;
      $EXT[$i]['app']     ="MeetMe";

      if(empty($this->goFonPIN)) {
        $EXT[$i]['appdata'] =$this->telephoneNumber."|".$parameter;
      }else{
        $EXT[$i]['appdata'] =$this->telephoneNumber."|".$parameter."|".$this->goFonPIN;
      }
      $i++; 

      // Start Conference 
      $EXT[$i]['exten']   =$this->cn;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']=1;
      $EXT[$i]['app']     ="Goto";
      $EXT[$i]['appdata'] =$this->telephoneNumber."|1";
      $SQL=array();

      foreach($EXT as $keytop => $valtop){
        $s_keys = "";
        $s_values = ""; 
        foreach($valtop as $key=>$val){
          $s_keys   .="`".$key."`,";
          $s_values .="'".$val."',";
        }
        $s_keys   =preg_replace("/\,$/","",$s_keys); 
        $s_values =preg_replace("/\,$/","",$s_values); 
        $SQL[]="INSERT INTO ".$cfg_Current['EXT_TABLE']." (".$s_keys.") VALUES (".$s_values.");"; 
      }
      foreach($SQL as $sqlsyn){
        mysql_query($sqlsyn,$res_cur);
      }
    } 

    @mysql_close($res_cur);
  }


  /* Remove initial entry from database 
   * This function checks if there is an entry in the 
   *  initial home server that uses this->old_cn or $this->old_tele_number
   *  and removes this entries. 
   * This function is called from save and remove_from parent.
   * 
   *  The parameter '$save' is false if we just 
   *   want to check if a remove is possible. 
   *  And true if we realy want to remove the entries.   
   */
  function SQL_remove_me($save)
  {
    /* check database access */
    $str = $this->check_database_accessibility();
    if($str){
      return($str);
    }

    /* Connect to old database */
    $cfg_Init  = $this->goFonHomeServers[$this->init_HomeServer] ;
    $r_init    =  @mysql_pconnect($cfg_Init['SERVER'],$cfg_Init['LOGIN'],$cfg_Init['PASSWORD']);
    $db_init   =  @mysql_select_db($cfg_Init['DB'],$r_init);
   
    /* Check if there is an old entry */
    $query = "SELECT id FROM ".$cfg_Init['EXT_TABLE']." WHERE exten='".$this->old_tele_number."' OR '".$this->old_cn."';";
    $res = @mysql_query($query,$r_init);
    if(!$res){
      gosa_log(@mysql_error($r_init));
      return(_("Can not check if entry exists in old database. Please have a look a the gosa logfiles."));
    }
   
    /* There are entries using this cn and/or phone number */
    if($save && mysql_affected_rows($r_init)) {
      $SQL = "DELETE FROM ".$cfg_Init['EXT_TABLE']." 
      WHERE   (exten='".$this->old_tele_number."') 
        OR    (exten='".$this->old_cn."')";
  
      /* Query and ensure that everything went fine */
      $res =  @mysql_query($SQL,$r_init);
      if(!$res){
        gosa_log(@mysql_error($r_init));
        return(_("Can not remove old entries from initial home server. Please have a look a the gosa logfiles."));
      }

    }//ENDE  old num availiable ...
    @mysql_close($r_init);
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

    $num = $this->telephoneNumber;
    if((isset($numbers[$num]))&&(($numbers[$num]['cn'][0]!=$this->old_cn))){
      if(isset($numbers[$num]['uid'][0])){
        return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['uid'][0]);
      }else{
        return sprintf(_("The specified telephonenumber '%s' is already assigned to '%s'."),$num,$numbers[$num]['cn'][0]);
      }
    }
  }


  /* Save to LDAP */
  function save()
  {

    if((!isset($this->attrs['goFonPIN']))){
      $pin_use = false;
    }else{
      $pin_use = true;
    }

    plugin::save();

    if(empty($this->old_tele_number)){
      $this->old_tele_number= $this->telephoneNumber;
    }

    $this->SQL_add_me(true);

    if(empty($this->goFonConferenceOption_P)){
      if($pin_use){
        $this->attrs['goFonPIN']=array();
      }else{
        unset($this->attrs['goFonPIN']);
      }
    }
    $this->attrs['goFonConferenceOption']="";
    foreach(array("goFonConferenceOption_P","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
          "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_D","goFonConferenceOptionFormat") as $attrs){
      $this->attrs['goFonConferenceOption'] .= $this->$attrs;
      unset($this->attrs[$attrs]);         
    }

    $this->attrs['goFonConferenceOption'].="|".$this->goFonConferenceOptionLifetime;
    $this->attrs['goFonConferenceOption'].="|".$this->language;
    unset($this->attrs['goFonConferenceOptionLifetime']);

    /* Write back to ldap */

    unset($this->attrs['base']);

    foreach($this->attributes as $atr){
      if(chkacl($this->acl, $atr)!=""){
        unset($this->attrs[$atr]);
      }
    }

    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->cat($this->dn, array('dn'));

    if ($ldap->count()){
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs); 
      $this->handle_post_events('modify');
    } else {
      $ldap->create_missing_trees(preg_replace('/^[^,]+,/', '', $this->dn));
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events('add');
    }
    show_ldap_error($ldap->get_error(), _("Saving phone conference failed"));

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
