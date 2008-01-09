<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004  Cajus Pollmeier

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

class mailMethodCyrus extends mailMethod
{
  var $mbox= "-";
  var $config;
  var $gosaMailServer= "";
  var $uattrib= "uid";

  function mailMethodCyrus(&$config)
  {
    $this->config= $config->data['SERVERS']['IMAP'];
  }

  function connect($gosaMailServer)
  {
    $cfg=array();

    /* Connect to IMAP server. I don't want to see these warning here... */
    $this->gosaMailServer= $gosaMailServer;
    if (!isset($this->config[$gosaMailServer])){
      msg_dialog::display(_("Warning"), _("Mail server for this account is invalid!"), WARNING_DIALOG);
    } else {
      $cfg= $this->config[$gosaMailServer];
    }
    /* For some reason, hiding errors with @ does not wor here... */
    if(!isset($cfg['connect']))   $cfg['connect']="";
    if(!isset($cfg['admin']))     $cfg['admin']="";
    if(!isset($cfg['password']))  $cfg['password']="";

    /* Setting connect timeout to 10 seconds,
        else the GOsa UI may freeze for 60 seconds.
       (PHP default is 'default_socket_timeout = 60') */
    imap_timeout(1, 10 );

    $this->mbox = @imap_open($cfg['connect'], $cfg['admin'], $cfg['password'], OP_HALFOPEN);

    /* Mailbox reachable? */
    if ($this->mbox === FALSE){
      msg_dialog::display(_("IMAP error"), _("Cannot store mail settings on IMAP server!"), ERROR_DIALOG);
      return (FALSE);
    }
    return (TRUE);
  }

  function disconnect()
  {
    imap_close ($this->mbox);
  }

  function getQuota($folder)
  {
    $result= array('quotaUsage' => '', 'gosaMailQuota' => '');

    /* Load quota settings */
    error_reporting (0);
    $quota_value = @imap_get_quota($this->mbox, $folder);
    if(is_array($quota_value)) {
      if (isset($quota_value["STORAGE"]) && is_array($quota_value["STORAGE"])){
        /* use for PHP >= 4.3 */
        if($quota_value["STORAGE"]['limit'] == 2147483647){
          $result['quotaUsage']=    (int) ($quota_value["STORAGE"]['usage'] / 1024);
          $result['gosaMailQuota']= (int) ($quota_value["STORAGE"]['limit'] );
        }else{
          $result['quotaUsage']=    (int) ($quota_value["STORAGE"]['usage'] / 1024);
          $result['gosaMailQuota']= (int) ($quota_value["STORAGE"]['limit'] / 1024);
        }
      } else {
        /* backward icompatible */
        if($quota_value['usage'] == 2147483647){
          $result['quotaUsage']=    (int) ($quota_value['usage'] / 1024);
          $result['gosaMailQuota']= (int) ($quota_value['limit'] );
        }else{
          $result['quotaUsage']=    (int) ($quota_value['usage'] / 1024);
          $result['gosaMailQuota']= (int) ($quota_value['limit'] / 1024);
        }
      }
    }elseif(!$quota_value){
      /* If there is no quota defined for this account, the function imap_get_quota returns false. */
      return(array("quotaUsage"=>"","gosaMailQuota"=>""));
    }
 
    error_reporting (E_ALL | E_STRICT);
    return ($result);
  }


  /* return all folders of the users mailbox*/
  function getMailboxList($folder, $uid)
  {
    global $config;
    $result = array();

    /* Get domain an mail address if uid is an mail address */
    $domain = "";
    if(preg_match("/@/",$folder)){
      $domain = "@".preg_replace("/^.*@/","",$folder);
      $folder = preg_replace("/@.*$/","",$folder);
    }

    /* Get list of mailboxes for combo box */
    $cfg= $this->config[$this->gosaMailServer];

    /* Create search pattern
         (user/kekse*@domain.de
          user.kekse*@domain.de
          user.kekse*  )
       depending on given folder name) */
    $q = $folder."*".$domain;
    $list = imap_listmailbox($this->mbox, $cfg["connect"], $q);

    /* Create list of returned folder names */
    if (is_array($list)){
      foreach ($list as $val){

        /* Cut domain name */
        $val = preg_replace("/@.*$/","",$val);
        $result[]=preg_replace ("/^.*".normalizePreg($folder)."/","INBOX", mb_convert_encoding($val, "UTF-8", "UTF7-IMAP"));
      }
    }

    /* Append "INBOX" to the folder array if result is empty and request comes from user dialog */
    if(empty($result) && !empty($uid)){
      $result[] = "INBOX";
    }

    return ($result);
  }


  function updateMailbox($folder)
  {
    /* Check if mailbox exists */
    $cfg= $this->config[$this->gosaMailServer];
    $list = imap_listmailbox($this->mbox, $cfg["connect"], $folder);
    if ($list === FALSE){
      if (!imap_createmailbox($this->mbox, $cfg["connect"]. $folder)){
        msg_dialog::display(_("IMAP error"), sprintf(_("Cannot create IMAP mailbox: %s"), '<br><br><i>'.imap_last_error().'</i>'), ERROR_DIALOG);
        return;
      }
    }
  }


  function setQuota($folder, $gosaMailQuota)
  {
    /* Workaround for the php imap extension */
    if (($gosaMailQuota == "") || ($gosaMailQuota== "2147483647")){
      $gosaMailQuota= "2147483647";
    }elseif($gosaMailQuota > 0){
      $gosaMailQuota = $gosaMailQuota *1024;
    }
    
    
    /* Write mail quota */
    if (!imap_set_quota($this->mbox, $folder, $gosaMailQuota)){
      msg_dialog::display(_("IMAP error"), sprintf(_("Cannot modify IMAP mailbox quota: %s"), '<br><br><i>'.imap_last_error().'</i>'), ERROR_DIALOG);
      return (FALSE);
    }
    return (TRUE);
  }


  function setSharedFolderPermissions($folder, $permissions)
  {
    /* Get list of subfolders */
    $folders= $this->getMailboxList($folder, "");
    $folders[]= $folder;
    
    foreach ($folders as $subfolder){

      /* Set shared folder acl's */
      if (function_exists('imap_getacl')){

        /* Remove all acl's for this folder */
        $users= @imap_getacl ($this->mbox, $subfolder);
        if(is_array($users)){
          foreach ($users as $userid => $perms){
            imap_setacl ($this->mbox, $subfolder, $userid, "");
          }
        }
      } else {
        msg_dialog::display(_("Internal error"), _("Cannot remove IMAP ACL: imap_getacl not implemented!"), ERROR_DIALOG);
      }

      /* Set permissions for this folder */
      foreach ($permissions as $user => $acl){
        imap_setacl ($this->mbox, $subfolder, $user, $acl);
      }
    }

  }


  function getSharedFolderPermissions($folder)
  {
    $result= array();

    /* imap_getacl available? */
    if (!function_exists('imap_getacl')){
      msg_dialog::display(_("Internal error"), _("Cannot retrieve IMAP ACL: imap_getacl not implemented!"), ERROR_DIALOG);
    }

    /* Get permissions in case of shared folders */
    else {
      $users= imap_getacl ($this->mbox, $folder);

      foreach ($users as $userid => $perms){
        $result[preg_replace('/^user\./', '', $userid)]= $perms;
      }

    }

    return ($result);
  }


  function deleteMailbox($folder)
  {
    $cfg= $this->config[$this->gosaMailServer];
    imap_setacl ($this->mbox, $folder, $cfg["admin"], "lrswipcda");
    if (!imap_deletemailbox($this->mbox, $cfg["connect"].$folder)){
      msg_dialog::display(_("IMAP error"), sprintf(_('Cannot remove IMAP mailbox: %s'), '<br><br><i>'.imap_last_error().'</i>'), ERROR_DIALOG);
      return (FALSE);
    }
    return (TRUE);
  }


  function configureFilter($user, $gosaMailDeliveryMode,
      $mail, $gosaMailAlternateAddress,
      $gosaMailMaxSize,
      $gosaSpamMailbox, $gosaSpamSortLevel,
      $gosaVacationMessage)
  {
    $cfg= $this->config[$this->gosaMailServer];

    /* Build spamlevel. Spamassassin tags mails with "*" for each integer
       point of spam. So a spam level of 5.3 gets "*****" which can be
       checked easily by spam filters */
    $spamlevel= "";
    for ($i= 0; $i<$gosaSpamSortLevel; $i++){
      $spamlevel .= "*";
    }

    /* Log into the mail server */
    $sieve= new sieve($cfg["sieve_server"], $cfg["sieve_port"], $user,
        $cfg["password"], $cfg["admin"]);

    if (!$sieve->sieve_login()){
      msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot log into SIEVE server: %s"), '<br><br><i>'.to_string($sieve->error_raw).'</i>'), ERROR_DIALOG);
      return;
    }

    /* Load current script from server and remove everything between the comments
       "###GOSA" */
    $script= "";
    if($sieve->sieve_listscripts()){
      if (in_array("gosa", $sieve->response)){

        /* get old GOsa script */
        if(!$sieve->sieve_getscript("gosa")){
          msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot retrieve SIEVE script: %s"), '<br><br><i>'.to_string($sieve->error_raw).'</i>'), ERROR_DIALOG);
          return;
        }

        foreach ($sieve->response as $line){
          if (preg_match ("/^###GOSA/", $line)){
            break;
          }
          $line= rtrim($line);
          if (!preg_match ('/^\s*$/', $line)){
            $script .= $line."\n";
          }
        }

      }
    }

    /* Only create a new one, if it is not empty */
    if (is_integer(strpos($gosaMailDeliveryMode, "R")) ||
        is_integer(strpos($gosaMailDeliveryMode, "C")) ||
        !is_integer(strpos($gosaMailDeliveryMode, "L")) ||
        is_integer(strpos($gosaMailDeliveryMode, "V")) ||
        is_integer(strpos($gosaMailDeliveryMode, "S"))){

      $text= preg_replace('/"/', '\\"', implode ("", file(CONFIG_DIR."/sieve-header.txt")));
      eval ("\$script.=\"$text\";");
    }

    /* Add anti-spam code */
    if (is_integer(strpos($gosaMailDeliveryMode, "S"))){
      $spambox= $gosaSpamMailbox;
      $text= preg_replace('/"/', '\\"', implode ("", file(CONFIG_DIR."/sieve-spam.txt")));
      eval ("\$script.=\"$text\";");
    }

    /* Add "reject due to mailsize" code, message is currently not
       adjustable through GOsa. */
    if (is_integer(strpos($gosaMailDeliveryMode, "R"))){
      $maxsize= $gosaMailMaxSize;
      $text= preg_replace('/"/', '\\"', implode ("", file(CONFIG_DIR."/sieve-mailsize.txt")));
      eval ("\$script.=\"$text\";");
    }

    /* Add vacation information */
    if (is_integer(strpos($gosaMailDeliveryMode, "V"))){

      /* Sieve wants all destination addresses for the
         vacation message, so we've to assemble them from
         mail and mailAlternateAddress */
      $addrlist= "\"".$mail."\"";
      foreach ($gosaMailAlternateAddress as $val){
        $addrlist .= ", \"$val\"";
      }
      $vacmsg= $gosaVacationMessage;
      $text= preg_replace('/"/', '\\"', implode ("", file(CONFIG_DIR."/sieve-vacation.txt")));
      eval ("\$script.=\"$text\";");
    }

    /* If no local delivery is wanted, tell the script to discard the mail */
    if (!is_integer(strpos($gosaMailDeliveryMode, "L"))){
      $text= preg_replace('/"/', '\\"', implode ("", file(CONFIG_DIR."/sieve-discard.txt")));
      eval ("\$script.=\"$text\";");
    }

    /* Just be aware of null scripts... */
    if (!isset ($script)){
      $script= "";
    }

    /* Upload script and make it the default one */
    if (!$sieve->sieve_sendscript("gosa", $script)){
      msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot store SIEVE script: %s"), '<br><br><i>'.to_string($sieve->error_raw).'</i>'), ERROR_DIALOG);
      return;
    }
    if(!$sieve->sieve_setactivescript("gosa")){
      msg_dialog::display(_("SIEVE error"), sprintf(_("Cannot activate SIEVE script: %s"), '<br><br><i>'.to_string($sieve->error_raw).'</i>'), ERROR_DIALOG);
      return;
    }

    $sieve->sieve_logout();
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
