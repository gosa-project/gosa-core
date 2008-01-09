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

class mailMethodKolab extends mailMethodCyrus
{
  var $mbox= "-";
  var $config;
  var $gosaMailServer= "";
  var $uattrib= "mail";

  var $attribute_map= array("gosaMailAlternateAddress"  =>  "alias",
                            "gosaMailQuota"             =>  "cyrus-userquota",
                            "gosaMailServer"            =>  "kolabHomeServer");

  function mailMethodKolab(&$config)
  {
    $this->config= $config->data['SERVERS']['IMAP'];
  }

  function updateMailbox($folder)
  {
  }

  function setQuota($folder, $gosaMailQuota)
  {
    return (TRUE);
  }

  /* Get quota and divide it by 1024, because in gosa we display in MB
      but we get Kb */
  function getQuota($folder)
  {
    $result= array('quotaUsage' => '', 'gosaMailQuota' => '');

    /* Only use lower case folder names, if folder name is like "@domain.com" */
    if(preg_match("/@/",$folder)){
      $folder = strtolower($folder);
    }

    error_reporting(0);  
  
    /* Load quota settings */
    $quota_value = @imap_get_quota($this->mbox, $folder);
    if(is_array($quota_value)) {
      if (isset($quota_value["STORAGE"]) && is_array($quota_value["STORAGE"])){
        /* use for PHP >= 4.3 */
        $result['quotaUsage']=    (int) ($quota_value["STORAGE"]['usage'] / 1024);
        $result['gosaMailQuota']= (int) ($quota_value["STORAGE"]['limit'] / 1024);
      } else {
        /* backward icompatible */
        $result['quotaUsage']=    (int) ($quota_value['usage'] / 1024);
        $result['gosaMailQuota']= (int) ($quota_value['limit'] / 1024);
      }
    }elseif(!$quota_value){
      return(false);
    }

    error_reporting(E_ALL | E_STRICT); 
 
    return ($result);
  }


  function fixAttributesOnLoad(&$mailObject)
  {
    /* Kolab shared folder names are like ' shared.uid@server.de ' 
        So overwrite uid to match these folder names. Else we can't read quota settings etc. 
        #FIXME is there a better way to detect if it is 'shared.' or 'shared+' or 'kolab+shared.' or what ever ?*/
    if(get_class($mailObject) == "mailgroup"){
     $mailObject->uid = "shared.".$mailObject->uid."@".preg_replace("/^.*@/","",$mailObject->mail);
    }

    /* Convert attributes and objectClasses */
    foreach ($this->attribute_map as $dest => $source){
      /* Hickert 11.11.05 : Alternate email addresses were saved, but not displayed again.
      if (isset($mailObject->attrs[$source])){
        $mailObject->attrs[$dest]= $mailObject->attrs[$source];
        unset ($mailObject->attrs[$source]);
      */

      if (isset($mailObject->attrs[$source])){
        unset($mailObject->attrs[$source]['count']);
        $mailObject->attrs[$dest]= $mailObject->attrs[$source];
        $mailObject->$dest=        $mailObject->attrs[$source];

        /* Ensure that cleanup will recognize the ampped attributes too */
        if(isset($mailObject->saved_attributes)){
          $mailObject->saved_attributes[$dest] = $mailObject->attrs[$source];
          $mailObject->saved_attributes[$source] = $mailObject->attrs[$source];
        }       

        unset ($mailObject->$dest['count']);
        unset ($mailObject->attrs[$source]);
      }
    }

    /* Adjust server name if needed */
    foreach ($mailObject->config->data['SERVERS']['IMAP'] as $srv => $dummy){
      if (preg_match("%".$mailObject->attrs['gosaMailServer'][0]."$%", $srv)){
        $mailObject->attrs['gosaMailServer']= array(0 => $srv, "count" => 1);
        break;
      }
    }
  }


  function fixAttributesOnStore(&$mailObject)
  {
    global $config;
  
    /* If quota is empty, remove quota restrictions by setting quota to 0 */
    if(isset($mailObject->gosaMailQuota) && (empty($mailObject->gosaMailQuota))){
      $mailObject->attrs['gosaMailQuota'] = 0;
    }

    /* Convert attributes and objectClasses */
    foreach ($this->attribute_map as $source => $dest){
      if (isset($mailObject->attrs[$source])){
        $mailObject->attrs[$dest]= $mailObject->attrs[$source];
        unset ($mailObject->attrs[$source]);
      }
    }
    $objectclasses= array();
    foreach ($mailObject->attrs['objectClass'] as $oc){
      if ($oc !=  'kolabInetOrgPerson' && $oc !=  'kolabSharedFolder'){
        $objectclasses[]= $oc;
      }
    }
    $mailObject->attrs['objectClass']= $objectclasses;
    if (in_array("posixGroup", $mailObject->attrs['objectClass'])){
  
      /* Add kolabSharedFoleder Class */
      $mailObject->attrs['objectClass'][]= 'kolabSharedFolder';

      /* Work on acl attribute */
      $new_acl= array();
      foreach ($mailObject->attrs['acl'] as $uacl){

        /* Get user=(mail) & acls  */
        list($user, $acl) = split(" ", $uacl);
      
        /* Add al users which have seperated acls 
           %members% are all users in this group, 
           which have the standard group acl
        */
        if ($user != "%members%"){
          $new_acl[$user]= $uacl;
        } else {
        
          /* All groupmembers will be added */
          $ldap = $config->get_ldap_link();
          $ldap->cd($config->current['BASE']);
          foreach ($mailObject->members as $member){

            /* Get user mail address .... */
            $ldap->search("(&(objectClass=person)(|(uid=".$member.")(mail=".$member.")))",array("mail"));
            $res = $ldap->fetch();

            /* Default mail address is set to uid - 
               So if there is no mail address defined the uid is added 
             */
            $mail = $member;

            /* Use mail address if it is available */
            if(isset($res['mail'][0])){
              $mail = $res['mail'][0];
            }

            /* only append this mail/permission string to acl,
               if there arn't already some (special) configs for this user */
            $found =false;
            foreach($mailObject->imapacl as $mailA => $acl){
              if(strtolower(trim($mailA))==strtolower(trim($mail))){
                $found = true;
              }
            }

            /* Skipp user, with no email adress too */     
            if($member == $mail){
              $found = true;
            }
   
            /* Append new user acl */
            if(!$found){
              $new_acl[$member]= "$mail $acl";
            }

            /* Old line */
            //  $new_acl[$member]= "$member $acl";
          }
        }
      }
 
      /* Save shared folder target */
      $mailObject->attrs['gosaSharedFolderTarget']= "kolab+shared.".$mailObject->mail;

      /* Kolab shared folder names are like ' shared.uid@server.de ' 
        So overwrite uid to match these folder names. Else we can't read quota settings etc. 
        #FIXME is there a better way to detect if it is 'shared.' or 'shared+' or 'kolab+shared.' or what ever ?*/
      $mailObject->uid = "shared.".$mailObject->uid."@".preg_replace("/^.*@/","",$mailObject->mail);
  
      /* Assign new acls */
      $mailObject->attrs['acl']= array();
      foreach ($new_acl as $key => $value){
        $mailObject->attrs['acl'][]= $value;
      }
    } else {
      $mailObject->attrs['objectClass'][]= 'kolabInetOrgPerson';
    }

    /* Remove imap:// tagging */
    $mailObject->attrs['kolabHomeServer']= preg_replace('%imap://%', '', $mailObject->attrs['kolabHomeServer']);
    $mailObject->attrs['gosaMailServer']= $mailObject->attrs['kolabHomeServer'];
    $mailObject->attrs['kolabDeleteFlag']= array();
  }

  function fixAttributesOnRemove(&$mailObject)
  {
    /* Add attribute for object deletion and remove GOsa specific
       values from entry. */
    foreach($this->attribute_map as $kolabAttr){
      $mailObject->attrs[$kolabAttr] = array();
    }  

    /* Only add kolab delete Flag in case of an user.mailAccount */
    if(!in_array("posixGroup", $mailObject->attrs['objectClass'])){
      $mailObject->attrs['kolabDeleteFlag']= preg_replace('%imap://%', '', $mailObject->gosaMailServer);
    }else{
      /* Kolab shared folder names are like ' shared.uid@server.de ' 
        So overwrite uid to match these folder names. Else we can't read quota settings etc. 
        #FIXME is there a better way to detect if it is 'shared.' or 'shared+' or 'kolab+shared.' or what ever ?*/
      $mailObject->uid = "shared.".$mailObject->uid."@".preg_replace("/^.*@/","",$mailObject->mail);
      
    }
  }


  function deleteMailbox($folder)
  {
    /* Remove shared folders and skip removing users. 
       KolabD is not able to remove shared folders yet, so we do it instead */
    if(preg_match("/^shared/",$folder)){
      return mailMethodCyrus::deleteMailbox($folder);
    }
    return (TRUE);
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
