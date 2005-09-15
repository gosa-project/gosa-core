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
  var $acl = "";
  var $old_cn         = "";

  var $goFonConferenceOption   	= "";
  var $goFonConferenceOption_P 	= "";   // Set PIN 
  var $goFonConferenceOption_r 	= "";   // record Conference
  var $goFonConferenceOption_M 	= "";   // Play Music opn hold
  var $goFonConferenceOption_s 	= "";   // activate menu
  var $goFonConferenceOption_i 	= "";   // announce new and leaving user
  var $goFonConferenceOption_c 	= "";   // Count User
  var $goFonConferenceOption_D 	= "";   // Conference Type, no PIN/PIN

  var $goFonConferenceOwner    = "";

  var $goFonConferenceOptionFormat 		= "";
  var $goFonConferenceOptionLifetime 	= "";
  var $telephoneNumber 		= "";

  var $old_tele_number	        = false;
  var $generate_error           = "";

  var $old_dn;

  /* Headpage attributes */
  var $last_dep_sorting= "invalid";
  var $departments= array();

  /* attribute list for save action */
  var $attributes= array("cn","base", "description", "goFonPIN","goFonConferenceOption_P","goFonConferenceOption_r",
      "goFonConferenceOption_M","goFonConferenceOption_s","goFonConferenceOption_i","goFonConferenceOption_c",
      "goFonConferenceOption_D","goFonConferenceOptionFormat","goFonConferenceOptionLifetime","telephoneNumber","goFonConferenceOwner");

  var $objectclasses= array("top", "goFonConference");

  function conference ($config, $dn)
  {
    plugin::plugin($config, $dn);
    $this->is_account	= TRUE;
    $this->ui			= get_userinfo();
    $this->dn			= $dn;
    $this->orig_dn		= $dn;
    $this->config		= $config;

    /* Set base */
    if ($this->dn == "new"){
      $ui= get_userinfo();
      if(isset($_SESSION['conferencefilter']['depselect'])){
        $this->base = $_SESSION['conferencefilter']['depselect'];
      }else{
        $this->base= dn2base($ui->dn);
      }
    } else {
      $this->base= preg_replace ("/^[^,]+,/", "", $this->dn);
    }

    $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
    $this->acl= get_module_permission($acl, "goFonConference", $this->dn);
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
      $this->old_tele_number                = $this->telephoneNumber;
    }
    $this->old_dn = $this->dn;
    $this->old_cn = $this->cn;
  }

  function execute()
  {
    //$acl= get_permissions ($this->dn, $this->ui->subtreeACL);
    //$this->acl= get_module_permission($acl, "goFonConference", $this->dn);

    $this->config->departments    = get_departments($this->dn);
    $this->config->make_idepartments();
    $smarty= get_smarty();

    $smarty->assign("bases" ,array_flip($this->config->departments));
    $smarty->assign("base"  ,$this->base);

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


  /* Check values */
  function check()
  {
    $message= array();

    if($this->is_number_used()){
      $message[] =  $this->is_number_used();
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

    if(!is_numeric($this->goFonConferenceOptionLifetime)){
      $message[] =_("Only numbers are allowed in Lifetime.");
    }

    $this->SQL_remove_me(false);
    $this->SQL_add_me(false);

    if(!empty($this->generate_error)){
      $message[]=$this->generate_error;
      $this->generate_error="";
    }

    return $message;
  }


  function SQL_add_me($save){

    if(!isset($_SESSION['config']->data['SERVERS']['FON'])){
      $this->generate_error = _("There is currently no asterisk server defined. Possibly you are missing a server that handles the asterisk management (goFonServer). \n  Your Settings can't be saved to asterisk Database. ");
      return(false);
    }
  
    // Get Configuration for Mysql database Server
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];

    // Connect to DB server
    $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

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

    if((!empty($this->telephoneNumber))&&($save==true)){

      $EXT=array();

      $parameter  ="";
      foreach(array("goFonConferenceOption_P","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_D") as $attrs){
        $parameter .= $this->$attrs;
      }

      $i=1;
      $context="GOsa";
      // Set Language to German
      $EXT[$i]['exten']   =$this->telephoneNumber;
      $EXT[$i]['context'] = $context;
      $EXT[$i]['priority']= $i;
      $EXT[$i]['app']     ="SetLanguage";
      $EXT[$i]['appdata'] ="de";
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
        $SQL[]="INSERT INTO ".$a_SETUP['EXT_TABLE']." (".$s_keys.") VALUES (".$s_values.");"; 
      }
      foreach($SQL as $sqlsyn){
        mysql_query($sqlsyn);
      }
    } 


    @mysql_close($r_con);
    return(true);
  }

  function SQL_remove_me($save){

    if($this->old_tele_number){
      // Get Configuration for Mysql database Server
      $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];

      // Connect to DB server
      $r_con =  @mysql_pconnect($a_SETUP['SERVER'],$a_SETUP['LOGIN'],$a_SETUP['PASSWORD']);

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

      $SQL = "DELETE FROM ".$a_SETUP['EXT_TABLE']." 
      WHERE   (exten='".$this->old_tele_number."') 
        OR    (exten='".$this->telephoneNumber."') 
        OR    (exten='".$this->cn."')  
        OR    (exten='".$this->old_cn."')";

      if($save){
        @mysql_query($SQL);
      } 

    }//ENDE  old num availiable ...
    @mysql_close($r_con);
    return(true);
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

    $this->SQL_remove_me(true);
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
    unset($this->attrs['goFonConferenceOptionLifetime']);

    /* Write back to ldap */

    unset($this->attrs['base']);

    foreach($this->attributes as $atr){
      if(chkacl($this->acl, $atr)!=""){
        unset($this->attrs[$atr]);
      }
    }

    $ldap= $this->config->get_ldap_link();

    $ldap->cat($this->dn);
    $a= $ldap->fetch();
    $ldap->cd($this->dn);
    if (count($a)){
      $ldap->modify($this->attrs);
      $this->handle_post_events('modify');
    } else {
      $ldap->add($this->attrs);
      $this->handle_post_events('add');
    }
    show_ldap_error($ldap->get_error());

    /* Optionally execute a command after we're done */
    $this->postcreate();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
