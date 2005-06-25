<?php
/*****************************************************************************
  newldap.inc - version 1.0
  Copyright (C) 2003 Alejandro Escanero Blanco <alex@ofmin.com>
  Copyright (C) 2004 Cajus Pollmeier <pollmeier@gonicus.de>

  Based in code of ldap.inc of
  Copyright (C) 1998  Eric Kilfoil <eric@ipass.net>
 *****************************************************************************/

define("ALREADY_EXISTING_ENTRY",-10001);
define("UNKNOWN_TOKEN_IN_LDIF_FILE",-10002);
define("NO_FILE_UPLOADED",10003);


define("INSERT_OK",10000);



class LDAP{

  var $hascon   =false;
  var $hasres   =false;
  var $reconnect=false;
  var $tls      = false;
  var $basedn   ="";
  var $cid;
  var $error    = ""; // Any error messages to be returned can be put here
  var $start    = 0; // 0 if we are fetching the first entry, otherwise 1
  var $objectClasses = array(); // Information read from slapd.oc.conf
  var $binddn   = "";
  var $bindpw   = "";
  var $hostname = "";
  var $follow_referral = FALSE;
  var $referrals= array();

  function LDAP($binddn,$bindpw, $hostname, $follow_referral= FALSE, $tls= FALSE)
  {
    $this->follow_referral= $follow_referral;
    $this->tls=$tls;
    $this->binddn=$binddn;
    $this->bindpw=$bindpw;
    $this->hostname=$hostname;
    $this->connect();
  }

  function connect()
  {
    $this->hascon=false;
    $this->reconnect=false;
    if ($this->cid= @ldap_connect($this->hostname)) {
      @ldap_set_option($this->cid, LDAP_OPT_PROTOCOL_VERSION, 3);
      if (function_exists("ldap_set_rebind_proc") && $this->follow_referral) {
        @ldap_set_option($this->cid, LDAP_OPT_REFERRALS, 1);
        @ldap_set_rebind_proc($this->cid, array(&$this, "rebind"));
      }
      if (function_exists("ldap_start_tls") && $this->tls){
        @ldap_start_tls($this->cid);
      }

      $this->error = "No Error";
      if ($bid = @ldap_bind($this->cid, $this->binddn, $this->bindpw)) {
        $this->error = "Success";
        $this->hascon=true;
      } else {
        if ($this->reconnect){
          if ($this->error != "Success"){
            $this->error = "Could not rebind to " . $this->binddn;
          }
        } else {
          $this->error = "Could not bind to " . $this->binddn;
        }
      }
    } else {
      $this->error = "Could not connect to LDAP server";
    }
  }

  function rebind($ldap, $referral)
  {
    $credentials= $this->get_credentials($referral);
    if (@ldap_bind($ldap, $credentials['ADMIN'], $credentials['PASSWORD'])) {
      $this->error = "Success";
      $this->hascon=true;
      $this->reconnect= true;
      return (0);
    } else {
      $this->error = "Could not bind to " . $binddn;
      return NULL;
    }
  }

  function reconnect()
  {
    if ($this->reconnect){
      @ldap_unbind($this->cid);
      $this->cid = NULL;
    }
  }

  function unbind()
  {
    @ldap_unbind($this->cid);
    $this->cid = NULL;
  }

  function disconnect()
  {
    if($this->hascon){
      @ldap_close($this->cid);
      $this->hascon=false;
    }
  }

  function cd($dir)
  {
    if ($dir == "..")
      $this->basedn = $this->getParentDir();
    else
      $this->basedn = $dir;
  }

  function getParentDir($basedn = "")
  {
    if ($basedn=="")
      $basedn = $this->basedn;
    return(ereg_replace("[^,]*[,]*[ ]*(.*)", "\\1", $basedn));
  }

  function search($filter, $attrs= array())
  {
    
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $this->clearResult();
      $this->sr = @ldap_search($this->cid, $this->basedn, $filter, $attrs);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;
      return($this->sr);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function ls($filter = "(objectclass=*)", $basedn = "")
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $this->clearResult();
      if ($basedn == "")
        $basedn = $this->basedn;
      $this->sr = @ldap_list($this->cid, $basedn, $filter);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;
      return($this->sr);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function cat($dn)
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $this->clearResult();
      $filter = "(objectclass=*)";
      $this->sr = @ldap_read($this->cid, $dn, $filter);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;
      return($this->sr);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function set_size_limit($size)
  {
    /* Ignore zero settings */
    if ($size == 0){
      @ldap_set_option($this->cid, LDAP_OPT_SIZELIMIT, 10000000);
    }
    if($this->hascon){
      @ldap_set_option($this->cid, LDAP_OPT_SIZELIMIT, $size);
    } else {
      $this->error = "Could not connect to LDAP server";
    }
  }

  function fetch()
  {
    if($this->hascon){
      if($this->hasres){
        if ($this->start == 0)
        {
          $this->start = 1;
          $this->re= @ldap_first_entry($this->cid, $this->sr);
        } else {
          $this->re= @ldap_next_entry($this->cid, $this->re);
        }
        if ($this->re)
        {
          $att= @ldap_get_attributes($this->cid, $this->re);
          $att['dn']= @ldap_get_dn($this->cid, $this->re);
        }
        $this->error = @ldap_error($this->cid);
        if (!isset($att)){
          $att= array();
        }
        return($att);
      }else{
        $this->error = "Perform a Fetch with no Search";
        return("");
      }
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function resetResult()
  {
    $this->start = 0;
  }

  function clearResult()
  {
    if($this->hasres){
      $this->hasres = false;
      @ldap_free_result($this->sr);
    }
  }

  function getDN()
  {
    if($this->hascon){
      if($this->hasres){

        if(!$this->re)
          {
          $this->error = "Perform a Fetch with no valid Result";
          }
          else
          {
          $rv = @ldap_get_dn($this->cid, $this->re);
        
          $this->error = @ldap_error($this->cid);
          $rv= preg_replace("/[ ]*,[ ]*/", ",", $rv);
          return($rv);
           }
      }else{
        $this->error = "Perform a Fetch with no Search";
        return("");
      }
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function count()
  {
    if($this->hascon){
      if($this->hasres){
        $rv = @ldap_count_entries($this->cid, $this->sr);
        $this->error = @ldap_error($this->cid);
        return($rv);
      }else{
        $this->error = "Perform a Fetch with no Search";
        return("");
      }
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function rm($attrs = "", $dn = "")
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      if ($dn == "")
        $dn = $this->basedn;

      $r = @ldap_mod_del($this->cid, $dn, $attrs);
      $this->error = @ldap_error($this->cid);
      return($r);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function rename($attrs, $dn = "")
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      if ($dn == "")
        $dn = $this->basedn;

      $r = @ldap_mod_replace($this->cid, $dn, $attrs);
      $this->error = @ldap_error($this->cid);
      return($r);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function rmdir($deletedn)
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $r = @ldap_delete($this->cid, $deletedn);
      $this->error = @ldap_error($this->cid);
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function modify($attrs)
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $r = @ldap_modify($this->cid, $this->basedn, $attrs);
      $this->error = @ldap_error($this->cid);
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function add($attrs)
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $r = @ldap_add($this->cid, $this->basedn, $attrs);
      $this->error = @ldap_error($this->cid);
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function create_missing_trees($target)
  {
    
    /* Ignore create_missing trees if the base equals target */
    if ($target == $this->basedn){
     return;
    }
    $l= array_reverse(explode(",", preg_replace("/,".$this->basedn."/", "", $target)));
    $cdn= $this->basedn;
    foreach ($l as $part){
      $cdn= "$part,$cdn";

      /* Ignore referrals */
      $found= false;
      foreach($this->referrals as $ref){
        $base= preg_replace('!^[^:]+://[^/]+/([^?]+).*$!', '\\1', $ref['URL']);
        if ($base == $cdn){
          $found= true;
          break;
        }
      }
      if ($found){
        continue;
      }

      $this->cat ($cdn);
      $attrs= $this->fetch();

      /* Create missing entry? */
      if (!count ($attrs)){
        $type= preg_replace('/^([^=]+)=.*$/', '\\1', $cdn);
        $param= preg_replace('/^[^=]+=([^,]+),.*$/', '\\1', $cdn);

        $na= array();
        switch ($type){
          case 'ou':
            $na["objectClass"]= "organizationalUnit";
            $na["ou"]= $param;
            break;
          case 'dc':
            $na["objectClass"]= array("dcObject", "top", "locality");
            $na["dc"]= $param;
            break;
          default:
            print_red(sprintf(_("Autocreation of type '%s' is currently not supported. Please report to the GOsa team."), $type));
            echo $_SESSION['errors'];
            exit;
        }
        $this->cd($cdn);
        $this->add($na);
      }
    }
  }

  function recursive_remove()
  {
    $delarray= array();

    /* Get sorted list of dn's to delete */
    $this->search ("(objectClass=*)");
    while ($this->fetch()){
      $deldn= $this->getDN();
      $delarray[$deldn]= strlen($deldn);
    }
    arsort ($delarray);
    reset ($delarray);

    /* Delete all dn's in subtree */
    foreach ($delarray as $key => $value){
      $this->rmdir($key);
    }
  }

  function get_attribute($dn, $name)
  {
    $data= "";
    if ($this->reconnect) $this->connect();
    $sr= @ldap_read($this->cid, $dn, "objectClass=*", array("$name"));

    /* fill data from LDAP */
    if ($sr) {
      $ei= @ldap_first_entry($this->cid, $sr);
      if ($ei) {
        if ($info= @ldap_get_values_len($this->cid, $ei, "$name")){
          $data= $info[0];
        }
      }
    }

    return ($data);
  }

  function get_additional_error()
  {
    $error= "";
    @ldap_get_option ($this->cid, LDAP_OPT_ERROR_STRING, $error);
    return ($error);
  }

  function get_error()
  {
    if ($this->error == 'Success'){
      return $this->error;
    } else {
      $error= $this->error." (".$this->get_additional_error().")";
      return $error;
    }
  }

  function get_credentials($url, $referrals= NULL)
  {
    $ret= array();
    $url= preg_replace('!\?\?.*$!', '', $url);
    $server= preg_replace('!^([^:]+://[^/]+)/.*$!', '\\1', $url);

    if ($referrals == NULL){
      $referrals= $this->referrals;
    }

    if (isset($referrals[$server])){
      return ($referrals[$server]);
    } else {
      $ret['ADMIN']= $this->binddn;
      $ret['PASSWORD']= $this->bindpw;
    }

    return ($ret);
  }


  function gen_ldif ($dn, $filter= "(objectClass=*)", $attributes= array('*'), $recursive= TRUE)
  {
    $display= "";

    if ($recursive){
      $this->cd($dn);
      $this->search("$filter", array('dn'));
      while ($attrs= $this->fetch()){
        $display.= $this->gen_one_entry($attrs['dn'], $filter, $attributes);
        $display.= "\n";
      }
    } else {
      $display.= $this->gen_one_entry($dn);
    }

    return ($display);
  }


  function gen_one_entry($dn, $filter= "(objectClass=*)" , $name= array("*"))
  {
    $ret = "";
    $data = "";
    if($this->reconnect){
      $this->connect();
    }

    /* Searching Ldap Tree */
    $sr= @ldap_read($this->cid, $dn, $filter, $name);

    /* Get the first entry */		
    $entry= @ldap_first_entry($this->cid, $sr);

    /* Get all attributes related to that Objekt */
    $atts = array();
    
    /* Assemble dn */
    $atts[0]['name']  = "dn";
    $atts[0]['value'] = array('count' => 1, 0 => $dn);

    /* Reset index */
    $i = 1 ; 

    $attribute= @ldap_first_attribute($this->cid,$entry,$identifier);
    while ($attribute) {
      $i++;
      $atts[$i]['name']  = $attribute;
      $atts[$i]['value'] = @ldap_get_values_len($this->cid, $entry, "$attribute");

      /* Next one */
      $attribute= @ldap_next_attribute($this->cid,$entry,$identifier);
    }

    foreach($atts as $at)
    {
      for ($i= 0; $i<$at['value']['count']; $i++){

        /* Check if we must encode the data */
        if(!preg_match('/^[a-z0-9+@#.=, \/ -]+$/i', $at['value'][$i])) {
          $ret .= $at['name'].":: ".base64_encode($at['value'][$i])."\n";
        } else {
          $ret .= $at['name'].": ".$at['value'][$i]."\n";
        }
      }
    }

    return($ret);
  }


  function dn_exists($dn)
  {
    return @ldap_list($this->cid, $dn, "(objectClass=*)", array("objectClass"));
  }
  


  function import_complete_ldif($str_attr,&$error,$overwrite,$cleanup)
  {
    if($this->reconnect) $this->connect();

    /* First we have to splitt the string ito detect empty lines
       An empty line indicates an new Entry */
    $entries = split("\n",$str_attr);

    $data = "";
    $cnt = 0; 
    $current_line = 0;

    /* Every single line ... */
    foreach($entries as $entry) {
      $current_line ++;

      /* Removing Spaces to .. 
         .. test if a new entry begins */
      $tmp  = str_replace(" ","",$data );

      /* .. prevent empty lines in an entry */
      $tmp2 = str_replace(" ","",$entry);

      /* If the Block ends (Empty Line) */
      if((empty($entry))&&(!empty($tmp))) {
        /* Add collected lines as a complete block */
        $all[$cnt] = $data;
        $cnt ++;
        $data ="";
      } else {

        /* Append lines ... */
        if(!empty($tmp2)) {
          /* check if we need base64_decode for this line */
          if(ereg("::",$tmp2))
          {
            $encoded = split("::",$entry);
            $attr  = $encoded[0];
            $value = base64_decode($encoded[1]);
            /* Add linenumber */
            $data .= $current_line."#".$attr.":".$value."\n";
          }
          else
          {
            /* Add Linenumber */ 
            $data .= $current_line."#".$entry."\n";
          }
        }
      }
    }

    /* The Data we collected is not in the array all[];
       For example the Data is stored like this..

       all[0] = "1#dn : .... \n 
       2#ObjectType: person \n ...."
       
       Now we check every insertblock and try to insert */
    foreach ( $all as $single) {
      $lineone = split("\n",$single);  
      $ndn = split("#", $lineone[0]);
      $line = $ndn[1];

      $dnn = split (":",$line);
      $current_line = $ndn[0];
      $dn    = $dnn[0];
      $value = $dnn[1];

      /* Every block must begin with a dn */
      if($dn != "dn") {
        $error= sprintf(_("This is not a valid DN: '%s'. A block for import should begin with 'dn: ...' in line %s"), $line, $current_line);
        return -2;  
      }

      /* Should we use Modify instead of Add */
      $usemodify= false;

      /* Delete before insert */
      $usermdir= false;
    
      /* The dn address already exists! */
      if (($this->dn_exists($value))&&((!$overwrite)&&(!$cleanup))) {

        $error= sprintf(_("The dn: '%s' (from line %s) already exists in the LDAP database."), $line, $current_line);
        return ALREADY_EXISTING_ENTRY;   

      } elseif(($this->dn_exists($value))&&($cleanup)){

        /* Delete first, then add */
        $usermdir = true;        

      } elseif(($this->dn_exists($value))&&($overwrite)) {
        
        /* Modify instead of Add */
        $usemodify = true;
      }
     
      /* If we can't Import, return with a file error */
      if(!$this->import_single_entry($single,$usemodify,$usermdir) ) {
        $error= sprintf(_("Error while importing dn: '%s', please check your LDIF from line %s on!"), $line,
                        $current_line);
        return UNKNOWN_TOKEN_IN_LDIF_FILE;      }
    }

    return (INSERT_OK);
  }


  /* Imports a single entry */
  function import_single_entry($str_attr,$modify,$delete)
  {
    if($this->reconnect) $this->connect();

    $ret = false;
    $rows= split("\n",$str_attr);
    $data= false;

    foreach($rows as $row) {
      
      /* Check if we use Linenumbers (when import_complete_ldif is called we use
         Linenumbers) Linenumbers are use like this 123#attribute : value */
      if(!empty($row)) {
        if((strpos($row,"#")!=FALSE)&&(strpos($row,"#")<strpos($row,":"))) {

          /* We are using line numbers 
             Because there is a # before a : */
          $tmp1= split("#",$row);
          $current_line= $tmp1[0];
          $row= $tmp1[1];
        }

        /* Split the line into  attribute  and value */
        $attr   = split(":", $row);
        $attr[0]= trim($attr[0]);  /* attribute */
        $attr[1]= trim($attr[1]);  /* value */

        /* Check for attributes that are used more than once */
        if(!isset($data[$attr[0]])) {
          $data[$attr[0]]=$attr[1];
        } else {
          $tmp = $data[$attr[0]];

          if(!is_array($tmp)) {
            $new[0]=$tmp;
            $new[1]=$attr[1];
            $datas[$attr[0]]['count']=1;             
            $data[$attr[0]]=$new;
          } else {
            $cnt = $datas[$attr[0]]['count'];           
            $cnt ++;
            $data[$attr[0]][$cnt]=$attr[1];
            $datas[$attr[0]]['count'] = $cnt;
          }
        }
      }
    } 
    
    /* If dn is an index of data, we should try to insert the data */
    if(isset($data['dn'])) {
      /* Creating Entry */
      $this->cd($data['dn']);

      /* Delete existing entry */
      if($delete){
        $this->rmdir($data['dn']);
      }
      
      /* Create missing trees */
      $this->create_missing_trees($data['dn']);
      unset($data['dn']);
      
      /* If entry exists use modify */
      if(!$modify){
        $ret = $this->add($data);
      } else {
        $ret = $this->modify($data);
      }
    }

    return($ret);
  }

  
  function importcsv($str)
  {
    $lines = split("\n",$str);
    foreach($lines as $line)
    {
      /* continue if theres a comment */
      if(substr(trim($line),0,1)=="#"){
        continue;
      }

      $line= str_replace ("\t\t","\t",$line);
      $line= str_replace ("\t"  ,"," ,$line);
      echo $line;

      $cells = split(",",$line )  ;
      $linet= str_replace ("\t\t",",",$line);
      $cells = split("\t",$line);
      $count = count($cells);  
    }

  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
