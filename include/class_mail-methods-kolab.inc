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
require_once("class_mail-methods-cyrus.inc");

class mailMethodKolab extends mailMethodCyrus
{
  var $mbox= "-";
  var $config;
  var $gosaMailServer= "";
  var $uattrib= "mail";

  var $attribute_map= array("gosaMailAlternateAddress"  =>  "alias",
                            "gosaMailQuota"             =>  "cyrus-userquota",
                            "gosaMailServer"            =>  "kolabHomeServer");

  function mailMethodKolab($config)
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


  function deleteMailbox($folder)
  {
    return (TRUE);
  }


  function fixAttributesOnLoad(&$mailObject)
  {
    /* Convert attributes and objectClasses */
    foreach ($this->attribute_map as $dest => $source){
      if (isset($mailObject->attrs[$source])){
        $mailObject->attrs[$dest]= $mailObject->attrs[$source];
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
      $mailObject->attrs['objectClass'][]= 'kolabSharedFolder';

      /* Work on acl attribute */
      $new_acl= array();
      foreach ($mailObject->attrs['acl'] as $uacl){
        list($user, $acl) = split(" ", $uacl);
        if ($user != "%members%"){
          $new_acl[$user]= $uacl;
        } else {
          foreach ($mailObject->members as $member){
            $new_acl[$member]= "$member $acl";
          }
        }
      }
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
    unset ($mailObject->attrs['mail']);
    $mailObject->attrs['kolabDeleteFlag']= preg_replace('%imap://%', '', $mailObject->gosaMailServer);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
