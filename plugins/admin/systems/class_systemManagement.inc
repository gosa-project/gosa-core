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
require "tabs_arpnewdevice.inc";


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
  var $DivListSystem;

  function systems ($config, $ui)
  {
    /* Save configuration for internal use */
    $this->config= $config;
    $this->ui= $ui;

    /* Creat dialog object */
    $this->DivListSystem = new divListSystem($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    $_SESSION['LOCK_VARS_TO_USE'] = array("/^user_edit_/i","/^user_del_/","/^act/","/^id/");

    /********************
      Check for functional posts, edit|delete|add|... system devices 
     ********************/
    $s_action     = "";                       // Contains the action to proceed
    $s_entry      = "";                       // The value for s_action
    $base_back    = "";                       // The Link for Backbutton
    $smarty       = get_smarty();

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
      }elseif(preg_match("/user_new.*/",$key)){
        $s_action="new";
      }elseif(preg_match("/user_tplnew.*/i",$key)){
        $s_action="new_tpl";
      }elseif(preg_match("/user_setpwd_.*/i",$key)){
        $s_action="change_pw";
        $s_entry  = preg_replace("/user_setpwd_/i","",$key);
      }elseif(preg_match("/gen_cd_.*/i",$key)){
        $s_action="gen_cd";
        $s_entry  = preg_replace("/gen_cd_/i","",$key);
      }elseif(preg_match("/newsystem_.*/i",$key)){
        $s_action="newsystem";
        $s_entry  = preg_replace("/newsystem_/i","",$key);
      }
    }

    /* Incoming handling  
     * If someone made a systemtype and ogroup selection 
     * Display the new requested entry type ... servtab  in case of server and so on.
     */
    if(isset($_POST['SystemTypeChoosen'])){
      $s_action = "SelectedSystemType";
      $s_entry  = $_POST['SystemType'];
      $_SESSION['SelectedSystemType']['ogroup'] = $_POST['ObjectGroup'];
      $this->systab = NULL;
    }

    /* remove image tags from posted entry  (posts looks like this 'name_x')*/
    $s_entry  = preg_replace("/_.$/","",$s_entry);

    /* Edit was requested by pressing the name(link) of an item */
    if((isset($_GET['act']))&&($_GET['act']=="edit_entry")){
      $s_action ="edit";
      $s_entry  = $_GET['id'];
    }

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
     * Or create specified object of selected system type, from given incoming object  
     */

    $save_object_directly = false;
    if(($s_action == "SelectedSystemType") && (isset($_POST['ObjectGroup']) && ($_POST['ObjectGroup'] != "none"))){
      $save_object_directly = true;
    }
    
    if (isset($_POST['create_system'])||$s_action=="newsystem"||$s_action == "SelectedSystemType") {

      /* If the current entry is an incoming object 
       * $sw = System type as posted in new incoming handling dialog 
       */ 
      if($s_action == "SelectedSystemType") {
        $sw         = $s_entry;
      }else{
        if(isset($_POST['system'])){
          $sw = $_POST['system'];
        }else{
          $sw = $s_entry;
        }
        $this->dn= "new";
      }
      $this->acl= array(":all");
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABNAME"=>"termgeneric",     "TABCLASS" =>"termtabs"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABNAME"=>"workgeneric",     "TABCLASS" =>"worktabs"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABNAME"=>"servgeneric",     "TABCLASS" =>"servtabs"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABNAME"=>"printgeneric",    "TABCLASS" =>"printtabs"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABNAME"=>"phonegeneric",    "TABCLASS" =>"phonetabs"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABNAME"=>"componentgeneric","TABCLASS" =>"componenttabs"));

      if(isset($tabs[$sw])){
        $class    = $tabs[$sw]["CLASS"];
        $tabname  = $tabs[$sw]["TABNAME"];
        $tabclass = $tabs[$sw]["TABCLASS"];
        $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn);
        $this->systab->set_acl ($this->acl);
        $this->systab->by_object[$tabname]->base = $this->DivListSystem->selectedBase;
        $this->systab->base = $this->DivListSystem->selectedBase;
      }
    }

    /********************
      Edit system ...   
     ********************/

    /* User wants to edit data? */
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
      $ldap->cat($this->dn, array('objectClass','gotoMode'));
      $attrs= $ldap->fetch();
      $type= $this->get_system_type($attrs);

      /* Lock the current entry, so everyone will get the
         above dialog */
      $tabs = array(
          "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs"),
          "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs"),
          "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs"),
          "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs"),
          "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs"),
          "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs"),
          "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs"));


      if($type == "ArpNewDevice"){
        $this->systab = new ArpNewDeviceTabs($this->config,$this->config->data['TABS']['ARPNEWDEVICETABS'],$this->dn);
      }elseif($type == "NewDevice"){
        $this->systab = new SelectDeviceType($this->config,$this->dn) ;
      }elseif(isset($tabs[$type])){

        $class    = $tabs[$type]["CLASS"];
        $tabclass = $tabs[$type]["TABCLASS"];

        $this->systab= new $tabclass($this->config, $this->config->data['TABS'][$class], $this->dn);
        $this->systab->set_acl($acl);
        $_SESSION['objectinfo']= $this->dn;
        add_lock ($this->dn, $this->ui->dn);
      }else{ 
        print_red (_("You can't edit this object type yet!"));
        del_lock($this->dn);
      }
    }

    /********************
      Create FAI CD ...   
     ********************/
    if ($s_action=="gen_cd"){
      $this->dn= $this->terminals[$s_entry]['dn'];
      $_SESSION['objectinfo']= $this->dn;
      return ($smarty->fetch(get_template_path('gencd.tpl', TRUE)));
    }

    /* Start CD-Creation */
    if (isset($_POST["cd_create"])){
      $smarty->assign("src", "?plug=".$_GET['plug']."&amp;PerformIsoCreation");
      return ($smarty->fetch(get_template_path('gencd_frame.tpl', TRUE)));
    }
  
    if ($this->dn != "" && isset($_GET['PerformIsoCreation'])){

      $return_button   = "<form method='get' action='main.php' target='_parent'>
                            <input type='submit' value='"._("Back")."'>
                            <input type='hidden' name='plug' value='".$_GET['plug']."'/>
                          </form>";

      $dsc             = array(0 => array("pipe", "r"), 1 => array("pipe", "w"), 2 => array("pipe", "w"));

      /* Get and check command */
      $command= search_config($this->config->data['TABS'], "workgeneric", "ISOCMD");
      if (check_command($command)){
        @DEBUG (DEBUG_SHELL, __LINE__, __FUNCTION__, __FILE__, $command, "Execute");

        /* Print out html introduction */
        echo '  <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2//EN">
                <html>
                  <head>
                    <title></title>
                    <style type="text/css">@import url("themes/default/style.css");</style>
                    <script language="javascript" src="include/focus.js" type="text/javascript"></script>
                  </head>
                  <body style="background: none; margin:4px;" id="body" >
                  <pre>';

        /* Open process handle and check if it is a valid process */
        $process= proc_open($command." '".$this->dn."'", $dsc, $pipes);
        if (is_resource($process)) {
          fclose($pipes[0]);

          /* Print out returned lines && write JS to scroll down each line */
          while (!feof($pipes[1])){
            $cur_dat = fgets($pipes[1], 1024);
            echo $cur_dat;
            echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
            flush();
          }
        }
    
        /* Get error string && close streams */
        $buffer = "";
        while (!feof($pipes[2])){
          $buffer .= fgets($pipes[2],256);
        }
      
        fclose($pipes[1]);
        fclose($pipes[2]);
        echo "</pre>";
      
        /* Check return code */ 
        $ret= proc_close($process);
        if ($ret != 0){
          echo "<h1 style='color:red'>"._("Creating the image failed. Please see the report below.")."</h1>";
          echo "<pre style='color:red'>$buffer</pre>";
        }

        echo $return_button."<br>";

      } else {
        $tmp= "<h1 style='color:red'>".sprintf(_("Command '%s', specified for ISO creation doesn't seem to exist."), $command)."</h1>";
        echo $tmp;
      }

      /* Scroll down completly */
      echo '<script language="javascript" type="text/javascript">scrollDown2();</script>' ;
      echo '</body></html>';
      flush();
      exit;
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
        $smarty->assign("warning", sprintf(_("You're about to delete all information about the component at '%s'."), @LDAP::fix($this->dn)));
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
        $ldap->cat($this->dn, array('objectClass','gotoMode'));
        $attrs= $ldap->fetch();
        $type= $this->get_system_type($attrs);

        $tabs = array(
            "terminal"    => array("CLASS"=>"TERMTABS",     "TABCLASS" =>"termtabs"),
            "workstation" => array("CLASS"=>"WORKTABS",     "TABCLASS" =>"worktabs"),
            "server"      => array("CLASS"=>"SERVTABS",     "TABCLASS" =>"servtabs"),
            "printer"     => array("CLASS"=>"PRINTTABS",    "TABCLASS" =>"printtabs"),
            "phone"       => array("CLASS"=>"PHONETABS",    "TABCLASS" =>"phonetabs"),
            "winstation"  => array("CLASS"=>"WINTABS",      "TABCLASS" =>"wintabs"),
            "component"   => array("CLASS"=>"COMPONENTTABS","TABCLASS" =>"componenttabs"));

        /* get object type */
        $tabtype  = "termtabs";
        $tabobj   = "TERMTABS";
        if(isset($tabs[$type])){
          $tabtype = $tabs[$type]['TABCLASS'];
          $tabobj  = $tabs[$type]['CLASS'];
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
    if ((isset($_POST['edit_finish'])) && (isset($this->systab->config)) || $save_object_directly){

      /* Check tabs, will feed message array */
      $message = array();
      if(!$save_object_directly){
        $message = $this->systab->check();
      }else{
        $found = false;
  
        /* Set gotoMode to active if we there was an ogroup selected . (save_object_directly) */
        foreach(array("workgeneric"=>"active","servgeneric"=>"active","termgeneric"=>"graphic") as $tab => $value){
          if(isset($this->systab->by_object[$tab]->gotoMode)) {
            $found = true;
            $this->systab->by_object[$tab]->gotoMode = $value;
          }
        }   
        if(!$found){
          print_red(sprintf(_("Can't set gotoMode to status 'active', the current object couldn't be identified.")));
        }

      }
      /* Save, or display error message? */
      if (count($message) == 0){

        /* Incoming behavior; you can select a system type and an ogroup membership. 
         * If this object is an Incoming object, $_SESSION['SelectedSystemType'] isset.
         * Check if we must add the new object to an object group.
         * 
         * If this is done, delete the old incoming entry... it is still there, because this is a new 
         * entry and not an edited one, so we will delete it.
         */

        if(isset($_SESSION['SelectedSystemType'])){
          $SelectedSystemType= $_SESSION['SelectedSystemType'];
          unset($_SESSION['SelectedSystemType']);
          if($SelectedSystemType['ogroup'] != "none"){
            $og = new ogroup($this->config,$SelectedSystemType['ogroup']);
            if($og){
              $og->AddDelMembership($this->systab->dn);
              $og->save();
            }
          }
        }

        /* Save terminal data to ldap */
        if(isset($_SESSION['SelectedSystemType']['ogroup']) && $_SESSION['SelectedSystemType']['ogroup'] != 'none'){
          foreach (array("workservice", "termservice") as $cls){
            if (isset($this->systab->by_object[$cls])){
              $this->systab->by_object[$cls]->gotoXMouseport= "";
              $this->systab->by_object[$cls]->gotoXMouseType= "";
              $this->systab->by_object[$cls]->gotoXResolution= "";
              $this->systab->by_object[$cls]->gotoXColordepth= "";
            }
          }
        }

        $this->systab->save();
        gosa_log ("System object'".$this->dn."' has been saved");

        if(isset($_SESSION['SelectedSystemType'])){
          if(!isset($ldap)){
            $ldap = $this->config->get_ldap_link();
          }
          $ldap->cd ($this->dn);
          $ldap->cat($this->dn, array('dn'));  
          if(count($ldap->fetch())){
            $ldap->cd($this->dn);
            $ldap->rmDir($this->dn);
          }
          $ldap->cd($this->config->current['BASE']);
        }


        /* Terminal has been saved successfully, remove lock from
           LDAP. */
        if ($this->dn != "new"){
          del_lock ($this->dn);
        }

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

      /* Remove ogroup selection, which was set while editing a new incoming entry */
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

    /* Display dialog with system list */
    $this->DivListSystem->parent = $this;
    $this->DivListSystem->execute();

    /* Add departments if subsearch is disabled */
    if(!$this->DivListSystem->SubSearch){
      $this->DivListSystem->AddDepartments($this->DivListSystem->selectedBase);
    }
    $this->reload();
    $this->DivListSystem->setEntries($this->terminals);
    return($this->DivListSystem->Draw());
  }



  function remove_from_parent()
  {
    /* Optionally execute a command after we're done */
    $this->postremove();
  }


  /* Save data to object */
  function save_object()
  {
    $this->DivListSystem->save_object();
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

  function reload()
  {
    /* Load terminal shortcuts */
    $responsible= array();
    foreach ($this->config->departments as $key => $value){
      if (get_module_permission(get_permissions ($value, $this->ui->subtreeACL),
            "systems", $value) != ""){
        $responsible[$key]= $value;
      }
    }

    /* some var init */
    $res              = array();
    $this->terminals  = array();
    $userregex        = "";

    /* Set base for all searches */
    $base=  $this->DivListSystem->selectedBase;

    /* Prepare samba class name */
    $samba  ="";
    if ($this->DivListSystem->ShowWinWorkstations){
      if ($this->config->current['SAMBAVERSION'] == "3"){
        $samba= "sambaSamAccount";
      } else {
        $samba= "sambaAccount";
      }
    }

    /* This array represents the combination between checkboxes and search filters */
    $objs = array( "ShowServers"        => array("CLASS" => "goServer"        ,"TREE" => "ou=servers,ou=systems," ),
        "ShowTerminals"      => array("CLASS" => "gotoTerminal"    ,"TREE" => "ou=terminals,ou=systems,"), 
        "ShowPrinters"       => array("CLASS" => "gotoPrinter"     ,"TREE" => "ou=printers,ou=systems," ),
        "ShowDevices"        => array("CLASS" => "ieee802Device"   ,"TREE" => "ou=netdevices,ou=systems," ),
        "ShowPhones"         => array("CLASS" => "goFonHardware"   ,"TREE" => "ou=phones,ou=systems," ),
        "ShowWorkstations"   => array("CLASS" => "gotoWorkstation" ,"TREE" => "ou=workstations,ou=systems," ),
        "ShowWinWorkstations"=> array("CLASS" => $samba            ,"TREE" => get_winstations_ou() ));

    /* Include the 'Display Systems of user' attribute */ 
    if ((!empty($this->DivListSystem->UserRegex)) && ($this->DivListSystem->UserRegex!= "*")){
      $userregex = "(gotoLastUser=".$this->DivListSystem->UserRegex.")";
    }

    /* Walk through all possible search combinations, and search for some objects if the checkbox is enabled  */
    foreach($objs as $checkBox => $oc){
      if($this->DivListSystem->$checkBox){
        if($this->DivListSystem->SubSearch){
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, $base,
                  array("cn", "description", "macAddress", "objectClass", "sambaDomainName", "FAIstate","gotoMode"), GL_NONE | GL_SUBSEARCH | GL_SIZELIMIT));
          }
        }else{
          /* User filter? */
          if($oc['CLASS'] != ""){
            $filter = "(&".$userregex."(objectClass=".$oc['CLASS'].")(cn=".$this->DivListSystem->Regex."))";
            $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, $oc['TREE'].$base,
                  array("cn", "description", "macAddress", "objectClass", "sambaDomainName", "FAIstate", "gotoMode"), GL_NONE | GL_SIZELIMIT));
          }
        }
      } 
    }

    /* Search for incoming objects */ 
    $filter = "(|(&".$userregex."(objectClass=goHard)(cn=".$this->DivListSystem->Regex.")))";
    $res = array_merge($res,get_list($filter,$this->ui->subtreeACL, "ou=incoming,".$base,
          array("cn", "description", "macAddress", "objectClass", "sambaDomainName","gotoMode"), GL_NONE | GL_SIZELIMIT));

    /* Get all gotoTerminal's */
    foreach ($res as $value){

      $tmp= $value['dn'];
      $add= "";

      /* Extract base */
      foreach($objs as $obj){
        if(preg_match("/,".$obj['TREE']."/i",$value['dn'])){
          $tmp = trim( preg_replace("/^[^,]+,[^o]*".$obj['TREE']."/i","",$value['dn']));
        }
      }

      /* check if current object is a new one */
      if (preg_match ("/,ou=incoming,/i", $tmp)){
        if (in_array_ics('gotoTerminal', $value['objectClass'])){
          $add= "- "._("New terminal");
        }elseif (in_array_ics('gotoWorkstation', $value['objectClass'])){
          $add= "- "._("New workstation");
        }elseif (in_array_ics('GOhard', $value['objectClass']) && !isset($value['gotoMode'])){
          $add= "- "._("Unknown device");
        }elseif (in_array_ics('GOhard', $value['objectClass'])){
          $add= "- "._("New Device");
        }
      } 

      /* Create a string containing the last part of the department. */
      $dn_name = preg_replace("#^([^/]+/)*#","",convert_department_dn(@LDAP::fix($tmp)));
      if(empty($dn_name)){
        $dn_name = "/";
      }
      
      /* Detect type of object and create an entry for $this->terminals */
      $terminal = array();
      if ((in_array ($tmp, $responsible)) || ($add != "")){
        if (in_array_ics('gotoTerminal', $value["objectClass"])){
          if (isset($value["macAddress"][0]) && $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "T";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['message']  = _("Terminal template for")."&nbsp;'".$dn_name."'&nbsp;";
            $terminal['location'] = array_search($tmp, $this->config->departments); 
          }
        } elseif (in_array_ics('gotoWorkstation', $value["objectClass"])){
          if (isset($value["macAddress"][0]) &&  $value["macAddress"][0] != "-"){
            $terminal             = $value;
            $terminal['type']     = "L";
            $terminal['is_new']   = $add;
          } else {
            $terminal             = $value;
            $terminal['type']     = "D";
            $terminal['location'] = array_search($tmp, $this->config->departments);
            $terminal['message']  = _("Workstation template for")."&nbsp;'".$dn_name."'&nbsp;";
          }
          if (isset($value["FAIstate"][0])){
            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
          }
        } elseif (in_array_ics('gotoPrinter', $value["objectClass"])){
          $terminal             = $value;
          $terminal['type']     = "P";
        } elseif (in_array_ics('goServer', $value["objectClass"])){
          $terminal             = $value;
          $terminal['type']     = "S";
          if (isset($value["FAIstate"][0])){
            $terminal['type']= $this->getState($terminal['type'], $value["FAIstate"][0]);
          }
        } elseif (in_array_ics('goFonHardware', $value["objectClass"])){
          $terminal             = $value;
          $terminal['type']     = "F";
        }elseif (in_array_ics("GOhard",$value['objectClass'])){
          $terminal = $value;
          $terminal['type']   = "Q";
          $terminal['is_new'] = $add;
        } elseif (in_array_ics('ieee802Device', $value["objectClass"])){
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


  function get_system_type($attrs)
  {
    $enable_arp_device_handling = preg_match("/ArpNewDevice/i",search_config($this->config->data['TABS'], "ArpNewDevice", "CLASS"));
    $classes = $attrs['objectClass'];

    $type= "";
    if (in_array_ics('ieee802Device', $classes)){
      $type= "component";
    }elseif (in_array_ics('gotoTerminal', $classes)){
      $type= "terminal";
    }elseif (in_array_ics('gotoWorkstation', $classes)){
      $type= "workstation";
    }elseif (in_array_ics('gotoPrinter', $classes)){
      $type= "printer";
    }elseif (in_array_ics('goFonHardware', $classes)){
      $type= "phone";
    }elseif (in_array_ics('goServer', $classes)){
      $type= "server";
    }elseif (in_array_ics('GOhard', $classes) && !isset($attrs['gotoMode']) && $enable_arp_device_handling){
      $type= "ArpNewDevice";
    }elseif (in_array_ics('GOhard', $classes)){
      $type= "NewDevice";
    }elseif (in_array_ics('sambaAccount', $classes) ||
        in_array_ics('sambaSamAccount', $classes)){
      $type= "winstation";
    }
    return ($type);
  }


  function convert_list($input)
  {
    $temp= "";
    $conv= array(	
        "NQ" => array("select_newsystem.png",_("New System from incoming")),
        "D" => array("select_default.png",_("Template")),
        "T" => array("select_terminal.png",_("Terminal")),
        "L" => array("select_workstation.png",_("Workstation")),
        "GL" => array("select_workstation_green.png",_("Workstation is installing")),
        "YL" => array("select_workstation_yellow.png",_("Workstation is waiting for action")),
        "RL" => array("select_workstation_red.png",_("Workstation installation failed")),
        "F" => array("select_phone.png",_("Phone")),
        "S" => array("select_server.png",_("Server")),
        "GS" => array("select_server_green.png",_("Server is installing")),
        "YS" => array("select_server_yellow.png",_("Server is waiting for action")),
        "RS" => array("select_server_red.png",_("Server installation failed")),
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

  
  function getState($type, $state)
  {
    switch (preg_replace('/:.*$/', '', $state)) {
      case 'installing':
                $type= 'G'.$type;
                break;
      case 'error':
                $type= 'R'.$type;
                break;
      case 'install':
      case 'sysinfo':
      case 'softupdate':
      case 'scheduledupdate':
                $type= 'Y'.$type;
                break;
    }


    return ($type);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
