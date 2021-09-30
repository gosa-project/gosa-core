<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_DaemonEvent_reinstall.inc 19070 2010-07-23 09:34:18Z hickert $$
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

class DaemonEvent_reinstall extends DaemonEvent
{
    var $progress = 0;  
    var $status   = "";  
    var $visible_for = SYSTEM_EVENT;

    public function __construct($config,$data = array())
    {
        DaemonEvent::__construct($config,$data);
        $this->s_Menu_Name  = _("Reinstall");
        $this->s_Event_Name = _("Reinstall");
        $this->s_Schedule_Action = "job_trigger_action_reinstall";
        $this->s_Trigger_Action= "job_trigger_action_reinstall";
        $this->s_Queued_Action= "trigger_action_reinstall";
        $this->s_Menu_Image = "plugins/goto/images/reinstall.png";
        $this->s_List_Image = "plugins/goto/images/reinstall.png";

        if(!$this->is_new()){
            if(isset($data['PROGRESS'])){
                $this->progress = $data['PROGRESS'];
            }
            if(isset($data['STATUS'])){
                $this->status = $data['STATUS'];
            }
        }
    }

    public function execute()
    {
        DaemonEvent::execute();

        $theme = getThemeName();

        $display = $this->get_header();

        $tmp = $this->data;

        /* Check if target add dialog is open */
        if($this->is_target_list_open() && $this->is_new){
            return($this->get_target_add_list());
        }

        $smarty = get_smarty();
        $smarty->assign("status"      , $this->status);
        $smarty->assign("progress"    , $this->progress);
        $smarty->assign("data"        , $this->data);
        $smarty->assign("target_list" , $this->get_target_list());
        $smarty->assign("is_new"      , $this->is_new);
        $smarty->assign("timestamp"   , $this->get_time_select());
        switch ($variable) {
            case 'classic':
                $display.= $smarty->fetch(get_template_path('DaemonEvent_reinstall.tpl', TRUE, dirname(__FILE__)));
                break;
            
            default:
                $display.= $smarty->fetch(get_template_path('DaemonEvent.tpl', TRUE, dirname(__FILE__)));
                break;
        }
        $display.= $this->get_footer();
        return($display);
    }
} 
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
