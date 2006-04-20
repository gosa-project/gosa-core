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
require "tabs_terminal.inc";
require "tabs_workstation.inc";
require "tabs_printers.inc";
require "tabs_phone.inc";
require "tabs_server.inc";
require "tabs_component.inc";
require "tabs_winstation.inc";


class systems extends plugin
{
  /* Definitions */
  var $plHeadline= "Systems";
  var $plDescription= "This does something";
  var $departments=array();

  /* Dialog attributes */
  var $systab= NULL;
  var $terminals= array();
  var $ui= NULL;
  var $acl= "";

  function systems ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Get global filter config */
    if (!is_global("terminalfilter")){
      $base = get_base_from_people($ui->dn);
      $terminalfilter= array("workstations" => "checked",
          "thins" => "checked",
          "winstations" => "checked",
          "servers" => "checked",
          "printers" => "checked",
          "phones" => "checked",
          "netdev" => "checked",
          "user" => "*",
          "regex" => "*");
      register_global("terminalfilter", $terminalfilter);
    }
    if(!isset($_SESSION['CurrentMainBase'])){
      $ui   = get_userinfo();
      $_SESSION['CurrentMainBase'] = get_base_from_people($ui->dn);
    }
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^user_edit_/i","/^user_del_/","/^act/","/^id/");

    /********************
      Filter handling, check posted filter options and store them in our Session obejct
     ********************/
  
    /* Save posted filter data */
    $terminalfilter= get_global("terminalfilter");
    if(isset($_POST['SystemHeadpagePosted'])){
      foreach( array("user", "regex") as $type){
        if (isset($_POST[$type])){
          $terminalfilter[$type]= $_POST[$type];
        }
      }
    } 

    if(isset($_POST['CurrentMainBase'])){
      $_SESSION['CurrentMainBase'] = $_POST['CurrentMainBase']; 
    }

    /* Check if filter checkboxes are selected */
    if (isset($_POST['SystemHeadpagePosted'])){
      foreach( array("workstations", "thins", "winstations", "printers", "phones", "servers", "netdev") as $type){
        if (isset($_POST[$type])) {
          $terminalfilter[$type]= "checked";
        } else {
          $terminalfilter[$type]= "";
        }
      }
    }

    /* Check for search post */
    if (isset($_GET['search'])){
      $s= mb_substr($_GET['search'], 0, 1, "UTF8")."*";
      if ($s == "**"){
        $s= "*";
      }
      $terminalfilter['regex']= $s;
    }


    /********************
      Check for functional posts, edit|delete|add|... system devices 
     ********************/
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty       = get_smarty();

    /* check if base was changed */
    if(isset($_GET['act'])&&($_GET['act']=="dep_open")){
            $s_action="open";
            $s_entry = base64_decode($_GET['dep_id']);
            $_SESSION['CurrentMainBase']= $this->config->departments[trim($s_entry)];
            $this->reload();
    }

    /* Test Posts */
    foreach($_POST as $key => $val){
      // Post for delete
      if(preg_match("/user_del.*/",$key)){
        $s_action = "del";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for edit
      }elseif(preg_match("/user_edit_.*/",$key)){
        $s_action="edit";
        $s_entry  = preg_replace("/user_".$s_action."_/i","",$key);
        // Post for new
      }elseif(preg_match("/dep_back.*/i",$key)){
        $s_action="back";
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/dep_home.*/i",$key)){
        $s_action="home";
      }elseif(preg_match("/user_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/user_setpwd_.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_setpwd_/i","",$key);
      }elseif(preg_match("/dep_root.*/i",$key)){
        $s_action="root";
      }elseif(preg_match("/newsystem_.*/i",$key)){
        $s_action="newsystem";
        $s_entry  = preg_replace("/newsystem_/i","",$key);
      }
    }

    /* 09.02.2006 : New incoming handling ; hickert
     * If someone made a systemtype and ogroup selection 
     * Display the new requested entry type ... servtab  in case of server and so on.
     */
    if(isset($_POST['SystemTypeChoosen'])){
      $s_action = "SelectedSystemType";
      $s_entry  = $_POST['SystemType'];
      $this->systab = NULL;
    }

    /* remove image tags from posted entry  (posts looks like this 'name_x')*/
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Edit was requested by pressing the name(link) of an item */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

    /* Department changed? */
    if(isset($_POST['CurrentMainBase']) && $_POST['CurrentMainBase']){
      $_SESSION['CurrentMainBase']= $_POST['CurrentMainBase'];
    }

    /* Homebutton is posted */
    if($s_action=="home"){
      $_SESSION['CurrentMainBase']=(preg_replace("/^[^,]+,/","",$this->ui->dn));
      $_SESSION['CurrentMainBase']=(preg_replace("/^[^,]+,/","",$_SESSION['CurrentMainBase']));
    }

    /* back to the roots ^^ */
    if($s_action=="root"){
      $_SESSION['CurrentMainBase']=($this->config->current['BASE']);
    }

    /* If Backbutton is Posted */
    if($s_action=="back"){
      $base_back          = preg_replace("/^[^,]+,/","", $_SESSION['CurrentMainBase']);
      $base_back          = convert_department_dn($base_back);

      if(isset($this->config->departments[trim($base_back)])){
        $_SESSION['CurrentMainBase']= $this->config->departments[trim($base_back)];
      }else{
        $_SESSION['CurrentMainBase']= $this->config->departments["/"];
      }
    }

    /* Save Termfilter .... */
    register_global("terminalfilter", $terminalfilter);
    $this->reload();
    
    /* Check for exeeded sizelimit */
    if (($message= check_sizelimit()) != ""){
      return($message);
    }

    /* Try to get informations about what kind of system to create */
    if ($s_action=="new") {
      return ($smarty->fetch(get_template_path('chooser.tpl', TRUE)));
    }

    /********************
     Create new system ...   
     ********************/
    /* Create new default terminal 
     *
     * 09.02.2006 ; New incoming handling ; hickert
     * Or create specified object of selected system type, from given incoming object  
     */
    if (isset($_POST['create_system'])||$s_action=="newsystem"||$s_action == "SelectedSystemType") {

      /* If the current entry is an incoming object 
       * $sw = System type as posted in new incoming handling dialog 
       */ 
      if($s_action == "SelectedSystemType") {
        $sw         = $s_entry;
        $dn_backup  = $this->dn;
      }else{
        if(isset($_POST['system'])){
          $sw = $_POST['system'];
        }else{
          $sw = $s_entry;
        }
      }

      $this->dn= "new";
      $this->acl= array(":all");

      switch ($sw){
        case 'terminal':
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['termgeneric']->cn= "default";
          $this->systab->by_object['termservice']->cn= "default";
          $this->systab->by_object['termgeneric']->base = $_SESSION['CurrentMainBase'];  
          break;

        case 'workstation':
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['workgeneric']->cn= "wdefault";
          $this->systab->by_object['workservice']->cn= "default";
          $this->systab->by_object['workgeneric']->base = $_SESSION['CurrentMainBase'];  
          break;

        case 'server':
          $this->systab= new servtabs($this->config,
              $this->config->data['TABS']['SERVTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['servgeneric']->base = $_SESSION['CurrentMainBase'];  
          break;

        case 'printer':
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['printgeneric']->base = $_SESSION['CurrentMainBase'];  
          break;

        case 'phone':
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['phonegeneric']->base = $_SESSION['CurrentMainBase'];  
          break;

        case 'component':
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
          $this->systab->set_acl ($this->acl);
          $this->systab->by_object['componentgeneric']->base = $_SESSION['CurrentMainBase'];  
          break;
      }
     
      /* 09.02.2006 ; New incoming handling ; hickert.
       * We must create a NEW object of given system type (Posted from SelectDeviceType).
       * But we have to use the same attributes as used in incoming object, thats
       * what we do here. 
       */ 
      if($s_action == "SelectedSystemType"){
    
        /* Store some informations, to be able to add this object to 
         * To specified objectgroup and delete incoming object
         */
        $_SESSION['SelectedSystemType']['dn']     = $this->dn;
        $_SESSION['SelectedSystemType']['server'] = $s_entry;
        $_SESSION['SelectedSystemType']['ogroup'] = $_POST['ObjectGroup'];
        
        /* restore dn */
        $this->dn = $dn_backup;

        /* Get properties from incoming object */
        $ldap = $this->config->get_ldap_link();
        $ldap->cd($this->dn);
        $ldap->cat($this->dn);
        $res = $ldap->fetch();
      
        /* Unset not needed attributes */
        unset($res['dn']);
        unset($res['objectClass']);
    
        /* Walk through all tabs and set attributes if available */
        foreach($this->systab->by_object as $name => $value){
          foreach($this->systab->by_object[$name]->attributes as $atr){
            if((isset($value))&&(isset($res[$atr]))){
              $this->systab->by_object[$name]->$atr = $res[$atr][0];
            }
          }
        }
      }
  
      /* set base ... of current divlist position */
      $this->systab->base = $_SESSION['CurrentMainBase'];  
    }


    /********************
     Edit system ...   
     ********************/

    /* User wants to edit data? */
//    if ($s_action == "edit"){
    if (($s_action == "edit") && (!isset($this->systab->config))){

      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Check locking, save current plugin in 'back_plugin', so
         the dialog knows where to return. */
      if (($user= get_lock($this->dn)) != ""){
        return(gen_locked_message ($user, $this->dn));
      }

      /* Set up the users ACL's for this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);

      /* Find out more about the object type */
      $ldap= $this->config->get_ldap_link();
      $ldap->cat($this->dn);
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs['objectClass']);

      /* Lock the current entry, so everyone will get the
         above dialog */

      switch ($type){
        case "NewDevice" :
          $this->systab = new SelectDeviceType($this->config,$this->dn) ;
        break;
        case "terminal":
          /* Register systab to trigger edit dialog */
          $this->systab= new termtabs($this->config,
              $this->config->data['TABS']['TERMTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "server":

          /* Register systab to trigger edit dialog */
          $this->systab= new servtabs($this->config,$this->config->data['TABS']['SERVTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "workstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new worktabs($this->config,
              $this->config->data['TABS']['WORKTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "printer":
          /* Register systab to trigger edit dialog */
          $this->systab= new printtabs($this->config,
              $this->config->data['TABS']['PRINTTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "phone":
          /* Register systab to trigger edit dialog */
          $this->systab= new phonetabs($this->config,
              $this->config->data['TABS']['PHONETABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "component":
          /* Register systab to trigger edit dialog */
          $this->systab= new componenttabs($this->config,
              $this->config->data['TABS']['COMPONENTTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;

        case "winstation":
          /* Register systab to trigger edit dialog */
          $this->systab= new wintabs($this->config,
              $this->config->data['TABS']['WINTABS'], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
        break;


        default:
        print_red (_("You can't edit this object type yet!"));
        del_lock($this->dn);
        break;
      }
    }
    

    /********************
     Change password ...   
     ********************/

    /* Set terminals root password */
    if ($s_action=="change_pw"){
      $this->dn= $this->terminals[$s_entry]['dn'];
      $_SESSION['objectinfo']= $this->dn;
      return ($smarty->fetch(get_template_path('password.tpl', TRUE)));
    }


    /********************
      Password cahnge finish, but check if entered data is ok 
     ********************/

    /* Correctly specified? */
    if (isset($_POST['password_finish'])){
      if ($_POST['new_password'] != $_POST['repeated_password']){
        print_red (_("Passwords entered as new and repeated do not match!"));
        return($smarty->fetch(get_template_path('password.tpl', TRUE)));
      }
    }

    /********************
      Password change finish
     ********************/

    /* Change terminal password */
    if (isset($_POST['password_finish']) && 
        $_POST['new_password'] == $_POST['repeated_password']){

      /* Check if user is allowed to set password */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($acl, "password") != ""){
        print_red (_("You are not allowed to set this systems password!"));
      } else {
        $ldap= $this->config->get_ldap_link();
        $ldap->cd($this->dn);

        $attrs= array();
        if ($_POST['new_password'] == ""){
          $attrs['gotoRootPasswd']= array();
        } else {
          $attrs['gotoRootPasswd']= crypt($_POST['new_password'],
              substr(session_id(),0,2));
        }
        $ldap->modify($attrs);
        gosa_log ("Password for '".$this->dn."' has been changed");
      }
      unset($_SESSION['objectinfo']);
    }


    /********************
      Delete system cancel
     ********************/

    /* Delete terminal canceled? */
    if (isset($_POST['delete_cancel']) || isset($_POST['password_cancel'])){
      del_lock ($this->dn);
      unset($_SESSION['objectinfo']);
    }


    /********************
      Delete system, confirm dialog
     ********************/

    /* Remove terminal was requested */
    if ($s_action=="del"){

      /* Get 'dn' from posted termlinst */
      $this->dn= $this->terminals[$s_entry]['dn'];

      /* Load permissions for selected 'dn' and check if
         we're allowed to remove this 'dn' */
      $acl= get_permissions ($this->dn, $this->ui->subtreeACL);
      $this->acl= get_module_permission($acl, "terminal", $this->dn);
      if (chkacl($this->acl, "delete") == ""){

        /* Check locking, save current plugin in 'back_plugin', so
           the dialog knows where to return. */
        if (($user= get_lock($this->dn)) != ""){
          return(gen_locked_message ($user, $this->dn));
        }

        /* Lock the current entry, so nobody will edit it during deletion */
        add_lock ($this->dn, $this->ui->dn);
        $smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), LDAP::fix($this->dn)));
        return($smarty->fetch(get_template_path('remove.tpl', TRUE)));
      } else {

        /* Obviously the user isn't allowed to delete. Show message and
           clean session. */
        print_red (_("You are not allowed to delete this component!"));
      }
    }

  
    /********************
      Delete system, confirmed
     ********************/
    /* Confirmation for deletion has been passed. Terminal should be deleted. */
    if (isset($_POST['delete_terminal_confirm'])){

      /* Some nice guy may send this as POST, so we've to check
         for the permissions again. */
      if (chkacl($this->acl, "delete") == ""){

        /* Find out more about the object type */
        $ldap= $this->config->get_ldap_link();
        $ldap->cat($this->dn);
        $attrs= $ldap->fetch();
        $type= $this->get_system_type($attrs['objectClass']);

        switch ($type){
          case "terminal":
            $tabtype  = "termtabs";
            $tabobj   = "TERMTABS";
          break;

          case "workstation":
            $tabtype  = "worktabs";
            $tabobj   = "WORKTABS";
          break;

          case "phone":
            $tabtype  = "phonetabs";
            $tabobj   = "PHONETABS";
          break;

          case "server":
            $tabtype  = "servtabs";
            $tabobj   = "SERVTABS";
          break;

          default:
            $tabtype  = "termtabs";
            $tabobj   = "TERMTABS";
          break;
        }

        /* Delete request is permitted, perform LDAP action */
        if($tabtype=="phonetabs"){
          $this->systab= new $tabtype($this->config,
             $this->config->data['TABS'][$tabobj], $this->dn);
          $this->systab->set_acl(array($this->acl));
          $this->systab->by_object['phonegeneric']->remove_from_parent ();
        }else{  
          $this->systab= new $tabtype($this->config,
             $this->config->data['TABS'][$tabobj], $this->dn);
          $this->systab->set_acl(array($this->acl));
          $this->systab->delete();
          #$this->systab->by_object['termgeneric']->remove_from_parent ();
        }
        unset ($this->systab);
        gosa_log ("System object'".$this->dn."' has been removed");
        $this->systab= NULL;

        /* Terminal list has changed, reload it. */
        $this->reload ();
      } else {

        /* Normally this shouldn't be reached, send some extra
           logs to notify the administrator */
        print_red (_("You are not allowed to delete this component!"));
        gosa_log ("Warning: '".$this->ui->uid."' tried to trick system ".
            "deletion.");
      }

      /* Remove lock file after successfull deletion */
      del_lock ($this->dn);
    }


    /********************
      Edit system type finished, check if everything went ok 
     ********************/
    /* Finish user edit is triggered by the tabulator dialog, so
       the user wants to save edited data. Check and save at this
       point. */
    if ((isset($_POST['edit_finish'])) && (isset($this->systab->config))){

      /* Check tabs, will feed message array */
      $message= $this->systab->check();

      /* Save, or display error message? */
      if (count($message) == 0){

        /* Save terminal data to ldap */
        gosa_log ("System object'".$this->dn."' has been saved");
        $this->systab->save();

        /* Terminal has been saved successfully, remove lock from
           LDAP. */

        /* 09.02.2006 Hickert 
         * New System incoming behavior; you can select a system type and an ogroup membership. 
         * If this object is an Incoming object, $_SESSION['SelectedSystemType'] isset.
         * Check if we must add the new object to an object group.
         * !! Don't forget to unset the $_SESSION['SelectedSystemType']... else all edited objects 
         * !! will be added to  the object group.
         * 
         * If this is done, delete the old incoming entry... it is still there, because this is a new 
         * entry and not an edited one.
         */
        if(isset($_SESSION['SelectedSystemType'])){
          $SelectedSystemType= $_SESSION['SelectedSystemType'];
          unset($_SESSION['SelectedSystemType']);
          if($SelectedSystemType['ogroup'] != "none"){
            $og = new ogroup($this->config,$SelectedSystemType['ogroup']);
            if($og){
              $og->AddDelMembership($this->systab->dn);
            }
            $og->save();
          }
          if(!isset($ldap)){
            $ldap = $this->config->get_ldap_link();
          }
          $ldap->cd ($this->dn);
          $ldap->cat($this->dn);  
          if(count($ldap->fetch())){
            $ldap->cd($this->dn);
            $ldap->rmDir($this->dn);
          }
          $ldap->cd($this->config->current['BASE']);
        }

        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

        /* There's no page reload so we have to read new terminals at
           this point. */
        $this->reload ();
        unset ($this->systab);
        $this->systab= NULL;
        unset($_SESSION['objectinfo']);

      } else {
        /* Ok. There seem to be errors regarding to the tab data,
           show message and continue as usual. */
        show_errors($message);
      }

    }


    /********************
      Edit system was canceled 
     ********************/
    /* Cancel dialogs */
    if (isset($_POST['edit_cancel']) || isset($_POST['password_cancel']) || isset($_POST['SystemTypeAborted'])){
      if (isset($this->systab)){
        del_lock ($this->systab->dn);
        unset ($this->systab);
      }
      $this->systab= NULL;
      unset($_SESSION['objectinfo']);

      /* 09.02.2006 ; New incoming handling ; hickert 
       * remove session object which stores our ogroup selection 
       * for the new incoming handling
       */
      if(isset($_SESSION['SelectedSystemType'])){
        unset($_SESSION['SelectedSystemType']);
      }
    }


    /********************
      Display edit dialog, or some other  
     ********************/

    /* Show tab dialog if object is present */
    if (isset($this->systab->config)){
      $display= $this->systab->execute();

      
      /* Don't show buttons if tab dialog requests this */
      if ((isset($this->systab->by_object))&&(!$this->systab->by_object[$this->systab->current]->dialog)){
        $display.= "<p style=\"text-align:right\">\n";
        $display.= "<input type=\"submit\" name=\"edit_finish\" value=\""._("Save")."\">\n";
        $display.= "&nbsp;\n";
        $display.= "<input type=\"submit\" name=\"edit_cancel\" value=\""._("Cancel")."\">\n";
        $display.= "</p>";
      }
      return ($display);
    }


    /********************
      Entry handling finished (edit delete ... )
      Now the list generation is the next part of this script.
     ********************/
    
    /* Prepare departments, 
       which are shown in the listbox on top of the listbox 
     */
    $options= "";
    foreach ($this->config->idepartments as $key => $value){
      if ($_SESSION['CurrentMainBase'] == $key){
        $options.= "<option selected='selected' value='$key'>$value</option>";
      } else {
        $options.= "<option value='$key'>$value</option>";
      }
    }


    /* NEW LIST MANAGMENT */

    /* Create list header
     */
    $listhead = "<div style='background:#F0F0F9;padding:5px;'>".
    " <input class='center' type='image' src='images/list_root.png' align='middle' 
        title='"._("Go to root department")."' name='dep_root' alt='"._("Root")."'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/list_back.png' 
        title='"._("Go up one department")."' alt='"._("Up")."' name='dep_back'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/list_home.png' 
        title='"._("Go to users department")."' alt='"._("Home")."'                     name='dep_home'>&nbsp;".
    " <input class='center' type='image' src='images/list_reload.png' align='middle' title='"._("Reload list")."' name='submit_department' alt='"._("Submit")."'>&nbsp;".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;".
    " <input class='center' type='image' align='middle' src='images/select_new_terminal.png'   
        name='newsystem_terminal'    alt='"._("New Terminal template")."' title='"._("New Terminal")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_workstation.png' 
        name='newsystem_workstation' alt='"._("New Workstation template")."' title='"._("New Workstation")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_server.png'     name='newsystem_server'      alt='"._("New Server")."' 
        title='"._("New Server")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_printer.png'    name='newsystem_printer'     alt='"._("New Printer")."' 
        title='"._("New Printer")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_phone.png'      name='newsystem_phone'       alt='"._("New Phone")."' 
        title='"._("New Phone")."'>".
    " <input class='center' type='image' align='middle' src='images/select_new_component.png'  name='newsystem_component'   alt='"._("New Component")."' 
        title='"._("New Component")."'>".
    " <img class='center' src='images/list_seperator.png' align='middle' alt='' height='16' width='1'>&nbsp;"._("Base")."&nbsp;".
    " <select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
    " <input class='center' type='image' src='images/list_submit.png' align='middle' 
        title='"._("Submit department")."' name='submit_department' alt='".           _("Submit")."'>&nbsp;".
    "</div>";


    /* Edit delete link for system types 
     */
    $action= "<input class='center' type='image' src='images/edit.png' alt='"._("edit")."'     name='user_edit_%KEY%' title='"._("Edit system")."'>";
    $action.= "<input class='center' type='image' src='images/edittrash.png' alt='"._("delete")."'   name='user_del_%KEY%' title='"._("Delete system")."'>";
    $linkopen = "<a href='?plug=".$_GET['plug']."&amp;act=dep_open&amp;dep_id=%s'>%s</a>";

    /* Create new divlist, and add the header elements 
     */
    $divlist = new divlist("systemstab");
    $divlist->SetSummary(_("This table displays all systems, in the selected tree."));
    $divlist->SetEntriesPerPage(0);
    $divlist->SetHeader(array(
          array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"),
          array("string" => _("System")." / "._("Department"), "attach" => "style=''"),
          array("string" => _("Actions"), "attach" => "style='width:60px;border-right:0px;text-align:right;'" )));


    /* Add departments, to be able to switch into them
     */
    foreach($this->departments as $key=> $val){
      
      /* Add missing entries ... */
      if(!isset($this->config->departments[trim($key)])){
        $this->config->departments[trim($key)]="";
      }

      /* check if this department contains sub-departments 
         Display different image in this case 
       */ 
      $non_empty="";
      $nkey= normalizePreg($key);
      foreach($this->config->departments as $keyd=>$vald ){
        if(preg_match("/$nkey\/.*/",$keyd)){
          $non_empty="full";
        }
      }

      /* Add to divlist */
      $field1 = array("string" => "<img src='images/".$non_empty."folder.png' alt='department'>", "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($linkopen,base64_encode($key),$val), "attach" => "style=''");
      $field3 = array("string" => "&nbsp;", "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry(array($field1,$field2,$field3));
    }

    // Iamge spacer 
    $empty    ="&nbsp;";

    // User and Template  Images
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    // Pictures for Extensions
    $img1  = "<img class='center' src='images/printer.png'            alt='C' title='"._("Cups Server")  ."'>";
    $img2  = "<img class='center' src='images/scanner.png'            alt='L' title='"._("Log Db") ."'>";
    $img3  = "<img class='center' src='images/select_terminal.png'    alt='L' title='"._("Syslog Server") ."'>";
    $img4  = "<img class='center' src='images/mailto.png'             alt='M' title='"._("Mail Server")  ."'>";
    $img5  = "<img class='center' src='images/select_phone.png'       alt='I' title='"._("Imap Server") ."'>";
    $img6  = "<img class='center' src='images/fax_small.png'          alt='F' title='"._("Nfs Server")   ."'>";
    $img7  = "<img class='center' src='images/select_winstation.png'  alt='K' title='"._("Kerberos Server") ."'>";
    $img8  = "<img class='center' src='images/select_phone.png'       alt='A' title='"._("Asterisk Server") ."'>";
    $img9  = "<img class='center' src='images/fax_small.png'          alt='F' title='"._("Fax Server") ."'>";
    $img10 = "<img class='center' src='images/save.png'               alt='L' title='"._("Ldap Server") ."'>";

    // Test Every Entry and generate divlist Array
    foreach($this->terminals as $key => $val){
      // Specify Pics for Extensions
      if(in_array("goLdapServer"    ,$val['objectClass'])){
        
      }

      /* Generate picture list, which is currently disabled */     
      if(in_array("goCupsServer"    ,$val['objectClass'])) $cups    = $img1;   else $cups  =$empty;
      if(in_array("goLogDBServer"   ,$val['objectClass'])) $logdb   = $img2;   else $logdb =$empty;
      if(in_array("goSyslogServer"  ,$val['objectClass'])) $syslog  = $img3;   else $syslog=$empty;
      if(in_array("goImapServer"    ,$val['objectClass'])) $imap    = $img4;   else $imap  =$empty;
      if(in_array("sambaSamAccount" ,$val['objectClass'])) $samba   = $img5;   else $samba =$empty;
      if(in_array("goShareServer"   ,$val['objectClass'])) $nfs     = $img6;   else $nfs   =$empty;
      if(in_array("goKrbServer"     ,$val['objectClass'])) $krb     = $img7;   else $krb   =$empty;
      if(in_array("goFonServer"     ,$val['objectClass'])) $fon     = $img8;   else $fon   =$empty;
      if(in_array("goFaxServer"     ,$val['objectClass'])) $fax     = $img9;   else $fax   =$empty;
      if(in_array("goLdapServer"    ,$val['objectClass'])) $ldap     = $img10;   else $ldap   =$empty;
      $pics = $cups.$logdb.$syslog.$imap.$samba.$nfs.$krb.$fon.$fax.$ldap;
      $pics = "";
    
      $val['cn'][0]= preg_replace('/\$$/', '', $val['cn'][0]);

      // Generate Array to Add
      if((isset($val['is_new']))&&(!empty($val['is_new']))){
        $display= "".$val["cn"][0]." ".$val['is_new'];
      }else{
        $display= "".$val["cn"][0]."";
      }

      if((in_array("gotoTerminal",$val['objectClass']))||(in_array("gotoWorkstation",$val['objectClass']))){
        $action2 = "<input class='center' type='image' src='images/certs.png' alt='"._("Password")."'   name='user_setpwd_%KEY%' title='"._("Set root password")."'>";
      }else{
        $action2 = "";
      }

      if(isset($val['message'])){
        $display.= "  (".$val['message']." '".$this->config->idepartments[$_SESSION['CurrentMainBase']]."'  )";
      }

      $img=$this->convert_list($val);

      $field1 = array("string" => sprintf($img['img'],$val['dn']), "attach" => "style='text-align:center;width:20px;'");
      $field2 = array("string" => sprintf($editlink,$key,$display), "attach" => "style='' title='".@LDAP::fix($val['dn'])."'");
      $field3 = array("string" => preg_replace("/%KEY%/", "$key", $action2.$action), "attach" => "style='width:60px;border-right:0px;text-align:right;'");
      $divlist->AddEntry( array($field1,$field2,$field3));
    }

    /* Show main page */
    $smarty->assign("terminalshead", $listhead);
    $smarty->assign("terminals", $divlist->DrawList());
    $smarty->assign("search_image", get_template_path('images/search.png'));
    $smarty->assign("searchu_image", get_template_path('images/search_user.png'));
    $smarty->assign("tree_image", get_template_path('images/tree.png'));
    $smarty->assign("infoimage", get_template_path('images/info.png'));
    $smarty->assign("launchimage", get_template_path('images/launch.png'));
    foreach( array( "user", "regex", "workstations", "thins", "servers",
          "winstations", "printers", "phones", "netdev") as $type){

      $smarty->assign("$type", $terminalfilter[$type]);
    }
    $smarty->assign("deplist", $this->config->idepartments);
    $smarty->assign("CurrentMainBase", $_SESSION['CurrentMainBase']);

    /* Extend if we are not using javascript */
    $smarty->assign("apply", apply_filter());
    $smarty->assign("alphabet", generate_alphabet());
    $smarty->assign("hint", print_sizelimit_warning());

    return($smarty->fetch(get_template_path('headpage.tpl', TRUE)));
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(	
        "NQ" => array("select_newsystem.png",_("New System from incoming")),
        "D" => array("select_default.png",_("Template")),
        "T" => array("select_terminal.png",_("Terminal")),
        "L" => array("select_workstation.png",_("Workstation")),
        "F" => array("select_phone.png",_("Phone")),
        "S" => array("select_server.png",_("Server")),
        "W" => array("select_winstation.png",_("Winstation")),
        "C" => array("select_component.png",_("Network Device")),
        "NT"=> array("select_new_terminal.png",_("New Terminal")),
        "NL"=> array("select_new_workstation.png",_("New Workstation")),
        "P" => array("select_printer.png",_("Printer")));

   if((isset($input['is_new']))&&(!empty($input['is_new']))){
     $input['type']="N".$input['type'];
   }
   foreach ($conv  as $key => $value){
      if($input['type']==$key){
        $tmp['img'] ="<img class='center' src='images/".$value[0]."' alt='".$key."' title='".$value['1']."'>";
        $tmp['class']=$key;
        return $tmp;
      }
    }
  }

  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
  }


  /* Check values */
  function check()
  {
  }


  /* Save to LDAP */
  function save()
  {
  }

  function adapt_from_template($dn)
  {
  }

  function password_change_needed()
  {
  }

  function show_header($button_text, $text, $disabled= FALSE)
  {
  }

  function reload()
  {
    /* Load terminal shortcuts */
    $responsible= array();
    foreach ($this->config->departments as $key => $value){
      if (get_module_permission(get_permissions ($value, $this->ui->subtreeACL),
            "terminal", $value) == "#all#"){
        $responsible[$key]= $value;
      }
    }

    /* Get config */
    $terminalfilter= get_global('terminalfilter');
    $filter= "";

    /* Set base for all searches */
    $base= $_SESSION['CurrentMainBase'];

    /* Regex filter? */
    if ($terminalfilter['regex'] != ""){
      $regex= $terminalfilter['regex'];
    } else {
      $regex= "*";
    }

    /* Get list of terminals to be shown */
    if ($terminalfilter['thins'] == "checked"){
      $termfilter= "(&(objectClass=goHard)(cn=$regex))";
    } else {
      $termfilter= "";
    }
    if ($terminalfilter['workstations'] == "checked"){
      $workfilter= "(&(objectClass=gotoWorkstation)(cn=$regex))";
    } else {
      $workfilter= "";
    }
    if ($terminalfilter['winstations'] == "checked"){
      if ($this->config->current['SAMBAVERSION'] == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
      $winfilter= "(&(objectClass=$samba)(cn=$regex\$))";
    } else {
      $winfilter= "";
    }
    if ($terminalfilter['printers'] == "checked"){
      $printfilter= "(&(objectClass=gotoPrinter)(cn=$regex))";
    } else {
      $printfilter= "";
    }
    if ($terminalfilter['phones'] == "checked"){
      $phonefilter= "(&(objectClass=goFonHardware)(cn=$regex))";
    } else {
      $phonefilter= "";
    }
    if ($terminalfilter['netdev'] == "checked"){
      $netfilter= "(&(objectClass=ieee802Device)(cn=$regex))";
    } else {
      $netfilter= "";
    }
    if ($terminalfilter['servers'] == "checked"){
      $serverfilter= "(&(objectClass=goServer)(cn=$regex))";
    } else {
      $serverfilter= "";
    }

    /* User filter? */
    if ($terminalfilter['user'] != "" && $terminalfilter['user'] != "*"){
      $filter.= "(gotoLastUser=".$terminalfilter['user'].")";
    }

    /* Get all gotoTerminal's */
    $this->terminals= array();

    //FIXME: All get_list calls are called without GL_SIZELIMIT? All contain sambaDomainName???
    $res= get_list("(|$termfilter)", $this->ui->subtreeACL, "ou=terminals,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE);

    $res= array_merge($res,
          get_list("(|$termfilter)", $this->ui->subtreeACL, "ou=incoming,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$serverfilter)", $this->ui->subtreeACL, "ou=servers,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$phonefilter)", $this->ui->subtreeACL, "ou=phones,ou=systems,".$base,
                    array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$netfilter)", $this->ui->subtreeACL, "ou=netdevices,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$printfilter)", $this->ui->subtreeACL, "ou=printers,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$workfilter)", $this->ui->subtreeACL, "ou=workstations,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

    $res= array_merge($res,
          get_list("(|$winfilter)", $this->ui->subtreeACL, "ou=winstations,ou=systems,".$base,
                   array("cn", "description", "macAddress", "objectClass", "sambaDomainName"), GL_NONE));

/* NEW LIST MANAGMENT
     * We also need to search for the departments
     * So we are able to navigate like in konquerer
     */

    $peopleOU = get_people_ou();

    if(empty($peopleOU)){
      $base2 = $base;
    }else{
      $base2 = preg_replace("/".$peopleOU."/i","",$base);
    }

    /* Get all departments within this subtree */
    $deps= get_list("(&(|(ou=$regex)(description=$regex))(objectClass=gosaDepartment))", $this->ui->subtreeACL,
        $base2, array("ou", "description"), GL_SIZELIMIT | GL_CONVERT);

    $this->departments = array();
    foreach($deps as $value){
      if(isset($value['description'][0])){
        $this->departments[$value['dn']]= get_sub_department($value['dn'])." - [".$value["description"][0]."]";
      }else{
        $this->departments[$value['dn']]= get_sub_department($value['dn']);
      }
    }
    natcasesort($this->departments);

    /* END NEW LIST MANAGMENT
     */


    foreach ($res as $value){

      /* Look for new terminals and mark them with '+' */
      $tmp= "";
      if (preg_match('/,ou=terminals,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=terminals,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=workstations,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=workstations,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=servers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=servers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,'.get_winstations_ou().'/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*".get_winstations_ou()."/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=printers,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=printers,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=phones,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=phones,ou=systems,/i", "", $value["dn"]);
      }
      if (preg_match('/,ou=netdevices,ou=systems,/i', $value["dn"])){
        $tmp= preg_replace("/^[^,]+,[^o]*ou=netdevices,ou=systems,/i", "", $value["dn"]);
      }
      if ($tmp == ""){
        $tmp= $value["dn"];
      }
      $add= "";
      if (preg_match ("/,ou=incoming,/i", $tmp)){
         if (in_array('gotoTerminal', $value['objectClass'])){
          $add= "- "._("New terminal");
        }elseif (in_array('gotoWorkstation', $value['objectClass'])){
          $add= "- "._("New workstation");
        }elseif (in_array('GOhard', $value['objectClass'])){
          $add= "- "._("New Device");
        }
      } 

      $terminal = array();
  
      if (in_array ($tmp, $responsible) || $add != ""){
        if (in_array('gotoTerminal', $value["objectClass"])){
          if (isset($value["macAddress"][0]) && $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "T";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['message']  = _("Terminal template for");
            $terminal['location'] = array_search($tmp, $this->config->departments); 
          }
        } elseif (in_array('gotoWorkstation', $value["objectClass"])){
          if (isset($value["macAddress"][0]) &&  $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "L";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['location'] = array_search($tmp, $this->config->departments);
            $terminal['message']  = _("Workstation template for");
          }
        } elseif (in_array('gotoPrinter', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "P";
        } elseif (in_array('goServer', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "S";
        } elseif (in_array('goFonHardware', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "F";
        }elseif (in_array("GOhard",$value['objectClass'])){
          $terminal = $value;
          $terminal['type']   = "Q";
          $terminal['is_new'] = $add;
        } elseif (in_array('ieee802Device', $value["objectClass"])){
            $terminal             = $value;
            $terminal['type']     = "C";
        } else{
          $name= preg_replace('/\$$/', '', $value['cn'][0]);
          if (isset($value['sambaDomainName'])){
            $domain= " [".$value['sambaDomainName'][0]."]";
          } else {
            $domain= "";
          }
          $terminal=$value;
          $terminal['type']     ="W";
          $terminal['domain']   = $name.$domain;
        }
        $this->terminals[]=$terminal;
      }
    }

    $tmp=array();
    foreach($this->terminals as $tkey => $val ){
      $tmp[strtolower($val['cn'][0]).$val['dn']]=$val;
    }
    ksort($tmp);
    $this->terminals=array();
    foreach($tmp as $val){
      $this->terminals[]=$val;
    }
    reset ($this->terminals);
  }

  function remove_lock()
  {
    if (isset($this->systab->dn)){
      del_lock ($this->systab->dn);
    }
  }


  function get_system_type($classes)
  {
    $type= "";

    if (in_array('ieee802Device', $classes)){
      $type= "component";
    }else
    if (in_array('gotoTerminal', $classes)){
      $type= "terminal";
    }else
    if (in_array('gotoWorkstation', $classes)){
      $type= "workstation";
    }else
    if (in_array('gotoPrinter', $classes)){
      $type= "printer";
    }else
    if (in_array('goFonHardware', $classes)){
      $type= "phone";
    }else
    if (in_array('goServer', $classes)){
      $type= "server";
    }else
    if (in_array('GOhard', $classes)){
      $type= "NewDevice";
    }else
    if (in_array('sambaAccount', $classes) ||
        in_array('sambaSamAccount', $classes)){
      $type= "winstation";
    }

    return ($type);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
