<?php

/*
 * $Id: class_sieve.inc,v 1.1 2005/02/21 09:33:01 cajus Exp $ 
 *
 * Copyright 2001 Dan Ellis <danellis@rushmore.com>
 *
 * See the enclosed file COPYING for license information (GPL).  If you
 * did not receive this file, see http://www.fsf.org/copyleft/gpl.html.
 */

// TODO before next release:	remove ::status() and dependencies


define ("F_NO", 0);		
define ("F_OK", 1);
define ("F_DATA", 2);
define ("F_HEAD", 3);

define ("EC_NOT_LOGGED_IN", 0);
define ("EC_QUOTA", 10);
define ("EC_NOSCRIPTS", 20);
define ("EC_UNKNOWN", 255);
/*

   SIEVE-PHP.LIB VERSION 0.0.8

   (C) 2001 Dan Ellis.

   PLEASE READ THE README FILE FOR MORE INFORMATION.

   Basically, this is the first re-release.  Things are much better than before.

Notes:
This program/libary has bugs.
.	This was quickly hacked out, so please let me know what is wrong and if you feel ambitious submit
a patch :).

Todo:
.	Provide better error diagnostics.  			(mostly done with ver 0.0.5)
.	Allow other auth mechanisms besides plain		(in progress)
.	Have timing mechanism when port problems arise.		(not done yet)
.	Maybe add the NOOP function.				(not done yet)
.	Other top secret stuff....				(some done, believe me?)

Dan Ellis (danellis@rushmore.com)

This program is released under the GNU Public License.

You should have received a copy of the GNU Public
License along with this package; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.        

See CHANGES for updates since last release

Contributers of patches:
Atif Ghaffar
Andrew Sterling Hanenkamp <sterling@hanenkamp.com>
 */


class sieve
{
  var $host;
  var $port;
  var $user;
  var $pass;
  var $auth_types;		/* a comma seperated list of allowed auth types, in order of preference */
  var $auth_in_use;		/* type of authentication attempted */

  var $line;
  var $fp;
  var $retval;
  var $tmpfile;
  var $fh;
  var $len;
  var $script;

  var $loggedin;
  var $capabilities;
  var $error;
  var $error_raw;
  var $responses;
  var $options = "";

  //maybe we should add an errorlvl that the user will pass to new sieve = sieve(,,,,E_WARN)
  //so we can decide how to handle certain errors?!?

  //also add a connection type, like PLAIN, MD5, etc...


  function get_response()
  {
    if($this->loggedin == false or feof($this->fp)){
      $this->error = EC_NOT_LOGGED_IN;
      $this->error_raw = "You are not logged in.";
      return false;
    }

    unset($this->response);
    unset($this->error);
    unset($this->error_raw);

    $this->line=fgets($this->fp,1024);
    $this->token = explode(" ", $this->line, 2);

    if($this->token[0] == "NO"){
      /* we need to try and extract the error code from here.  There are two possibilites: one, that it will take the form of:
         NO ("yyyyy") "zzzzzzz" or, two, NO {yyyyy} "zzzzzzzzzzz" */
      $this->x = 0;
      list($this->ltoken, $this->mtoken, $this->rtoken) = explode(" ", $this->line." ", 3);
      if($this->mtoken[0] == "{"){
        while($this->mtoken[$this->x] != "}" or $this->err_len < 1){
          $this->err_len = substr($this->mtoken, 1, $this->x);
          $this->x++;    
        }
#print "<br>Trying to receive $this->err_len bytes for result<br>";
        $this->line = fgets($this->fp,$this->err_len);
        $this->error_raw[]=substr($this->line, 0, strlen($this->line) -2);    //we want to be nice and strip crlf's
        $this->err_recv = strlen($this->line);

        /* Ensure max loop of 1000, to keep the ui from freezing */
        $max = 1000;
        $cur = 0;

        while($this->err_recv < ($this->err_len) && ($cur < $max)){

          $cur ++;
          $this->line 	 = fgets($this->fp,4096);
          $this->err_recv += strlen($this->line);
          $this->error_raw[]=preg_replace("/\r\n/","",$this->line);    //we want to be nice and strip crlf'
        } 
      } /* end if */
      elseif($this->mtoken[0] == "("){
        switch($this->mtoken){
          case "(\"QUOTA\")":
            $this->error = EC_QUOTA;
          $this->error_raw=$this->rtoken;
          break;
          default:
          $this->error = EC_UNKNOWN;
          $this->error_raw=$this->rtoken;
          break;
        } /* end switch */
      } /* end elseif */
      else{
        $this->error = EC_UNKNOWN;
        $this->error_raw = $this->line;
      }     
      return false;

    } /* end if */
    elseif(substr($this->token[0],0,-2) == "OK"){
      return true;
    } /* end elseif */
    elseif($this->token[0][0] == "{"){

      /* Unable wild assumption:  that the only function that gets here is the get_script(), doesn't really matter though */       

      /* the first line is the len field {xx}, which we don't care about at this point */
      $this->line = fgets($this->fp,1024);
      while(substr($this->line,0,2) != "OK" and substr($this->line,0,2) != "NO"){
        $this->response[]=$this->line;
        $this->line = fgets($this->fp, 1024);
      }
      if(substr($this->line,0,2) == "OK")
        return true;
      else
        return false;
    } /* end elseif */
    elseif($this->token[0][0] == "\""){

      /* I'm going under the _assumption_ that the only function that will get here is the listscripts().
         I could very well be mistaken here, if I am, this part needs some rework */

      $this->found_script=false;        

      while(substr($this->line,0,2) != "OK" and substr($this->line,0,2) != "NO"){
        $this->found_script=true;
        list($this->ltoken, $this->rtoken) = explode(" ", $this->line." ",2);
        //hmmm, a bug in php, if there is no space on explode line, a warning is generated...

        if(strcmp(rtrim($this->rtoken), "ACTIVE")==0){
          $this->response["ACTIVE"] = substr(rtrim($this->ltoken),1,-1);  
        }
        else
          $this->response[] = substr(rtrim($this->ltoken),1,-1);
        $this->line = fgets($this->fp, 1024);
      } /* end while */

      return true;

    } /* end elseif */
    else{
      $this->error = EC_UNKNOWN;
      $this->error_raw = $this->line;
      print "<b><i>UNKNOWN ERROR (Please report this line to danellis@rushmore.com to include in future releases): $this->line</i></b><br>";
      return false;
    } /* end else */   
  } /* end get_response() */

  function __construct($host, $port, $user, $pass, $auth="",$options ="", $auth_types="PLAIN DIGEST-MD5")
  {
    $this->host=$host;
    $this->port=$port;
    $this->user=$user;
    $this->pass=$pass;
    if(!strcmp($auth, ""))		/* If there is no auth user, we deem the user itself to be the auth'd user */
      $this->auth = $this->user;
    else
      $this->auth = $auth;
    $this->auth_types=$auth_types;	/* Allowed authentication types */
    $this->fp=0;
    $this->line="";
    $this->retval="";
    $this->tmpfile="";
    $this->fh=0;
    $this->len=0;
    $this->capabilities="";
    $this->loggedin=false;
    $this->error= "";
    $this->error_raw="";
    $this->options = $options;

    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, $this->error_raw,
        "<b>SIEVE: Host: $host:$port. Options: $options - Auth Types: $auth_types </b>");
  }

  function parse_for_quotes($string)
  {
    /* This function tokenizes a line of input by quote marks and returns them as an array */

    $start = -1;
    $index = 0;

    for($ptr = 0; $ptr < strlen($string); $ptr++){
      if($string[$ptr] == '"' and $string[$ptr] != '\\'){
        if($start == -1){
          $start = $ptr;
        } /* end if */
        else{
          $token[$index++] = substr($string, $start + 1, $ptr - $start - 1);
          $found = true;
          $start = -1;
        } /* end else */

      } /* end if */  

    } /* end for */

    if(isset($token))
      return $token;
    else
      return false;
  } /* end function */            

  function status($string)
  {
    //this should probably be replaced by a smarter parser.

    /*  Need to remove this and all dependencies from the class */

    switch (substr($string, 0,2)){
      case "NO":
        return F_NO;		//there should be some function to extract the error code from this line
      //NO ("quota") "You are oly allowed x number of scripts"
      break;
      case "OK":
        return F_OK;
      break;
      default:
      switch ($string[0]){
        case "{":
          //do parse here for {}'s  maybe modify parse_for_quotes to handle any parse delimiter?
          return F_HEAD;
        break;
        default:
        return F_DATA;
        break;
      } /* end switch */
    } /* end switch */
  } /* end status() */

  function sieve_login()
  {

    $this->fp=@fsockopen($this->host,$this->port,$err_id,$err_str);
    if($this->fp == false){

      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,$this->error_raw,
          "<b>SIEVE: Socket connection failed. (".$this->host.":".$this->port.")</b>");

      $this->error_raw = $err_str;
      return false;
    }

    $this->line=fgets($this->fp,1024);

    //Hack for older versions of Sieve Server.  They do not respond with the Cyrus v2. standard
    //response.  They repsond as follows: "Cyrus timsieved v1.0.0" "SASL={PLAIN,........}"
    //So, if we see IMLEMENTATION in the first line, then we are done.

    if(strstr($this->line, "IMPLEMENTATION"))
    {
      //we're on the Cyrus V2 sieve server
      while(sieve::status($this->line) == F_DATA){

        $this->item = sieve::parse_for_quotes($this->line);

        if(strcmp($this->item[0], "IMPLEMENTATION") == 0)
          $this->capabilities["implementation"] = $this->item[1];

        elseif(strcmp($this->item[0], "SIEVE") == 0 or strcmp($this->item[0], "SASL") == 0){

          if(strcmp($this->item[0], "SIEVE") == 0){
            $this->cap_type="modules";
          }else{
            $this->cap_type="auth";
          }

          $this->modules = explode(" ", $this->item[1]);
          if(is_array($this->modules)){
            foreach($this->modules as $this->module)
              $this->capabilities[$this->cap_type][$this->module]=true;
          } /* end if */
          elseif(is_string($this->modules))
            $this->capabilites[$this->cap_type][$this->modules]=true;
        }    
        else{ 
          $this->capabilities["unknown"][]=$this->line;
        }    
        $this->line=fgets($this->fp,1024);

      }// end while
    }
    else
    {
      //we're on the older Cyrus V1. server  
      //this version does not support module reporting.  We only have auth types.
      $this->cap_type="auth";

      //break apart at the "Cyrus timsieve...." "SASL={......}"
      $this->item = sieve::parse_for_quotes($this->line);

      $this->capabilities["implementation"] = $this->item[0];

      //we should have "SASL={..........}" now.  Break out the {xx,yyy,zzzz}
      $this->modules = substr($this->item[1], strpos($this->item[1], "{"),strlen($this->item[1])-1);

      //then split again at the ", " stuff.
      $this->modules = explode(", ", $this->modules);

      //fill up our $this->modules property
      if(is_array($this->modules)){
        foreach($this->modules as $this->module)
          $this->capabilities[$this->cap_type][$this->module]=true;
      } /* end if */
      elseif(is_string($this->modules))
        $this->capabilites[$this->cap_type][$this->module]=true;
    }




    if(sieve::status($this->line) == F_NO){		//here we should do some returning of error codes?
      $this->error=EC_UNKNOWN;

      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,$this->error_raw,
          "<b>SIEVE: Unknown sieve response, giving up.</b>");

      $this->error_raw = "Server not allowing connections.";
      return false;
    }

    /* Loop through each allowed authentication type and see if the server allows the type */
    foreach(explode(" ",$this->auth_types) as $auth_type)
    {
      if (isset($this->capabilities["auth"][$auth_type]))
      {
        /* We found an auth type that is allowed. */
        $this->auth_in_use = $auth_type;
        break;
      }
    }

    /* Sometimes we do not get any authentification methods, assume PLAIN here.
     * #TODO Maybe there is a better solution for this?
     */
    if($this->auth_in_use == ""){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,$this->error_raw,
          "<b>SIEVE: No authentification mechanisms received, try using PLAIN!.</b>");
        $this->auth_in_use  = "PLAIN";
    }

    /* Fill error message if no auth types are present */
    if (($this->options != "tls") && (!isset($this->capabilities["auth"]))){
      $this->error=EC_UNKNOWN;
      $this->error_raw = "No authentication methods found - please check your sieve setup for missing sasl modules";
      return false;
    }

    /* DEBUG */    
    $imp = "";
    if(isset($this->capabilities['implementation'])){
      $imp = $this->capabilities['implementation'];
    }
    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,$imp,
        "<b>SIEVE: Socket connection established. </b>");

    /* call our authentication program */  
    return sieve::authenticate();

  }

  function sieve_logout()
  {
    if($this->loggedin==false)
      return false;

    fputs($this->fp,"LOGOUT\r\n");
    fclose($this->fp);

    @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
        "<b>SIEVE: Logout!</b>");

    $this->loggedin=false;
    return true;
  }

  function sieve_sendscript($scriptname, $script)
  {
    if($this->loggedin==false)
      return false;
    $this->script=stripslashes($script);
    $len=strlen($this->script);
    fputs($this->fp, "PUTSCRIPT \"$scriptname\" {".$len."+}\r\n");
    fputs($this->fp, "$this->script\r\n");

    if(sieve::get_response()){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
          "<b>SIEVE: Script '".$scriptname."' successfully send!</b>");
      return(TRUE);
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,trim($this->error_raw),
          "<b>SIEVE: Script couldn't be send!</b>");
      return(FALSE);
    }
  }  

  //it appears the timsieved does not honor the NUMBER type.  see lex.c in timsieved src.
  //don't expect this function to work yet.  I might have messed something up here, too.
  function sieve_havespace($scriptname, $scriptsize)
  {
    if($this->loggedin==false)
      return false;
    fputs($this->fp, "HAVESPACE \"$scriptname\" $scriptsize\r\n");
    return sieve::get_response();

  }  

  function sieve_setactivescript($scriptname)
  {
    if($this->loggedin==false)
      return false;

    fputs($this->fp, "SETACTIVE \"$scriptname\"\r\n");  

    if(sieve::get_response()){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
          "<b>SIEVE: Set active script '".$scriptname."' was successful!</b>");
      return(TRUE);
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,trim($this->error_raw),
          "<b>SIEVE: Set active script '".$scriptname."' failed!</b>");
      return(FALSE);
    }
  }

  function sieve_getscript($scriptname)
  {
    unset($this->script);
    if($this->loggedin==false)
      return false;

    fputs($this->fp, "GETSCRIPT \"$scriptname\"\r\n");

    if(sieve::get_response()){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
          "<b>SIEVE: Get script '".$scriptname."' was successful!</b>");
      return(TRUE);
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,trim($this->error_raw),
          "<b>SIEVE: Get script '".$scriptname."' failed!</b>");
      return(FALSE);
    }
  }


  function sieve_deletescript($scriptname)
  {
    if($this->loggedin==false)
      return false;

    fputs($this->fp, "DELETESCRIPT \"$scriptname\"\r\n");    

    if(sieve::get_response()){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
          "<b>SIEVE: Delete script '".$scriptname."' was successful!</b>");
      return(TRUE);
    }else{
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,trim($this->error_raw),
          "<b>SIEVE: Delete  script '".$scriptname."' failed!</b>");
      return(FALSE);
    }
  }


  function sieve_listscripts() 
  { 
    fputs($this->fp, "LISTSCRIPTS\r\n"); 
    sieve::get_response();		//should always return true, even if there are no scripts...
    if(isset($this->found_script) and $this->found_script){
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, implode($this->response,", "),
          "<b>SIEVE: List scripts was successful!</b>");
      return true;
    }else{
      $this->error=EC_NOSCRIPTS;	//sieve::getresponse has no way of telling whether a script was found...
      $this->error_raw="No scripts found for this account.";
      @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__, $this->error_raw,
          "<b>SIEVE: List scripts returned no scripts</b>");
      return false;
    }
  }

  function sieve_alive()
  {
    if(!isset($this->fp) or $this->fp==0){
      $this->error = EC_NOT_LOGGED_IN;
      return false;
    }
    elseif(feof($this->fp)){			
      $this->error = EC_NOT_LOGGED_IN;
      return false;
    }
    else
      return true;
  }


  function authenticate()
  {

    /* Check if a TLS bases connection is required 
     */
    if($this->options == "tls"){

      /* Check if PHP supports TLS encryption for sockets 
       */
      if(function_exists("stream_socket_enable_crypto")){

        /* Initiate TLS and get response 
         */
        if (@ fputs ($this->fp, "STARTTLS\r\n") === false) {
          @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"couldn't send data to socket.",
              "<b>SIEVE: TLS couldn't be initiated. </b>");
          $this->error_raw = "fputs() returned 'false'"; return (false);
        } 
        @ $linha = fgets ($this->fp, 1024); 
        if ($linha === false) {
          $this->error_raw = "fgets() returned 'false'"; 
          @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"couldn't read data from socket.",
              "<b>SIEVE: TLS couldn't be initiated. </b>");
          return (false);
        }
        if (! preg_match ("/\\s*OK\\s/i", $linha)) {
          @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"server returned '".trim ($linha)."' instead of 'OK'",
              "<b>SIEVE: TLS couldn't be initiated. </b>");
          $this->error_raw = "expected an 'OK', but server replied: '" . trim ($linha) . "'"; 
          return (false);
        } 
        if (! @ stream_socket_enable_crypto ($this->fp, true, STREAM_CRYPTO_METHOD_TLS_CLIENT)) {
          @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,
              "The socket doesn't seem to support STREAM_CRYPTO_METHOD_TLS_CLIENT",
              "<b>SIEVE: TLS couldn't be initiated. </b>");
          $this->error_raw = "stream_socket_enable_crypto() returned 'false'"; 
          return (false);
        }
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
            "<b>SIEVE: TLS successfully initiated. </b>");
      }
    }

    switch ($this->auth_in_use) {

      case "PLAIN":
      {
        $auth=base64_encode("$this->user\0$this->auth\0$this->pass");

        $this->len=strlen($auth);			
        fputs($this->fp, "AUTHENTICATE \"PLAIN\" {".$this->len."+}\r\n");
        fputs($this->fp, "$auth\r\n");

        $this->line=fgets($this->fp,1024);		
        while(sieve::status($this->line) == F_DATA)
          $this->line=fgets($this->fp,1024);

        if(sieve::status($this->line) == F_NO){
          $this->error_raw = $this->line;
          @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,trim($this->error_raw),
              "<b>SIEVE: Authentication for '".$this->user."-".$this->auth_in_use."' failed.</b>");

          return false;
        }

        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"",
            "<b>SIEVE: Authentication for '".$this->user."-".$this->auth_in_use."' was successful.</b>");
        $this->loggedin=true;
        return true;    
      }break;

      default:
      {
        @DEBUG (DEBUG_MAIL, __LINE__, __FUNCTION__, __FILE__,"Unsupported authentication method '".$this->auth_in_use."'!",
            "<b>SIEVE: Authentication for '".$this->user."' with type '".$this->auth_in_use."' failed.</b>");

        $this->error_raw = "Unsupported authentication method '".$this->auth_in_use."'!";
        return false;
      }   break;

    }//end switch


  }//end authenticate()


}


// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
