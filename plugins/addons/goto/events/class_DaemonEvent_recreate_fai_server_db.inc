<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_DaemonEvent_reload_ldap_config.inc 9815 2008-03-14 09:54:45Z cajus $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class DaemonEvent_recreate_fai_server_db extends DaemonEvent
{
    var $visible_for = HIDDEN_EVENT;

    public function __construct($config,$data = array())
    {
        DaemonEvent::__construct($config,$data);
        $this->s_Menu_Name  = _("Reload FAI server db"); 
        $this->s_Event_Name = _("Reload FAI server db"); 
        $this->s_Schedule_Action= "gosa_recreate_fai_server_db";
        $this->s_Trigger_Action = "gosa_recreate_fai_server_db";
        $this->s_Queued_Action  = "recreate_fai_release_db";
        $this->s_Menu_Image = "images/edit.png";
        $this->s_List_Image = "images/edit.png";
    }

    public function execute()
    {
        DaemonEvent::execute();
        return("Cannot be displayed");
    }

    public function save_object()
    {
        DaemonEvent::save_object();
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
