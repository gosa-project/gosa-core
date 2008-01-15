<?php
/*****************************************************************************
  newldap.inc - version 1.0
  Copyright (C) 2003 Alejandro Escanero Blanco <alex@ofmin.com>
  Copyright (C) 2004-2006 Cajus Pollmeier <pollmeier@gonicus.de>

  Based in code of ldap.inc of
  Copyright (C) 1998  Eric Kilfoil <eric@ipass.net>
 *****************************************************************************/

define("ALREADY_EXISTING_ENTRY",-10001);
define("UNKNOWN_TOKEN_IN_LDIF_FILE",-10002);
define("NO_FILE_UPLOADED",10003);
define("INSERT_OK",10000);
define("SPECIALS_OVERRIDE", TRUE);

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
  var $max_ldap_query_time = 0;   // 0, empty or negative values will disable this check 

  function LDAP($binddn,$bindpw, $hostname, $follow_referral= FALSE, $tls= FALSE)
  {
    global $config;
    $this->follow_referral= $follow_referral;
    $this->tls=$tls;
    $this->binddn=$this->convert($binddn);

    $this->bindpw=$bindpw;
    $this->hostname=$hostname;

    /* Check if MAX_LDAP_QUERY_TIME is defined */ 
    if(isset($config->data['MAIN']['MAX_LDAP_QUERY_TIME'])){
      $str = $config->data['MAIN']['MAX_LDAP_QUERY_TIME'];
      $this->max_ldap_query_time = (float)($str);
    }

    $this->connect();
  }


  /* Function to replace all problematic characters inside a DN by \001XX, where
     \001 is decoded to chr(1) [ctrl+a]. It is not impossible, but very unlikely
     that this character is inside a DN.
     
     Currently used codes:
      ,   => CO
      \2C => CO
      \22 => TO
      \"  => TO
      \+  => PL
      (   => OB
      )   => CB
      /   => SL                                                                  */
  function convert($dn)
  {
    if (SPECIALS_OVERRIDE == TRUE){
      $tmp= preg_replace(array("/\\\\,/", "/\\\\2C/", "/\(/", "/\)/", "/\//", "/\\\\22/", '/\\\\"/', "/\\\\3D/", "/\\\\2B/", '/\\\\+/'),
                           array("\001CO", "\001CO", "\001OB", "\001CB", "\001SL", "\001TO", "\001TO", "\001GL", "\001PL", "\001PL"),
                           $dn);
      return (preg_replace('/,\s+/', ',', $tmp));
    } else {
      return ($dn);
    }
  }


  /* Function to fix all problematic characters inside a DN by replacing \001XX
     codes to their original values. See "convert" for mor information. 
     ',' characters are always expanded to \, (not \2C), since all tested LDAP
     servers seem to take it the correct way.                                  */
  function fix($dn)
  {
    if (SPECIALS_OVERRIDE == TRUE){
      return (preg_replace(array("/\001CO/", "/\001OB/", "/\001CB/", "/\001SL/", "/\001TO/", "/\001PL/", "/\001GL/"),
                           array("\,", "(", ")", "/", '\"', "\+", "="),
                           $dn));
    } else {
      return ($dn);
    }
  }


  /* Function to fix problematic characters in DN's that are used for search
     requests. I.e. member=....                                               */
  static function prepare4filter($dn)
  {
        return normalizeLdap(preg_replace('/\\\\/', '\\\\\\', @LDAP::fix($dn)));
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
      if ($bid = @ldap_bind($this->cid, $this->fix($this->binddn), $this->bindpw)) {
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
    if (@ldap_bind($ldap, $this->fix($credentials['ADMIN']), $credentials['PASSWORD'])) {
      $this->error = "Success";
      $this->hascon=true;
      $this->reconnect= true;
      return (0);
    } else {
      $this->error = "Could not bind to " . $credentials['ADMIN'];
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
      $this->basedn = $this->convert($dir);
  }

  function getParentDir($basedn = "")
  {
    if ($basedn=="")
      $basedn = $this->basedn;
    else
      $basedn = $this->convert($this->basedn);
    return(ereg_replace("[^,]*[,]*[ ]*(.*)", "\\1", $basedn));
  }

  function search($filter, $attrs= array())
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();

      $start = microtime();
   
      $this->clearResult();
      $this->sr = @ldap_search($this->cid, $this->fix($this->basedn), $filter, $attrs);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;
   
      /* Check if query took longer as specified in max_ldap_query_time */
      if($this->max_ldap_query_time){
        $diff = get_MicroTimeDiff($start,microtime());
        if($diff > $this->max_ldap_query_time){
          print_red(sprintf(_("The LDAP server is slow (%.2fs for the last query). This may be responsible for performance breakdowns."),$diff)) ;
        }
      }

      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=search('".$this->fix($this->basedn)."', '$filter')");

      return($this->sr);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function ls($filter = "(objectclass=*)", $basedn = "",$attrs = array("*"))
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $this->clearResult();
      if ($basedn == "")
        $basedn = $this->basedn;
      else
        $basedn= $this->convert($basedn);
  
      $start = microtime();

      $this->sr = @ldap_list($this->cid, $this->fix($basedn), $filter,$attrs);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;

       /* Check if query took longer as specified in max_ldap_query_time */
      if($this->max_ldap_query_time){
        $diff = get_MicroTimeDiff($start,microtime());
        if($diff > $this->max_ldap_query_time){
          print_red(sprintf(_("The ldapserver is answering very slow (%.2f), this may be responsible for performance breakdowns."),$diff)) ;
        }
      }

      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=ls('".$this->fix($basedn)."', '$filter')");

      return($this->sr);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  function cat($dn,$attrs= array("*"))
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $start = microtime();
      $this->clearResult();
      $filter = "(objectclass=*)";
      $this->sr = @ldap_read($this->cid, $this->fix($dn), $filter,$attrs);
      $this->error = @ldap_error($this->cid);
      $this->resetResult();
      $this->hasres=true;
      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=cat('".$this->fix($dn)."')");
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
    $att= array();
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
          $att['dn']= trim($this->convert(@ldap_get_dn($this->cid, $this->re)));
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
          return(trim($this->convert($rv)));
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

      $r = @ldap_mod_del($this->cid, $this->fix($dn), $attrs);
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

      $r = @ldap_mod_replace($this->cid, $this->fix($dn), $attrs);
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
      $start= microtime();
      $r = @ldap_delete($this->cid, $this->fix($deletedn));
      $this->error = @ldap_error($this->cid);
      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=delete('".$this->fix($deletedn)."')");
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  /**
  *  Function rmdir_recursive
  *
  *  Description: Based in recursive_remove, adding two thing: full subtree remove, and delete own node.
  *  Parameters:  The dn to delete
  *  GiveBack:    True on sucessfull , 0 in error, and "" when we don't get a ldap conection
  *
  */

  function rmdir_recursive($deletedn)
  {
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $delarray= array();
        
      /* Get sorted list of dn's to delete */
      $this->ls ("(objectClass=*)",$deletedn);
      while ($this->fetch()){
        $deldn= $this->getDN();
        $delarray[$deldn]= strlen($deldn);
      }
      arsort ($delarray);
      reset ($delarray);

      /* Really Delete ALL dn's in subtree */
      foreach ($delarray as $key => $value){
        $this->rmdir_recursive($key);
      }
      
      /* Finally Delete own Node */
      $r = @ldap_delete($this->cid, $this->fix($deletedn));
      $this->error = @ldap_error($this->cid);
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }

  /* Copy given attributes and sub-dns with attributes to destination dn 
  */
  function copy_FAI_resource_recursive($sourcedn,$destinationdn,$destinationName,$type="branch",$is_first = true,$depth=0)
  {
    error_reporting(E_ALL);
    
    if($is_first){
      echo "<h2>".sprintf(_("Creating copy of %s"),"<i>".@LDAP::fix($sourcedn)."</i>")."</h2>";
    }else{
      if(preg_match("/^ou=/",$sourcedn)){
        echo "<h3>"._("Processing")." <i>".@LDAP::fix($destinationdn)."</i></h3>";
      }else{
        $tmp = split(",",$sourcedn);
        
        echo "&nbsp;<b>"._("Object").":</b> ";

        $deststr = @LDAP::fix($destinationdn);
        if(strlen($deststr) > 96){
          $deststr = substr($deststr,0,96)."...";
        }

        echo $deststr."<br>";
      }
    }

    flush();
    
    if($this->hascon){
      if ($this->reconnect) $this->connect();

      /* Save base dn */
      $basedn= $this->basedn;
      $delarray= array();
     
      /* Check if destination entry already exists */
      $this->cat($destinationdn);

      if($this->count()){
        return;
      }else{
        
        $this->clearResult();

        /* Get source entry */
        $this->cd($basedn);
        $this->cat($sourcedn);
        $attr = $this->fetch();

        /* Error while fetching object / attribute abort*/
        if((!$attr) || (count($attr)) ==0) {
          echo _("Error while fetching source dn - aborted!");
          return;
        }
  
        /* check if this is a department */
        if(in_array("organizationalUnit",$attr['objectClass'])){
          $attr['dn'] = $this->convert($destinationdn);
          $this->cd($basedn);
          $this->create_missing_trees($destinationdn);
          $this->cd($destinationdn);

          /* If is first entry, append FAIbranch to department entry */
          if($is_first){
            $this->cat($destinationdn);
            $attr= $this->fetch();

            /* Filter unneeded informations */
            foreach($attr as $key => $value){
              if(is_numeric($key)) unset($attr[$key]);
              if(isset($attr[$key]['count'])){
                if(is_array($attr[$key])){
                  unset($attr[$key]['count']);
                }
              }
            }
            
            unset($attr['count']);
            unset($attr['dn']);

            /* Add marking attribute */
            $attr['objectClass'][] = "FAIbranch";
            
            /* Add this entry */
            $this->modify($attr);
          }
        }else{

          /* If this is no department */
          foreach($attr as $key => $value){
            if(in_array($key ,array("FAItemplateFile","FAIscript", "gotoLogonScript", "gosaApplicationIcon"))){
              $sr= ldap_read($this->cid, $this->fix($sourcedn), "$key=*", array($key));
              $ei= ldap_first_entry($this->cid, $sr);
              if ($tmp= @ldap_get_values_len($this->cid, $ei,$key)){
                $attr[$key] = $tmp;
              }
            }

            if(is_numeric($key)) unset($attr[$key]);
            if(isset($attr[$key]['count'])){
              if(is_array($attr[$key])){
                unset($attr[$key]['count']);
              }
            }
          }
          unset($attr['count']);
          unset($attr['dn']);

          if(!in_array("gosaApplication" , $attr['objectClass'])){
            if($type=="branch"){
              $attr['FAIstate'] ="branch";
            }elseif($type=="freeze"){
              $attr['FAIstate'] ="freeze";
            }else{
              print_red(_("Unknown FAIstate %s"),$type);
            }
          }elseif(in_array("gosaApplication",$attr['objectClass'])){
            if(!in_array("FAIobject",$attr['objectClass'])){
              $attr['objectClass'][] = "FAIobject";
            }
            $attr['FAIstate'] = $type;
          }

          /* Replace FAIdebianRelease with new release name */
          if(in_array("FAIpackageList" , $attr['objectClass'])){
            $attr['FAIdebianRelease'] = $destinationName;
            if($type=="branch"){
              $attr['FAIstate'] ="branch";
            }elseif($type=="freeze"){
              $attr['FAIstate'] ="freeze";
            }else{
              print_red(_("Unknown FAIstate %s"),$type);
            }
          }

          /* Add entry */
          $this->cd($destinationdn);
          $this->cat($destinationdn);
          $a = $this->fetch();
          if(!count($a)){
            $this->add($attr);
          }

          if($this->error != "Success"){
            /* Some error occurred */
            print "---------------------------------------------";
            print $this->get_error()."<br>";
            print $sourcedn."<br>";
            print $destinationdn."<br>";
            print_a( $attr);
            exit();
          }          
        }
      }

      echo "<script language=\"javascript\" type=\"text/javascript\">scrollDown2();</script>" ;

      $this->ls ("(objectClass=*)",$sourcedn);
      while ($this->fetch()){
        $deldn= $this->getDN();
        $delarray[$deldn]= strlen($deldn);
      }
      asort ($delarray);
      reset ($delarray);

       $depth ++;
      foreach($delarray as $dn => $bla){
        if($dn != $destinationdn){
          $this->cd($basedn);
          $item = $this->fetch($this->cat($dn));
          if(!in_array("FAIbranch",$item['objectClass'])){
            $this->copy_FAI_resource_recursive($dn,str_replace($sourcedn,$destinationdn,$dn),$destinationName,$type,false,$depth);
          } 
        }
      }
    }
    if($is_first){
      echo "<p class='seperator'>&nbsp;</p>";
    }

  }

  function modify($attrs)
  {
    if(count($attrs) == 0){
      return (0);
    }
    if($this->hascon){
      if ($this->reconnect) $this->connect();
      $start= microtime();
      $r = @ldap_modify($this->cid, $this->fix($this->basedn), $attrs);
      $this->error = @ldap_error($this->cid);
      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=modify('$this->basedn')");
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
      $start= microtime();
      $r = @ldap_add($this->cid, $this->fix($this->basedn), $attrs);
      $this->error = @ldap_error($this->cid);
      $this->log("LDAP operation: time=".get_MicroTimeDiff($start,microtime())." operation=add('$this->basedn')");
      return($r ? $r : 0);
    }else{
      $this->error = "Could not connect to LDAP server";
      return("");
    }
  }


  function create_missing_trees($target)
  {
    global $config;

    $real_path= substr($target, 0, strlen($target) - strlen($this->basedn) -1 );

    if ($target == $this->basedn){
      $l= array("dummy");
    } else {
      $l= array_reverse(gosa_ldap_explode_dn($real_path));
    }
    unset($l['count']);
    $cdn= $this->basedn;
    $tag= "";

    /* Load schema if available... */
    $classes= $this->get_objectclasses();

    foreach ($l as $part){
      if ($part != "dummy"){
        $cdn= "$part,$cdn";
      }

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
      if (count ($attrs)){
        /* Catch the tag - if present */
        if (isset($attrs['gosaUnitTag'][0])){
          $tag= $attrs['gosaUnitTag'][0];
        }

      } else {
        $type= preg_replace('/^([^=]+)=.*$/', '\\1', $cdn);
        $param= preg_replace('/^[^=]+=([^,]+),.*$/', '\\1', $cdn);

        $na= array();

        /* Automatic or traditional? */
        if(count($classes)){

          /* Get name of first matching objectClass */
          $ocname= "";
          foreach($classes as $class){
            if (isset($class['MUST']) && $class['MUST'] == "$type"){

              /* Look for first classes that is structural... */
              if (isset($class['STRUCTURAL'])){
                $ocname= $class['NAME'];
                break;
              }

              /* Look for classes that are auxiliary... */
              if (isset($class['AUXILIARY'])){
                $ocname= $class['NAME'];
              }
            }
          }

          /* Bail out, if we've nothing to do... */
          if ($ocname == ""){
            print_red(sprintf(_("Autocreation of subtree failed. No objectClass found for attribute '%s'."), $type));
            echo $_SESSION['errors'];
            exit;
          }

          /* Assemble_entry */
          if ($tag != ""){
            $na['objectClass']= array($ocname, "gosaAdministrativeUnitTag");
            $na["gosaUnitTag"]= $tag;
          } else {
            $na['objectClass']= array($ocname);
          }
          if (isset($classes[$ocname]['AUXILIARY'])){
            $na['objectClass'][]= $classes[$ocname]['SUP'];
          }
          if ($type == "dc"){
            /* This is bad actually, but - tell me a better way? */
            $na['objectClass'][]= 'locality';
          }
          $na[$type]= $param;
          if (is_array($classes[$ocname]['MUST'])){
            foreach($classes[$ocname]['MUST'] as $attr){
              $na[$attr]= "filled";
            }
          }

        } else {

          /* Use alternative add... */
          switch ($type){
            case 'ou':
              if ($tag != ""){
                $na["objectClass"]= array("organizationalUnit", "gosaAdministrativeUnitTag");
                $na["gosaUnitTag"]= $tag;
              } else {
                $na["objectClass"]= "organizationalUnit";
              }
              $na["ou"]= $param;
              break;
            case 'dc':
              if ($tag != ""){
                $na["objectClass"]= array("dcObject", "top", "locality", "gosaAdministrativeUnitTag");
                $na["gosaUnitTag"]= $tag;
              } else {
                $na["objectClass"]= array("dcObject", "top", "locality");
              }
              $na["dc"]= $param;
              break;
            default:
              print_red(sprintf(_("Autocreation of type '%s' is currently not supported. Please report to the GOsa team."), $type));
              echo $_SESSION['errors'];
              exit;
          }

        }
        $this->cd($cdn);
        $this->add($na);
        show_ldap_error($this->get_error(), sprintf(_("Creating subtree '%s' failed."),$cdn));
        if (!preg_match('/success/i', $this->error)){
          return FALSE;
        }
      }
    }

    return TRUE;
  }


  function create_missing_trees_old($target)
  {
    /* Ignore create_missing trees if the base equals target */
    if ($target == $this->basedn){
     return;
    }

    $real_path= substr($target, 0, strlen($target) - strlen($this->basedn) -1 );
    $tmp = ldap_explode_dn($real_path,0);
    if(!$tmp){
      print_red(sprintf(_("The referral url '%s' is missing the ldap base. It should look like this 'ldap://server:port/base'."),$this->fix($this->basedn)));
      return;
    }

    $l= array_reverse($tmp);
    unset($l['count']);
    $cdn= $this->basedn;
    $tag= "";

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
      if (count ($attrs)){
      
        /* Catch the tag - if present */
        if (isset($attrs['gosaUnitTag'][0])){
          $tag= $attrs['gosaUnitTag'][0];
        }

      } else {
        $type= preg_replace('/^([^=]+)=.*$/', '\\1', $cdn);
        $param= preg_replace('/^[^=]+=([^,]+),.*$/', '\\1', $cdn);

        $na= array();
        switch ($type){
          case 'ou':
            if ($tag != ""){
              $na["objectClass"]= array("organizationalUnit", "gosaAdministrativeUnitTag");
              $na["gosaUnitTag"]= $tag;
            } else {
              $na["objectClass"]= "organizationalUnit";
            }
            $na["ou"]= $param;
            break;
          case 'dc':
            if ($tag != ""){
              $na["objectClass"]= array("dcObject", "top", "locality", "gosaAdministrativeUnitTag");
              $na["gosaUnitTag"]= $tag;
            } else {
              $na["objectClass"]= array("dcObject", "top", "locality");
            }
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

  function get_attribute($dn, $name,$r_array=0)
  {
    $data= "";
    if ($this->reconnect) $this->connect();
    $sr= @ldap_read($this->cid, $this->fix($dn), "objectClass=*", array("$name"));

    /* fill data from LDAP */
    if ($sr) {
      $ei= @ldap_first_entry($this->cid, $sr);
      if ($ei) {
        if ($info= @ldap_get_values_len($this->cid, $ei, "$name")){
          $data= $info[0];
        }

      }
    }
    if($r_array==0)
    return ($data);
    else
    return ($info);
  
  
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
      $adderror= $this->get_additional_error();
      if ($adderror != ""){
        $error= $this->error." (".$this->get_additional_error().", ".sprintf(_("while operating on '%s' using LDAP server '%s'"), $this->basedn, $this->hostname).")";
      } else {
        $error= $this->error." (".sprintf(_("while operating on LDAP server %s"), $this->hostname).")";
      }
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
      $ret['ADMIN']= $this->fix($this->binddn);
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

function gen_xls ($dn, $filter= "(objectClass=*)", $attributes= array('*'), $recursive= TRUE,$r_array=0)
  {
    $display= "";

      $this->cd($dn);
      $this->search("$filter");

      $i=0;
      while ($attrs= $this->fetch()){
        $j=0;

        foreach ($attributes as $at){
          $display[$i][$j]= $this->get_attribute($attrs['dn'], $at,$r_array);
          $j++;
        }

        $i++;
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
    $sr= @ldap_read($this->cid, $this->fix($dn), $filter, $name);

    /* Get the first entry */   
    $entry= @ldap_first_entry($this->cid, $sr);

    /* Get all attributes related to that Objekt */
    $atts = array();
    
    /* Assemble dn */
    $atts[0]['name']  = "dn";
    $atts[0]['value'] = array('count' => 1, 0 => $dn);

    /* Reset index */
    $i = 1 ; 
  $identifier = array();
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
    return @ldap_list($this->cid, $this->fix($dn), "(objectClass=*)", array("objectClass"));
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

      $dnn = split (":",$line,2);
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
        $attr   = split(":", $row,2);
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
  
  function get_objectclasses_old()
  {
    $objectclasses = array();

	  # Get base to look for schema 
	  $sr           = @ldap_read ($this->cid, NULL, "objectClass=*", array("subschemaSubentry"));
	  $attr         = @ldap_get_entries($this->cid,$sr);

	  if (!isset($attr[0]['subschemasubentry'][0])){
      $this->error  = @ldap_error($this->cid);
      gosa_log($this->get_error());
      return array();
	  }
	
	  # Get list of objectclasses
	  $nb= $attr[0]['subschemasubentry'][0];
	  $objectclasses= array();
	  $sr= ldap_read ($this->cid, $nb, "objectClass=*", array("objectclasses"));
	  $attrs= ldap_get_entries($this->cid,$sr);
	  if (!isset($attrs[0])){
	    return array();
	  }
	  foreach ($attrs[0]['objectclasses'] as $val){
	    $name= preg_replace("/^.* NAME\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $val);
	    if ($name != $val){
	      $objectclasses[$name]= $val;
	    }
	  }
	  
	  return $objectclasses;
  }


  function get_objectclasses()
  {
    global $config;
    $objectclasses = array();

    if(isset($config) && preg_match("/config/i",get_class($config))){
      if(!isset($config->data['MAIN']['SCHEMA_CHECK']) || !preg_match("/true/i",$config->data['MAIN']['SCHEMA_CHECK'])){
        return($objectclasses);
      }
    }

# Get base to look for schema
    $sr = @ldap_read ($this->cid, NULL, "objectClass=*", array("subschemaSubentry"));
    if(!$sr){
      $sr = @ldap_read ($this->cid, "", "objectClass=*", array("subschemaSubentry"));
    }

    $attr = @ldap_get_entries($this->cid,$sr);
    if (!isset($attr[0]['subschemasubentry'][0])){
      return array();
    }

    /* Get list of objectclasses and fill array */
    $nb= $attr[0]['subschemasubentry'][0];
    $objectclasses= array();
    $sr= ldap_read ($this->cid, $nb, "objectClass=*", array("objectclasses"));
    $attrs= ldap_get_entries($this->cid,$sr);
    if (!isset($attrs[0])){
      return array();
    }
    foreach ($attrs[0]['objectclasses'] as $val){
      if (preg_match('/^[0-9]+$/', $val)){
        continue;
      }
      $name= "OID";
      $pattern= split(' ', $val);
      $ocname= preg_replace("/^.* NAME\s+\(*\s*'([^']+)'\s*\)*.*$/", '\\1', $val);
      $objectclasses[$ocname]= array();

      foreach($pattern as $chunk){
        switch($chunk){

          case '(':
            $value= "";
            break;

          case ')': if ($name != ""){
                      $objectclasses[$ocname][$name]= $this->value2container($value);
                    }
                    $name= "";
                    $value= "";
                    break;

          case 'NAME':
          case 'DESC':
          case 'SUP':
          case 'STRUCTURAL':
          case 'ABSTRACT':
          case 'AUXILIARY':
          case 'MUST':
          case 'MAY':
                    if ($name != ""){
                      $objectclasses[$ocname][$name]= $this->value2container($value);
                    }
                    $name= $chunk;
                    $value= "";
                    break;

          default:  $value.= $chunk." ";
        }
      }

    }

    return $objectclasses;
  }

  function value2container($value)
  {
    /* Set emtpy values to "true" only */
    if (preg_match('/^\s*$/', $value)){
      return true;
    }

    /* Remove ' and " if needed */
    $value= preg_replace('/^[\'"]/', '', $value);
    $value= preg_replace('/[\'"] *$/', '', $value);

    /* Convert to array if $ is inside... */
    if (preg_match('/\$/', $value)){
      $container= preg_split('/\s*\$\s*/', $value);
    } else {
      $container= chop($value);
    }

    return ($container);
  }

  function log($string)
  {
    if (isset($_SESSION['config'])){
      $cfg= $_SESSION['config'];
      if (isset($cfg->current['LDAPSTATS']) && preg_match('/true/i', $cfg->current['LDAPSTATS'])){
        syslog (LOG_INFO, $string);
      }
    }
  }
  
  /* added by Guido Serra aka Zeph <zeph@purotesto.it> */
  function getCn($dn){
    $simple= split(",", $dn);

    foreach($simple as $piece) {
      $partial= split("=", $piece);

      if($partial[0] == "cn"){
        return $partial[1];
      }
    }

  }

  function get_naming_contexts($server, $admin= "", $password= "")
  {
    /* Build LDAP connection */
    $ds= ldap_connect ($server);
    if (!$ds) {
      die ("Can't bind to LDAP. No check possible!");
    }
    ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
    $r= ldap_bind ($ds, $admin, $password);

    /* Get base to look for naming contexts */
    $sr  = @ldap_read ($ds, "", "objectClass=*", array("+"));
    $attr= @ldap_get_entries($ds,$sr);

    return ($attr[0]['namingcontexts']);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
