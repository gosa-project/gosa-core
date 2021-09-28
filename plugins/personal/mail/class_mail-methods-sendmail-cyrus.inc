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

class mailMethodSendmailCyrus extends mailMethodCyrus
{

  public function fixAttributesOnLoad()
  {
    /* Remove possible local alias from forwarders */
    mailMethodCyrus::fixAttributesOnLoad();
    if(isset($this->parent->attrs['gosaMailForwardingAddress'])){
      $newForwarder= array();
      for($i = 0; $i < $this->parent->attrs['gosaMailForwardingAddress']['count']; $i++){
        $addr = $this->parent->attrs['gosaMailForwardingAddress'][$i];
        if (!preg_match('/^\\\\/', $addr)){
          $newForwarder[]= $addr;
        }
      }
      $newForwarder['count'] = count($newForwarder);
      $this->parent->attrs['gosaMailForwardingAddress'] = $newForwarder;
    }
  }

  public function fixAttributesOnStore()
  {
    mailMethodCyrus::fixAttributesOnStore();

    /* Add local user if checked */
    $uattrib = $this->getUAttrib();
    if (preg_match("/L/", $mailObject->gosaMailDeliveryMode)) {
      if(!isset($this->parent->attrs['gosaMailForwardingAddress'])){
        $this->parent->attrs['gosaMailForwardingAddress'] = array();
      }
      $this->parent->attrs['gosaMailForwardingAddress'][]= "\\".$this->parent->$uattrib;
    }
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
