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

class goFonMacro extends plugin
{
  /* Definitions */
  var $plHeadline		    = "Phone macros";
  var $plDescription	  = "This does something";

  /* Dialog attributes */
  var $macrotabs		    = NULL;
  var $macros		        = array();
  var $ui				        = NULL;
  var $DivListMacro     = NULL;

  var $CopyPasteHandler = NULL;
  var $start_pasting_copied_objects = FALSE;

  function gofonMacro(&$config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    $this->DivListMacro = new divListMacro($this->config,$this);

    /* Copy & Paste enabled ?*/
    if((isset($this->config->data['MAIN']['ENABLECOPYPASTE']))&&(preg_match("/true/i",$this->config->data['MAIN']['ENABLECOPYPASTE']))){
      $this->CopyPasteHandler = new CopyPasteHandler($this->config);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^goFonMacro_/","/^act$/","/^id$/","/^item_selected/","/^remove_multiple_macros/");

    /*****************
      Variable initialisation
     *****************/

    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty= get_smarty();


    /*****************
      Check Posts 
     *****************/

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/^goFonMacro_del/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/^goFonMacro_del_/i","",$key);
        // Post for edit
      }elseif(preg_match("/^goFonMacro_edit_/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/^goFonMacro_edit_/i","",$key);
        // Post for new
      }elseif(preg_match("/^goFonMacro_new/",$key)){
        $s_action="new";
      }elseif(preg_match("/^remove_multiple_macros/",$key)){
        $s_action="del_multiple";
      }elseif(preg_match("/^editPaste.*/i",$key)){
        $s_action="editPaste";
      }elseif(preg_match("/^copy_.*/",$key)){
        $s_action="copy";
        $s_entry  = preg_replace("/^copy_/i","",$key);
      }elseif(preg_match("/^cut_.*/",$key)){
        $s_action="cut";
        $s_entry  = preg_replace("/^cut_/i","",$key);
      }elseif(preg_match("/^multiple_copy_objects/",$key)){
        $s_action = "copy_multiple";
      }elseif(preg_match("/^multiple_cut_objects/",$key)){
        $s_action = "cut_multiple";
      }
    }
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }
    $s_entry  = preg_replace("/_.$/","",$s_entry);


    /********************
      Copy & Paste Handling  ...
     ********************/

    
    /* handle C&P from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^multiple_copy_systems/",$_POST['menu_action'])){
      $s_action = "copy_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^multiple_cut_systems/",$_POST['menu_action'])){
      $s_action = "cut_multiple";
    }
    if(isset($_POST['menu_action']) && preg_match("/^editPaste/",$_POST['menu_action'])){
      $s_action = "editPaste";
    }

    /* Create options */
    if(isset($_POST['menu_action']) && $_POST['menu_action'] == "goFonMacro_new"){
      $s_action = "new";
    }

    /* handle remove from layers menu */
    if(isset($_POST['menu_action']) && preg_match("/^remove_multiple/",$_POST['menu_action'])){
      $s_action = "del_multiple";
    }

    /* Display the copy & paste dialog, if it is currently open */
    $ret = $this->copyPasteHandling_from_queue($s_action,$s_entry);
    if($ret){
      return($ret);
    }


    /*****************
      Create a new Macro 
     *****************/

    /* New macro? */
    if ($s_action=="new"){

      /* By default we set 'dn' to 'new', all relevant plugins will
         react on this. */
      $this->dn= "new";

      /* Create new usertab object */
      $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn,"gofonmacro");
      $this->macrotabs->set_acl_base($this->DivListMacro->selectedBase);
    }

    /*****************
      Edit || Password canceled  
     *****************/

    /* Cancel dialogs */
    if (isset($_POST['edit_cancel'])){
      del_lock ($this->macrotabs->dn);
      unset ($this->macrotabs);
      $this->macrotabs= NULL;
      session::un_set('objectinfo');
    }


    /*****************
      Edit finised
     *****************/

    /* Finish mac edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish']) || isset($_POST['edit_apply'])) && (isset($this->macrotabs->config))){

      /* Check tabs, will feed message array */
      $this->macrotabs->last= $this->macrotabs->current;
      $this->macrotabs->save_object();
      $message= $this->macrotabs->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save data data to ldap */
        $this->macrotabs->save();

        if (!isset($_POST['edit_apply'])){
          /* macro has been saved successfully, remove lock from
             LDAP. */
          if ($this->dn != "new"){
            del_lock ($this->dn);
          }

          unset ($this->macrotabs);
          $this->macrotabs= NULL;
          session::un_set('objectinfo');
        }
      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }
    }


    /*****************
      Edit macro 
     *****************/

    /* User wants to edit data */
    if (($s_action=="edit") && (!isset($this->macrotabs->config))){

      $dn  = $this->macros[$s_entry]['dn'];
      $acl = $this->ui->get_permissions($dn,"gofonmacro/macro");
      if(preg_match("/r/",$acl)){
        $this->dn = $dn;

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so everyone will get the
           above dialog */
        add_lock ($this->dn, $this->ui->dn);
        /* Register macrotabs to trigger edit dialog */
        $this->macrotabs= new macrotabs($this->config,
            $this->config->data['TABS']['MACROTABS'], $this->dn,"gofonmacro");
        $_SESSION['objectinfo']= $this->dn;
      }
    }



    /********************
      Delete MULTIPLE entries requested, display confirm dialog
     ********************/

    if ($s_action=="del_multiple"){
      $ids = $this->list_get_selected_items();

      if(count($ids)){

        foreach($ids as $id){
          $dn = $this->macros[$id]['dn'];
          if (($user= get_lock($dn)) != ""){
            return(gen_locked_message ($user, $dn));
          }
          $this->dns[$id] = $dn;
        }

        $dns_names = "<br><pre>";
        foreach($this->dns as $dn){
          add_lock ($dn, $this->ui->dn);
          $dns_names .= $dn."\n";
        }
        $dns_names .="</pre>";

        /* Lock the current entry, so nobody will edit it during deletion */
        $smarty->assign("intro",     sprintf(_("You're about to delete the following user(s) %s"), @LDAP::fix($dns_names)));
        $smarty->assign("multiple", true);
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      }
    }


    /********************
      Delete MULTIPLE entries confirmed
     ********************/

    /* Confirmation for deletion has been passed. Users should be deleted. */
    if (isset($_POST['delete_multiple_macro_confirm'])){

      /* Remove user by user and check acls before removeing them */
      foreach($this->dns as $key => $dn){

        $acl = $this->ui->get_permissions($dn,"gofonmacro/macro");
        if(preg_match("/d/",$acl)){

          /* Delete request is permitted, perform LDAP action */
          $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $dn,"gofonmacro");
          $this->macrotabs->delete ();
          unset ($this->macrotabs);
          $this->macrotabs= NULL;

        } else {
          print_red (_("You are not allowed to delete this macro!"));
        }

        /* Remove lock file after successfull deletion */
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /********************
      Delete MULTIPLE entries Canceled
     ********************/

    /* Remove lock */
    if(isset($_POST['delete_multiple_macro_cancel'])){
      foreach($this->dns as $key => $dn){
        del_lock ($dn);
        unset($this->dns[$key]);
      }
    }


    /*****************
      Remove macro
     *****************/

    /* Remove user was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted 'uid' */
      $dn  = $this->macros[$s_entry]['dn'];
      $acl = $this->ui->get_permissions($dn,"gofonmacro/macro");

      if(preg_match("/d/",$acl)){

        $this->dn = $dn;
  
        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return (gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty= get_smarty();
        $smarty->assign("intro", sprintf(_("You're about to delete the macro '%s'."), $this->dn));
        $smarty->assign("multiple", false);
        return($smarty->fetch (get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this macro!"));
      }
    }


    /*****************
      Remove macro
     *****************/

    /* Confirmation for deletion has been passed. Macro should be deleted. */
    if (isset($_POST['delete_macro_confirm'])){

      $acl = $this->ui->get_permissions($this->dn,"gofonmacro/macro");

      if(preg_match("/r/",$acl)){

        /* Delete request is permitted, perform LDAP action */
        $this->macrotabs= new macrotabs($this->config,$this->config->data['TABS']['MACROTABS'], $this->dn,"gofonmacro");
        $this->macrotabs->delete ();
        unset ($this->macrotabs);
        $this->macrotabs= NULL;
      } else {
        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this macro!"));
        new log("security","gofonmacro/".get_class($this),$this->dn,array(),"Tried to trick deletion.");
      }
      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /*****************
      Display open dialogs  
     *****************/

    /* Show tab dialog if object is present */
    if (($this->macrotabs) && (isset($this->macrotabs->config))){
      $display= $this->macrotabs->execute();

      /* Don't show buttons if tab dialog requests this */
      if (!$this->macrotabs->by_object[$this->macrotabs->current]->dialog){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" style=\"width:80px\" value=\""._("Ok")."\">\n";
        $display.= "&nbsp;\n";
        if ($this->dn != "new"){
          $display.= "<input type=submit name=\"edit_apply\" value=\""._("Apply")."\">\n";
          $display.= "&nbsp;\n";
        }
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /*****************
      Display entry list 
     *****************/

    /* Check if there is a snapshot dialog open */
    $base = $this->DivListMacro->selectedBase;
    if($str = $this->showSnapshotDialog($base,$this->get_used_snapshot_bases())){
      return($str);
    }

    /* Return rendered main page */
    /* Display dialog with system list */
    $this->DivListMacro->parent = $this;
    $this->DivListMacro->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListMacro->SubSearch){
      $this->DivListMacro->AddDepartments($this->DivListMacro->selectedBase,4,1);
    }
    $this->reload();
    $this->DivListMacro->setEntries($this->macros);
    return($this->DivListMacro->Draw());
  }


  function copyPasteHandling_from_queue($s_action,$s_entry)
  {
    /* Check if Copy & Paste is disabled */
    if(!is_object($this->CopyPasteHandler)){
      return("");
    }

    /* Add a single entry to queue */
    if($s_action == "cut" || $s_action == "copy"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();
      $dn = $this->macros[$s_entry]['dn'];
      $this->CopyPasteHandler->add_to_queue($dn,$s_action,"macrotabs","MACROTABS","gofonmacro");
    }

    /* Add entries to queue */
    if($s_action == "copy_multiple" || $s_action == "cut_multiple"){

      /* Cleanup object queue */
      $this->CopyPasteHandler->cleanup_queue();

      /* Add new entries to CP queue */
      foreach($this->list_get_selected_items() as $id){
        $dn = $this->macros[$id]['dn'];

        if($s_action == "copy_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"copy","macrotabs","MACROTABS","gofonmacro");
        }
        if($s_action == "cut_multiple"){
          $this->CopyPasteHandler->add_to_queue($dn,"cut","macrotabs","MACROTABS","gofonmacro");
        }
      }
    }

    /* Start pasting entries */
    if($s_action == "editPaste"){
      $this->start_pasting_copied_objects = TRUE;
    }

    /* Return C&P dialog */
    if($this->start_pasting_copied_objects && $this->CopyPasteHandler->entries_queued()){

      /* Load entry from queue and set base */
      $this->CopyPasteHandler->load_entry_from_queue();
      $this->CopyPasteHandler->SetVar("base",$this->DivListMacro->selectedBase);

      /* Get dialog */
      $data = $this->CopyPasteHandler->execute();

      /* Return dialog data */
      if(!empty($data)){
        return($data);
      }
    }

    /* Automatically disable status for pasting */
    if(!$this->CopyPasteHandler->entries_queued()){
      $this->start_pasting_copied_objects = FALSE;
    }
    return("");
  }



  /* Return departments, that will be included within snapshot detection */
  function get_used_snapshot_bases()
  {
    return(array(get_ou('macroou').$this->DivListMacro->selectedBase));
  }


  function reload()
  {
    /* Set base for all searches */
    $base       = $this->DivListMacro->selectedBase;
    $SubSearch  = $this->DivListMacro->SubSearch;
    $Regex      = $this->DivListMacro->Regex;
    $Filter     = "(&(cn=".$Regex.")(objectClass=goFonMacro))";
    $Flags      = GL_SIZELIMIT;
    $Attrs      = array("cn","description","displayName","goFonMacroVisible");

    /* Prepare for ls or search*/        
    if($SubSearch){
      $Flags |= GL_SUBSEARCH;
    }else{
      $base = get_ou('macroou').$base;
    }

    /* Generate macro list */
    $res= get_list($Filter, "gofonmacro", $base, $Attrs, $Flags);

    $tmp=array();
    foreach($res as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['cn'][0]]=$val;
    }
    ksort($tmp);
    $this->macros=array();
    foreach($tmp as $val){
      $this->macros[]=$val;
    }

    reset ($this->macros);
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListMacro->save_object();
  }


  function list_get_selected_items()
  {
    $ids = array();
    foreach($_POST as $name => $value){
      if(preg_match("/^item_selected_[0-9]*$/",$name)){
        $id   = preg_replace("/^item_selected_/","",$name);
        $ids[$id] = $id;
      }
    }
    return($ids);
  }


  function adapt_from_template($dn)  { }
  function check() { }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
