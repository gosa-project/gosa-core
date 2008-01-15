<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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
require "tabs_conference.inc";

class phoneConferenceManagment extends plugin
{
  /* Definitions */
  var $plHeadline				  = "Phone conferences";
  var $plDescription			= "Management";

  /* attribute list for save action */
  var $attributes				  = array();
  var $objectclasses			= array();

  var $conferences			  = array();
  var $conftab            = false;

  var $DivListConference  = NULL;

  /* Initialise Class */
  function phoneConferenceManagment ($config, $ui)
  {
    $this->ui			= $ui;
    $this->dn			= "";
    $this->config		= $config;

    $acl= get_permissions ($ui->dn, $ui->subtreeACL);
    $this->acl= get_module_permission($acl, "conference", $ui->dn);

    $this->DivListConference = new divListConference($this->config,$this);
  }


  /* Execute class and display something */
  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    /***************
      Variable initialisation
     ***************/
  
    /* Reload departments */
    $smarty						          = get_smarty();
    $display 					          = "";
    $s_action 					        = "";  // Will contain an action, like del or edit
    $s_entry  					        = "";  // The entry name for edit delete -...


    /***************
      Check posts  
     ***************/
  
    foreach($_POST as $key => $post){
      if(preg_match("/.*new.*/i",$key)){
        $s_action = "new";
        // Post for delete
      }elseif(preg_match("/conf_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/conf_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
        // Post for edit
      }elseif(preg_match("/conf_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/conf_".$s_action."_/i","",$key);
        $s_entry  = preg_replace("/_.*$/","",$s_entry);
      } 
    }

    /* Edit Entry */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action = "edit";
      $s_entry  = $_GET['id'];
    }


    /***************
      Cancel dialogs 
     ***************/
  
    /* Reset requested? */
    if (isset($_POST['edit_cancel'])){
      if (isset($this->conftab)){
        del_lock ($this->conftab->dn);
        unset ($this->conftab);
      }
      $this->conftab= NULL;
      $this->lognames= array();;
      $this->cn= "";
      unset ($_SESSION['objectinfo']);
    }


    /***************
      Delete 
     ***************/
  
    /* Delete Entry if Posted action (s_action) == del
     * The entry which will be deleted is defined in $s_entry
     */
    if ($s_action =="del"){
      $this->dn= $this->conferences[$s_entry]['dn'];

      /* Check locking */
      if (($conf= get_lock($this->dn)) != ""){
        $_SESSION['dn']= $this->dn;
        return(gen_locked_message($conf, $this->dn));
      } else {
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("info", sprintf(_("You're about to delete the whole LDAP subtree placed under '%s'."), $this->dn));
        $display.= $smarty->fetch (get_template_path('remove.tpl', TRUE));
        return ($display);
      }
    }


    /***************
      Delete confirmed  
     ***************/
  
    /* If department deletion is accepted ...
     * Finally delete department
     */
    if (isset($_POST['delete_department_confirm'])){
      if (chkacl($this->acl, "delete") == ""){
        $this->remove_from_parent();
        gosa_log ("Department object'".$this->dn."' has been removed");
      } else {
        print_red (_("You have no permission to remove this department."));
      }
    }


    /***************
      Edit
     ***************/
  
    /* Edit Entry if Posted action (s_action) == edit
     * The entry which will be edited is defined in $s_entry
     */
    if (($s_action=="edit") && (!isset($this->conftab->config))){

      $this->dn= $this->conferences[$s_entry]['dn'];

      if (($conf= get_lock($this->dn)) != ""){
        return(gen_locked_message ($conf, $this->dn));
      }

      /* Lock the current entry, so everyone will get the  above dialog */
      add_lock ($this->dn, $this->ui->dn);

      /* Register conftab to trigger edit dialog */
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn);
      $_SESSION['objectinfo']= $this->dn;
    }


    /***************
      Create new  
     ***************/
  
    /* Insert new entry*/    
    if($s_action == "new"){
      
      /* Set up the users ACL's for this 'dn' */
      $this->dn= "new";
      $this->conftab= new conferencetabs($this->config,$this->config->data['TABS']['CONFERENCETABS'], $this->dn);
      unset($_SESSION['objectinfo']);
    }


    /***************
      Save entry  
     ***************/
  
    /* Edit finished, check and save changes */
    if ((isset($_POST['edit_finish'])) && (isset($this->conftab->config))){
      /* Check tabs, will feed message array */
      $this->conftab->last= $this->conftab->current;
      $this->conftab->save_object();
      $message= $this->conftab->check();

      if (count($message) == 0){
        if($this->conftab->save() == 1){
          gosa_log ("goFonConference object '".$this->dn."' saving failed.");
          return;
        }
        gosa_log ("goFonConference object '".$this->dn."' has been saved");

        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        del_lock ($this->conftab->dn);;
        unset ($this->conftab);
        $this->conftab= NULL;
        unset ($_SESSION['objectinfo']);
      } else {
        show_errors($message);
      }
    }


    /***************
      Display dialogs   
     ***************/
  
    /* if edit or new, show dialog */
    if(($this->conftab) && (isset($this->conftab->config))){
      $display= $this->conftab->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->conftab->by_object[$this->conftab->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /***************
      display divlist
     ***************/

    /* Return rendered main page */
    /* Display dialog with system list */
    $this->DivListConference->parent = $this;
    $this->DivListConference->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListConference->SubSearch){
      $this->DivListConference->AddDepartments($this->DivListConference->selectedBase,5);
    }
    $this->reload();
    $this->DivListConference->setEntries($this->conferences);
    return($this->DivListConference->Draw());
  }


  /* Reload entries for divlist.
   * reload all conferences for the current base, with the given regex
   */
  function reload()
  {
    $Base             = "ou=conferences,ou=asterisk,ou=configs,ou=systems,".$this->DivListConference->selectedBase;
    $SubSearch        = $this->DivListConference->SubSearch;
    $Regex            = $this->DivListConference->Regex;
    $Flags            = GL_SIZELIMIT ; 
    $Filter           = "(&(|(cn=".$Regex.")(description=".$Regex."))(objectClass=goFonConference))";
    $Attrs            = array("cn","goFonConferenceOwner","goFonPIN","telephoneNumber","description");
 
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }

    $this->conferences= get_list($Filter,  $this->ui->subtreeACL, $Base, $Attrs, $Flags);
  }

  function remove_from_parent()
  {
    $cfg = new conference($this->config, $this->dn);

    $cfg->remove_from_parent();
    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->dn);
    $ldap->recursive_remove();

    /* Optionally execute a command after we're done */
    $this->postremove();

    /* Delete references to object groups */
    $ldap->cd ($this->config->current['BASE']);
    $ldap->search ("(&(objectClass=gosaGroupOfNames)(member=".LDAP::prepare4filter($this->dn)."))", array("cn"));
    while ($ldap->fetch()){
      $og= new conftab($this->config, $ldap->getDN());
      unset($og->member[$this->dn]);
      $og->save ();
    }

  }

  function save_object()
  {
    $this->DivListConference->save_object();
  }

  function remove_lock()
  {
    if (isset($this->dn)){
      del_lock ($this->dn);
    }
  }

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
