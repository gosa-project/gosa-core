<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

/* Snap shot dialog class */
class SnapShotDialog extends plugin 
{
    var $config;
    var $attributes= array("CurrentDescription");
    var $CurrentDescription= "";
    var $parent= NULL;
    var $display_restore_dialog= false;		      /* Defines the restore mode */
    var $display_all_removed_objects= false; 		/* Specifies which objects will be listed, all
                                                       snapshots for a single entry or all snapshots
                                                       of already deleted objects  */	
    var $dialog= true;
    var $del_dn= "";
    var $ui;
    var $acl;
    var $dns = array();
    var $snap_shot_bases = array();
    var $last_list = array();


    function SnapShotDialog(&$config, $dn, &$parent)
    {
        plugin::plugin($config,$dn);
        $this->parent 	= &$parent;
        $this->ui 		  = get_userinfo();

        // Prepare lists
        $this->snapList = new sortableListing();
        $this->snapList->setDeleteable(false);
        $this->snapList->setEditable(false);
        $this->snapList->setWidth("100%");
        $this->snapList->setHeight("200px");
        $this->snapList->setHeader(array(_("Date"), _("Name") ));
        $this->snapList->setColspecs(array('140px','*','60px'));
        $this->snapList->setDefaultSortColumn(0);

    }


    /* Show deleted snapshots from these bases */
    function set_snapshot_bases($bases)
    {
        $this->snap_shot_bases = $bases;
    }


    /* Display snapshot dialog */
    function execute()
    {
        plugin::execute();
        $smarty = get_smarty();

        $ui = get_userinfo();
        $once = true;
        foreach($_POST as $name => $value){
            if((preg_match("/^RemoveSnapShot_/",$name)) && ($once)){
                $once = false;
                $entry = preg_replace("/^RemoveSnapShot_/","",$name);
                $entry = base64_decode($entry);
                $found = false;
                foreach($this->last_list as $t_stamp => $obj){
                    if($obj['dn'] == $entry){
                        $found = true;
                        break;
                    }
                }

                if($found){
                    $this->del_dn 	= $entry;
                    $smarty= get_smarty();
                    $smarty->assign("info", sprintf(_("You are about to delete the snapshot %s."), bold(LDAP::fix($this->del_dn))));
                    return($smarty->fetch (get_template_path('removeSnapshots.tpl')));
                }
            }
        }

        /* We must restore a snapshot */

        if($this->display_restore_dialog){

            /* Should we only display all snapshots of already deleted objects 
               or the snapshots for the given object dn */
            $res = array();
            $tmp = array();
            $handler = new SnapshotHandler($this->config);

            if($this->display_all_removed_objects){
                if(count($this->snap_shot_bases)){
                    foreach($this->snap_shot_bases as $dn){
                        $tmp = array_merge($tmp,$handler->getAllDeletedSnapshots($dn,true));
                    }
                }else{
                    $tmp = $handler->getAllDeletedSnapshots($this->snap_shot_bases,true);
                }
            }else{
                $tmp = $handler->Available_SnapsShots($this->dn,true);
            }

            $this->snapList->setAcl('rwcdm');

            $list_of_elements = array();			

            /* Walk through all entries and setup the display text */
            foreach($tmp as $key => $entry){

                /* Check permissions */
                $TimeStamp = $entry['gosaSnapshotTimestamp'][0];
                $list_of_elements[$TimeStamp] = $entry;
            }

            /* Sort generated list */
            krsort($list_of_elements);

            /* Add Elements to list */	
            $this->last_list = $list_of_elements;

            $data = $lData = array();
            foreach($list_of_elements as $entry){
                $actions=  image('images/lists/restore.png','RestoreSnapShot_%KEY',_("Restore snapshot"));
                $actions.= image('images/lists/trash.png','RemoveSnapShot_%KEY',_("Delete snapshot"));
                $time_stamp 	= date(_("Y-m-d, H:i:s"),preg_replace("/\-.*$/","",$entry['gosaSnapshotTimestamp'][0]));
                $display_data 	= $entry['description'][0];
                $data[$entry['dn']] = $entry;
                $lData[$entry['dn']] = array('data'=>
                        array(
                            $time_stamp,
                            htmlentities(utf8_decode(LDAP::fix($display_data))),
                            str_replace("%KEY",base64_encode($entry['dn']), $actions)));
            }		

            $this->snapList->setListData($data, $lData);
            $this->snapList->update();
            $smarty->assign("SnapShotList",$this->snapList->render());
            $smarty->assign("CountSnapShots",count($list_of_elements));
        }

        $smarty->assign("restore_deleted",$this->display_all_removed_objects);
        $smarty->assign("RestoreMode",$this->display_restore_dialog);
        $smarty->assign("CurrentDate",date(_("Y-m-d, H:i:s")));
        $smarty->assign("CurrentDN",LDAP::fix($this->dn));
        $smarty->assign("CurrentDescription",$this->CurrentDescription);
        return($smarty->fetch(get_template_path("snapshotdialog.tpl")));
    }


    function check()
    {
        $message = plugin::check();
        if(!$this->display_restore_dialog){
            if(empty($this->CurrentDescription)){
                $message[]= msgPool::invalid(_("Description"));
            }
        }
        return($message);
    }


    function save_object()
    {	
//        plugin::save_object();
        foreach($this->attributes as $name){
            if(isset($_POST[$name])){
                $this->$name = stripslashes($_POST[$name]);
            }
        }
    }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
