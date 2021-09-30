<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_DaemonEvent_wakeup.inc 10484 2008-04-16 06:24:25Z cajus $$
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

class DaemonEvent_goto_reload extends DaemonEvent
{
    public $visible_for = HIDDEN_EVENT;
    public $mac = array();
    protected $attributes = array("mac");

    public function set_macs($macs)
    {
        $this->mac = $macs;
    }

    public function __construct($config, $data = array())
    {
        DaemonEvent::__construct($config, $data);
        $this->s_Menu_Name  = _("GOto reload");
        $this->s_Event_Name = _("Reload GOto settings");
        $this->s_Schedule_Action= "job_trigger_goto_settings_reload";
        $this->s_Trigger_Action = "gosa_trigger_goto_settings_reload";
        $this->s_Queued_Action  = "trigger_action_goto_settings_reload";
        $this->s_Menu_Image = "images/lists/reload.png";
        $this->s_List_Image = "images/lists/reload.png";
    }

    public function execute()
    {
        DaemonEvent::execute();

        $theme = getThemeName();

        $display = $this->get_header();

        $tmp = $this->data;

        /* Check if target add dialog is open */
        if ($this->is_target_list_open() && $this->is_new) {
            return($this->get_target_add_list());
        }

        $smarty = get_smarty();
        $smarty->assign("data", $this->data);
        $smarty->assign("target_list", $this->get_target_list());
        $smarty->assign("is_new", $this->is_new);
        $smarty->assign("timestamp", $this->get_time_select());
        switch ($theme) {
            case 'classic':
                $display.= $smarty->fetch(get_template_path('DaemonEvent_goto_reload.tpl', true, dirname(__FILE__)));
                break;
            
            default:
                $display.= $smarty->fetch(get_template_path('DaemonEvent.tpl', true, dirname(__FILE__)));
                break;
        }
        $display.= $this->get_footer();
        return($display);
    }

    public function save_object()
    {
        DaemonEvent::save_object();
    }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
