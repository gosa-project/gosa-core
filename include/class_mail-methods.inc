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


class mailMethod
{
  var $uattrib= "uid";
  
  function mailMethod($config)
  {
    /* Check if the mail account identification attribute
       is overridden in the configuration file
     */
    if(isset($config->current['MAIL_ATTRIB']) && !empty($config->current['MAIL_ATTRIB'])){
      $new_uattrib= strtolower($config->current['MAIL_ATTRIB']);
      if(in_array($new_uattrib,array("mail","uid"))){
        $this->uattrib = $new_uattrib;
      }else{
        trigger_error(sprintf("Unsupported MAIL_ATTRIB in gosa configuration specified, use 'mail' or 'uid', instead of '%s'.",            $new_uattrib));
      }
    }
  }

  function connect($gosaMailServer)
  {
    return (TRUE);
  }

  function disconnect()
  {
  }

  function getQuota($folder)
  {
    return (array('quotaUsage' => 0, 'gosaMailQuota' => 0));
  }

  function getMailboxList($folder, $uid)
  {
    return (array("INBOX"));
  }

  function setQuota($folder, $gosaMailQuota)
  {
    return (TRUE);
  }

  function updateMailbox($folder)
  {
  }

  function deleteMailbox($folder)
  {
    return (TRUE);
  }

  function setSharedFolderPermissions($folder, $permissions)
  {
  }

  function configureFilter($user, $gosaMailDeliveryMode,
      $mail, $gosaMailAlternateAddress,
      $gosaMailMaxSize,
      $gosaSpamMailbox, $gosaSpamSortLevel,
      $gosaVacationMessage)
  {
  }

  function fixAttributesOnLoad(&$mailObject)
  {
  }

  function fixAttributesOnStore(&$mailObject)
  {
  }

  function fixAttributesOnRemove(&$mailObject)
  {
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
