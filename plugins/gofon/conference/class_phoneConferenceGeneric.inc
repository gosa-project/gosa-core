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

  var $goFonConferenceOption   	= "";
  var $goFonConferenceOption_D 	= "";   // Set PIN 
  var $goFonConferenceOption_r 	= "";   // record Conference
  var $goFonConferenceOption_M 	= "";   // Play Music opn hold
  var $goFonConferenceOption_s 	= "";   // activate menu
  var $goFonConferenceOption_i 	= "";   // announce new and leaving user
  var $goFonConferenceOption_c 	= "";   // Count User
  var $goFonConferenceOption_P 	= "";   // Conference Type, no PIN/PIN

  var $goFonConferenceOptionFormat 		= "";
  var $goFonConferenceOptionLifetime 	= "";
  var $goFonConferenceOptionNumber 		= "";

  var $old_tele_number	        = false;
  var $generate_error           = "";

  /* Headpage attributes */
  var $last_dep_sorting= "invalid";
  var $departments= array();

  /* attribute list for save action */
  var $attributes= array("cn", "description", "goFonPIN","goFonConferenceOption_D","goFonConferenceOption_r",
						 "goFonConferenceOption_M","goFonConferenceOption_s","goFonConferenceOption_i","goFonConferenceOption_c",
						 "goFonConferenceOption_P","goFonConferenceOptionFormat","goFonConferenceOptionLifetime","goFonConferenceOptionNumber");

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


    /* Parse Options ... 
     * Parameter|Lifetime|number
     */
    if($this->dn!="new"){
      $tmp1= split("\|",$this->attrs['goFonConferenceOption'][0]);

      for($i = 0 ; $i < strlen($tmp1[0]);$i++){
        $varname = "goFonConferenceOption_".$tmp1[0][$i];
        $this->$varname = $tmp1[0][$i];    
      }

      $this->goFonConferenceOptionLifetime  = $tmp1[1];
      $this->goFonConferenceOptionNumber    = $tmp1[2];
      $this->old_tele_number                = $tmp1[2];
    }
  }

  function execute()
  {
	$this->config->departments    = get_departments($this->dn);
	$this->config->make_idepartments();
	$smarty= get_smarty();

	$smarty->assign("goFonConferenceOptions",		    array("P"=>"Conference ",""=>"Conference without PIN"));
	$smarty->assign("goFonConferenceOptionFormats",	array("wav"=>"Wave","gsm"=>"GSM","wav49"=>"Wave49"));
	$smarty->assign("goFonConferenceOption",        $this->goFonConferenceOption_P);
	
	foreach ($this->attributes as $val){
		$smarty->assign("$val", $this->$val);
		$smarty->assign("$val"."ACL", chkacl($this->acl, "$val"));
		if(!$this->$val){
			$smarty->assign($val."CHK", "");
		}else{
			$smarty->assign($val."CHK", " checked ");
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
      foreach(array("goFonConferenceOption_D","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_P") as $attrs){
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
  
  if(empty($this->cn)){
    $message[] =_("Please enter a name for the conference.");
  }
  
  if(!is_numeric($this->goFonConferenceOptionNumber)){
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
    // Get Configuration for Mysql database Server
    $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];

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
   
    if((!empty($this->goFonConferenceOptionNumber))&&($save==true)){
      
      $EXT=array();

      $parameter  ="";
      foreach(array("goFonConferenceOption_D","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_P") as $attrs){
        $parameter .= $this->$attrs;
      }

      $i=1;
      $context="GOsa";
      // Set Language to German
      $EXT[$i]['exten']   =$this->goFonConferenceOptionNumber;
      $EXT[$i]['context'] = $context;
      $EXT[$i]['priority']= $i;
      $EXT[$i]['app']     ="SetLanguage";
      $EXT[$i]['appdata'] ="de";
      $i++;      

      if($this->goFonConferenceOption_r == "r"){

      // Recordingformat for conference
      $EXT[$i]['exten']   =$this->goFonConferenceOptionNumber;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']= $i;
      $EXT[$i]['app']     ="Setvar";
      $EXT[$i]['appdata'] ="MEETME_RECORDINGFORMAT=".$this->goFonConferenceOptionFormat;
      $i++;      

      }

      // Answer Call
      $EXT[$i]['exten']   =$this->goFonConferenceOptionNumber;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']=$i;
      $EXT[$i]['app']     ="answer";
      $EXT[$i]['appdata'] ="";
      $i++;      

      // Start Conference 
      $EXT[$i]['exten']   =$this->goFonConferenceOptionNumber;
      $EXT[$i]['context'] =$context;
      $EXT[$i]['priority']=$i;
      $EXT[$i]['app']     ="MeetMe";
      $EXT[$i]['appdata'] =$this->goFonConferenceOptionNumber."|".$parameter."|".$this->goFonPIN;
      
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
    


    return(true);
  }

  function SQL_remove_me($save){

    if($this->old_tele_number){
      // Get Configuration for Mysql database Server
      $a_SETUP = $_SESSION['config']->data['SERVERS']['FON'];

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

      $SQL = "DELETE FROM ".$a_SETUP['EXT_TABLE']." WHERE (exten= '".$this->old_tele_number."') OR (exten='".$this->goFonConferenceOptionNumber."')";
      
      if($save){
        @mysql_query($SQL);
      } 

    }//ENDE  old num availiable ...
    return(true);
  }


  /* Save to LDAP */
  function save()
  {
	plugin::save();

  $this->SQL_remove_me(true);
  $this->SQL_add_me(true);

  $this->attrs['goFonConferenceOption']="";
  foreach(array("goFonConferenceOption_D","goFonConferenceOption_r","goFonConferenceOption_M","goFonConferenceOption_s",
            "goFonConferenceOption_i","goFonConferenceOption_c","goFonConferenceOption_P","goFonConferenceOptionFormat") as $attrs){
    $this->attrs['goFonConferenceOption'] .= $this->$attrs;
    unset($this->attrs[$attrs]);         
  }

  $this->attrs['goFonConferenceOption'].="|".$this->goFonConferenceOptionLifetime;
  unset($this->attrs['goFonConferenceOptionLifetime']);

  $this->attrs['goFonConferenceOption'].="|".$this->goFonConferenceOptionNumber;
  unset($this->attrs['goFonConferenceOptionNumber']);
	

  /* Write back to ldap */
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
